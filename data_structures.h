#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include "pico/stdlib.h"

#define IN_BUFFER_SIZE 8
#define TOTAL_CHORDS 4

typedef struct {
    uint8_t midi;
} MidiNote;

// Creates a struct of MidiNotes and assigns them a note on the western scale
typedef struct {
    const MidiNote C;
    const MidiNote C_Sharp;
    const MidiNote D;
    const MidiNote D_Sharp;
    const MidiNote E;
    const MidiNote F;
    const MidiNote F_Sharp;
    const MidiNote G;
    const MidiNote G_Sharp;
    const MidiNote A;
    const MidiNote A_Sharp;
    const MidiNote B;
} NoteRegistry;

extern const NoteRegistry Notes;

// holds an array of four MidiNotes
typedef struct {
    MidiNote buffer[4];
} PackedNotes;

// NoteRingBuffer holds a buffer of PackedNotes, oldest marker, count, and total generated
typedef struct {
    PackedNotes buffer[TOTAL_CHORDS];
    uint8_t oldest;
    uint8_t count;
    uint8_t generated;
} NoteRingBuffer;

// NoteRingBuffer functions
void NoteRingBuffer_init(NoteRingBuffer* cb);
void NoteRingBuffer_push(NoteRingBuffer* cb, PackedNotes *newNoteSet);
PackedNotes NoteRingBuffer_getOldest(const NoteRingBuffer* cb);
bool NoteRingBuffer_isFull(const NoteRingBuffer* cb);
uint8_t NoteRingBuffer_size(const NoteRingBuffer* cb);
uint8_t NoteRingBuffer_generated(const NoteRingBuffer* cb);
void NoteRingBuffer_pop(NoteRingBuffer* cb);
PackedNotes NoteRingBuffer_peek(const NoteRingBuffer* cb);

/*
    DataRingBuffer declarations
*/

// DataRingBuffer holds a buffer of data, a sorted buffer of the same data, oldest marker, cout marker and a count of total generated
typedef struct {
    float buffer[IN_BUFFER_SIZE];
    float sortedBuffer[IN_BUFFER_SIZE];
    uint8_t oldest;
    uint8_t count;
    uint8_t generated;
} DataRingBuffer;

// DataRingBuffer functions
void DataRingBuffer_init(DataRingBuffer* cb);
void DataRingBuffer_sort(DataRingBuffer* cb);
void DataRingBuffer_push(DataRingBuffer* cb, float value);
float DataRingBuffer_getOldest(const DataRingBuffer* cb);
float DataRingBuffer_getNewest(const DataRingBuffer* cb);
bool DataRingBuffer_isFull(const DataRingBuffer* cb);
uint8_t DataRingBuffer_size(const DataRingBuffer* cb);
void DataRingBuffer_pop(DataRingBuffer* cb);

#endif