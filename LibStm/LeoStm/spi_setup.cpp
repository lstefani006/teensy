#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/spi.h>

#include <stdio.h>
#include <string.h>
#include <ring.hpp>
#include <errno.h>

#include <spi_setup.hpp>

uint8_t SPIClass::transfer(uint8_t v) { return (uint8_t)spi_xfer(_spi, v); }

uint32_t SPIClass::SR()  const { return SPI_SR(_spi) ; }
uint32_t SPIClass::CR1() const { return SPI_CR1(_spi) ; }
uint32_t SPIClass::CR2() const { return SPI_CR2(_spi) ; }

static void S_spi_init_clock(int spi)
{
	/* SPI1 ==> e' attaccato al bus clock APB2 */
	/* SPI2 ==> e' attaccato al bus clock APB1 */
	switch (spi)
	{
	case SPI1: 
		rcc_periph_clock_enable(RCC_GPIOA);
		rcc_periph_clock_enable(RCC_SPI1); 
		gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, /*GPIO5*/GPIO_SPI1_SCK | /*GPIO7*/GPIO_SPI1_MOSI);
		gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, /*GPIO6*/GPIO_SPI1_MISO);
		gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, /*GPIO4*/GPIO_SPI1_NSS);

		gpio_set(GPIOA, GPIO_SPI1_MISO);
		gpio_set(GPIOA, GPIO_SPI1_NSS);
		break;

	case SPI2: 
		rcc_periph_clock_enable(RCC_GPIOB);
		rcc_periph_clock_enable(RCC_SPI2); 

		gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_SPI2_SCK | GPIO_SPI2_MOSI);
		gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO_SPI2_NSS);
		gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_SPI2_MISO);

		gpio_set(GPIOB, GPIO_SPI2_MISO);
		gpio_set(GPIOB, GPIO_SPI2_NSS);
		break;

	default: 
		HALT;
	}
}

/*
 * SPI1 ==> SS=PA4  SCK=PA5  MISO=PA6  MOSI=PA7
 * SPI1 ==> SS=PA15 SCK=PB3  MISO=PB4  MOSI=PB5
 *
 * SPI2 ==> SS=PB12 SCK=PB13 MISO=PB14 MOSI=PB15
 */
void SPIClass::begin(bool enable16bits)
{
	//rcc_periph_clock_enable(RCC_AFIO);

	S_spi_init_clock(_spi);

	spi_reset(_spi);


	int br = SPI_CR1_BAUDRATE_FPCLK_DIV_32;

	// In arduino
	// Mode 0 (the default) 
	// - clock is normally low (CPOL = 0), 
	// - and the data is sampled on the transition from low to high (leading edge) (CPHA = 0)
	// - The default is most-significant bit first,
	spi_init_master(_spi,
			br,
			SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
			SPI_CR1_CPHA_CLK_TRANSITION_1,
			enable16bits ? SPI_CR1_DFF_16BIT : SPI_CR1_DFF_8BIT,
			SPI_CR1_MSBFIRST);

	/* Enable SPI1 periph. */
	spi_enable(_spi);
}
void SPIClass::begin(SPISettings tr)
{
	//rcc_periph_clock_enable(RCC_AFIO);

	S_spi_init_clock(_spi);

	spi_reset(_spi);

	int br = SPI_CR1_BAUDRATE_FPCLK_DIV_32;

	// In arduino
	// Mode 0 (the default) 
	// - clock is normally low (CPOL = 0), 
	// - and the data is sampled on the transition from low to high (leading edge) (CPHA = 0)
	// - The default is most-significant bit first,
	spi_init_master(_spi,
			br,
			SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
			SPI_CR1_CPHA_CLK_TRANSITION_1,
			SPI_CR1_DFF_8BIT,
			SPI_CR1_MSBFIRST);


	this->setSpeedMaximum(tr.SpeedMaximum);
	this->setDataMode(tr.SpiMode);
	this->setBitOrder(tr.BitOrder);

	/* Enable SPI1 periph. */
	spi_enable(_spi);
}
/////////////////////////////////////////////////////////

void SPIClass::setBitOrder(BIT_ORDER order)
{
	switch (order)
	{
	case MSBFIRST: spi_send_lsb_first(_spi); break;
	case LSBFIRST: spi_send_msb_first(_spi); break;
	default: HALT;
	}
}
void SPIClass::setDataMode(SPI_MODE mode)
{
	switch(mode)
	{
	case SPI_MODE0: spi_set_standard_mode(_spi, 0); break;
	case SPI_MODE1: spi_set_standard_mode(_spi, 1); break;
	case SPI_MODE2: spi_set_standard_mode(_spi, 2); break;
	case SPI_MODE3: spi_set_standard_mode(_spi, 3); break;
	default: HALT;
	}
}

/* SPI1 ==> e' attaccato al bus clock APB2 */
/* SPI2 ==> e' attaccato al bus clock APB1 */
/*
   uint32_t rcc_apb1_frequency = 8000000;
   uint32_t rcc_apb2_frequency = 8000000;
   uint32_t rcc_ahb_frequency = 8000000;
   */
void SPIClass::setSpeedMaximum(uint32_t f)
{
	// la frequenza del clock sul SPI
	uint32_t hz;
	switch (_spi)
	{
	case SPI1: hz = rcc_apb2_frequency; break;
	case SPI2: hz = rcc_apb1_frequency; break;
	default: HALT;
	}

	uint8_t baudrate;
	/***/if (hz / 2 <= f) baudrate = SPI_CR1_BR_FPCLK_DIV_2;
	else if (hz / 4 <= f) baudrate = SPI_CR1_BR_FPCLK_DIV_4;
	else if (hz / 8 <= f) baudrate = SPI_CR1_BR_FPCLK_DIV_8;
	else if (hz / 16 <= f) baudrate = SPI_CR1_BR_FPCLK_DIV_16;
	else if (hz / 32 <= f) baudrate = SPI_CR1_BR_FPCLK_DIV_32;
	else if (hz / 64 <= f) baudrate = SPI_CR1_BR_FPCLK_DIV_64;
	else if (hz / 128 <= f) baudrate = SPI_CR1_BR_FPCLK_DIV_128;
	else if (hz / 256 <= f) baudrate = SPI_CR1_BR_FPCLK_DIV_256;
	else  baudrate = SPI_CR1_BR_FPCLK_DIV_256;

	spi_set_baudrate_prescaler(_spi, baudrate);
}

void SPIClass::setClockDivider(SPI_CLOCK_DIV divider)
{
	int f = 16 * 1000*1000; // La frequenza di arduino 16Mhz
	switch (divider)
	{
	case SPI_CLOCK_DIV2:   f /= 2; break;
	case SPI_CLOCK_DIV4:   f /= 4; break;
	case SPI_CLOCK_DIV8:   f /= 8; break;
	case SPI_CLOCK_DIV16:  f /= 16; break;
	case SPI_CLOCK_DIV32:  f /= 32; break;
	case SPI_CLOCK_DIV64:  f /= 64; break;
	case SPI_CLOCK_DIV128: f /= 128; break;
	default: HALT;
	}

	this->setSpeedMaximum(f);
}
void SPIClass::beginTransaction(SPISettings tr)
{
	spi_disable(_spi);

	this->setSpeedMaximum(tr.SpeedMaximum);
	this->setDataMode(tr.SpiMode);
	this->setBitOrder(tr.BitOrder);

	spi_enable(_spi);
}

void SPIClass::endTransaction() {
}
//////////////////////////////////////

uint16_t SPIClass::read() {
	/* Wait for transfer finished. */
	while (!(SPI_SR(_spi) & SPI_SR_RXNE));

	/* Read the data (8 or 16 bits, depending on DFF bit) from DR. */
	return SPI_DR(_spi);
}
void SPIClass::send(uint16_t data) {
	/* Wait for transfer finished. */
	while (!(SPI_SR(_spi) & SPI_SR_TXE));

	/* Write data (8 or 16 bits, depending on DFF) into DR. */
	SPI_DR(_spi) = data;
}
int16_t SPIClass::xfer(uint16_t data)
{
	/* Write data (8 or 16 bits, depending on DFF) into DR. */
	SPI_DR(_spi) = data;

	/* Wait for transfer finished. */
	while (!(SPI_SR(_spi) & SPI_SR_RXNE));

	/* Read the data (8 or 16 bits, depending on DFF bit) from DR. */
	return SPI_DR(_spi);
}


void SPIClass::enable() { SPI_CR1(_spi) |= SPI_CR1_SPE; }
void SPIClass::disable() { SPI_CR1(_spi) &= ~SPI_CR1_SPE; }

uint16_t SPIClass::clean_disable() 
{
	/* Wait to receive last data */ 
	while (!(SPI_SR(_spi) & SPI_SR_RXNE));

	uint16_t data = SPI_DR(_spi);

	/* Wait to transmit last data */
	while (!(SPI_SR(_spi) & SPI_SR_TXE));

	/* Wait until not busy */
	while (SPI_SR(_spi) & SPI_SR_BSY);

	SPI_CR1(_spi) &= ~SPI_CR1_SPE;

	return data;
}
