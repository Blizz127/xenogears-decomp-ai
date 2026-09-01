/* Retail PsyQ NormalLightCol: V0 + RGBC -> NCCS -> RGB2. */
#include <string.h>

#include "common.h"
#include "psyq_normal_light_col.h"

extern void MTC2(unsigned int value, int reg);
extern unsigned int MFC2(int reg);
extern int doCOP2(int op);

static u32 nl_load_u32(const void* source)
{
    u32 value;
    memcpy(&value, source, sizeof(value));
    return value;
}

static void nl_store_u32(void* destination, u32 value)
{
    memcpy(destination, &value, sizeof(value));
}

void NormalLightCol(void* normal, void* input_color, void* output_color)
{
#if defined(W34N120_MUTANT_WRONG_VXY_REGISTER)
    MTC2(nl_load_u32(normal), 2);
#else
    MTC2(nl_load_u32(normal), 0);
#endif

#if !defined(W34N120_MUTANT_DROP_VZ_LOAD)
    MTC2(nl_load_u32((const u8*)normal + 4), 1);
#endif

#if defined(W34N120_MUTANT_WRONG_RGBC_REGISTER)
    MTC2(nl_load_u32(input_color), 7);
#else
    MTC2(nl_load_u32(input_color), 6);
#endif

#if defined(W34N120_MUTANT_WRONG_OPCODE)
    (void)doCOP2(0x0108041Au);
#else
    (void)doCOP2(0x0108041Bu);
#endif

#if defined(W34N120_MUTANT_WRONG_OUTPUT_REGISTER)
    nl_store_u32(output_color, MFC2(21));
#elif defined(W34N120_MUTANT_CORRUPT_OUTPUT)
    nl_store_u32(output_color, MFC2(22) ^ 1u);
#else
    nl_store_u32(output_color, MFC2(22));
#endif
}
