
#include <Arduino.h>

// Timer2 supporta solo overflow 0xff=>0x00 scatta l'interrupt
class Timer2
{
public:
	uint8_t _cnt;

	void setup_overflow(uint32_t microSec)
	{
		auto sreg = SREG;
		cli();

		TCCR2A = 0; // COM2A1 COM2A0 COM2B1 COM2B0     –    – WGM21 WGM20
		TCCR2B = 0; // FOC2A   FOC2B      –      – WGM22 CS22  CS21  CS20

		uint32_t cy = F_CPU / (1000ul*1000ul /* * 2 */) * microSec;
		constexpr uint32_t m = 256; // risoluzione del timer
		uint32_t cnt;
		if (cy <= m * 1)
		{
			TCCR2B |= _BV(CS20);
			cnt = cy;
		}
		else if (cy <= m * 8)
		{
			TCCR2B |= _BV(CS21);
			cnt = cy / 8;
		}
		else if (cy <= m * 32)
		{
			TCCR2B |= _BV(CS21) | _BV(CS20);
			cnt = cy / 32;
		}
		else if (cy <= m * 64)
		{
			TCCR2B |= _BV(CS22);
			cnt = cy / 64;
		}
		else if (cy <= m * 128)
		{
			TCCR2B |= _BV(CS22) | _BV(CS20);
			cnt = cy / 128;
		}
		else if (cy <= m * 256)
		{
			TCCR2B |= _BV(CS22) | _BV(CS21);
			cnt = cy / 256;
		}
		else if (cy <= m * 1024)
		{
			TCCR2B |= _BV(CS22) | _BV(CS21) | _BV(CS20);
			cnt = cy / 1024;
		}
		else
		{
			TCCR2B |= _BV(CS22) | _BV(CS21) | _BV(CS20);
			cnt = 0xff;
		}
	

		// cnt = F_CPU / prescaler * microSec / (1000*1000);
		if (cnt > 0xff) cnt = 0xff;
		_cnt = 0xff - cnt;
		TCNT2 = _cnt;

		// enable Timer2 overflow interrupt:
		TIMSK2 |= _BV(TOIE2);

		SREG = sreg;
	}


	void setup_ctc(uint32_t microSec)
	{
		auto sreg = SREG;
		cli();

		TCCR2A = 0; // COM2A1 COM2A0 COM2B1 COM2B0     –    – WGM21 WGM20
		TCCR2B = 0; // FOC2A   FOC2B      –      – WGM22 CS22  CS21  CS20

		uint32_t cy = F_CPU / (1000ul*1000ul /* * 2ul*/) * microSec;
		constexpr uint32_t m = 256; // risoluzione del timer
		uint32_t cnt;
		if (cy <= m * 1)
		{
			TCCR2B |= _BV(CS20);
			cnt = cy;
		}
		else if (cy <= m * 8)
		{
			TCCR2B |= _BV(CS21);
			cnt = cy / 8;
		}
		else if (cy <= m * 32)
		{
			TCCR2B |= _BV(CS21) | _BV(CS20);
			cnt = cy / 32;
		}
		else if (cy <= m * 64)
		{
			TCCR2B |= _BV(CS22);
			cnt = cy / 64;
		}
		else if (cy <= m * 128)
		{
			TCCR2B |= _BV(CS22) | _BV(CS20);
			cnt = cy / 128;
		}
		else if (cy <= m * 256)
		{
			TCCR2B |= _BV(CS22) | _BV(CS21);
			cnt = cy / 256;
		}
		else if (cy <= m * 1024)
		{
			TCCR2B |= _BV(CS22) | _BV(CS21) | _BV(CS20);
			cnt = cy / 1024;
		}
		else
		{
			TCCR2B |= _BV(CS22) | _BV(CS20);
			cnt = 0xff;
		}

		// cnt = F_CPU / prescaler * microSec / (1000*1000);
		//ICR2 = cnt;

		// enable Timer2
		TIMSK2 |= _BV(OCIE2A);

		SREG = sreg;
	}
};

extern Timer2 timer2;
void timer2_tick();
