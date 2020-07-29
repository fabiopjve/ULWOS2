#include <ULWOS2.h>

/*
  ULWOS2 - single thread running LED blinker on Arduino
  Author: Fábio Pereira
  Please visit www.embeddedsystems.io for more information on ULWOS2
*/

// This is our thread, it blinks the builtin LED without blocking code
void thread1(void)
{
  ULWOS2_THREAD_START();	// this is necessary for each thread in ULWOS2!
  while(1)
  {
    digitalWrite(LED_BUILTIN, HIGH);	// turn LED on
    ULWOS2_THREAD_SLEEP_MS(50);	      // sleep for 50ms
    digitalWrite(LED_BUILTIN, LOW);	  // turn LED off
    ULWOS2_THREAD_SLEEP_MS(500);	    // sleep for 500ms
  }
}  

void setup() {
  ULWOS2_INIT();	// initialize ULWOS2
  ULWOS2_THREAD_CREATE(thread1, 10);	// create thread 1 with priority 10
  pinMode(LED_BUILTIN, OUTPUT);	// initialize LED I/O pin as output
}

void loop() {
  // run ULWOS2 scheduler (it will run all threads that are ready to run)
  ULWOS2_START_SCHEDULER();
}
