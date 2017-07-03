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
	switch (_spi)
	{
	case SPI1: 
		rcc_periph_clock_enable(RCC_GPIOA);
		gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO4 | GPIO5 | GPIO7);
		gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO6);
		rcc_periph_clock_enable(RCC_SPI1); 
		spi_reset(SPI1_BASE);
		break;

	case SPI2: 
		rcc_periph_clock_enable(RCC_GPIOB);
		gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO12 | GPIO13 | GPIO15);
		gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO14);
		rcc_periph_clock_enable(RCC_SPI2); 
		spi_reset(SPI2_BASE);
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

	// Set NSS management to software.
	// Note:
	// Setting nss high is very important, even if we are controlling the GPIO
	// ourselves this bit needs to be at least set to 1, otherwise the spi
	// peripheral will not send any data out.
	spi_enable_software_slave_management(_spi);
	spi_set_nss_high(_spi);

	/* Enable SPI1 periph. */
	spi_enable(_spi);
}
