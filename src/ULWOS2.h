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

#ifndef ULWOS2_MAX_THREADS
#define ULWOS2_MAX_THREADS 32
#endif

#define ULWOS2_PRIO_MAX 0
#define ULWOS2_PRIO_MIN 255

#define ULWOS2_INVALID_HANDLER 255

#define GLUE1(x, y) x ## y
#define GLUE2(x, y) GLUE1(x, y)
#define ULWOS2_THREAD_START() static void *jumper = NULL; if (jumper!=NULL) goto *jumper;
#define ULWOS2_THREAD_YIELD() ({jumper= &&GLUE2(LB,__LINE__); return; GLUE2(LB,__LINE__): while(0);})
#define ULWOS2_THREAD_SLEEP_MS(interval) ({ULWOS2_setThreadTimerMs(interval); ULWOS2_THREAD_YIELD();})
#define ULWOS2_THREAD_SEND_SIGNAL(signal) ({ULWOS2_sendSignal(signal); ULWOS2_THREAD_YIELD();})
#define ULWOS2_THREAD_WAIT_FOR_SIGNAL(signal) ({ULWOS2_waitForSignal(signal); ULWOS2_THREAD_YIELD();})
#define ULWOS2_THREAD_RESET() ({jumper = NULL; return;})

typedef enum {
    THREAD_NOT_READY,
    THREAD_BLOCKED,
    THREAD_SLEEPING,
    THREAD_WAITING_FOR_SIGNAL,
    THREAD_WAITING_FOR_TIMER,
    THREAD_READY
} eULWOS2threadState;

typedef uint8_t tULWOS2threadHandler;
typedef uint8_t tULWOS2threadPriority;
typedef uint8_t tULWOS2threadSignal;

typedef struct {
  void (*address)();                  // thread entry point
  tULWOS2threadPriority priority;     // priority
  eULWOS2threadState state;           // current state
  tULWOS2threadHandler nextThread;    // handler of the next thread
  tULWOS2Timer timerStart;            // milliseconds when timer was set
  tULWOS2Timer timerInterval;         // desired timer interval in ms
  tULWOS2threadSignal signal;         // the signal this thread is waiting for
} tULWOS2threadControlBlock;

void ULWOS2_setThreadTimerMs(tULWOS2Timer interval);
void ULWOS2_sendSignal(tULWOS2threadSignal signal);
void ULWOS2_waitForSignal(tULWOS2threadSignal signal);
void ULWOS2_init();
tULWOS2threadHandler ULWOS2_createThread(void(*thisFunction)(), tULWOS2threadPriority thisPriority);
void ULWOS2_startScheduler();

#endif
