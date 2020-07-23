/******************************************************************************
  ULWOS2 example on Arduino, debouncing a push button
  - Thread 1 - debouncing thread

  In order to test this example, connect a push button between pin D2 and GND
  
  Author: Fábio Pereira
  Date: Jul, 21, 2020
  embeddedsystems.io
*******************************************************************************/

#include <ULWOS2.h>

#define BUTTON_PIN    2   // the pin connected to the push button
#define DEBOUCE_TIME  30  // debounce time in ms

void debouncer(void)
{
  ULWOS2_THREAD_START();  // this is necessary for each thread in ULWOS2!
  while (1) {
    // wait for button press
    while (digitalRead(BUTTON_PIN) == HIGH) ULWOS2_THREAD_SLEEP_MS(5);  // wait 5ms for next sample
    // if we are here, the button was pressed!
    // wait to make sure it is still pressed (debounce it)
    ULWOS2_THREAD_SLEEP_MS(DEBOUCE_TIME);
    if (digitalRead(BUTTON_PIN) == LOW) {
        // key is pressed, toggle LED state
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        // now wait until the key is released
        while (digitalRead(BUTTON_PIN) == LOW) ULWOS2_THREAD_SLEEP_MS(5);  // wait 5ms for next try
    }
  }
}

void setup() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_BUILTIN, OUTPUT);

    // set initial LED state
    digitalWrite(LED_BUILTIN, LOW);

    ULWOS2_INIT();  // initialize ULWOS2
    ULWOS2_THREAD_CREATE(debouncer, 0);  // create debouncer thread with priority 0 (maximum)
}

void loop() {
    // run ULWOS2 scheduler (it will run all threads that are ready to run)
    ULWOS2_START_SCHEDULER();  
}
