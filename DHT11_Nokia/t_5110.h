﻿#ifndef __t_5110_h__
#define __t_5110_h__

#include "t_SPI.h"
#if defined(__arm__) && defined(TEENSYDUINO) && defined(KINETISK)
#	include <TimerThree.h>
#else
#	include <TimerOne.h>
#endif

#include "fonts.hpp"


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


namespace t 
{
	extern const uint8_t ASCII[][5] PROGMEM;

	template <typename  T, int8_t _pinRST, int8_t _pinDC, bool timerUpdate> class Lcd : public Print
	{
		enum class LcdCommand : uint8_t { LCD_CMD = 0, LCD_DATA = 1 };

		T &_spi;

		void write(LcdCommand dc, uint8_t data)
		{
			digitalWrite(_pinDC, (int8_t)dc);
			_spi.write(data);
		}
		void writeCommand(uint8_t data) { write(LcdCommand::LCD_CMD, data); }
		void writeData(uint8_t data) { write(LcdCommand::LCD_DATA, data); }

		// Size of the LCD
	public:
		typedef int8_t gint;
		constexpr static gint LCD_X = 84;
		constexpr static gint LCD_Y = 48;
	private:
		constexpr static int16_t LCD_SZ = int16_t(LCD_X) * int16_t(LCD_Y) / 8;
		const uint8_t *_font;

		uint8_t _buff[LCD_SZ];  // 84*48/8 = 504 byte !!!
		uint8_t _invalid;
		bool    _inverse;
		gint _cx;
		gint _cy;

		inline void xy(gint x, gint y) {
			writeCommand(0x80 | x);  // Column.
			writeCommand(0x40 | y / 8);  // Row.
		}
	public:

		// PDC8544 max clock 4Mhz ==> SPI_CLOCK_DIV4
		Lcd(T &spi) : 
			_spi(spi)
		{
			_invalid = 0;
			_font = font_05x07;
			_cy = 0;
			_cx = -wch(_font);
			_inverse = false;
		}
		void setContrast(uint8_t val) 
		{
			if (val > 0x7f)
				val = 0x7f;
			writeCommand(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION );
			writeCommand(PCD8544_SETVOP | val); 
			writeCommand(PCD8544_FUNCTIONSET);

		}

		void setInverse(bool v) { _inverse = v; }
		bool inverse() const { return _inverse; }

		void begin()
		{
			pinMode(_pinDC,  OUTPUT);
			pinMode(_pinRST, OUTPUT);

			digitalWrite(_pinRST, LOW);
			delay(500);
			digitalWrite(_pinRST, HIGH);

			typename T::Settings spiSettings(1*1000L*1000L, MSBFIRST, SPI_MODE0);
			typename T::SPITransaction tr(_spi, spiSettings);
			writeCommand(0x21);  // LCD Extended Commands.
			writeCommand(0xBf);  // Set LCD Vop (Contrast). //B1
			writeCommand(0x04);  // Set Temp coefficent. //0x04
			writeCommand(0x14);  // LCD bias mode 1:48. //0x13
			writeCommand(0x0C);  // LCD in normal mode. 0x0d for inverse
			writeCommand(0x20);
			writeCommand(0x0C);

#if defined(__arm__) && defined(TEENSYDUINO) && defined(KINETISK)
			if (timerUpdate) {
				sThis = this;
				Timer3.initialize(1000 * 20);
				Timer3.attachInterrupt(s_updateTimer);
			}
#else
			if (timerUpdate) {
				sThis = this;
				Timer1.initialize(1000 * 20);
				Timer1.attachInterrupt(s_updateTimer);
			}
#endif

			clear();
			update();
		}

		static Lcd *sThis;
		static void s_updateTimer() {
			if (sThis)
				sThis->updateScreen();
		}

	private:
		void updateScreen()
		{
			if (_invalid == 0) 
				return;

			typename T::Settings spiSettings(1*1000L*1000L, MSBFIRST, SPI_MODE0);
			typename T::SPITransaction tr(_spi, spiSettings);
			for (gint y = 0; y < LCD_Y / 8; ++y)
			{
				if (_invalid & (1u << y))
				{
					xy(0, y * 8);
					for (gint x = 0; x < LCD_X; x++)
						writeData(_buff[int16_t(y) * LCD_X + x]);
				}
			}
			_invalid = 0;
		}

	public:

		void setFont(const uint8_t *font)
		{
			_font = font;
			_cx = -wch(_font);
			_cy = 0;
			gotoXY(0,0);
		}
		void update()
		{
			if (timerUpdate == false)
				updateScreen();
			else
			{
				// si aspetta il timer....
			}
		}
		void clear()
		{
			//if (timerUpdate) noInterrupts();
			for (int16_t i = 0; i < LCD_SZ; i++)
				_buff[i] = 0x00;
			_cx = -wch(_font);
			_cy = 0;
			_invalid = 0x3f;  // 6 righe
			//if (timerUpdate) interrupts();
		}


		bool getPixel(gint x, gint y) const
		{
			if (x < 0 || x >= LCD_X) return false;
			if (y < 0 || y >= LCD_Y) return false;

			x = LCD_X - 1 - x;
			y = LCD_Y - 1 - y;

			const uint8_t *p = &_buff[int16_t(y) / 8 * LCD_X + x];
			return *p & (1u << (y&7)) ? true : false;
		}

		void putPixel(gint x, gint y, bool v = true)
		{
			if (x < 0 || x >= LCD_X) return;
			if (y < 0 || y >= LCD_Y) return;

			x = LCD_X - 1 - x;
			y = LCD_Y - 1 - y;

			uint8_t *p = &_buff[int16_t(y) / 8 * LCD_X + x];

			if (v)
				*p = *p | (1u << (y&7));
			else
				*p = *p & ~(1u << (y&7));

			_invalid = _invalid | (1 << (y / 8));
		}
		void line(gint x0, gint y0, gint x1, gint y1)
		{
			gint dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
			gint dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
			gint err = (dx>dy ? dx : -dy)/2, e2;

			for(;;)
			{
				putPixel(x0, y0, true);
				if (x0==x1 && y0==y1) break;
				e2 = err;
				if (e2 >-dx) { err -= dy; x0 += sx; }
				if (e2 < dy) { err += dx; y0 += sy; }
			}
		}
		void line(gint x0, gint y0, gint x1, gint y1, gint d)
		{
			gint dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
			gint dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
			gint err = (dx>dy ? dx : -dy)/2, e2;

			gint nn = 0;
			for(;;)
			{
				putPixel(x0, y0, nn++ % d == 0);
				if (x0==x1 && y0==y1) break;
				e2 = err;
				if (e2 >-dx) { err -= dy; x0 += sx; }
				if (e2 < dy) { err += dx; y0 += sy; }
			}
		}
		void h_line(gint x0, gint x1, gint y)
		{
			gint sx = (x0 < x1) ? 1 : -1;
			for (;;) 
			{
				putPixel(x0, y, true);
				if (x0==x1) break;
				x0 += sx;
			}
		}
		void h_line(gint x0, gint x1, gint y, gint d)
		{
			gint sx = (x0 < x1) ? 1 : -1;
			for (;;) 
			{
				putPixel(x0, y, (x0 % d == 0) ? true : false);
				if (x0==x1) break;
				x0 += sx;
			}
		}
		void v_line(gint x, gint y0, gint y1)
		{
			gint sy = (y0 < y1) ? 1 : -1;
			for (;;) 
			{
				putPixel(x, y0, true);
				if (y0==y1) break;
				y0 += sy;
			}
		}
		void v_line(gint x, gint y0, gint y1, gint d)
		{
			gint sy = (y0 < y1) ? 1 : -1;
			gint t = 0;
			for (;;) 
			{
				putPixel(x, y0, t == 0);
				if (y0==y1) break;
				y0 += sy;
				t += 1; if (t == d) t = 0;
			}
		}
		void box(gint x0, gint y0, gint x1, gint y1)
		{
			x1 -= 1;
			y1 -= 1;

			line(x0, y0, x1, y0);
			line(x0, y1, x1, y1);

			line(x0, y0, x0, y1);
			line(x1, y0, x1, y1);
		}
		void circle(gint xm, gint ym, gint r)
		{
			gint x = -r, y = 0, err = 2-2*r; /* II. Quadrant */ 
			do {
				putPixel(xm-x, ym+y); /*   I. Quadrant */
				putPixel(xm-y, ym-x); /*  II. Quadrant */
				putPixel(xm+x, ym-y); /* III. Quadrant */
				putPixel(xm+y, ym+x); /*  IV. Quadrant */
				r = err;
				if (r <= y) err += ++y*2+1;           /* e_xy+e_y < 0 */
				if (r > x || err > y) err += ++x*2+1; /* e_xy+e_x > 0 or no 2nd y-step */
			} while (x < 0);
		}

		void ellipse(gint x0, gint y0, gint x1, gint y1)
		{
			gint a = abs(x1-x0), b = abs(y1-y0), b1 = b&1; /* values of diameter */
			int dx = 4*(1-a)*b*b, dy = 4*(b1+1)*a*a; /* error increment */
			int err = dx+dy+b1*a*a, e2; /* error of 1.step */

			if (x0 > x1) { x0 = x1; x1 += a; } /* if called with swapped points */
			if (y0 > y1) y0 = y1; /* .. exchange them */
			y0 += (b+1)/2; y1 = y0-b1;   /* starting pixel */
			a *= 8*a; b1 = 8*b*b;

			do {
				putPixel(x1, y0); /*   I. Quadrant */
				putPixel(x0, y0); /*  II. Quadrant */
				putPixel(x0, y1); /* III. Quadrant */
				putPixel(x1, y1); /*  IV. Quadrant */
				e2 = 2*err;
				if (e2 <= dy) { y0++; y1--; err += dy += a; }  /* y step */ 
				if (e2 >= dx || 2*err > dy) { x0++; x1--; err += dx += b1; } /* x step */
			} while (x0 <= x1);

			while (y0-y1 < b) {  /* too early stop of flat ellipses a=1 */
				putPixel(x0-1, y0); /* -> finish tip of ellipse */
				putPixel(x1+1, y0++); 
				putPixel(x0-1, y1);
				putPixel(x1+1, y1--); 
			}
		}

	public:
		size_t write(uint8_t character) override 
		{
			uint8_t ch_w = wch(_font);
			uint8_t ch_h = hch(_font);

			if (character == '\r') 
				return 1;
			if (character == '\n')
			{
				_cx = -ch_w;
				_cy += ch_h;
				return 1;
			}

			_cx += ch_w;
			if (_cx + ch_w > LCD_X) 
			{
				for (gint x = _cx; x < LCD_X ; x++)
					for (gint y = _cy; y < _cy + ch_h; ++y)
						putPixel(x, y, false);

				_cx = 0;
				_cy += ch_h;
			}

			if (character < 0x20 || character > 0x7f) character = '?';

			if (_cy + ch_h > LCD_Y)
			{
				_cy -= ch_h;
				scrollUp();
			}

			if (false)
			{
				for (uint8_t y = 0; y < ch_h; ++y)
					for (uint8_t x = 0; x < ch_w; ++x)
					{
						bool v = pxch(_font, character, x, y);
						putPixel(_cx + x, _cy + y, v);
					}
			}
			else
			{
				FontNavigator nv(_font, character);
				for (uint8_t y = 0; y < ch_h; ++y)
				{
					nv.inc_y();
					for (uint8_t x = 0; x < ch_w; ++x)
					{
						bool v = nv.get(x);
						if (_inverse) v = !v;
						putPixel(_cx + x, _cy + y, v);
					}
				}
			}

			return 1;
		}

		void scrollUp() 
		{
			uint8_t ch_h = hch(_font) - 1;

			for (gint y = ch_h; y < LCD_Y; y++)
			{
				for (gint x = 0; x < LCD_X; x++)
				{
					bool v = getPixel(x, y);
					putPixel(x, y - ch_h, v);
				}
			}
			for (gint y = _cy; y < LCD_Y; y++)
			{
				for (gint x = 0; x < LCD_X; x++)
					putPixel(x, y, false);
			}
		}

		void gotoXY(gint x, gint y)
		{
			if (x < 0 || x >= LCD_X) return;
			if (y < 0 || y >= LCD_Y) return;

			uint8_t ch_w = wch(_font);
			_cx = x - ch_w;
			_cy = y;
		}

		gint getX() const { 
			uint8_t ch_w = wch(_font);
			return _cx + ch_w; 
		}
		gint getY() const { return _cy; }

	};
	template <typename  T, int8_t pinRST, int8_t pinDC, bool timerUpdate>
		Lcd<T, pinRST, pinDC, timerUpdate> * Lcd<T, pinRST, pinDC, timerUpdate>::sThis = nullptr;
}
#endif
