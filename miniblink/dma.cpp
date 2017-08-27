#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/dac.h>

#include <Arduino.h>


class Dma
{
public:
	void begin(void *s1, void *s2)
	{
		uint32_t dma = DMA1;
		uint8_t dma_channel = DMA_CHANNEL1;

		rcc_periph_clock_enable(RCC_DMA1);

		/* MEM2MEM mode for channel 1. */
		dma_enable_mem2mem_mode(dma, dma_channel);

		/* Highest priority. */
		dma_set_priority(dma, dma_channel, DMA_CCR_PL_VERY_HIGH);

		/* 32Bit wide transfer for source and destination. */
		dma_set_memory_size(dma, dma_channel, DMA_CCR_MSIZE_32BIT);
		dma_set_peripheral_size(dma, dma_channel, DMA_CCR_PSIZE_32BIT);

		/* After every 32bits we have to increase the address because we use RAM.  */
		dma_enable_memory_increment_mode(dma, dma_channel);
		dma_enable_peripheral_increment_mode(dma, dma_channel);

		/* We define the source as peripheral. */
		dma_set_read_from_memory(dma, dma_channel);

		/* We want to transfer string s1. */
		dma_set_peripheral_address(dma, dma_channel, (uint32_t)s1);

		/* Destination should be string s2. */
		dma_set_memory_address(dma, dma_channel, (uint32_t)s2);

		/* Set number of DATA to transfer.
		 * Remember that this means not necessary bytes but MSIZE or PSIZE
		 * depending on your source device.  */
		dma_set_number_of_data(dma, dma_channel, 5);

		/* Start DMA transfer. */
		dma_enable_channel(dma, dma_channel);

		/* TODO: Write a function to get the interrupt flags. */
		while (!(DMA_ISR(dma) & 0x0000001))
			;

		dma_disable_channel(dma, dma_channel);

	}
};

////// DMA DAC
static void timer_setup(int PERIOD)
{
	/* Enable TIM2 clock. */
	rcc_periph_clock_enable(RCC_TIM2);
	timer_reset(TIM2);

	/* Timer global mode: - No divider, Alignment edge, Direction up */
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	timer_continuous_mode(TIM2);
	timer_set_period(TIM2, PERIOD);

	timer_disable_oc_output(TIM2, TIM_OC2);
	timer_disable_oc_output(TIM2, TIM_OC3);
	timer_disable_oc_output(TIM2, TIM_OC4);
	timer_enable_oc_output(TIM2, TIM_OC1);

	timer_disable_oc_clear(TIM2, TIM_OC1);
	timer_disable_oc_preload(TIM2, TIM_OC1);

	timer_set_oc_slow_mode(TIM2, TIM_OC1);
	timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_TOGGLE);
	timer_set_oc_value(TIM2, TIM_OC1, 500);
	timer_disable_preload(TIM2);
	/* Set the timer trigger output (for the DAC) to the channel 1 output compare */
	timer_set_master_mode(TIM2, TIM_CR2_MMS_COMPARE_OC1REF);
	timer_enable_counter(TIM2);
}

static void dma_setup(void)
{
	uint32_t dma = DMA1;
	uint8_t dma_channel = DMA_CHANNEL1;
	uint8_t waveform[256] { 0,1,2,3,};
	uint16_t waveform_sz = 256;


	/* DAC channel 1 uses DMA controller 1 Stream 5 Channel 7. */
	/* Enable DMA1 clock and IRQ */
	switch (dma)
	{
	case DMA1: 
		rcc_periph_clock_enable(RCC_DMA1);
		switch (dma_channel)
		{
		case DMA_CHANNEL1: nvic_enable_irq(NVIC_DMA1_CHANNEL1_IRQ); break;
		case DMA_CHANNEL2: nvic_enable_irq(NVIC_DMA1_CHANNEL2_IRQ); break;
		case DMA_CHANNEL3: nvic_enable_irq(NVIC_DMA1_CHANNEL3_IRQ); break;
		case DMA_CHANNEL4: nvic_enable_irq(NVIC_DMA1_CHANNEL4_IRQ); break;
		case DMA_CHANNEL5: nvic_enable_irq(NVIC_DMA1_CHANNEL5_IRQ); break;
		case DMA_CHANNEL6: nvic_enable_irq(NVIC_DMA1_CHANNEL6_IRQ); break;
		case DMA_CHANNEL7: nvic_enable_irq(NVIC_DMA1_CHANNEL7_IRQ); break;
		default: HALT;
		}
		break;

	case DMA2:
		rcc_periph_clock_enable(RCC_DMA2);
		switch (dma_channel)
		{
		case DMA_CHANNEL1: nvic_enable_irq(NVIC_DMA2_CHANNEL1_IRQ); break;
		case DMA_CHANNEL2: nvic_enable_irq(NVIC_DMA2_CHANNEL2_IRQ); break;
		case DMA_CHANNEL3: nvic_enable_irq(NVIC_DMA2_CHANNEL3_IRQ); break;
		case DMA_CHANNEL4: nvic_enable_irq(NVIC_DMA2_CHANNEL4_5_IRQ); break;
		case DMA_CHANNEL5: nvic_enable_irq(NVIC_DMA2_CHANNEL5_IRQ); break;
		default: HALT;
		}
		break;

	default:
		HALT;
	}

	dma_channel_reset(dma, dma_channel);

	dma_set_priority(dma, dma_channel, DMA_CCR_PL_LOW);
	dma_enable_circular_mode(dma, dma_channel);
	dma_set_read_from_memory(dma, dma_channel);
	//dma_set_read_from_peripheral(dma, dma_channel);
	dma_set_number_of_data(dma, dma_channel, waveform_sz);

	dma_enable_memory_increment_mode(dma, dma_channel);
	dma_set_memory_size(dma, dma_channel, DMA_CCR_MSIZE_8BIT);
	dma_set_memory_address(dma, dma_channel, (uint32_t) waveform);

	dma_disable_peripheral_increment_mode(dma, dma_channel);
	dma_set_peripheral_size(dma, dma_channel, DMA_CCR_MSIZE_8BIT);
	dma_set_peripheral_address(dma, dma_channel, (uint32_t) &DAC_DHR8RD);

	dma_enable_transfer_complete_interrupt(dma, dma_channel);
	dma_enable_channel(dma, dma_channel);
}

/*--------------------------------------------------------------------*/
static void dac_setup(void)
{
	/* Enable the DAC clock on APB1 */
	rcc_periph_clock_enable(RCC_DAC);
	/* Setup the DAC channel 1, with timer 2 as trigger source.
	 * Assume the DAC has woken up by the time the first transfer occurs */
	dac_trigger_enable(CHANNEL_1);
	dac_set_trigger_source(DAC_CR_TSEL1_T2);
	dac_dma_enable(CHANNEL_1);
	dac_enable(CHANNEL_1);
}
