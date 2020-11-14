#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"
#include "nvs_flash.h"
#include "nvs.h"
}

#define OUTPUT 1
#define INPUT 2
#define HIGH 1
#define LOW 0

inline void pinMode(int pin, int mode)
{
	gpio_num_t p = (gpio_num_t)pin;
	gpio_pad_select_gpio(p);
	if (mode == OUTPUT)
	{
		gpio_set_direction(p, GPIO_MODE_OUTPUT);
	}
	else if (mode == INPUT)
	{
		gpio_set_direction(p, GPIO_MODE_INPUT);
	}
}
inline void digitalWrite(int pin, int v)
{
	gpio_set_level((gpio_num_t)pin, v != LOW ? 1 : 0);
}
inline void yield()
{
	taskYIELD();
}
inline void delay(int ms)
{
	vTaskDelay(ms / portTICK_RATE_MS);
}

inline uint64_t millis()
{
	return xTaskGetTickCount() * portTICK_RATE_MS;
}


#define RISING 1
#define FALLING 2
void attachInterrupt(int pin, void (*)(void *), int mode);
void detachInterrupt(int pin);


#define B1000 0b1000
#define B111 0b111

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

typedef uint8_t byte;

class SPIClass;
class SPISettings;
class Stream;



 // Mode         Clock Polarity (CPOL)   Clock Phase (CPHA)
 //  SPI_MODE0       0                   0
 //  SPI_MODE1       0                   1
 //  SPI_MODE2       1                   0
 //  SPI_MODE3       1                   1

 #define SPI_MODE0 0x00
 #define SPI_MODE1 0x0F
 #define SPI_MODE2 0xF0
 #define SPI_MODE3 0xFF

#define MSBFIRST   0x1

class SPISettings
{
public:
	SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode)
		: clock_(clock), bit_order_(bitOrder), data_mode_(dataMode) {}

	SPISettings()
		: clock_(4000000), bit_order_(MSBFIRST), data_mode_(SPI_MODE0) {}

private:
	friend class SPIClass;
	uint32_t clock_;
	uint8_t bit_order_;
	uint8_t data_mode_;
};
class SPIClass
{
public:
	SPIClass(gpio_num_t clk, gpio_num_t mosi, gpio_num_t miso, gpio_num_t cs, gpio_num_t rst, gpio_num_t irq);

	void begin() {}
	void end() {}
	void beginTransaction(SPISettings &) {}
	void endTransaction() {}

	void setup();

	uint8_t transfer(uint8_t);

private:
	spi_device_handle_t spi; // SPI handle.
	gpio_num_t clk;
	gpio_num_t mosi;
	gpio_num_t miso;
	gpio_num_t cs;
	gpio_num_t rst;
	gpio_num_t irq;

/*
	void lcd_cmd(uint8_t cmd);
	void lcd_data(const uint8_t *data, int len);
	uint32_t lcd_get_id(int sz);
*/
};

class String
{
public:
	String() { _pt = strdup(""); }
	String(const String &r) { _pt = strdup(r._pt); }
	String(const char *r) { _pt = strdup(r); }
	~String() { free(_pt); }
	int Len() const { return strlen(_pt); }
	
	const String & operator = (const String &r) { free(_pt); _pt = strdup(r._pt); return *this; }

	const char * c_str() const { return _pt; }

	void Append(const char *p)
	{
		int sz = 1 + strlen(_pt) + strlen(p);
		char *t = (char *)malloc(sz);
		strcpy(t, _pt);
		strcat(t, p);
		free(_pt);
		_pt = t;
	}
private:
	char *_pt;
};