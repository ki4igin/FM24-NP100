#ifndef __RAMP_H
#define __RAMP_H

#include "stdint.h"
#include "main.h"

#define RAMP_BUF_SIZE 128

enum ramp_type {
    RAMP_TYPE_SYM = 1,
    RAMP_TYPE_NONSYM = 2,
};

extern uint16_t ramp_buf[RAMP_BUF_SIZE];

void ramp_make(uint32_t amp_code, enum ramp_type);
void ramp_change_amp(uint32_t amp_code);
void ramp_change_type(enum ramp_type type);

#endif