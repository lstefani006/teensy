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
	rcc_periph_clock_enable(RCC_GPIOB);

	// Set GPIO12 (in GPIO port C) to 'output push-pull'. 
	// Manually: 
	// GPIOC_CRH = (GPIO_CNF_OUTPUT_PUSHPULL << (((13 - 8) * 4) + 2));
	// GPIOC_CRH |= (GPIO_MODE_OUTPUT_2_MHZ << ((13 - 8) * 4));
	// Using API functions: 
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO12);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO14);
}


const char *halt_fn = nullptr;
int halt_ln = 0;
void halt(const char *fn, int ln) __attribute__((noreturn));
void halt(const char *fn, int ln) { 
	halt_fn = fn;
	halt_ln = ln;
	for(;;); 
}
#define HALT halt(__func__, __LINE__)

extern "C" void __cxa_pure_virtual() { 
	HALT;
}

class Timer
{
public:
	Timer(uint32_t tm) : _tm(tm) {}

	void begin(int freq, int maxCount);

	void irq();

protected:
	virtual void UpdateInterrupt() = 0;
	virtual void CompareInterrupt(int oc) = 0;

private:
	uint32_t _tm;
};

Timer *S_Timer[8+1] = { nullptr, };

void Timer::begin(int freq, int maxCount)
{
	switch (_tm)
	{
	case TIM1: 
	case TIM2: 
	case TIM3: 
	case TIM4: 
		break;
		/*
	case TIM5: 
	case TIM6: 
	case TIM7: 
	case TIM8: 
		break;
		*/

	default: HALT;
	}
	// Enable TIM2 clock. 
	switch (_tm)
	{
	case TIM1: rcc_periph_clock_enable(RCC_TIM1); nvic_enable_irq(NVIC_TIM1_UP_IRQ); nvic_enable_irq(NVIC_TIM1_CC_IRQ);   rcc_periph_reset_pulse(RST_TIM1); break;
	case TIM2: rcc_periph_clock_enable(RCC_TIM2); nvic_enable_irq(NVIC_TIM2_IRQ);    nvic_set_priority(NVIC_TIM2_IRQ, 1); rcc_periph_reset_pulse(RST_TIM2); break;
	case TIM3: rcc_periph_clock_enable(RCC_TIM3); nvic_enable_irq(NVIC_TIM3_IRQ);    nvic_set_priority(NVIC_TIM3_IRQ, 1); rcc_periph_reset_pulse(RST_TIM3); break;
	case TIM4: rcc_periph_clock_enable(RCC_TIM4); nvic_enable_irq(NVIC_TIM4_IRQ);    nvic_set_priority(NVIC_TIM4_IRQ, 1); rcc_periph_reset_pulse(RST_TIM4); break;

	case TIM5: rcc_periph_clock_enable(RCC_TIM5); nvic_enable_irq(NVIC_TIM5_IRQ); nvic_set_priority(NVIC_TIM5_IRQ, 1); rcc_periph_reset_pulse(RST_TIM5); break;
	case TIM6: rcc_periph_clock_enable(RCC_TIM6); nvic_enable_irq(NVIC_TIM6_IRQ); /*nvic_set_priority(NVIC_TIM6_IRQ, 1); */rcc_periph_reset_pulse(RST_TIM6); break;
	case TIM7: rcc_periph_clock_enable(RCC_TIM7); nvic_enable_irq(NVIC_TIM7_IRQ); nvic_set_priority(NVIC_TIM7_IRQ, 1); rcc_periph_reset_pulse(RST_TIM7); break;
	case TIM8: rcc_periph_clock_enable(RCC_TIM8); nvic_enable_irq(NVIC_TIM8_UP_IRQ); nvic_enable_irq(NVIC_TIM8_CC_IRQ); rcc_periph_reset_pulse(RST_TIM8); break;
	default: HALT;
	}

	// reset del timer
	timer_reset(_tm);

	/*---------------------------------------------------------------------------*/
	/** @brief Set the Timer Mode.

	  The modes are:

	  Clock divider ratio (to form the sampling clock for the input filters, and the dead-time clock in the advanced timers 1 and 8)
	  Edge/centre alignment
	  Count direction

	  The alignment and count direction are effective only for timers 1 to 5 and 8
	  while the clock divider ratio is effective for all timers except 6,7
	  The remaining timers are limited hardware timers which do not support these mode settings.

	  @note: When center alignment mode is selected, count direction is controlled by
	  hardware and cannot be written. The count direction setting has no effect
	  in this case.

	  @param[in] timer_peripheral Unsigned int32. Timer register address base @ref tim_reg_base (TIM1, TIM2 ... TIM5, TIM8)
	  @param[in] clock_div Unsigned int32. Clock Divider Ratio in bits 8,9: @ref tim_x_cr1_cdr
	  @param[in] alignment Unsigned int32. Alignment bits in 5,6: @ref tim_x_cr1_cms
	  @param[in] direction Unsigned int32. Count direction in bit 4,: @ref tim_x_cr1_dir
	  */
	timer_set_mode(_tm, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP); // imposta falgs in CR1

	switch (_tm)
	{
	case TIM1:
		timer_set_prescaler(_tm, (rcc_apb2_frequency * 1) / freq - 1);  // imposta il registro PSC
		break;
	case TIM2:
	case TIM3:
	case TIM4:
		timer_set_prescaler(_tm, (rcc_apb1_frequency * 2) / freq - 1);  // imposta il registro PSC
		break;

	case TIM5:
	case TIM6:
	case TIM7:
	case TIM8:
		timer_set_prescaler(_tm, (rcc_apb1_frequency * 2) / freq - 1);  // imposta il registro PSC
		break;
	default:
		HALT;
	}

	// imposta il valore nell'auto--reload register (ARR)
	// 1/2 secondo (forse 1 sec ???)
	// Quando il contatore raggiunge ARR e va a ZERO (se conta UP) o da Zero va a ARR (se conta down)
	// si genera un evento "update event ==> UE"
	// Che di solito provoca un UI ==> update interrupt
	timer_set_period(_tm, maxCount); // imposta il registro ARR

	switch (_tm)
	{
	case TIM1: S_Timer[1] = this; break;
	case TIM2: S_Timer[2] = this; break;
	case TIM3: S_Timer[3] = this; break;
	case TIM4: S_Timer[4] = this; break;

	case TIM5: S_Timer[5] = this; break;
	case TIM6: S_Timer[6] = this; break;
	case TIM7: S_Timer[7] = this; break;
	case TIM8: S_Timer[8] = this; break;
	default: HALT;
	}

	switch (_tm)
	{
	case TIM6:
	case TIM7:
		timer_enable_update_event(_tm);
		timer_enable_irq(_tm, TIM_DIER_UIE);
		timer_enable_counter(_tm);
		break;
	default:
		timer_set_oc_value(_tm, TIM_OC1, 1000); // CCR1/2/3 a seconda se TIM_OC1/2/3
		timer_set_oc_value(_tm, TIM_OC2, 2000); // CCR1/2/3 a seconda se TIM_OC1/2/3
		timer_set_oc_value(_tm, TIM_OC3, 3000); // CCR1/2/3 a seconda se TIM_OC1/2/3
		timer_set_oc_value(_tm, TIM_OC4, 4000); // CCR1/2/3 a seconda se TIM_OC1/2/3

		timer_enable_counter(_tm); // imposta un flag in CR1
		timer_enable_preload(_tm); // imposta un flag in CR1 
		timer_enable_irq(_tm, TIM_DIER_CC4IE | TIM_DIER_CC3IE | TIM_DIER_CC2IE | TIM_DIER_CC1IE | TIM_DIER_UIE); // Capture/compare 1 interrupt enable. --- Update Interrupt enable
		break;
	}
}
void Timer::irq()
{
	// CC1IF: Capture/compare 1 interrupt flag
	if (timer_get_flag(_tm, TIM_SR_CC1IF)) 
	{
		// Clear compare interrupt flag. 
		timer_clear_flag(_tm, TIM_SR_CC1IF);

		this->CompareInterrupt(1);
	}
	if (timer_get_flag(_tm, TIM_SR_CC2IF)) 
	{
		// Clear compare interrupt flag. 
		timer_clear_flag(_tm, TIM_SR_CC2IF);
		this->CompareInterrupt(2);
	}
	if (timer_get_flag(_tm, TIM_SR_CC3IF)) 
	{
		// Clear compare interrupt flag. 
		timer_clear_flag(_tm, TIM_SR_CC3IF);
		this->CompareInterrupt(3);
	}
	if (timer_get_flag(_tm, TIM_SR_CC4IF)) 
	{
		// Clear compare interrupt flag. 
		timer_clear_flag(_tm, TIM_SR_CC4IF);
		this->CompareInterrupt(4);
	}

	// UIF: Update interrupt flag
	if (timer_get_flag(_tm, TIM_SR_UIF)) 
	{
		// Clear compare interrupt flag. 
		timer_clear_flag(_tm, TIM_SR_UIF);

		this->UpdateInterrupt();
	}
}

extern "C" void tim1_up_isr(void) { if (S_Timer[1]) S_Timer[1]->irq(); }
extern "C" void tim1_cc_isr(void) { if (S_Timer[1]) S_Timer[1]->irq(); }
extern "C" void tim2_isr(void) { if (S_Timer[2]) S_Timer[2]->irq(); }
extern "C" void tim3_isr(void) { if (S_Timer[3]) S_Timer[3]->irq(); }
extern "C" void tim4_isr(void) { if (S_Timer[4]) S_Timer[4]->irq(); }

extern "C" void tim5_isr(void) { if (S_Timer[5]) S_Timer[5]->irq(); }
extern "C" void tim6_isr(void) { if (S_Timer[6]) S_Timer[6]->irq(); }
extern "C" void tim7_isr(void) { if (S_Timer[7]) S_Timer[7]->irq(); }
extern "C" void tim8_up_isr(void) { if (S_Timer[8]) S_Timer[8]->irq(); }
extern "C" void tim8_cc_isr(void) { if (S_Timer[8]) S_Timer[8]->irq(); }

#if 0
static void timer_setup(void)
{
	// Enable TIM2 clock. 
	rcc_periph_clock_enable(RCC_TIM2);

	// Enable TIM2 interrupt. 
	nvic_enable_irq(NVIC_TIM2_IRQ);
	nvic_set_priority(NVIC_TIM2_IRQ, 1);

	// Reset TIM2 peripheral to defaults. 
	rcc_periph_reset_pulse(RST_TIM2);

	// reset del timer
	timer_reset(TIM2);

	/* Timer global mode:
	 * - No divider ==> 3 valori possibili
	 * TIM_CR1_CKD_CK_INT	
	 * TIM_CR1_CKD_CK_INT_MUL_2
	 * TIM_CR1_CKD_CK_INT_MUL_4
	 * - Alignment edge
	 * - Direction up
	 * (These are actually default values after reset above, so this call
	 * is strictly unnecessary, but demos the api for alternative settings)
	 **/
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP); // imposta falgs in CR1

	/*
	 * Please take note that the clock source for STM32 timers
	 * might not be the raw APB1/APB2 clocks.  In various conditions they
	 * are doubled.  See the Reference Manual for full details!
	 * In our case, TIM2 on APB1 is running at double frequency, so this
	 * sets the prescaler to have the timer run at 5kHz
	 */
	// imposta il prescaler per avere fout = fin / (fin / 5000) 
	// ossia fin/fin*5000 ==> 5Khz
	timer_set_prescaler(TIM2, (rcc_apb1_frequency * 2) / 5000 - 1);  // imposta il registro PSC

	// imposta il valore nell'auto--reload register (ARR)
	// 1/2 secondo (forse 1 sec ???)
	// Quando il contatore raggiunge ARR e va a ZERO (se conta UP) o da Zero va a ARR (se conta down)
	// si genera un evento "update event ==> UE"
	// Che di solito provoca un UI ==> update interrupt
	timer_set_period(TIM2, 5000); // imposta il registro ARR

	// Set the initial output compare value for OC1.
	timer_set_oc_value(TIM2, TIM_OC1, 1000); // CCR1/2/3 a seconda se TIM_OC1/2/3

	// Counter enable. 
	timer_enable_counter(TIM2); // imposta un flag in CR1

	// During counter operation this causes the counter to be loaded from its
	// auto-reload register only at the next update event.
	timer_enable_preload(TIM2); // imposta un flag in CR1 

	// Abilita IRQ sui due eventi
	timer_enable_irq(TIM2, TIM_DIER_CC1IE | TIM_DIER_UIE); // Capture/compare 1 interrupt enable. --- Update Interrupt enable
}

volatile bool start = false;

extern "C" void tim2_isr(void)
{
	// CC1IF: Capture/compare 1 interrupt flag
	if (timer_get_flag(TIM2, TIM_SR_CC1IF)) 
	{
		// Clear compare interrupt flag. 
		timer_clear_flag(TIM2, TIM_SR_CC1IF);
		if (start) gpio_toggle(GPIOB, GPIO12);	// LED on/off 
	}

	// UIF: Update interrupt flag
	if (timer_get_flag(TIM2, TIM_SR_UIF)) 
	{
		// Clear compare interrupt flag. 
		timer_clear_flag(TIM2, TIM_SR_UIF);
		if (start) gpio_toggle(GPIOC, GPIO13);	// LED on/off 
	}
}
#endif

USARTIRQ Serial(USART1);

uint8_t rx[16];
uint8_t tx[16];

volatile bool start = false;

class Timer2 : public Timer
{
public:
	Timer2(uint32_t freq) : Timer(freq) {}
protected:
	void UpdateInterrupt()
	{
		//if (start) gpio_toggle(GPIOC, GPIO13);	// LED on/off 
		gpio_set(GPIOB, GPIO12);
		gpio_set(GPIOB, GPIO13);
		gpio_set(GPIOB, GPIO14);
		gpio_set(GPIOC, GPIO13);
	}
	void CompareInterrupt(int oc)
	{
		switch (oc)
		{
		case 1: gpio_clear(GPIOB, GPIO12); break;
		case 2: gpio_clear(GPIOB, GPIO13); break;
		case 3: gpio_clear(GPIOB, GPIO14); break;
		case 4: gpio_clear(GPIOC, GPIO13); break;
		}
	}
};

Timer2 tm2(TIM4);

void cb()
{
	gpio_toggle(GPIOB, GPIO12);	// LED on/off 
	static int nn = 0;
	Serial.println(nn++);
	Serial.print("+");
	Serial.println(rtc_counter);
}

int main()
{
	// hse => quarzo esterno
	// hsi => oscillatore RC interno
	// lse => 32768Khz external clock
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	gpio_setup();
	tm2.begin(5000, 5000);

	systick_setup();
	Serial.begin(rx, sizeof(rx), tx, sizeof(tx));
	rtc_setup();

	if (false)
	{
		for (int i = 0; i < 10; ++i)
		{
			delay(200);
			gpio_toggle(GPIOC, GPIO13);	// LED on/off 
		}
		for (int i = 0; i < 10; ++i)
		{
			delay(200);
			gpio_toggle(GPIOB, GPIO12);	// LED on/off 
		}
	}

	//rtc_cb = cb;

	start = true;
	for (;;);

	int bb = 0;
	for (;;)
	{
		delay(1000);
		gpio_toggle(GPIOC, GPIO13);	// LED on/off 

		Serial.print(bb++).print(" ").println(rtc_counter);

		int h,m,s;
		rtc_get_hms(h, m, s);
		Serial.printf("leo - %02d:%02d:%02d", h, m, s).println();

		/*
		   int D,M,Y;
		   rtc_get_dmy(D, M, Y);
		   sprintf(b, "%02d-%02d-%04d\n\r", D, M, Y);
		   Serial << bb++ <<  " leo2 - " << b;
		   */

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


