#include "ramp.h"
#include "main.h"

typedef uint32_t q32_8_t;

uint16_t ramp_buf[RAMP_BUF_SIZE];

static uint32_t ramp_amp_code = 200;
static enum ramp_type ramp_type = RAMP_TYPE_SYM;

static void Ramp_Make_NonSym(uint32_t amp_code)
{
    // Используются 8 знаков после запятой (двоичной),
    // максимальное возможное значение 1048386 (укладывается в 32 бита)
    q32_8_t amp = amp_code << 8;
    q32_8_t half_dac_range = 2048 << 8;
    q32_8_t start_val = half_dac_range - amp / 2;

    q32_8_t k_ramp = amp / (RAMP_BUF_SIZE - 1);

    for (uint32_t i = 0; i < RAMP_BUF_SIZE; i++) {
        q32_8_t val = start_val + i * k_ramp;
        ramp_buf[i] = val >> 8;
    }
}

static void Ramp_Make_Sym(uint32_t amp_code)
{
    // Используются 8 знаков после запятой (двоичной),
    // максимальное возможное значение 1048448 (укладывается в 32 бита)
    q32_8_t amp = amp_code << 8;
    q32_8_t half_dac_range = 2048 << 8;
    q32_8_t start_val = half_dac_range - amp / 2;
    // В центре будет два одинаковых значения, поэтому при вычислении
    // коэффициента наклона из размера буфера вычитаются эти два отчета
    q32_8_t k_ramp = 2 * amp / (RAMP_BUF_SIZE - 2);

    for (uint32_t i = 0; i < (RAMP_BUF_SIZE / 2); i++) {
        q32_8_t val = start_val + i * k_ramp;
        ramp_buf[i] = val >> 8;
        ramp_buf[RAMP_BUF_SIZE - 1 - i] = ramp_buf[i];
    }
}

/**
@brief Generates a ramp waveform based on the specified parameters.
@param ramp: Type of ramp waveform (COMMAND_RAMP1 or COMMAND_RAMP2)
@param amp: Amplitude of the ramp waveform
@retval None
@note This function generates a ramp waveform based on the specified parameters.
*/
void Ramp_Make(uint32_t amp_code, enum ramp_type type)
{
    ramp_amp_code = amp_code < 4095 ? amp_code : 4095;
    ramp_type = type;

    switch (ramp_type) {
    case RAMP_TYPE_NONSYM:
        Ramp_Make_NonSym(ramp_amp_code);
        break;
    case RAMP_TYPE_SYM:
        Ramp_Make_Sym(ramp_amp_code);
        break;
    default:
        break;
    }
}

void Ramp_Change_Type(enum ramp_type type)
{
    Ramp_Make(ramp_amp_code, type);
}

void Ramp_Change_Amp(uint32_t amp_code)
{
    Ramp_Make(amp_code, ramp_type);
}
