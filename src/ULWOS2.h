/******************************************************************************

ULWOS2 - Ultra LightWeight Operating System 2
This is a very simple thread scheduler completely written in C

Author: Fábio Pereira
Date: Apr, 23, 2020
embeddedsystems.io

*******************************************************************************/

#ifndef __ULWOS2_H
#define __ULWOS2_H

#include <stdint.h>
#include <stdbool.h>
#include "ULWOS2_HAL.h"

#define ULWOS2_PRIO_MAX 0
#define ULWOS2_PRIO_MIN 255

// This full initializer is required by g++ (Arduino)
#ifdef ULWOS2_SIGNALS
    #define _TCB_INIT {NULL,NULL,NULL,0,0,THREAD_NOT_READY,0,0}
#else
    #define _TCB_INIT {NULL,NULL,NULL,0,0,THREAD_NOT_READY,0}
#endif
/*
 * ULWOS2_THREAD_START() 
 * This is the initial constructor for any thread. It is responsible for
 * changing code flow upon resuming a thread
 */
#define ULWOS2_THREAD_START()   _ULWOS2_THREAD_START()

/*
 * ULWOS2_THREAD_YIELD() 
 * Causes the thread to yield control back to the scheduler. It doesn't change thread state,
 * so if there are other threads ready to run with a priority higher or equal to the current
 * level (of this thread), they are gonna have a chance to run
 */
#define ULWOS2_THREAD_YIELD()   _ULWOS2_THREAD_YIELD()

/*
 * ULWOS2_THREAD_SLEEP_MS(interval) 
 * Causes the thread to suspend for the given time interval in milliseconds (up to 65535). 
 * Running this command yields control back to the scheduler.
 */
#define ULWOS2_THREAD_SLEEP_MS(interval) _ULWOS2_THREAD_SLEEP_MS(interval)

/*
 * ULWOS2_DEFINE_SIGNAL(signal) 
 * Define a new signal
 */
#define ULWOS2_DEFINE_SIGNAL(signal)  _ULWOS2_DEFINE_SIGNAL(signal)

/*
 * ULWOS2_THREAD_SEND_SIGNAL(signal) 
 * Send a signal. This is a synchronization command. All threads that are waiting for the
 * given signal will resume their operation (according to their priority)
 */
#define ULWOS2_THREAD_SEND_SIGNAL(signal) _ULWOS2_THREAD_SEND_SIGNAL(signal)

/*
 * ULWOS2_THREAD_WAIT_FOR_SIGNAL(signal) 
 * Suspend the thread and wait for the given signal. The thread is gonna wait until the signal
 * is received. Note that if the signal is sent before running this command, it will not 
 * be detected by this thread!
 */
#define ULWOS2_THREAD_WAIT_FOR_SIGNAL(signal) _ULWOS2_THREAD_WAIT_FOR_SIGNAL(signal)

/*
 * ULWOS2_CREATE_MESSAGE_QUEUE(id,type,size) 
 * Create a new message queue using "id" as identifier. The queue created by this call is going to 
 * have "size" elements of type "type"
 * Example: in order to create a 16-level deep queue of chars named MY_QUEUE use
 * ULWOS2_CREATE_MESSAGE_QUEUE(MY_QUEUE, char, 16)
 * Note: queues created this way are only accessible within the same module (file). In
 * order to have a globally accessible queue, define it in a separate header and include
 * that header in all modules that are going to access it
 */
#define ULWOS2_CREATE_MESSAGE_QUEUE(id,type,size)  _ULWOS2_CREATE_MESSAGE_QUEUE(id,type,size)

/*
 * ULWOS2_MESSAGE_ENQUEUE(id,data)
 * Enqueue data into the message queue "id". This command does not block the thread 
 * and can be used from outside thread code.
 * Returns:
 *  true - data was successfully queued
 *  false - data was not queued (queue is full)
 */
#define ULWOS2_MESSAGE_ENQUEUE(id,data) _ULWOS2_MESSAGE_ENQUEUE(id,data)

/*
 * ULWOS2_MESSAGE_TRY_ENQUEUE(id,data)
 * Tries to enqueue data into the message queue "id". If the message queue is full, this
 * command will block the thread until the queue has space available.
 * Note that this command can only be used inside a thread!
 */
#define ULWOS2_MESSAGE_TRY_ENQUEUE(id,data) _ULWOS2_MESSAGE_TRY_ENQUEUE(id,data)  

/*
 * ULWOS2_MESSAGE_DEQUEUE(id, data)
 * Dequeue data from the message queue "id". This command does not block the thread and
 * can be used from outside thread code.
 * Note that this is a macro and it directly changes the contents of "data" variable. You
 * don't need to pass a pointer to that variable.
 * Returns:
 *  true - data was successfully dequeued
 *  false - data was not dequeued (queue is empty)
 */
#define ULWOS2_MESSAGE_DEQUEUE(id, data)    _ULWOS2_MESSAGE_DEQUEUE(id, data)

/*
 * ULWOS2_MESSAGE_TRY_DEQUEUE(id,data)
 * Tries to dequeue data from the message queue "id". If the message queue is empty, this
 * command will block the thread until the queue has data available.
 * Note that this command can only be used inside a thread!
 */
#define ULWOS2_MESSAGE_TRY_DEQUEUE(id, data) _ULWOS2_MESSAGE_TRY_DEQUEUE(id, data) 

/*
 * ULWOS2_THREAD_RESET() - forces the thread to resume operation from its initial state. Note that
 * this command does not change the state of any variable.
 */
#define ULWOS2_THREAD_RESET() _ULWOS2_THREAD_RESET()

/*
 * ULWOS2_THREAD_KILL() - kills this thread, it will not run again
 */
#define ULWOS2_THREAD_KILL() _ULWOS2_THREAD_KILL()

/*
 * ULWOS2_THREAD_CREATE(func,prio) - creates a new thread using the given function and priority
 */
#define ULWOS2_THREAD_CREATE(func,prio) ULWOS2_createThread(func,prio)

/*
 * ULWOS2_INIT() - initializes ULWOS2
 */
#define ULWOS2_INIT() ULWOS2_init()

/*
 * ULWOS2_START_SCHEDULER() - starts ULWOS2 scheduler (this function does not return)
 */
#define ULWOS2_START_SCHEDULER() ULWOS2_startScheduler()

/*
 * ULWOS2 implementations
 */
#define _ULWOS2_THREAD_YIELD()                  ({ULWOS2_tcb.resumePoint= (void (*)())&&GLUE3(LB,__FUNCTION__,__LINE__);\
                                                return; GLUE3(LB,__FUNCTION__,__LINE__): while(0);})
#define _ULWOS2_THREAD_SLEEP_MS(interval)       ({ULWOS2_setThreadTimerMs(interval); ULWOS2_THREAD_YIELD();})
#define _ULWOS2_DEFINE_SIGNAL(signal)           enum {signal = __COUNTER__};\
                                                ULWOS2_ASSERT(signal<256,SIGNAL_ID_DOES_NOT_FIT_8BITS);
#define _ULWOS2_THREAD_SEND_SIGNAL(signal)      ({ULWOS2_sendSignal(signal); ULWOS2_THREAD_YIELD();})
#define _ULWOS2_THREAD_WAIT_FOR_SIGNAL(signal)  ({ULWOS2_waitForSignal(signal); ULWOS2_THREAD_YIELD();})
#define _ULWOS2_CREATE_MESSAGE_QUEUE(id,type,size)      type id##_QUEUE[size];\
                                                tULWOS2_queueCTRL id##_CTRL = { 0, 0, 0, size};\
                                                ULWOS2_DEFINE_SIGNAL(id)\
                                                ULWOS2_ASSERT(size<256,QUEUE_SIZE_DOES_NOT_FIT_8BITS)
#define _ULWOS2_MESSAGE_ENQUEUE(id,data)        ((id##_CTRL.queueUsed < id##_CTRL.queueSize) ? \
                                                id##_QUEUE[id##_CTRL.head] = data,\
                                                ULWOS2_updateEnqueue(&id##_CTRL),\
                                                ULWOS2_sendSignal(id),true\
                                                :false)
#define _ULWOS2_MESSAGE_TRY_ENQUEUE(id,data)    {while(id##_CTRL.queueUsed >= id##_CTRL.queueSize) {ULWOS2_THREAD_WAIT_FOR_SIGNAL(id);}\
                                                id##_QUEUE[id##_CTRL.head] = data;\
                                                ULWOS2_updateEnqueue(&id##_CTRL);\
                                                ULWOS2_sendSignal(id);}
#define _ULWOS2_MESSAGE_DEQUEUE(id, data)       ((id##_CTRL.queueUsed > 0) ? \
                                                data = id##_QUEUE[id##_CTRL.tail],\
                                                ULWOS2_updateDequeue(&id##_CTRL),\
                                                ULWOS2_sendSignal(id),true\
                                                :false)
#define _ULWOS2_MESSAGE_TRY_DEQUEUE(id, data)   {while (id##_CTRL.queueUsed == 0) {ULWOS2_THREAD_WAIT_FOR_SIGNAL(id);} \
                                                data = id##_QUEUE[id##_CTRL.tail];\
                                                ULWOS2_updateDequeue(&id##_CTRL);\
                                                ULWOS2_sendSignal(id);}
#define _ULWOS2_THREAD_RESET()                  ({ULWOS2_tcb.resumePoint = NULL; return;})
#define _ULWOS2_THREAD_KILL()                   ({ULWOS2_killThread(); return;})
#define _ULWOS2_THREAD_START()                  static tULWOS2threadControlBlock ULWOS2_tcb = _TCB_INIT;\
                                                if (ULWOS2_tcb.resumePoint!=NULL) goto *ULWOS2_tcb.resumePoint; else ULWOS2_tempPointer = &ULWOS2_tcb;\
                                                ULWOS2_THREAD_YIELD()
#define ULWOS2_ASSERT(cond,msg)                 enum {GLUE4(ASSERTION_ERROR_LINE_,__LINE__,_,msg) = 1/(cond)}
#define GLUE(x, y)                              x ## y
#define GLUE2(x,y)                              GLUE(x, y)
#define GLUE3(x,y,z)                            GLUE2(GLUE2(x, y), z)
#define GLUE4(a,b,c,d)                          GLUE3(a,b,GLUE2(c,d))

/*
 * ULWOS2 typedefs
 */
typedef enum {
    THREAD_NOT_READY,
    THREAD_WAITING_FOR_SIGNAL,
    THREAD_WAITING_FOR_TIMER,
    THREAD_READY
} __attribute__ ((__packed__)) eULWOS2threadState;

typedef uint8_t tULWOS2threadHandler;
typedef uint8_t tULWOS2threadPriority;
typedef uint8_t tULWOS2threadSignal;
typedef void(*tVoidFunctionPointer)();

typedef struct sTCB {
    tVoidFunctionPointer entryPoint;          // thread entry point
    tVoidFunctionPointer resumePoint;         // thread resume point
    struct sTCB *nextThread;                  // pointer to the next thread
    uint16_t timerInterval;                   // desired timer interval in ms
    tULWOS2threadPriority priority;           // priority
    eULWOS2threadState state;                 // current state  
    tULWOS2Timer timerStart;                  // milliseconds when timer was set 
    #ifdef ULWOS2_SIGNALS
    tULWOS2threadSignal signal;               // the signal this thread is waiting for
    #endif
} tULWOS2threadControlBlock;

typedef struct sULWOS2_queue {
    uint8_t head;                           // Queue head
    uint8_t tail;                           // Queue tail
    uint8_t queueUsed;                      // Number of elements currently queued
    uint8_t queueSize;                      // Maximum number of elements in queue
} tULWOS2_queueCTRL;

extern tULWOS2threadControlBlock *ULWOS2_tempPointer;

#ifdef __cplusplus
    // This is needed for Arduino
    extern "C" {void ULWOS2_setThreadTimerMs(uint16_t interval);}
    extern "C" {void ULWOS2_sendSignal(tULWOS2threadSignal signal);}
    extern "C" {void ULWOS2_waitForSignal(tULWOS2threadSignal signal);}
    extern "C" {void ULWOS2_updateEnqueue(tULWOS2_queueCTRL * queue);}
    extern "C" {void ULWOS2_updateDequeue(tULWOS2_queueCTRL * queue);}
    extern "C" {void ULWOS2_killThread(void);}
    extern "C" {void ULWOS2_init();}
    extern "C" {tULWOS2threadControlBlock * ULWOS2_createThread(void(*thisFunction)(), tULWOS2threadPriority thisPriority);}
    extern "C" {void ULWOS2_startScheduler() __attribute__ ((noreturn));}
#else
    // Standard C building environmet
    void ULWOS2_setThreadTimerMs(uint16_t interval);
    void ULWOS2_sendSignal(tULWOS2threadSignal signal);
    void ULWOS2_waitForSignal(tULWOS2threadSignal signal);
    void ULWOS2_updateEnqueue(tULWOS2_queueCTRL * queue);
    void ULWOS2_updateDequeue(tULWOS2_queueCTRL * queue);
    void ULWOS2_killThread(void);
    void ULWOS2_init();
    tULWOS2threadControlBlock * ULWOS2_createThread(void(*thisFunction)(), tULWOS2threadPriority thisPriority);
    void ULWOS2_startScheduler() __attribute__ ((noreturn));
#endif	/* __cplusplus */

#endif
