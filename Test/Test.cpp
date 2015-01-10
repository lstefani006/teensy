#include <Arduino.h>
#include <math.h>
#include <vector>
#include "t_io.h"


// Pin 13 has an LED connected on most Arduino boards.
// Pin 11 has the LED on Teensy 2.0
// Pin 6  has the LED on Teensy++ 2.0
// Pin 13 has the LED on Teensy 3.0
int led = 13;

// the setup routine runs once when you press reset:
void setup() {                
	pinMode(led, OUTPUT);     
	Serial.begin(9600);
	t::SetPrint(&Serial);
	delay(3000);
	printf("Test gcc/newlib\n");
	delay(1000);
}


void blink(int msec)
{
	digitalWrite(led, HIGH);
	delay(msec);            
	digitalWrite(led, LOW); 
	delay(msec);            
}

int test = 0;
bool fail = false;

void T(bool v) {
	if (v)
		printf("test %d OK\n", test);
	else
	{
		printf("test %d FAILED\n", test);
		fail = true;
	}
}


float f = 0;
bool t1() {
	f += 3.14f;
	if (f > 6.2f)
		return true;
	return false;
}
double d = 0;
bool t2() {
	d += 3.14;
	if (d > 6.2)
		return true;
	return false;
}

void loop() {
	blink(100);
	switch (test)
	{
		default:
			{
				static bool first = true;
				if (first)
				{
					if (fail)
						printf("Test failed !!!\n");
					else
						printf("Test ALL OK !!!\n");
					first = false;
				}
				blink(100);
			}
			return;

		case 0:
			{
				int n = strlen("leo");
				T(n == 3);
			}
			break;

		case 1: T(t1() == false); break;
		case 2: T(t1() == true); break;
		case 3: T(t2() == false); break;
		case 4: T(t2() == true); break;

		case 5:
				{
					std::vector<int> v;
					int k = 0;
					for (auto i = 0; i < 1000; ++i) {
						v.push_back(i);
						k += i;
					}

					for (auto i : v) {
						k -= i;
					}
					T(k == 0);
				}
				break;
		case 6:
				{
					std::vector<int> v;
					int k = 0;
					for (auto i = 0; i < 2000; ++i) {
						v.push_back(i);
						k += i;
					}

					for (auto i : v) {
						k -= i;
					}
					T(k == 0);
				}
				break;
				
		case 7:
				{
					for (auto i = 0; i < 2; ++i) {
						char *s = (char *)malloc(1024);
						int sz = 1024;
						for (;;) {
							char *p = (char *)malloc(sz);
							if (p == nullptr) break;

							int k = 0;
							for (auto i = 0; i < sz; ++i) { p[i] = i % 100; k += p[i]; }
							for (auto i = 0; i < sz; ++i) { k -= p[i]; }

							if (k != 0) {
								free(s);
								T(false);
								break;
							}
							free(p);
							sz += 1024;
							printf("malloc(%d)\n", sz);
						}
						printf("max malloc(%d)\n", sz);
					}
				}
				break;

	}
	delay(500);
	test += 1;
}
