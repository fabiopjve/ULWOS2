
# ULWOS2 - Change history

This is the change history of ULWOS2.

* v1.0.4
  * Included a simple unit test (Linux-only)
  * Included RGB breathing LED for HiFive 1 (using Arduino)
* v1.0.3
  * Improvements to 7-segment counter Arduino example
* v1.0.2
  * Added new Arduino examples (breathing LED and 7-segment counter)
* v1.0.1
  * Fixed issues with C++ on Arduino
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
