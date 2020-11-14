#pragma once
#include <string.h>

#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/i2c.h"

class ESP32_I2C
{
	i2c_port_t _port;
	gpio_num_t _sda;
	gpio_num_t _scl;
	gpio_num_t _rst;
	int _addr;

public:
	ESP32_I2C(i2c_port_t port, gpio_num_t sda, gpio_num_t scl, gpio_num_t rst, int addr)
	{
		this->_port = port; // I2C_NUM_0
		this->_sda = sda;
		this->_scl = scl;
		this->_rst = rst;
		this->_addr = addr;
	}

	bool Setup()
	{
		if (this->_rst != gpio_num_t::GPIO_NUM_NC)
		{
			gpio_pad_select_gpio(this->_rst);
			gpio_set_direction(this->_rst, GPIO_MODE_OUTPUT);

			gpio_set_level(this->_rst, 1);
			vTaskDelay(1);					   // VDD goes high at start10 pause for 1 ms
			gpio_set_level(this->_rst, 0);	   // Bring reset low
			vTaskDelay(10 / portTICK_RATE_MS); // Wait 10ms
			gpio_set_level(this->_rst, 1);	   // Bring out of reset
			vTaskDelay(1);					   // diamogli un po' di tempo,
		}

		i2c_config_t conf;
		memset(&conf, 0, sizeof(conf));
		conf.mode = I2C_MODE_MASTER;
		conf.sda_io_num = this->_sda;
		conf.scl_io_num = this->_scl;
		conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
		conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
		conf.master.clk_speed = 100000;
		ESP_ERROR_CHECK(i2c_param_config(this->_port, &conf));

		// install the driver
		ESP_ERROR_CHECK(i2c_driver_install(this->_port, I2C_MODE_MASTER, 0, 0, 0));

		// faccio una sorta di ricerca del bus i2c, solo per l'indirizzo del display.
		// se risponde bene.
		i2c_cmd_handle_t cmd = i2c_cmd_link_create();
		ESP_ERROR_CHECK(i2c_master_start(cmd));
		ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (this->_addr << 1) | I2C_MASTER_WRITE, true));
		ESP_ERROR_CHECK(i2c_master_stop(cmd));
		auto erc = i2c_master_cmd_begin(this->_port, cmd, 1000 / portTICK_RATE_MS);
		i2c_cmd_link_delete(cmd);

		return erc == ESP_OK;
	}
	void Destroy()
	{
		ESP_ERROR_CHECK(i2c_driver_delete(this->_port));
	}
	void Write(const uint8_t *data_array, int data_size)
	{
		i2c_cmd_handle_t cmd = i2c_cmd_link_create();
		ESP_ERROR_CHECK(i2c_master_start(cmd));
		ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (_addr << 1) | I2C_MASTER_WRITE, true));
		ESP_ERROR_CHECK(i2c_master_write(cmd, const_cast<uint8_t *>(data_array), data_size, true));
		ESP_ERROR_CHECK(i2c_master_stop(cmd));
		ESP_ERROR_CHECK(i2c_master_cmd_begin(this->_port, cmd, 1000 / portTICK_RATE_MS));
		i2c_cmd_link_delete(cmd);
	}
	void Write(uint8_t data, const uint8_t *data_array, int data_size)
	{
		i2c_cmd_handle_t cmd = i2c_cmd_link_create();
		ESP_ERROR_CHECK(i2c_master_start(cmd));
		ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (_addr << 1) | I2C_MASTER_WRITE, true));
		ESP_ERROR_CHECK(i2c_master_write_byte(cmd, data, true));
		ESP_ERROR_CHECK(i2c_master_write(cmd, const_cast<uint8_t *>(data_array), data_size, true));
		ESP_ERROR_CHECK(i2c_master_stop(cmd));
		ESP_ERROR_CHECK(i2c_master_cmd_begin(this->_port, cmd, 1000 / portTICK_RATE_MS));
		i2c_cmd_link_delete(cmd);
	}
};
