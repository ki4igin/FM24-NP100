#ifndef __OPAMP_H
#define __OPAMP_H

#include "stm32f3xx.h"

void MX_OPAMP1_Init(void);
void MX_OPAMP2_Init(void);
void MX_OPAMP3_Init(void);
void MX_OPAMP4_Init(void);

/******************************************************************************/
/* Function Name : opamp_enable */
/* Description : Starts the specified operational amplifier. */
/* Parameters : opamp: Pointer to the operational amplifier instance */
/* Return : None */
/******************************************************************************/
void opamp_enable(OPAMP_TypeDef *opamp)
{
    SET_BIT(opamp->CSR, OPAMP_CSR_OPAMPxEN);
}

#endif