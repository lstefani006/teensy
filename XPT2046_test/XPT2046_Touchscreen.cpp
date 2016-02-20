/* Touchscreen library for XPT2046 Touch Controller Chip
 * Copyright (c) 2015, Paul Stoffregen, paul@pjrc.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "XPT2046_Touchscreen.h"

#define Z_THRESHOLD     400
#define Z_THRESHOLD_INT	75
#define MSEC_THRESHOLD  3
#define SPI_SETTING     SPISettings(2000000, MSBFIRST, SPI_MODE0)

static bool S_isrWake;
static void isrPin(void) { S_isrWake = true; }

XPT2046_Touchscreen::XPT2046_Touchscreen(uint8_t cs, uint8_t tirq)
{
	_csPin = cs;
	_irqPin = tirq;
	_msraw = 0;
	_xraw = 0;
	_yraw = 0;
	_zraw = 0;
	S_isrWake = true;
}

bool XPT2046_Touchscreen::begin()
{
	SPI.begin();
	pinMode(_csPin, OUTPUT);
	digitalWrite(_csPin, HIGH);
	if (255 != _irqPin)
	{
		pinMode(_irqPin, INPUT_PULLUP);
		attachInterrupt(digitalPinToInterrupt(_irqPin), isrPin, FALLING);
	}
	return true;
}

TS_Point XPT2046_Touchscreen::getPoint()
{
	update();
	return TS_Point(_xraw, _yraw, _zraw);
}

bool XPT2046_Touchscreen::touched()
{
	update();
	return _zraw >= Z_THRESHOLD;
}

void XPT2046_Touchscreen::readData(uint16_t *x, uint16_t *y, uint8_t *z)
{
	update();
	*x = _xraw;
	*y = _yraw;
	*z = _zraw;
}


static int16_t besttwoavg(int16_t x , int16_t y , int16_t z)
{
	int16_t da, db, dc;
	if (x > y) da = x - y; else da = y - x;
	if (x > z) db = x - z; else db = z - x;
	if (z > y) dc = z - y; else dc = y - z;

	int16_t r;
	/***/if (da <= db && da <= dc) r = (x + y) >> 1;
	else if (db <= da && db <= dc) r = (x + z) >> 1;
	else                           r = (y + z) >> 1;

	return r;
}

// TODO: perhaps a future version should offer an option for more oversampling,
//       with the RANSAC algorithm https://en.wikipedia.org/wiki/RANSAC
void XPT2046_Touchscreen::update()
{
	if (!S_isrWake) return;
	uint32_t now = millis();
	if (now - _msraw < MSEC_THRESHOLD) return;

	SPI.beginTransaction(SPI_SETTING);
	digitalWrite(_csPin, LOW);
	SPI.transfer(0xB1 /* Z1 */);

	int16_t z;
	if (true)
	{
		int16_t z1 = SPI.transfer16(0xC1 /* Z2 */) >> 3;
		z = z1 + 4095;
		int16_t z2 = SPI.transfer16(0x91 /* X */) >> 3;
		z -= z2;
	}

	int16_t data[6];
	int8_t nn = 5;
	do { data[nn] = 0; } while (--nn >= 0);

	if (z >= Z_THRESHOLD)
	{
		SPI.transfer16(0x91 /* X */);  // dummy X measure, 1st is always noisy
		data[0] = SPI.transfer16(0xD1 /* Y */) >> 3;
		data[1] = SPI.transfer16(0x91 /* X */) >> 3; // make 3 x-y measurements
		data[2] = SPI.transfer16(0xD1 /* Y */) >> 3;
		data[3] = SPI.transfer16(0x91 /* X */) >> 3;
	}
	data[4] = SPI.transfer16(0xD0 /* Y */) >> 3;	// Last Y touch power down
	data[5] = SPI.transfer16(0) >> 3;

	digitalWrite(_csPin, HIGH);
	SPI.endTransaction();
	if (z < 0) z = 0;
	if (z < Z_THRESHOLD)
	{
		_zraw = 0;
		if (z < Z_THRESHOLD_INT)
		{
			if (255 != _irqPin) 
				S_isrWake = false;
		}
		return;
	}
	_zraw = z;

	int16_t x = besttwoavg(data[0], data[2], data[4]);
	int16_t y = besttwoavg(data[1], data[3], data[5]);

	if (z >= Z_THRESHOLD)
	{
		_msraw = now;	// good read completed, set wait
		_xraw = x;
		_yraw = y;
	}
}
