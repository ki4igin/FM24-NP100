#include "main.h"

extern UART_HandleTypeDef huart1;

extern volatile uint16_t Triangle_DAC[SIZE_BUFFER_DAC];
extern volatile uint16_t count_dma_period;
extern volatile uint8_t period_number_dac;
extern volatile uint8_t count_dac_period;

extern uint8_t UART_command[SIZE_UART_RX];

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
void Collect_ADC_Complete(struct flags flags_temp)
{
    if (flags_temp.data_adc_collect) {
        flags.data_adc_collect = 0;
        count_dac_period = 0;
        UART_command[0] = 0;
        UART_command[1] = 0;
        flags.en_adc_dac = 1;
        message_ADC12.preamble.start_byte = start_byte;
        message_ADC12.preamble.period_number = period_number_dac;
        message_ADC12.preamble.message_size = SIZE_BUFFER_ADC * count_dma_period * (sizeof(uint32_t) / sizeof(uint8_t)); // bytes;
        count_dma_period = 0;
        HAL_UART_Transmit_IT(&huart1, (uint8_t *)&message_ADC12, sizeof(message_ADC12));
    }
    return;
}

/**
@brief Generates a ramp waveform based on the specified parameters.
@param ramp: Type of ramp waveform (COMMAND_RAMP1 or COMMAND_RAMP2)
@param ampl: Amplitude of the ramp waveform
@retval None
@note This function generates a ramp waveform based on the specified parameters.
*/
void Make_Ramp(uint8_t ramp, uint16_t ampl)
{
    if (ramp == COMMAND_RAMP1) {
        int k_ramp = ((2 * ampl) / SIZE_BUFFER_DAC) + 1;
        for (uint16_t i = 0; i < SIZE_BUFFER_DAC; i++) {
            if (i < SIZE_BUFFER_DAC / 2 + 1) {
                Triangle_DAC[i] = (0 + i * k_ramp);
            } else {
                Triangle_DAC[i] = Triangle_DAC[SIZE_BUFFER_DAC - i];
            }
        }
    } else // for RAMP2
    {
        int k_ramp = ((1 * ampl) / SIZE_BUFFER_DAC) + 1;
        for (uint16_t i = 0; i < SIZE_BUFFER_DAC; i++) {
            Triangle_DAC[i] = i * k_ramp;
        }
    }
    MODIFY_REG(DMA2_Channel3->CMAR, DMA_CMAR_MA, (uint32_t)(Triangle_DAC)); // Adress of buffer
}

/**
@brief Enables DAC and ADC based on the specified flags.
@param flags_temp: Temporary flags structure
@retval None
@note This function enables the DAC and ADC based on the specified flags.
*/
void Enable_DAC_ADC(struct flags flags_temp)
{
    if (flags_temp.en_adc_dac == 1) {
        SET_BIT(TIM2->CR1, TIM_CR1_CEN);
        SET_BIT(TIM4->CR1, TIM_CR1_CEN);
        period_number_dac = UART_command[1];
        flags_temp.en_adc_dac = 0;
    }
}
