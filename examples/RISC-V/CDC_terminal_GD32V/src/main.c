/*
    ULWOS2 + USB CDC on GD32V

    This example runs two threads:
    - one prints a string and toggles green LED every 10 seconds
    - a second thread that handles CDC processing and character echoing and toggles 
      red LED for each character (or block) received

    Author: Fábio Pereira
    www.embeddedsystems.io
    Date: September, 3rd, 2020
*/

#include "cdc_core.h"
#include "gd32v_pjt_include.h"
#include "ULWOS2.h"

/*
    Print a message on CDC device every 10 seconds
*/
void printEveryTenSeconds_thread(void)
{
    ULWOS2_THREAD_START();
    while(1)
    {
        ULWOS2_THREAD_SLEEP_MS(10000);
        cdc_print("Periodic ULWOS2 thread, 10 seconds!\n");
        LEDG_TOG;   // toggle green LED
    }
}

/*
    Process characters received on CDC device and prints them on the LCD
*/
void CDC_thread(void)
{
    ULWOS2_THREAD_START();
    // wait here until USB CDC is configured
    while (!cdc_isConfigured()) ULWOS2_THREAD_SLEEP_MS(1);
    // create printing thread
    ULWOS2_THREAD_CREATE(printEveryTenSeconds_thread, 10);
    while(1)
    {
        cdc_process();
        uint8_t buf[64];
        uint32_t rxLength = cdc_getReceivedData(buf, 64);
        if (rxLength) {
            cdc_sendData(buf, rxLength);
            LEDR_TOG;   // toggle red LED
        }
        // sleep for 1ms so that other threads can run
        ULWOS2_THREAD_SLEEP_MS(1);
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
    cdc_init();    
}

int main(void)
{
    longanNanoInit();
    ULWOS2_INIT();
    ULWOS2_THREAD_CREATE(CDC_thread, 1);
    ULWOS2_START_SCHEDULER();	
}