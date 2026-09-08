# Music_Maker_v3
Initial working program to read humidity and temperature from DHT22 sensor and send Midi output via USB.

MIDI MACHINE:
Perpetual Midi output stream sending a triad chord from the C Major scale including a fourth note that duplicates the root note an octave below every whole note.
Also outputs a single melody note every quarter note.
Root note of each chord is selected randomly which is duplicated an octave below. To complete the triad, a third and fifth above the root is calculated.
All melody notes are selected randomly from the C Major scale.
Midi notes are sent based on a global timer that ticks the quarter notes in microseconds based on the global tempo(bpm) value.
An alarm is set to mark the ending of a note which is set to be exactly 200ms before the start of the next note/chord.
Both timer and alarms are updated based on the changing tempo (more on this in the DHT22 section).
A separate value 0-127 is sent via CC 91 intended to control reverb mix based on the changing humidity value (more on this in the DHT22 section).

DHT22:
Takes humidity and temperature data input from DHT22 via one-wire protocol.
Each value is pushed into its own ringbuffer. The values in the ringbuffers are used to calculate a median value to ensure accuracy.
