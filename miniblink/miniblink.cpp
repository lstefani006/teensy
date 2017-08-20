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
#include <libopencm3/stm32/dac.h>

#include <Arduino.h>

#include "usart_setup.hpp"
#include "systick_setup.hpp"
#include "rtc_setup.hpp"
#include "Timer.hpp"

static void gpio_setup(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);

	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO12);

	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO0);
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO1);
}
static void dac_setup(void)
{
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO5);
	dac_disable(CHANNEL_2);
	dac_disable_waveform_generation(CHANNEL_2);
	dac_enable(CHANNEL_2);
	dac_set_trigger_source(DAC_CR_TSEL2_SW);
}

/*
		uint16_t target = input_adc0 / 2;
		dac_load_data_buffer_single(target, RIGHT12, CHANNEL_2);
		dac_software_trigger(CHANNEL_2);
		*/


///////////////////////////////////////////////////////////////////////

class Timer2 : public Timer
{
public:
	Timer2() : Timer(TIM2) {}
protected:
	void UpdateInterrupt()
	{
//			gpio_set(GPIOA, GPIO0);
//			gpio_set(GPIOA, GPIO1);
			gpio_set(GPIOB, GPIO12);
			gpio_set(GPIOC, GPIO13);
	}
	void CompareInterrupt(int oc)
	{
		switch (oc)
		{
//		case 1: gpio_clear(GPIOA, GPIO0); break;
//		case 2: gpio_clear(GPIOA, GPIO1); break;
		case 3: gpio_clear(GPIOB, GPIO12); break;
		case 4: gpio_clear(GPIOC, GPIO13); break;
		}
	}
};

Timer2 tm2;

void setup()
{
	gpio_setup();

	tm2.begin(5000, 5000);
	tm2.setUpdateIrq();
	tm2.setOutputCompareDigIO(Timer::OC1, 1000);
	tm2.setOutputCompareDigIO(Timer::OC2, 2000);
	tm2.setOutputCompareIrq(Timer::OC3, 3000);
	tm2.setOutputCompareIrq(Timer::OC4, 5000);
}

void loop() 
{
	delay(1013);
	Serial.println(tm2.getCounter());
}
