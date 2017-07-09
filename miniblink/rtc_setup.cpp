#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/f1/bkp.h>


#include "rtc_setup.hpp"

void rtc_set_ts(int Y, int M, int D, int h, int m, int s, int state);

void rtc_setup()
{
	rtc_auto_awake(RCC_LSE, 0x7fff);

	// Without this the RTC interrupt routine will never be called.
	nvic_enable_irq(NVIC_RTC_IRQ);
	nvic_set_priority(NVIC_RTC_IRQ, 1);

	// Enable the RTC interrupt to occur off the SEC flag.
	rtc_interrupt_enable(RTC_SEC);
}

static bool leapYear(int year) { return ((year % 4 == 0) && (year % 100!= 0)) || (year%400 == 0); }

// Dom=0 Lun=1 ecc
static int dayofweek(int y, int m, int d)	// 1 <= m <= 12,  y > 1752 (in the U.K.) 
{
	static const int8_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	y -= m < 3;
	return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}

static void getDaySaveLigthDays(int Y, int &D03, int &D10)
{
	int lastMar = dayofweek(Y, 3, 31);
	int lastOct = dayofweek(Y, 10, 31);

	D03 = 31 - lastMar;
	D10 = 31 - lastOct;
}

static int getDaySaveState(int Y, int M, int D, int h)
{
	int D03, D10;
	getDaySaveLigthDays(Y, D03, D10);

	if (M < 3) return 1;
	if (M == 3 && D < D03) return 1;
	if (M == 3 && D == D03 && h < 2) return 1;

	if (M < 10) return 2;
	if (M == 10 && D < D10) return 2;
	if (M == 10 && D == D10 && h < 2) return 2;
	if (M == 10 && D == D10 && h < 3) return 3;

	return 1;
}

volatile uint32_t rtc_counter = 0;
extern "C" void rtc_isr(void)
{
	// The interrupt flag isn't cleared by hardware, we have to do it.
	rtc_clear_flag(RTC_SEC);

	// get value
	rtc_counter = rtc_get_counter_val();

	// partendo da rtc_counter calcola la data e l'ora.
	int h, m, s;
	int cnt = rtc_get_hms(h,m,s);
	int D = BKP_DR1;
	int M = BKP_DR2;
	int Y = BKP_DR3;
	int state = BKP_DR4; // 1=inverno 2=estate 3=cambio estate/inverno

	// ora in cnt ho il numero di giorni dalla partenza
	if (cnt > 0)
	{
		static const int8_t months[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
		while (cnt > 0)
		{
			D += 1;
			cnt -= 1;

			int nd = M == 2 && leapYear(Y) ? 29 : months[M-1];
			if (nd == D)
			{
				D = 1;
				M += 1;

				if (M == 13)
				{
					Y += 1;
					M = 1;
				}
			}
		}
		// aggiorno la data MA preservo lo stato Estate/Inverno
		rtc_set_ts(Y, M, D, h, m, s, state);
	}

	// calcolo il nuovo stato
	int newState = getDaySaveState(Y, M, D, h);

	switch (state)
	{
	case 1: // da inverno
		switch (newState) // sono inverno
		{
		case 1:
			// rimango in inverno
			break;

		case 2: // a estate .... aggiorno il clock
			rtc_set_counter_val(rtc_get_counter_val() + 3600);
			break;

		case 3: // a estate/inverno .... vuole andare in inverno e c'è già
			rtc_set_counter_val(rtc_get_counter_val() + 3600);
			pwr_disable_backup_domain_write_protect();
			BKP_DR4 = newState;
			pwr_enable_backup_domain_write_protect();
			break;
		}
		break;

	case 2: // sono estate
		switch (newState)
		{
		case 2:
			break;
		case 3: // vuole andare tra estate e inverno
		case 1: // vuole andate in inverno
			rtc_set_counter_val(rtc_get_counter_val() - 3600);
			pwr_disable_backup_domain_write_protect();
			BKP_DR4 = newState;
			pwr_enable_backup_domain_write_protect();
			break;
		}
		break;

	case 3: // da estate/inverno
		switch (newState)
		{
		case 2: // a estate.... metto in inverno per poi transitare in estato
		case 1: // a inverno... vado in inverno
			pwr_disable_backup_domain_write_protect();
			BKP_DR4 = newState;
			pwr_enable_backup_domain_write_protect();
			break;

		case 3: // a estate/inverno
			break;
		}
		break;
	}

}

int rtc_get_hms(int &h, int &m, int &s)
{
	int cnt = rtc_counter;

	s = cnt % 60; cnt /= 60; 
	m = cnt % 60; cnt /= 60;
	h = cnt % 24; cnt /= 24;

	return cnt;
}
void rtc_get_dmy(int &d, int &m, int &y)
{
	d = BKP_DR1;
	m = BKP_DR2;
	y = BKP_DR3;
}

void rtc_set_ts(int Y, int M, int D, int h, int m, int s, int state)
{
	int cnt = s + m * 60 + h * 60 * 60;
	pwr_disable_backup_domain_write_protect();
	BKP_DR1 = D;
	BKP_DR2 = M;
	BKP_DR3 = Y;
	BKP_DR4 = state;
	pwr_enable_backup_domain_write_protect();
	rtc_set_counter_val(cnt);
}

void rtc_set_ts(int Y, int M, int D, int h, int m, int s)
{
	int state = getDaySaveState(Y, M, D, h);
	rtc_set_ts(Y, M, D, h, m, s, state);
}
