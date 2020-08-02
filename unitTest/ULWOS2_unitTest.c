/******************************************************************************

This is a simple unit test for ULWOS2
Author: Fábio Pereira
Date: Apr, 23, 2020
embeddedsystems.io

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "../src/ULWOS2.h"

#define TEST(cond) {totalTests++; if (!(cond)) {printf("Fail line %d\n",__LINE__);totalFails++;}}

uint32_t globalTest = 0, totalTests = 0, totalFails = 0;
tULWOS2threadControlBlock *TCB[4];

#define THREAD2_START 1

void testThread1(void)
{
    ULWOS2_THREAD_START();
    globalTest = 1;
    ULWOS2_THREAD_SLEEP_MS(1);
    globalTest = 2;
    ULWOS2_THREAD_SLEEP_MS(2);
    globalTest = 3;
    ULWOS2_THREAD_KILL();
}

void testThread2(void)
{
    ULWOS2_THREAD_START();
    ULWOS2_THREAD_WAIT_FOR_SIGNAL(THREAD2_START);
    globalTest = 4;
    ULWOS2_THREAD_SLEEP_MS(1);
    globalTest = 5;
    ULWOS2_THREAD_KILL();
}

void unitTest_results(void)
{
    printf("ULWOS2 unit test results: %d tests %d fails\n",totalTests,totalFails);
    if (!totalFails) printf("PASS!!!\n"); else printf("FAIL\n");
    exit(0);
}

void unitTest_controlThread(void)
{
    ULWOS2_THREAD_START();
    globalTest = 0;
    // start testing thread 1
    ULWOS2_THREAD_SLEEP_MS(1);
    TEST(globalTest==1);    // test thread 1 is yielding properly
    TEST(TCB[0]->state == THREAD_READY);    // thread 1 should be ready
    ULWOS2_THREAD_SLEEP_MS(1);
    TEST(globalTest==2);    // test thread 1 is yielding properly
    TEST(TCB[0]->state == THREAD_WAITING_FOR_TIMER);// thread 1 should be waiting for timer
    ULWOS2_THREAD_SLEEP_MS(2);
    TEST(globalTest==3);    // test thread 1 is yielding properly
    TEST(TCB[0]->state == THREAD_NOT_READY);// thread 1 should not be ready (killed)
    // now test thread 2
    globalTest = 0;
    TEST(TCB[1]->state == THREAD_WAITING_FOR_SIGNAL);// thread 2 should be waiting for signal
    // send signal
    ULWOS2_THREAD_SEND_SIGNAL(THREAD2_START);
    // thread 2 should be ready to run but as it is lower priority, it should not run yet
    TEST(TCB[1]->state == THREAD_READY);    // thread 2 should be ready
    TEST(globalTest==0);        // as thread 2 didn't run yet, globalTest should still be 0
    ULWOS2_THREAD_SLEEP_MS(1);  // sleep for 1ms, this is gonna give a chance for thread 2 to run
    TEST(globalTest==4);
    ULWOS2_THREAD_SLEEP_MS(1);  // sleep for 1ms, this is gonna give a chance for thread 2 to run
    TEST(globalTest==5);
    TEST(TCB[1]->state == THREAD_NOT_READY);// thread 2 should not be ready (killed)
    unitTest_results();   
}

int main()
{
    ULWOS2_INIT();
    TCB[0] = ULWOS2_THREAD_CREATE(testThread1, 10);
    TCB[1] = ULWOS2_THREAD_CREATE(testThread2, 1);
    TCB[2] = ULWOS2_THREAD_CREATE(unitTest_controlThread, 0);
    // check TCBs
    TEST(TCB[0]->entryPoint == testThread1);            // make sure entry point is correct
    TEST(TCB[0]->resumePoint > testThread1);            // make sure resume point is after entry point
    TEST(TCB[0]->nextThread == TCB[1]);                 // make sure this TCB points to the right next one
    TEST(TCB[1]->entryPoint == testThread2);            // make sure entry point is correct
    TEST(TCB[1]->resumePoint > testThread2);            // make sure resume point is after entry point
    TEST(TCB[1]->nextThread == TCB[2]);                 // make sure this TCB points to the right next one       
    TEST(TCB[2]->entryPoint == unitTest_controlThread); // make sure entry point is correct
    TEST(TCB[2]->resumePoint > unitTest_controlThread); // make sure resume point is after entry point
    TEST(TCB[2]->nextThread == NULL);                   // the last TCB should point to NULL
    // now start the scheduler
    ULWOS2_START_SCHEDULER();
}
