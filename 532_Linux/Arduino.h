#ifndef __Arduino_h__
#define __Arduino_h__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

class HardwareSerial
{
public:
	void begin(int);
	void write(uint8_t b);
	void write(const uint8_t *b, int sz);
	uint8_t read();
	bool available();
};

extern HardwareSerial Serial1;

enum PrintInt { HEX, DEC };

class SerialClass {
public:
	void begin(int) {}
	void print(const char *str);
	void println(const char *str);

	void print(uint8_t);
	void print(int);
	void print(uint32_t);
	void print(char);

	void print(int n, PrintInt);
	void println(int n, PrintInt);
};
extern SerialClass Serial;

typedef bool boolean;

void delay(int msec);
int millis();

#endif // __Arduino_h__
