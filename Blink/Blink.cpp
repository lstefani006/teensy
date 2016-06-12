#include <Arduino.h>
#include <math.h>
#include <t_io.h>

extern "C" {
	extern char *__brkval;
	/*
	void *_sbrk(int incr) {

		return nullptr;
	}
	*/
}


// Pin 13 has an LED connected on most Arduino boards.
// Pin 11 has the LED on Teensy 2.0
// Pin 6  has the LED on Teensy++ 2.0
// Pin 13 has the LED on Teensy 3.0
int led = 13;

// the setup routine runs once when you press reset:
void setup()
{
	pinMode(led, OUTPUT);
	Serial.begin(9600);
	while (!Serial) ; // wait for Arduino Serial Monitor
	t::SetPrint(&Serial);
}

void blink(int msec)
{
	digitalWrite(led, HIGH);
	delay(msec);
	digitalWrite(led, LOW);
	delay(msec);
}

int n = 0;
float f = 0;

void loop() {
	blink(100);
	printf("ciao %d %f\n", n++, f++);
	if (f > 10)
	{
		if (n&1)
			blink(200);
		else
			blink(1000);
	}
}
