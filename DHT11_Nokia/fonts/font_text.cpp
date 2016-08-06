#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fonts.h"


void printBin(uint8_t c, int numBit)
{
	uint8_t b = 0b1000'0000;

	b >>= numBit;

	for (int i = numBit; i < 8; ++i)
	{
		if (c&b) printf("X"); else printf(".");
		b = b >> 1;
	}
}

int main(int ac, const char *av[])
{
	const uint8_t *p = nullptr;

	/***/if (ac > 0 && strstr(av[1], "04x06") != nullptr) p = font_04x06;
	else if (ac > 0 && strstr(av[1], "05x08") != nullptr) p = font_05x08;
	else if (ac > 0 && strstr(av[1], "05x12") != nullptr) p = font_05x12;
	else if (ac > 0 && strstr(av[1], "06x08") != nullptr) p = font_06x08;
	else if (ac > 0 && strstr(av[1], "06x10") != nullptr) p = font_06x10;
	else if (ac > 0 && strstr(av[1], "07x12") != nullptr) p = font_07x12;
	else if (ac > 0 && strstr(av[1], "08x08") != nullptr) p = font_08x08;
	else if (ac > 0 && strstr(av[1], "08x12") != nullptr) p = font_08x12;
	else if (ac > 0 && strstr(av[1], "08x14") != nullptr) p = font_08x14;
	else if (ac > 0 && strstr(av[1], "10x16") != nullptr) p = font_10x16;
	else if (ac > 0 && strstr(av[1], "12x16") != nullptr) p = font_12x16;
	else if (ac > 0 && strstr(av[1], "12x20") != nullptr) p = font_12x20;
	else if (ac > 0 && strstr(av[1], "16x26") != nullptr) p = font_16x26;
	else return 1;

	int w = p[0];
	int h = p[1];
	p+=2;

	int nb = 1;
	if (w > 8) nb = 2;

	for (int c = ' '; c < 127; ++c)
	{
		int ci = c - ' ';

		printf("%d %c\n", c, c);
		for (int i = 0; i < h; ++i)
		{
			if (nb == 1)
			{
				printBin(p[ci*h*nb + nb*i+0], 8-w);
			}
			else
			{
				printBin(p[ci*h*nb + nb*i+1], 16-w);
				printBin(p[ci*h*nb + nb*i+0], 0);
			}

			printf("\n");
		}
		printf("\n");
	}
}


bool getPixel(const uint8_t *font, char ch, uint8_t x, uint8_t y)
{
	uint8_t w = font[0];
	uint8_t h = font[1];

	const uint8_t *p = font + 2;

	uint8_t  nb = 1 + (w > 8);
	uint8_t  ci = ch - ' ';

	uint8_t b = 0b1000'0000;
	uint8_t v;
	if (nb == 1)
	{
		v = p[ci*h + y];
		b >>= 8-w;
		b >>= x;
	}
	else
	{
		if (x < 8)
		{
			v = p[2 * (ci*h + y) + 0];
			b >>= 8-w;
			b >>= x;
		}
		else
		{
			v = p[2 * (ci*h + y) + 1];
			b >>= x-8;
		}
	}
	return (v & b) ? true : false;
}
