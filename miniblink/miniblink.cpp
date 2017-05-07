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

#include <stdio.h>
#include <string.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/rtc.h>

#include "usart_irq.hpp"
#include "systick_setup.hpp"
#include "rtc_setup.hpp"

static void gpio_setup(void)
{
	// Enable GPIOC clock. 
	// Manually: 
	// RCC_APB2ENR |= RCC_APB2ENR_IOPCEN;
	// Using API functions: 
	rcc_periph_clock_enable(RCC_GPIOC);

	// Set GPIO12 (in GPIO port C) to 'output push-pull'. 
	// Manually: 
	// GPIOC_CRH = (GPIO_CNF_OUTPUT_PUSHPULL << (((13 - 8) * 4) + 2));
	// GPIOC_CRH |= (GPIO_MODE_OUTPUT_2_MHZ << ((13 - 8) * 4));
	// Using API functions: 
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
}


static void tim_setup(void)
{
	// Enable TIM2 clock. 
	rcc_periph_clock_enable(RCC_TIM2);

	// Enable TIM2 interrupt. 
	nvic_enable_irq(NVIC_TIM2_IRQ);

	// Reset TIM2 peripheral to defaults. 
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

	// Disable preload. 
	timer_disable_preload(TIM2);
	timer_continuous_mode(TIM2);

	// count full range, as we'll update compare value continuously 
	timer_set_period(TIM2, 65535);

	// Set the initual output compare value for OC1. 
	timer_set_oc_value(TIM2, TIM_OC1, 1000);

	// Counter enable. 
	timer_enable_counter(TIM2);

	// Enable Channel 1 compare interrupt to recalculate compare values 
	timer_enable_irq(TIM2, TIM_DIER_CC1IE);
}

extern "C" void tim2_isr(void)
{
	if (timer_get_flag(TIM2, TIM_SR_CC1IF)) 
	{
		// Clear compare interrupt flag. 
		timer_clear_flag(TIM2, TIM_SR_CC1IF);

		// Get current timer value to calculate next
		// compare register value.
		uint16_t compare_time = timer_get_counter(TIM2);

		// Calculate and set the next compare value. 
		uint16_t frequency = 1000;
		uint16_t new_time = compare_time + frequency;

		timer_set_oc_value(TIM2, TIM_OC1, new_time);
	}
}


class SerialClass
{
public:
	SerialClass(int usart) : _usart(usart) {}

	void begin()
	{
		rcc_periph_clken ckgpio, ckusart;
		uint32_t gpioport;
		uint16_t gpios_tx, gpios_rx;
		switch (_usart)
		{
		case USART1: ckgpio = RCC_GPIOA; ckusart = RCC_USART1; gpioport = GPIOA; gpios_tx = GPIO_USART1_TX; gpios_rx = GPIO_USART1_RX; break;
		case USART2: ckgpio = RCC_GPIOA; ckusart = RCC_USART2; gpioport = GPIOA; gpios_tx = GPIO_USART2_TX; gpios_rx = GPIO_USART2_RX; break;
		case USART3: ckgpio = RCC_GPIOB; ckusart = RCC_USART3; gpioport = GPIOB; gpios_tx = GPIO_USART3_TX; gpios_rx = GPIO_USART3_RX; break;
		default: return;
		}
		rcc_periph_clock_enable(ckgpio);
		rcc_periph_clock_enable(ckusart);
		gpio_set_mode(gpioport, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, gpios_tx);
		gpio_set_mode(gpioport, GPIO_MODE_INPUT,         GPIO_CNF_INPUT_FLOAT,           gpios_rx);

		// Setup UART parameters.
		usart_set_baudrate(_usart, 38400);
		usart_set_databits(_usart, 8);
		usart_set_stopbits(_usart, USART_STOPBITS_1);
		usart_set_mode(_usart, USART_MODE_TX);
		usart_set_parity(_usart, USART_PARITY_NONE);
		usart_set_flow_control(_usart, USART_FLOWCONTROL_NONE);

		usart_enable(_usart);
	}

	void write(const char *p, int sz) {
		const char *e = p + sz;
		while (p != e)
			usart_send_blocking(_usart, *p++);
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
	// hse => quarzo esterno
	// hsi => oscillatore RC interno
	// lse => 32768Khz external clock
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	gpio_setup();
	tim_setup();
	usart_setup();
	systick_setup();
	rtc_setup();

	//usart_setup();
	//SerialClass Serial1(USART1); Serial1.begin();
	//SerialClass Serial3(USART3); Serial3.begin();

	gpio_toggle(GPIOC, GPIO13);	// LED on/off 
	gpio_toggle(GPIOC, GPIO13);	// LED on/off 

	for (;;)
	{
		delay(1000);

		char b[16];
		sprintf(b, "%5d\n\r", int(rtc_counter));
		usart_write(b, strlen(b));

		int h,m,s;
		rtc_get_hms(h, m, s);
		sprintf(b, "%02d:%02d:%02d\n\r", h, m, s);
		usart_write(b, strlen(b));

		int D,M,Y;
		rtc_get_dmy(D, M, Y);
		sprintf(b, "%02d-%02d-%04d\n\r", D, M, Y);
		usart_write(b, strlen(b));
	}


	if (false)
	{
		// Blink the LED (PC13) on the board. 
		for (int n = 0; n < 1024*1024; ++n)
		{
			// Using API function gpio_toggle(): 
			//gpio_toggle(GPIOC, GPIO13);	// LED on/off 
			//for (auto i = 0; i < 500000; i++)	// Wait a bit. 
			//	__asm__("nop");

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
			delay(1000);
			gpio_toggle(GPIOC, GPIO13);	// LED on/off 

		}
	}

	return 0;
}
