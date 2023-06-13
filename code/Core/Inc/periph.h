#ifndef __PERIPH_H
#define __PERIPH_H

#include "stm32f3xx.h"
#include "gen.h"
#include "main.h"

#define ADC_DAC_MAX_FREQ 2000000

enum freq {
    FREQ_2M = 0,
    FREQ_1M,
    FREQ_500K,
    FREQ_250K,
    FREQ_125K,
    FREQ_62500,
    FREQ_31250,
    FREQ_15625,
    FREQ_7812H5,
    FREQ_3906H25,
    FREQ_1953H125,
    FREQ_976H5625,
    FREQ_488H28125,
    FREQ_244H140625,
    FREQ_122H0703125,
    FREQ_65H03515625,
    FREQ_30H517578125,
    FREQ_15H2587890625,
};

void ADC12_Dual_Init(void);
void TIM2_Init(void);
void TIM4_Init(void);
void DAC1_Init(void);

inline static uint32_t Freq_To_TimArr(enum freq freq)
{
    uint32_t max_tim_arr = (SYSTEM_CORE_CLOCK / ADC_DAC_MAX_FREQ);
    return max_tim_arr * (1 << freq);
}

inline static void TIM_Change_ARR(TIM_TypeDef *TIMx, uint32_t arr)
{
    WRITE_REG(TIMx->ARR, arr);
}

inline static void ADC12_Dual_Change_Len(uint32_t len)
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
inline static void ADC12_Dual_Start(uint32_t len)
{
    ADC12_Dual_Change_Len(len);
    SET_BIT(TIM4->EGR, TIM_EGR_UG);
    SET_BIT(TIM4->CR1, TIM_CR1_CEN);
}

inline static void ADC12_Dual_Stop(void)
{
    CLEAR_BIT(TIM4->CR1, TIM_CR1_CEN);
}

inline static void ADC12_Dual_Change_Fd(enum freq fd)
{
    if (fd > FREQ_125K) {
        return;
    }

    uint32_t arr = Freq_To_TimArr(fd);
    TIM_Change_ARR(TIM4, arr);
}

// DAC стартует по переполнению таймера TIM2
inline static void DAC1_Start(void)
{
    SET_BIT(TIM2->EGR, TIM_EGR_UG);
    SET_BIT(TIM2->CR1, TIM_CR1_CEN);
}

inline static void DAC1_Stop(void)
{
    CLEAR_BIT(TIM2->CR1, TIM_CR1_CEN);
}

inline static void DAC1_Change_Fm(enum freq fm)
{
    if ((fm < FREQ_3906H25) || (fm > FREQ_15H2587890625)) {
        return;
    }

    // (32 - __CLZ(GEN_BUF_SIZE) - 1) эквивалентно log2(GEN_BUF_SIZE)
    enum freq dac_fd = fm - (32 - __CLZ(GEN_BUF_SIZE) - 1);

    uint32_t arr = Freq_To_TimArr(dac_fd);
    TIM_Change_ARR(TIM2, arr);
}

#endif