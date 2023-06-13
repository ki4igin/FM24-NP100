#include "gen.h"
#include "main.h"

typedef uint32_t q32_8_t;

uint16_t gen_buf[GEN_BUF_SIZE];

static uint32_t gen_amp_code = 200;
static enum gen_type gen_type = GEN_TYPE_RAMP_SYM;

static void Gen_Ramp_NonSym(uint32_t amp_code)
{
    // Используются 8 знаков после запятой (двоичной),
    // максимальное возможное значение 1048386 (укладывается в 32 бита)
    q32_8_t amp = amp_code << 8;
    q32_8_t half_dac_range = 2048 << 8;
    q32_8_t start_val = half_dac_range - amp / 2;

    q32_8_t k_ramp = amp / (GEN_BUF_SIZE - 1);

    for (uint32_t i = 0; i < GEN_BUF_SIZE; i++) {
        q32_8_t val = start_val + i * k_ramp;
        gen_buf[i] = val >> 8;
    }
}

static void Gen_Ramp_Sym(uint32_t amp_code)
{
    // Используются 8 знаков после запятой (двоичной),
    // максимальное возможное значение 1048448 (укладывается в 32 бита)
    q32_8_t amp = amp_code << 8;
    q32_8_t half_dac_range = 2048 << 8;
    q32_8_t start_val = half_dac_range - amp / 2;
    // В центре будет два одинаковых значения, поэтому при вычислении
    // коэффициента наклона из размера буфера вычитаются эти два отчета
    q32_8_t k_ramp = 2 * amp / (GEN_BUF_SIZE - 2);

    for (uint32_t i = 0; i < (GEN_BUF_SIZE / 2); i++) {
        q32_8_t val = start_val + i * k_ramp;
        gen_buf[i] = val >> 8;
        gen_buf[GEN_BUF_SIZE - 1 - i] = gen_buf[i];
    }
}

static void Gen_DC(uint32_t amp_code)
{
    for (uint32_t i = 0; i < GEN_BUF_SIZE; i++) {
        gen_buf[i] = amp_code;
    }
}

/**
@brief Generates a ramp waveform based on the specified parameters.
@param ramp: Type of ramp waveform (COMMAND_RAMP1 or COMMAND_RAMP2)
@param amp: Amplitude of the ramp waveform
@retval None
@note This function generates a ramp waveform based on the specified parameters.
*/
void Gen_Make(uint32_t amp_code, enum gen_type type)
{
    gen_amp_code = amp_code < 4095 ? amp_code : 4095;
    gen_type = type;

    switch (gen_type) {
    case GEN_TYPE_RAMP_NONSYM:
        Gen_Ramp_NonSym(gen_amp_code);
        break;
    case GEN_TYPE_RAMP_SYM:
        Gen_Ramp_Sym(gen_amp_code);
        break;
    case GEN_TYPE_DC:
        Gen_DC(gen_amp_code);
        break;
    default:
        break;
    }
}

void Gen_Change_Type(enum gen_type type)
{
    Gen_Make(gen_amp_code, type);
}

void Gen_Change_AmpCode(uint32_t amp_code)
{
    Gen_Make(amp_code, gen_type);
}
