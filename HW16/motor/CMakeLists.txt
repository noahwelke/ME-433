add_executable(motor)

target_sources(motor PRIVATE motor.c)

pico_enable_stdio_usb(motor 1)
pico_enable_stdio_uart(motor 0)

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(motor pico_stdlib hardware_i2c hardware_adc hardware_pwm)


# create map/bin/hex file etc.
pico_add_extra_outputs(motor)