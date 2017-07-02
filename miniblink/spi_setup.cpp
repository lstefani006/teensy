#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/spi.h>

#include <stdio.h>
#include <string.h>
#include <ring.hpp>
#include <errno.h>

#include <spi_setup.hpp>

void SPI::begin(int speed, bool enable16bits)
{
	switch (_spi)
	{
	case SPI1: rcc_periph_clock_enable(RCC_SPI1); spi_reset(SPI1_BASE); break;
	case SPI2: rcc_periph_clock_enable(RCC_SPI2); spi_reset(SPI2_BASE); break;
	default: return;
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
