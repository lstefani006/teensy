#include <Arduino.h>
#include "leo.h"
#include <string.h>

int len(const char *p)
{
	auto e = p;
	while (*p)
		p += 1;
	return p - e;
}

void setup()
{
	Serial.begin(9600);
}

auto n = 0;

void loop()
{
	delay(200);
	Serial.print("N=>");
	Serial.println(n);
	n++;
}
