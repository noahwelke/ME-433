#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "ssd1306.h"
#include "font.h"
#include <stdlib.h>

#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE
#define LED_PIN 25  // Pin connected to the onboard LED (built in led)
#define BUFFER_SIZE 50

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 0
#define UART_RX_PIN 1


// I2C defines
#define I2C_PORT i2c0  // I2C port to be used
#define I2C_SDA 4      // I2C data pin
#define I2C_SCL 5      // I2C clock pin

// SSD1306 I2C address
#define SSD1306_I2C_ADDR 0x3C  // I2C address for the SSD1306 OLED display

// GPIO for LED
#define LED_PIN 25  // Pin connected to the onboard LED

// Function to initialize the I2C interface
void init_i2c() {
    i2c_init(I2C_PORT, 400 * 1000);  // Initialize I2C with a frequency of 400kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);  // Set SDA pin to I2C function
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);  // Set SCL pin to I2C function
    gpio_pull_up(I2C_SDA);  // Enable pull-up on SDA pin
    gpio_pull_up(I2C_SCL);  // Enable pull-up on SCL pin
}

// Function to initialize the GPIO for the LED
void init_gpio() {
    gpio_init(LED_PIN);  // Initialize the GPIO for the LED
    gpio_set_dir(LED_PIN, GPIO_OUT);  // Set the LED pin as an output
}

// Function to draw a single character at a specified position
void drawChar(int x, int y, char c) {
    if (c < 32 || c > 127) return;  // Only draw characters in the valid range
    for (int col = 0; col < 5; col++) {  // Loop through each column of the character
        uint8_t line = ASCII[c - 32][col];  // Get the column data from the font
        for (int row = 0; row < 8; row++) {  // Loop through each row of the column
            if (line & 1) {
                ssd1306_drawPixel(x + col, y + row, 1);  // Draw pixel if bit is set
            } else {
                ssd1306_drawPixel(x + col, y + row, 0);  // Clear pixel if bit is not set
            }
            line >>= 1;  // Shift to the next bit
        }
    }
}

// Function to draw a string of characters at a specified position
void drawString(int x, int y, const char *str) {
    while (*str) {  // Loop through each character in the string
        drawChar(x, y, *str);  // Draw the character
        x += 6;  // Move x position for next character (5 pixels + 1 pixel space)
        str++;
    }
}

// Function to print a message on the OLED display
void printMessage(int x, int y, const char *message) {
    drawString(x, y, message);  // Draw the message on the display
    ssd1306_update();  // Update the display with the new data
}

static int chars_rxed = 0;
char message[50];
char buffer[BUFFER_SIZE];
int buffer_index = 0;

volatile int kk = 0;
char m[100] = {0};  // Initialize the array to zero

// RX interrupt handler
void on_uart_rx(){

    // while (uart_is_readable(UART_ID)) {
    //     char ch = uart_getc(UART_ID);
    //     if (ch == '\n') {
    //         buffer[buffer_index] = '\0';
    //         printf("Received: %s\n", buffer);
    //         buffer_index = 0;  // Reset buffer index
    //     } else {
    //         if (buffer_index < BUFFER_SIZE - 1) {
    //             buffer[buffer_index++] = ch;
    //         }
    //     }
    // }

    // Sending buffered chars (as full strings rather than 1 at a time)
    while (uart_is_readable(UART_ID)) {
        uint8_t ch = uart_getc(UART_ID);

        if (ch == '\r' || ch == '\n') {
            m[kk] = '\0';  // Null-terminate the string
            printMessage(0, 0, m);  // Draw the string on the display
            uart_putc(UART_ID, ch);
            sprintf(message, "%c", m);  // Format message with ADC voltage
            kk = 0;  // Reset the counter
            // // Optionally clear the array
            // memset(m, 0, sizeof(m));
        } 
        else {
            if (kk < sizeof(m) - 1) {  // Ensure we do not overflow the buffer
                m[kk] = ch;
                kk++;
            }


        // // Sending Individual chars 
        // if (uart_is_writable(UART_ID)) {
        //     // Change it slightly first!
        //     // ch++;
        //     uart_putc(UART_ID, ch);
        //     sprintf(message, "%c", ch);  // Format message with ADC voltage
        //     ssd1306_clear();  // Clear the display
        //     printMessage(0, 0, message);  // Print the message on the display

        // }
        }
    }
}

int main() {
    stdio_init_all();  // Initialize all standard I/O
    init_i2c();  // Initialize I2C
    init_gpio();  // Initialize GPIO
    ssd1306_setup();  // Setup the SSD1306 display

    // Set the TX and RX pins by using the function select on the GPIO
    // See datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    // Set up our UART with the desired baud rate
    uart_init(UART_ID, BAUD_RATE);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(UART_ID, false, false);

    // Set our data format
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

    // Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled(UART_ID, false);

    // Set up a RX interrupt
    // We need to set up the handler first
    // Select correct interrupt for the UART we are using
    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(UART_ID, true, false);

    // OK, all set up.
    // Lets send a basic string out, and then run a loop and wait for RX interrupts
    // The handler will count them, but also reflect the incoming data back with a slight change!
    printMessage(0,0,"Ready");

    uart_puts(UART_ID, "\nReady for uart interrupts\n");
    

    int j = 0;  // Counter variable
    while (1){
        gpio_put(LED_PIN, 1);  // Turn on LED
        sleep_ms(500);  // Delay for 500 milliseconds

        gpio_put(LED_PIN, 0);  // Turn off LED
        sleep_ms(500);  // Delay for 500 milliseconds

        j++;  // Increment counter
    }
}