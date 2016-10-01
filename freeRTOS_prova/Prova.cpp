#include <stdlib.h>
#include <string.h>
#include <Arduino.h>

#include <Arduino_FreeRTOS.h>
#include <semphr.h>

SemaphoreHandle_t xSerialSemaphore = nullptr;

void TaskDigitalRead(void *pvParameters);
void TaskAnalogRead(void *pvParameters);


void setup()
{
	Serial.begin(9600);

	if (xSerialSemaphore == nullptr)
	{
		xSerialSemaphore = xSemaphoreCreateMutex();
		if (xSerialSemaphore != nullptr)
			xSemaphoreGive(xSerialSemaphore);
	}

	xTaskCreate(TaskDigitalRead, "DigitalRead", 128, nullptr, 2 , nullptr);
	xTaskCreate(TaskAnalogRead,  "AnalogRead",  128, nullptr, 1 , nullptr);
}

void loop()
{
}


void TaskDigitalRead(void *pvParameters __attribute__((unused)))
{
	pinMode(2, INPUT);
	for (;;)
	{
		int buttonState = digitalRead(2);

		if (xSemaphoreTake(xSerialSemaphore, 5) == pdTRUE)
		{
			Serial.println(buttonState);
			xSemaphoreGive(xSerialSemaphore);
		}

		vTaskDelay(1);
	}
}

void TaskAnalogRead(void *pvParameters __attribute__((unused)))
{
	for (;;)
	{
		int sensorValue = analogRead(A0);

		if (xSemaphoreTake(xSerialSemaphore, 5) == pdTRUE)
		{
			Serial.println(sensorValue);
			xSemaphoreGive(xSerialSemaphore);
		}
		vTaskDelay(1);
	}
}
