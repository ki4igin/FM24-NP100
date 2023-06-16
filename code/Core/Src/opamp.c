#include "opamp.h"
#include "stm32f3xx_ll_opamp.h"
#include "stm32f3xx_ll_gpio.h"
#include "stm32f3xx_ll_bus.h"

void OPAMP1_Init(void)
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    /**OPAMP1 GPIO Configuration
    PA1   ------> OPAMP1_VINP
    PA2   ------> OPAMP1_VOUT
    PA3   ------> OPAMP1_VINM
    */
    LL_GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin = LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3,
        .Mode = LL_GPIO_MODE_ANALOG,
        .Pull = LL_GPIO_PULL_NO,
    };
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    LL_OPAMP_InitTypeDef OPAMP_InitStruct = {
        .FunctionalMode = LL_OPAMP_MODE_STANDALONE,
        .InputNonInverting = LL_OPAMP_INPUT_NONINVERT_IO0,
        .InputInverting = LL_OPAMP_INPUT_INVERT_IO1,
    };
    LL_OPAMP_Init(OPAMP1, &OPAMP_InitStruct);

    LL_OPAMP_SetInputsMuxMode(OPAMP1, LL_OPAMP_INPUT_MUX_DISABLE);
    LL_OPAMP_SetTrimmingMode(OPAMP1, LL_OPAMP_TRIMMING_FACTORY);
    LL_OPAMP_Enable(OPAMP1);
}

void OPAMP2_Init(void)
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    /**OPAMP2 GPIO Configuration
    PA5   ------> OPAMP2_VINM
    PA6   ------> OPAMP2_VOUT
    PA7   ------> OPAMP2_VINP
    */
    LL_GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin = LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7,
        .Mode = LL_GPIO_MODE_ANALOG,
        .Pull = LL_GPIO_PULL_NO,
    };
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    LL_OPAMP_InitTypeDef OPAMP_InitStruct = {
        .FunctionalMode = LL_OPAMP_MODE_STANDALONE,
        .InputNonInverting = LL_OPAMP_INPUT_NONINVERT_IO0,
        .InputInverting = LL_OPAMP_INPUT_INVERT_IO1,
    };
    LL_OPAMP_Init(OPAMP2, &OPAMP_InitStruct);

    LL_OPAMP_SetInputsMuxMode(OPAMP2, LL_OPAMP_INPUT_MUX_DISABLE);
    LL_OPAMP_SetTrimmingMode(OPAMP2, LL_OPAMP_TRIMMING_FACTORY);
    LL_OPAMP_Enable(OPAMP2);
}

void OPAMP3_Init(void)
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    /**OPAMP3 GPIO Configuration
    PB0   ------> OPAMP3_VINP
    PB1   ------> OPAMP3_VOUT
    */
    LL_GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1,
        .Mode = LL_GPIO_MODE_ANALOG,
        .Pull = LL_GPIO_PULL_NO,
    };
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    LL_OPAMP_InitTypeDef OPAMP_InitStruct = {
        .FunctionalMode = LL_OPAMP_MODE_FOLLOWER,
        .InputNonInverting = LL_OPAMP_INPUT_NONINVERT_IO0,
    };
    LL_OPAMP_Init(OPAMP3, &OPAMP_InitStruct);

    LL_OPAMP_SetInputsMuxMode(OPAMP3, LL_OPAMP_INPUT_MUX_DISABLE);
    LL_OPAMP_SetTrimmingMode(OPAMP3, LL_OPAMP_TRIMMING_FACTORY);
    LL_OPAMP_Enable(OPAMP3);
}

void OPAMP4_Init(void)
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    /**OPAMP4 GPIO Configuration
    PB11   ------> OPAMP4_VINP
    PB12   ------> OPAMP4_VOUT
    */
    LL_GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin = LL_GPIO_PIN_11 | LL_GPIO_PIN_12,
        .Mode = LL_GPIO_MODE_ANALOG,
        .Pull = LL_GPIO_PULL_NO,
    };

    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    LL_OPAMP_InitTypeDef OPAMP_InitStruct = {
        .FunctionalMode = LL_OPAMP_MODE_FOLLOWER,
        .InputNonInverting = LL_OPAMP_INPUT_NONINVERT_IO3,
    };
    LL_OPAMP_Init(OPAMP4, &OPAMP_InitStruct);
    LL_OPAMP_SetInputsMuxMode(OPAMP4, LL_OPAMP_INPUT_MUX_DISABLE);
    LL_OPAMP_SetTrimmingMode(OPAMP4, LL_OPAMP_TRIMMING_FACTORY);
    LL_OPAMP_Enable(OPAMP4);
}
