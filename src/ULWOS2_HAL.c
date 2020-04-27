#include <sys/time.h>
#include "ULWOS2_HAL.h"

#if ULWOS2_TARGET == LINUX
tULWOS2Timer getMilliseconds(void)
{
    struct timeval now;
    gettimeofday(&now,NULL);
    return now.tv_sec*1000LLU + now.tv_usec/1000;
}
#endif
