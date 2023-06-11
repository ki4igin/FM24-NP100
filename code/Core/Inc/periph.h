#ifndef __PERIPH_H
#define __PERIPH_H

#include "stm32f3xx.h"

void ADC1_2_Dual_Init(void);
void TIM2_Init(void);
void TIM4_Init(void);
void DAC1_Init(void);

inline static void ADC1_2_Dual_Change_Len(uint32_t len)
{
    // перед изменением длины посылки DMA, его нужно выключить
    uint32_t dma_ccr_en = READ_BIT(DMA1_Channel1->CCR, DMA_CCR_EN);
    CLEAR_BIT(DMA1_Channel1->CCR, DMA_CCR_EN);
    MODIFY_REG(DMA1_Channel1->CNDTR,
               DMA_CNDTR_NDT_Msk,
               (len << DMA_CNDTR_NDT_Pos));
    SET_BIT(DMA1_Channel1->CCR, dma_ccr_en);
}

// ADC стартует по переполнению таймера TIM4
inline static void adc_start(uint32_t len)
{
    ADC1_2_Dual_Change_Len(len);
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