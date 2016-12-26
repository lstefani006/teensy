#include <Arduino.h>
#include <math.h>
#include <DallasTemperature.h>
#include "SPI.h"
#include <Adafruit_GFX_AS.h>    // Core graphics library, with extra fonts.
#include <Adafruit_ILI9341_STM.h> // STM32 DMA Hardware-specific library
#include <uprintf.hpp>

// For the Adafruit shield, these are the default.
#define TFT_CS         8                  
#define TFT_DC         10                
#define TFT_RST        9 

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341_STM g_gr(TFT_CS, TFT_DC, TFT_RST);
bool g_use_gr = false;

OneWire g_ow(PA0);
DallasTemperature g_dt(&g_ow);
DeviceAddress g_da[1];


// Pin 13 has an LED connected on most Arduino boards.
// Pin 11 has the LED on Teensy 2.0
// Pin 6  has the LED on Teensy++ 2.0
// Pin 13 has the LED on Teensy 3.0
constexpr int led = PC13;

void printAddress(DeviceAddress deviceAddress)
{
	for (auto i = 0; i < 8; i++)
		uprintf("%02x", deviceAddress[i]);
}


void setup()
{
	delay(1000*10);
	pinMode(led, OUTPUT);

	Serial.begin(38400);
	uprintf_cb = [](char c) -> bool { Serial.print(c); return true; };

	uprintf("Staring\nLed=%d\n", led);
	delay(2000);

	g_dt.begin();
	uprintf("Found %d devices\n",g_dt.getDeviceCount());

	if (g_dt.getDeviceCount() == 0)
	{
		uprintf("No device found");
		return;
	}

	// report parasite power requirements
	uprintf("Parasite power is: "); 
	if (g_dt.isParasitePowerMode()) 
		uprintf("ON\n");
	else 
		uprintf("OFF\n");

	if (!g_dt.getAddress(g_da[0], 0)) 
		uprintf("Unable to find address for Device 0\n"); 
	else
	{
		// show the addresses we found on the bus
		uprintf("Device 0 Address: ");
		printAddress(g_da[0]);
		uprintf("\n");

		// set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
		g_dt.setResolution(g_da[0], 12);
		uprintf("Device 0 Resolution: %d\n", g_dt.getResolution(g_da[0])); 
	}


	if (g_use_gr)
	{
		g_gr.begin();
		g_gr.fillScreen(ILI9341_BLACK);
	}
}

constexpr int msec = 300;

void loop()
{
	digitalWrite(led, HIGH);
	delay(msec);
	digitalWrite(led, LOW);
	delay(msec);

	if (g_dt.getDeviceCount() > 0)
	{
		uprintf("Requesting temperatures...");
		g_dt.requestTemperatures(); // Send the command to get temperatures
		uprintf("DONE\n");

		// It responds almost immediately. Let's print out the data
		auto tempC = g_dt.getTempC(g_da[0]);
		if (tempC == DEVICE_DISCONNECTED_C)
			uprintf("Device disconneted\n");
		else
			uprintf("Temp C: %f\n", tempC);
	}
}
