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
#define ULWOS2_THREAD_YIELD_STATE(state) ({jumper = &&state; return;})
#define ULWOS2_THREAD_SLEEP_MS(interval) ({ULWOS2_setThreadTimerMs(interval); jumper= &&GLUE2(LB,__LINE__); return; GLUE2(LB,__LINE__): while(0);})
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

typedef struct {
  void * address;
  tULWOS2threadPriority priority;
  eULWOS2threadState state;
  tULWOS2threadHandler nextThread;
  tULWOS2Timer timer;
} tULWOS2threadControlBlock;

void ULWOS2_setThreadTimerMs(tULWOS2Timer time);
void ULWOS2_init();
tULWOS2threadHandler ULWOS2_createThread(void* thisFunction, tULWOS2threadPriority thisPriority);
void ULWOS2_startScheduler();

#endif
