/*
    GD32V systick library

    This is an improvement on the GigaDevice's original V1.0.0 systick library

    Author: Fábio Pereira
    www.embeddedsystems.io

    Date: August, 30th, 2020
*/

#include "gd32vf103.h"
#include "GD32V_systick.h"

/*
    Halts execution for the specified amount of milliseconds
    @param millis - number of milliseconds to pause
*/
void GD32V_delayMs(uint32_t millis)
{
    uint64_t startTime, deltaTime;
    startTime = get_timer_value();
    // wait until internal timer tick increments
    while (get_timer_value() == startTime);
    // wait here until the desired amount of time has elapsed
    do {
        deltaTime = get_timer_value() - startTime;
    } while (deltaTime < (SystemCoreClock/4000 * millis));
}

/*
    Halts execution for the specified amount of microseconds
    @param micros - number of microseconds to pause
*/
void GD32V_delayus(uint32_t micros)
{
    uint64_t startTime, deltaTime;
    startTime = get_timer_value();
    // wait until internal timer tick increments
    while (get_timer_value() == startTime);
    // wait here until the desired amount of time has elapsed
    do {
        deltaTime = get_timer_value() - startTime;
    } while (deltaTime < (SystemCoreClock/4000000 * micros));
}

/*
    Gets the amount of milliseconds ellapsed since last reset
    @return milliseconds since reset
*/
uint32_t GD32V_getMillis(void)
{
    uint64_t ticks = get_timer_value();
    return (ticks / (SystemCoreClock / 4000));
}

/*
    Gets the amount of microseconds ellapsed since last reset
    @return microseconds since reset
*/
uint32_t GD32V_getMicros(void)
{
    uint64_t ticks = get_timer_value();
    return (ticks / (SystemCoreClock / 4000000));
}
