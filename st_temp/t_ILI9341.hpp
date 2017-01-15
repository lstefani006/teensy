#include <Arduino.h>
#include <SPI.h>

#include "fonts.hpp"

#define ILI9341_TFTWIDTH  240
#define ILI9341_TFTHEIGHT 320

#define ILI9341_NOP     0x00
#define ILI9341_SWRESET 0x01
#define ILI9341_RDDID   0x04
#define ILI9341_RDDST   0x09

#define ILI9341_SLPIN   0x10
#define ILI9341_SLPOUT  0x11
#define ILI9341_PTLON   0x12
#define ILI9341_NORON   0x13

#define ILI9341_RDMODE  0x0A
#define ILI9341_RDMADCTL  0x0B
#define ILI9341_RDPIXFMT  0x0C
#define ILI9341_RDIMGFMT  0x0A
#define ILI9341_RDSELFDIAG  0x0F

#define ILI9341_INVOFF  0x20
#define ILI9341_INVON   0x21
#define ILI9341_GAMMASET 0x26
#define ILI9341_DISPOFF 0x28
#define ILI9341_DISPON  0x29

#define ILI9341_CASET   0x2A
#define ILI9341_PASET   0x2B
#define ILI9341_RAMWR   0x2C
#define ILI9341_RAMRD   0x2E

#define ILI9341_PTLAR   0x30
#define ILI9341_MADCTL  0x36
#define ILI9341_PIXFMT  0x3A

#define ILI9341_FRMCTR1 0xB1
#define ILI9341_FRMCTR2 0xB2
#define ILI9341_FRMCTR3 0xB3
#define ILI9341_INVCTR  0xB4
#define ILI9341_DFUNCTR 0xB6

#define ILI9341_PWCTR1  0xC0
#define ILI9341_PWCTR2  0xC1
#define ILI9341_PWCTR3  0xC2
#define ILI9341_PWCTR4  0xC3
#define ILI9341_PWCTR5  0xC4
#define ILI9341_VMCTR1  0xC5
#define ILI9341_VMCTR2  0xC7

#define ILI9341_RDID1   0xDA
#define ILI9341_RDID2   0xDB
#define ILI9341_RDID3   0xDC
#define ILI9341_RDID4   0xDD

#define ILI9341_GMCTRP1 0xE0
#define ILI9341_GMCTRN1 0xE1
/*
#define ILI9341_PWCTR6  0xFC

*/

enum class Color : uint16_t
{
	BLACK       = 0x0000,
	NAVY        = 0x000F,
	DARKGREEN   = 0x03E0,
	DARKCYAN    = 0x03EF,
	MAROON      = 0x7800,
	PURPLE      = 0x780F,
	OLIVE       = 0x7BE0,
	LIGHTGREY   = 0xC618,
	DARKGREY    = 0x7BEF,
	BLUE        = 0x001F,
	GREEN       = 0x07E0,
	CYAN        = 0x07FF,
	RED         = 0xF800,
	MAGENTA     = 0xF81F,
	YELLOW      = 0xFFE0,
	WHITE       = 0xFFFF,
	ORANGE      = 0xFD20,
	GREENYELLOW = 0xAFE5,
	PINK        = 0xF81F,
};




// If the SPI library has transaction support, these functions
// establish settings and protect from interference from other
// libraries.  Otherwise, they simply do nothing.
#ifdef SPI_HAS_TRANSACTION
inline void spi_begin(void) { SPI.beginTransaction(SPISettings(36000000, MSBFIRST, SPI_MODE0)); }
inline void spi_end(void) { SPI.endTransaction(); }
#else
inline void spi_begin() {}
inline void spi_end() {}
#endif
inline	void spiwrite(uint8_t c) { SPI.write(c); }


template <int8_t _cs, int8_t _dc, int8_t _rst>
class t_ILI9341 : public Print
{
private:
	volatile uint32 *mosiport, *clkport, *dcport, *rsport, *csport;
	uint32_t  cspinmask, dcpinmask;
	uint16_t lineBuffer[ILI9341_TFTHEIGHT]; // DMA buffer. 16bit color data per pixel

	int16_t _width;
	int16_t _height;
	const uint8_t *_font;
	int16_t _cx;
	int16_t _cy;

	Color _foreColor;
	Color _backColor;

	uint8_t _rotation;

public:
	t_ILI9341()
	{
		_width  = ILI9341_TFTWIDTH;
		_height = ILI9341_TFTHEIGHT;

		_font = font_08x08;
		_cy = 0;
		_cx = -wch(_font);

		_foreColor = Color::YELLOW;
		_backColor = Color::BLUE;

		_rotation = 0;
	}

	void setForeColor(Color fc) { _foreColor = fc; }
	void setBackColor(Color fc) { _backColor = fc; }

	Color foreColor() const { return _foreColor; }
	Color backColor() const { return _backColor; }

	const uint8_t * setFont(const uint8_t *f)
	{
		auto t = _font;
		_font = f;
		setCursor(0, 0);
		return t;
	}

	void setCursor(uint16_t cx, uint16_t cy)
	{
		_cy = cy;
		_cx = cx - wch(_font);
	}
	uint16_t cursorX() const { return _cx + wch(_font); }
	uint16_t cursorY() const { return _cy; }

	uint8_t fontW()  const { return wch(_font); }
	uint8_t fontH() const { return hch(_font); }

	void begin()
	{
		if (_rst > 0)
		{
			pinMode(_rst, OUTPUT);
			digitalWrite(_rst, LOW);
		}

		pinMode(_dc, OUTPUT);
		pinMode(_cs, OUTPUT);
		csport    = portOutputRegister(digitalPinToPort(_cs));
		cspinmask = digitalPinToBitMask(_cs);
		dcport    = portOutputRegister(digitalPinToPort(_dc));
		dcpinmask = digitalPinToBitMask(_dc);

		SPI.begin();
		SPI.setClockDivider(SPI_CLOCK_DIV2);
		SPI.setBitOrder(MSBFIRST);
		SPI.setDataMode(SPI_MODE0);

		// toggle RST low to reset
		if (_rst > 0)
		{
			digitalWrite(_rst, HIGH);
			delay(5);
			digitalWrite(_rst, LOW);
			delay(20);
			digitalWrite(_rst, HIGH);
			delay(150);
		}

		spi_begin();
		writecommand(0xEF);
		writedata(0x03);
		writedata(0x80);
		writedata(0x02);

		writecommand(0xCF);
		writedata(0x00);
		writedata(0XC1);
		writedata(0X30);

		writecommand(0xED);
		writedata(0x64);
		writedata(0x03);
		writedata(0X12);
		writedata(0X81);

		writecommand(0xE8);
		writedata(0x85);
		writedata(0x00);
		writedata(0x78);

		writecommand(0xCB);
		writedata(0x39);
		writedata(0x2C);
		writedata(0x00);
		writedata(0x34);
		writedata(0x02);

		writecommand(0xF7);
		writedata(0x20);

		writecommand(0xEA);
		writedata(0x00);
		writedata(0x00);

		writecommand(ILI9341_PWCTR1);    //Power control
		writedata(0x23);   //VRH[5:0]

		writecommand(ILI9341_PWCTR2);    //Power control
		writedata(0x10);   //SAP[2:0];BT[3:0]

		writecommand(ILI9341_VMCTR1);    //VCM control
		writedata(0x3e); //�Աȶȵ���
		writedata(0x28);

		writecommand(ILI9341_VMCTR2);    //VCM control2
		writedata(0x86);  //--

		writecommand(ILI9341_MADCTL);    // Memory Access Control
		writedata(0x48);

		writecommand(ILI9341_PIXFMT);
		writedata(0x55);

		writecommand(ILI9341_FRMCTR1);
		writedata(0x00);
		writedata(0x18);

		writecommand(ILI9341_DFUNCTR);    // Display Function Control
		writedata(0x08);
		writedata(0x82);
		writedata(0x27);

		writecommand(0xF2);    // 3Gamma Function Disable
		writedata(0x00);

		writecommand(ILI9341_GAMMASET);    //Gamma curve selected
		writedata(0x01);

		writecommand(ILI9341_GMCTRP1);    //Set Gamma
		writedata(0x0F);
		writedata(0x31);
		writedata(0x2B);
		writedata(0x0C);
		writedata(0x0E);
		writedata(0x08);
		writedata(0x4E);
		writedata(0xF1);
		writedata(0x37);
		writedata(0x07);
		writedata(0x10);
		writedata(0x03);
		writedata(0x0E);
		writedata(0x09);
		writedata(0x00);

		writecommand(ILI9341_GMCTRN1);    //Set Gamma
		writedata(0x00);
		writedata(0x0E);
		writedata(0x14);
		writedata(0x03);
		writedata(0x11);
		writedata(0x07);
		writedata(0x31);
		writedata(0xC1);
		writedata(0x48);
		writedata(0x08);
		writedata(0x0F);
		writedata(0x0C);
		writedata(0x31);
		writedata(0x36);
		writedata(0x0F);

		writecommand(ILI9341_SLPOUT);    //Exit Sleep
		spi_end();
		delay(120);
		spi_begin();
		writecommand(ILI9341_DISPON);    //Display on
		spi_end();
	}

	void drawPixel(int16_t x, int16_t y, Color color)
	{
		if (x < 0 || x >= _width) return;
		if (y < 0 || y >= _height) return;

		spi_begin();

		// N.B. x/y -- (x+1)/(y+1)
		setAddrWindow(x, y, /*x+1, y+1*/x,y);

		*dcport |=  dcpinmask;
		*csport &= ~cspinmask;

		spiwrite(uint16_t(color) >> 8);
		spiwrite(uint16_t(color));

		*csport |= cspinmask;
		spi_end();
	}

	// estremi inclusi
	void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, Color color)
	{
		if (y0 == y1)
		{
			// linea orizzontale
			drawFastHLine(x0, y0, x1, color);
			return;
		}
		if (x0 == x1)
		{
			// linea verticale
			drawFastVLine(x0, y0, y1, color);
			return;
		}

		auto steep = abs(y1 - y0) > abs(x1 - x0);
		if (steep)
		{
			swap(x0, y0);
			swap(x1, y1);
		}

		if (x0 > x1)
		{
			swap(x0, x1);
			swap(y0, y1);
		}

		auto dx = x1 - x0;
		auto dy = abs(y1 - y0);

		auto err = dx / 2;
		auto ystep = (y0 < y1) ? 1 : -1;

		while (x0 <= x1)
		{
			if (steep)
				drawPixel(y0, x0, color);
			else
				drawPixel(x0, y0, color);

			err -= dy;
			if (err < 0)
			{
				y0 += ystep;
				err += dx;
			}

			++x0;
		}
	}

	void drawRect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, Color color)
	{
		drawLine(x0, y0, x1, y0, color);
		drawLine(x1, y0, x1, y1, color);
		drawLine(x1, y1, x0, y1, color);
		drawLine(x0, y1, x0, y0, color);
	}


	void fillScreen(Color color)
	{
		spi_begin();

		setAddrWindow(0, 0, _width - 1, _height - 1);
		*dcport |=  dcpinmask;
		*csport &= ~cspinmask;
		SPI.setDataSize(SPI_CR1_DFF); // Set spi 16bit mode
		lineBuffer[0] = uint16_t(color);
		SPI.dmaSend(lineBuffer, 65535, 0);
		SPI.dmaSend(lineBuffer, _width * _height - 65535, 0);
		SPI.setDataSize(0);

		_backColor = color;

		spi_end();
	}

	void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, Color color)
	{
		// rudimentary clipping (drawChar w/big text requires this)
		if ((x >= _width) || (y >= _height || h < 1 || w < 1)) return;
		if ((x + w - 1) >= _width)  w = _width  - x;
		if ((y + h - 1) >= _height) h = _height - y;
		if (w == 1 && h == 1)
		{
			drawPixel(x, y, color);
			return;
		}

		spi_begin();
		setAddrWindow(x, y, x + w - 1, y + h - 1);

		*dcport |=  dcpinmask;
		*csport &= ~cspinmask;

		SPI.setDataSize (SPI_CR1_DFF); // Set spi 16bit mode
		lineBuffer[0] = color;
		if (w * h <= 65535)
			SPI.dmaSend(lineBuffer, (w * h), 0);
		else
		{
			SPI.dmaSend(lineBuffer, (65535), 0);
			SPI.dmaSend(lineBuffer, ((w * h) - 65535), 0);
		}
		SPI.setDataSize (0);
		*csport |= cspinmask;
		spi_end();
	}

	void drawChar(char ch)
	{
		write(ch);
	}

	// Pass 8-bit (each) R,G,B, get back 16-bit packed color
	Color color565(uint8_t r, uint8_t g, uint8_t b)
	{
		return Color(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
	}


	void setRotation(uint8_t m)
	{
#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

		_rotation = m % 4;

		spi_begin();
		writecommand(ILI9341_MADCTL);
		switch (_rotation)
		{
		case 0:
			writedata(MADCTL_MX | MADCTL_BGR);
			_width  = ILI9341_TFTWIDTH;
			_height = ILI9341_TFTHEIGHT;
			break;
		case 1:
			writedata(MADCTL_MV | MADCTL_BGR);
			_width  = ILI9341_TFTHEIGHT;
			_height = ILI9341_TFTWIDTH;
			break;
		case 2:
			writedata(MADCTL_MY | MADCTL_BGR);
			_width  = ILI9341_TFTWIDTH;
			_height = ILI9341_TFTHEIGHT;
			break;
		case 3:
			writedata(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
			_width  = ILI9341_TFTHEIGHT;
			_height = ILI9341_TFTWIDTH;
			break;
		}
		spi_end();
	}
	uint8_t rotation() const { return _rotation; }

	uint16_t w() const { return _width; }
	uint16_t h() const { return _height; }

private:
	void writecommand(uint8_t c)
	{
		*dcport &= ~dcpinmask;
		*csport &= ~cspinmask;
		spiwrite(c);
		*csport |= cspinmask;
	}

	void writedata(uint8_t c)
	{
		*dcport |=  dcpinmask;
		*csport &= ~cspinmask;
		spiwrite(c);
		*csport |= cspinmask;
	}

	void setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
	{
		writecommand(ILI9341_CASET); // Column addr set
		*dcport |=  dcpinmask;
		*csport &= ~cspinmask;
		SPI.setDataSize (SPI_CR1_DFF);
		SPI.write(x0);
		SPI.write(x1);
		writecommand(ILI9341_PASET); // Row addr set

		*dcport |=  dcpinmask;
		*csport &= ~cspinmask;
		SPI.write(y0);
		SPI.write(y1);
		SPI.setDataSize(0);
		writecommand(ILI9341_RAMWR); // write to RAM
	}
	template <typename T> static void swap(T &a, T &b)
	{
		auto t = a;
		a = b;
		b = t;
	}

	void drawFastVLine(int16_t x0, int16_t y0, int16_t y1, Color color)
	{
		if (y0 > y1) swap(y0, y1);
		if (y0 == y1) { drawPixel(x0, y0, color); return; }

		spi_begin();
		setAddrWindow(x0, y0, x0, y1);
		*dcport |=  dcpinmask;
		*csport &= ~cspinmask;

		SPI.setDataSize(SPI_CR1_DFF); // Set SPI 16bit mode
		lineBuffer[0] = uint16_t(color);
		SPI.dmaSend(lineBuffer, y1-y0+1, 0);
		SPI.setDataSize(0);
		*csport |= cspinmask;
		spi_end();
	}


	void drawFastHLine(int16_t x0, int16_t y0, int16_t x1, Color color)
	{
		if (x0 > x1) swap(x0, x1);
		if (x1 == x0) { drawPixel(x0, y0, color); return; }

		spi_begin();
		setAddrWindow(x0, y0, x1, y0);
		*dcport |=  dcpinmask;
		*csport &= ~cspinmask;

		SPI.setDataSize(SPI_CR1_DFF); // Set spi 16bit mode
		lineBuffer[0] = uint16_t(color);
		SPI.dmaSend(lineBuffer, x1-x0+1, 0);
		SPI.setDataSize(0);
		*csport |= cspinmask;
		spi_end();
	}

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
		if (_cx + ch_w > _width)
		{   
			for (int16_t x = _cx; x < _width ; x++)
				for (int16_t y = _cy; y < _cy + ch_h; ++y)
					drawPixel(x, y, _backColor);

			_cx = 0;
			_cy += ch_h;
		}

		if (character < 0x20 || character > 0x7f) character = '?';

		if (_cy + ch_h > _height)
		{   
			_cy -= ch_h;
			//scrollUp();
		}

		for (uint16_t y = 0; y < ch_h; ++y)
			for (uint16_t x = 0; x < ch_w; ++x)
			{   
				bool v = pxch(_font, character, x, y);
				drawPixel(_cx + x, _cy + y, v ? _foreColor : _backColor);
			}
		return 1;
	}
};
