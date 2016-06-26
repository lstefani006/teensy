#ifndef __t_5110_h__
#define __t_5110_h__

#include "t_SPI.h"
#if defined(__arm__) && defined(TEENSYDUINO) && defined(KINETISK)
#	include <TimerThree.h>
#else
#	include <TimerOne.h>
#endif


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
	extern const uint8_t ASCII[][5];

	template <typename  T, int8_t _pinRST, int8_t _pinDC, bool timerUpdate> class Lcd : public Print
	{
		enum LcdCommand { LCD_CMD = 0, LCD_DATA = 1 };

		T &_spi;
		typename T::Settings _spiSettings;

		void write(LcdCommand dc, uint8_t data)
		{
			digitalWrite(_pinDC, (int8_t)dc);
			_spi.write(data);
		}
		void writeCommand(uint8_t data) { write(LCD_CMD, data); }
		void writeData(uint8_t data) { write(LCD_DATA, data); }

		// Size of the LCD
	public:
		constexpr static int8_t LCD_X = 84;
		constexpr static int8_t LCD_Y = 48;
	private:
		constexpr static int16_t LCD_SZ = LCD_X * LCD_Y / 8;

		int8_t _wch;
		int8_t _hch;
		uint8_t _buff[LCD_SZ];
		uint8_t _invalid;
		int8_t _cx;
		int8_t _cy;

		inline void xy(int8_t x, int8_t y) {
			writeCommand(0x80 | x);  // Column.
			writeCommand(0x40 | y / 8);  // Row.
		}
	protected:
		size_t write(uint8_t ch) {
			if (ch == '\r') return 1;
			print(ch);
			return 1;
		}
	public:

		// PDC8544 max clock 4Mhz ==> SPI_CLOCK_DIV4
		Lcd(T &spi) : 
			_spi(spi), 
			_spiSettings(1*1000*1000, MSBFIRST, SPI_MODE0) 
		{
			_wch = 6;
			_hch = 8;
			_cx = 0;
			_cy = 0;
			_invalid = 0;
		}
		void setContrast(uint8_t val) 
		{
			if (val > 0x7f)
				val = 0x7f;
			writeCommand(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION );
			writeCommand(PCD8544_SETVOP | val); 
			writeCommand(PCD8544_FUNCTIONSET);

		}

		void begin()
		{
			pinMode(_pinDC,  OUTPUT);
			pinMode(_pinRST, OUTPUT);

			digitalWrite(_pinRST, LOW);
			delay(500);
			digitalWrite(_pinRST, HIGH);

			typename T::SPITransaction tr(_spi, _spiSettings);
			writeCommand(0x21);  // LCD Extended Commands.
			writeCommand(0xBf);  // Set LCD Vop (Contrast). //B1
			writeCommand(0x04);  // Set Temp coefficent. //0x04
			writeCommand(0x14);  // LCD bias mode 1:48. //0x13
			writeCommand(0x0C);  // LCD in normal mode. 0x0d for inverse
			writeCommand(0x20);
			writeCommand(0x0C);

#if defined(__arm__) && defined(TEENSYDUINO) && defined(KINETISK)
			if (timerUpdate) {
				Timer3.initialize(1000 * 20);
				sThis = this;
				Timer3.attachInterrupt(s_update);
			}
#else
			if (timerUpdate) {
				Timer1.initialize(1000 * 20);
				sThis = this;
				Timer1.attachInterrupt(s_update);
			}
#endif

			clear();
			if (!timerUpdate)
				updateTimer();
		}

		static Lcd *sThis;
		static void s_update() {
			if (sThis) {
				sThis->updateTimer();
			}
		}

	private:
		void updateTimer()
		{
			if (_invalid == 0) 
				return;

			typename T::SPITransaction tr(_spi, _spiSettings);
			for (int8_t y = 0; y < LCD_Y / 8; ++y)
			{
				if (_invalid & (1u << y))
				{
					xy(0, y * 8);
					for (int8_t x = 0; x < LCD_X; x++)
						writeData(_buff[y * LCD_X + x]);
				}
			}
			_invalid = 0;
		}

	public:
		void update()
		{
			if (timerUpdate == false) updateTimer();
		}
		void clear()
		{
			noInterrupts();
			for (int8_t i = 0; i < LCD_SZ; i++)
				_buff[i] = 0x00;
			_cx = -_wch;
			_cy = 0;
			_invalid = 0x3f;  // 6 righe
			interrupts();
		}

		bool getPixel(int8_t x, int8_t y) const
		{
			if (x < 0 || x >= LCD_X) return false;
			if (y < 0 || y >= LCD_Y) return false;

			x = LCD_X - 1 - x;
			y = LCD_Y - 1 - y;

			const uint8_t *p = &_buff[y / 8 * LCD_X + x];
			return *p & (1u << (y&7)) ? true : false;
		}

		void putPixel(int8_t x, int8_t y, bool v = true)
		{
			if (x < 0 || x >= LCD_X) return;
			if (y < 0 || y >= LCD_Y) return;

			x = LCD_X - 1 - x;
			y = LCD_Y - 1 - y;

			uint8_t *p = &_buff[y / 8 * LCD_X + x];

			if (v)
				*p = *p | (1u << (y&7));
			else
				*p = *p & ~(1u << (y&7));

			_invalid = _invalid | (1 << (y / 8));
		}
		void line(int x0, int y0, int x1, int y1)
		{
			int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
			int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
			int err = (dx>dy ? dx : -dy)/2, e2;

			for(;;)
			{
				putPixel(x0, y0, true);
				if (x0==x1 && y0==y1) break;
				e2 = err;
				if (e2 >-dx) { err -= dy; x0 += sx; }
				if (e2 < dy) { err += dx; y0 += sy; }
			}
		}
		void circle(int xm, int ym, int r)
		{
			int x = -r, y = 0, err = 2-2*r; /* II. Quadrant */ 
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

		void ellipse(int x0, int y0, int x1, int y1)
		{
			int a = abs(x1-x0), b = abs(y1-y0), b1 = b&1; /* values of diameter */
			long dx = 4*(1-a)*b*b, dy = 4*(b1+1)*a*a; /* error increment */
			long err = dx+dy+b1*a*a, e2; /* error of 1.step */

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

		void print(uint8_t character)
		{
			if (character == '\n')
			{
				_cx = -_wch;
				_cy += _hch;
				return;
			}

			_cx += _wch;
			if (_cx + _wch > LCD_X) 
			{
				for (int8_t x = _cx; x < LCD_X ; x++)
					for (int8_t y = _cy; y < _cy + _hch; ++y)
						putPixel(x, y, false);

				_cx = 0;
				_cy += _hch;
			}

			if (character < 0x20 || character > 0x7f) character = '?';

			if (_cy + _hch > LCD_Y)
			{
				_cy -= _hch;
				scrollUp();
			}

			for (int8_t x = 0; x < _wch; x++)
			{
				uint8_t a;
				if (x == _wch - 1)
					a = 0;
				else
					a = ASCII[character - 0x20][x];

				for (int8_t y = 0; y < _hch; ++y)
				{
					bool v = (a & (1 << y)) ? true : false;
					putPixel(_cx + x, _cy + y, v);
				}
			}
		}

		void scrollUp() 
		{
			for (int8_t y = _hch; y < LCD_Y; y++)
			{
				for (int8_t x = 0; x < LCD_X; x++)
				{
					bool v = getPixel(x, y);
					putPixel(x, y - _hch, v);
				}
			}
			for (int8_t y = _cy; y < LCD_Y; y++)
			{
				for (int8_t x = 0; x < LCD_X; x++)
					putPixel(x, y, false);
			}
		}

		void print(const char *characters)
		{
			while (*characters)
				print(*characters++);
		}


		void gotoXY(int8_t x, int8_t y)
		{
			if (x < 0 || x >= LCD_X) return;
			if (y < 0 || y >= LCD_Y) return;
			_cx = x - _wch;
			_cy = y;
		}
	};
	template <typename  T, int8_t pinRST, int8_t pinDC, bool timerUpdate>
		Lcd<T, pinRST, pinDC, timerUpdate> * Lcd<T, pinRST, pinDC, timerUpdate>::sThis = nullptr;
}
#endif
