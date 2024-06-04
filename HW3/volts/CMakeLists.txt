add_executable(volts)

target_sources(volts PRIVATE volts.c)

pico_enable_stdio_usb(volts 1)
pico_enable_stdio_uart(volts 0)

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(volts pico_stdlib hardware_i2c hardware_adc)


# create map/bin/hex file etc.
pico_add_extra_outputs(volts)