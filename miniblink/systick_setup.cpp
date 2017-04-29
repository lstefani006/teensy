#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/usart.h>

#include "systick_setup.hpp"

///////////////////////////////////////////////////////////////////

// Global state:
volatile uint32_t systick_millis = 0;  // Millisecond counter.


// Delay for the specified number of milliseconds.
// This is implemented by configuring the systick timer to increment a count
// every millisecond and then busy waiting in a loop.
void delay(uint32_t milliseconds) 
{
	uint32_t target = systick_millis + milliseconds;
	while (target > systick_millis);
}

// Setup the systick timer to increment a count every millisecond.  This is
// useful for implementing a delay function based on wall clock time.
void systick_setup(void) 
{
	// By default the Dash CPU will use an internal 16mhz oscillator for the CPU
	// clock speed.  To make the systick timer reset every millisecond (or 1000
	// times a second) set its reload value to:
	//   CPU_CLOCK_HZ / 1000
	systick_set_reload(16000);
	// Set the systick clock source to the main CPU clock and enable it and its
	// reload interrupt.
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	systick_counter_enable();
	systick_interrupt_enable();
}

// Systick timer reload interrupt handler.  Called every time the systick timer
// reaches its reload value.
extern "C" void sys_tick_handler() 
{
	// Increment the global millisecond count.
	systick_millis++;
}
