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


#ifdef ARDUINO
inline uint8_t wch(const uint8_t *font) { return pgm_read_byte(font + 0); }
inline uint8_t hch(const uint8_t *font) { return pgm_read_byte(font + 1); }

inline bool pxch(const uint8_t *font, char ch, uint8_t x, uint8_t y)
{
	uint8_t w = pgm_read_byte(font + 0);
	uint8_t h = pgm_read_byte(font + 1);

	font += 2;

	uint8_t  nb = 1 + (w > 8);
	uint16_t  ci = ch - ' ';

	uint8_t b = 0b10000000;
	uint8_t v;
	if (nb == 1)
	{
		v = pgm_read_byte(&font[ci*h + y]);
		b >>= 8-w;
		b >>= x;
	}
	else
	{
		if (x < 8)
		{
			v = pgm_read_byte(&font[2 * (ci*h + y) + 0]);
			b >>= 8-w;
			b >>= x;
		}
		else
		{
			v = pgm_read_byte(&font[2 * (ci*h + y) + 1]);
			b >>= x-8;
		}
	}
	return (v & b) ? true : false;
}
#endif

#endif //fonts_h
