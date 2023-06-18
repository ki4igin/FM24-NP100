#ifndef __UART_H
#define __UART_H

#include "stm32f3xx.h"

void UART_Init(void);
void UART_Send_Array(void *buf, uint32_t size);

#endif
