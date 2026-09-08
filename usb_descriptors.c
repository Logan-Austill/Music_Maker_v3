/*
    When the host asks who the device is over Endpoint 0, this is the response.
*/

#include "tusb.h"

// --------------------------------------------------------------------+
// Device Descriptor
// --------------------------------------------------------------------+
/*
    Root profile of hardware

    bcdUSB = 0x0200: Tells host that device complies with USB 2.0 framework
    bDeviceClass = 0x00: "Do no apply a single driver profile to the entire chip. Look further down inside my Interface Descriptors to discover what I am."
        Allows for a single USB port to simultaneously act as both a MIDI device and a serial port (CDC)
    idVendor & idProduct: 16-bit IDs Windows uses to group drivers
        0xCAFE is commonly used in open-source examples
    bNumConfigurations = 0x01: Devices can change their entire profile layout on the fly
        ex: Camera switching from web-cam to SD-card storage
        0x01 defines a simple, rigid operation profile
*/

tusb_desc_device_t const desc_device = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200, // USB 2.0
    .bDeviceClass       = 0x00,   // Class defined in interface
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor           = 0xCAFE, // Example Vendor ID
    .idProduct          = 0x4001, // Example Product ID for MIDI
    .bcdDevice          = 0x0100,
    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,
    .bNumConfigurations = 0x01
};

uint8_t const * tud_descriptor_device_cb(void) {
    return (uint8_t const *) &desc_device;
}

// --------------------------------------------------------------------+
// Configuration Descriptor
// --------------------------------------------------------------------+
enum {
    ITF_NUM_AUDIO = 0,
    ITF_NUM_MIDI,
    ITF_NUM_TOTAL
};

// hierarchical structure packed into a flat byte array that lists every interface and Endpoin
#define CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_MIDI_DESC_LEN)
#define EPNUM_MIDI        0x01

uint8_t const desc_configuration[] = {
    /* 
        Configuration Header

        1: Configuration index number
        ITF_NUM_TOTAL: tells the host os how many interfaces it needs to initialize
        100: Max Power Limit (2mA steps). requires 200mA of electrical current from host USB port to drive Pico
    */
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, 0x00, 100),
    /* 
        MIDI Interface Descriptor (Handles Jack definitions and EP configurations)

        Auido Control Interface: required parent interface framework for audio devices
        Streaming MIDI Interface: data carriage layout
        Embedded MIDI Jacks: virtual definitions of "In Jacks" and "Out Jacks" that route traffic inside the chip architecture
        EPNUM_MIDI: 0x01 defines an OUT Endpoint (host to device)
        0x80 | EPNUM_MIDI: 0x80 flag shifts the direction bit to indicate an IN Endpoint (device to host)
            pipeline that device uses to stream data to host
        64: sets maximum transmission packet chunk limit for the endpoint hardware lanes
    */
    TUD_MIDI_DESCRIPTOR(ITF_NUM_MIDI, 0, EPNUM_MIDI, 0x80 | EPNUM_MIDI, 64)
};

uint8_t const * tud_descriptor_configuration_cb(uint8_t index) {
    (void) index;
    return desc_configuration;
}

// --------------------------------------------------------------------+
// String Descriptors
// --------------------------------------------------------------------+
/*
    When plugging in device to host, host asks Endpoint 0, "Give me String Descriptor Index 2"
        tud_descriptor_string_cb() runs, intercepts index 2, grabs "Pico MIDI Sequencer", encodes it into UTF-16 Little Endian and streams it back
*/

char const* string_desc_arr [] = {
    (const char[]) { 0x09, 0x04 }, // 0: Supported language (English)
    "TinyUSB",                     // 1: Manufacturer
    "Pico MIDI Sequencer",         // 2: Product Name
    "123456"                       // 3: Serial Number
};

static uint16_t _desc_str[32];

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void) langid;
    uint8_t chr_count;

    if (index == 0) {
        memcpy(&_desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
    } else {
        if (!(index < sizeof(string_desc_arr)/sizeof(string_desc_arr[0]))) return NULL;
        const char* str = string_desc_arr[index];
        chr_count = strlen(str);
        if (chr_count > 31) chr_count = 31;
        for(uint8_t i=0; i<chr_count; i++) _desc_str[1+i] = str[i];
    }

    _desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);
    return _desc_str;
}
