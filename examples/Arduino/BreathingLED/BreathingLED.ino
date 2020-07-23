/******************************************************************************
  ULWOS2 example on Arduino, breathing LED with two threads
  - Thread 1 - software PWM on builtin LED
  - Thread 2 - LED brightness control
  
  Author: Fábio Pereira
  Date: Jul, 20, 2020
  embeddedsystems.io
*******************************************************************************/

#include <ULWOS2.h>

uint8_t dutyCycle = 0, period = 25;

// Software PWM thread
void softPWMthread(void)
{
    int8_t sleepTime;
    ULWOS2_THREAD_START();
    while(1)
    {
        // first calculate active active time (LED on)
        sleepTime = (dutyCycle * period) / 100;
        if (sleepTime>0) {
            digitalWrite(LED_BUILTIN, HIGH);  // turn LED on
            ULWOS2_THREAD_SLEEP_MS(sleepTime);
        }
        // now calculate inactive time (LED off)
        sleepTime = ((100 - dutyCycle) * period) / 100;
        if (sleepTime>0) {
            digitalWrite(LED_BUILTIN, LOW);   // turn LED off
            ULWOS2_THREAD_SLEEP_MS(sleepTime);
        }
    }
}

// LED brightness control (breathing)
void breathThread(void)
{
    ULWOS2_THREAD_START();
    while(1)
    {
        // count up to 100 (100%)
        while (dutyCycle < 100) {
            dutyCycle++;
            ULWOS2_THREAD_SLEEP_MS(10); // update PWM every 10ms
        }
        // now count down to 0 (0%)
        while (dutyCycle > 0) {
            dutyCycle--;
            ULWOS2_THREAD_SLEEP_MS(10); // update PWM every 10ms
        }
        // wait 250ms before starting another cycle
        ULWOS2_THREAD_SLEEP_MS(250);
    }
}


void setup() {
    ULWOS2_INIT();
    ULWOS2_THREAD_CREATE(softPWMthread, 1);
    ULWOS2_THREAD_CREATE(breathThread, 0);
    pinMode(LED_BUILTIN, OUTPUT);  // initialize LED I/O pin as output
}

void loop() {
    // run ULWOS2 scheduler (it will run all threads that are ready to run)
    ULWOS2_START_SCHEDULER();
}
