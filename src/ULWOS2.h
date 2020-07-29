/******************************************************************************

ULWOS2 - Ultra lightweight Operating System 2
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

#define ULWOS2_INVALID_HANDLER 255

#define GLUE(x, y) x ## y
#define GLUE2(x,y) GLUE(x, y)
#define GLUE3(x, y, z) GLUE2(GLUE2(x, y), z)

// This full initializer is required by g++ (Arduino)
#ifdef ULWOS2_SIGNALS
#define _TCB_INIT {NULL,NULL,NULL,0,0,THREAD_NOT_READY,0,0}
#else
#define _TCB_INIT {NULL,NULL,NULL,0,0,THREAD_NOT_READY,0}
#endif
/*
 * ULWOS2_THREAD_START() - this is the initial constructor for any thread. It is responsible for
 * changing code flow upon resuming a thread
 */
#define ULWOS2_THREAD_START()   static tULWOS2threadControlBlock ULWOS2_tcb = _TCB_INIT;\
                                if (ULWOS2_tcb.resumePoint!=NULL) goto *ULWOS2_tcb.resumePoint; else ULWOS2_tempPointer = &ULWOS2_tcb;\
                                ULWOS2_THREAD_YIELD()

/*
 * ULWOS2_THREAD_YIELD() - causes the thread to yield control back to the scheduler. It doesn't
 * change thread state, so if there are other threads ready to run with a priority higher or
 * equal to the current level (of this thread), they are gonna have a chance to run
 */
#define ULWOS2_THREAD_YIELD() ({ULWOS2_tcb.resumePoint= (void (*)())&&GLUE3(LB,__FUNCTION__,__LINE__); return; GLUE3(LB,__FUNCTION__,__LINE__): while(0);})

/*
 * ULWOS2_THREAD_SLEEP_MS(interval) - causes the thread to suspend for the given time interval in
 * milliseconds (up to 65535). Running this command yields control back to the scheduler.
 */
#define ULWOS2_THREAD_SLEEP_MS(interval) ({ULWOS2_setThreadTimerMs(interval); ULWOS2_THREAD_YIELD();})

/*
 * ULWOS2_THREAD_SEND_SIGNAL(signal) - sends a signal. This is a synchronization command. All threads
 * that are waiting for the given signal will resume their operation (according to their priority)
 */
#define ULWOS2_THREAD_SEND_SIGNAL(signal) ({ULWOS2_sendSignal(signal); ULWOS2_THREAD_YIELD();})

/*
 * ULWOS2_THREAD_WAIT_FOR_SIGNAL(signal) - suspends the thread and waits for the given signal. The 
 * thread is gonna wait until the signal is received. Note that the signal is sent before running
 * this command, it will not be detected by this thread
 */
#define ULWOS2_THREAD_WAIT_FOR_SIGNAL(signal) ({ULWOS2_waitForSignal(signal); ULWOS2_THREAD_YIELD();})

/*
 * ULWOS2_THREAD_RESET() - forces the thread to resume operation from its initial state. Note that
 * this command does not change the state of any variable.
 */
#define ULWOS2_THREAD_RESET() ({ULWOS2_tcb.resumePoint = NULL; return;})

/*
 * ULWOS2_THREAD_KILL() - kills this thread, it will not run again
 */
#define ULWOS2_THREAD_KILL() ({ULWOS2_killThread(); return;})

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
 * ULWOS2 typedefs
 */
typedef enum {
    THREAD_NOT_READY,
    THREAD_SLEEPING,
    THREAD_WAITING_FOR_SIGNAL,
    THREAD_WAITING_FOR_TIMER,
    THREAD_READY
} __attribute__ ((__packed__)) eULWOS2threadState;

typedef uint8_t tULWOS2threadHandler;
typedef uint8_t tULWOS2threadPriority;
typedef uint8_t tULWOS2threadSignal;

typedef struct sTCB {
    void (*entryPoint)();                     // thread entry point
    void (*resumePoint)();                    // thread resume point
    struct sTCB *nextThread;                  // pointer to the next thread
    uint16_t timerInterval;                   // desired timer interval in ms
    tULWOS2threadPriority priority;           // priority
    eULWOS2threadState state;                 // current state  
    tULWOS2Timer timerStart;                  // milliseconds when timer was set 
    #ifdef ULWOS2_SIGNALS
    tULWOS2threadSignal signal;               // the signal this thread is waiting for
    #endif
} tULWOS2threadControlBlock;

extern tULWOS2threadControlBlock *ULWOS2_tempPointer;

#ifdef __cplusplus
    // This is needed for Arduino
    extern "C" {void ULWOS2_setThreadTimerMs(uint16_t interval);}
    extern "C" {void ULWOS2_sendSignal(tULWOS2threadSignal signal);}
    extern "C" {void ULWOS2_waitForSignal(tULWOS2threadSignal signal);}
    extern "C" {void ULWOS2_killThread(void);}
    extern "C" {void ULWOS2_init();}
    extern "C" {tULWOS2threadControlBlock * ULWOS2_createThread(void(*thisFunction)(), tULWOS2threadPriority thisPriority);}
    extern "C" {void ULWOS2_startScheduler() __attribute__ ((noreturn));}
#else
    // Standard C building environmet
    void ULWOS2_setThreadTimerMs(uint16_t interval);
    void ULWOS2_sendSignal(tULWOS2threadSignal signal);
    void ULWOS2_waitForSignal(tULWOS2threadSignal signal);
    void ULWOS2_killThread(void);
    void ULWOS2_init();
    tULWOS2threadControlBlock * ULWOS2_createThread(void(*thisFunction)(), tULWOS2threadPriority thisPriority);
    void ULWOS2_startScheduler() __attribute__ ((noreturn));
#endif	/* __cplusplus */

#endif
