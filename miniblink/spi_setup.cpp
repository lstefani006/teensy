#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/spi.h>

#include <stdio.h>
#include <string.h>
#include <ring.hpp>
#include <errno.h>

#include <spi_setup.hpp>

/*
 * SPI1 ==> SS=PA4  SCK=PA5  MISO=PA6  MOSI=PA7
 * SPI1 ==> SS=PA15 SCK=PB3  MISO=PB4  MOSI=PB5
 *
 * SPI2 ==> SS=PB12 SCK=PB13 MISO=PB14 MOSI=PB15
 */
void SPIClass::begin(int speed, bool enable16bits)
{
	//rcc_periph_clock_enable(RCC_AFIO);

	switch (_spi)
	{
	case SPI1: 
		rcc_periph_clock_enable(RCC_GPIOA);
		rcc_periph_clock_enable(RCC_SPI1); 
		gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, /*GPIO5*/GPIO_SPI1_SCK | /*GPIO7*/GPIO_SPI1_MOSI);
		gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, /*GPIO6*/GPIO_SPI1_MISO);
		gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, /*GPIO4*/GPIO_SPI1_NSS);

		gpio_set(GPIOA, GPIO_SPI1_MISO);
		gpio_set(GPIOA, GPIO_SPI1_NSS);

		spi_reset(SPI1);
		break;

	case SPI2: 
		rcc_periph_clock_enable(RCC_GPIOB);
		rcc_periph_clock_enable(RCC_SPI2); 

		gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_SPI2_SCK | GPIO_SPI2_MOSI);
		gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO_SPI2_NSS);
		gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_SPI2_MISO);

		gpio_set(GPIOB, GPIO_SPI2_MISO);
		gpio_set(GPIOB, GPIO_SPI2_NSS);

		spi_reset(SPI2);
		break;

	default: 
		return;
	}



	// In arduino
	// Mode 0 (the default) 
	// - clock is normally low (CPOL = 0), 
	// - and the data is sampled on the transition from low to high (leading edge) (CPHA = 0)
	// - The default is most-significant bit first,
	spi_init_master(_spi,
			speed,
			SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
			SPI_CR1_CPHA_CLK_TRANSITION_1,
			enable16bits ? SPI_CR1_DFF_16BIT : SPI_CR1_DFF_8BIT,
			SPI_CR1_MSBFIRST);
	/*

	 spi_set_standard_mode(_spi,  0);

	// Set NSS management to software.
	// Note:
	// Setting nss high is very important, even if we are controlling the GPIO
	// ourselves this bit needs to be at least set to 1, otherwise the spi
	// peripheral will not send any data out.
	spi_enable_software_slave_management(_spi);
	spi_set_nss_high(_spi);

	spi_disable_crc(_spi);
	spi_disable_error_interrupt(_spi);
//	spi_disable_rx_buffer_not_empty_interrupt(_spi);
//	spi_disable_tx_buffer_empty_interrupt(_spi);
//	spi_set_full_duplex_mode(_spi);
//	spi_set_unidirectional_mode(_spi);
//	*/

	/* Enable SPI1 periph. */
	spi_enable(_spi);
}
