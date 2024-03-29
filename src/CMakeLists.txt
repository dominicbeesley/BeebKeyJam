# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(beebkbd C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(beebkbd beebkbd.c keymap.c )

pico_generate_pio_header(beebkbd ${CMAKE_CURRENT_LIST_DIR}/uart_rx.pio)

pico_set_program_name(beebkbd "beebkbd")
pico_set_program_version(beebkbd "0.1")

#pico_enable_stdio_uart(beebkbd 1)
pico_enable_stdio_usb(beebkbd 1)

# Add the standard library to the build
target_link_libraries(beebkbd pico_stdlib)

# Add any user requested libraries
target_link_libraries(beebkbd
        hardware_pio
        hardware_timer
        pico_multicore 
        )

pico_add_extra_outputs(beebkbd)

