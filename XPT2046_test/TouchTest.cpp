#include <Arduino.h>
#include "Adafruit_ILI9341.h"

#define TS 2
#if TS == 1
#include <XPT2046.h>
#else
#include <XPT2046_Touchscreen.h>
#endif
#include <SPI.h>
#include <calibrate.h>

#define CS_PIN  5
// MOSI=11, MISO=12, SCK=13

//XPT2046_Touchscreen ts(CS_PIN);
#define TIRQ_PIN  3


#if TS == 1
//XPT2046_Touchscreen ts(CS_PIN);          // Param 2 - NULL - No interrupts
//XPT2046_Touchscreen ts(CS_PIN, 255);     // Param 2 - 255 - No interrupts
XPT2046 ts(CS_PIN, TIRQ_PIN);  // Param 2 - Touch IRQ Pin - interrupt enabled polling
#else
XPT2046_Touchscreen ts(CS_PIN, TIRQ_PIN);     // Param 2 - 255 - No interrupts
#endif


// For the Adafruit shield, these are the default.
#define TFT_RESET 8
#define TFT_DC  9
#define TFT_CS 10
//
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RESET);

MATRIX matrix;

const int8_t Delta = 20;

POINT display[3] =
{
	Delta,     Delta,
	240-Delta, 320-Delta,
	Delta,     320-Delta
};
POINT touch[3];


int32_t colors[] = {
	ILI9341_BLACK,
	ILI9341_BLUE,
	ILI9341_RED,
	ILI9341_GREEN,
	ILI9341_CYAN,
	ILI9341_MAGENTA,
	ILI9341_YELLOW,
	ILI9341_WHITE
};
void cls()
{
	tft.fillScreen(ILI9341_BLACK);
	int d=20;
	for (int c = 0; c < int(sizeof(colors)/sizeof(colors[0])); c++)
	{
		tft.fillRect(40+d*c, 0, d, d, colors[c]);
		tft.drawRect(40+d*c, 0, d, d, ILI9341_WHITE);
	}
	tft.fillRect(0, 0, d, d, ILI9341_WHITE);
}

void setup() {
	Serial.begin(9600);
	ts.begin();
	while (!Serial && (millis() <= 1000));
	Serial.println("Start");

	tft.begin();
	cls();

	for (int8_t i = 0; i < 3; ++i)
		tft.drawPixel(display[i].x, display[i].y, ILI9341_WHITE);
	matrix.An = 5800;
	matrix.Bn = -147800;
	matrix.Cn = 132322200;
	matrix.Dn = 200480;
	matrix.En = -7280;
	matrix.Fn = -10061320;
	matrix.Divider = 528370;


	matrix.An = -5600;
	matrix.Bn = -144800;
	matrix.Cn = 135157600;
	matrix.Dn = 193200;
	matrix.En = 5600;
	matrix.Fn = -17247200;
	matrix.Divider = 499000;
}

#if TS == 1
void loop() {
	if (ts.isTouching()) {
		uint16_t x, y;
		ts.getPosition(x, y);
		Serial.print(x);
		Serial.print("/");
		Serial.println(y);
		delay(20);
	}
}
#else

int8_t nn = 4;


void loop() 
{
	if (nn < 3)
	{
		if (ts.touched()) {
			TS_Point p = ts.getPoint();
			touch[nn].x = p.x/4;
			touch[nn].y = p.y/4;
			nn += 1;
			Serial.print(p.x);
			Serial.print("/");
			Serial.println(p.y);
			delay(1*1000);
			Serial.println(nn);
		}
	}
	else if (nn == 3)
	{
		setCalibrationMatrix(display, touch, &matrix);
		Serial.println("Matrix");
		Serial.print("matrix.An = "); Serial.print(matrix.An); Serial.println(";");
		Serial.print("matrix.Bn = "); Serial.print(matrix.Bn); Serial.println(";");
		Serial.print("matrix.Cn = "); Serial.print(matrix.Cn); Serial.println(";");
		Serial.print("matrix.Dn = "); Serial.print(matrix.Dn); Serial.println(";");
		Serial.print("matrix.En = "); Serial.print(matrix.En); Serial.println(";");
		Serial.print("matrix.Fn = "); Serial.print(matrix.Fn); Serial.println(";");
		Serial.print("matrix.Divider = "); Serial.print(matrix.Divider); Serial.println(";");

		nn+=1;
	}
	else
	{
		static int32_t col = ILI9341_RED;

		if (ts.touched()) 
		{
			TS_Point p = ts.getPoint();
			POINT touch;
			touch.x = p.x/4;
			touch.y = p.y/4;
			POINT display;
			getDisplayPoint(&display, &touch, &matrix);


			bool found = false;

			for (int c = 0; c < int(sizeof(colors)/sizeof(colors[0])); c++)
			{
				int16_t d=20;
				int16_t xa = 40+d*c;
				int16_t xb = 40+d*c + d;

				int16_t ya = 0;
				int16_t yb = 0 + d;

				if (display.x >= xa && display.x <= xb && display.y >= ya && display.y <= yb)
				{
					col = colors[c];
					tft.fillRect(0, 0, d, d, col);
					tft.drawRect(0, 0, d, d, ILI9341_WHITE);
					found = true;

					if (col == ILI9341_BLACK) {
						cls();
						col = ILI9341_WHITE;

					}
					break;
				}
			}

			if (found == false)
			tft.drawPixel(display.x, display.y, col);

		}
	}
}
#endif
