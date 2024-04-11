/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

#define LED_PIN 25

#define ADDR _u(0b0100000)

#define REG_IODIR _u(0x00)
#define REG_GPIO _u(0x09)
#define REG_OLAT _u(0x0A)

#define PICO_DEFAULT_I2C_SDA_PIN 12
#define PICO_DEFAULT_I2C_SCL_PIN 13

void i2c_chip_init() {
    uint8_t buf[2];

    // send register number followed by its corresponding value
    buf[0] = REG_IODIR;
    buf[1] = 0b01111111; //GP7 output, rest are inputs
    i2c_write_blocking(i2c_default, ADDR, buf, 2, false); //function has start, stop bit included. however will be blocked if we don't get ack bit back
}

void setPin(unsigned char address, unsigned char reg, unsigned char value) {
    uint8_t buf[2];

    // send reg number and corresponding value
    buf[0] = reg;
    buf[1] = value; 
    i2c_write_blocking(i2c_default, address, buf, 2, false); 

}

bool readPin(unsigned char address, unsigned char reg) {
   
    uint8_t buf[1];
    i2c_write_blocking(i2c_default, ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, ADDR, buf, 1, false); 

    if (buf[0] & 0b1 == 0b1){
        return 1; //the pin is high
    } else {
        return 0;
    }
}

int main() {
    // Initialize chosen serial port
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    //Initialize I2C port at 100kHz
    i2c_init(i2c_default, 100 * 1000);

    // Initialize I2C pins
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);

    i2c_chip_init();

    sleep_ms(250);
    while (1) {
        // Blink the builtin green LED as a heart-beat
        gpio_put(LED_PIN, 1);
        sleep_ms(500);
        gpio_put(LED_PIN, 0);
        sleep_ms(500);

        // Read button and control LED
        if (readPin(ADDR, REG_GPIO) == 1){
            setPin(ADDR, REG_OLAT, 0b00000000);
        } else if (readPin(ADDR, REG_GPIO) == 0){
            setPin(ADDR, REG_OLAT, 0b10000000);
        }
    }
}

