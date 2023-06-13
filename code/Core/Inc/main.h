#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f3xx_hal.h"

#define DAC_AMP_CODE_INIT      200
#define SYSTEM_CORE_CLOCK      72000000
#define UART_RX_NBUF           4

#define ADC_BUF_LEN_MAX        8192
#define ADC_REF_mV             3300
#define DEVIATION_FREQ_MAX_kHz 200000
#define SENSITIVITY_VCO_kHz    760000
#define UART_BAUD_RATE         115200

struct pac_adc {
    struct preamble {
        uint32_t id   :8;
        uint32_t size :24;
    } preamble;

    union {
        struct {
            uint16_t adc1;
            uint16_t adc2;
        };

        uint32_t adc12;
    } data[ADC_BUF_LEN_MAX];
};

struct flags {
    uint32_t is_new_cmd       :1;
    uint32_t adc_data_collect :1;
    uint32_t start_req        :1;
};

extern uint32_t adc_number_samples;
extern struct pac_adc pac_adc;
extern UART_HandleTypeDef huart1;
extern volatile struct flags flags;

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
