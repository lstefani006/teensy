#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/spi.h>

#include <rtc_setup.hpp>
#include <spi_setup.hpp>
#include <systick_setup.hpp>
#include <usart_setup.hpp>
#include <ring.hpp>
#include <uprintf.hpp>

#include <Arduino.h>

#include <MFRC522.h>
#include <MFRC522Debug.h>

const char *halt_fn = nullptr;
int halt_ln = 0;
void halt(const char *fn, int ln) { 
	halt_fn = fn;
	halt_ln = ln;
	for(;;); 
}
//////////////////////////////////7

static uint16_t getGPIO(int pin)
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
	switch (pin >> 4)
	{
	case 0: ro = RCC_GPIOA; gpioport = GPIOA; break;
	case 1: ro = RCC_GPIOB; gpioport = GPIOB; break;
	case 2: ro = RCC_GPIOC; gpioport = GPIOC; break;
	default: return;
	}
	rcc_periph_clock_enable(ro);
	uint16_t gpios = getGPIO(pin);

	switch (mode)
	{
	case INPUT:        gpio_set_mode(gpioport, GPIO_MODE_INPUT,         GPIO_CNF_INPUT_FLOAT,       gpios); break;
	case OUTPUT:       gpio_set_mode(gpioport, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,   gpios); break;
	case INPUT_PULLUP: gpio_set_mode(gpioport, GPIO_MODE_INPUT,         GPIO_CNF_INPUT_PULL_UPDOWN, gpios); gpio_set(gpioport, gpios); break;
	}
}

void digitalWrite(int pin, int v) 
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

int digitalRead(int pin) 
{
	uint32_t gpioport;
	switch (pin >> 4)
	{
	case 0: gpioport = GPIOA; break;
	case 1: gpioport = GPIOB; break;
	case 2: gpioport = GPIOC; break;
	default: return -1;
	}
	uint16_t gpios = getGPIO(pin);

	return gpio_get(gpioport, gpios) != 0 ? HIGH : LOW;
}

uint8_t rx[16];
uint8_t tx[256];
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

	systick_setup();
	rtc_setup();
	Serial.begin(rx, sizeof(rx), tx, sizeof(tx));
	//Serial.begin();
	
	/*
	ST::digitalWrite(PC13, HIGH);
	ST::digitalWrite(PC13, LOW);

	if (ST::digitalRead(PC13))
	{
		pinMode(PC13, OUTPUT);
		digitalWrite(PC13, HIGH);
		for (int i = 0; i < 20; ++i)
		{
			if (digitalRead(PC13) == HIGH)
				digitalWrite(PC13, LOW);
			else
				digitalWrite(PC13, HIGH);
			delay(500);
		}
	}
	*/

	Serial.printf("ciao");

	setup();
	while(1)
		loop();
}
