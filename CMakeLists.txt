cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(modplay)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(main
    main.c
    MODPlay/modplay.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(main
    pico_stdlib pico_multicore hardware_pwm
    hardware_clocks hardware_irq)

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(main)
