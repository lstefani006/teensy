#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "font8x8.h"
#include "font6x8.h"


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

	/*
	if (ac > 0 && strcmp(av[1], "04x06") == 0) p = font_04x06;
	else if (ac > 0 && strcmp(av[1], "05x08") == 0) p = font_05x08;
	else if (ac > 0 && strcmp(av[1], "05x12") == 0) p = font_05x12;
	else if (ac > 0 && strcmp(av[1], "06x08") == 0) p = font_06x08;
	else if (ac > 0 && strcmp(av[1], "06x10") == 0) p = font_06x10;
	else if (ac > 0 && strcmp(av[1], "07x12") == 0) p = font_07x12;
	else if (ac > 0 && strcmp(av[1], "08x08") == 0) p = font_08x08;
	else if (ac > 0 && strcmp(av[1], "08x12") == 0) p = font_08x12;
	else if (ac > 0 && strcmp(av[1], "08x14") == 0) p = font_08x14;
	else if (ac > 0 && strcmp(av[1], "10x16") == 0) p = font_10x16;
	else if (ac > 0 && strcmp(av[1], "12x16") == 0) p = font_12x16;
	else if (ac > 0 && strcmp(av[1], "12x20") == 0) p = font_12x20;
	else if (ac > 0 && strcmp(av[1], "16x26") == 0) p = font_16x26;
	*/
	//p = font_8x8;
	p = font_6x8;

	int w = p[0];
	int h = p[1];
	p+=2;

	int nb = 1;
	if (w > 8) nb = 2;

	//for (int c = 'A'; c < 'E'; ++c)
	for (int c = 0; c < 256; ++c)
	{
		int ci = c /*- ' '*/;

		printf("%d ", c);
		if (c >= 32 && c < 127) printf("%c", c);
		printf("\n");

		for (int i = 0; i < h; ++i)
		{
			if (nb == 1)
			{
				printBin(p[ci*h*nb + nb*i+0], 0);
				//printBin(p[ci*h*nb + nb*i+0], 8-w);
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
