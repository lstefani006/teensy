﻿#include <Arduino.h>
#include "t_io.h"

//////////////////////////////////////////////////////////////////
#include "SPI.h"

#include "ILI9341_t3.h"

//Function to send and receive data for both master and slave
uint8_t spi_tranceiver(uint8_t data) __attribute__((always_inline));
uint8_t spi_tranceiver(uint8_t data)
{
	// Load data into the buffer
	SPDR = data;

	//Wait until transmission complete
	while(!(SPSR & (1<<SPIF) ));

	// Return received data
	return SPDR;
}
void spi_send(uint8_t data) __attribute__((always_inline));
void spi_send(uint8_t data)
{
	SPDR = data;
}


// For the Adafruit shield, these are the default.
#define TFT_RESET 8
#define TFT_DC  9
#define TFT_CS 10

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
ILI9341_t3 tft(TFT_CS, TFT_DC, TFT_RESET);

void start();

const uint8_t w = 50;
const uint8_t h = 30;
const uint8_t d = 5;

void setup()
{
	tft.begin();
	tft.fillScreen(ILI9341_BLACK);
	tft.setTextColor(ILI9341_YELLOW);
	tft.setTextSize(1);
	tft.println("Waiting for Arduino Serial Monitor...");

	Serial.begin(9600);
	while (!Serial) ; // wait for Arduino Serial Monitor
	Serial.println("ILI9341 Test!");

	tft.fillScreen(ILI9341_BLACK);
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void loop()
{
#ifdef TEENSY
	tft.setupScroll(ILI9341_RED, ILI9341_BLUE);

	for (int i = 0; i < 245; ++i)
	{
		tft.print("Hello World! = ");
		tft.println(i);
	}
	delay(5000);
	tft.resetScroll(ILI9341_BLACK);
#endif

	start();
}
unsigned long testFillScreen()
{
	unsigned long start = micros();
	tft.fillScreen(ILI9341_BLACK);
	tft.fillScreen(ILI9341_RED);
	tft.fillScreen(ILI9341_GREEN);
	tft.fillScreen(ILI9341_BLUE);
	tft.fillScreen(ILI9341_BLACK);
	return micros() - start;
}

unsigned long testText()
{
	tft.fillScreen(ILI9341_BLACK);
	unsigned long start = micros();
	tft.setCursor(0, 0);
	tft.setTextColor(ILI9341_WHITE);
	tft.setTextSize(1);
	tft.println("Hello World!");
	tft.setTextColor(ILI9341_YELLOW);
	tft.setTextSize(2);
	tft.println(1234.56);
	tft.setTextColor(ILI9341_RED);
	tft.setTextSize(3);
	tft.println(0xDEADBEEF, HEX);
	tft.println();
	tft.setTextColor(ILI9341_GREEN);
	tft.setTextSize(5);
	tft.println("Groop");
	tft.setTextSize(2);
	tft.println("I implore thee,");
	tft.setTextSize(1);
	tft.println("my foonting turlingdromes.");
	tft.println("And hooptiously drangle me");
	tft.println("with crinkly bindlewurdles,");
	tft.println("Or I will rend thee");
	tft.println("in the gobberwarts");
	tft.println("with my blurglecruncheon,");
	tft.println("see if I don't!");
	return micros() - start;
}

unsigned long testLines(uint16_t color)
{
	unsigned long start, t;
	int           x1, y1, x2, y2,
				  w = tft.width(),
				  h = tft.height();

	tft.fillScreen(ILI9341_BLACK);

	x1 = y1 = 0;
	y2 = h - 1;
	start = micros();
	for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
	x2 = w - 1;
	for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);
	t     = micros() - start; // fillScreen doesn't count against timing

	tft.fillScreen(ILI9341_BLACK);

	x1 = w - 1;
	y1 = 0;
	y2 = h - 1;
	start = micros();
	for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
	x2    = 0;
	for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);
	t += micros() - start;

	tft.fillScreen(ILI9341_BLACK);

	x1 = 0;
	y1 = h - 1;
	y2 = 0;
	start = micros();
	for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
	x2    = w - 1;
	for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);
	t += micros() - start;

	tft.fillScreen(ILI9341_BLACK);

	x1 = w - 1;
	y1 = h - 1;
	y2 = 0;
	start = micros();
	for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
	x2    = 0;
	for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);

	return micros() - start;
}

unsigned long testFastLines(uint16_t color1, uint16_t color2)
{
	unsigned long start;
	int           x, y, w = tft.width(), h = tft.height();

	tft.fillScreen(ILI9341_BLACK);
	start = micros();
	for (y = 0; y < h; y += 5) tft.drawFastHLine(0, y, w, color1);
	for (x = 0; x < w; x += 5) tft.drawFastVLine(x, 0, h, color2);

	return micros() - start;
}

unsigned long testRects(uint16_t color)
{
	unsigned long start;
	int           n, i, i2,
				  cx = tft.width()  / 2,
				  cy = tft.height() / 2;

	tft.fillScreen(ILI9341_BLACK);
	n = min(tft.width(), tft.height());
	start = micros();
	for (i = 2; i < n; i += 6)
	{
		i2 = i / 2;
		tft.drawRect(cx - i2, cy - i2, i, i, color);
	}

	return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2)
{
	unsigned long start, t = 0;
	int           n, i, i2,
				  cx = tft.width()  / 2 - 1,
				  cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9341_BLACK);
	n = min(tft.width(), tft.height());
	for (i = n; i > 0; i -= 6)
	{
		i2    = i / 2;
		start = micros();
		tft.fillRect(cx - i2, cy - i2, i, i, color1);
		t    += micros() - start;
		// Outlines are not included in timing results
		tft.drawRect(cx - i2, cy - i2, i, i, color2);
	}

	return t;
}

unsigned long testFilledCircles(uint8_t radius, uint16_t color)
{
	unsigned long start;
	int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

	tft.fillScreen(ILI9341_BLACK);
	start = micros();
	for (x = radius; x < w; x += r2)
	{
		for (y = radius; y < h; y += r2)
		{
			tft.fillCircle(x, y, radius, color);
		}
	}

	return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color)
{
	unsigned long start;
	int           x, y, r2 = radius * 2,
				  w = tft.width()  + radius,
				  h = tft.height() + radius;

	// Screen is not cleared for this one -- this is
	// intentional and does not affect the reported time.
	start = micros();
	for (x = 0; x < w; x += r2)
	{
		for (y = 0; y < h; y += r2)
		{
			tft.drawCircle(x, y, radius, color);
		}
	}

	return micros() - start;
}

unsigned long testTriangles()
{
	unsigned long start;
	int           n, i, cx = tft.width()  / 2 - 1,
				  cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9341_BLACK);
	n     = min(cx, cy);
	start = micros();
	for (i = 0; i < n; i += 5)
	{
		tft.drawTriangle(
				cx    , cy - i, // peak
				cx - i, cy + i, // bottom left
				cx + i, cy + i, // bottom right
				tft.color565(0, 0, i));
	}

	return micros() - start;
}

unsigned long testFilledTriangles()
{
	unsigned long start, t = 0;
	int           i, cx = tft.width()  / 2 - 1,
				  cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9341_BLACK);
	start = micros();
	for (i = min(cx, cy); i > 10; i -= 5)
	{
		start = micros();
		tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
				tft.color565(0, i, i));
		t += micros() - start;
		tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
				tft.color565(i, i, 0));
	}

	return t;
}

unsigned long testRoundRects()
{
	unsigned long start;
	int           w, i, i2,
				  cx = tft.width()  / 2 - 1,
				  cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9341_BLACK);
	w     = min(tft.width(), tft.height());
	start = micros();
	for (i = 0; i < w; i += 6)
	{
		i2 = i / 2;
		tft.drawRoundRect(cx - i2, cy - i2, i, i, i / 8, tft.color565(i, 0, 0));
	}

	return micros() - start;
}

unsigned long testFilledRoundRects()
{
	unsigned long start;
	int           i, i2,
				  cx = tft.width()  / 2 - 1,
				  cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9341_BLACK);
	start = micros();
	for (i = min(tft.width(), tft.height()); i > 20; i -= 6)
	{
		i2 = i / 2;
		tft.fillRoundRect(cx - i2, cy - i2, i, i, i / 8, tft.color565(0, i, 0));
	}

	return micros() - start;
}


// 320x240
void start()
{
	tft.drawLine(0, 0, 240/2, 0, ILI9341_RED);
	tft.drawLine(0, 0, 0, 320/2, ILI9341_RED);
	delay(2000);

	// read diagnostics (optional but can help debug problems)
	uint8_t x = tft.readcommand8(ILI9341_RDMODE);
	Serial.print("Display Power Mode: 0x");
	Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDMADCTL);
	Serial.print("MADCTL Mode: 0x");
	Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDPIXFMT);
	Serial.print("Pixel Format: 0x");
	Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDIMGFMT);
	Serial.print("Image Format: 0x");
	Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDSELFDIAG);
	Serial.print("Self Diagnostic: 0x");
	Serial.println(x, HEX);
	Serial.println(F("Benchmark                Time (microseconds)"));
	delay(2000);

	Serial.print(F("Screen fill              "));
	Serial.println(testFillScreen());
	delay(200);

	Serial.print(F("Text                     "));
	Serial.println(testText());
	delay(2000);

	Serial.print(F("Lines                    "));
	Serial.println(testLines(ILI9341_CYAN));
	delay(200);

	Serial.print(F("Horiz/Vert Lines         "));
	Serial.println(testFastLines(ILI9341_RED, ILI9341_BLUE));
	delay(200);

	Serial.print(F("Rectangles (outline)     "));
	Serial.println(testRects(ILI9341_GREEN));
	delay(200);

	Serial.print(F("Rectangles (filled)      "));
	Serial.println(testFilledRects(ILI9341_YELLOW, ILI9341_MAGENTA));
	delay(200);

	Serial.print(F("Circles (filled)         "));
	Serial.println(testFilledCircles(10, ILI9341_MAGENTA));

	Serial.print(F("Circles (outline)        "));
	Serial.println(testCircles(10, ILI9341_WHITE));
	delay(200);

	Serial.print(F("Triangles (outline)      "));
	Serial.println(testTriangles());
	delay(200);

	Serial.print(F("Triangles (filled)       "));
	Serial.println(testFilledTriangles());
	delay(200);

	Serial.print(F("Rounded rects (outline)  "));
	Serial.println(testRoundRects());
	delay(200);

	Serial.print(F("Rounded rects (filled)   "));
	Serial.println(testFilledRoundRects());
	delay(200);

	Serial.println(F("Done!"));
}

#if 0
void show_img()
{
	// tft.fillScreen(ILI9341_BLACK);
	tft.setRotation(3);
	tft.drawImg((const uint16_t *)gimp_image.pixel_data);
	tft.setRotation(0);
	return;

	for (int r = 0; r < 1; r++)
	{
		tft.setRotation(3);
		for (int y = 0; y < (int)gimp_image.height; ++y)
			for (int x = 0; x < (int)gimp_image.width; ++x)
			{
				unsigned a = gimp_image.pixel_data[y * gimp_image.width * 2 + 2 * x + 0];
				unsigned b = gimp_image.pixel_data[y * gimp_image.width * 2 + 2 * x + 1];
				tft.drawPixel(x, y, b << 8 | a);
			}
	}
}
#endif

