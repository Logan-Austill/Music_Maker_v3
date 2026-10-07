#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/multicore.h"
#include "hardware/timer.h"
#include "dht_read.h"
#include "data_structures.h"
#include "generator.h"
#include "midi_sequencer.h"
#include "tusb.h"
//#include "playback.h"

#define DHT_PIN 16                          // pin that defines our dht22 sensor

// Sequencer definitions
#define STEPS_PER_MEASURE 4                 // quarter notes in a measure
#define BPM_MULTIPLIER 60000000             // multiplier to convert bpm to ms

// Global volatile state
volatile bool step_tick_flag = false;       // keeps track of each quarter note
volatile bool note_off_flag = false;        // flags the timing of turning a note(s) off
volatile bool kill_chord_next_alarm = false;

volatile bool read_success = false;         // flags whether dht22 read was successful

// Master timer and note off alarm
alarm_id_t note_off_alarm_id = 0;           // alarm for note off times
alarm_id_t seq_alarm_id = 0;

// Flat global tracking state for the Note-Off Callback
volatile bool release_chord_this_alarm = false;

uint8_t current_tempo = 120;
uint8_t current_reverb = 64;

uint8_t active_chord_notes[4] = {0, 0, 0, 0};
uint8_t active_melody_note = 0;

// updates bpm based on temperature input
uint8_t temp_to_bpm (float temp)
{
    uint8_t intermediate_bpm = (uint8_t)((temp - 22) * 5 + 120);                         // calculates new bpm based on temperature value

    return (intermediate_bpm < 50) ? 50: intermediate_bpm;
}

// Helper to convert BPM to quarter-note interval in microseconds
inline int64_t bpm_to_us(uint8_t bpm) {
    return (BPM_MULTIPLIER / bpm);
}

// timer for catching ticks (quarter notes)
bool timer_callback(struct repeating_timer *t) {
    step_tick_flag = true;                          // resets step_tick_flag
    return true;                                    // Keep timer running
}

int64_t sequencer_alarm_callback(alarm_id_t id, void *user_data)
{
    step_tick_flag = true;

    int64_t next_step_duration_us = bpm_to_us(current_tempo);
    return -next_step_duration_us;
}

int64_t note_off_callback(alarm_id_t id, void *user_data)
{
    // 1. Turn off the active melody note for this quarter note step
    if (active_melody_note > 0 && active_melody_note <= 127) {
        send_midi_note_off(active_melody_note);
        active_melody_note = 0; // Clear it out
    }

    // 2. Turn off the chords only if this was the 4th step of the measure
    if (release_chord_this_alarm) {
        for (uint8_t i = 0; i < 4; i++) {
            if (active_chord_notes[i] > 0 && active_chord_notes[i] <= 127) {
                send_midi_note_off(active_chord_notes[i]);
                active_chord_notes[i] = 0; // Clear it out
            }
        }
        release_chord_this_alarm = false; // Reset the flag
    }
    
    note_off_alarm_id = 0;
    return 0; // Single-shot alarm, do not repeat automatically
}

void core1_entry(void)
{
    float humidity_median = 50.0f;
    float temperature_median = 22.0f;

    // Initializing ring buffers
    DataRingBuffer humidity_buffer;                                                 // Create humidity value buffer
    DataRingBuffer_init(&humidity_buffer);                                          // initialize humidity value buffer

    DataRingBuffer temperature_buffer;                                              // Create temperature value buffer
    DataRingBuffer_init(&temperature_buffer);                                       // initilize temperature value buffer

    dht22_data_t current_read;
    dht_init(&current_read);

    while (1)
    {
        if (read_dht(DHT_PIN, &current_read))
        {
            float humidity = current_read.humidity / 10.0f;
            float temperature = current_read.temperature / 10.0f;

            DataRingBuffer_push(&humidity_buffer, humidity);
            DataRingBuffer_push(&temperature_buffer, temperature);

            if (!DataRingBuffer_isFull(&humidity_buffer))
            {
                DataRingBuffer_sort(&humidity_buffer);
                humidity_median = (humidity_buffer.sortedBuffer[4] + humidity_buffer.sortedBuffer[5]) / 2;
            }
            else
            {
                humidity_median = DataRingBuffer_getNewest(&humidity_buffer);
            }

            if (!DataRingBuffer_isFull(&temperature_buffer))
            {
                DataRingBuffer_sort(&temperature_buffer);
                temperature_median = (temperature_buffer.sortedBuffer[4] + temperature_buffer.sortedBuffer[5]) / 2;
            }
            else
            {
                temperature_median = DataRingBuffer_getNewest(&temperature_buffer);
            }

            uint16_t humidity_send = (uint16_t)(humidity_median * 10);
            uint16_t temperature_send = (uint16_t)(temperature_median * 10);
            uint32_t fifo_packet = humidity_send << 16 | temperature_send;
            multicore_fifo_push_blocking(fifo_packet);

            read_success = true;    // set flag true if read was successful
        }
        else
        {
            read_success = false;   // set flag false if read failed
        }
        sleep_ms(2000);
    }
}

int main() {
    NoteRingBuffer chord_buffer;                                                    // Create chord note buffer
    NoteRingBuffer_init(&chord_buffer);                                             // initialize chord note buffer

    NoteRingBuffer melody_buffer;                                                   // Create melody note buffer
    NoteRingBuffer_init(&melody_buffer);                                            // initialize melody note buffer

    PackedNotes current_chord;
    PackedNotes current_melody;

    srand(to_us_since_boot(get_absolute_time()));                                   // seeding randomizer

    while (!NoteRingBuffer_isFull(&chord_buffer))                                   // fill chord and melody buffers
    {
        chord_builder(&current_chord);
        NoteRingBuffer_push(&chord_buffer, &current_chord);
    }
    while (!NoteRingBuffer_isFull(&melody_buffer))
    {
        melody_builder(&current_melody);
        NoteRingBuffer_push(&melody_buffer, &current_melody);
    }
    
    int64_t quarter_note_us = bpm_to_us(current_tempo);
    seq_alarm_id = add_alarm_in_us(quarter_note_us, sequencer_alarm_callback, NULL, true);

    stdio_init_all();                                                               // initialize I/O
    gpio_init(DHT_PIN);                                                             // initialize dht pin
    tusb_init();                                                                    // initialize TinyUSB

    multicore_launch_core1(core1_entry);                                            // start core 1 process

    uint8_t current_step = 0;                                                       // keep track of current quarter note
    // Main Core 0 Loop
    while (true) {
        tud_task();                                                                 // Keeping TinyUSB alive

        if (step_tick_flag)
        {
            step_tick_flag = false;

            if (current_step == 0)
            {
                // A. Check for new Core 1 sensor data and update global conditions
                if (multicore_fifo_rvalid())
                {
                    //gpio_put(15, 1);
                    // Core 1 pushed data. Read raw data without blocking
                    uint32_t received_data = multicore_fifo_pop_blocking();   // pulling 32 bit word from FIFO buffer

                    int16_t hum_median = (int16_t)((received_data) >> 16 & 0xFFFF);
                    int16_t temp_median = (int16_t)(received_data & 0xFFFF);

                    float hum_value = hum_median / 10.0f;
                    float temp_value = temp_median / 10.0f;

                    // updating bpm based on new temperature median and update sequencer timer
                    current_tempo = temp_to_bpm(temp_value);

                    // updating reverb mix based on new humidity median
                    // we have to cast as uint8_t and scale from 0 - 127
                    current_reverb = (uint8_t)(hum_value * 127 / 100);
                }

                current_chord = NoteRingBuffer_getOldest(&chord_buffer);
                NoteRingBuffer_pop(&chord_buffer);

                current_melody = NoteRingBuffer_getOldest(&melody_buffer);
                NoteRingBuffer_pop(&melody_buffer);

                for (uint8_t i = 0; i < 4; i++) {
                    active_chord_notes[i] = current_chord.buffer[i].midi;
                    send_midi_note_on(active_chord_notes[i]);
                }
            }
            active_melody_note = current_melody.buffer[current_step].midi;
            send_midi_note_on(active_melody_note);

            // If we are on the final step (Step 3), tell the alarm to turn off the chord notes too
            if (current_step == (STEPS_PER_MEASURE - 1)) {
                release_chord_this_alarm = true;
            }

            // Timing Calculations
            int64_t step_duration_us = bpm_to_us(current_tempo);
            int64_t note_duration_us = step_duration_us - 200000; // 200ms space

            if (note_duration_us <= 0) {
                note_duration_us = step_duration_us / 10; // Tiny safety margin for ultra-fast tempos
            }

            // Safety flush lingering alarms
            if (note_off_alarm_id > 0) {
                cancel_alarm(note_off_alarm_id);
            }

            // Schedule the single-shot alarm callback
            note_off_alarm_id = add_alarm_in_us(note_duration_us, note_off_callback, NULL, true);

            // Advance step index safely
            current_step = (current_step + 1) % STEPS_PER_MEASURE;
        }
        
        // Maintain queues during idle time
        if (!NoteRingBuffer_isFull(&chord_buffer)) {
            chord_builder(&current_chord);
            NoteRingBuffer_push(&chord_buffer, &current_chord);
        }
        if (!NoteRingBuffer_isFull(&melody_buffer)) {
            melody_builder(&current_melody);
            NoteRingBuffer_push(&melody_buffer, &current_melody);
        }
    }
}
