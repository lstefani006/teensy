/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "usart_irq.hpp"
#include <stdio.h>
#include <string.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/usart.h>


static void gpio_setup(void)
{
	/* Enable GPIOC clock. */
	/* Manually: */
	// RCC_APB2ENR |= RCC_APB2ENR_IOPCEN;
	/* Using API functions: */
	rcc_periph_clock_enable(RCC_GPIOC);

	/* Set GPIO12 (in GPIO port C) to 'output push-pull'. */
	/* Manually: */
	// GPIOC_CRH = (GPIO_CNF_OUTPUT_PUSHPULL << (((13 - 8) * 4) + 2));
	// GPIOC_CRH |= (GPIO_MODE_OUTPUT_2_MHZ << ((13 - 8) * 4));
	/* Using API functions: */
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

	gpio_set(GPIOC, GPIO13);
}


static void tim_setup(void)
{
	/* Enable TIM2 clock. */
	rcc_periph_clock_enable(RCC_TIM2);

	/* Enable TIM2 interrupt. */
	nvic_enable_irq(NVIC_TIM2_IRQ);

	/* Reset TIM2 peripheral to defaults. */
	rcc_periph_reset_pulse(RST_TIM2);

	/* Timer global mode:
	 * - No divider
	 * - Alignment edge
	 * - Direction up
	 * (These are actually default values after reset above, so this call
	 * is strictly unnecessary, but demos the api for alternative settings)
	 **/
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

	/*
	 * Please take note that the clock source for STM32 timers
	 * might not be the raw APB1/APB2 clocks.  In various conditions they
	 * are doubled.  See the Reference Manual for full details!
	 * In our case, TIM2 on APB1 is running at double frequency, so this
	 * sets the prescaler to have the timer run at 5kHz
	 */
	timer_set_prescaler(TIM2, ((rcc_apb1_frequency * 2) / 5000));

	/* Disable preload. */
	timer_disable_preload(TIM2);
	timer_continuous_mode(TIM2);

	/* count full range, as we'll update compare value continuously */
	timer_set_period(TIM2, 65535);

	/* Set the initual output compare value for OC1. */
	timer_set_oc_value(TIM2, TIM_OC1, 1000);

	/* Counter enable. */
	timer_enable_counter(TIM2);

	/* Enable Channel 1 compare interrupt to recalculate compare values */
	timer_enable_irq(TIM2, TIM_DIER_CC1IE);
}
static void tim_setup_leo(void)
{
	/* Enable TIM2 clock. */
	rcc_periph_clock_enable(RCC_TIM2);

	/* Enable TIM2 interrupt. */
	nvic_enable_irq(NVIC_TIM2_IRQ);

	/* Reset TIM2 peripheral to defaults. */
	rcc_periph_reset_pulse(RST_TIM2);

	/* Timer global mode:
	 * - No divider
	 * - Alignment edge
	 * - Direction up
	 * (These are actually default values after reset above, so this call
	 * is strictly unnecessary, but demos the api for alternative settings)
	 **/
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

	/*
	 * Please take note that the clock source for STM32 timers
	 * might not be the raw APB1/APB2 clocks.  In various conditions they
	 * are doubled.  See the Reference Manual for full details!
	 * In our case, TIM2 on APB1 is running at double frequency, so this
	 * sets the prescaler to have the timer run at 5kHz
	 */
	// TIM_PSC(TIM2) = value;
	timer_set_prescaler(TIM2, ((rcc_apb1_frequency * 2) / 5000));


	/* Disable preload. */
	// TIM_CR1(TIM2) &= ~TIM_CR1_ARPE  ??? non si capisce
	timer_disable_preload(TIM2);

	// TIM_CR1(TIM2) &= ~TIM_CR1_OPM
	timer_continuous_mode(TIM2);

	/* count full range, as we'll update compare value continuously */
	//  TIM_ARR(tTIM2) = period;
	timer_set_period(TIM2, 65535);

	/* Set the initual output compare value for OC1. */
	// TIM_CCR2(TIM2) = value;
	timer_set_oc_value(TIM2, TIM_OC1, 1000);

	/* Counter enable. */
	// TIM_CR1(TIM2) |= TIM_CR1_CEN;
	timer_enable_counter(TIM2);

	/* Enable Channel 1 compare interrupt to recalculate compare values */
	// TIM_DIER(TIM2) |= irq;
	timer_enable_irq(TIM2, TIM_DIER_CC1IE);
}

extern "C" void tim2_isr(void)
{
	if (timer_get_flag(TIM2, TIM_SR_CC1IF)) {

		/* Clear compare interrupt flag. */
		timer_clear_flag(TIM2, TIM_SR_CC1IF);

		/*
		 * Get current timer value to calculate next
		 * compare register value.
		 */
		uint16_t compare_time = timer_get_counter(TIM2);

		/* Calculate and set the next compare value. */
		uint16_t frequency = 1000;
		uint16_t new_time = compare_time + frequency;

		timer_set_oc_value(TIM2, TIM_OC1, new_time);

		/* Toggle LED to indicate compare event. */
		gpio_toggle(GPIOC, GPIO13);	/* LED on/off */

		//putstr(G_usart, "Ciao\n\r");
	}
}


class SerialClass
{
public:
	SerialClass(int usart) : _usart(usart) {}

	void begin()
	{
		switch (_usart)
		{
		case USART1: rcc_periph_clock_enable(RCC_GPIOA); break;
		case USART2: rcc_periph_clock_enable(RCC_GPIOA); break;
		case USART3: rcc_periph_clock_enable(RCC_GPIOB); break;
		default: return;
		}

		switch (_usart)
		{
		case USART1: rcc_periph_clock_enable(RCC_USART1); break;
		case USART2: rcc_periph_clock_enable(RCC_USART2); break;
		case USART3: rcc_periph_clock_enable(RCC_USART3); break;
		default: return;
		}

		switch (_usart)
		{
		case USART1: gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX); break;
		case USART2: gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX); break;
		case USART3: gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART3_TX); break;
		default: return;
		}

		/* Setup UART parameters. */
		usart_set_baudrate(_usart, 38400);
		usart_set_databits(_usart, 8);
		usart_set_stopbits(_usart, USART_STOPBITS_1);
		usart_set_mode(_usart, USART_MODE_TX);
		usart_set_parity(_usart, USART_PARITY_NONE);
		usart_set_flow_control(_usart, USART_FLOWCONTROL_NONE);

		usart_enable(_usart);
	}

	void write(const char *p) {
		while (*p)
			usart_send_blocking(_usart, *p++);
	}

	void write(int n) {
		char b[10];
		sprintf(b, "%5d", n);
		write(b);
	}

private:
	int _usart;
};

int main()
{
	rcc_clock_setup_in_hse_8mhz_out_72mhz();
	gpio_setup();
	tim_setup();
	usart_setup();

	//usart_setup();
	//SerialClass Serial1(USART1); Serial1.begin();
	//SerialClass Serial3(USART3); Serial3.begin();
		 
	gpio_toggle(GPIOC, GPIO13);	/* LED on/off */
	gpio_toggle(GPIOC, GPIO13);	/* LED on/off */

	/* Blink the LED (PC13) on the board. */
	for (int n = 0; n < 1024*1024; ++n)
	{
		/* Using API function gpio_toggle(): */
		//gpio_toggle(GPIOC, GPIO13);	/* LED on/off */
		for (auto i = 0; i < 500000; i++)	/* Wait a bit. */
			__asm__("nop");

		//Serial1.write(n);
		//Serial3.write(n);

		//Serial1.write(" TX1\n\r");
		//Serial3.write(" TX3\n\r");

		if (true)
		{
			const char *pp = "12345678901234567890 ";
			usart_write(pp, strlen(pp));
			char b[10];
			sprintf(b, "%5d\n\r", n);
			usart_write(b, strlen(b));
		}
	}

	return 0;
}


///////////////////////////////////////////////////////////////////

/*
// Global state:
volatile uint32_t systick_millis = 0;  // Millisecond counter.


// Delay for the specified number of milliseconds.
// This is implemented by configuring the systick timer to increment a count
// every millisecond and then busy waiting in a loop.
static void delay(uint32_t milliseconds) {
	uint32_t target = systick_millis + milliseconds;
	while (target > systick_millis);
}

// Setup the systick timer to increment a count every millisecond.  This is
// useful for implementing a delay function based on wall clock time.
static void systick_setup(void) 
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
void sys_tick_handler() 
{
	// Increment the global millisecond count.
	systick_millis++;
}
*/


#if 0
static void nvic_setup(void)
{
	nvic_enable_irq(NVIC_TIM2_IRQ);
	nvic_set_priority(NVIC_TIM2_IRQ, 1);
}

void tim2_isr(void)
{
	gpio_toggle(GPIOC, GPIO13);	/* LED on/off */
	TIM_SR(TIM2) &= ~TIM_SR_UIF; /* Clear interrrupt flag. */
}

int main(void)
{
	rcc_clock_setup_in_hse_16mhz_out_72mhz();
	gpio_setup();
	nvic_setup();

	gpio_toggle(GPIOB, GPIO6);   /* LED2 on/off. */
	gpio_toggle(GPIOB, GPIO6);   /* LED2 on/off. */

	rcc_periph_clock_enable(RCC_TIM2);

	/*
	 * The goal is to let the LED2 glow for a second and then be
	 * off for a second.
	 */

	/* Set timer start value. */
	TIM_CNT(TIM2) = 1;

	/* Set timer prescaler. 72MHz/1440 => 50000 counts per second. */
	TIM_PSC(TIM2) = 1440;

	/* End timer value. If this is reached an interrupt is generated. */
	TIM_ARR(TIM2) = 12500;

	/* Update interrupt enable. */
	TIM_DIER(TIM2) |= TIM_DIER_UIE;

	/* Start timer. */
	TIM_CR1(TIM2) |= TIM_CR1_CEN;

	while (1); /* Halt. */

	return 0;
}
#endif
