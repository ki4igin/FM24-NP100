#ifndef __RAMP_H
#define __RAMP_H

#include "stdint.h"
#include "main.h"

#define RAMP_BUF_SIZE 256

enum ramp_type {
    RAMP_TYPE_SYM = 1,
    RAMP_TYPE_NONSYM = 2,
};

extern uint16_t ramp_buf[RAMP_BUF_SIZE];

void Ramp_Make(uint32_t amp_code, enum ramp_type);
void Ramp_Change_Amp(uint32_t amp_code);
void Ramp_Change_Type(enum ramp_type type);

#endif