
#ifndef __GPIO_H__
#define __GPIO_H__

#include "stm32f3xx.h"
#include "stm32f3xx_ll_gpio.h"

#define GPIO_TP_Pin         LL_GPIO_PIN_8
#define GPIO_EXT_INTPUT_Pin LL_GPIO_PIN_11
#define GPIO_LED_Pin        LL_GPIO_PIN_12
#define GPIO_Port           GPIOA

void GPIO_Init(void);

inline static void GPIO_LedOn(void)
{
    LL_GPIO_ResetOutputPin(GPIO_Port, GPIO_LED_Pin);
}

inline static void GPIO_LedOff(void)
{
    LL_GPIO_SetOutputPin(GPIO_Port, GPIO_LED_Pin);
}

inline static void GPIO_LedToggle(void)
{
    LL_GPIO_TogglePin(GPIO_Port, GPIO_LED_Pin);
}

inline static void GPIO_TestPinSet(void)
{
    LL_GPIO_SetOutputPin(GPIO_Port, GPIO_TP_Pin);
}

inline static void GPIO_TestPinReset(void)
{
    LL_GPIO_ResetOutputPin(GPIO_Port, GPIO_TP_Pin);
}

inline static void GPIO_TestPinToggle(void)
{
    LL_GPIO_TogglePin(GPIO_Port, GPIO_TP_Pin);
}

#endif
