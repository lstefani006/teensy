#include <Arduino.h>
#include <stdlib.h>

#include "PCD8544.h"

// the memory buffer for the LCD
static uint8_t pcd8544_buffer[LCDWIDTH * LCDHEIGHT / 8] = {0,};

PCD8544::PCD8544(int8_t DC, int8_t CS, int8_t RST)
{
	_dc = DC;
	_rst = RST;
	_cs = CS;
}

// the most basic function, set a single pixel
void PCD8544::drawPixel(int16_t x, int16_t y, uint16_t color) 
{
	if (x < 0 || x >= LCDWIDTH || y < 0 || y >= LCDHEIGHT)
		return;

	// x is which column
	if (color)
		pcd8544_buffer[x + y / 8 * LCDWIDTH] |= _BV(y%8);  
	else
		pcd8544_buffer[x + y / 8 * LCDWIDTH] &= ~_BV(y%8); 
}


// the most basic function, get a single pixel
uint8_t PCD8544::getPixel(int8_t x, int8_t y) 
{
	if (x < 0 || x >= LCDWIDTH || y < 0 || y >= LCDHEIGHT)
		return 0;

	return (pcd8544_buffer[x+ (y/8)*LCDWIDTH] >> (y%8)) & 0x1;  
}


void PCD8544::begin(uint8_t contrast, uint8_t bias) 
{
	// Setup hardware SPI.
	SPI.begin();
	SPI.setClockDivider(PCD8544_SPI_CLOCK_DIV);
	SPI.setDataMode(SPI_MODE0);
	SPI.setBitOrder(MSBFIRST);

	// Set common pin outputs.
	pinMode(_dc, OUTPUT);
	if (_rst > 0)
		pinMode(_rst, OUTPUT);
	if (_cs > 0)
		pinMode(_cs, OUTPUT);

	// toggle RST low to reset
	if (_rst > 0) {
		digitalWrite(_rst, LOW);
		delay(500);
		digitalWrite(_rst, HIGH);
	}

	// get into the EXTENDED mode!
	command(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION );

	// LCD bias select (4 is optimal?)
	command(PCD8544_SETBIAS | bias);

	// set VOP
	if (contrast > 0x7f)
		contrast = 0x7f;

	command(PCD8544_SETVOP | contrast); // Experimentally determined

	// normal mode
	command(PCD8544_FUNCTIONSET);

	// Set display to Normal
	command(PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL);

	// initial display line
	// set page address
	// set column address
	// write display data

	// set up a bounding box for screen updates
	// Push out pcd8544_buffer to the Display (will show the AFI logo)
	clearDisplay();
	display();
}


void PCD8544::command(uint8_t c) {
	digitalWrite(_dc, LOW);
	if (_cs > 0)
		digitalWrite(_cs, LOW);
	SPI.transfer(c);
	if (_cs > 0)
		digitalWrite(_cs, HIGH);
}

void PCD8544::data(uint8_t c) {
	digitalWrite(_dc, HIGH);
	if (_cs > 0)
		digitalWrite(_cs, LOW);
	SPI.transfer(c);
	if (_cs > 0)
		digitalWrite(_cs, HIGH);
}

void PCD8544::setContrast(uint8_t val) 
{
	if (val > 0x7f)
		val = 0x7f;
	command(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION );
	command(PCD8544_SETVOP | val); 
	command(PCD8544_FUNCTIONSET);

}

void PCD8544::display() 
{
	for (uint8_t p = 0; p < 6; p++) 
	{
		command(PCD8544_SETYADDR | p);

		// start at the beginning of the row
		uint8_t col = 0;
		command(PCD8544_SETXADDR | col);

		digitalWrite(_dc, HIGH);

		if (_cs > 0)
			digitalWrite(_cs, LOW);

		for(; col < LCDWIDTH; col++)
			SPI.transfer(pcd8544_buffer[LCDWIDTH * p + col]);

		if (_cs > 0)
			digitalWrite(_cs, HIGH);
	}

	command(PCD8544_SETYADDR);  // no idea why this is necessary but it is to finish the last byte?
}

// clear everything
void PCD8544::clearDisplay() 
{
	memset(pcd8544_buffer, 0, LCDWIDTH * LCDHEIGHT / 8);
}
