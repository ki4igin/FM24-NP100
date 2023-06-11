#include "ramp.h"
#include "opamp.h"
#include "periph.h"

UART_HandleTypeDef huart1;

volatile uint32_t BUFF_ADC1_2[SIZE_BUFFER_ADC] = {0};

volatile uint32_t count_dma_period = 0;
volatile uint32_t count_periods = 0;
volatile uint32_t number_periods = 0;

uint8_t uart_buf[UART_RX_NBUF];

static struct cmd {
    enum command id :8;
    uint32_t arg    :24;
} cmd;

struct message_ADC message_ADC12 = {
    .preamble.id = 0x01,
};

volatile struct flags flags = {0};

static void cmd_work(struct cmd);
static void uart_send_test_cmd(UART_HandleTypeDef *huart);
static void start(uint32_t arg);
static void Collect_ADC_Complete(void);
static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_OPAMP4_Init();
    MX_OPAMP1_Init();
    MX_OPAMP2_Init();
    MX_OPAMP3_Init();
    MX_USART1_UART_Init();

    opamp_enable(OPAMP1);
    opamp_enable(OPAMP2);
    opamp_enable(OPAMP3);
    opamp_enable(OPAMP4);

    ADC1_2_Dual_Init();
    DAC1_Init();
    TIM2_Init();
    TIM4_Init();
    ramp_make(DAC_AMP_CODE_INIT, RAMP_TYPE_NONSYM);
    dac_start();

    uart_send_test_cmd(&huart1);
    HAL_UART_Receive_IT(&huart1, uart_buf, UART_RX_NBUF);

    while (1) {
        if (flags.is_new_cmd) {
            flags.is_new_cmd = 0;
            cmd_work(cmd);
        }
        if (flags.data_adc_collect) {
            flags.data_adc_collect = 0;
            Collect_ADC_Complete();
        }
    }
}

static void cmd_work(struct cmd cmd)
{
    switch (cmd.id) {
    case COMMAND_START:
        start(cmd.arg);
        break;
    case COMMAND_RESET:
        HAL_NVIC_SystemReset();
        break;
    case COMMAND_TEST:
        uart_send_test_cmd(&huart1);
        break;
    case COMMAND_RAMP:
        ramp_change_type(cmd.arg);
        break;
    case COMMAND_AMP:
        ramp_change_amp(cmd.arg);
        break;
    default:
        break;
    }
}

static void start(uint32_t arg)
{
    if (READ_BIT(TIM4->CR1, TIM_CR1_CEN) == 0) {
        number_periods = arg;
        flags.start_req = 1;
    }
}

static void uart_send_test_cmd(UART_HandleTypeDef *huart)
{
    struct cmd cmd = {
        .id = COMMAND_TEST,
        .arg = 0x112233};

    HAL_UART_Transmit(huart, (uint8_t *)&cmd, sizeof(cmd), 1000);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1) {
        cmd = *(struct cmd *)uart_buf;
        flags.is_new_cmd = 1;
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1) {
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1) {
        HAL_UART_Receive_IT(&huart1, uart_buf, UART_RX_NBUF);
    }
}

static void Collect_ADC_Complete(void)
{
    message_ADC12.preamble.number_periods = number_periods;
    message_ADC12.preamble.size = SIZE_BUFFER_ADC * count_dma_period * sizeof(uint32_t);
    HAL_UART_Transmit_IT(
        &huart1,
        (uint8_t *)&message_ADC12,
        sizeof(message_ADC12.preamble) + message_ADC12.preamble.size);
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void)
{
    NVIC_SetPriority(USART1_IRQn, 2);

    huart1.Instance = USART1;
    huart1.Init.BaudRate = UART_BAUD_RATE;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }

    // Максимальное время между байтами 200 битовых знаков
    HAL_UART_ReceiverTimeout_Config(&huart1, 200);
    HAL_UART_EnableReceiverTimeout(&huart1);
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);

    /*Configure GPIO pin : PC13 */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pin : PB15 */
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pin : PA4 */
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    __disable_irq();
    while (1) {
    }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
