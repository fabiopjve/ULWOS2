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

ULWOS2_DEFINE_SIGNAL(THREAD2_START);
ULWOS2_DEFINE_SIGNAL(THREAD3_START);
ULWOS2_CREATE_QUEUE(TEST_QUEUE1, uint8_t, 4);
ULWOS2_CREATE_QUEUE(TEST_QUEUE2, uint32_t, 8);

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

void testThread3(void)
{
    static uint32_t queueTest;
    ULWOS2_THREAD_START();
    ULWOS2_THREAD_WAIT_FOR_SIGNAL(THREAD3_START);
    globalTest = 6;
    ULWOS2_TRY_DEQUEUE(TEST_QUEUE2, queueTest);
    if (queueTest == 123456) globalTest = 7; else globalTest = 8;
    ULWOS2_TRY_DEQUEUE(TEST_QUEUE2, queueTest);
    if (queueTest == 7654321) globalTest = 9; else globalTest = 10;
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
    // now test thread 3
    globalTest = 0;
    TEST(TCB[2]->state == THREAD_WAITING_FOR_SIGNAL);// thread 3 should be waiting for signal
    // send signal
    ULWOS2_THREAD_SEND_SIGNAL(THREAD3_START);
    // thread 2 should be ready to run but as it is lower priority, it should not run yet
    TEST(TCB[2]->state == THREAD_READY);    // thread 3 should be ready
    TEST(globalTest==0);        // as thread 3 didn't run yet, globalTest should still be 0
    TEST(ULWOS2_ENQUEUE(TEST_QUEUE2,123456));   // successfully enqueued data
    ULWOS2_THREAD_SLEEP_MS(1);
    TEST(globalTest==7);        // thread 3 dequeued data correctly
    TEST(TCB[2]->state == THREAD_WAITING_FOR_SIGNAL);// thread 3 should be waiting for signal (blocked due to queue empty)
    ULWOS2_TRY_ENQUEUE(TEST_QUEUE2,7654321);
    ULWOS2_THREAD_SLEEP_MS(1);
    TEST(globalTest==9);        // thread 3 dequeued data correctly
    TEST(TCB[2]->state == THREAD_NOT_READY);// thread 2 should not be ready (killed)

    unitTest_results();   
}

int main()
{
    ULWOS2_INIT();
    TCB[0] = ULWOS2_THREAD_CREATE(testThread1, 10);
    TCB[1] = ULWOS2_THREAD_CREATE(testThread2, 1);
    TCB[2] = ULWOS2_THREAD_CREATE(testThread3, 11);
    TCB[3] = ULWOS2_THREAD_CREATE(unitTest_controlThread, 0);
    // check TCBs
    TEST(TCB[0]->entryPoint == testThread1);            // make sure entry point is correct
    TEST(TCB[0]->resumePoint > testThread1);            // make sure resume point is after entry point
    TEST(TCB[0]->nextThread == TCB[1]);                 // make sure this TCB points to the right next one
    TEST(TCB[1]->entryPoint == testThread2);            // make sure entry point is correct
    TEST(TCB[1]->resumePoint > testThread2);            // make sure resume point is after entry point
    TEST(TCB[1]->nextThread == TCB[2]);                 // make sure this TCB points to the right next one
    TEST(TCB[2]->entryPoint == testThread3);            // make sure entry point is correct
    TEST(TCB[2]->resumePoint > testThread3);            // make sure resume point is after entry point
    TEST(TCB[2]->nextThread == TCB[3]);                 // the last TCB should point to NULL    
    TEST(TCB[3]->entryPoint == unitTest_controlThread); // make sure entry point is correct
    TEST(TCB[3]->resumePoint > unitTest_controlThread); // make sure resume point is after entry point
    TEST(TCB[3]->nextThread == NULL);                   // the last TCB should point to NULL
    // Check queue operation
    TEST(ULWOS2_ENQUEUE(TEST_QUEUE1,10));               // make sure we can enqueue data
    uint8_t testData = 0;
    TEST(ULWOS2_DEQUEUE(TEST_QUEUE1,testData));         // we successfully dequeued data
    TEST(testData == 10);                               // dequeued data was correct
    TEST(ULWOS2_DEQUEUE(TEST_QUEUE1,testData)==false);  // we should not be able to dequeue again (queue is empty)
    TEST(ULWOS2_ENQUEUE(TEST_QUEUE1,20));               // make sure we can enqueue data
    TEST(ULWOS2_ENQUEUE(TEST_QUEUE1,30));               // make sure we can enqueue data
    TEST(ULWOS2_ENQUEUE(TEST_QUEUE1,40));               // make sure we can enqueue data
    TEST(ULWOS2_ENQUEUE(TEST_QUEUE1,50));               // make sure we can enqueue data
    TEST(ULWOS2_ENQUEUE(TEST_QUEUE1,60)==false);        // make should not be able to enqueue again (queue is full)
    TEST(ULWOS2_DEQUEUE(TEST_QUEUE1,testData));         // we successfully dequeued data
    TEST(testData == 20);                               // dequeued data was correct

    // now start the scheduler
    ULWOS2_START_SCHEDULER();
}
