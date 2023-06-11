#include "main.h"

extern UART_HandleTypeDef huart1;

extern volatile uint16_t count_dma_period;
extern volatile uint8_t count_periods;

extern uint8_t uart_buf[UART_RX_NBUF];

extern struct flags flags;
extern struct message_ADC message_ADC12;

const uint8_t start_byte = 0x01;

/**
@brief This function handles the completion of ADC data collection.
@param flags_temp: Current status of flags
@retval None
@note
      This function is called when the ADC data collection is complete.
      It prepares the data for transmission over UART and initiates the transmission process.
*/
void Collect_ADC_Complete(void)
{
    message_ADC12.preamble.start_byte = start_byte;
    message_ADC12.preamble.period_number = number_periods;
    message_ADC12.preamble.message_size = SIZE_BUFFER_ADC * count_dma_period * sizeof(uint32_t); // bytes;
    HAL_UART_Transmit_IT(
        &huart1,
        (uint8_t *)&message_ADC12,
        sizeof(message_ADC12.preamble) + message_ADC12.preamble.message_size);
}

/**
@brief Enables DAC and ADC based on the specified flags.
@param flags_temp: Temporary flags structure
@retval None
@note This function enables the DAC and ADC based on the specified flags.
*/
void Enable_DAC_ADC(void)
{
    if (READ_BIT(TIM4->CR1, TIM_CR1_CEN) == 0) {
        // count_dma_period = 0;
        // SET_BIT(TIM2->CR1, TIM_CR1_CEN);
        // SET_BIT(TIM4->CR1, TIM_CR1_CEN);
        start_req = 1;
    }
}
