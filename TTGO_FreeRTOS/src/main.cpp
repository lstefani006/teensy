#include <stdio.h>

extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
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
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_SCK 5
#define LORA_CS 18
#define LORA_IRQ 26 // LORA_DIO0
#define LORA_RST 14
#define LORA_BAND 868E6

// https://github.com/sandeepmistry/arduino-LoRa/blob/master/API.md
// http://www.lilygo.cn/prod_view.aspx?TypeId=50003&Id=1142&FId=t3:50003:3

ESP32_I2C i2c(I2C_NUM_0, OLED_SDA, OLED_SCL, OLED_RST, 0x3C);
SD1306 OLED(i2c, SCREEN_WIDTH, SCREEN_HEIGHT);

SD1306_Driver dr(OLED);

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
        /*
        OLED.clear();
        OLED.drawString(b);
        OLED.refresh();
        */
        //dr.clear();
        dr.setCursor(0, 16);
        dr.setFont(font_05x07);
        dr.drawString(b);

        int cx=0, cy=0;
        dr.getCursor(cx, cy);

        printf("%d %d\n", cx, cy);

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

    xTaskCreate(&blink_task, "hello_task", 4048, NULL, 5, NULL);
    xTaskCreate(&hello_task, "hello_task", 4048, NULL, 5, NULL);
}
