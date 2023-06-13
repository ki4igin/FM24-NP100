#ifndef __GEN_H
#define __GEN_H

#include "stdint.h"

#define GEN_BUF_SIZE 256

enum gen_type {
    GEN_TYPE_RAMP_SYM = 1,
    GEN_TYPE_RAMP_NONSYM = 2,
    GEN_TYPE_DC = 3,
};

extern uint16_t gen_buf[GEN_BUF_SIZE];

void Gen_Make(uint32_t amp_code, enum gen_type);
void Gen_Change_AmpCode(uint32_t amp_code);
void Gen_Change_Type(enum gen_type type);

#endif