#include "usart_irq.hpp"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
 #include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

typedef int32_t ring_size_t;

struct ring 
{
	uint8_t *data;
	ring_size_t size;
	uint32_t beg;
	uint32_t end;
};

#define RING_SIZE(RING)  ((RING)->size - 1)
#define RING_DATA(RING)  (RING)->data
#define RING_EMPTY(RING) ((RING)->beg == (RING)->end)

static void ring_init(volatile ring *ring, uint8_t *buf, ring_size_t size)
{
	ring->data = buf;
	ring->size = size;
	ring->beg = 0;
	ring->end = 0;
}

static int32_t ring_write_ch(volatile ring *ring, uint8_t ch)
{
	// aspetto che il buffer si svuoti almeno parzialmente
	if (((ring->end + 1) % ring->size) == ring->beg)
		USART_CR1(USART1) |= USART_CR1_TXEIE;
	while (((ring->end + 1) % ring->size) == ring->beg) {}

	if (((ring->end + 1) % ring->size) != ring->beg) 
	{
		ring->data[ring->end] = ch;
		ring->end = (ring->end + 1) % ring->size;
		return (int32_t)ch;
	}

	return -1;
}

static void ring_write(volatile ring *ring, const uint8_t *data, ring_size_t size)
{
	for (int32_t i = 0; i < size; i++)
		ring_write_ch(ring, data[i]);
}

static int32_t ring_read_ch(volatile ring *ring)
{
	if (ring->beg == ring->end) 
		return -1;

	int32_t ret = ring->data[ring->beg];
	ring->beg = (ring->beg + 1) % ring->size;
	return ret;
}


#define BUFFER_SIZE 8

static struct ring output_ring;
static uint8_t output_ring_buffer[BUFFER_SIZE];

extern "C" void usart1_isr(void)
{
	// Check if we were called because of RXNE. 
	if (((USART_CR1(USART1) & USART_CR1_RXNEIE) != 0) &&
			((USART_SR(USART1) & USART_SR_RXNE) != 0)) {

		/*
		 LEO commentato perchè servirebbe un buffer in uscita

		// Retrieve the data from the peripheral.
		ring_write_ch(&output_ring, usart_recv(USART1));

		// Enable transmit interrupt so it sends back the data.
		USART_CR1(USART1) |= USART_CR1_TXEIE;
		*/
	}

	// Check if we were called because of TXE.
	if (((USART_CR1(USART1) & USART_CR1_TXEIE) != 0) &&
			((USART_SR(USART1) & USART_SR_TXE) != 0)) {

		int32_t data = ring_read_ch(&output_ring);
		if (data == -1) {
			// Disable the TXE interrupt, it's no longer needed.
			USART_CR1(USART1) &= ~USART_CR1_TXEIE;
		} else {
			// Put data into the transmit register.
			usart_send(USART1, data);
		}
	}
}

int usart_write(const char *ptr, int len)
{
	ring_write(&output_ring, (const uint8_t *)ptr, len);
	USART_CR1(USART1) |= USART_CR1_TXEIE;
	return len;
}


void usart_setup(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_AFIO);
	rcc_periph_clock_enable(RCC_USART1);

	// Initialize output ring buffer. 
	ring_init(&output_ring, output_ring_buffer, BUFFER_SIZE);

	// Enable the USART1 interrupt. 
	nvic_enable_irq(NVIC_USART1_IRQ);

	// Setup GPIO pin GPIO_USART1_RE_TX on GPIO port B for transmit. 
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);

	// Setup GPIO pin GPIO_USART1_RE_RX on GPIO port B for receive. 
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);

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
