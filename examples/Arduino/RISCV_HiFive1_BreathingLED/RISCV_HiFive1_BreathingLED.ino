/******************************************************************************
  ULWOS2 example on HiFive 1 board, breathing RGB LED with three threads
  - Thread 1 - Red LED brightness
  - Thread 2 - Green LED brightness 
  - Thread 3 - Blue LED brightness
  
  Author: Fábio Pereira
  Date: Aug, 02, 2020
  embeddedsystems.io
*******************************************************************************/

#include <ULWOS2.h>

uint8_t redDutyCycle = 0;
uint8_t greenDutyCycle = 0;
uint8_t blueDutyCycle = 0;

#define RED_LED     6   // Red LED is connected to GPIO22 which maps to Arduino D6
#define GREEN_LED   3   // Green LED is connected to GPIO19 which maps to Arduino D3
#define BLUE_LED    5   // Blue LED is connected to GPIO21 which maps to Arduino D5

// Red LED brightness control (breathing)
void redThread(void)
{
    ULWOS2_THREAD_START();
    while(1)
    {
        // count up to 255 (100%)
        while (redDutyCycle < 255) {
            redDutyCycle++;
            analogWrite(RED_LED, redDutyCycle);
            ULWOS2_THREAD_SLEEP_MS(5); // update PWM every 10ms
        }
        // now count down to 0 (0%)
        while (redDutyCycle > 0) {
            redDutyCycle--;
            analogWrite(RED_LED, redDutyCycle);
            ULWOS2_THREAD_SLEEP_MS(5); // update PWM every 10ms
        }
        // wait 250ms before starting another cycle
        ULWOS2_THREAD_SLEEP_MS(250);
    }
}

// Green LED brightness control (breathing)
void greenThread(void)
{
    ULWOS2_THREAD_START();
    while(1)
    {
        // count up to 255 (100%)
        while (greenDutyCycle < 255) {
            greenDutyCycle++;
            analogWrite(GREEN_LED, greenDutyCycle);
            ULWOS2_THREAD_SLEEP_MS(6); // update PWM every 10ms
        }
        // now count down to 0 (0%)
        while (greenDutyCycle > 0) {
            greenDutyCycle--;
            analogWrite(GREEN_LED, greenDutyCycle);
            ULWOS2_THREAD_SLEEP_MS(6); // update PWM every 10ms
        }
        // wait 250ms before starting another cycle
        ULWOS2_THREAD_SLEEP_MS(250);
    }
}

// Blue LED brightness control (breathing)
void blueThread(void)
{
    ULWOS2_THREAD_START();
    while(1)
    {
        // count up to 255 (100%)
        while (blueDutyCycle < 255) {
            blueDutyCycle++;
            analogWrite(BLUE_LED, blueDutyCycle);
            ULWOS2_THREAD_SLEEP_MS(7); // update PWM every 10ms
        }
        // now count down to 0 (0%)
        while (blueDutyCycle > 0) {
            blueDutyCycle--;
            analogWrite(BLUE_LED, blueDutyCycle);
            ULWOS2_THREAD_SLEEP_MS(7); // update PWM every 10ms
        }
        // wait 250ms before starting another cycle
        ULWOS2_THREAD_SLEEP_MS(250);
    }
}


void setup() {
    ULWOS2_INIT();
    ULWOS2_THREAD_CREATE(redThread, 0);
    ULWOS2_THREAD_CREATE(greenThread, 0);
    ULWOS2_THREAD_CREATE(blueThread, 0);
    pinMode(RED_LED, OUTPUT);   // initialize Red LED I/O pin as output
    pinMode(GREEN_LED, OUTPUT); // initialize Green LED I/O pin as output
    pinMode(BLUE_LED, OUTPUT);  // initialize Blue LED I/O pin as output
}

void loop() {
    // run ULWOS2 scheduler (it will run all threads that are ready to run)
    ULWOS2_START_SCHEDULER();
}
