#include "main.h"
#include "gen.h"

/******************************************************************************/
/* Function Name : ADC12_Dual_Init */
/* Description : Initializes ADC1 and ADC2 in dual mode. Configures the */
/* ADC settings, ADC channels, ADC triggers, DMA1 Channel 1, and interrupts. */
/* Enables the ADCs and starts the conversions, but not start TIM4 */
/* Parameters : None */
/* Return : None */
/******************************************************************************/
void ADC12_Dual_Init(void)
{
    SET_BIT(RCC->AHBENR, RCC_AHBENR_ADC12EN);
    SET_BIT(RCC->AHBENR, RCC_AHBENR_DMA1EN);

    /* Multi-mode
     * Regular simultaneous mode only
     * DMA Circular mode
     * Enable DMA for 12 and 10 bit resolution
     * delay 5 cycles
     */
    SET_BIT(ADC12_COMMON->CCR, ADC12_CCR_CKMODE_0);
    SET_BIT(RCC->CFGR2, RCC_CFGR2_ADCPRE12_DIV1);
    SET_BIT(RCC->AHBENR, RCC_AHBENR_ADC12EN);
    SET_BIT(ADC12_COMMON->CCR, ADC12_CCR_MULTI_1 | ADC12_CCR_MULTI_2);
    SET_BIT(ADC12_COMMON->CCR, (1 << ADC12_CCR_DMACFG_Pos));
    SET_BIT(ADC12_COMMON->CCR, ADC12_CCR_MDMA_1);
    CLEAR_BIT(ADC12_COMMON->CCR, (1 << ADC12_CCR_DELAY_Pos));

    /* Setting ADC1
     * Resolution 12 bit
     * 3 channel for first conversation (PA2)
     * Left alignment
     * External event Timer 4 TRGO
     * External TRG enable, Rising edge
     * Channel 3, 15 cycles for conversation
     * Enable DMA
     */
    CLEAR_BIT(ADC1->CFGR, ADC_CFGR_RES_0);
    CLEAR_BIT(ADC1->CFGR, ADC_CFGR_RES_1);
    SET_BIT(ADC1->SQR1, (3 << ADC_SQR1_SQ1_Pos));
    SET_BIT(ADC1->CFGR, ADC_CFGR_ALIGN);
    MODIFY_REG(ADC1->CFGR, ADC_CFGR_EXTSEL, 12 << ADC_CFGR_EXTSEL_Pos);
    MODIFY_REG(ADC1->CFGR, ADC_CFGR_EXTEN, 1 << ADC_CFGR_EXTEN_Pos);
    MODIFY_REG(ADC1->SMPR1, ADC_SMPR1_SMP3, 1 << ADC_SMPR1_SMP3_Pos);
    SET_BIT(ADC1->CFGR, ADC_CFGR_DMAEN);

    /* Setting ADC2
     * Resolution 12 bit
     * 3 channel for first conversation (PA6)
     * Left alignment
     * Channel 3, 15 cycles for conversation
     */
    CLEAR_BIT(ADC2->CFGR, ADC_CFGR_RES_0);
    CLEAR_BIT(ADC2->CFGR, ADC_CFGR_RES_1);
    SET_BIT(ADC2->SQR1, (3 << ADC_SQR1_SQ1_Pos));
    SET_BIT(ADC2->CFGR, ADC_CFGR_ALIGN);
    MODIFY_REG(ADC2->SMPR2, ADC_SMPR1_SMP3, 1 << ADC_SMPR1_SMP3_Pos);

    // Calibration ADC1
    SET_BIT(ADC1->CR, ADC_CR_ADCAL);
    while ((ADC1->CR & ADC_CR_ADCAL) != 0) {
        ;
    }

    // Calibration ADC2
    SET_BIT(ADC2->CR, ADC_CR_ADCAL);
    while ((ADC2->CR & ADC_CR_ADCAL) != 0) {
        ;
    }

    SET_BIT(ADC1->CR, ADC_CR_ADEN);
    SET_BIT(ADC1->CR, ADC_CR_ADSTART);
    SET_BIT(ADC2->CR, ADC_CR_ADEN);

    /* Init DMA
     * Interrupt enable, complete transfer
     * Interrupt enable, half transfer
     * peripheral to memory
     * circular mode enable
     * Set low priority level
     * incrementing memory address
     * disabled incrementing peripheral address
     * peripheral data size 32b (word)
     * memory data size 32b (word)
     */
    MODIFY_REG(DMA1_Channel1->CPAR, DMA_CPAR_PA, (uint32_t)(&ADC12_COMMON->CDR));
    MODIFY_REG(DMA1_Channel1->CMAR, DMA_CMAR_MA, (uint32_t)(pac_adc.data));
    SET_BIT(DMA1_Channel1->CCR, DMA_CCR_TCIE);
    CLEAR_BIT(DMA1_Channel1->CCR, DMA_CCR_DIR);
    SET_BIT(DMA1_Channel1->CCR, DMA_CCR_CIRC);
    CLEAR_BIT(DMA1_Channel1->CCR, (1 << DMA_CCR_PL_Pos));
    SET_BIT(DMA1_Channel1->CCR, DMA_CCR_MINC);
    CLEAR_BIT(DMA1_Channel1->CCR, DMA_CCR_PINC);
    SET_BIT(DMA1_Channel1->CCR, (2 << DMA_CCR_PSIZE_Pos));
    SET_BIT(DMA1_Channel1->CCR, (2 << DMA_CCR_MSIZE_Pos));
    SET_BIT(DMA1_Channel1->CNDTR, (ADC_BUF_LEN_MAX << DMA_CNDTR_NDT_Pos));
    SET_BIT(DMA1_Channel1->CCR, DMA_CCR_EN);
    NVIC_SetPriority(DMA1_Channel1_IRQn, 1);
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

/******************************************************************************/
/* Function Name : DAC1_Init */
/* Description : Initializes DAC1 and configures its settings, triggers, */
/* and DMA. Enables DAC1 and DMA2 Channel 3, but not start TIM2. */
/* Parameters : None */
/* Return : None */
/******************************************************************************/
void DAC1_Init(void)
{
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_DAC1EN);
    SET_BIT(RCC->AHBENR, RCC_AHBENR_DMA2EN);

    /* Init DMA
     * DAC channel1 enable
     * Unmask bits[11:0] of LFSR/ triangle amplitude equal to 4095
     * Timer 2 TRGO event
     * DAC channel1 trigger enable
     * DAC channel1 software trigger disabled
     */
    SET_BIT(DAC->CR, DAC_CR_EN1);
    SET_BIT(DAC->CR, DAC_CR_DMAEN1);
    MODIFY_REG(DAC->CR, DAC_CR_MAMP1, 11 << DAC_CR_MAMP1_Pos);
    MODIFY_REG(DAC->CR, DAC_CR_TSEL1, 4 << DAC_CR_TSEL1_Pos);
    SET_BIT(DAC->CR, DAC_CR_TEN1);
    CLEAR_BIT(DAC->SWTRIGR, DAC_SWTRIGR_SWTRIG1);

    /* Init DMA
     * Interrupt enable, complete transfer
     * memory to peripheral
     * circular mode enable
     * Set low priority level
     * incrementing memory address
     * disabled incrementing peripheral address
     * peripheral data size 16b (half-word)
     * memory data size 16b (half-word)
     */
    MODIFY_REG(DMA2_Channel3->CPAR, DMA_CPAR_PA, (uint32_t)(&DAC->DHR12R1));
    MODIFY_REG(DMA2_Channel3->CMAR, DMA_CMAR_MA, (uint32_t)(gen_buf));
    SET_BIT(DMA2_Channel3->CCR, DMA_CCR_TCIE);
    SET_BIT(DMA2_Channel3->CCR, DMA_CCR_DIR);
    SET_BIT(DMA2_Channel3->CCR, DMA_CCR_CIRC);
    DMA2_Channel3->CCR &= ~(1 << DMA_CCR_PL_Pos);
    SET_BIT(DMA2_Channel3->CCR, DMA_CCR_MINC);
    CLEAR_BIT(DMA2_Channel3->CCR, DMA_CCR_PINC);
    DMA2_Channel3->CCR |= (1 << DMA_CCR_PSIZE_Pos);
    DMA2_Channel3->CCR |= (1 << DMA_CCR_MSIZE_Pos);
    DMA2_Channel3->CNDTR |= (GEN_BUF_SIZE << DMA_CNDTR_NDT_Pos);
    SET_BIT(DMA2_Channel3->CCR, DMA_CCR_EN);

    NVIC_SetPriority(DMA2_Channel3_IRQn, 0);
    NVIC_EnableIRQ(DMA2_Channel3_IRQn);
}

/******************************************************************************/
/* Function Name : TIM4_Init */
/* Description : Initializes TIM4 and configures its settings for ADC. */
/* Parameters : None */
/* Return : None */
/******************************************************************************/
void TIM4_Init(void)
{
    // clock to TIM4 72MHz
    // Update Event for ADC1
    // slave mode: Gated
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM4EN);

    CLEAR_REG(TIM4->CR1);
    MODIFY_REG(TIM4->CR2, TIM_CR2_MMS, 2 << TIM_CR2_MMS_Pos);
    WRITE_REG(TIM4->PSC, 0);
}

/******************************************************************************/
/* Function Name : TIM2_Init */
/* Description : Initializes TIM2 and configures its settings for DAC. */
/* Parameters : None */
/* Return : None */
/******************************************************************************/
void TIM2_Init(void)
{
    // clock to TIM2 72MHz
    // Update Event for DAC1
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);

    CLEAR_REG(TIM2->CR1);
    MODIFY_REG(TIM2->CR2, TIM_CR2_MMS, 2 << TIM_CR2_MMS_Pos);
    WRITE_REG(TIM2->PSC, 0);
}
