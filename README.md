# ULWOS2
A super lightweight cooperative OS completely written in C which can target almost any platform.
## Basic API
*ULWOS2_THREAD_START - this is the constructor that enables jumping to a specific point within the thread when resuming;
*ULWOS2_THREAD_YIELD - yields back to the scheduler. This is used for giving  a chance of other threads to run (it doesn't block the thread);
*ULWOS2_THREAD_SLEEP_MS - blocks the thread for a specific amount of milliseconds;
*ULWOS2_THREAD_SEND_SIGNAL - sends a signal to other threads;
*ULWOS2_THREAD_WAIT_FOR_SIGNAL - suspends the thread until the given signal arrives;
*ULWOS2_THREAD_RESET - reinitialize thread internal pointer (but does not touch any local variable). This forces the thread to run from the beginning;
*ULWOS2_THREAD_KILL - to kill (destroy) a thread;
## Examples
Check the examples folder for some multi-platform examples, or try it online: [![Run on Repl.it](https://repl.it/badge/github/fabiopjve/ULWOS2)](https://repl.it/github/fabiopjve/ULWOS2)
