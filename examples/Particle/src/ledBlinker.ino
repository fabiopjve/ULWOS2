/**
 * ULWOS2 - led blinker
 * This example creates two threads, each one blinking one LED
 * ledBlinkThread1 blinks an LED connected to D7
 * ledBlinkThread2 blinks an LED connected to D6
 * A third thread is used for servicing ParticleOS
 * 
 * Author:  Fábio Pereira
 * Date:    Apr, 26, 2020
 * embeddedsystems.io
 **/
#include "Particle.h"
#include "ULWOS2.h"

#define LED1	D7
#define LED2	D6

void ledBlinkThread1(void)
{
	ULWOS2_THREAD_START();
	while (1) {
		digitalWrite(LED1, HIGH);		// LED on
		ULWOS2_THREAD_SLEEP_MS(250);	// wait 250ms
		digitalWrite(LED1, LOW);		// LED off
		ULWOS2_THREAD_SLEEP_MS(250);	// wait 250ms 	
	}
}

void ledBlinkThread2(void)
{
	ULWOS2_THREAD_START();
	while (1) {
		digitalWrite(LED2, HIGH);		// LED on
		ULWOS2_THREAD_SLEEP_MS(250);	// wait 250ms
		digitalWrite(LED2, LOW);		// LED off
		ULWOS2_THREAD_SLEEP_MS(250);	// wait 250ms 	
	}
}

void threadKeepAlive(void)
{
	ULWOS2_THREAD_START();
	while (1) {
		Particle.process();
		ULWOS2_THREAD_SLEEP_MS(100);	// wait 100ms 	
	}
}

void setup()
{
	pinMode(LED1, OUTPUT);
	pinMode(LED2, OUTPUT);
}

void loop() 
{
  ULWOS2_init();
  ULWOS2_createThread(ledBlinkThread1, 2);
  ULWOS2_createThread(ledBlinkThread2, 2);
  ULWOS2_createThread(threadKeepAlive, 1);
  ULWOS2_startScheduler();  
}