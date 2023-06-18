#include "uart.h"
#include "stm32f3xx_ll_gpio.h"
#include "stm32f3xx_ll_usart.h"
#include "stm32f3xx_ll_bus.h"
#include "fifo.h"
#include "buf.h"

#define UART_BAUD_RATE 115200

fifo_declare(uart_fifo_tx, UART_NBUF_TX);
buf_declare(uart_buf_rx, UART_NBUF_RX);

struct uart_rx uart_rx;

void UART_Init(void)
{
    /* Peripheral clock enable */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

    /**USART1 GPIO Configuration
    PA9   ------> USART1_TX
    PA10   ------> USART1_RX
    */
    LL_GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin = LL_GPIO_PIN_9 | LL_GPIO_PIN_10,
        .Mode = LL_GPIO_MODE_ALTERNATE,
        .Speed = LL_GPIO_SPEED_FREQ_HIGH,
        .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
        .Pull = LL_GPIO_PULL_NO,
        .Alternate = LL_GPIO_AF_7,
    };
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(USART1_IRQn);

    LL_USART_InitTypeDef USART_InitStruct = {
        .BaudRate = UART_BAUD_RATE,
        .DataWidth = LL_USART_DATAWIDTH_8B,
        .StopBits = LL_USART_STOPBITS_1,
        .Parity = LL_USART_PARITY_NONE,
        .TransferDirection = LL_USART_DIRECTION_TX_RX,
        .HardwareFlowControl = LL_USART_HWCONTROL_NONE,
        .OverSampling = LL_USART_OVERSAMPLING_16,
    };
    LL_USART_Init(USART1, &USART_InitStruct);

    LL_USART_DisableIT_CTS(USART1);
    LL_USART_ConfigAsyncMode(USART1);
    LL_USART_SetRxTimeout(USART1, 200);
    LL_USART_EnableRxTimeout(USART1);
    LL_USART_EnableIT_RTO(USART1);
    LL_USART_Enable(USART1);
}

void UART_Send_Array(void *buf, uint32_t size)
{
    uint8_t *pbuf = buf;
    for (uint32_t i = 0; i < size; i++) {
        fifo_push(&uart_fifo_tx, *pbuf++);
    }
    LL_USART_EnableIT_TXE(USART1);
}

void USART1_IRQHandler(void)
{
    if (LL_USART_IsActiveFlag_TXE(USART1) && LL_USART_IsEnabledIT_TXE(USART1)) {
        LL_USART_TransmitData8(USART1, fifo_pop(&uart_fifo_tx));
        if (fifo_is_empty(&uart_fifo_tx)) {
            LL_USART_DisableIT_TXE(USART1);
        }
    }

    if (LL_USART_IsActiveFlag_RXNE(USART1)) {
        uart_buf_rx.data[uart_buf_rx.count] = LL_USART_ReceiveData8(USART1);

        if (uart_buf_rx.count == UART_NBUF_RX) {
            uart_buf_rx.count = 0;
            uart_rx.is_new_data = 1;
            *(uint32_t *)uart_rx.data = *(uint32_t *)uart_buf_rx.data;
        }
    }

    if (LL_USART_IsActiveFlag_RTO(USART1)) {
        LL_USART_ClearFlag_RTO(USART1);
        uart_buf_rx.count = 0;
    }
}
