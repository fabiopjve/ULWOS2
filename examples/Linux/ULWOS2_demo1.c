/******************************************************************************

ULWOS2 example with four threads
Author: Fábio Pereira
Date: Apr, 23, 2020
embeddedsystems.io

*******************************************************************************/
#include <stdio.h>
#include "../../src/ULWOS2.h"

#define SIGNAL_RUN_THREAD3  1
#define SIGNAL_RUN_THREAD4  2

#define CLEAR_TERMINAL() printf("\x1B[2J")
#define PRINT_RED() printf("\x1B[31m")
#define PRINT_GREEN() printf("\x1B[32m")
#define PRINT_BLUE() printf("\x1B[34m")
#define PRINT_WHITE() printf("\x1B[0m")

void asciiCursorXY(uint8_t x, uint8_t y)
{
  printf("\x1B[%hu;%huH",y,x);
}

void printCharXY(uint8_t x, uint8_t y, char ch)
{
  asciiCursorXY(x,y);
  printf("%c",ch);
  fflush(stdout);
}

void testThread1(void)
{
    static uint8_t posX = 0;
    const uint8_t posY = 2;
    PRINT_RED();
    ULWOS2_THREAD_START();
    asciiCursorXY(0,posY);
    printf("Thread 1:");
    while (1) {
        if (posX<20) {
            printCharXY(10+posX,posY,'.');
            ULWOS2_THREAD_SLEEP_MS(200);
            if (posX == 10) ULWOS2_THREAD_SEND_SIGNAL(SIGNAL_RUN_THREAD3);
            posX++;
        } else break;
    }
    asciiCursorXY(10+posX,posY);
    printf(" Done!\n");
    ULWOS2_THREAD_KILL();
}

void testThread2(void)
{
    static uint8_t posX = 0;
    const uint8_t posY = 3;
    PRINT_BLUE();
    ULWOS2_THREAD_START();
    asciiCursorXY(0,posY);
    printf("Thread 2:");
    while (1) {
        if (posX<20) {
            printCharXY(10+posX,posY,'.');
            ULWOS2_THREAD_SLEEP_MS(300);
            posX++;
        } else break;
    }
    asciiCursorXY(10+posX,posY);
    printf(" Done!\n");
    ULWOS2_THREAD_SEND_SIGNAL(SIGNAL_RUN_THREAD4);
    ULWOS2_THREAD_KILL();
}

void testThread3(void)
{
    static uint8_t posX = 0;
    const uint8_t posY = 4;
    PRINT_GREEN();
    ULWOS2_THREAD_START();
    asciiCursorXY(0,posY);
    printf("Thread 3:");
    ULWOS2_THREAD_WAIT_FOR_SIGNAL(SIGNAL_RUN_THREAD3);
    while (1) {
        if (posX<20) {
            printCharXY(10+posX,posY,'.');
            ULWOS2_THREAD_SLEEP_MS(250);
            posX++;
        } else break;
    }
    asciiCursorXY(10+posX,posY);
    printf(" Done!\n");
    ULWOS2_THREAD_KILL();
}

void testThread4(void)
{
    static uint8_t posX = 0;
    const uint8_t posY = 5;
    PRINT_WHITE();
    ULWOS2_THREAD_START();
    asciiCursorXY(0,posY);
    printf("Thread 4:");
    ULWOS2_THREAD_WAIT_FOR_SIGNAL(SIGNAL_RUN_THREAD4);
    while (1) {
        if (posX<20) {
            printCharXY(10+posX,posY,'.');
            ULWOS2_THREAD_SLEEP_MS(250);
            posX++;
        } else break;
    }
    asciiCursorXY(10+posX,posY);
    printf(" Done!\n");
    ULWOS2_THREAD_KILL();
}

int main()
{
    CLEAR_TERMINAL();
    asciiCursorXY(0,1);
    printf("ULWOS2 demo!\n");
    ULWOS2_INIT();
    ULWOS2_THREAD_CREATE(testThread1, 1);
    ULWOS2_THREAD_CREATE(testThread2, 2);
    ULWOS2_THREAD_CREATE(testThread3, 3);
    ULWOS2_THREAD_CREATE(testThread4, 4);    
    ULWOS2_START_SCHEDULER();
}
