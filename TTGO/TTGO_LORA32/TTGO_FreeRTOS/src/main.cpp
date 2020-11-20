#include <stdio.h>

#include "Arduino.h"
#include "LoRa.hpp"

#include <iostream>
#include <vector>
#include <sstream>

#include "FT.hpp"

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
SPIClass SPI(LORA_SCK, LORA_MOSI, LORA_MISO, LORA_CS, LORA_RST, LORA_IRQ);

const gpio_num_t BLINK_GPIO = gpio_num_t::GPIO_NUM_2;

struct BlinkTask : public FT::Task
{
	void Run()
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
};

FT::Queue<void> loraIrqEvent(2);

void onLoraIrq()
{
	loraIrqEvent.SendFromISR();
}

class HelloTask : public FT::Task
{
	void Run() override;
};

void HelloTask::Run()
{
	int G_time = 0;

	dr.clear();
	dr.setCursor(0, 4);

	SPI.setup();
	LoRa.setSPI(SPI);
	LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);
	if (LoRa.begin(868E6))
		printf("LORA OK\n");
	else
		printf("LORA NOT OK\n");

	LoRa.onIrq(onLoraIrq);

	//LoRa.disableInvertIQ();
	LoRa.receive();

	while (1)
	{
		if (loraIrqEvent.Receive(400) == false)
		{
			// timeout
			G_time++;
			char b[14];
			sprintf(b, "%d", G_time);
			String s(b);

			dr.setCursor(0, 8 * 6);
			dr.setFont(font_05x07);
			dr.drawString(b);
			printf("TX=%s\n", b);

			LoRa.idle();
			//LoRa.enableInvertIQ();
			LoRa.beginPacket();
			LoRa.write((const uint8_t *)s.c_str(), s.Len());
			LoRa.endPacket(/*async*/ true);
		}
		else
		{
			int nx = LoRa.HandleIrq();
			switch (nx)
			{
			case -1: // TX done
				//LoRa.disableInvertIQ();
				LoRa.receive();
				printf("TX OK\n");
				break;

			case -2: // niente da fare
				printf("CRC\n");
				break;

			default: // nx caratteri da ricevere
			{
				String r;
				for (int i = 0; i < nx; ++i)
				{
					char b[2];
					b[0] = LoRa.read();
					b[1] = 0;
					r.Append(b);
				}

				String v("RX=");
				v.Append(r.c_str());
				dr.setCursor(0, 0 * 8);
				dr.setFont(font_05x07);
				dr.drawString(v.c_str());

				auto rssi = LoRa.packetRssi();
				auto sn = LoRa.packetSnr();
				auto fe = LoRa.packetFrequencyError();

				std::ostringstream s1;
				s1 << "RSSI= " << rssi << "     ";
				std::ostringstream s2;
				s2 << "SN  = " << sn << "     ";
				std::ostringstream s3;
				s3 << "FE  = " << fe << "     ";

				dr.setCursor(0, 1 * 8);
				dr.drawString(s1.str().c_str());
				dr.setCursor(0, 2 * 8);
				dr.drawString(s2.str().c_str());
				dr.setCursor(0, 3 * 8);
				dr.drawString(s3.str().c_str());

				printf("%s\n", v.c_str());
				break;
			}
			}
		}
	}
}

BlinkTask blink;
HelloTask hello;

extern "C" void app_main()
{
	// nvs_flash_init();
	printf("inizio\n");

	i2c.Setup();
	OLED.setup();
	dr.setup(); // <= fa partire il task

	blink.Create(4048, "blink_task");
	hello.Create(4048, "hello_task");
}
