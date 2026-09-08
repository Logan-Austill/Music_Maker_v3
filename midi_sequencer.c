#include "midi_sequencer.h"
#include "pico/stdlib.h"
#include "tusb.h"

void send_midi_note_on(uint8_t note)
{
    if (note == 0) return;
    // [cable # | CIN][ON/OFF][NOTE][VELOCITY]
    uint8_t note_on_packet[4] = {(CHANNEL_ONE << 4) | CIN_NOTES, ON, note, 127};
    tud_midi_stream_write(0, note_on_packet, 4);
    tud_midi_stream_write(0, NULL, 0);
    /*
    tud_midi_packet_write(note_on_packet);
    */
}

void send_midi_note_off(uint8_t note)
{
    if (note == 0) return;
    uint8_t note_off_packet[4] = {(CHANNEL_ONE << 4) | CIN_NOTES, 0, note, 0};
    tud_midi_stream_write(0, note_off_packet, 4);
    tud_midi_stream_write(0, NULL, 0);
    /*
    tud_midi_packet_write(note_off_packet);
    */
}

void send_midi_reverb(uint8_t humidity)
{
    // calibrating humidity value to the 0-127 scale
    uint8_t reverbMix = (humidity * 127) / 100;

    // sending reverb mix midi packet
    uint8_t reverb_packet[4] = {
        (CHANNEL_ONE << 4) | CIN_CC,   // Byte 0: Cable number (0) & CIN (0x0B)
        CC_STATUS | CHANNEL_ONE,       // Byte 1: CC Status (0xB0) on Channel 1
        CC,                            // Byte 2: Controller number (91 for Reverb)
        reverbMix                      // Byte 3: Value (0-127)
    };
    tud_midi_stream_write(0, reverb_packet, 4);
    tud_midi_stream_write(0, NULL, 0);
    //tud_midi_packet_write(reverb_packet);
}