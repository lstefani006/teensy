#ifndef __rtc_setup_hpp__
#define __rtc_setup_hpp__

/*
 * Imposta il RTC del STM32, dovrebbe funzianare anche solo con la VBatt attaccata
 * L'irq del RTC incrementa un contatore. 
 * Con questo contatore si determina l'ora/min/sec
 * Ad ogni isr si calcola hh/mm/ss e il contatore supera giorno in secondi
 * si avanza anche il giorno/mese/anno.
 *
 * Dovrebbe essere gestito anche l'ora legale e i gg bisestili.
 */

void rtc_setup();
extern volatile uint32_t rtc_counter;
extern void (* volatile rtc_cb)();

int rtc_get_hms(int &h, int &m, int &s);
void rtc_get_dmy(int &d, int &m, int &y);
void rtc_set_ts(int Y, int M, int D, int h, int m, int s);

#endif
