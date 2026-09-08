#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "pico/stdlib.h"
#include "data_structures.h"

// macros
#define OCTAVE 12
#define THIRD 1
#define FIFTH 2
#define SCALE_LEN 7
#define NOTE_COUNT 4

extern const uint8_t C_Major[7];
extern const uint8_t C_Minor[7];

void chord_builder(PackedNotes *newChord);
void melody_builder(PackedNotes *newMelody);

#endif