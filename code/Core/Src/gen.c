#include "gen.h"
#include "main.h"

typedef uint32_t u32_8_t;
typedef int32_t i32_20_t;

uint16_t gen_buf[GEN_BUF_SIZE];

static uint32_t gen_amp_code = 200;
static enum gen_type gen_type = GEN_TYPE_RAMP_SYM;

// clang-format off
static const i32_20_t sin_table[GEN_BUF_SIZE] = {
          0,   12867,   25726,   38569,   51389,   64178,   76929,   89633,
     102284,  114872,  127392,  139834,  152193,  164460,  176627,  188689,  
     200636,  212463,  224162,  235726,  247148,  258421,  269538,  280493,
     291279,  301889,  312318,  322559,  332605,  342451,  352090,  361518,
     370728,  379714,  388472,  396996,  405280,  413321,  421112,  428650,
     435930,  442947,  449697,  456176,  462381,  468307,  473951,  479309,
     484379,  489157,  493640,  497826,  501712,  505296,  508576,  511549,
     514214,  516569,  518613,  520345,  521763,  522867,  523656,  524130,
     524288,  524130,  523656,  522867,  521763,  520345,  518613,  516569,
     514214,  511549,  508576,  505296,  501712,  497826,  493640,  489157,
     484379,  479309,  473951,  468307,  462381,  456176,  449697,  442947,
     435930,  428650,  421112,  413321,  405280,  396996,  388472,  379714,  
     370728,  361518,  352090,  342451,  332605,  322559,  312318,  301889,  
     291279,  280493,  269538,  258421,  247148,  235726,  224162,  212463,
     200636,  188689,  176627,  164460,  152193,  139834,  127392,  114872,
     102284,   89633,   76929,   64178,   51389,   38569,   25726,   12867,
          0,  -12867,  -25726,  -38569,  -51389,  -64178,  -76929,  -89633,
    -102284, -114872, -127392, -139834, -152193, -164460, -176627, -188689,
    -200636, -212463, -224162, -235726, -247148, -258421, -269538, -280493,
    -291279, -301889, -312318, -322559, -332605, -342451, -352090, -361518, 
    -370728, -379714, -388472, -396996, -405280, -413321, -421112, -428650, 
    -435930, -442947, -449697, -456176, -462381, -468307, -473951, -479309, 
    -484379, -489157, -493640, -497826, -501712, -505296, -508576, -511549, 
    -514214, -516569, -518613, -520345, -521763, -522867, -523656, -524130, 
    -524288, -524130, -523656, -522867, -521763, -520345, -518613, -516569, 
    -514214, -511549, -508576, -505296, -501712, -497826, -493640, -489157, 
    -484379, -479309, -473951, -468307, -462381, -456176, -449697, -442947, 
    -435930, -428650, -421112, -413321, -405280, -396996, -388472, -379714, 
    -370728, -361518, -352090, -342451, -332605, -322559, -312318, -301889, 
    -291279, -280493, -269538, -258421, -247148, -235726, -224162, -212463, 
    -200636, -188689, -176627, -164460, -152193, -139834, -127392, -114872, 
    -102284,  -89633,  -76929,  -64178,  -51389,  -38569,  -25726,  -12867,
};

// clang-format on

static void Gen_Ramp_NonSym(uint32_t amp_code)
{
    // Используются 8 знаков после запятой (двоичной),
    // максимальное возможное значение 1048386 (укладывается в 32 бита)
    u32_8_t amp = amp_code << 8;
    u32_8_t half_dac_range = 2048 << 8;
    u32_8_t start_val = half_dac_range - amp / 2;

    u32_8_t k_ramp = amp / (GEN_BUF_SIZE - 1);

    for (uint32_t i = 0; i < GEN_BUF_SIZE; i++) {
        u32_8_t val = start_val + i * k_ramp;
        gen_buf[i] = val >> 8;
    }
}

static void Gen_Ramp_Sym(uint32_t amp_code)
{
    // Используются 8 знаков после запятой (двоичной),
    // максимальное возможное значение 1048448 (укладывается в 32 бита)
    u32_8_t amp = amp_code << 8;
    u32_8_t half_dac_range = 2048 << 8;
    u32_8_t start_val = half_dac_range - amp / 2;
    // В центре будет два одинаковых значения, поэтому при вычислении
    // коэффициента наклона из размера буфера вычитаются эти два отчета
    u32_8_t k_ramp = 2 * amp / (GEN_BUF_SIZE - 2);

    for (uint32_t i = 0; i < (GEN_BUF_SIZE / 2); i++) {
        u32_8_t val = start_val + i * k_ramp;
        gen_buf[i] = val >> 8;
        gen_buf[GEN_BUF_SIZE - 1 - i] = gen_buf[i];
    }
}

static void Gen_Sin(uint32_t amp_code)
{
    for (uint32_t i = 0; i < GEN_BUF_SIZE; i++) {
        i32_20_t sin_sample_i32_20 = (sin_table[i] * amp_code);
        int32_t sin_sample = sin_sample_i32_20 >> 20;
        gen_buf[i] = (uint16_t)(sin_sample & 0xFFF);
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
    case GEN_TYPE_SIN:
        Gen_Sin(gen_amp_code);
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
