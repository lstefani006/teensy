#include "usart_setup.hpp"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

#include <stdio.h>
#include <string.h>
#include <ring.hpp>
#include <errno.h>


#define BUFFER_SIZE 8
static uint8_t _output_ring_buffer[BUFFER_SIZE];
static uint8_t _input_ring_buffer[BUFFER_SIZE];

static Ring output_ring(_output_ring_buffer, BUFFER_SIZE);
static Ring input_ring(_input_ring_buffer, BUFFER_SIZE);

void usart_setup(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_AFIO);
	rcc_periph_clock_enable(RCC_USART1);

	// Enable the USART1 interrupt. 
	nvic_enable_irq(NVIC_USART1_IRQ);

	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT,         GPIO_CNF_INPUT_FLOAT,           GPIO_USART1_RX);

	// Setup UART parameters. 
	usart_set_baudrate(USART1, 38400);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	usart_set_mode(USART1, USART_MODE_TX_RX);

	// Enable USART1 Receive interrupt. 
	USART_CR1(USART1) |= USART_CR1_RXNEIE;

	// Finally enable the USART. 
	usart_enable(USART1);
}

extern "C" void usart1_isr(void) 
{
	// Check if we were called because of RXNE.
	if ((USART_CR1(USART1) & USART_CR1_RXNEIE) != 0 && (USART_SR(USART1) & USART_SR_RXNE) != 0) 
	{
		// Retrieve the data from the peripheral.
		input_ring.WriteCh(usart_recv(USART1));
	}

	// Check if we were called because of TXE.
	if ((USART_CR1(USART1) & USART_CR1_TXEIE) != 0 && (USART_SR(USART1) & USART_SR_TXE) != 0)
	{
		int32_t data = output_ring.ReadCh();
		if (data < 0)
		{
			// Disable the TXE interrupt, it's no longer needed.
			USART_CR1(USART1) &= ~USART_CR1_TXEIE;
		} 
		else 
		{
			// Put data into the transmit register.
			usart_send(USART1, data);
		}
	}
}

int usart_write(const char *ptr, int len)
{
	int ret = output_ring.Write((uint8_t *)ptr, len);
	if (ret < 0)
		ret = -ret;

	// Abilitra TXE interrupt perchè c'è qualcosa da inviare ... ed è già nella coda.
	USART_CR1(USART1) |= USART_CR1_TXEIE;
	return ret;
}

void usart_write(const char *ptr)
{
	while (*ptr)
	{
		int r;
		do { r = usart_write(ptr, 1); } while (r < 0);
		ptr++;
	}
}

int usart_read(char *ptr, int len)
{
	int n;
	for (n = 0; n < len; ++n)
	{
		int r = input_ring.ReadCh();
		if (r < 0) break;
		*ptr++ = (char)(uint8_t)r;
	}
	return n;
}


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
void USART::write(const char *p) 
{
	while (*p)
		usart_send_blocking(_usart, *p++);
}

void USART::write(int n) 
{
	char b[16];
	sprintf(b, "%d", n);
	write(b);
}
