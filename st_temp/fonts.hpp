#ifndef fonts_h
#define fonts_h

#ifdef ARDUINO
#include <Arduino.h>
#else 
#define PROGMEM
#endif


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

inline const uint8_t * getFont(uint8_t fi)
{
	switch (fi) 
	{
	case 0: return font_04x06;
	case 1: return font_05x07;
	case 2: return font_05x08;
	case 3: return font_05x12;
	case 4: return font_06x08;
	case 5: return font_06x10;
	case 6: return font_07x12;
	case 7: return font_08x08;
	case 9: return font_08x12;
	case 10: return font_08x14;
	case 11: return font_10x16;
	case 12: return font_12x16;
	case 13: return font_12x20;
	case 14: return font_16x26;

	default: return nullptr;
	}
}


inline uint8_t wch(const uint8_t *font) { return pgm_read_byte(font + 0); }
inline uint8_t hch(const uint8_t *font) { return pgm_read_byte(font + 1); }

inline uint8_t pch(const uint8_t *font, char ch, int8_t y)
{
	//uint8_t w = pgm_read_byte(font + 0);
	uint8_t h = pgm_read_byte(font + 1);
	font += 2;

	uint16_t  ci = ch - ' ';
	auto v = pgm_read_byte(&font[ci*h + y]);
	return v;

}
inline bool pxch(const uint8_t *font, char ch, uint8_t x, uint8_t y)
{
	uint8_t w = pgm_read_byte(font + 0);
	uint8_t h = pgm_read_byte(font + 1);

	font += 2;

	uint16_t  ci = ch - ' ';

	if (w <= 8)
	{
		uint8_t b = 0b10000000;
		b = uint8_t(b >> (8u-w));
		b = uint8_t(b >> x);

		uint8_t v = pgm_read_byte(&font[ci*h + y]);
		return (v & b) ? true : false;
	}
	else
	{
		uint16_t b = 0b1000000000000000;
		b = uint16_t(b >> (16u-w));
		b = uint16_t(b >> x);

		uint16_t v1 = pgm_read_byte(&font[2 * (ci*h + y) + 0]);
		uint16_t v2 = pgm_read_byte(&font[2 * (ci*h + y) + 1]);
		uint16_t v = (v1 << 8) | v2;
		return (v & b) ? true : false;
	}
}

#endif //fonts_h
