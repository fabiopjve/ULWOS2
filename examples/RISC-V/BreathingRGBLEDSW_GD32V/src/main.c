/*
    RGB breathing LED with ULWOS2 on GD32V

    Author: Fábio Pereira
    www.embeddedsystems.io
    Date: September, 1st, 2020
*/

#include "gd32v_pjt_include.h"
#include "ULWOS2.h"
#include "LEDThreads.h"

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
    createLEDthreads();
	ULWOS2_START_SCHEDULER();	
}