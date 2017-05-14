#ifndef __usart_irq_hpp__
#define __usart_irq_hpp__

void usart_setup(void);
int usart_write(const char *ptr, int len);

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
