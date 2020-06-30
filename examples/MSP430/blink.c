#include <msp430.h>
#include "../../src/ULWOS2.h"

/******************************************************************************

ULWOS2 example with a single thread on the MSP430F2013
Author: Fábio Pereira
Date: Jun, 20, 2020
embeddedsystems.io

*******************************************************************************/

static int16_t milliSeconds;

tULWOS2Timer ULWOS2_getMilliseconds(void)
{
	return milliSeconds;
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
	// configure P1.0 as output
	P1DIR = 0x01;
	// configure Timer A as a free running timer (1us per increment)
	TACTL = TASSEL_2 | ID_3 | MC_2;
	// configure Timer A channel 0 to compare mode
	CCR0 = 999;
	CCTL0 = CCIE;					// CCR0 interrupt enabled
	__bis_SR_register(GIE);        // enable interrupts
}

void thread1(void)
{
	ULWOS2_THREAD_START();
	while(1)
	{
		P1OUT ^= 0x01;				// toggle P1.0
		ULWOS2_THREAD_SLEEP_MS(250);
	}
}

void main(void)
{
	systemInit();
	ULWOS2_INIT();
	ULWOS2_THREAD_CREATE(thread1, 10);
	ULWOS2_START_SCHEDULER();	
}