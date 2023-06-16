#include "main.h"
#include "rcc.h"
#include "gpio.h"
#include "periph.h"
#include "opamp.h"
#include "gen.h"
#include "tools.h"

#define UART_RX_NBUF 4

enum __attribute__((packed)) cmd_id {
    CMD_START = 1,
    CMD_STOP = 2,
    CMD_RESET = 3,
    CMD_TEST = 4,
    CMD_GEN_TYPE = 5,
    CMD_AMP = 6,
    CMD_DEVIATION = 7,
    CMD_SENSITIVITY = 8,
    CMD_FD = 9,
    CMD_FM = 10,
};

UART_HandleTypeDef huart1;
uint32_t adc_number_samples = ADC_BUF_LEN_MAX;
struct pac_adc pac_adc = {
    .preamble.id = 0x01,
};
volatile struct flags flags = {0};

static uint32_t vco_sensitivity = VCO_SENSITIVITY_INIT;
static uint8_t uart_buf[UART_RX_NBUF];

static struct cmd {
    enum cmd_id id :8;
    uint32_t arg   :24;
} cmd;

static void Cmd_Work(struct cmd);
static void Change_DF(uint32_t deviation_freq_kHz);
static void Change_Amp(uint32_t amp_mV);
static void Change_Sensitivity(uint32_t sensitivity);
static void UART_Send_Test(UART_HandleTypeDef *huart);
static void ADC_Start_Collect(uint32_t number_samples);
static void UART_Send_ADC_Data(void);
static void USART1_UART_Init(void);

int main(void)
{
    HAL_Init();
    RCC_SystemClock_Config();
    GPIO_Init();
    OPAMP1_Init();
    OPAMP2_Init();
    OPAMP3_Init();
    OPAMP4_Init();
    USART1_UART_Init();

    ADC12_Dual_Init();
    DAC1_Init();
    TIM2_Init();
    TIM4_Init();

    Gen_Make(DAC_AMP_CODE_INIT, GEN_TYPE_RAMP_NONSYM);

    ADC12_Dual_Change_Fd(FREQ_500K);
    DAC1_Change_Fm(FREQ_30H517578125);
    DAC1_Start();

    UART_Send_Test(&huart1);
    HAL_UART_Receive_IT(&huart1, uart_buf, UART_RX_NBUF);

    while (1) {
        static uint32_t led_cnt = 0;
        if (led_cnt++ > 0x7FFFFF) {
            led_cnt = 0;
            GPIO_LedToggle();
        }

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
    case CMD_START:
        ADC_Start_Collect(cmd.arg);
        break;
    case CMD_RESET:
        HAL_NVIC_SystemReset();
        break;
    case CMD_TEST:
        UART_Send_Test(&huart1);
        break;
    case CMD_GEN_TYPE:
        Gen_Change_Type(cmd.arg);
        break;
    case CMD_AMP:
        Change_Amp(cmd.arg);
        break;
    case CMD_FD:
        ADC12_Dual_Change_Fd(cmd.arg);
        break;
    case CMD_FM:
        DAC1_Change_Fm(cmd.arg);
        break;
    case CMD_DEVIATION:
        Change_DF(cmd.arg);
        break;
    case CMD_SENSITIVITY:
        Change_Sensitivity(cmd.arg);
        break;

    default:
        break;
    }
}

static void Change_Sensitivity(uint32_t sensitivity)
{
    vco_sensitivity = sensitivity;
}

static void Change_DF(uint32_t deviation_freq_kHz)
{
    if (deviation_freq_kHz > DEVIATION_FREQ_MAX_kHz) {
        return;
    }

    uint32_t amp_code = df2code(deviation_freq_kHz, vco_sensitivity);
    Gen_Change_AmpCode(amp_code);
}

static void Change_Amp(uint32_t amp_mV)
{
    uint32_t amp_code = volt2code(amp_mV, ADC_REF_mV);
    Gen_Change_AmpCode(amp_code);
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
        .id = CMD_TEST,
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
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void USART1_UART_Init(void)
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
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    __disable_irq();
    while (1) {
    }
}
