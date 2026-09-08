cmake_minimum_required(VERSION 3.13)

# Import the Raspberry Pi Pico SDK infrastructure
include(pico_sdk_import.cmake)

project(music_maker_v2 C CXX ASM)

# setting cmake compile commands, and cmake versions for c and c++
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK hardware drivers
pico_sdk_init()

# Define your application build targets
add_executable(music_maker_v2
    main.c
    dht_read.c
    data_structures.c
    usb_descriptors.c
    midi_sequencer.c
    generator.c
)

# Crucial: Include the path where your tusb_config.h file is located
# Assumes tusb_config.h is sitting in the same folder as your source files
target_include_directories(music_maker_v2 PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# Link the required TinyUSB device-mode dependencies
target_link_libraries(music_maker_v2
    pico_stdlib
    pico_multicore
    tinyusb_device
    tinyusb_board
)

# Route standard input/output away from USB so it doesn't conflict with MIDI
pico_enable_stdio_usb(music_maker_v2 0)
pico_enable_stdio_uart(music_maker_v2 1)

# Generate convenience flashing outputs (.uf2 binary image format)
pico_add_extra_outputs(music_maker_v2)

add_custom_command(
    TARGET music_maker_v2 POST_BUILD
    COMMAND picotool uf2 convert music_maker_v2.elf music_maker_v2.uf2
    COMMENT "Forcing conversion of executable binary into drag-and-drop .uf2 format..."
    VERBATIM
)