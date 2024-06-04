add_executable(servo)

target_sources(servo PRIVATE servo.c)

pico_enable_stdio_usb(servo 1)
pico_enable_stdio_uart(servo 0)

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(servo pico_stdlib hardware_i2c hardware_adc hardware_pwm)


# create map/bin/hex file etc.
pico_add_extra_outputs(servo)