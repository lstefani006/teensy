#ifndef __usart_irq_hpp__
#define __usart_irq_hpp__

#include <ring.hpp>
#include <stdio.h>
#include <uprintf.hpp>
/*

// Lettura/scrttura della USART1
void usart_setup(void);

// scrive sulla seriale, ritorna il numero di char scritti che
// quasi sicuramente sono meno di len
int usart_write(const char *ptr, int len);

// scrive TUTTA la stringa sulla seriale.
// Se la stringa è lunga aspetta la seriale finchè non ha spazio sufficiente...
void usart_write(const char *ptr);

// Legge dalla seriale.
// ritorna il numero di caratteri letti, Zero se non c'e' niente da leggere
// La funzione non si blocca ad aspettare un carattere, controlla solo se c'è
int usart_read(char *ptr, int len);
*/

////////////////////////////////////////////////////////////////////
enum Format { HEX, DEC };
////////////////////////////////////////////////////////////////////
// Sola scrittura a POLLING
class USART
{
public:
	USART(uint32_t usart) : _usart(usart) {}

	void begin();
	void begin(int /*speed*/) {} // per compatibilità
	void write(const char *p, int sz);

	USART & operator << (int n) { return print(n); }
	USART & operator << (const char *n) { return print(n); }

	USART& println();
	USART& print(const char *p);
	USART& println(const char *p);

	USART& print(char ch);
	USART& print(int n);
	USART& println(int n);

	USART& print(int n, Format f);
	USART& println(int n, Format f);

private:
	uint32_t _usart;
};


////////////////////////////////////////////////////////////////////
// Scrittura/LEttura ad INTERRUPT
class USARTIRQ
{
public:
	USARTIRQ(int usart) : _usart(usart), _rx(), _tx(), _error(0) {}

	void begin(uint8_t *brx, int szrx, uint8_t *btx, int sztx);
	void begin(int /*speed*/) {} // per compatibilità

	int write(const char *tx, int sz) { return write((const uint8_t *)tx, sz); }
	int write(const uint8_t *tx, int sz);

	int read(uint8_t *rx, int sz);

	USARTIRQ & println();
	USARTIRQ & print(const char *p);
	USARTIRQ & println(const char *p);

	USARTIRQ & print(char ch);
	USARTIRQ & print(int n);
	USARTIRQ & println(int n);

	USARTIRQ & print(int n, Format f);
	USARTIRQ & println(int n, Format f);

	USARTIRQ & printf(const char *fmt, ...);

	int getch();

	USARTIRQ & operator << (int n) { print(n); return *this; }
	USARTIRQ & operator << (const char *n) { print(n); return *this; }

	bool rxError() const { return (_error & 1) > 0; }
	void clearError() { _error = 0; }

	void irq();
private:
	int _usart;
	Ring _rx;
	Ring _tx;
	int _error;
};
#endif
