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

struct gp
{
	uint32_t gpioport;
	uint16_t gpios;
};
gp GP[] = {
	GPIOA, GPIO0,
	GPIOA, GPIO1,
	GPIOA, GPIO2,
	GPIOA, GPIO3,
	GPIOA, GPIO4,
	GPIOA, GPIO5,
	GPIOA, GPIO6,
	GPIOA, GPIO7,
	GPIOA, GPIO8,
	GPIOA, GPIO9,
	GPIOA, GPIO10,
	GPIOA, GPIO11,
	GPIOA, GPIO12,
	GPIOA, GPIO13,
	GPIOA, GPIO14,
	GPIOA, GPIO15,

	GPIOB, GPIO0,
	GPIOB, GPIO1,
	GPIOB, GPIO2,
	GPIOB, GPIO3,
	GPIOB, GPIO4,
	GPIOB, GPIO5,
	GPIOB, GPIO6,
	GPIOA, GPIO7,
	GPIOB, GPIO8,
	GPIOB, GPIO9,
	GPIOB, GPIO10,
	GPIOB, GPIO11,
	GPIOB, GPIO12,
	GPIOB, GPIO13,
	GPIOB, GPIO14,
	GPIOB, GPIO15,

	GPIOC, GPIO0,
	GPIOC, GPIO1,
	GPIOC, GPIO2,
	GPIOC, GPIO3,
	GPIOC, GPIO4,
	GPIOC, GPIO5,
	GPIOC, GPIO6,
	GPIOC, GPIO7,
	GPIOC, GPIO8,
	GPIOC, GPIO9,
	GPIOC, GPIO10,
	GPIOC, GPIO11,
	GPIOC, GPIO12,
	GPIOC, GPIO13,
	GPIOC, GPIO14,
	GPIOC, GPIO15,
};


void pinMode(int pin, int mode) 
{
	switch (mode)
	{
	case INPUT:
		gpio_set_mode( GP[pin].gpioport, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GP[pin].gpios);  
		break;
	case OUTPUT:
		gpio_set_mode( GP[pin].gpioport, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GP[pin].gpios);
		break;
	case INPUT_PULLUP:
		gpio_set_mode( GP[pin].gpioport, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GP[pin].gpios);
		break;
	}
}

void digitalWrite(int pin, int v) 
{
	if (v)
		gpio_set(GP[pin].gpioport, GP[pin].gpios);
	else
		gpio_clear(GP[pin].gpioport, GP[pin].gpios);
}

int digitalRead(int pin) 
{
	return gpio_get(GP[pin].gpioport, GP[pin].gpios);
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
	rtc_setup();
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
