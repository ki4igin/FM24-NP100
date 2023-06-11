#include "stm32f3xx.h"

/**
 * @brief OPAMP1 Initialization Function
 * @param None
 * @retval None
 */
void MX_OPAMP1_Init(void)
{
    static OPAMP_HandleTypeDef hopamp;

    hopamp.Instance = OPAMP1;
    hopamp.Init.Mode = OPAMP_STANDALONE_MODE;
    hopamp.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO0;
    hopamp.Init.InvertingInput = OPAMP_INVERTINGINPUT_IO1;
    hopamp.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
    hopamp.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
    HAL_OPAMP_Init(&hopamp);
}

/**
 * @brief OPAMP2 Initialization Function
 * @param None
 * @retval None
 */
void MX_OPAMP2_Init(void)
{
    static OPAMP_HandleTypeDef hopamp;

    hopamp.Instance = OPAMP2;
    hopamp.Init.Mode = OPAMP_STANDALONE_MODE;
    hopamp.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO0;
    hopamp.Init.InvertingInput = OPAMP_INVERTINGINPUT_IO1;
    hopamp.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
    hopamp.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
    HAL_OPAMP_Init(&hopamp);
}

/**
 * @brief OPAMP3 Initialization Function
 * @param None
 * @retval None
 */
void MX_OPAMP3_Init(void)
{
    static OPAMP_HandleTypeDef hopamp;

    hopamp.Instance = OPAMP3;
    hopamp.Init.Mode = OPAMP_FOLLOWER_MODE;
    hopamp.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO0;
    hopamp.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
    hopamp.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
    HAL_OPAMP_Init(&hopamp);
}

/**
 * @brief OPAMP4 Initialization Function
 * @param None
 * @retval None
 */
void MX_OPAMP4_Init(void)
{
    static OPAMP_HandleTypeDef hopamp;

    hopamp.Instance = OPAMP4;
    hopamp.Init.Mode = OPAMP_FOLLOWER_MODE;
    hopamp.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO3;
    hopamp.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
    hopamp.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
    HAL_OPAMP_Init(&hopamp);
}
