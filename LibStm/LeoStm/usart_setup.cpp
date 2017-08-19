#include "usart_setup.hpp"

#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/spi.h>

#include <stdio.h>
#include <string.h>
#include <ring.hpp>
#include <errno.h>

/////////////////////////////////////////////////////////////

void USART::begin()
{
	rcc_periph_clken ckgpio, ckusart;
	uint32_t gpioport;
	uint16_t gpios_tx, gpios_rx;
	switch (_usart)
	{
	case USART1: ckgpio = RCC_GPIOA; ckusart = RCC_USART1; gpioport = GPIOA; gpios_tx = GPIO_USART1_TX; gpios_rx = GPIO_USART1_RX; break;
	case USART2: ckgpio = RCC_GPIOA; ckusart = RCC_USART2; gpioport = GPIOA; gpios_tx = GPIO_USART2_TX; gpios_rx = GPIO_USART2_RX; break;
	case USART3: ckgpio = RCC_GPIOB; ckusart = RCC_USART3; gpioport = GPIOB; gpios_tx = GPIO_USART3_TX; gpios_rx = GPIO_USART3_RX; break;
	default: return;
	}
	rcc_periph_clock_enable(ckgpio);
	rcc_periph_clock_enable(ckusart);
	gpio_set_mode(gpioport, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, gpios_tx);
	gpio_set_mode(gpioport, GPIO_MODE_INPUT,         GPIO_CNF_INPUT_FLOAT,           gpios_rx);

	// Setup UART parameters.
	usart_set_baudrate(_usart, 38400);
	usart_set_databits(_usart, 8);
	usart_set_stopbits(_usart, USART_STOPBITS_1);
	usart_set_mode(_usart, USART_MODE_TX);
	usart_set_parity(_usart, USART_PARITY_NONE);
	usart_set_flow_control(_usart, USART_FLOWCONTROL_NONE);

	usart_enable(_usart);
}

void USART::write(const char *p, int sz) 
{
	const char *e = p + sz;
	while (p != e)
		usart_send_blocking(_usart, *p++);
}
USART& USART::print(const char *p) 
{
	while (*p)
		usart_send_blocking(_usart, *p++);
	return *this;
}
USART& USART::println()
{
	print("\r\n");
	return *this;
}
USART& USART::println(const char *s)
{
	print(s);
	return println();
}
USART & USART::print  (int n, Format f) { printf(f == HEX ? "%x" : "%d", n); return *this; }
USART & USART::println(int n, Format f) { printf(f == HEX ? "%x" : "%d", n); return println(); }

USART& USART::print(char ch) { char b[] = {ch, 0}; return print(b); }
USART& USART::print(int n) { return print(n, DEC); }
USART& USART::println(int n) { return println(n, DEC); }

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
static USARTIRQ *RS[3] = {nullptr, nullptr, nullptr};

void USARTIRQ::begin(uint8_t *brx, int szrx, uint8_t *btx, int sztx)
{
	_rx.begin(brx, szrx);
	_tx.begin(btx, sztx);

	rcc_periph_clken ckgpio, ckusart;
	uint32_t gpioport;
	uint16_t gpios_tx, gpios_rx;
	auto nvic = NVIC_USART1_IRQ;
	switch (_usart)
	{
	case USART1: nvic = NVIC_USART1_IRQ; ckgpio = RCC_GPIOA; ckusart = RCC_USART1; gpioport = GPIOA; gpios_tx = GPIO_USART1_TX; gpios_rx = GPIO_USART1_RX; break;
	case USART2: nvic = NVIC_USART2_IRQ; ckgpio = RCC_GPIOA; ckusart = RCC_USART2; gpioport = GPIOA; gpios_tx = GPIO_USART2_TX; gpios_rx = GPIO_USART2_RX; break;
	case USART3: nvic = NVIC_USART3_IRQ; ckgpio = RCC_GPIOB; ckusart = RCC_USART3; gpioport = GPIOB; gpios_tx = GPIO_USART3_TX; gpios_rx = GPIO_USART3_RX; break;
	default: return;
	}
	nvic_enable_irq(nvic);
	rcc_periph_clock_enable(ckgpio);
	rcc_periph_clock_enable(ckusart);
	gpio_set_mode(gpioport, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, gpios_tx);
	gpio_set_mode(gpioport, GPIO_MODE_INPUT,         GPIO_CNF_INPUT_FLOAT,           gpios_rx);

	// Setup UART parameters.
	usart_set_baudrate(_usart, 38400);
	usart_set_databits(_usart, 8);
	usart_set_stopbits(_usart, USART_STOPBITS_1);
	usart_set_mode(_usart, USART_MODE_TX_RX);
	usart_set_parity(_usart, USART_PARITY_NONE);
	usart_set_flow_control(_usart, USART_FLOWCONTROL_NONE);

	// Enable USART1 Receive interrupt. 
	switch (_usart)
	{
	case USART1: RS[0] = this; break;
	case USART2: RS[1] = this; break;
	case USART3: RS[2] = this; break;
	}
	usart_enable_rx_interrupt(_usart);

	usart_enable(_usart);
}


void USARTIRQ::irq()
{
	// Check if we were called because of RXNE.
	if ((USART_CR1(_usart) & USART_CR1_RXNEIE) != 0 && (USART_SR(_usart) & USART_SR_RXNE) != 0) 
	{
		// Retrieve the data from the peripheral.
		if (this->_rx.WriteCh(usart_recv(_usart)) < 0)
			this->_error = 1;
	}

	// Check if we were called because of TXE.
	if ((USART_CR1(_usart) & USART_CR1_TXEIE) != 0 && (USART_SR(_usart) & USART_SR_TXE) != 0)
	{
		int32_t data = this->_tx.ReadCh();
		if (data < 0)
		{
			// Disable the TXE interrupt, it's no longer needed.
			USART_CR1(_usart) &= ~USART_CR1_TXEIE;
		} 
		else 
		{
			// Put data into the transmit register.
			usart_send(_usart, data);
		}
	}
}

extern "C" void usart1_isr(void) { if (RS[0]) RS[0]->irq(); }
extern "C" void usart2_isr(void) { if (RS[1]) RS[1]->irq(); }
extern "C" void usart3_isr(void) { if (RS[2]) RS[2]->irq(); }

int USARTIRQ::write(const uint8_t *tx, int sz)
{
	if (sz == 0) return sz;

	int ret = _tx.Write(tx, sz);
	if (ret < 0)
		ret = -ret;

	// Abilitra TXE interrupt perchè c'è qualcosa da inviare ... ed è già nella coda.
	usart_enable_tx_interrupt(_usart);
	return ret;
}
int USARTIRQ::read(uint8_t *ptr, int len)
{
	int n;
	for (n = 0; n < len; ++n)
	{
		int r = _rx.ReadCh();
		if (r < 0) break;
		*ptr++ = (char)(uint8_t)r;
	}
	return n;
}

USARTIRQ & USARTIRQ::println()
{
	return print("\r\n");
}
USARTIRQ & USARTIRQ::print(const char *str)
{
	int sz = strlen(str);
	while (sz > 0)
	{
		int n = write((const uint8_t *)str, sz);
		sz -= n;
		str += n;
	}
	return *this;
}
USARTIRQ & USARTIRQ::println(const char *str)
{
	print(str);
	return println();
}
USARTIRQ & USARTIRQ::print(char ch) { char b[] = {ch, 0}; return print(b); }
USARTIRQ & USARTIRQ::print  (int n) { return print(n, DEC); }
USARTIRQ & USARTIRQ::println(int n) { return println(n, DEC); }

USARTIRQ & USARTIRQ::print  (int n, Format f) { printf(f == HEX ? "%x" : "%d", n); return *this; }
USARTIRQ & USARTIRQ::println(int n, Format f) { printf(f == HEX ? "%x" : "%d", n); return println(); }

USARTIRQ & USARTIRQ::printf(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	upf_t cb;
	cb.ag = this;
	cb.pf = [] (char ch, void *ag) -> bool { char b[] = { ch, 0 }; ((USARTIRQ*)ag)->print(b); return true; };
	uvprintf(cb, false, fmt, args);
	va_end (args);
	return *this;
}
int USARTIRQ::getch()
{
	uint8_t c;
	int t = read(&c, 1);
	if (t == 0) return -1;
	return c;
}
