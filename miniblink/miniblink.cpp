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
#include <libopencm3/stm32/spi.h>

#include "usart_setup.hpp"
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


static void timer_setup(void)
{
	// Enable TIM2 clock. 
	rcc_periph_clock_enable(RCC_TIM2);

	// Enable TIM2 interrupt. 
	nvic_enable_irq(NVIC_TIM2_IRQ);

	// Reset TIM2 peripheral to defaults. 
	rcc_periph_reset_pulse(RST_TIM2);

	// reset del timer
	timer_reset(TIM2);

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
	timer_set_prescaler(TIM2, ((rcc_apb1_frequency * 2) / 5000 - 1)); 

	// imposta il valore nell'auto--reload register
	// 1/2 secondo
	timer_set_period(TIM2, 5000);

	// Set the initual output compare value for OC1. 
	timer_set_oc_value(TIM2, TIM_OC1, 100);

	// Counter enable. 
	timer_enable_counter(TIM2);

	// sul update event il counter si carica dal suo auto-reload register
	timer_enable_preload(TIM2);

	// Enable Channel 1 compare interrupt to recalculate compare values 
	// Abilito IRQ sul compare e sul raggiungimento del period
	timer_enable_irq(TIM2, TIM_DIER_CC1IE | TIM_DIER_UIE);

	/* OLD
	// Disable preload. 
	timer_disable_preload(TIM2);
	timer_continuous_mode(TIM2);

	// count full range, as we'll update compare value continuously 
	// imposta il valore nell'auto--reload register
	timer_set_period(TIM2, 65535);

	// Set the initual output compare value for OC1. 
	timer_set_oc_value(TIM2, TIM_OC1, 1000);

	// Counter enable. 
	timer_enable_counter(TIM2);

	// Enable Channel 1 compare interrupt to recalculate compare values 
	timer_enable_irq(TIM2, TIM_DIER_CC1IE);
	*/
}

extern "C" void tim2_isr(void)
{
	if (timer_get_flag(TIM2, TIM_SR_CC1IF)) 
	{
		// Clear compare interrupt flag. 
		timer_clear_flag(TIM2, TIM_SR_CC1IF);

//		gpio_toggle(GPIOC, GPIO13);	// LED on/off 
	}
	if (timer_get_flag(TIM2, TIM_SR_UIF)) 
	{
		// Clear compare interrupt flag. 
		timer_clear_flag(TIM2, TIM_SR_UIF);

//		gpio_toggle(GPIOC, GPIO13);	// LED on/off 
	}
}

USARTIRQ Serial(USART1);


uint8_t rx[4];
uint8_t tx[4];


int main()
{
	// hse => quarzo esterno
	// hsi => oscillatore RC interno
	// lse => 32768Khz external clock
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	gpio_setup();
	timer_setup();
	systick_setup();
	Serial.begin(rx, sizeof(rx), tx, sizeof(tx));
	rtc_setup();

	int bb = 0;
	for (;;)
	{
		delay(1000);

		Serial << rtc_counter << "\n\r";

		char b[20];
		int h,m,s;
		rtc_get_hms(h, m, s);
		sprintf(b, "%02d:%02d:%02d\n\r", h, m, s);
		Serial << "leo1 - " << b;

		int D,M,Y;
		rtc_get_dmy(D, M, Y);
		sprintf(b, "%02d-%02d-%04d\n\r", D, M, Y);
		Serial << bb++ <<  " leo2 - " << b;

		/*
		{
			int ch = Serial.getch();
			if (ch >= 0)
				Serial << bb++ << " Leggo " << ch << "\n\r";
			else
				Serial << bb++ << " NON Leggo\n\r";
		}

		if (Serial.rxError())
		{
			Serial << "ERRORRE !!!!!\n\r";
			Serial.clearError();
		}
		*/
	}

	return 0;
}


