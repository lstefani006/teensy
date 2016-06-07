
#include <Arduino.h>

// Timer2 supporta solo overflow 0xff=>0x00 scatta l'interrupt
class Timer2
{
public:
	uint8_t _cnt;

	int setup_overflow(uint32_t microSec)
	{
		auto sreg = SREG;
		cli();

		TCCR2A = 0; // COM2A1 COM2A0 COM2B1 COM2B0     –    – WGM21 WGM20
		TCCR2B = 0; // FOC2A   FOC2B      –      – WGM22 CS22  CS21  CS20

		uint32_t cy = F_CPU / (1000ul*1000ul) * microSec;
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
			SREG = sreg;
			return -1;
		}


		// cnt = F_CPU / prescaler * microSec / (1000*1000);
		if (cnt > 0xff) cnt = 0xff;
		_cnt = 0xff - cnt;
		TCNT2 = _cnt;

		// enable Timer2 overflow interrupt:
		TIMSK2 |= _BV(TOIE2);

		SREG = sreg;
		return 0;
	}


	int setup_CTC(uint32_t microSec)
	{
		auto sreg = SREG;
		cli();


		// reset dei bit che indicano come funziona il timer2 a livello di interrupt
		// TOIE2  ==> overflow                  ==> setta TIFR2 bit TOV2   ==> overflow interrupt ISR(TIMER2_OVF_vect)
		// OCIE2A ==> TCT sul registro OCIE2A   ==> setta TIFR2 bit OCF2A  ==> ISR(TIMER2_COMPA_vect)
		// OCIE2B ==> TCT sul registro OCIE2B   ==> setta TIFR2 bit OCF2B  ==> ISR(TIMER2_COMPB_vect)
		TIMSK2 &= ~(_BV(OCIE2B) | _BV(OCIE2A) | _BV(TOIE2));

		// TCCR2A controlla come controllare il Pin OC2A quando il timer e in Normal Mode o CTC
		// I bit COM2A1 COM2A0 controllano quello che succede quando il timer supera la soglia A
		// I bit COM2B1 COM2B0 controllano quello che succede quando il timer supera la soglia B
		// I bit WGM21 WGM20 (il WGM22 e' nel registro B) controllano come funziona il timer
		// (WGM22 WGM21 WGM20) = (0,0,0)  ==> Normal
		// (WGM22 WGM21 WGM20) = (0,1,0)  ==> CTC
		TCCR2A &= ~(_BV(WGM20) | _BV(WGM21));
		TCCR2B &= ~(_BV(WGM22));

		// TCCR2B controlla il prescaler oltre che il WGM22 sopra
		// (CS22 CS21 CS20) prescaler = <niente>/1/8/32/64/128/256/1024
		//                              0        1 2  3  4   5   6    7
		TCCR2B &= ~(_BV(CS22) | _BV(CS21) | _BV(CS20));     // nessun clock ==> fermo il timer

		// TCNT2 ==> questo registro contiene il contatore tra 0 e FF
		// OCR2A ==> primo valore di soglia
		// OCR2B ==> secondo valore di soglia
		// ASSR  ==> controlla da dove viene il clock
		ASSR &= ~(_BV(AS2));   // clock interno. Se fosse 1 clock dal pin TOSC1

		// ORA cominciamo a mettere i valori buoni.


		uint32_t cy = F_CPU / (1000UL*1000UL)* microSec;
		constexpr uint32_t m = 256; // risoluzione del timer

		if (cy <= m * 1)
		{
			TCCR2B |= _BV(CS20);
			OCR2A = cy/1 - 1;
		}
		else if (cy <= m * 8)
		{
			TCCR2B |= _BV(CS21);
			OCR2A = cy/8 - 1;
		}
		else if (cy <= m * 32)
		{
			TCCR2B |= _BV(CS21) | _BV(CS20);
			OCR2A = cy/32 - 1;
		}
		else if (cy <= m * 64)
		{
			TCCR2B |= _BV(CS22);
			OCR2A = cy/64 - 1;
		}
		else if (cy <= m * 128)
		{
			TCCR2B |= _BV(CS22) | _BV(CS20);
			OCR2A = cy/128 - 1;
		}
		else if (cy <= m * 256)
		{
			TCCR2B |= _BV(CS22) | _BV(CS21);
			OCR2A = cy/256 - 1;
		}
		else if (cy <= m * 1024)
		{
			TCCR2B |= _BV(CS22) | _BV(CS21) | _BV(CS20);
			OCR2A = cy/1024 - 1;
		}
		else
		{
			SREG = sreg;
			return -1;
		}
		// Abilito CTC
		TCCR2A = _BV(WGM21);

		// Abilito gli interrupt 
		TIMSK2 |= _BV(OCIE2A);

		SREG = sreg;
		return 0;
	}
};

extern Timer2 timer2;
void timer2_tick();
