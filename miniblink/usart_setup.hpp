#ifndef __usart_irq_hpp__
#define __usart_irq_hpp__

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

#endif
