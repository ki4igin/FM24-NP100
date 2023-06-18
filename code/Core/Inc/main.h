#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f3xx.h"

#define DAC_AMP_CODE_INIT      200

#define ADC_BUF_LEN_MAX        8192
#define DEVIATION_FREQ_MAX_kHz 200000
#define VCO_SENSITIVITY_INIT   760000

#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0 ((uint32_t)0x00000007)
#define NVIC_PRIORITYGROUP_1 ((uint32_t)0x00000006)
#define NVIC_PRIORITYGROUP_2 ((uint32_t)0x00000005)
#define NVIC_PRIORITYGROUP_3 ((uint32_t)0x00000004)
#define NVIC_PRIORITYGROUP_4 ((uint32_t)0x00000003)
#endif

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
extern volatile struct flags flags;

void Error_Handler(void);

#endif
