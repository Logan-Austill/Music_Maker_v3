#include "generator.h"

const uint8_t third = 1;
const uint8_t fifth = 2;

const uint8_t C_Major[7] = {60, 62, 64, 65, 67, 69, 71};
const uint8_t C_Minor[7] = {60, 62, 63, 65, 67, 68, 70};

uint8_t chord_seed = 0;
uint8_t melody_seed[4] = {0, 0, 0, 0};

void chord_seed_generator()
{
    srand(to_us_since_boot(get_absolute_time()));

    chord_seed = rand() % 6;
}

void melody_seed_generator()
{
    for (uint8_t i = 0; i < 4; i++)
    {
        melody_seed[i] = rand() % 6;
    }
}

// everytime we call this we get one chord
void chord_builder(PackedNotes *newChord)
{   
    chord_seed_generator();

    newChord->buffer[0].midi = C_Major[chord_seed] - OCTAVE;

    // handling wraparound logic, in case indexing exceeds size of CMajor we multiply by octave step
    newChord->buffer[1].midi = C_Major[chord_seed];
    newChord->buffer[2].midi = C_Major[(chord_seed + THIRD) % SCALE_LEN] + (((chord_seed + THIRD) / SCALE_LEN) * OCTAVE);
    newChord->buffer[3].midi = C_Major[(chord_seed + FIFTH) % SCALE_LEN] + (((chord_seed + FIFTH) / SCALE_LEN) * OCTAVE);
}

// everytime we call this we get a 4 note melody0
void melody_builder(PackedNotes *newMelody)
{
    melody_seed_generator();
    
    newMelody->buffer[0].midi = (C_Major[(melody_seed[0] + SCALE_LEN - 1) % SCALE_LEN] + (melody_seed[0] / SCALE_LEN) * OCTAVE) + OCTAVE;       
    newMelody->buffer[1].midi = (C_Major[(melody_seed[1] + 2) % SCALE_LEN] + ((melody_seed[1] + 2) / SCALE_LEN) * OCTAVE) + OCTAVE;       
    newMelody->buffer[2].midi = (C_Major[(melody_seed[2] + 3) % SCALE_LEN] + ((melody_seed[2] + 3) / SCALE_LEN) * OCTAVE) + OCTAVE;       
    newMelody->buffer[3].midi = (C_Major[(melody_seed[3] + 4) % SCALE_LEN] + ((melody_seed[3] + 4) / SCALE_LEN) * OCTAVE) + OCTAVE;
}