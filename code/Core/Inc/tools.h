#ifndef __TOOLS_H
#define __TOOLS_H

#include "stm32f3xx.h"

#define ADC_REF_mV 3300

typedef uint32_t u32_x_t;
typedef u32_x_t u32_4_t;

inline static uint32_t volt2code(u32_x_t volt, u32_x_t ref)
{
    return 4095 * ref / volt;
}

// deviation_freq - девиация частоты в кГц, от 0 до 200000
// sensitivity - чувствительность в кГц/В, до 2000000
inline static uint32_t df2code(uint32_t deviation_freq_kHz,
                               uint32_t sensitivity_kHz_V)
{
    u32_4_t deviation_freq = deviation_freq_kHz << 4;
    u32_4_t amp_mv = 1000 * deviation_freq / sensitivity_kHz_V;
    u32_4_t ref_mv = ADC_REF_mV << 4;

    return volt2code(amp_mv, ref_mv);
}

#endif