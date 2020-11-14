#include <stdio.h>

#include "Arduino.h"
#include "LoRa.hpp"

#include <iostream>
#include <vector>

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

class Time
{
	int hh = 9;
	int mm = 13;
	int ss = 0;

public:
	Time();

	void Inc()
	{
		ss += 1;
		if (ss < 60)
			return;
		ss = 0;
		mm += 1;
		if (mm < 60)
			return;
		mm = 0;
		hh += 1;
		if (hh < 24)
			return;
		hh = 0;
	}

	String Print()
	{
		char b[100];
		sprintf(b, "%02d:%02d:%02d", hh, mm, ss);
		return b;
	}
};

Time::Time()
{
	hh = 0;
	mm = 0;
	ss = 0;
}

static xQueueHandle gpio_evt_queue = NULL;
bool rxIrq = true;
bool txIrq = true;

volatile bool rxDone = false;
void onReceive(int n)
{
	rxDone = true;

	static int nn = n;
	xQueueSendFromISR(gpio_evt_queue, &nn, NULL);
}

volatile bool txDone = false;
void onTxDone()
{
	txDone = true;
	
	LoRa.disableInvertIQ();
	LoRa.receive();
}

void hello_task(void *pvParameter)
{
	Time G_time;

	dr.clear();
	dr.setCursor(0, 4);

	SPI.setup();
	LoRa.setSPI(SPI);
	LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);
	if (LoRa.begin(868E6))
		printf("LORA OK\n");
	else
		printf("LORA NOT OK\n");

	if (rxIrq)
		LoRa.onReceive(onReceive);
	if (txIrq)
		LoRa.onTxDone(onTxDone);

	LoRa.disableInvertIQ();
	LoRa.receive();

	gpio_evt_queue = xQueueCreate(10, sizeof(int));
	// auto lastTx = millis() + 1000;

	while (1)
	{
		int nrx;
		if (xQueueReceive(gpio_evt_queue, &nrx, /*portMAX_DELAY*/ 1000 / portTICK_RATE_MS) == pdTRUE)
		{
			String r;
			for (int i = 0; i < nrx; ++i)
			{
				char b[2];
				b[0] = LoRa.read();
				b[1] = 0;
				r.Append(b);
			}

			String v("RX int=");
			v.Append(r.c_str());
			dr.setCursor(0, 32);
			dr.setFont(font_05x07);
			dr.drawString(v.c_str());
		}
		if (txDone)
		{
			txDone = false;
			printf("TRASMESSO\n");
		}
		if (rxDone)
		{
			rxDone = false;
			printf("RICEVUTO\n");

		}

		if (/*millis() >= lastTx*/ true)
		{

			G_time.Inc();
			dr.setCursor(0, 16);
			dr.setFont(font_05x07);
			dr.drawString(G_time.Print().c_str());
			printf("TX=%s\n", G_time.Print().c_str());

			LoRa.idle();
			LoRa.enableInvertIQ();

			auto s = G_time.Print();
			LoRa.beginPacket();
			LoRa.write((const uint8_t *)s.c_str(), s.Len());

			if (txIrq)
				LoRa.endPacket(/*async*/ true);
			else
				LoRa.endPacket(/*async*/ false);
			// lastTx = millis() + 1000;
		}

		if (!rxIrq)
		{
			auto n = LoRa.parsePacket();
			if (n)
			{
				String r("RX no int=");
				while (LoRa.available())
				{
					char b[2];
					b[0] = LoRa.read();
					b[1] = 0;
					r.Append(b);
				}

				dr.setCursor(0, 32);
				dr.setFont(font_05x07);
				dr.drawString(r.c_str());
			}
		}
	}
}

void lora_task(void *pvParameter)
{
	for (;;)
	{
		auto n = LoRa.parsePacket();
		if (n)
		{
			String r;
			while (LoRa.available())
			{
				char b[2];
				b[0] = LoRa.read();
				b[1] = 0;
				r.Append(b);
			}

			dr.setCursor(0, 32);
			dr.setFont(font_05x07);
			dr.drawString(r.c_str());
		}
	}
}

extern "C" void app_main()
{
	// nvs_flash_init();
	printf("inizio\n");

	i2c.Setup();
	OLED.setup();
	dr.setup(); // <= fa partire il task

	//LoRa.setFrequency(868E6);

	xTaskCreate(&blink_task, "blink_task", 4048, NULL, 5, NULL);
	xTaskCreate(&hello_task, "hello_task", 4048, NULL, 5, NULL);
	//xTaskCreate(&lora_task, "lora_task", 4048, NULL, 5, NULL);
}

////////////////////////

class LoRa_Driver
{
	QueueHandle_t _h;

	static void S_Task(void *pv) { ((LoRa_Driver *)pv)->Task(); }

public:
	LoRa_Driver() { _h = nullptr; }
	void setup()
	{
		_h = xQueueCreate(16, sizeof(std::vector<uint8_t> *));
		xTaskCreate(&S_Task, "LORA", 4048, this, 5, nullptr);
	}

	void sendPacket(const std::vector<uint8_t> &v)
	{
		auto d = new std::vector<uint8_t>(v);
		xQueueSend(_h, &d, portMAX_DELAY);
	}

	void Task()
	{
		for (;;)
		{
			std::vector<uint8_t> *d = nullptr;
			if (xQueueReceive(_h, &d, /*portMAX_DELAY*/ 100 / portTICK_RATE_MS) == pdTRUE)
			{
				LoRa.beginPacket();
				LoRa.write(d->data(), d->size());
				LoRa.endPacket();
				delete d;
			}
			else
			{
				auto n = LoRa.parsePacket();
				if (n)
				{
					auto b = new std::vector<uint8_t>();
					while (LoRa.available())
					{
						b->push_back(LoRa.read());
					}
				}
			}
		}
	}
};
