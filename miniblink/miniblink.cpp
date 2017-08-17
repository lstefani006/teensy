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
#include "Timer.hpp"

static void gpio_setup(void)
{
	// Enable GPIOC clock. 
	// Manually: 
	// RCC_APB2ENR |= RCC_APB2ENR_IOPCEN;
	// Using API functions: 
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);

	// Set GPIO12 (in GPIO port C) to 'output push-pull'. 
	// Manually: 
	// GPIOC_CRH = (GPIO_CNF_OUTPUT_PUSHPULL << (((13 - 8) * 4) + 2));
	// GPIOC_CRH |= (GPIO_MODE_OUTPUT_2_MHZ << ((13 - 8) * 4));
	// Using API functions: 
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO12);

	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO0);
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO1);
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

#if 0
class Timer
{
public:
	Timer(uint32_t tm) : _tm(tm) {}

	void begin(int freq, int maxCount);
	void setUpdateIrq(bool enable = true);
	void setOutputCompareIrq(tim_oc_id oc, uint32_t value);
	void setOutputCompareDigIO(tim_oc_id oc, uint32_t value);

	void irq();

protected:
	virtual void UpdateInterrupt() = 0;
	virtual void CompareInterrupt(int oc) = 0;

private:
	uint32_t _tm;
};

Timer *S_Timer1 = nullptr;
Timer *S_Timer2 = nullptr;
Timer *S_Timer3 = nullptr;
Timer *S_Timer4 = nullptr;

void gpio_enable_clock(uint32_t port) 
{
	switch (port) 
	{
	case GPIOA: rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPAEN); break;
	case GPIOB: rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPBEN); break;
	case GPIOC: rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPCEN); break;
	case GPIOD: rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPDEN); break;
	default: HALT;
	};
}
void Timer::begin(int freq, int maxCount)
{
	switch (_tm)
	{
	case TIM1: 
	case TIM2: 
	case TIM3: 
	case TIM4: 
		break;
	default: HALT;
	}

	// Enable TIM2 clock. 
	switch (_tm)
	{
	case TIM1: rcc_periph_clock_enable(RCC_TIM1); 
			   nvic_enable_irq(NVIC_TIM1_UP_IRQ); 
			   nvic_enable_irq(NVIC_TIM1_CC_IRQ);   
			   rcc_periph_reset_pulse(RST_TIM1); 
			   break;
	case TIM2: rcc_periph_clock_enable(RCC_TIM2); nvic_enable_irq(NVIC_TIM2_IRQ); nvic_set_priority(NVIC_TIM2_IRQ, 1); rcc_periph_reset_pulse(RST_TIM2); break;
	case TIM3: rcc_periph_clock_enable(RCC_TIM3); nvic_enable_irq(NVIC_TIM3_IRQ); nvic_set_priority(NVIC_TIM3_IRQ, 1); rcc_periph_reset_pulse(RST_TIM3); break;
	case TIM4: rcc_periph_clock_enable(RCC_TIM4); nvic_enable_irq(NVIC_TIM4_IRQ); nvic_set_priority(NVIC_TIM4_IRQ, 1); rcc_periph_reset_pulse(RST_TIM4); break;
	default: HALT;
	}

	// reset del timer
	timer_reset(_tm);

	// set del fattore di divisione, dell'edge, del UP/DOWN
	timer_set_mode(_tm, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP); // imposta falgs in CR1

	// prescaler
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
	default:
		HALT;
	}

	// imposta il valore nell'auto--reload register (ARR)
	// 1/2 secondo (forse 1 sec ???)
	// Quando il contatore raggiunge ARR e va a ZERO (se conta UP) o da Zero va a ARR (se conta down)
	// si genera un evento "update event ==> UE"
	// Che di solito provoca un UI ==> update interrupt
	timer_set_period(_tm, maxCount); // imposta il registro ARR

	// variabili per eventuale IRQ
	switch (_tm)
	{
	case TIM1: S_Timer1 = this; break;
	case TIM2: S_Timer2 = this; break;
	case TIM3: S_Timer3 = this; break;
	case TIM4: S_Timer4 = this; break;
	default: HALT;
	}

	switch (_tm)
	{
	case TIM1:
	case TIM2:
	case TIM3:
	case TIM4:
		timer_enable_counter(_tm); // imposta un flag in CR1
		timer_enable_preload(_tm); // imposta un flag in CR1 
		break;
	default: HALT;
	}
}
void Timer::setUpdateIrq(bool enable)
{
	if (enable)
		timer_enable_irq(_tm, TIM_DIER_UIE);
	else
		timer_disable_irq(_tm, TIM_DIER_UIE);
}

void Timer::setOutputCompareIrq(tim_oc_id oc, uint32_t value)
{
	switch (_tm)
	{
	case TIM1:
	case TIM2:
	case TIM3:
	case TIM4:
		timer_set_oc_value(_tm, oc, value);
		break;
	default: HALT;
	}
	switch (oc)
	{
	case TIM_OC1: timer_enable_irq(_tm, TIM_DIER_CC1IE); break;
	case TIM_OC2: timer_enable_irq(_tm, TIM_DIER_CC2IE); break;
	case TIM_OC3: timer_enable_irq(_tm, TIM_DIER_CC3IE); break;
	case TIM_OC4: timer_enable_irq(_tm, TIM_DIER_CC4IE); break;
	default: HALT;
	}
}

void Timer::setOutputCompareDigIO(tim_oc_id oc, uint32_t value)
{
	switch (_tm)
	{
	case TIM1:
	case TIM2:
	case TIM3:
	case TIM4:
		timer_set_oc_value(_tm, oc, value);
		break;

	default: HALT;
	}
	switch (_tm) 
	{
	case TIM1:
		switch (TIM_OC1)
		{
		case TIM_OC1:
			gpio_enable_clock(GPIOA);
			gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO8);

			timer_disable_oc_output(_tm, TIM_OC1);
			timer_set_oc_mode(_tm, TIM_OC1, /*TIM_OCM_TOGGLE*/ TIM_OCM_PWM1);
			timer_enable_oc_output(_tm, TIM_OC1);
			break;

		case TIM_OC2:
			gpio_enable_clock(GPIOA);
			gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO9);

			timer_disable_oc_output(_tm, TIM_OC2);
			timer_set_oc_mode(_tm, TIM_OC2, /*TIM_OCM_TOGGLE*/ TIM_OCM_PWM1);
			timer_enable_oc_output(_tm, TIM_OC2);
			break;

		case TIM_OC3:
			gpio_enable_clock(GPIOA);
			gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO10);

			timer_disable_oc_output(_tm, TIM_OC3);
			timer_set_oc_mode(_tm, TIM_OC3, /*TIM_OCM_TOGGLE*/ TIM_OCM_PWM1);
			timer_enable_oc_output(_tm, TIM_OC3);
			break;

		case TIM_OC4:
			gpio_enable_clock(GPIOA);
			gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO11);

			timer_disable_oc_output(_tm, TIM_OC4);
			timer_set_oc_mode(_tm, TIM_OC4, /*TIM_OCM_TOGGLE*/ TIM_OCM_PWM1);
			timer_enable_oc_output(_tm, TIM_OC4);
			break;
		}
		break;

	case TIM2:
		switch (oc)
		{
		case TIM_OC1:
			gpio_enable_clock(GPIOA);
			gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO0);

			timer_disable_oc_output(_tm, TIM_OC1);
			timer_set_oc_mode(_tm, TIM_OC1, /*TIM_OCM_TOGGLE*/ TIM_OCM_PWM1);
			timer_enable_oc_output(_tm, TIM_OC1);
			break;
		case TIM_OC2:
			gpio_enable_clock(GPIOA);
			gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO1);

			timer_disable_oc_output(_tm, TIM_OC2);
			timer_set_oc_mode(_tm, TIM_OC2, /*TIM_OCM_TOGGLE*/ TIM_OCM_PWM1);
			timer_enable_oc_output(_tm, TIM_OC2);
			break;
		case TIM_OC3:
			gpio_enable_clock(GPIOA);
			gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO2);

			timer_disable_oc_output(_tm, TIM_OC3);
			timer_set_oc_mode(_tm, TIM_OC3, /*TIM_OCM_TOGGLE*/ TIM_OCM_PWM1);
			timer_enable_oc_output(_tm, TIM_OC3);
			break;
		case TIM_OC4:
			gpio_enable_clock(GPIOA);
			gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO3);

			timer_disable_oc_output(_tm, TIM_OC4);
			timer_set_oc_mode(_tm, TIM_OC4, /*TIM_OCM_TOGGLE*/ TIM_OCM_PWM1);
			timer_enable_oc_output(_tm, TIM_OC4);
			break;
		}
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

extern "C" void tim1_up_isr(void) { if (S_Timer1) S_Timer1->irq(); }
extern "C" void tim1_cc_isr(void) { if (S_Timer1) S_Timer1->irq(); }
extern "C" void tim2_isr(void) { if (S_Timer2) S_Timer2->irq(); }
extern "C" void tim3_isr(void) { if (S_Timer3) S_Timer3->irq(); }
extern "C" void tim4_isr(void) { if (S_Timer4) S_Timer4->irq(); }

#endif

///////////////////////////////////////////////////////////////////////

USARTIRQ Serial(USART1);

uint8_t rx[16];
uint8_t tx[16];

class Timer2 : public Timer
{
	bool _b;
public:
	Timer2(uint32_t freq) : Timer(freq), _b(true) {}
protected:
	void UpdateInterrupt()
	{
		if (_b)
		{
//			gpio_set(GPIOA, GPIO0);
//			gpio_set(GPIOA, GPIO1);
			gpio_set(GPIOB, GPIO12);
			gpio_set(GPIOC, GPIO13);
		}
	}
	void CompareInterrupt(int oc)
	{
		if (_b) 
		{
			switch (oc)
			{
//			case 1: gpio_clear(GPIOA, GPIO0); break;
//			case 2: gpio_clear(GPIOA, GPIO1); break;
			case 3: gpio_clear(GPIOB, GPIO12); break;
			case 4: gpio_clear(GPIOC, GPIO13); break;
			}
		}
	}
};

Timer2 tm2(TIM2);

static void cb()
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
	tm2.setUpdateIrq();
	tm2.setOutputCompareDigIO(Timer::OC1, 1000);
	tm2.setOutputCompareDigIO(Timer::OC2, 2000);
	tm2.setOutputCompareIrq(Timer::OC3, 3000);
	tm2.setOutputCompareIrq(Timer::OC4, 5000);

	systick_setup();
	Serial.begin(rx, sizeof(rx), tx, sizeof(tx));
	rtc_setup();

	for (;;);

	return 0;
}
