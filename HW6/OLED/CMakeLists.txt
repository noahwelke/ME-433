add_executable(OLED)

target_sources(OLED PRIVATE OLED.c)

pico_enable_stdio_usb(OLED 1)
pico_enable_stdio_uart(OLED 0)

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(OLED pico_stdlib hardware_i2c hardware_adc)


# create map/bin/hex file etc.
pico_add_extra_outputs(OLED)