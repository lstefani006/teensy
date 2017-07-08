#include <Arduino.h>
#include <math.h>

// Pin 13 has an LED connected on most Arduino boards.
// Pin 11 has the LED on Teensy 2.0
// Pin 6  has the LED on Teensy++ 2.0
// Pin 13 has the LED on Teensy 3.0
#ifdef ARDUINO
int led = PC13;
#else
int led = PC13; 
#endif

// the setup routine runs once when you press reset:
void setup()
{
	pinMode(led, OUTPUT);
	Serial.begin(38400);
	/*
	while (!Serial) ; // wait for Arduino Serial Monitor
	Serial.println("Ciao");
	*/
}

void blink(int msec)
{
	digitalWrite(led, HIGH);
	delay(msec);
	digitalWrite(led, LOW);
	delay(msec);
}

float f = 0;
void loop()
{
	/*
	auto p = new char [100];
	delete []p;
	*/
	blink(100);

	/*
	f += 0.1f;
	if (f > 12)
		f = 0;

	Serial.println("Ciao");
	*/
	/*
	f += 0.1;
	if (f > 5)
		Serial.println("f >");
		*/
}

/*

void loop() {
	blink(100);
	blink(100);
	blink(1000);
	blink(500);
	f += 1.1 + sin(f);

	if (f > 30)
	blink(1000);
}
*/
