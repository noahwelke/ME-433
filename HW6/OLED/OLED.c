// based on adafruit and sparkfun libraries

#include <string.h> // for memset
#include <stdio.h>
#include "ssd1306.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "font.h"
#include "hardware/adc.h"

#define LED_PIN 25
#define PICO_DEFAULT_I2C_SDA_PIN 12
#define PICO_DEFAULT_I2C_SCL_PIN 13

#define LED_PIN 25

unsigned char SSD1306_ADDRESS = 0b0111100; // 7bit i2c address
unsigned char ssd1306_buffer[513]; // 128x32/8. Every bit is a pixel except first byte

void ssd1306_setup() {
    // first byte in ssd1306_buffer is a command
    ssd1306_buffer[0] = 0x40;
    // give a little delay for the ssd1306 to power up
    //_CP0_SET_COUNT(0);
    //while (_CP0_GET_COUNT() < 48000000 / 2 / 50) {
    //}
    sleep_ms(20);
    ssd1306_command(SSD1306_DISPLAYOFF);
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);
    ssd1306_command(0x80);
    ssd1306_command(SSD1306_SETMULTIPLEX);
    ssd1306_command(0x1F); // height-1 = 31
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);
    ssd1306_command(0x0);
    ssd1306_command(SSD1306_SETSTARTLINE);
    ssd1306_command(SSD1306_CHARGEPUMP);
    ssd1306_command(0x14);
    ssd1306_command(SSD1306_MEMORYMODE);
    ssd1306_command(0x00);
    ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);
    ssd1306_command(SSD1306_SETCOMPINS);
    ssd1306_command(0x02);
    ssd1306_command(SSD1306_SETCONTRAST);
    ssd1306_command(0x8F);
    ssd1306_command(SSD1306_SETPRECHARGE);
    ssd1306_command(0xF1);
    ssd1306_command(SSD1306_SETVCOMDETECT);
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYON);
    ssd1306_clear();
    ssd1306_update();
}

// send a command instruction (not pixel data)
void ssd1306_command(unsigned char c) {
    //i2c_master_start();
    //i2c_master_send(ssd1306_write);
    //i2c_master_send(0x00); // bit 7 is 0 for Co bit (data bytes only), bit 6 is 0 for DC (data is a command))
    //i2c_master_send(c);
    //i2c_master_stop();

    uint8_t buf[2];
    buf[0] = 0x00;
    buf[1] =c;
    i2c_write_blocking(i2c_default, SSD1306_ADDRESS, buf, 2, false);
}

// update every pixel on the screen
void ssd1306_update() {
    ssd1306_command(SSD1306_PAGEADDR);
    ssd1306_command(0);
    ssd1306_command(0xFF);
    ssd1306_command(SSD1306_COLUMNADDR);
    ssd1306_command(0);
    ssd1306_command(128 - 1); // Width

    unsigned short count = 512; // WIDTH * ((HEIGHT + 7) / 8)
    unsigned char * ptr = ssd1306_buffer; // first address of the pixel buffer
    /*
    i2c_master_start();
    i2c_master_send(ssd1306_write);
    i2c_master_send(0x40); // send pixel data
    // send every pixel
    while (count--) {
        i2c_master_send(*ptr++);
    }
    i2c_master_stop();
    */

    i2c_write_blocking(i2c_default, SSD1306_ADDRESS, ptr, 513, false);
}

// set a pixel value. Call update() to push to the display)
void ssd1306_drawPixel(unsigned char x, unsigned char y, unsigned char color) {
    if ((x < 0) || (x >= 128) || (y < 0) || (y >= 32)) {
        return;
    }

    if (color == 1) {
        ssd1306_buffer[1 + x + (y / 8)*128] |= (1 << (y & 7));
    } else {
        ssd1306_buffer[1 + x + (y / 8)*128] &= ~(1 << (y & 7));
    }
}

// zero every pixel value
void ssd1306_clear() {
    memset(ssd1306_buffer, 0, 512); // make every bit a 0, memset in string.h
    ssd1306_buffer[0] = 0x40; // first byte is part of command
}

void drawChar(unsigned char x, unsigned char y, unsigned char letter) {
  for (int i = 0; i < 5; i++) {
    unsigned char c = ASCII[letter - 32][i];
    for (int j = 7; j >= 0; j--) {
      unsigned int pixelOn = (c >> j) & 0b1;
      if (pixelOn) {
        ssd1306_drawPixel(x + i, y + j, 1);
      } else {
        ssd1306_drawPixel(x + i, y + j, 0);
      }

    }
  }
}

void drawMessage(unsigned char x, unsigned char y, char * message) {
  int i = 0;
  while (message[i]) {
    drawChar(x + (5 * i), y, message[i]);
    i++;
  }
}

int main() {
  stdio_init_all();

  // initialize heartbeat LED
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT); // config as output

  adc_init(); // init the adc module
  adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
  adc_select_input(0); // select to read from ADC0
  
  // I2C is "open drain", pull ups to keep signal high when no data is being sent
  i2c_init(i2c_default, 600 * 1000);
  gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);

  sleep_ms(50); 
  gpio_put(LED_PIN, 1);
  sleep_ms(50); 
  
  ssd1306_setup();

  unsigned char message[50];
  sprintf(message, "Hello, World!");

  while(1) {
    gpio_put(LED_PIN, 1);
    sleep_ms(50);
    gpio_put(LED_PIN, 0);

    uint16_t result = adc_read();
    float volts = (result / 4096.0) * 3.3;
    unsigned int start = to_us_since_boot(get_absolute_time());

    sprintf(message, "ADC0: %.2fV", volts);
    drawMessage(0, 0, message);
    ssd1306_update();

    unsigned int stop = to_us_since_boot(get_absolute_time());
    
    unsigned int timeElapsed = stop - start;
    sprintf(message, "FPS = %.2f", 1000000.0/timeElapsed);
    drawMessage(0, 8, message);
    ssd1306_update();

    sleep_ms(50);
  }
}