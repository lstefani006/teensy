#ifndef __usart_irq_hpp__
#define __usart_irq_hpp__

#include <ring.hpp>
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
////////////////////////////////////////////////////////////////////
// Sola scrittura a POLLING
class USART
{
public:
	USART(int usart) : _usart(usart) {}

	void begin();
	void write(const char *p, int sz);
	void write(const char *p);
	void write(int n);

	USART & operator << (int n) { write(n); return *this; }
	USART & operator << (const char *n) { write(n); return *this; }

private:
	int _usart;
};

enum Format { HEX, DEC };

////////////////////////////////////////////////////////////////////
// Scrittura/LEttura ad INTERRUPT
class USARTIRQ
{
public:
	USARTIRQ(int usart) : _usart(usart), _rx(), _tx(), _error(0) {}

	void begin(uint8_t *brx, int szrx, uint8_t *btx, int sztx);
	void begin(int /*speed*/) {} // per compatibilità

	int write(const uint8_t *tx, int sz);
	int read(uint8_t *rx, int sz);

	void write(const char *);
	void write(int n);

	void println() { write("\r\n"); }
	void print(const char *p) { write(p); }
	void println(const char *p) { write(p); println(); }

	void print(int n) { write(n); }
	void println(int n) { print(n); write("\r\n"); }

	void print(int n, Format) { print(n); }
	void println(int n, Format) { println(n); }

	int getch();

	USARTIRQ & operator << (int n) { write(n); return *this; }
	USARTIRQ & operator << (const char *n) { write(n); return *this; }

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
