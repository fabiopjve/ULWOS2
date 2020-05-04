/******************************************************************************

ULWOS2 example with two threads
Author: Fábio Pereira
Date: Apr, 23, 2020
embeddedsystems.io

*******************************************************************************/
#include <stdio.h>
#include "../../src/ULWOS2.h"

#define SIGNAL_RUN_THREAD3  1
#define SIGNAL_RUN_THREAD4  2

void testThread1(void)
{
    static uint8_t counter = 5;
    ULWOS2_THREAD_START();
    while (1) {
        printf(".");
        fflush(stdout);
        ULWOS2_THREAD_YIELD();
        printf(".");
        fflush(stdout);
        ULWOS2_THREAD_SLEEP_MS(250);
        printf("1");
        fflush(stdout);
        ULWOS2_THREAD_YIELD();
        printf("1");
        fflush(stdout);
        ULWOS2_THREAD_SLEEP_MS(250);
        counter--;
        if (counter == 0) {
            counter = 5;
            ULWOS2_THREAD_SLEEP_MS(3000);
            ULWOS2_THREAD_SEND_SIGNAL(SIGNAL_RUN_THREAD3);
        }
    }
}

void testThread2(void)
{
    ULWOS2_THREAD_START();
    while (1) {
        printf("!");
        fflush(stdout);
        ULWOS2_THREAD_YIELD();
        printf("!");
        fflush(stdout);
        ULWOS2_THREAD_SLEEP_MS(250);
        printf("2");
        fflush(stdout);
        ULWOS2_THREAD_YIELD();
        printf("2");
        fflush(stdout);
        ULWOS2_THREAD_SLEEP_MS(260);
    }
}

void testThread3(void)
{
    ULWOS2_THREAD_START();
    while(1) {
        printf("T3 WAIT 1 ");
        fflush(stdout);
        ULWOS2_THREAD_WAIT_FOR_SIGNAL(SIGNAL_RUN_THREAD3);
        ULWOS2_THREAD_SEND_SIGNAL(SIGNAL_RUN_THREAD4);
    }
}

void testThread4(void)
{
    static uint8_t counter = 5;
    ULWOS2_THREAD_START();
    while(1) {
        printf("T4 WAIT 3 ");
        fflush(stdout);
        ULWOS2_THREAD_WAIT_FOR_SIGNAL(SIGNAL_RUN_THREAD4);
        counter--;
        if (counter == 0) {
            printf("T4 BYE ");
            fflush(stdout);
            ULWOS2_THREAD_KILL();
        }
    }
}

int main()
{
    printf("ULWOS2 demo!\n");
    ULWOS2_INIT();
    ULWOS2_THREAD_CREATE(testThread1, 1);
    ULWOS2_THREAD_CREATE(testThread2, 2);
    ULWOS2_THREAD_CREATE(testThread3, 20);
    ULWOS2_THREAD_CREATE(testThread4, 20);
    ULWOS2_START_SCHEDULER();
}
