/*
 * (C) 2014, Tamas Bondar (www.tamasbondar.info)
 */

#include "stm32f10x.h"

class Leo
{
public:
	Leo(int a);
private:
	int _v;
};

Leo::Leo(int v) {
	this->_v = v;
}

Leo aa(3);

volatile int i;
volatile int n;


//Define time keeping variables
volatile uint32_t Milliseconds = 0, Seconds = 0;

#define SysTick_Handler sys_tick_handler

// Systick interrupt handler, interrupts at “interrupt rate” per second
//Currently set to interrupt at millisecond intervals
extern "C" void SysTick_Handler(void)
{
	Milliseconds++;
	if (Milliseconds%1000 == 0)
		Seconds++;
}

void DelayMil(uint32_t MilS)
{
	volatile uint32_t MSStart = Milliseconds;
	while (Milliseconds - MSStart < MilS);
}


void ser_put(USART_TypeDef *ser, int c)
{
	while (USART_GetFlagStatus(ser, USART_FLAG_TXE) == RESET);
	ser->DR = c & 0xff;
}

#define MillisecondsIT 1000

int main(void)
{
	SystemInit(); //Ensure CPU is running at correctly set clock speed
	SystemCoreClockUpdate(); //Update SystemCoreClock variable to current clock speed
	SysTick_Config(SystemCoreClock/MillisecondsIT); //Set up a systick interrupt every millisecond


	if (true)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

		if (true)
		{
			GPIO_InitTypeDef  gpio_init;
			GPIO_StructInit(&gpio_init);
			gpio_init.GPIO_Pin = GPIO_Pin_13;
			gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
			gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_Init(GPIOC, &gpio_init);
		}
	}

	if (true)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);
		if (true)
		{
			GPIO_InitTypeDef tx;
			GPIO_StructInit(&tx);
			tx.GPIO_Pin = GPIO_Pin_9;
			tx.GPIO_Mode = GPIO_Mode_AF_PP;
			tx.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOC, &tx);
		}

		if (true)
		{
			GPIO_InitTypeDef rx;
			GPIO_StructInit(&rx);
			rx.GPIO_Pin = GPIO_Pin_10;
			rx.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			rx.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOC, &rx);
		}

		if (true)
		{
			USART_InitTypeDef ser;
			USART_StructInit(&ser);
			ser.USART_BaudRate = 38400;
			ser.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
			ser.USART_Parity = USART_Parity_No;
			USART_Init(USART1, &ser);
			USART_Cmd(USART1, ENABLE);
		}
	}

	for (;;)
	{
		/* Busy delay */
		DelayMil(300);

		//GPIO_ReadBit(GPIOC, GPIO_Pin_13);

		//sys_tick_handler
		auto v = GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13);
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, v ? Bit_RESET : Bit_SET);

		ser_put(USART1, v + '0');
		ser_put(USART1, '\n');
		ser_put(USART1, '\r');
	}
}


void assert_failed(uint8_t* file, uint32_t line)
{
	for (;;) {}
}
