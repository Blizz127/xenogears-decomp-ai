#include "common.h"
#ifdef XENO_PC_PORT
#include "psyq/libgte.h"
#endif


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main68", func_800A3490);
INCLUDE_ASM("asm/battle/nonmatchings/main68", func_800A3514);
INCLUDE_ASM("asm/battle/nonmatchings/main68", func_800A3578);
INCLUDE_ASM("asm/battle/nonmatchings/main68", func_800A35C8);
INCLUDE_ASM("asm/battle/nonmatchings/main68", func_800A3640);
INCLUDE_ASM("asm/battle/nonmatchings/main68", func_800A3E98);
INCLUDE_ASM("asm/battle/nonmatchings/main68", func_800A429C);
INCLUDE_ASM("asm/battle/nonmatchings/main68", func_800A4348);
INCLUDE_ASM("asm/battle/nonmatchings/main68", func_800A43F8);
INCLUDE_ASM("asm/battle/nonmatchings/main68", func_800A44C0);
INCLUDE_ASM("asm/battle/nonmatchings/main68", func_800A4654);
INCLUDE_ASM("asm/battle/nonmatchings/main68", func_800A4820);
INCLUDE_ASM("asm/battle/nonmatchings/main68", func_800A48EC);
INCLUDE_ASM("asm/battle/nonmatchings/main68", func_800A4B3C);
INCLUDE_ASM("asm/battle/nonmatchings/main68", func_800A4CF8);
INCLUDE_ASM("asm/battle/nonmatchings/main68", func_800A4DB8);
#endif


#ifdef XENO_PC_PORT
/* Retail effect progression callbacks selected by func_800AA820.
 * Arithmetic is signed 16-bit on entry and wraps to signed 16-bit on
 * return. Zero divisors are invalid in retail (BREAK 7), never a value. */
static s32 effect_divide(s32 numerator, s16 divisor) {
    if (divisor == 0) __builtin_trap();
    return numerator / divisor;
}

s32 func_800A3490(s16 phase, s16 divisor, s16 base) {
    return (s16)(base + effect_divide(rsin(phase) + 0x1000, divisor));
}

s32 func_800A3514(s16 phase, s16 divisor, s16 base) {
    s16 value = (s16)(base + effect_divide(phase, divisor));
    return value < 0x21 ? value : -1;
}

s32 func_800A3578(s16 phase, s16 divisor, s16 base) {
    return (s16)(base - effect_divide(phase, divisor));
}

s32 func_800A35C8(s16 phase, s16 divisor, s16 base) {
    s16 value = (s16)(0x20 - effect_divide(phase, divisor));
    return value < base ? base : value;
}
#endif


#ifndef XENO_PC_PORT
extern u32 D_800D3344;
#endif
#ifndef XENO_PC_PORT
extern u32 D_800D39CC;
#endif
#ifndef XENO_PC_PORT
extern u8 D_800C3B74;
#endif
#ifndef XENO_PC_PORT
extern u8 D_800C3D6C;
#endif
#ifndef XENO_PC_PORT
extern u8* D_800C3610;
#endif


/* func_800A577C.s */
u32 func_800A577C(void) {
    return D_800D3344;
}
/* func_800A578C.s */
u32 func_800A578C(void) {
    return D_800D39CC;
}
