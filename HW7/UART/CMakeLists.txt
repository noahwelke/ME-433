add_executable(UART
        UART.c
        ssd1306.c
        )

target_sources(UART PRIVATE UART.c)

pico_enable_stdio_usb(UART 1)
pico_enable_stdio_uart(UART 0)

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(UART pico_stdlib hardware_i2c hardware_adc hardware_uart)


# create map/bin/hex file etc.
pico_add_extra_outputs(UART)