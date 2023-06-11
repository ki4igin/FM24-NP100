#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f3xx_hal.h"

#define SYS_CLOCK         72000000 // 72MHz
#define DAC_AMP_CODE_INIT 200
#define SIZE_BUFFER_ADC   128
#define UART_RX_NBUF      4
#define TIM2_ARR          500                   // for DAC
#define TIM4_ARR          125                   // for ADC
#define ADC_PER_DAC       (TIM2_ARR / TIM4_ARR) // ADC_PERIODS_PER_DAC_PERIOD
// #define FREQ_DAC           SYS_CLOCK/TIM2_ARR
// #define FREQ_ADC           SYS_CLOCK/TIM4_ARR
#define MAX_DAC_PERIODS 4
#define MAX_ADC_PERIODS (MAX_DAC_PERIODS * ADC_PER_DAC)
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
        uint8_t id;
        uint8_t number_periods;
        uint16_t size;
    } preamble;

    uint32_t BUFF[SIZE_BUFFER_ADC * MAX_ADC_PERIODS];
};

struct flags {
    uint32_t is_new_cmd       :1;
    uint32_t data_adc_collect :1;
    uint32_t start_req        :1;
};

extern volatile uint32_t number_periods;
extern volatile uint32_t count_dma_period;
extern volatile uint32_t count_periods;
extern volatile uint32_t BUFF_ADC1_2[SIZE_BUFFER_ADC];
extern struct message_ADC message_ADC12;
extern UART_HandleTypeDef huart1;
extern volatile struct flags flags;

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
