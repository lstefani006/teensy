
#ifndef __Arduino_h__
#define __Arduino_h__

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

#include <usart_setup.hpp>
#include <systick_setup.hpp>
#include <uprintf.hpp>


extern USARTIRQ Serial;

#define LOW  0
#define HIGH 1

void digitalWrite(int pin, int v);
int digitalRead(int pin);
void pinMode(int pin, int mode);


enum Pins
{
	PA0, PA1, PA3, PA4, PA5, PA6, PA7, PA8, PA9, PA10, PA11, PA12, PA13, PA14, PA15,
	PB0, PB1, PB3, PB4, PB5, PB6, PB7, PB8, PB9, PB10, PB11, PB12, PB13, PB14, PB15,
	PC0, PC1, PC3, PC4, PC5, PC6, PC7, PC8, PC9, PC10, PC11, PC12, PC13, PC14, PC15,
};

#define OUTPUT 1
#define INPUT  2
#define INPUT_PULLUP 3

#endif
