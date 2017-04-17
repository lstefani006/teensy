#include <Arduino.h>

//////////////////////////////////////////////////////////////////
#include "SPI.h"
#include "OneWire.h"
#include "DallasTemperature.h"

#ifdef __MK20DX256__
#	include "ILI9341_t3.h"
#else
#	include "Adafruit_ILI9341.h"
#endif

// For the Adafruit shield, these are the default.
#define TFT_RESET 8
#define TFT_DC  9
#define TFT_CS 10

#ifdef __MK20DX256__
ILI9341_t3 tft(TFT_CS, TFT_DC, TFT_RESET);
#else
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RESET);
#endif

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

DeviceAddress insideThermometer;

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		if (deviceAddress[i] < 16) tft.print("0");
		tft.print(deviceAddress[i], HEX);
	}
}

void setup()
{
	tft.begin();

	tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
	tft.setTextSize(1);
	tft.println("Waiting for Arduino Serial Monitor...");

	Serial.begin(9600);
	while (!Serial) ; // wait for Arduino Serial Monitor
	Serial.println("ILI9341/Dallas Test!");

	sensors.begin();

	tft.print("Found ");
	tft.print(sensors.getDeviceCount(), DEC);
	tft.println(" devices.");

	// report parasite power requirements
	tft.print("Parasite power is: "); 
	if (sensors.isParasitePowerMode()) tft.println("ON");
	else tft.println("OFF");

	// Method 1:
	// Search for devices on the bus and assign based on an index. Ideally,
	// you would do this to initially discover addresses on the bus and then 
	// use those addresses and manually assign them (see above) once you know 
	// the devices on your bus (and assuming they don't change).
	if (!sensors.getAddress(insideThermometer, 0)) 
		tft.println("Unable to find address for Device 0"); 

	// show the addresses we found on the bus
	tft.print("Device 0 Address: ");
	printAddress(insideThermometer);
	tft.println();

	// set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
	sensors.setResolution(insideThermometer, 12);

	tft.print("Device 0 Resolution: ");
	tft.print(sensors.getResolution(insideThermometer), DEC); 
	tft.println();

	delay(500);
	tft.fillScreen(ILI9341_BLACK);
}

class Graph
{
public:
	Graph()
	{
	}
	Graph(int16_t x, int16_t y, int16_t w, int16_t h)
	{
		Set(x,y, w,h);
	}
	void Set(int16_t x, int16_t y, int16_t w, int16_t h)
	{
		_x = x;
		_y = y;
		_w = w;
		_h = h;
	}

	int16_t _x;
	int16_t _y;
	int16_t _w;
	int16_t _h;

	int16_t _tmin, _tmax, _tstep;
	int16_t _yc;

	void DrawScale()
	{
		_tmin = 0; // -10;
		_tmax = 30; // 50;
		_tstep = 5;
		_yc = _h / ((_tmax-_tmin)/_tstep) / _tstep;

		tft.fillRect(_x, _y, _w, _h, ILI9341_BLACK);

		dl(   0,    0, _w-1,    0, ILI9341_RED);
		dl(_w-1,    0, _w-1, _h-1, ILI9341_RED);
		dl(_w-1, _h-1, 0,    _h-1, ILI9341_RED);
		dl(   0, _h-1, 0,       0, ILI9341_RED);

		int16_t cy = 8;

		tft.setTextSize(1);

		for (int16_t t = _tmin; t <= _tmax; t+=_tstep)
		{
			tft.setCursor(2, yc(cy));
			tft.print(t);
			cy += _yc * _tstep;

			if (cy + 8 >= _h) break;
		}
		for (int16_t t = _tmin; t <= _tmax; t+=1)
		{
			int16_t cy = 8/2;
			int16_t y = (t - _tmin) * _yc;
			if (y+cy > _h) break;
			dl(6*3+4, y+cy, tft.width()-3, y+cy, ILI9341_BLUE);
		}
	}

	void DrawTemp(const int16_t t_buff[], int16_t t_top, int16_t t_len, int16_t campioni_per_ora)
	{
		int16_t x_min = 6*3+4;
		int16_t x_max = tft.width()-3;

		int16_t dx = (x_max-x_min)*64/t_len;

		int16_t ylast = 0;
		int16_t xlast = 0;

		for (int16_t i = 0; i < t_len; i += campioni_per_ora)
		{
			int16_t x = i * dx/64 + x_min;
			dl(x, 0, x, _h, ILI9341_BLUE);

			tft.setCursor(xc(x), yc(8));
			tft.print((i)/campioni_per_ora);
		}

		for (int16_t i = 0; i < t_top; ++i)
		{
			int16_t t = t_buff[i] / 10;
			int16_t cy = 8/2;
			int16_t y = (t - 10*_tmin) * _yc/10 + cy;

			int16_t x = i * dx/64 + x_min;

			if (i == 0)
				tft.drawPixel(xc(x), yc(y), ILI9341_WHITE);
			else
				tft.drawLine(xc(xlast), yc(ylast), xc(x), yc(y), ILI9341_WHITE);
			xlast = x;
			ylast = y;
		}
	}

	void dl(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color=ILI9341_YELLOW)
	{
		tft.drawLine(xc(x1), yc(y1), xc(x2), yc(y2), color);
	}
	int16_t xc(int16_t x) const { return _x + x; }
	int16_t yc(int16_t y) const { return _y + _h - y; }
};

Graph gr;

constexpr int t_len = 6*24;
int16_t t_top = 0;
int16_t t_buff[t_len];  // ogni 10 minuti per 24 ore.

void loop()
{
	static bool first = true;

	sensors.requestTemperatures(); // Send the command to get temperatures
	tft.drawRect(0, 0, 240, 8*4+4+4, ILI9341_RED);
	tft.setCursor((240 - (7*6*4))/2, 4+1);
	tft.setTextSize(4);

	int32_t t = (int32_t)sensors.getTemp(insideThermometer) * 100 / 128;
	if (t < 0) { t = -t; tft.print('-'); }
	tft.print(t/100);
	tft.print('.');
	int32_t cc = t%100;
	if (cc < 10) tft.print('0');
	tft.print(cc);
	tft.print(F(" C"));

	int16_t y = 8*4+4+4;
	if (/*first*/true)
	{
		gr.Set(0, y + 8, tft.width(), tft.height()-(y+8)-1);
		gr.DrawScale();
	}

	if (t_top == t_len)
	{
		for (int16_t i = 1; i < t_len; ++i)
			t_buff[i-1] = t_buff[i];
		t_top--;
	}
	t_buff[t_top++] = t;

	int16_t campioni_per_ora = 12;

	gr.DrawTemp(t_buff, t_top, t_len, campioni_per_ora);

	int16_t periodo_sec = 3600 / campioni_per_ora;
	delay(1000L * periodo_sec);

	first = false;
}
