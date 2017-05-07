#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/f1/bkp.h>

#include "rtc_setup.hpp"

void rtc_setup()
{
	rtc_auto_awake(RCC_LSE, 0x7fff);

	// Without this the RTC interrupt routine will never be called.
	nvic_enable_irq(NVIC_RTC_IRQ);
	nvic_set_priority(NVIC_RTC_IRQ, 1);

	// Enable the RTC interrupt to occur off the SEC flag.
	rtc_interrupt_enable(RTC_SEC);
}

static bool leapYear(int year)
{
	if (((year % 4 == 0) && (year % 100!= 0)) || (year%400 == 0)) return true;
	return false;
}
// Dom=0 Lun=1 ecc
static int dayofweek(int y, int m, int d)	// 1 <= m <= 12,  y > 1752 (in the U.K.) 
{
	static const int8_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	y -= m < 3;
	return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}

static void getDaySaveLigthDays(int Y, int &mD, int &oD)
{
	int lastMar = dayofweek(Y, 3, 31);
	int lastOct = dayofweek(Y, 10, 31);

	mD = 31 - lastMar;
	oD = 31 - lastOct;
}

volatile uint32_t rtc_counter = 0;
extern "C" void rtc_isr(void)
{
	// The interrupt flag isn't cleared by hardware, we have to do it.
	rtc_clear_flag(RTC_SEC);

	// get value
	rtc_counter = rtc_get_counter_val();

Again:

	// partendo da rtc_counter calcola la data e l'ora.
	int h, m, s;
	int cnt = rtc_get_hms(h,m,s);


	int D = BKP_DR1;
	int M = BKP_DR2;
	int Y = BKP_DR3;


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
		pwr_disable_backup_domain_write_protect();
		BKP_DR1 = D;
		BKP_DR2 = M;
		BKP_DR3 = Y;
		pwr_enable_backup_domain_write_protect();

		rtc_set_ts(Y, M, D, h, m, s);
	}

	int mD, oD;
	getDaySaveLigthDays(Y, mD, oD);

	bool oraLegaleS = (M ==  3 && D == mD && h >= 2) || (M ==  3 && D > mD) || M > 3;
	bool oraLegaleE = (M == 10 && D == oD && h <  3) || (M == 10 && D < mD) || M < 10;
	bool oraLegale = oraLegaleS && oraLegaleE;

	bool oraLegaleApplicata = BKP_DR4 != 0;

	if (oraLegale == true && oraLegaleApplicata == false)
	{
		rtc_counter += 3600;
		pwr_disable_backup_domain_write_protect();
		BKP_DR3 = 1;
		pwr_enable_backup_domain_write_protect();

		goto Again;
	}
	if (oraLegale == false && oraLegaleApplicata == true)
	{
		rtc_counter -= 3600;
		pwr_disable_backup_domain_write_protect();
		BKP_DR3 = 0;
		pwr_enable_backup_domain_write_protect();

		goto Again;
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

void rtc_set_ts(int Y, int M, int D, int h, int m, int s)
{
	int cnt = s + m * 60 + h * 60 * 60;
	pwr_disable_backup_domain_write_protect();
	BKP_DR1 = D;
	BKP_DR2 = M;
	BKP_DR3 = Y;
	pwr_enable_backup_domain_write_protect();
	rtc_set_counter_val(cnt);
}
