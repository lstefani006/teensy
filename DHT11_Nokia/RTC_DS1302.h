#ifndef _DS1302_H_
#define _DS1302_H_

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <Time.h>

class DS1302
{
public:
	DS1302(int8_t DS1302_SCLK_PIN, int8_t DS1302_IO_PIN, int8_t DS1302_CE_PIN);

	void begin();

	void GetTime();

	void SetTime(int16_t Y, int8_t M, int8_t D, int8_t h24, int8_t m, int8_t s, int8_t wd);

	int8_t  h24() const { return _h; }  // 0..23
	int8_t  m()   const { return _m; }  // 0..59
	int8_t  s()   const { return _s; }  // 0..59

	int16_t Y()   const { return _Y; }  // 2013
	int8_t  M()   const { return _M; }  // 1..12
	int8_t  D()   const { return _D; }  // 1..31

	int8_t  WD()  const { return _WD; } // 1..7

	operator time_t()
	{
		tmElements_t tm;
		tm.Year   = this->Y()-1970;
		tm.Month  = this->M();
		tm.Day    = this->D();
		tm.Hour   = this->h24();
		tm.Minute = this->m();
		tm.Second = this->s();
		tm.Wday   = this->WD();
		return makeTime(tm);
	}

	void SetTime(time_t t)
	{
		tmElements_t tm;
		breakTime(t, tm);
		SetTime(tm.Year + 1970, tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second, tm.Wday);
	}


private:
	int8_t _DS1302_SCLK_PIN;
	int8_t _DS1302_IO_PIN;
	int8_t _DS1302_CE_PIN;

	int8_t _h;
	int8_t _m;
	int8_t _s;

	int16_t _Y;
	int8_t _M;
	int8_t _D;

	int8_t _WD;


	void    DS1302_start();
	void    DS1302_clock_burst_read(uint8_t *p);
	void    DS1302_clock_burst_write(uint8_t *p);
	uint8_t DS1302_read(int address);
	void    DS1302_write(int address, uint8_t data);
	void    DS1302_stop();
	uint8_t DS1302_toggleread();
	void    DS1302_togglewrite(uint8_t data, uint8_t release);
};

#endif
