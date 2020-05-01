#ifndef __ULWOS2_HAL_H
#define __ULWOS2_HAL_H

#include <stdint.h>
#include <stddef.h>
#include "ULWOS2_config.h"

#if defined(ULWOS2_TARGET_LINUX)
	typedef uint64_t tULWOS2Timer;
#elif defined(ULWOS2_TARGET_PARTICLE)
	typedef uint32_t tULWOS2Timer;
#else
	
#endif

tULWOS2Timer ULWOS2_getMilliseconds(void);

#endif
