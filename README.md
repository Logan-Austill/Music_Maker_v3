# Music_Maker_v3
Initial working program to read humidity and temperature from DHT22 sensor and send Midi output via USB.

Equipment Used:
- Raspberry Pi Pico W
- DHT22
- 10k resistor
- jumper wires
- breadboard

Wiring Instructions:
- Pico Pin 39 goes to positive rail
- Pico Pin 38 (or any ground) goes to ground rail
- 10k resistor jumps between DHT22 Power and Out pins
- DHT22 Power pin goes to positive rail
- DHT22 Ground pin goes to ground rail

MIDI MACHINE:
Perpetual Midi output stream sending a triad chord (an octave below middle C) from the C Major scale including a fourth note that duplicates the root note an octave below every whole note.
Also outputs a single melody note every quarter note an octave above middle C.
Root note of each chord is selected randomly which is duplicated an octave below. To complete the triad, a third and fifth above the root is added.
All melody notes are selected randomly from the C Major scale.
Midi notes are sent based on a global timer that ticks the quarter notes in microseconds based on the global tempo (bpm) value.
An alarm is set to mark the ending of a note which is set to be exactly 200ms before the start of the next note/chord.
Both timer and alarms are updated based on the changing tempo (more on this in the DHT22 section).
A separate value 0-127 is sent via CC 91 intended to control reverb mix based on the changing humidity value (more on this in the DHT22 section).

DHT22 Reads:
Takes humidity and temperature data input from DHT22 via one-wire protocol.
Each value is pushed into a ring buffer, one for temperature, one for humidity. The values in the ring buffers are used to calculate a median value to ensure accuracy.

Future Improvements:
1) Set an initialization script to establish the CC 91 channel
2) Gracefully handle sensor disconnection and reconnection
3) Allow for manual control of reverb signal
4) Light sensor to switch between Major/Minor scales
5) Come up with some way to change Keys. Can't come up with any logical triggers.
