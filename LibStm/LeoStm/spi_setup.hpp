#ifndef __spi_hpp__
#define __spi_hpp__

#include <Arduino.h>
// #include <libopencm3/stm32/spi.h>


enum SPI_CLOCK_DIV
{
	SPI_CLOCK_DIV2   = 1,
	SPI_CLOCK_DIV4   = 2,
	SPI_CLOCK_DIV8   = 3,
	SPI_CLOCK_DIV16  = 4,
	SPI_CLOCK_DIV32  = 5,
	SPI_CLOCK_DIV64  = 6,
	SPI_CLOCK_DIV128 = 7,
};

enum BIT_ORDER
{
	MSBFIRST=1,
	LSBFIRST=2
};

enum SPI_MODE : int
{
	SPI_MODE0=0,
	SPI_MODE1=1,
	SPI_MODE2=2,
	SPI_MODE3=3
};

struct SPISettings
{
	SPISettings(int speedMaximum , BIT_ORDER bitOrder , SPI_MODE mode) 
		: SpeedMaximum(speedMaximum), BitOrder(bitOrder), SpiMode(mode) {}

	int SpeedMaximum;
	BIT_ORDER BitOrder;
	SPI_MODE SpiMode;
};

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

	void beginTransaction(SPISettings settings);
	void endTransaction();

	void setBitOrder(BIT_ORDER dataOrder);
	void setDataMode(SPI_MODE mode);
	void setClockDivider(SPI_CLOCK_DIV divider);
	void setSpeedMaximum(int speed);

	uint32_t SR()  const;
	uint32_t CR1() const;
	uint32_t CR2() const;

public:
	uint32_t _spi;
};

extern SPIClass SPI;

#endif
