/******************************************************************************

    ULWOS2 example 2 eight threads on the MSP430F2013
    - Thread 1 - software PWM on P1.0
    - Thread 2 - LED brightness control
    - Thread 3 - LED blinker on P1.1
    - Thread 4 - LED blinker on P1.2
    - Thread 5 - LED blinker on P1.3
    - Thread 6 - LED blinker on P1.4
    - Thread 7 - LED blinker on P1.5
    - Thread 8 - LED blinker on P1.6
    
    Note: this code is a demonstration only! Following ULWOS2 v1.0.0 RAM usage
    increased slightly and now this example is playing a dangerous game with a stack
    overflow, it is working as it is, but it would be safer to work with only seven
    threads.

    Author: Fábio Pereira
    Date: Jun, 28, 2020
    embeddedsystems.io

*******************************************************************************/

#include <msp430.h>
#include "../../src/ULWOS2.h"

static int16_t milliSeconds;
uint8_t dutyCycle, period;

tULWOS2Timer ULWOS2_getMilliseconds(void)
{
	return milliSeconds;    // no need for critical section here!
}

void __attribute__ ((interrupt(TIMERA0_VECTOR))) Timer_A_Ch0 (void)
{
	CCR0 += 1000;		// Add 1ms offset to CCR0
	milliSeconds++;
}

void systemInit(void)
{
	WDTCTL = WDTPW | WDTHOLD;		// stop watchdog timer
	// configure the DCO to operate at 16MHz
	DCOCTL = CALDCO_16MHZ;
	BCSCTL1 = CALBC1_16MHZ;
	// set SMCLK to 8MHz
	BCSCTL2 = DIVS_1;
	// configure P1.0, 1.1 and 1.2 as outputs
	P1DIR = 0x07;
	// configure Timer A as a free running timer (1us per increment)
	TACTL = TASSEL_2 | ID_3 | MC_2;
	// configure Timer A channel 0 to compare mode
	CCR0 = 999;
	CCTL0 = CCIE;					// CCR0 interrupt enabled
	__bis_SR_register(GIE);        // enable interrupts
	period = 25;    // period is 50ms
	dutyCycle = 0;  // dutyCycle is 0%
}

// software PWM thread on P1.0
void softPWMthread(void)
{
    int8_t sleepTime;
    ULWOS2_THREAD_START();
    while(1)
    {
        sleepTime = (dutyCycle * period) / 100;
        P1OUT |= 0x01;				// turn LED on
        ULWOS2_THREAD_SLEEP_MS(sleepTime);
        sleepTime = ((100 - dutyCycle) * period) / 100;
        P1OUT &= ~0x01;              // turn LED off
        ULWOS2_THREAD_SLEEP_MS(sleepTime);
    }
}

// LED brightness control (breathing)
void breathThread(void)
{
    ULWOS2_THREAD_START();
    while(1)
    {
        while (dutyCycle < 100) {
          dutyCycle++;
          ULWOS2_THREAD_SLEEP_MS(10); // update PWM every 10ms
        }
        while (dutyCycle > 0) {
          dutyCycle--;
          ULWOS2_THREAD_SLEEP_MS(10); // update PWM every 10ms
        }
        // wait 250ms before starting another cycle
        ULWOS2_THREAD_SLEEP_MS(250);
    }
}

// Blink an LED connected to pin P1.1 at 2 Hz
void blink1(void)
{
    ULWOS2_THREAD_START();
    while(1)
    {
        P1OUT ^= 0x02;  // complement state of pin P1.1
        ULWOS2_THREAD_SLEEP_MS(250);
    }
}

// Blink an LED connected to pin P1.2 at 1 Hz
void blink2(void)
{
    ULWOS2_THREAD_START();
    while(1)
    {
        P1OUT ^= 0x04;  // complement state of pin P1.2
        ULWOS2_THREAD_SLEEP_MS(500);
    }
}

// Blink an LED connected to pin P1.3 at 4 Hz
void blink3(void)
{
    ULWOS2_THREAD_START();
    while(1)
    {
        P1OUT ^= 0x08;  // complement state of pin P1.3
        ULWOS2_THREAD_SLEEP_MS(125);
    }
}

// Blink an LED connected to pin P1.4 at ~8 Hz
void blink4(void)
{
    ULWOS2_THREAD_START();
    while(1)
    {
        P1OUT ^= 0x10;  // complement state of pin P1.4
        ULWOS2_THREAD_SLEEP_MS(62);
    }
}

// Blink an LED connected to pin P1.5 at ~16 Hz
void blink5(void)
{
    ULWOS2_THREAD_START();
    while(1)
    {
        P1OUT ^= 0x20;  // complement state of pin P1.5
        ULWOS2_THREAD_SLEEP_MS(31);
    }
}

// Blink an LED connected to pin P1.6 at ~33 Hz
void blink6(void)
{
    ULWOS2_THREAD_START();
    while(1)
    {
        P1OUT ^= 0x40;  // complement state of pin P1.6
        ULWOS2_THREAD_SLEEP_MS(15);
    }
}

void main(void)
{
    systemInit();
    ULWOS2_INIT();
    ULWOS2_THREAD_CREATE(softPWMthread, 0);
    ULWOS2_THREAD_CREATE(breathThread, 1);
    ULWOS2_THREAD_CREATE(blink1, 2);
    ULWOS2_THREAD_CREATE(blink2, 2);
    ULWOS2_THREAD_CREATE(blink3, 2);
    ULWOS2_THREAD_CREATE(blink4, 2);
    ULWOS2_THREAD_CREATE(blink5, 2);
    ULWOS2_THREAD_CREATE(blink6, 2);
    ULWOS2_START_SCHEDULER();
}
