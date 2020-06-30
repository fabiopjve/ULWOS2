/******************************************************************************

ULWOS2 - Ultra LightWeight Operating System 2
This is a very simple thread scheduler completely written in C

Author: Fábio Pereira
Date: Apr, 23, 2020
embeddedsystems.io

*******************************************************************************/

#include "ULWOS2.h"

static tULWOS2threadControlBlock ULWOS2_threads[ULWOS2_MAX_THREADS];
static tULWOS2threadHandler currentQueueHead;
static tULWOS2threadHandler totalThreads;
static tULWOS2threadHandler lastThreadIndex;
static bool invalidateThreadPriorityQueue;

/*
 * ULWOS2_updateQueueOrder
 * Update queue ordering for the given thread
 */
static void ULWOS2_updateQueueOrder(tULWOS2threadHandler newThread)
{
    tULWOS2threadHandler currentIndex = currentQueueHead, previousIndex = ULWOS2_INVALID_HANDLER;
    if (currentQueueHead == ULWOS2_INVALID_HANDLER) {
        currentQueueHead = newThread;
        return;
    }
    if (ULWOS2_threads[newThread].priority < ULWOS2_threads[currentQueueHead].priority) {
        ULWOS2_threads[newThread].nextThread = currentQueueHead;
        currentQueueHead = newThread;
        return;
    }
    while (currentIndex < lastThreadIndex) {
        if (ULWOS2_threads[newThread].priority < ULWOS2_threads[currentIndex].priority) {
            ULWOS2_threads[previousIndex].nextThread = newThread;
            ULWOS2_threads[newThread].nextThread = currentIndex;
            return;
        }
        previousIndex = currentIndex;
        currentIndex = ULWOS2_threads[currentIndex].nextThread;
    }
    ULWOS2_threads[previousIndex].nextThread = newThread;
    ULWOS2_threads[newThread].nextThread = ULWOS2_INVALID_HANDLER;
}

/*
 * ULWOS2_orderPriority
 * Order all threads in the given state according to their priority
 */
static void ULWOS2_orderPriority(eULWOS2threadState state)
{
    tULWOS2threadHandler currentIndex;
    // first we clear all ordering data within the TCB
    for (currentIndex = 0; currentIndex <= lastThreadIndex; currentIndex++) {
        ULWOS2_threads[currentIndex].nextThread = ULWOS2_INVALID_HANDLER;
    }
    // invalidate head, it will get set once threads are ordered (if there is any thread in the specified state)
    currentQueueHead = ULWOS2_INVALID_HANDLER;
    currentIndex = 0;
    // enqueue all threads according to their priority and state
    while (currentIndex <= lastThreadIndex) {
        if (ULWOS2_threads[currentIndex].state == state) ULWOS2_updateQueueOrder(currentIndex);
        currentIndex++;
    }
    invalidateThreadPriorityQueue = false;
}

/*
 * ULWOS2_setThreadTimerMs
 * Set the thread timer to the desired interval in ms and set thread state to sleeping.
 * The thread will resume operation once the time interval has elapsed
 */
void ULWOS2_setThreadTimerMs(tULWOS2Timer interval)
{
    ULWOS2_threads[currentQueueHead].timerStart = ULWOS2_getMilliseconds();
    ULWOS2_threads[currentQueueHead].timerInterval = interval;
    ULWOS2_threads[currentQueueHead].state = THREAD_WAITING_FOR_TIMER;
    invalidateThreadPriorityQueue = true;   // force a new sorting of the priority queue
}

/*
 * ULWOS2_checkTimers
 * Check all thread timers and resume the threads with expired timers
 */
void static ULWOS2_checkTimers(void)
{
    tULWOS2threadHandler thisThread = 0;
    tULWOS2Timer now = ULWOS2_getMilliseconds();
    while (thisThread <= lastThreadIndex) {
        if (ULWOS2_threads[thisThread].state == THREAD_WAITING_FOR_TIMER) {
            // check if enough time have elapsed
            if ((now-ULWOS2_threads[thisThread].timerStart) >= ULWOS2_threads[thisThread].timerInterval) {
                // ok enough time have elapsed, this thread is ready to run again
                ULWOS2_threads[thisThread].state = THREAD_READY;
                invalidateThreadPriorityQueue = true;
            }
        }
        thisThread++;
    }
}

/*
 * ULWOS2_waitForSignal
 * Force the thread into sleep waiting for the specified signal
 * The thread will resume operation once the signal arrives
 */
void ULWOS2_waitForSignal(tULWOS2threadSignal signal)
{
    ULWOS2_threads[currentQueueHead].signal = signal;
    ULWOS2_threads[currentQueueHead].state = THREAD_WAITING_FOR_SIGNAL;
    invalidateThreadPriorityQueue = true;   // force a new sorting of the priority queue   
}

/*
 * ULWOS2_sendSignal
 * Send the specified signal, this will resume all threads that are waiting for it
 */
void ULWOS2_sendSignal(tULWOS2threadSignal signal)
{
    tULWOS2threadHandler thisThread = 0;
    while (thisThread <= lastThreadIndex) {
        if (ULWOS2_threads[thisThread].state == THREAD_WAITING_FOR_SIGNAL && ULWOS2_threads[thisThread].signal == signal) {
            ULWOS2_threads[thisThread].state = THREAD_READY;
            invalidateThreadPriorityQueue = true;
        }
        thisThread++;
    }
}

/*
 * ULWOS2_killThread
 * Kills the thread, it won't run anymore
 */
void ULWOS2_killThread(void)
{
    ULWOS2_threads[currentQueueHead].state = THREAD_NOT_READY;
    invalidateThreadPriorityQueue = true;   // force a new sorting of the priority queue
    totalThreads--;
    if (currentQueueHead == lastThreadIndex) lastThreadIndex--;
}

/*
 * ULWOS2_init
 * Initializes all threads and ULWOS2 states
 */
void ULWOS2_init()
{
    tULWOS2threadHandler thisThread = 0;
    while (thisThread < ULWOS2_MAX_THREADS) {
        ULWOS2_threads[thisThread].address = NULL;
        ULWOS2_threads[thisThread].priority = ULWOS2_PRIO_MIN;
        ULWOS2_threads[thisThread].state = THREAD_NOT_READY;
        ULWOS2_threads[thisThread].nextThread = ULWOS2_INVALID_HANDLER;
        ULWOS2_threads[thisThread].timerStart = 0;
        ULWOS2_threads[thisThread].timerInterval = 0;
        thisThread++;
    }
    invalidateThreadPriorityQueue = true;
    totalThreads = 0;
    lastThreadIndex = 0;
    currentQueueHead = ULWOS2_INVALID_HANDLER;
}

/*
 * ULWOS2_createThread
 * Creates a new thread. It expects two parameters:
 *  newThread - the function to run
 *  newPriority - the desired priority for this thread (0 is the highest)
 * Returns: tULWOS2threadHandler, a handler to this thread
 */
tULWOS2threadHandler ULWOS2_createThread(void(*newThread)(), tULWOS2threadPriority newPriority)
{
    tULWOS2threadHandler thisThread = 0;
    if (totalThreads >= ULWOS2_MAX_THREADS) thisThread = ULWOS2_INVALID_HANDLER;
    // find the next available entry on TCB array
    while (thisThread < ULWOS2_MAX_THREADS) {
        if (ULWOS2_threads[thisThread].state == THREAD_NOT_READY) {
            ULWOS2_threads[thisThread].address = newThread;
            ULWOS2_threads[thisThread].priority = newPriority;
            ULWOS2_threads[thisThread].state = THREAD_READY;
            totalThreads++;
            invalidateThreadPriorityQueue = true;
            if (thisThread > lastThreadIndex) lastThreadIndex = thisThread;
            break;
        }
        thisThread++;
    }
    return thisThread;
}

/*
 * ULWOS2_startScheduler
 * Starts ULWOS2 scheduler, this function should not return!
 */
void __attribute__ ((naked)) ULWOS2_startScheduler()
{
    tULWOS2threadHandler queueHead;
    tULWOS2threadPriority currentPriority = ULWOS2_PRIO_MIN;
    void (*threadToRun)(void);
    while(1) {
        if (invalidateThreadPriorityQueue) {
            ULWOS2_orderPriority(THREAD_READY);
            queueHead = currentQueueHead;
            if (currentQueueHead != ULWOS2_INVALID_HANDLER) currentPriority = ULWOS2_threads[currentQueueHead].priority;
            else currentPriority = ULWOS2_PRIO_MIN;
        } else currentQueueHead = queueHead;
        // Runs all threads in READY state
        while (currentQueueHead != ULWOS2_INVALID_HANDLER && ULWOS2_threads[currentQueueHead].priority <= currentPriority) {
            threadToRun = ULWOS2_threads[currentQueueHead].address;
            if (threadToRun != NULL) threadToRun();
            currentQueueHead = ULWOS2_threads[currentQueueHead].nextThread;
        }
        ULWOS2_checkTimers();
    }
}
