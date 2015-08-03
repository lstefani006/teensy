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

int nn = 0;

double f = 0.0;

void loop() {
	printf("sizeof(float)=%d\n", sizeof(float));
	printf("sizeof(double)=%d\n", sizeof(double));
	printf("%d\n", nn);
	blink(100);
	if (++nn < 20) return;

	f += 1.1;
	if (f > 10) {
		blink(500);
	}
	printf(" -- %d\n", int(f));
	printf(" -- %f\n", f);
}
