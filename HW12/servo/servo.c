#include "pico/stdlib.h"
#include "hardware/pwm.h"
#define LEDPin 25 // the built in LED on the Pico
#define pwmPin 15 //pin outputing pwm 

uint16_t PWM_init(int pin){
    gpio_set_function(pin, GPIO_FUNC_PWM); // Set the inputted Pin to be PWM
    uint slice_num = pwm_gpio_to_slice_num(pin); // Get PWM slice number
    float div = 40; // must be between 1-255
    pwm_set_clkdiv(slice_num, div); // divider
    uint16_t wrap = 62500; // when to rollover, must be less than 65535
    pwm_set_wrap(slice_num, wrap); //frequency of the pwm will be 50Hz (required for rc servo)
    pwm_set_enabled(slice_num, true); // turn on the PWM
    return wrap; //return wrap value to use in main function
}

void set_angle(int pin, int duty){
    pwm_set_gpio_level(pin, duty); // set the duty cycle to inputted number (between 0 to wrap)
}

int main(){
    uint16_t w = PWM_init(pwmPin); 

    int start = (int) (w * 0.025);
    int end = (int) (w * 0.125); 

    while (1){
        for (int i = start; i < end; i = i + 3){
                set_angle(pwmPin, i); //set duty cycle 
                sleep_ms(1);
        }
        for (int i = end; i > start; i = i - 3){
                set_angle(pwmPin, i);
                sleep_ms(1);
        } 
    }
    



}