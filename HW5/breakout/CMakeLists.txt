add_executable(breakout)

target_sources(breakout PRIVATE breakout.c)

pico_enable_stdio_usb(breakout 1)
pico_enable_stdio_uart(breakout 0)

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(breakout pico_stdlib hardware_i2c)


# create map/bin/hex file etc.
pico_add_extra_outputs(breakout)