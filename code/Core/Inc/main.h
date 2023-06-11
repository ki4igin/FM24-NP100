#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f3xx_hal.h"
#include "core_cm4.h"

#define SYS_CLOCK       72000000 // 72MHz
#define SIZE_BUFFER_ADC 128
#define UART_RX_NBUF    4
#define TIM2_ARR        500                 // for DAC
#define TIM4_ARR        125                 // for ADC
#define ADC_PER_DAC     TIM2_ARR / TIM4_ARR // ADC_PERIODS_PER_DAC_PERIOD
// #define FREQ_DAC           SYS_CLOCK/TIM2_ARR
// #define FREQ_ADC           SYS_CLOCK/TIM4_ARR
#define MAX_DAC_PERIODS 4
#define MAX_ADC_PERIODS MAX_DAC_PERIODS *ADC_PER_DAC
#define UART_BAUD_RATE  115200


enum __attribute__((packed)) command {
    COMMAND_START = 1,
    COMMAND_STOP = 2,
    COMMAND_RESET = 3,
    COMMAND_TEST = 4,
    COMMAND_RAMP = 5,
    COMMAND_AMP = 6
};

struct message_ADC {
    struct preamble {
        uint8_t start_byte;
        uint8_t period_number;
        uint16_t message_size;
    } preamble;

    uint32_t BUFF[SIZE_BUFFER_ADC * MAX_ADC_PERIODS];
};

struct flags {
    uint32_t rx               :1;
    uint32_t data_adc_collect :1;
    uint32_t adc_start        :1;
};

void Error_Handler(void);

void ADC1_2_Dual_Init(void);
void DMA1_Channel1_IRQHandler(void);
void DMA2_Channel3_IRQHandler(void);
void TIM2_IRQHandler(void);
void TIM2_Init(void);
void TIM3_Init(void);
void TIM4_Init(void);
void TIM3_IRQHandler(void);
void DAC1_Init(void);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void Opamp_Enable(OPAMP_TypeDef *opamp);
void Collect_ADC_Complete(void);
void Enable_DAC_ADC(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
