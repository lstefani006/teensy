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

int main(void)
{
	GPIO_InitTypeDef  gpio_init;
  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	gpio_init.GPIO_Pin = GPIO_Pin_13;
	gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &gpio_init);

	for (;;)
	{
		/* Busy delay */
		for (i = 0; i < 1000; ++i)
		{
			if (i % 2 == 0)
			{
				n ++;
			}
		}

		/* LED toggle */
		GPIOC->ODR ^= GPIO_Pin_13;
	}
}


void assert_failed(uint8_t* file, uint32_t line)
{
	for (;;) {}
}
