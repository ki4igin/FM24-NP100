#include "ramp.h"
#include "opamp.h"
#include "periph.h"
#include "tools.h"

UART_HandleTypeDef huart1;

uint32_t adc_number_samples = ADC_BUF_LEN_MAX;

uint8_t uart_buf[UART_RX_NBUF];

enum __attribute__((packed)) command {
    COMMAND_START = 1,
    COMMAND_STOP = 2,
    COMMAND_RESET = 3,
    COMMAND_TEST = 4,
    COMMAND_RAMP = 5,
    COMMAND_AMP = 6,
    COMMAND_DF = 7,
    COMMAND_DF_VS_U = 8,
    COMMAND_FD = 9,
    COMMAND_FM = 10,
};

static struct cmd {
    enum command id :8;
    uint32_t arg    :24;
} cmd;

struct pac_adc pac_adc = {
    .preamble.id = 0x01,
};

volatile struct flags flags = {0};

static void Cmd_Work(struct cmd);
static void Change_DF(uint32_t deviation_freq_kHz);
static void Change_Amp(uint32_t amp_mV);
static void UART_Send_Test(UART_HandleTypeDef *huart);
static void ADC_Start_Collect(uint32_t number_samples);
static void UART_Send_ADC_Data(void);
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

    OPAMP_Enable(OPAMP1);
    OPAMP_Enable(OPAMP2);
    OPAMP_Enable(OPAMP3);
    OPAMP_Enable(OPAMP4);

    ADC12_Dual_Init();
    DAC1_Init();
    TIM2_Init();
    TIM4_Init();

    Ramp_Make(DAC_AMP_CODE_INIT, RAMP_TYPE_NONSYM);

    ADC12_Dual_Change_Fd(FREQ_500K);
    DAC1_Change_Fm(FREQ_30H517578125);
    DAC1_Start();

    UART_Send_Test(&huart1);
    HAL_UART_Receive_IT(&huart1, uart_buf, UART_RX_NBUF);

    while (1) {
        if (flags.is_new_cmd) {
            flags.is_new_cmd = 0;
            Cmd_Work(cmd);
        }
        if (flags.adc_data_collect) {
            flags.adc_data_collect = 0;
            UART_Send_ADC_Data();
        }
    }
}

static void Cmd_Work(struct cmd cmd)
{
    switch (cmd.id) {
    case COMMAND_START:
        ADC_Start_Collect(cmd.arg);
        break;
    case COMMAND_RESET:
        HAL_NVIC_SystemReset();
        break;
    case COMMAND_TEST:
        UART_Send_Test(&huart1);
        break;
    case COMMAND_RAMP:
        Ramp_Change_Type(cmd.arg);
        break;
    case COMMAND_AMP:
        Change_Amp(cmd.arg);
        break;
    case COMMAND_FD:
        ADC12_Dual_Change_Fd(cmd.arg);
        break;
    case COMMAND_FM:
        DAC1_Change_Fm(cmd.arg);
        break;
    case COMMAND_DF:
        Change_DF(cmd.arg);
        break;
    case COMMAND_DF_VS_U:
        break;

    default:
        break;
    }
}

static void Change_DF(uint32_t deviation_freq_kHz)
{
    if (deviation_freq_kHz > DEVIATION_FREQ_MAX_kHz) {
        return;
    }

    uint32_t dac_code = df2code(deviation_freq_kHz, SENSITIVITY_VCO_kHz);
    Ramp_Change_Amp(dac_code);
}

static void Change_Amp(uint32_t amp_mV)
{
    uint32_t dac_code = volt2code(amp_mV, ADC_REF_mV);
    Ramp_Change_Amp(dac_code);
}

static void ADC_Start_Collect(uint32_t number_samples)
{
    if (READ_BIT(TIM4->CR1, TIM_CR1_CEN) == 0) {
        if ((number_samples == 0) || (number_samples > ADC_BUF_LEN_MAX)) {
            return;
        }

        adc_number_samples = number_samples;
        pac_adc.preamble.size = adc_number_samples * sizeof(uint32_t);
        flags.start_req = 1;
    }
}

static void UART_Send_Test(UART_HandleTypeDef *huart)
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

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1) {
        HAL_UART_Receive_IT(&huart1, uart_buf, UART_RX_NBUF);
    }
}

static void UART_Send_ADC_Data(void)
{
    HAL_UART_Transmit_IT(
        &huart1,
        (uint8_t *)&pac_adc,
        sizeof(pac_adc.preamble) + pac_adc.preamble.size);
}

/**
 * @brief System Clock Configuration
 * System Core Clock = 72 MHz
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

    // Максимальное время между байтами в пакете 200 битовых знаков
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
