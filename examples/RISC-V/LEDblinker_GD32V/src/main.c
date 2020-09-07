/*
    LED blinker with ULWOS2 on GD32V

    Author: Fábio Pereira
    www.embeddedsystems.io
    Date: August, 30th, 2020
*/

#include "gd32v_pjt_include.h"
#include "ULWOS2.h"

/*
    Blinks red LED (50ms on and 450ms off)
*/
void ledBlinker_thread(void)
{
    ULWOS2_THREAD_START();
    while(1)
    {
        LEDR(0);    // red LED on
        ULWOS2_THREAD_SLEEP_MS(50);
        LEDR(1);    // red LED off
        ULWOS2_THREAD_SLEEP_MS(450);
    }
}

void longanNanoInit(void)
{
    SystemInit();
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1|GPIO_PIN_2);
    LEDR(1);    // red LED off
    LEDG(1);    // green LED off
    LEDB(1);    // blue LED off
}

void main(void)
{
    longanNanoInit();
    ULWOS2_INIT();
    ULWOS2_THREAD_CREATE(ledBlinker_thread, 10);
    ULWOS2_START_SCHEDULER();	
}