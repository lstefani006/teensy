#include <Arduino.h>
#include <math.h>
#include <t_io.h>

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
	t::SetPrint(&Serial);
}


void blink(int msec)
{
	digitalWrite(led, HIGH);
	delay(msec);            
	digitalWrite(led, LOW); 
	delay(msec);            
}

double f = 0.0;

void loop() {
	blink(100);

	f += 1.1;
	if (f > 10) {
		blink(500);
	}
	printf(" -- %d\n", int(f));
	printf(" -- %f\n", f);
}
