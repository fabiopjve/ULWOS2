/*
*/

#include "gd32v_pjt_include.h"
#include "ULWOS2.h"
#include "LEDThreads.h"

uint8_t redDutyCycle = 0;
uint8_t greenDutyCycle = 0;
uint8_t blueDutyCycle = 0;
uint8_t period = 25;

// Red LED software PWM
void redPWMthread(void)
{
    int8_t sleepTime;
    ULWOS2_THREAD_START();
    while(1)
    {
        // first calculate active active time (LED on)
        sleepTime = (redDutyCycle * period) / 100;
        if (sleepTime>0) {
            LEDR(0);    // red LED on
            ULWOS2_THREAD_SLEEP_MS(sleepTime);
        }
        // now calculate inactive time (LED off)
        sleepTime = ((100 - redDutyCycle) * period) / 100;
        if (sleepTime>0) {
            LEDR(1);    // red LED off
            ULWOS2_THREAD_SLEEP_MS(sleepTime);
        }
    }
}

// Red LED brightness control (breathing)
void redThread(void)
{
    ULWOS2_THREAD_START();
    ULWOS2_THREAD_CREATE(redPWMthread, 0);
    while(1)
    {
        // count up to 255 (100%)
        while (redDutyCycle < 255) {
            redDutyCycle++;
            ULWOS2_THREAD_SLEEP_MS(5); // update PWM every 10ms
        }
        // now count down to 0 (0%)
        while (redDutyCycle > 0) {
            redDutyCycle--;
            ULWOS2_THREAD_SLEEP_MS(5); // update PWM every 10ms
        }
        // wait 250ms before starting another cycle
        ULWOS2_THREAD_SLEEP_MS(250);
    }
}

// Green LED software PWM
void greenPWMthread(void)
{
    int8_t sleepTime;
    ULWOS2_THREAD_START();
    while(1)
    {
        // first calculate active active time (LED on)
        sleepTime = (greenDutyCycle * period) / 100;
        if (sleepTime>0) {
            LEDG(0);    // green LED on
            ULWOS2_THREAD_SLEEP_MS(sleepTime);
        }
        // now calculate inactive time (LED off)
        sleepTime = ((100 - greenDutyCycle) * period) / 100;
        if (sleepTime>0) {
            LEDG(1);    // green LED off
            ULWOS2_THREAD_SLEEP_MS(sleepTime);
        }
    }
}

// Green LED brightness control (breathing)
void greenThread(void)
{
    ULWOS2_THREAD_START();
    ULWOS2_THREAD_CREATE(greenPWMthread, 0);
    while(1)
    {
        // count up to 255 (100%)
        while (greenDutyCycle < 255) {
            greenDutyCycle++;
            ULWOS2_THREAD_SLEEP_MS(6); // update PWM every 10ms
        }
        // now count down to 0 (0%)
        while (greenDutyCycle > 0) {
            greenDutyCycle--;
            ULWOS2_THREAD_SLEEP_MS(6); // update PWM every 10ms
        }
        // wait 250ms before starting another cycle
        ULWOS2_THREAD_SLEEP_MS(250);
    }
}

// Blue LED software PWM
void bluePWMthread(void)
{
    int8_t sleepTime;
    ULWOS2_THREAD_START();
    while(1)
    {
        // first calculate active active time (LED on)
        sleepTime = (blueDutyCycle * period) / 100;
        if (sleepTime>0) {
            LEDB(0);    // blue LED on
            ULWOS2_THREAD_SLEEP_MS(sleepTime);
        }
        // now calculate inactive time (LED off)
        sleepTime = ((100 - blueDutyCycle) * period) / 100;
        if (sleepTime>0) {
            LEDB(1);    // blue LED off
            ULWOS2_THREAD_SLEEP_MS(sleepTime);
        }
    }
}

// Blue LED brightness control (breathing)
void blueThread(void)
{
    ULWOS2_THREAD_START();
    ULWOS2_THREAD_CREATE(bluePWMthread, 0);
    while(1)
    {
        // count up to 255 (100%)
        while (blueDutyCycle < 255) {
            blueDutyCycle++;
            ULWOS2_THREAD_SLEEP_MS(7); // update PWM every 10ms
        }
        // now count down to 0 (0%)
        while (blueDutyCycle > 0) {
            blueDutyCycle--;
            ULWOS2_THREAD_SLEEP_MS(7); // update PWM every 10ms
        }
        // wait 250ms before starting another cycle
        ULWOS2_THREAD_SLEEP_MS(250);
    }
}

void createLEDthreads(void)
{
    ULWOS2_THREAD_CREATE(redThread, 1);
    ULWOS2_THREAD_CREATE(greenThread, 1);
    ULWOS2_THREAD_CREATE(blueThread, 1);   
}