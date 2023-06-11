#ifndef __PERIPH_H
#define __PERIPH_H

#include "stm32f3xx.h"

void ADC1_2_Dual_Init(void);
void TIM2_Init(void);
void TIM3_Init(void);
void TIM4_Init(void);
void DAC1_Init(void);

// ADC стартует по переполнению таймера TIM4
inline static void adc_start(void)
{
    SET_BIT(TIM4->EGR, TIM_EGR_UG);
    SET_BIT(TIM4->CR1, TIM_CR1_CEN);
}

inline static void adc_stop(void)
{
    CLEAR_BIT(TIM4->CR1, TIM_CR1_CEN);
}

// DAC стартует по переполнению таймера TIM2
inline static void dac_start(void)
{
    SET_BIT(TIM2->EGR, TIM_EGR_UG);
    SET_BIT(TIM2->CR1, TIM_CR1_CEN);
}

inline static void dac_stop(void)
{
    CLEAR_BIT(TIM2->CR1, TIM_CR1_CEN);
}

#endif