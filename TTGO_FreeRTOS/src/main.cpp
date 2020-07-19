#include <stdio.h>

#include <Arduino.h>
#include <LoRa.hpp>

extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"
#include "nvs_flash.h"
#include "nvs.h"
}

#include "ssd1306/ssd1306_i2c.h"
#include "ssd1306/fonts.hpp"

#define OLED_SDA /* 4*/ gpio_num_t::GPIO_NUM_4
#define OLED_SCL /*15*/ gpio_num_t::GPIO_NUM_15
#define OLED_RST /*16*/ gpio_num_t::GPIO_NUM_16

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//define the pins used by the LoRa transceiver module
#define LORA_MISO gpio_num_t::GPIO_NUM_19
#define LORA_MOSI gpio_num_t::GPIO_NUM_27
#define LORA_SCK gpio_num_t::GPIO_NUM_5
#define LORA_CS gpio_num_t::GPIO_NUM_18
#define LORA_IRQ gpio_num_t::GPIO_NUM_26 // <== LORA_DIO0
#define LORA_RST gpio_num_t::GPIO_NUM_14
#define LORA_BAND 868E6

// https://github.com/sandeepmistry/arduino-LoRa/blob/master/API.md
// http://www.lilygo.cn/prod_view.aspx?TypeId=50003&Id=1142&FId=t3:50003:3

ESP32_I2C i2c(I2C_NUM_0, OLED_SDA, OLED_SCL, OLED_RST, 0x3C);
SD1306 OLED(i2c, SCREEN_WIDTH, SCREEN_HEIGHT);

SD1306_Driver dr(OLED);

// gpio_num_t clk, gpio_num_t mosi, gpio_num_t miso, gpio_num_t cs, gpio_num_t rst, gpio_num_t irq
SPIClass SPI(LORA_SCK, LORA_MOSI, LORA_MISO, LORA_CS, LORA_RST, LORA_IRQ);

const gpio_num_t BLINK_GPIO = gpio_num_t::GPIO_NUM_2;

void blink_task(void *pvParameter)
{
	gpio_pad_select_gpio(BLINK_GPIO);
	/* Set the GPIO as a push/pull output */
	gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
	while (1)
	{
		/* Blink off (output low) */
		gpio_set_level(BLINK_GPIO, 0);
		vTaskDelay(500 / portTICK_RATE_MS);
		/* Blink on (output high) */
		gpio_set_level(BLINK_GPIO, 1);
		vTaskDelay(500 / portTICK_RATE_MS);
	}
}

int hh = 9;
int mm = 13;
int ss = 0;

void hello_task(void *pvParameter)
{
	dr.clear();
	dr.setCursor(0, 4);
	while (1)
	{
		char b[100];
		sprintf(b, "%02d:%02d:%02d", hh, mm, ss);
		dr.setCursor(0, 16);
		dr.setFont(font_05x07);
		dr.drawString(b);

		int cx = 0, cy = 0;
		dr.getCursor(cx, cy);

		printf("%d %d\n", cx, cy);

		static int n = 0;
		if (++n == 10)
		{
			printf("SI PROVA\n");
			SPI.setup();
			LoRa.setSPI(SPI);
			LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);
			if (LoRa.begin(868E6))
				printf("LORA OK\n");
			else
				printf("LORA NOT OK\n");
		}

		ss += 1;
		if (ss >= 60)
		{
			ss = 0;
			mm += 1;
			if (mm >= 60)
			{
				mm = 0;
				hh += 1;
			}
		}

		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

extern "C" void app_main()
{
	// nvs_flash_init();
	printf("inizio");

	i2c.Setup();
	OLED.setup();
	dr.setup(); // <= fa partire il task

	//LoRa.setFrequency(868E6);

	xTaskCreate(&blink_task, "hello_task", 4048, NULL, 5, NULL);
	xTaskCreate(&hello_task, "hello_task", 4048, NULL, 5, NULL);
}

////////////////////////

#if 0
class SPI
{
	spi_device_handle_t spi; // SPI handle.
	gpio_num_t clk;
	gpio_num_t mosi;
	gpio_num_t miso;
	gpio_num_t cs;
	gpio_num_t rst;
	gpio_num_t irq;

public:
	SPI(gpio_num_t clk, gpio_num_t mosi, gpio_num_t miso, gpio_num_t cs, gpio_num_t rst, gpio_num_t irq)
		: clk(clk), mosi(mosi), miso(miso), cs(cs), rst(rst), irq(irq) {}

	void setup()
	{
		int DMA_CHAN = 2;

		spi_bus_config_t bus_config;
		memset(&bus_config, 0, sizeof(bus_config));
		bus_config.sclk_io_num = this->clk;  // CLK
		bus_config.mosi_io_num = this->mosi; // MOSI
		bus_config.miso_io_num = this->miso; // MISO
		bus_config.quadwp_io_num = -1;       // Not used
		bus_config.quadhd_io_num = -1;       // Not used
		bus_config.max_transfer_sz = 1000;   // LEO qui per gli LCD ci mettono la grandezza del display in pixel.
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

	uint32_t lcd_get_id(int sz)
	{
		//get_id cmd
		lcd_cmd(0x04);

		spi_transaction_t t;
		memset(&t, 0, sizeof(t));
		t.length = 8 * sz;
		t.flags = SPI_TRANS_USE_RXDATA; // Receive into rx_data member of spi_transaction_t instead into memory at rx_buffer.
		t.user = (void *)1;             // serve per pilotare il D/C - tramite il pre_cb 

		ESP_ERROR_CHECK(spi_device_polling_transmit(this->spi, &t));
		return *(uint32_t *)t.rx_data;
	}

	/* Send a command to the LCD. Uses spi_device_polling_transmit, which waits
	* until the transfer is complete.
	*
	* Since command transactions are usually small, they are handled in polling
	* mode for higher speed. The overhead of interrupt transactions is more than
	* just waiting for the transaction to complete.
	*/
	void lcd_cmd(uint8_t cmd)
	{
		spi_transaction_t t;
		memset(&t, 0, sizeof(t));                                    //Zero out the transaction
		t.length = 8;                                                //Command is 8 bits
		t.tx_buffer = &cmd;                                          //The data is the cmd itself
		t.user = (void *)0;                                          //D/C needs to be set to 0
		ESP_ERROR_CHECK(spi_device_polling_transmit(this->spi, &t)); //Transmit!
	}

	/* Send data to the LCD. Uses spi_device_polling_transmit, which waits until the
	* transfer is complete.
	*
	* Since data transactions are usually small, they are handled in polling
	* mode for higher speed. The overhead of interrupt transactions is more than
	* just waiting for the transaction to complete.
	*/
	void lcd_data(const uint8_t *data, int len)
	{
		if (len == 0)
			return; //no need to send anything
		spi_transaction_t t;
		memset(&t, 0, sizeof(t));                                    //Zero out the transaction
		t.length = len * 8;                                          //Len is in bytes, transaction length is in bits.
		t.tx_buffer = data;                                          //Data
		t.user = (void *)1;                                          //D/C needs to be set to 1
		ESP_ERROR_CHECK(spi_device_polling_transmit(this->spi, &t)); //Transmit!
	}

	
};

#endif