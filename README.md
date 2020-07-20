# ULWOS2
A super lightweight cooperative OS completely written in C which can target almost any platform.
ULWOS2 is different because it doesn't store a full context for each task, this greatly reduces stack usage.

Please check www.embeddedsystems.io for more information
## Basic API
* ULWOS2_INIT - initilizes ULWOS2
* ULWOS2_THREAD_CREATE - create a new ULWOS2 thread
* ULWOS2_THREAD_KILL - kill current thread (it won't run anymore)
* ULWOS2_START_SCHEDULER - start ULWOS2 scheduler (this function does not return)
* ULWOS2_THREAD_START - this is the entity that enables jumping to a specific point within the thread when resuming;
* ULWOS2_THREAD_YIELD - yield back to the scheduler. This is used for giving  a chance of other threads to run (it doesn't block the thread);
* ULWOS2_THREAD_SLEEP_MS - block the thread for a specific amount of milliseconds;
* ULWOS2_THREAD_SEND_SIGNAL - send a signal to other threads;
* ULWOS2_THREAD_WAIT_FOR_SIGNAL - suspend the thread until the given signal arrives;
* ULWOS2_THREAD_RESET - reinitialize thread internal pointer (but does not touch any local variable). This forces the thread to run from the beginning;
## Examples
Check the examples folder for some multi-platform examples, or try it online: [![Run on Repl.it](https://repl.it/badge/github/fabiopjve/ULWOS2)](https://repl.it/github/fabiopjve/ULWOS2)

## Changes
* v1.0.0 
  * Each thread now creates its own Thread Control Block as a static structure
  * TCB now uses pointers to create a linked list connecting all TCBs
  * New ULWOS2_SIGNALS define created to include signals field in TCB and enable signal-handling functions
  * Updated Linux example makefile to define ULWOS2_SIGNALS
  * ULWOS2_THREAD_KILL now forces thread to resume from start (in case it is recreated)
  * ULWOS2_THREAD_SLEEP_MS is now standardized to use 16-bit values (up to 65535 ms)
* v0.9.0
  * First release as an Arduino library
  * Modified ULWOS2 header to include special function prototypes when compiling on Arduino
  * Included first Arduino example (blinker)
* v0.8.0
  * Modified TCB structure to improve operation on MSP430
  * Included 2 examples for MSP430F2013
* v0.7.0
  * First version
  * Linux example
