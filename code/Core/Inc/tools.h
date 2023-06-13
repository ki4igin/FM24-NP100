#ifndef __TOOLS_H
#define __TOOLS_H

#include "stm32f3xx.h"
#include "main.h"

typedef uint32_t q32_x_t;
typedef q32_x_t q32_4_t;

// deviation_freq - девиация частоты в кГц, от 0 до 200000
// sensitivity - чувствительность в кГц, до 2000000
inline static uint32_t df2code(uint32_t deviation_freq, uint32_t sensitivity)
{
    q32_4_t deviation_freq = deviation_freq << 4;
    q32_4_t amp_mv = 1000 * deviation_freq / sensitivity;
    q32_4_t ref_mv = ADC_REF_mV << 4;

    return volt2code(amp_mv, ref_mv);
}

inline static uint32_t volt2code(q32_x_t volt, q32_x_t rev)
{
    return 4095 * rev / volt;
}

#endif