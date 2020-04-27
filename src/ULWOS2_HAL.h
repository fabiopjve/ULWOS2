#ifndef __ULWOS2_HAL_H
#define __ULWOS2_HAL_H

#include <stdint.h>
#include <stddef.h>

#ifndef ULWOS2_TARGET
#define ULWOS2_TARGET LINUX
#endif

#if ULWOS2_TARGET == LINUX
typedef uint64_t tULWOS2Timer;
#else
typedef uint32_t tULWOS2Timer;
#endif

tULWOS2Timer getMilliseconds(void);

#endif
