

class SPI
{
public:
	public SPI(int spi) : _spi(spi) {}
	void begin(int speed)
	{
		spi_reset(_spi);
	}

	void write(uint8_t n) { spi_write(_spi, n); }
	void write(uint16_t n) { spi_write(_spi, n); }

	uint8_t  read8()  { return spi_read(_spi); }
	uint16_t read16() { return spi_read(_spi); }

public:
	uint32_t _spi;
};
