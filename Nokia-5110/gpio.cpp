
#include <kinetis.h> 

/*
GPIOA_xyz
GPIOB_xyz
GPIOC_xyz
GPIOD_xyz

PDDR: Port Data Direction Register: 
      0 means this bit/pin is a input, 1 means output.

PDIR: Port Data Input Register: 
      0/1 according to the pin's state.
      (If any pin is an output it can still be read by PDIR and 
	  will simply read back the same value last output on that pin).

PDOR: Port Data Output Register: 
      0 sets the bit/pin to 0, 1 to 1.

The following three sub-registers are logically redundant, 
but save an instruction or two in some cases.

PSOR: Port Set Output Register: 0 has no effect. 
      1 sets the corresponding bit/pin to 1.

PCOR: Port Clear Output Register: 0 has no effect. 
      1 sets the corresponding bit/pin to 0.

PTOR: Port Toggle Output Register: 0 has no effect. 
      1 toggles the corresponding bit/pin.

Teensy3 pin / GPIOx / bit in GPIOx
 0 B 16
 1 B 17
 2 D  0
 3 A 12
 4 A 13
 5 D  7
 6 D  4
 7 D  2
 8 D  3
 9 C  3
10 C  4
11 C  6
12 C  7
13 C  5
14 D  1
15 C  0
16 B  0
17 B  1
18 B  3
19 B  2
20 D  5
21 D  6
22 C  1
23 C  2
24 A  5
25 B 19
26 E  1
27 C  9
28 C  8
29 C 10
30 C 11
31 E  0
32 B 18
33 A  4
*/

class GPIOA {
public:
	enum Direction { In=0, Out=1 };
	static void SetDirection(int pin, Direction dir) {
		if (dir == In)
			GPIOA_PDDR &= ~(1 << pin);
		else
			GPIOA_PDDR |= (1 << pin);
	}
	static void Write(int pin, bool v) {
		if (v)
			GPIOA_PSOR = (1 << pin);
		else
			GPIOA_PCOR = (1 << pin);
	}
	static bool Read(int pin) {
		return (GPIOA_PDIR & (1 << pin)) ? true : false;
	}
};

bool Trest() {
	GPIOA::SetDirection(3, GPIOA::Out);
	GPIOA::Write(3, true);
	GPIOA::Write(3, false);
	GPIOA::Write(3, true);
	GPIOA::Write(3, false);
	return GPIOA::Read(3);
}
