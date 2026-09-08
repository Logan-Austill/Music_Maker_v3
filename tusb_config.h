#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

// --------------------------------------------------------------------+
// COMMON CONFIGURATION
// --------------------------------------------------------------------+

/* 
    Define the controller speed (0 = Full Speed, 1 = High Speed)
    Raspberry Pi Pico (RP2040/RP2350) natively supports Full Speed

    RHPORT0 - root hub port 0, first physical USB controller on the rp2040
    OPT_MODE_DEVICE - tells compiler to load the peripheral stack (device, not host)
    OPT_MODE_FULL_SPEED - bus speed = 12 Mbps. rp2040 can NOT run at 480 Mbps(high speed)
*/ 
#ifndef CFG_TUSB_RHPORT0_MODE
#define CFG_TUSB_RHPORT0_MODE     OPT_MODE_DEVICE | OPT_MODE_FULL_SPEED
#endif

/* 
    Define the target board OS (required for timing/tasks)

    TinyUSB works with RTOS but here we are not using one
    If we were using an RTOS, TinyUSB would include mutexes/semaphores
*/
#ifndef CFG_TUSB_OS
#define CFG_TUSB_OS               OPT_OS_NONE
#endif

// Enable debug logging (0 = off, 1 = error, 2 = warning, 3 = info)
#define CFG_TUSB_DEBUG            0

// --------------------------------------------------------------------+
// DEVICE CONFIGURATION
// --------------------------------------------------------------------+

// Enable device mode
#define CFG_TUD_ENABLED           1

/* 
    Core management queues

    Enpoint 0 - bidirectional communication line for initial enumeration
        ex: Who are you? What drivers do you need?
    Set size to 64 bytes, static buffer chunk size for holding descriptor responses
*/
#define CFG_TUD_ENDPOINT0_SIZE    64

// --------------------------------------------------------------------+
// CLASS DRIVER CONFIGURATION
// --------------------------------------------------------------------+

/* 
    Enable the MIDI class driver (Crucial for your sequencer!)

    Activates USB audio/midi driver suite inside TinyUSB
    TX_BUFSIZE - 64 byte static lockless ring buffer in RAM for outgoing messages.
        When writing, bytes drop into this array. Engine reads directly from this
*/
#define CFG_TUD_MIDI              1

// Set the internal FIFO buffer sizes for MIDI streaming (in bytes)
#define CFG_TUD_MIDI_RX_BUFSIZE   64
#define CFG_TUD_MIDI_TX_BUFSIZE   64

#ifdef __cplusplus
}
#endif

#endif /* _TUSB_CONFIG_H_ */
