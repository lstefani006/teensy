#ifndef __Arduino_h__
#define __Arduino_h__

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/spi.h>

typedef uint8_t byte;
#define PROGMEM
#define __FlashStringHelper char

struct SPISettings
{
	SPISettings(int , int , int ) {}
};

#define SPI_CLOCK_DIV4 0
#define MSBFIRST 0
#define SPI_MODE0 0

#define F(a) (a)

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <usart_setup.hpp>
#include <systick_setup.hpp>
#include <uprintf.hpp>

#define pgm_read_byte(s) (*(s))


extern USARTIRQ Serial;

//////////////////////////////////////////
enum Pins
{
	PA0=00, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PA8, PA9, PA10, PA11, PA12, PA13, PA14, PA15,
	PB0=16, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PB8, PB9, PB10, PB11, PB12, PB13, PB14, PB15,
	PC0=32, PC1, PC2, PC3, PC4, PC5, PC6, PC7, PC8, PC9, PC10, PC11, PC12, PC13, PC14, PC15,
};

#define LOW  0
#define HIGH 1

void digitalWrite(int pin, int v);
int digitalRead(int pin);

#define OUTPUT 1
#define INPUT  2
#define INPUT_PULLUP 3
void pinMode(int pin, int mode);
//////////////////////////////////////////

namespace ST
{
	inline uint16_t getGPIO(int pin)
	{
		switch (pin)
		{
		case PA0:  return GPIO0;
		case PA1:  return GPIO1;
		case PA2:  return GPIO2;
		case PA3:  return GPIO3;
		case PA4:  return GPIO4;
		case PA5:  return GPIO5;
		case PA6:  return GPIO6;
		case PA7:  return GPIO7;
		case PA8:  return GPIO8;
		case PA9:  return GPIO9;
		case PA10: return GPIO10;
		case PA11: return GPIO11;
		case PA12: return GPIO12;
		case PA13: return GPIO13;
		case PA14: return GPIO14;
		case PA15: return GPIO15;

		case PB0:  return GPIO0;
		case PB1:  return GPIO1;
		case PB2:  return GPIO2;
		case PB3:  return GPIO3;
		case PB4:  return GPIO4;
		case PB5:  return GPIO5;
		case PB6:  return GPIO6;
		case PB7:  return GPIO7;
		case PB8:  return GPIO8;
		case PB9:  return GPIO9;
		case PB10: return GPIO10;
		case PB11: return GPIO11;
		case PB12: return GPIO12;
		case PB13: return GPIO13;
		case PB14: return GPIO14;
		case PB15: return GPIO15;

		case PC0:  return GPIO0;
		case PC1:  return GPIO1;
		case PC2:  return GPIO2;
		case PC3:  return GPIO3;
		case PC4:  return GPIO4;
		case PC5:  return GPIO5;
		case PC6:  return GPIO6;
		case PC7:  return GPIO7;
		case PC8:  return GPIO8;
		case PC9:  return GPIO9;
		case PC10: return GPIO10;
		case PC11: return GPIO11;
		case PC12: return GPIO12;
		case PC13: return GPIO13;
		case PC14: return GPIO14;
		case PC15: return GPIO15;

		default: return 0;
		}
	}

	inline void gpio_set(uint32_t gpioport, uint16_t gpios) { GPIO_BSRR(gpioport) = gpios; }
	inline void gpio_clear(uint32_t gpioport, uint16_t gpios) { GPIO_BSRR(gpioport) = (gpios << 16); }
	inline uint16_t gpio_get(uint32_t gpioport, uint16_t gpios) { return gpio_port_read(gpioport) & gpios; }


	inline void digitalWrite(int pin, int v)
	{
		uint32_t gpioport;
		switch (pin >> 4)
		{
		case 0: gpioport = GPIOA; break;
		case 1: gpioport = GPIOB; break;
		case 2: gpioport = GPIOC; break;
		default: return;
		}
		uint16_t gpios = getGPIO(pin);

		if (v)
			gpio_set(gpioport, gpios);
		else
			gpio_clear(gpioport, gpios);
	}

	inline bool digitalRead(int pin)
	{
		uint32_t gpioport;
		switch (pin >> 4)
		{
		case 0: gpioport = GPIOA; break;
		case 1: gpioport = GPIOB; break;
		case 2: gpioport = GPIOC; break;
		return false;
		}
		uint16_t gpios = getGPIO(pin);

		return gpio_get(gpioport, gpios) != 0;
	}


}


#endif
