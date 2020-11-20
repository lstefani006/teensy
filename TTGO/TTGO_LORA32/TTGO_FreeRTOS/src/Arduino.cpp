#include <Arduino.h>
#include <string.h>

SPIClass::SPIClass(gpio_num_t clk, gpio_num_t mosi, gpio_num_t miso, gpio_num_t cs, gpio_num_t rst, gpio_num_t irq)
	: spi(nullptr), clk(clk), mosi(mosi), miso(miso), cs(cs), rst(rst), irq(irq) {}

void SPIClass::setup()
{
	int DMA_CHAN = 2;

	spi_bus_config_t bus_config;
	memset(&bus_config, 0, sizeof(bus_config));
	bus_config.sclk_io_num = this->clk;	 // CLK
	bus_config.mosi_io_num = this->mosi; // MOSI
	bus_config.miso_io_num = this->miso; // MISO
	bus_config.quadwp_io_num = -1;		 // Not used
	bus_config.quadhd_io_num = -1;		 // Not used
	bus_config.max_transfer_sz = 1000;	 // LEO qui per gli LCD ci mettono la grandezza del display in pixel.
	ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &bus_config, DMA_CHAN));

	spi_device_interface_config_t dev_config;
	memset(&dev_config, 0, sizeof(dev_config));
	dev_config.address_bits = 0;
	dev_config.command_bits = 0;
	dev_config.dummy_bits = 0;
	dev_config.mode = 0;
	dev_config.duty_cycle_pos = 0;
	dev_config.cs_ena_posttrans = 0;
	dev_config.cs_ena_pretrans = 0;
	dev_config.clock_speed_hz = 10 * 1000 * 1000;
	dev_config.spics_io_num = this->cs;
	dev_config.flags = 0;
	dev_config.queue_size = 7;
	dev_config.pre_cb = nullptr; // lcd_spi_pre_transfer_callback;
	dev_config.post_cb = nullptr;
	ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &dev_config, &spi));

	// //Initialize non-SPI GPIOs
	// gpio_set_direction(dc, GPIO_MODE_OUTPUT);
	// gpio_set_direction(this->clk, GPIO_MODE_OUTPUT);

	//Reset the display
	gpio_set_direction(this->rst, GPIO_MODE_OUTPUT);
	gpio_set_level(this->rst, 0);
	vTaskDelay(100 / portTICK_RATE_MS);
	gpio_set_level(this->rst, 1);
	vTaskDelay(100 / portTICK_RATE_MS);
}


uint8_t SPIClass::transfer(uint8_t data)
{
	spi_transaction_t t;
	memset(&t, 0, sizeof(t));
	t.tx_buffer = &data; //The data is the cmd itself
	t.length = 8 * 1;
	t.flags = SPI_TRANS_USE_RXDATA; // Receive into rx_data member of spi_transaction_t instead into memory at rx_buffer.
	t.user = (void *)0;				// serve per pilotare il D/C - tramite il pre_cb
	ESP_ERROR_CHECK(spi_device_polling_transmit(this->spi, &t));

	if (false)
	{
		printf("TX=%02x ==> RX=", data);
		for (int i = 0; i < 4; ++i)
			printf("%02x ", t.rx_data[i]);
		printf("\n");
	}

	return t.rx_data[0];
}

//////////////////////////////

// interrupt service routine, called when the button is pressed
//void IRAM_ATTR arduino_isr_handler(void *arg)
//{
//}

static bool isr_service_installed = false;
void attachInterrupt(int pin, void (*pf)(void *), int mode)
{
	if (!isr_service_installed)
	{
		isr_service_installed = true;
#define ESP_INTR_FLAG_DEFAULT 0
		ESP_ERROR_CHECK(gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT));
	}

	if (mode == RISING)
		ESP_ERROR_CHECK(gpio_set_intr_type((gpio_num_t)pin, GPIO_INTR_POSEDGE));
	else if (mode == FALLING)
		ESP_ERROR_CHECK(gpio_set_intr_type((gpio_num_t)pin, GPIO_INTR_NEGEDGE));
	else
		ESP_ERROR_CHECK(ESP_ERR_INVALID_ARG);

	// attach the interrupt service routine
	if (pf)
		ESP_ERROR_CHECK(gpio_isr_handler_add((gpio_num_t)pin, pf, NULL));
	else
		ESP_ERROR_CHECK(gpio_isr_handler_remove((gpio_num_t)pin));
}
void detachInterrupt(int pin)
{
	gpio_isr_handler_remove((gpio_num_t)pin);
}
