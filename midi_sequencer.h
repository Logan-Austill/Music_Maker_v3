#ifndef MIDI_SEQUENCER_H
#define MIDI_SEQUENCER_H

#include <stdint.h>

// macros
#define CIN_NOTES 0x09
#define CIN_CC 0x0B
#define CC_STATUS 0xB0
#define CC 91
#define ON 0x90
#define CHANNEL_ONE 0

// functions
void send_midi_note_on(uint8_t note);
void send_midi_note_off(uint8_t note);
void send_midi_reverb(uint8_t humidity);

#endif