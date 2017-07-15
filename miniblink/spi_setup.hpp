#ifndef __spi_hpp__
#define __spi_hpp__

#include <Arduino.h>
#include <libopencm3/stm32/spi.h>

class SPIClass
{
public:
	SPIClass(int spi) : _spi(spi) {}
	void begin(int speed = SPI_CR1_BAUDRATE_FPCLK_DIV_16, bool enable16bits = false);

//	void write(uint8_t n) { spi_xfer(_spi, n); }
//	void write(uint16_t n) { spi_xfer(_spi, n); }

	uint8_t transfer(uint8_t v) { return (uint8_t)spi_xfer(_spi, v); }

//	uint8_t  read8()  { return spi_read(_spi); }
//	uint16_t read16() { return spi_read(_spi); }

	void beginTransaction(...) {}
	void endTransaction() {}

	uint32_t SR()  const { return SPI_SR(_spi) ; }
	uint32_t CR1() const { return SPI_CR1(_spi) ; }
	uint32_t CR2() const { return SPI_CR2(_spi) ; }

	void setBitOrder(int);
	void setDataMode(int);
	void setClockDivider(int);

public:
	uint32_t _spi;
};

extern SPIClass SPI;

#endif
