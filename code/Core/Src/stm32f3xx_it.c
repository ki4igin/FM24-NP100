#include "main.h"
#include "stm32f3xx_it.h"

extern UART_HandleTypeDef huart1;

extern volatile uint16_t count_dma_period;
extern volatile uint8_t count_periods;
extern volatile uint32_t BUFF_ADC1_2[SIZE_BUFFER_ADC];
extern volatile uint8_t firstByteWait;
extern uint8_t uart_buf[UART_RX_NBUF];
extern volatile struct flags flags;

struct message_ADC message_ADC12;

static uint32_t temp_size = 0;

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void)
{
    while (1) {
    }
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void)
{
    while (1) {
    }
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void)
{
    while (1) {
    }
}

/**
 * @brief This function handles Pre-fetch fault, memory access fault.
 */
void BusFault_Handler(void)
{
    while (1) {
    }
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void)
{
    while (1) {
    }
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void)
{
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void)
{
}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
    HAL_IncTick();
}

/******************************************************************************/
/* Function Name : USART1_IRQHandler */
/* Description : Interrupt handler for USART1. Handles the RXNE interrupt */
/* flag and performs necessary actions when the interrupt */
/* flag is set. Increases the count_rx variable, checks */
/* firstByteWait condition, clears the TIM3 counter register */
/* if firstByteWait is 1, and calls the HAL_UART_IRQHandler */
/* function. */
/* Parameters : None */
/* Return : None */
/******************************************************************************/
void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);
}

/******************************************************************************/
/* Function Name : DMA1_Channel1_IRQHandler */
/* Description : Interrupt handler for DMA1 channel 1 used for ADC1 and ADC2 */
/* dual mode. Handles the transfer complete and half transfer */
/* complete interrupts. Resets the interrupt flags and copies */
/* the ADC data to a message buffer. Additionally, it toggles */
/* a test pin(PINC 13) and checks if the desired number of ADC periods */
/* for DAC has been reached to enable data collection. */
/* Parameters : None */
/* Return : None */
/******************************************************************************/
// for ADC1_2 (dual)
void DMA1_Channel1_IRQHandler(void)
{
    temp_size = SIZE_BUFFER_ADC * count_dma_period;

    if (READ_BIT(DMA1->ISR, DMA_ISR_HTIF1)) {
        SET_BIT(DMA1->IFCR, DMA_IFCR_CHTIF1);

        for (uint32_t i = 0; i < SIZE_BUFFER_ADC / 2; i++) {
            message_ADC12.BUFF[i + temp_size] = BUFF_ADC1_2[i];
        }

    } else if (READ_BIT(DMA1->ISR, DMA_ISR_TCIF1)) {
        SET_BIT(DMA1->IFCR, DMA_IFCR_CTCIF1);

        for (uint32_t i = SIZE_BUFFER_ADC / 2; i < SIZE_BUFFER_ADC; i++) {
            message_ADC12.BUFF[i + temp_size] = BUFF_ADC1_2[i];
        }

        count_dma_period++;

        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); // TEST period PIN
    }
}

/******************************************************************************/
/* Function Name : DMA2_Channel3_IRQHandler */
/* Description : Interrupt handler for DMA2 channel 3 used for DAC1. */
/* Handles the transfer complete interrupt, toggles a test */
/* pin (PINB 15), resets the interrupt flag, and increments the */
/* count of DAC periods. */
/* Parameters : None */
/* Return : None */
/******************************************************************************/

// for DAC1
void DMA2_Channel3_IRQHandler(void)
{
    if (READ_BIT(DMA2->ISR, DMA_ISR_TCIF3)) {
        // TEST period PIN
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_15);
        SET_BIT(DMA2->IFCR, DMA_IFCR_CGIF3);

        if (READ_BIT(TIM4->CR1, TIM_CR1_CEN)) {
            if (++count_periods >= number_periods) {
                count_periods = 0;
                CLEAR_BIT(TIM4->CR1, TIM_CR1_CEN);
                flags.data_adc_collect = 1;
            }
        }

        if (start_req) {
            count_dma_period = 0;
            start_req = 0;
            SET_BIT(TIM4->CR1, TIM_CR1_CEN);
        }
    }
}
