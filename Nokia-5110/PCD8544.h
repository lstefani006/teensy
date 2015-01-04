#ifndef _PCD8544_H
#define _PCD8544_H

#include <Arduino.h>
#include <SPI.h>

#define BLACK 1
#define WHITE 0

#define LCDWIDTH 84
#define LCDHEIGHT 48

#define PCD8544_POWERDOWN 0x04
#define PCD8544_ENTRYMODE 0x02
#define PCD8544_EXTENDEDINSTRUCTION 0x01

#define PCD8544_DISPLAYBLANK 0x0
#define PCD8544_DISPLAYNORMAL 0x4
#define PCD8544_DISPLAYALLON 0x1
#define PCD8544_DISPLAYINVERTED 0x5

// H = 0
#define PCD8544_FUNCTIONSET 0x20
#define PCD8544_DISPLAYCONTROL 0x08
#define PCD8544_SETYADDR 0x40
#define PCD8544_SETXADDR 0x80

// H = 1
#define PCD8544_SETTEMP 0x04
#define PCD8544_SETBIAS 0x10
#define PCD8544_SETVOP 0x80

// Default to max SPI clock speed for PCD8544 of 4 mhz (16mhz / 4) for normal Arduinos.
// This can be modified to change the clock speed if necessary (like for supporting other hardware).
#define PCD8544_SPI_CLOCK_DIV SPI_CLOCK_DIV4

class PCD8544 {
public:
	PCD8544(int8_t DC, int8_t CS, int8_t RST);

	void begin(uint8_t contrast = 40, uint8_t bias = 0x04);

	void command(uint8_t c);
	void data(uint8_t c);

	void setContrast(uint8_t val);
	void clearDisplay(void);
	void display();

	void drawPixel(int16_t x, int16_t y, uint16_t color);
	uint8_t getPixel(int8_t x, int8_t y);

private:
	int8_t _dc, _rst, _cs;
};

#endif
