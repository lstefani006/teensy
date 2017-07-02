#ifndef __spi_hpp__
#define __spi_hpp__

#include <libopencm3/stm32/spi.h>

class SPI
{
public:
	SPI(int spi) : _spi(spi) {}
	void begin(int speed = SPI_CR1_BR_FPCLK_DIV_64, bool enable16bits = false);

	void write(uint8_t n) { spi_write(_spi, n); }
	void write(uint16_t n) { spi_write(_spi, n); }

	uint8_t  read8()  { return spi_read(_spi); }
	uint16_t read16() { return spi_read(_spi); }

public:
	uint32_t _spi;
};
#endif
