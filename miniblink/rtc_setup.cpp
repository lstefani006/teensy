#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rtc.h>

#include "rtc_setup.hpp"

void rtc_setup()
{
	rtc_auto_awake(RCC_LSE, 0x7fff);

	// Without this the RTC interrupt routine will never be called.
	nvic_enable_irq(NVIC_RTC_IRQ);
	nvic_set_priority(NVIC_RTC_IRQ, 1);

	// Enable the RTC interrupt to occur off the SEC flag.
	rtc_interrupt_enable(RTC_SEC);
}

volatile uint32_t rtc_counter = 0;
extern "C" void rtc_isr(void)
{
	// The interrupt flag isn't cleared by hardware, we have to do it.
	rtc_clear_flag(RTC_SEC);

	// get value
	rtc_counter = rtc_get_counter_val();

	// rtc_set_counter_val(2);
}
