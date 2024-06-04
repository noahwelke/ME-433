#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h" // need to add in cmakelists



int main() {
    stdio_init_all();
    adc_init();                 //initialize the ADC peripheral so that it is on

    while (!stdio_usb_connected())
    {
        sleep_ms(100);
    }
    
    printf("Start!\n");

    // Define which pin will turn on the LED 
    const uint LEDPIN = 15; //GP15
    const uint ADC = 26; //GP26
    const uint PUSHBUTTON = 14; //GP14

    gpio_init(LEDPIN); // Set GP15 as a general purpose I/O pin
    adc_gpio_init(ADC); //Specify that pin 26 should be used as a ADC pin rather than a general purpose io pin

    // Once it knows it's a general purpose i/o pin, we set the direction: Like TRIS SFR
    gpio_set_dir(LEDPIN, GPIO_OUT); // Set PIN GP15 to be an output PIN.
    gpio_set_dir(PUSHBUTTON, GPIO_IN); // Set PIN GP14 to be an input PIN.

    // Select ADC input 0 (GPIO26) 
    // 0 represents ADC0 
    adc_select_input(0); // If we wanted to bounce between reading 2 ADC pins would have to change this before adc_read() function


    // gpio_put function: put the pin 1 (on)
    gpio_put(LEDPIN, 1);

    printf("Enter the number of analog samples (1-100): ");
    
    int num_samples;
    scanf("%d", &num_samples);
    if (num_samples < 1 || num_samples > 100) {
        printf("Invalid number of samples. Please enter a number between 1 and 100.\n");
        return 1;  // Exit if the input is not within the valid range
    }
    
    while (1){ 
        
        while(!gpio_get(PUSHBUTTON)){ //while pushbutton remains low
        
        gpio_put(LEDPIN, 1);
        const float conversion_factor = 3.3f / (1 << 12);

        for (int i = 0; i < num_samples; i++) {
            uint16_t result = adc_read();
            printf("Sample %d: Raw value: 0x%03x, Voltage: %.3f V\n", i+1, result, result * conversion_factor);
            sleep_ms(500); // Sleep 500 ms = 2Hz (100Hz was too fast in my opinion)

            if(gpio_get(PUSHBUTTON)){
                break; // Exit the for loop when the pushbutton is high in order to immediately turn the LED off
            }
        }
        // // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
        // const float conversion_factor = 3.3f / (1 << 12);
        // //adc_read() => reads voltage on the pin
        // uint16_t result = adc_read(); // uint16_t => unsigned 16 bit number
        // printf("Raw value: 0x%03x, voltage: %f V\n", result, result * conversion_factor);
        // sleep_ms(500);
        }

    // Turn off led when pushbutton is HIGH
    gpio_put(LEDPIN, 0); // gpio_put function: put the pin 0 (off)

    }
    
    // // Turn off led when pushbutton is HIGH
    // gpio_put(LEDPIN, 0); // gpio_put function: put the pin 0 (off)

    return 0;
}