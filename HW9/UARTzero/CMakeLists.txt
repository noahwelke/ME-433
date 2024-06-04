add_executable(UARTzero
        UARTzero.c
        )

target_sources(UARTzero PRIVATE UARTzero.c)

pico_enable_stdio_usb(UARTzero 1)
pico_enable_stdio_uart(UARTzero 0)

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(UARTzero pico_stdlib hardware_i2c hardware_adc hardware_uart)


# create map/bin/hex file etc.
pico_add_extra_outputs(UARTzero)