#include "ramp.h"
#include "main.h"

uint16_t ramp_buf[RAMP_BUF_SIZE];

static uint32_t ramp_amp_code = 200;
static enum ramp_type ramp_type = RAMP_TYPE_SYM;

static void make_ramp_nonsym(uint32_t amp_code)
{
    // Используются 8 знаков после запятой (двоичной),
    // максимальное возможное значение 1048386 (укладывается в 32 бита)
    typedef uint32_t q32_8;

    q32_8 amp = amp_code << 8;
    q32_8 half_dac_range = 2048 << 8;
    q32_8 start_val = half_dac_range - amp / 2;

    q32_8 k_ramp = amp / (RAMP_BUF_SIZE - 1);

    for (uint32_t i = 0; i < RAMP_BUF_SIZE; i++) {
        q32_8 val = start_val + i * k_ramp;
        ramp_buf[i] = val >> 8;
    }
}

static void make_ramp_sym(uint32_t amp_code)
{
    // Используются 8 знаков после запятой (двоичной),
    // максимальное возможное значение 1048448 (укладывается в 32 бита)
    typedef uint32_t q32_8;

    q32_8 amp = amp_code << 8;
    q32_8 half_dac_range = 2048 << 8;
    q32_8 start_val = half_dac_range - amp / 2;
    // В центре будет два одинаковых значения, поэтому при вычислении
    // коэффициента наклона из размера буфера вычитаются эти два отчета
    q32_8 k_ramp = 2 * amp / (RAMP_BUF_SIZE - 2);

    for (uint32_t i = 0; i < (RAMP_BUF_SIZE / 2); i++) {
        q32_8 val = start_val + i * k_ramp;
        ramp_buf[i] = val >> 8;
        ramp_buf[RAMP_BUF_SIZE - 1 - i] = ramp_buf[i];
    }
}

/**
@brief Generates a ramp waveform based on the specified parameters.
@param ramp: Type of ramp waveform (COMMAND_RAMP1 or COMMAND_RAMP2)
@param ampl: Amplitude of the ramp waveform
@retval None
@note This function generates a ramp waveform based on the specified parameters.
*/
void ramp_make(uint32_t amp_code, enum ramp_type type)
{
    ramp_amp_code = amp_code < 4095 ? amp_code : 4095;
    ramp_type = type;

    switch (ramp_type) {
    case RAMP_TYPE_NONSYM:
        make_ramp_nonsym(ramp_amp_code);
        break;
    case RAMP_TYPE_SYM:
        make_ramp_sym(ramp_amp_code);
        break;
    default:
        break;
    }
}

void ramp_change_type(enum ramp_type type)
{
    ramp_make(ramp_amp_code, type);
}

void ramp_change_amp(uint32_t amp_code)
{
    ramp_make(amp_code, ramp_type);
}
