#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/spi.h>

#include <../miniblink/rtc_setup.cpp>
#include <../miniblink/spi_setup.cpp>
#include <../miniblink/systick_setup.cpp>
#include <../miniblink/usart_setup.cpp>
#include <../miniblink/ring.cpp>
#include <../util/uprintf.cpp>

#include <Arduino.h>

#include <../rfid/src/MFRC522.cpp>
#include <../rfid/src/MFRC522Debug.cpp>

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


/*
GPIO_CNF_INPUT_ANALOG            0x00 // Analog Input
GPIO_CNF_INPUT_FLOAT             0x01 // Digital Input Floating
GPIO_CNF_INPUT_PULL_UPDOWN       0x02 // Digital Input Pull Up and Down.
GPIO_CNF_OUTPUT_PUSHPULL         0x00 // Digital Output Pushpull. 
GPIO_CNF_OUTPUT_OPENDRAIN        0x01 // Digital Output Open Drain.
GPIO_CNF_OUTPUT_ALTFN_PUSHPULL   0x02 // Alternate Function Output Pushpull. 
GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN  0x03 // Alternate Function Output Open Drain.
*/
void pinMode(int pin, int mode) 
{
	rcc_periph_clken ro;
	uint32_t gpioport;
	switch (pin >> 16)
	{
	case 0: ro = RCC_GPIOA; gpioport = GPIOA; break;
	case 1: ro = RCC_GPIOB; gpioport = GPIOB; break;
	case 2: ro = RCC_GPIOC; gpioport = GPIOC; break;
	default: return;
	}
	rcc_periph_clock_enable(ro);
	uint16_t gpios = (uint16_t)(pin & 0xffffu);

	switch (mode)
	{
	case INPUT:        gpio_set_mode(gpioport, GPIO_MODE_INPUT,        GPIO_CNF_INPUT_FLOAT,     gpios); break;
	case OUTPUT:       gpio_set_mode(gpioport, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, gpios); break;
	case INPUT_PULLUP: gpio_set_mode(gpioport, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, gpios); break;
	}
}

void digitalWrite(int pin, int v) 
{
	uint32_t gpioport;
	switch (pin >> 16)
	{
	case 0: gpioport = GPIOA; break;
	case 1: gpioport = GPIOB; break;
	case 2: gpioport = GPIOC; break;
	default: return;
	}
	uint16_t gpios = (uint16_t)(pin & 0xffffu);

	if (v)
		gpio_set(gpioport, gpios);
	else
		gpio_clear(gpioport, gpios);
}

int digitalRead(int pin) 
{
	uint32_t gpioport;
	switch (pin >> 16)
	{
	case 0: gpioport = GPIOA; break;
	case 1: gpioport = GPIOB; break;
	case 2: gpioport = GPIOC; break;
	default: return -1;
	}
	uint16_t gpios = (uint16_t)(pin & 0xffffu);

	return gpio_get(gpioport, gpios) != 0 ? HIGH : LOW;
}

uint8_t rx[4];
uint8_t tx[4];
USARTIRQ Serial(USART1);

void setup();
void loop();

SPIClass SPI(SPI1);

int main()
{
	// hse => quarzo esterno
	// hsi => oscillatore RC interno
	// lse => 32768Khz external clock
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	gpio_setup();
	systick_setup();
	// rtc_setup();
	Serial.begin(rx, sizeof(rx), tx, sizeof(tx));

	gpio_set(GPIOC, GPIO13);
	gpio_toggle(GPIOC, GPIO13);	// LED on/off 
	delay(1000);
	gpio_toggle(GPIOC, GPIO13);	// LED on/off 
	delay(1000);
	gpio_set(GPIOC, GPIO13);

	setup();
	while(1)
		loop();
}
