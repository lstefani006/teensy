#include <Arduino.h>
#include "RTC_DS1302.h"

const int led = 13;
void blink(int msec) {
	digitalWrite(led, HIGH);
	delay(msec);           
	digitalWrite(led, LOW);
	delay(msec);           
}

int8_t DS1302_SCLK_PIN = 19;
int8_t DS1302_IO_PIN = 18;
int8_t DS1302_CE_PIN = 17;

DS1302 rtc(DS1302_SCLK_PIN, DS1302_IO_PIN, DS1302_CE_PIN);

int main()
{
	Serial.begin(9600);
	rtc.begin();

	pinMode(led, OUTPUT);     

	printf("Start\n");
	for (int i = 0; i < 3; ++i)
	{
		blink(500);
		printf("%d\n", i);
	}

	rtc.SetTime(2014, 11, 25, 21, 45, 0, 2);
	for (;;)
	{
		rtc.GetTime();

		printf("%04d/%02d/%02d %02d:%02d:%02d %d",
				rtc.Y(), rtc.M(), rtc.D(), 
				rtc.h24(), rtc.m(), rtc.s(),
				rtc.WD()); 
		printf("\n");

		blink(200);
	}
}

