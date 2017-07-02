#ifndef __systick_h__
#define __systick_h__

/*
 * Arma il systime per farsi chiamare 1000 volte al secondo
 * Aumenta la variabile systick_millis ogni 1ms
 *
 */

#include <stdint.h>

typedef uint64_t systick_t; 

extern volatile systick_t systick_millis;  // Millisecond counter.

// Delay for the specified number of milliseconds.
// This is implemented by configuring the systick timer to increment a count
// every millisecond and then busy waiting in a loop.
void delay(uint32_t milliseconds);

// Setup the systick timer to increment a count every millisecond.  This is
// useful for implementing a delay function based on wall clock time.
void systick_setup(void);

#endif
