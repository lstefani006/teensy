#include <Arduino.h>
#include <math.h>


// Pin 13 has an LED connected on most Arduino boards.
// Pin 11 has the LED on Teensy 2.0
// Pin 6  has the LED on Teensy++ 2.0
// Pin 13 has the LED on Teensy 3.0
int led = 13;

// the setup routine runs once when you press reset:
void setup()
{
	pinMode(led, OUTPUT);
	/*
	Serial.begin(38400);
	while (!Serial) ; // wait for Arduino Serial Monitor
	Serial.println("Ciao");
	*/
}

float f = 0;

void blink(int msec)
{
	digitalWrite(led, HIGH);
	delay(msec);
	digitalWrite(led, LOW);
	delay(msec);
}

void loop() {
	blink(100);
	blink(100);
	blink(1000);
	blink(500);
	f += 1.1 + sin(f);

	if (f > 30)
	blink(1000);
}
