#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fonts.h"

void PrintFont(const uint8_t *p);
void ReadFont(FILE *fin);


int main(int ac, const char *av[])
{
	const uint8_t *p = nullptr;

	for (int i = 1; i < ac; ++i)
	{
		p = nullptr;

		/***/if (ac > 1 && strstr(av[i], "04x06") != nullptr) p = font_04x06;
		else if (ac > 1 && strstr(av[i], "05x08") != nullptr) p = font_05x08;
		else if (ac > 1 && strstr(av[i], "05x12") != nullptr) p = font_05x12;
		else if (ac > 1 && strstr(av[i], "06x08") != nullptr) p = font_06x08;
		else if (ac > 1 && strstr(av[i], "06x10") != nullptr) p = font_06x10;
		else if (ac > 1 && strstr(av[i], "07x12") != nullptr) p = font_07x12;
		else if (ac > 1 && strstr(av[i], "08x08") != nullptr) p = font_08x08;
		else if (ac > 1 && strstr(av[i], "08x12") != nullptr) p = font_08x12;
		else if (ac > 1 && strstr(av[i], "08x14") != nullptr) p = font_08x14;
		else if (ac > 1 && strstr(av[i], "10x16") != nullptr) p = font_10x16;
		else if (ac > 1 && strstr(av[i], "12x16") != nullptr) p = font_12x16;
		else if (ac > 1 && strstr(av[i], "12x20") != nullptr) p = font_12x20;
		else if (ac > 1 && strstr(av[i], "16x26") != nullptr) p = font_16x26;

		if (p)
			PrintFont(p);
		else
		{
			FILE *fin = stdin;
			if (ac > 1) fin = fopen(av[i], "r");
			ReadFont(fin);
			if (ac > 1) fclose(fin);
		}
	}
}

void print_b(uint16_t v, int nb)
{
	printf("0b");

	uint16_t m = 0b1000'0000;
	if (nb == 16)
		m <<= 8;

	for (int i = 0; i < nb; ++i)
	{
		if (v&m) printf("1"); else printf("0");
		m >>= 1;
	}
}

void ReadFont(FILE *fin)
{
	char b[128];

	int ch;

	int w = 0, h = 0;

	int ch_h = 0;

	bool open = false;

	printf("#include <stdint.h>\n");
	printf("#ifdef ARDUINO\n");
	printf("#include \"fonts.hpp\"\n");
	printf("#include <avr/pgmspace.h>\n");
	printf("#endif\n");
	printf("#ifndef PROGMEM\n");
	printf("#define PROGMEM\n");
	printf("#endif\n");
	printf("\n");

	while (fgets(b, sizeof(b), fin))
	{
		if (sscanf(b, "// w=%d h=%d", &w, &h) == 2)
		{
			if (open)
				printf("};\n\n");

			printf("const uint8_t font_%02dx%02d[] PROGMEM = {\n", w, h);
			printf("\t%d, %d,\n", w, h);
			open = true;
			continue;
		}

		if (sscanf(b, "// ch=%d", &ch) == 1)
		{
			continue;
		}

		if (b[0] == '.' || b[0] == 'X')
		{
			uint16_t r = 0;
			uint16_t m = 1;

			for (int i = w-1; i >= 0; --i)
			{
				if (b[i] == 'X')
					r |= m;
				m <<= 1;
			}


			if (false)
			{
				if (ch_h == 0) printf("\t");
				if (w <= 8)
					printf("0x%02x,", r);
				else
				{
					printf("0x%02x,", r>>8);
					printf("0x%02x,", r&0xff);
				}

				ch_h += 1;
				if (ch_h == h)
				{
					printf(" // %3d %c\n", ch, ch);
					ch_h = 0;
				}
			}
			else
			{
				if (w <= 8)
				{
					printf("\t"); print_b(r, 8); printf(","); 
					if (ch_h == 0) printf(" // %3d %c", ch, ch);
				}
				else
				{
					printf("\t"); 
					print_b(r >> 8, 8); printf(","); 
					print_b(r & 0xff, 8); printf(","); 
					if (ch_h == 0) printf(" // %3d %c", ch, ch);
				}
				printf("\n"); 
				ch_h += 1;
				if (ch_h == h) { ch_h = 0; printf("\n"); }
			}
		}
	}

	if (open)
		printf("};\n");
}

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
void PrintFont(const uint8_t *p)
{
	int w = p[0];
	int h = p[1];


	printf("// w=%d h=%d\n\n", w,h);

	p+=2;

	int nb = 1;
	if (w > 8) nb = 2;

	for (int c = ' '; c < 127; ++c)
	{
		int ci = c - ' ';

		printf("// ch=%d %c\n", c, c);
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
