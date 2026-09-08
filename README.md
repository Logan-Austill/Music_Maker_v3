# Music_Maker_V2
Version 2 of sensor to midi output machine (synth)

In this second version I have decided to separate the critical one-wire protocol timing from the midi machine.
At this stage I am receiving valid sensor reads, loading buffers and retrieving median values successfully.

Next stage is to run the midi machine independently from the sensor read. In this stage, sensor data will not effect midi machine.
The goal is to simply run the sensor reads and midi machine simultaneously.

---* 9/3/26 UPDATE *---
I'm having some linking issues and I have yet to be able to test the playback system.
Everything at this point makes logical sense although I'm a little concerned that the timers and alarms may be a little wonky
