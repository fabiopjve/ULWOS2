/******************************************************************************

ULWOS2 - Ultra LightWeight Operating System 2
This is a very simple thread scheduler completely written in C

Author: Fábio Pereira
Date: Apr, 23, 2020
embeddedsystems.io

*******************************************************************************/

#include "ULWOS2.h"

tULWOS2threadControlBlock *ULWOS2_tempPointer;
static tULWOS2threadControlBlock *currentTCB, *head;
static uint8_t invalidateThreadPriorityQueue;
#ifdef ULWOS2_SIGNALS
    static void * queuePointer;
#endif

// Internal function prototypes
static void ULWOS2_orderPriority(void);
static void ULWOS2_checkTimers(void);

/*
 * ULWOS2_setThreadTimerMs
 * Set the thread timer to the desired interval in ms (up to 65535ms) and set thread state to sleeping.
 * The thread will resume operation once the time interval has elapsed
 */
void ULWOS2_setThreadTimerMs(uint16_t interval)
{
    if (currentTCB != NULL) {
        currentTCB->timerStart = ULWOS2_getMilliseconds();
        currentTCB->timerInterval = interval;
        currentTCB->state = THREAD_WAITING_FOR_TIMER;
        invalidateThreadPriorityQueue = true;   // force a new sorting of the priority queue
    }
}

/*
 * ULWOS2_waitForSignal
 * Force the thread into sleep waiting for the specified signal
 * The thread will resume operation once the signal arrives
 */
#ifdef ULWOS2_SIGNALS
void ULWOS2_waitForSignal(tULWOS2threadSignal signal)
{
    if (currentTCB != NULL) {
        currentTCB->signal = signal;
        currentTCB->state = THREAD_WAITING_FOR_SIGNAL;
        invalidateThreadPriorityQueue = true;   // force a new sorting of the priority queue         
    } 
}
#endif

/*
 * ULWOS2_sendSignal
 * Send the specified signal, this will resume all threads that are waiting for it
 */
#ifdef ULWOS2_SIGNALS
void ULWOS2_sendSignal(tULWOS2threadSignal signal)
{
    tULWOS2threadControlBlock *thread = head;
    tULWOS2Timer now = ULWOS2_getMilliseconds();
    while (thread != NULL) {
        if (thread->state == THREAD_WAITING_FOR_SIGNAL && thread->signal == signal) {
            thread->state = THREAD_READY;
            invalidateThreadPriorityQueue = true;
        }
        thread = thread->nextThread;
    }
}
#endif

#ifdef ULWOS2_SIGNALS
void ULWOS2_updateEnqueue(tULWOS2_queueCTRL * queue)
{
    queue->head++;
    if (queue->head >= queue->queueSize) queue->head = 0;
    queue->queueUsed++;
}
#endif

#ifdef ULWOS2_SIGNALS
void ULWOS2_updateDequeue(tULWOS2_queueCTRL * queue)
{
    queue->tail++;
    if (queue->tail >= queue->queueSize) queue->tail = 0;
    queue->queueUsed--;
}
#endif

/*
 * ULWOS2_killThread
 * Kill the thread, it won't run anymore
 */
void ULWOS2_killThread(void)
{
    if (currentTCB != NULL) {
        currentTCB->state = THREAD_NOT_READY;
        currentTCB->resumePoint = NULL;
        invalidateThreadPriorityQueue = true;   // force a new sorting of the priority queue         
    }    
}

/*
 * ULWOS2_init
 * Initialize all threads and ULWOS2 states
 */
void ULWOS2_init()
{
    currentTCB = NULL;
    queuePointer = NULL;
}

/*
 * ULWOS2_createThread
 * Create a new thread. It expects two parameters:
 *  thread - the function to run
 *  priority - the desired priority for this thread (0 is the highest)
 * Returns: tULWOS2threadControlBlock *, a pointer to this thread's TCB
 */
tULWOS2threadControlBlock * ULWOS2_createThread(void(*thread)(), tULWOS2threadPriority priority)
{
    ULWOS2_tempPointer = NULL;
    thread();
    if (ULWOS2_tempPointer != NULL) {
        ULWOS2_tempPointer->entryPoint = thread;
        ULWOS2_tempPointer->priority = priority;
        ULWOS2_tempPointer->state = THREAD_READY;
        ULWOS2_tempPointer->nextThread = NULL;
        if (head == NULL) head = ULWOS2_tempPointer; else {
            tULWOS2threadControlBlock *thisThread = head;
            while (thisThread != NULL) {
                // traverse the thread list to add to the tail
                if (thisThread->nextThread == NULL) {
                    thisThread->nextThread = ULWOS2_tempPointer;
                    break;
                } else thisThread = thisThread->nextThread;
            }
        }
        invalidateThreadPriorityQueue = true;
    }
    return ULWOS2_tempPointer;
}

/*
 * ULWOS2_checkTimers
 * Check alls threads and resume the ones waiting for a timer that is expired
 */
static void ULWOS2_checkTimers(void)
{
    tULWOS2threadControlBlock *thread = head;
    tULWOS2Timer now = ULWOS2_getMilliseconds();
    while (thread != NULL) {
        if (thread->state == THREAD_WAITING_FOR_TIMER) {
            // check if enough time have elapsed
            if ((now-thread->timerStart) >= thread->timerInterval) {
                // ok enough time have elapsed, this thread is ready to run again
                thread->state = THREAD_READY;
                invalidateThreadPriorityQueue = true;
            }
        }
        thread = thread->nextThread;
    }
}

/*
 * ULWOS2_orderPriority
 * Order all threads in READY state according to their priority
 */
static void ULWOS2_orderPriority(void)
{
    tULWOS2threadControlBlock *currentNode = head;
    tULWOS2threadControlBlock *previousNode, *previousNode2;
    previousNode = NULL;
    if (head == NULL) return;   // nothing to do if there are no threads
    while (currentNode->nextThread != NULL) {
        previousNode2 = previousNode; 
        previousNode = currentNode;
        currentNode = currentNode->nextThread;  // go to next node
        if (currentNode->state == THREAD_READY && (currentNode->priority < previousNode->priority || previousNode->state !=THREAD_READY)) {
            // swap places
            if (previousNode2 != NULL) previousNode2->nextThread = currentNode; else
                if (head == previousNode) head = currentNode;
            previousNode->nextThread = currentNode->nextThread;
            currentNode->nextThread = previousNode;
            // following a swap, we restart from head
            currentNode = head;
            previousNode = NULL;
        } 
    }
    invalidateThreadPriorityQueue = false;
}

/*
 * ULWOS2_startScheduler
 * Starts ULWOS2 scheduler, this function does not return!
 */
void __attribute__ ((naked)) ULWOS2_startScheduler()
{
    tULWOS2threadPriority currentPriority = ULWOS2_PRIO_MIN;
    while(1) {
        if (invalidateThreadPriorityQueue) {
            ULWOS2_orderPriority();
            if (head != NULL) currentPriority = head->priority;
        }
        currentTCB = head;
        // Runs all threads in READY state which match current priority
        while (1) {
            if (currentTCB != NULL) {
                if (currentTCB->priority != currentPriority || currentTCB->state != THREAD_READY) break;
                currentTCB->entryPoint();
                currentTCB = currentTCB->nextThread;
            } else break;
        }
        ULWOS2_checkTimers();
    }
}
