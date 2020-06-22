#ifndef __ULWOS2_CONFIG_H
#define __ULWOS2_CONFIG_H

// Define ULWOS target platform:
// ULWOS2_TARGET_LINUX for Linux console
// ULWOS2_TARGET_PARTICLE for Particle devices
// ULWOS2_TARGET_MSP430 for TI MSP430 devices
// New targets will get added soon!
#define ULWOS2_TARGET_LINUX 		1
#define ULWOS2_TARGET_PARTICLE 		2
#define ULWOS2_TARGET_MSP430        3

#ifndef ULWOS2_TARGET
#define ULWOS2_TARGET ULWOS2_TARGET_PARTICLE
#endif

#endif