/******************************************************************************

ULWOS2 example with two threads
Author: Fábio Pereira
Date: Apr, 23, 2020
embeddedsystems.io

*******************************************************************************/
#include <stdio.h>
#include "src/ULWOS2.h"

void testThread1(void)
{
    ULWOS2_THREAD_START()
    while (1) {
        printf(".");
        fflush(stdout);
        ULWOS2_THREAD_SLEEP_MS(250);
        printf("1");
        fflush(stdout);
        ULWOS2_THREAD_SLEEP_MS(250);
    }
}

void testThread2(void)
{
    ULWOS2_THREAD_START()
    while (1) {
        printf("!");
        fflush(stdout);
        ULWOS2_THREAD_SLEEP_MS(250);
        printf("2");
        fflush(stdout);
        ULWOS2_THREAD_SLEEP_MS(260);
    }
}

int main()
{
    printf("ULWOS2 demo!\n");
    ULWOS2_init();
    ULWOS2_createThread(testThread1, 1);
    ULWOS2_createThread(testThread2, 2);
    ULWOS2_startScheduler();
}
