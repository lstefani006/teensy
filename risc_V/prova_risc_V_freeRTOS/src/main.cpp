extern "C"
{
#include "gd32vf103.hpp"
}

#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include "lcd.hpp"

#define LED_RED /*PC13*/ 0
#define LED_GREEN /*PA1*/ 1
#define LED_BLUE /*PA2*/ 2

void led_on(int led)
{
	if (led == LED_RED)
		gpio_bit_reset(GPIOC, GPIO_PIN_13);
	else if (led == LED_GREEN)
		gpio_bit_reset(GPIOA, GPIO_PIN_1);
	else if (led == LED_BLUE)
		gpio_bit_reset(GPIOA, GPIO_PIN_2);
}
void led_off(int led)
{
	if (led == LED_RED)
		gpio_bit_set(GPIOC, GPIO_PIN_13);
	else if (led == LED_GREEN)
		gpio_bit_set(GPIOA, GPIO_PIN_1);
	else if (led == LED_BLUE)
		gpio_bit_set(GPIOA, GPIO_PIN_2);
}

//////////////////////////////////////////

QueueHandle_t xQueue = nullptr;
void Draw_Send(Draw_Base *d)
{
	xQueueSend(xQueue, &d, portMAX_DELAY);
}
static void LCD_Task(void *pv)
{
	for (;;)
	{
		Draw_Base *p = nullptr;
		xQueueReceive(xQueue, &p, portMAX_DELAY);
		p->Draw();
		delete p;
	}
}

////////////////////////////////////////////

static void TaskLed(void *pvParameters)
{
	int n = *(int *)pvParameters;
	TickType_t xLastWakeTime = xTaskGetTickCount();

	int pp = 0;
	for (;;)
	{
		led_on(n);
		vTaskDelayUntil(&xLastWakeTime, (1000 / portTICK_PERIOD_MS / (n + 1)));
		led_off(n);
		vTaskDelayUntil(&xLastWakeTime, (1000 / portTICK_PERIOD_MS / (n + 1)));

		char b[20];
		sprintf(b, "%d", ++pp);
		//LCD_ShowString(0, 0, (const u8 *)b, WHITE);

		u16 color = BLACK;
		if (n == LED_RED)
			color = RED;
		if (n == LED_GREEN)
			color = GREEN;
		if (n == LED_BLUE)
			color = BLUE;

		Draw_List *d = new Draw_List();
		d->Add(new Draw_Clear(color));
		d->Add(new Draw_ShowString(0, 0, (const u8 *)b, WHITE));
		Draw_Send(d);
	}
}

void init_uart0(void)
{
	/* enable GPIO clock */
	rcu_periph_clock_enable(RCU_GPIOA);

	/* enable USART clock */
	rcu_periph_clock_enable(RCU_USART0);

	/* connect port to USARTx_Tx */
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
	/* connect port to USARTx_Rx */
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

	/* USART configure */
	usart_deinit(USART0);
	usart_baudrate_set(USART0, 115200U);
	usart_word_length_set(USART0, USART_WL_8BIT);
	usart_stop_bit_set(USART0, USART_STB_1BIT);
	usart_parity_config(USART0, USART_PM_NONE);
	usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
	usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
	usart_receive_config(USART0, USART_RECEIVE_ENABLE);
	usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
	usart_enable(USART0);

	usart_interrupt_enable(USART0, USART_INT_RBNE);
}

int main()
{
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOC);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1 | GPIO_PIN_2);
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);

	//	init_uart0();

	LCD_Init();
	LCD_Clear(BLACK);
	LCD_BACK_COLOR = BLACK;
	LCD_setRotation(1);
	char b[20];
	sprintf(b, "ciao");
	LCD_ShowString(0, 0, (const u8 *)b, WHITE);

	led_off(LED_RED);
	led_off(LED_GREEN);
	led_off(LED_BLUE);

	int led[] = {LED_RED, LED_GREEN, LED_BLUE};

	xQueue = xQueueCreate(10, sizeof(Draw_Base *));
	xTaskCreate(LCD_Task, "LCD_Task", 256, nullptr, 3, nullptr);

	xTaskCreate(TaskLed, "yellowLED", 256, &led[0], 3, NULL);
	xTaskCreate(TaskLed, "yellowLED", 256, &led[1], 3, NULL);
	xTaskCreate(TaskLed, "yellowLED", 256, &led[2], 3, NULL);
	vTaskStartScheduler();
	for (;;)
	{
	}
	return 0;
}
