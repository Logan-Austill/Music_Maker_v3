#include "data_structures.h"

const NoteRegistry Notes = {
    .C          = {.midi = 60},
    .C_Sharp    = {.midi = 61},
    .D          = {.midi = 62},
    .D_Sharp    = {.midi = 63},
    .E          = {.midi = 64},
    .F          = {.midi = 65},
    .F_Sharp    = {.midi = 66},
    .G          = {.midi = 67},
    .G_Sharp    = {.midi = 68},
    .A          = {.midi = 69},
    .A_Sharp    = {.midi = 70},
    .B          = {.midi = 71}
};

// Initialize the buffer values
void DataRingBuffer_init(DataRingBuffer* cb)
{
    cb->oldest = 0;
    cb->count = 0;
    cb->generated = 0;
    for (uint8_t i = 0; i < IN_BUFFER_SIZE; i++)
    {
        cb->buffer[i] = 0.0f;
        cb->sortedBuffer[i] = 0.0f;
    }
}

void DataRingBuffer_sort(DataRingBuffer* cb)
{
    for (uint8_t i = 0; i < cb->count; i++)
    {
        uint8_t shadow_index = (cb->oldest + 1) % IN_BUFFER_SIZE;
        cb->sortedBuffer[i] = cb->buffer[shadow_index];
    }

    for (uint8_t i = 1; i < cb->count; i++)
    {
        float value_key = cb->sortedBuffer[i];
        int j = i - 1;

        while (j >= 0 && cb->sortedBuffer[j] > value_key)
        {
            cb->sortedBuffer[j + 1] = cb->sortedBuffer[j];
            j--;
        }
        cb->sortedBuffer[j + 1] = value_key;
    }
}

// Push a new value into the buffer safely
void DataRingBuffer_push(DataRingBuffer* cb, float value)
{
    uint8_t index;

    if (cb->count < IN_BUFFER_SIZE)
    {
        index = (cb->oldest + cb->count) % IN_BUFFER_SIZE;
        cb->count++;
    } 
    else
    {
        index = cb->oldest;
        cb->oldest = (cb->oldest + 1) % IN_BUFFER_SIZE;
    }

    cb->buffer[index] = value;
    DataRingBuffer_sort(cb);
}

// Get the oldest element
float DataRingBuffer_getOldest(const DataRingBuffer* cb)
{
    return cb->buffer[cb->oldest];
}

float DataRingBuffer_getNewest(const DataRingBuffer* cb)
{
    return cb->buffer[cb->count];
}

// Check if the buffer is full
bool DataRingBuffer_isFull(const DataRingBuffer* cb)
{
    return cb->count == IN_BUFFER_SIZE;
}

// Get the current number of elements
uint8_t DataRingBuffer_size(const DataRingBuffer* cb)
{
    return cb->count;
}

void DataRingBuffer_pop(DataRingBuffer* cb)
{
    if (cb->count > 0)
    {
        cb->oldest = (cb->oldest + 1) % IN_BUFFER_SIZE;
        cb->count--;

        DataRingBuffer_sort(cb);
    }
}

/*
    NoteRingBuffer functions
*/

// Initialize the buffer values
void NoteRingBuffer_init(NoteRingBuffer* cb)
{
    cb->oldest = 0;
    cb->count = 0;
    cb->generated = 0;
}

// Push a new value into the buffer
void NoteRingBuffer_push(NoteRingBuffer* cb, PackedNotes *newNoteSet)
{
    uint8_t index = (cb->oldest + cb->count) % TOTAL_CHORDS; // FIXED: Wrap around using TOTAL_CHORDS size
    cb->buffer[index] = *newNoteSet;

    if (cb->count < TOTAL_CHORDS)
    {
        cb->count++;
    } 
    else
    {
        cb->oldest = (cb->oldest + 1) % TOTAL_CHORDS;
    }
    cb->generated += 1;
}

// Get the oldest element
PackedNotes NoteRingBuffer_getOldest(const NoteRingBuffer* cb)
{
    return cb->buffer[cb->oldest];
}

// Check if the buffer is full
bool NoteRingBuffer_isFull(const NoteRingBuffer* cb)
{
    return cb->count == TOTAL_CHORDS;
}

// Get the current number of elements
uint8_t NoteRingBuffer_size(const NoteRingBuffer* cb)
{
    return cb->count;
}

uint8_t NoteRingBuffer_generated(const NoteRingBuffer* cb)
{
    return cb->generated;
}

void NoteRingBuffer_pop(NoteRingBuffer* cb)
{
    if (cb->count > 0)
    {
        cb->oldest = (cb->oldest + 1) % TOTAL_CHORDS;
        cb->count--;
    }
}

PackedNotes NoteRingBuffer_peek(const NoteRingBuffer* cb)
{
    uint8_t index = (cb->oldest + cb->count - 1) % TOTAL_CHORDS;
    return cb->buffer[index];
}