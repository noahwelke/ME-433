
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"

#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define LED_PIN 25  // Pin connected to the onboard LED (built in led)

void on_uart_rx() {
    while (uart_is_readable(UART_ID)) {
        uint8_t ch = uart_getc(UART_ID);
        putchar(ch); // Send received character to USB (PC)
    }
}

int main() {
    // Initialize UART0
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    // Enable UART interrupt
    irq_set_exclusive_handler(UART0_IRQ, on_uart_rx);
    irq_set_enabled(UART0_IRQ, true);
    uart_set_irq_enables(UART_ID, true, false);
    
    // Initialize stdio (for USB communication)
    stdio_init_all();

    while (true) {
        gpio_put(LED_PIN, 1);  // Turn on LED
        sleep_ms(500);  // Delay for 500 milliseconds

        gpio_put(LED_PIN, 0);  // Turn off LED
        sleep_ms(500);  // Delay for 500 milliseconds
        int num;
        if (scanf("%d", &num) == 1) {
            printf("Received from USB: %d\n", num);
            char buffer[20];
            sprintf(buffer, "%d\n", num);
            uart_puts(UART_ID, buffer);
        }
    }

    return 0;
}
