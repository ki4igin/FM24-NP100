#include "main.h"
#include "stm32f3xx_it.h"
#include "periph.h"

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
void DMA1_Channel1_IRQHandler(void)
{
    if (READ_BIT(DMA1->ISR, DMA_ISR_TCIF1)) {
        SET_BIT(DMA1->IFCR, DMA_IFCR_CTCIF1);
        ADC12_Dual_Stop();
        flags.adc_data_collect = 1;
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
void DMA2_Channel3_IRQHandler(void)
{
    if (READ_BIT(DMA2->ISR, DMA_ISR_TCIF3)) {
        // TEST period PIN
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_15);
        SET_BIT(DMA2->IFCR, DMA_IFCR_CGIF3);

        if (flags.start_req) {
            flags.start_req = 0;
            ADC12_Dual_Start(adc_number_samples);
        }
}
