#ifndef __spi_hpp__
#define __spi_hpp__

#include <Arduino.h>
// #include <libopencm3/stm32/spi.h>

struct SPISettings
{
	SPISettings(int , int , int ) {}
};

#define SPI_CLOCK_DIV2   1
#define SPI_CLOCK_DIV4   2
#define SPI_CLOCK_DIV8   3
#define SPI_CLOCK_DIV16  4
#define SPI_CLOCK_DIV32  5
#define SPI_CLOCK_DIV64  6
#define SPI_CLOCK_DIV128 7

#define MSBFIRST 1
#define LSBFIRST 2

#define SPI_MODE0 0

class SPIClass
{
public:
	SPIClass(int spi) : _spi(spi) {}
	void begin(/*int speed = SPI_CR1_BAUDRATE_FPCLK_DIV_16,*/ bool enable16bits = false);

//	void write(uint8_t n) { spi_xfer(_spi, n); }
//	void write(uint16_t n) { spi_xfer(_spi, n); }

	uint8_t transfer(uint8_t v);

//	uint8_t  read8()  { return spi_read(_spi); }
//	uint16_t read16() { return spi_read(_spi); }

	void beginTransaction(...);
	void endTransaction();

	uint32_t SR()  const;
	uint32_t CR1() const;
	uint32_t CR2() const;

	void setBitOrder(int dataOrder);
	void setDataMode(int);
	void setClockDivider(int);

public:
	uint32_t _spi;
};

extern SPIClass SPI;

#endif
