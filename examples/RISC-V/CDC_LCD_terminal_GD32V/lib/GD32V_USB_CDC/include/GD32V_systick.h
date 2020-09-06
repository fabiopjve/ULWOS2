/*
    GD32V systick library

    This is an improvement over GigaDevice's original V1.0.0 systick library

    Author: Fábio Pereira
    www.embeddedsystems.io

    Date: August, 30th, 2020
*/

#ifndef __GD32V_SYSTICK_H
#define __GD32V_SYSTICK_H

#include <stdint.h>

/*
    Halts execution for the specified amount of milliseconds
    @param millis - number of milliseconds to pause
*/
void GD32V_delayMs(uint32_t count);

/*
    Halts execution for the specified amount of microseconds
    @param micros - number of microseconds to pause
*/
void GD32V_delayus(uint32_t micros);

/*
    Gets the amount of milliseconds ellapsed since last reset
    @return milliseconds since reset
*/
uint32_t GD32V_getMillis(void);

/*
    Gets the amount of microseconds ellapsed since last reset
    @return microseconds since reset
*/
uint32_t GD32V_getMicros(void);

#endif /* __GD32V_SYSTICK_H */
