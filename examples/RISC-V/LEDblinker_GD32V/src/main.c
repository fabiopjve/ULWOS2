/*
    CDC example on GD32VF103 on Longan Nano using PlatformIO and modified
    CDC ACM library

    This very basic example will echo all received characters back to the
    host computer.

    Author: Fábio Pereira
    www.embeddedsystems.io
    Date: August, 29th, 2020
*/

#include "gd32v_pjt_include.h"
#include "ULWOS2.h"

void thread1(void)
{
	ULWOS2_THREAD_START();
	while(1)
	{
		LEDR(0); // toggle red LED
		ULWOS2_THREAD_SLEEP_MS(50);
        LEDR(1);
        ULWOS2_THREAD_SLEEP_MS(500);
	}
}

void longanNanoInit(void)
{
    SystemInit();
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1|GPIO_PIN_2);
    LEDR(1);
    LEDG(1);
    LEDB(1);    
}

void main(void)
{
    longanNanoInit();
	ULWOS2_INIT();
	ULWOS2_THREAD_CREATE(thread1, 10);
	ULWOS2_START_SCHEDULER();	
}