#ifndef fonts_h
#define fonts_h

#include <stdint.h>
#define PROGMEM

//if defined char range 0x20-0x7F otherwise 0x20-0xFF
#define FONT_END7F
#define FONT_START (0x20) //first character

extern const uint8_t font_04x06[] PROGMEM;
extern const uint8_t font_05x07[] PROGMEM;
extern const uint8_t font_05x08[] PROGMEM;
extern const uint8_t font_05x12[] PROGMEM;
extern const uint8_t font_06x08[] PROGMEM;
extern const uint8_t font_06x10[] PROGMEM;
extern const uint8_t font_07x12[] PROGMEM;
extern const uint8_t font_08x08[] PROGMEM;
extern const uint8_t font_08x12[] PROGMEM;
extern const uint8_t font_08x14[] PROGMEM;
extern const uint8_t font_10x16[] PROGMEM;
extern const uint8_t font_12x16[] PROGMEM;
extern const uint8_t font_12x20[] PROGMEM;
extern const uint8_t font_16x26[] PROGMEM;

inline uint8_t pgm_read_byte(const uint8_t *p) { return *p; }

inline uint8_t wch(const uint8_t *font) { return font[0]; }
inline uint8_t hch(const uint8_t *font) { return font[1]; }

inline uint32_t pch2(const uint8_t *font, char ch, int y)
{
	uint8_t w = wch(font);
	uint8_t h = hch(font);
	font += 2;

	if (y < 0 || y >= h)
		return uint32_t(-1);

	int ci = (ch - ' ') * h + y;

	uint32_t r;
	if (w > 24)
	{
		int s = 4 * ci;
		r = (font[s] << 24) | (font[s + 1] << 16) | (font[s + 2] << 8) | font[s + 3];
	}
	else if (w > 16)
	{
		int s = 3 * ci;
		r = (font[s] << 16) | (font[s + 1] << 8) | font[s + 2];
	}
	else if (w > 8)
	{
		int s = 2 * ci;
		r = (font[s] << 8) | font[s + 1];
	}
	else
	{
		int s = 1 * ci;
		r = font[s];
	}

	return r << (32 - w);
}

/*
class FontNavigator
{
public:
	FontNavigator(const uint8_t *font, char ch) : _p(font)
	{
		_ch_w = wch(_p);
		_ch_h = hch(_p);
		_p += 2;
		_v = 0;

		uint16_t ci = uint16_t(ch - ' ') * _ch_h;
		_p += ci;
		if (_ch_w > 8)
			_p += ci;

		inc_y();
	}

	bool get(uint8_t x)
	{
		uint16_t b = 0b10000000 << 8;
		b >>= 16 - _ch_w + x;
		return (_v & b) ? true : false;
	}
	void inc_y()
	{
		_v = pgm_read_byte(_p++) << 8;
		if (_ch_w - 8 > 8)
			_v |= pgm_read_byte(_p++);
	}

private:
	const uint8_t *_p;
	uint8_t _ch_w;
	uint8_t _ch_h;
	uint16_t _v;
};
*/

#endif //fonts_h
