#include <stdio.h>
#include <string.h>
#include <Arduino.h>
#include <Timer.hpp>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>



Timer *S_Timer1 = nullptr;
Timer *S_Timer2 = nullptr;
Timer *S_Timer3 = nullptr;
Timer *S_Timer4 = nullptr;

Timer::~Timer()
{
}

void Timer::irq()
{
	// CC1IF: Capture/compare 1 interrupt flag
	if (timer_get_flag(_tm, TIM_SR_CC1IF)) 
	{
		timer_clear_flag(_tm, TIM_SR_CC1IF);
		this->CompareInterrupt(1);
	}
	if (timer_get_flag(_tm, TIM_SR_CC2IF)) 
	{
		timer_clear_flag(_tm, TIM_SR_CC2IF);
		this->CompareInterrupt(2);
	}
	if (timer_get_flag(_tm, TIM_SR_CC3IF)) 
	{
		timer_clear_flag(_tm, TIM_SR_CC3IF);
		this->CompareInterrupt(3);
	}
	if (timer_get_flag(_tm, TIM_SR_CC4IF)) 
	{
		timer_clear_flag(_tm, TIM_SR_CC4IF);
		this->CompareInterrupt(4);
	}

	// UIF: Update interrupt flag
	if (timer_get_flag(_tm, TIM_SR_UIF)) 
	{
		timer_clear_flag(_tm, TIM_SR_UIF);
		this->UpdateInterrupt();
	}
}

extern "C" void tim1_up_isr(void) { if (S_Timer1) S_Timer1->irq(); }
extern "C" void tim1_cc_isr(void) { if (S_Timer1) S_Timer1->irq(); }
extern "C" void tim2_isr(void)    { if (S_Timer2) S_Timer2->irq(); }
extern "C" void tim3_isr(void)    { if (S_Timer3) S_Timer3->irq(); }
extern "C" void tim4_isr(void)    { if (S_Timer4) S_Timer4->irq(); }

static void gpio_enable_clock(uint32_t port) 
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
void Timer::begin(uint32_t freq, uint32_t maxCount)
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

void Timer::setOutputCompareIrq(OutputCompareType oc, uint32_t value)
{
	tim_oc_id noc;
	switch (oc)
	{
	case OC1: noc = TIM_OC1; break;
	case OC2: noc = TIM_OC2; break;
	case OC3: noc = TIM_OC3; break;
	case OC4: noc = TIM_OC4; break;
	default: HALT;
	}

	timer_set_oc_value(_tm, noc, value);

	switch (oc)
	{
	case OC1: timer_enable_irq(_tm, TIM_DIER_CC1IE); break;
	case OC2: timer_enable_irq(_tm, TIM_DIER_CC2IE); break;
	case OC3: timer_enable_irq(_tm, TIM_DIER_CC3IE); break;
	case OC4: timer_enable_irq(_tm, TIM_DIER_CC4IE); break;
	default: HALT;
	}
}

void Timer::setOutputCompareDigIO(OutputCompareType oc, uint32_t value)
{
	tim_oc_id noc;
	switch (oc)
	{
	case OC1: noc = TIM_OC1; break;
	case OC2: noc = TIM_OC2; break;
	case OC3: noc = TIM_OC3; break;
	case OC4: noc = TIM_OC4; break;
	default: HALT;
	}
	timer_set_oc_value(_tm, noc, value);

	timer_disable_oc_output(_tm, noc);
	switch (_tm) 
	{
	case TIM1:
		switch (oc)
		{
		case OC1:
			gpio_enable_clock(GPIOA);
			gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO8);
			break;
		case OC2:
			gpio_enable_clock(GPIOA);
			gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO9);
			break;
		case OC3:
			gpio_enable_clock(GPIOA);
			gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO10);
			break;
		case OC4:
			gpio_enable_clock(GPIOA);
			gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO11);
			break;
		default: HALT;
		}
		break;

	case TIM2:
		switch (oc)
		{
		case OC1:
			gpio_enable_clock(GPIOA);
			gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO0);
			break;
		case OC2:
			gpio_enable_clock(GPIOA);
			gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO1);
			break;
		case OC3:
			gpio_enable_clock(GPIOA);
			gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO2);
			break;
		case OC4:
			gpio_enable_clock(GPIOA);
			gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO3);
			break;
		default: HALT;
		}
	}
	timer_set_oc_mode(_tm, noc, /*TIM_OCM_TOGGLE*/ TIM_OCM_PWM1);
	timer_enable_oc_output(_tm, noc);
}

uint32_t Timer::getCounter()
{
	return timer_get_counter(_tm);
}
