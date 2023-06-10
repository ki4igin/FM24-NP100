#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f3xx_hal.h"
#include "core_cm4.h"

#define SYS_CLOCK               72000000   // 72MHz
#define SIZE_BUFFER_ADC           128
#define SIZE_BUFFER_DAC           128
#define SIZE_UART_RX               4
#define TIM2_ARR                  500  //for DAC
#define TIM8_ARR                  125  //for ADC
#define ADC_PER_DAC        TIM2_ARR/TIM8_ARR // ADC_PERIODS_PER_DAC_PERIOD
//#define FREQ_DAC           SYS_CLOCK/TIM2_ARR
//#define FREQ_ADC           SYS_CLOCK/TIM8_ARR
#define MAX_DAC_PERIODS            4 
#define MAX_ADC_PERIODS  MAX_DAC_PERIODS*ADC_PER_DAC
#define UART_BAUD_RATE          3000000


enum
{
  START_COMMAND    = 1,
  STOP_COMMAND     = 2,
  RESET_COMMAND    = 3,
  TEST_COMMAND     = 4,
  RAMP1_COMMAND    = 5,
  RAMP2_COMMAND    = 6,
  AMPL_COMMAND     = 7
};

struct message_ADC
{
	struct preamble
	{
		uint8_t start_byte;
		uint8_t period_number;
		uint16_t message_size;
	}preamble;
	uint32_t BUFF[SIZE_BUFFER_ADC*MAX_ADC_PERIODS];  
};

struct flags
{
	uint32_t en_adc_dac : 1;
	uint32_t rx : 1;
	uint32_t data_adc_collect : 1;
	uint32_t adc_start : 1;
};

void Error_Handler(void);

void ADC1_2_Dual_Init(void);
void DMA1_Channel1_IRQHandler(void);
void DMA2_Channel3_IRQHandler(void);
void TIM8_UP_IRQHandler(void);
void TIM8_Init(void);
void TIM2_IRQHandler(void);
void TIM2_Init(void);
void TIM3_Init(void);
void TIM3_IRQHandler(void);
void DAC1_Init(void);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void Opamp_Start(OPAMP_TypeDef* opamp);
void Make_Ramp(uint8_t ramp, uint16_t ampl);
void Collect_ADC_Complete(struct flags flags_temp);
void Enable_DAC_ADC(struct flags flags_temp);


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
