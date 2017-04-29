
extern volatile uint32_t systick_millis;  // Millisecond counter.

// Delay for the specified number of milliseconds.
// This is implemented by configuring the systick timer to increment a count
// every millisecond and then busy waiting in a loop.
void delay(uint32_t milliseconds);

// Setup the systick timer to increment a count every millisecond.  This is
// useful for implementing a delay function based on wall clock time.
void systick_setup(void);
