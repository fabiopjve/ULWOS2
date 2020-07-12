#include "ULWOS2_HAL.h"

#if ULWOS2_TARGET == ULWOS2_TARGET_LINUX
    #include <sys/time.h>
    // This is get milliseconds for Linux targets
    tULWOS2Timer ULWOS2_getMilliseconds(void)
    {
        struct timeval now;
        gettimeofday(&now,NULL);
        return now.tv_sec*1000LLU + now.tv_usec/1000;
    }
#elif ULWOS2_TARGET == ULWOS2_TARGET_PARTICLE
    #include "Particle.h"
    // This is get milliseconds for Particle targets
    tULWOS2Timer ULWOS2_getMilliseconds(void)
    {
        return millis();
    }
#elif ULWOS2_TARGET == ULWOS2_TARGET_ARDUINO
    #include <Arduino.h>
    // This is get milliseconds for Arduino targets
    tULWOS2Timer ULWOS2_getMilliseconds(void)
    {
        return millis();
    }    
#else

#endif
