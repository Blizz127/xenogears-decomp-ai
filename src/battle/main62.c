#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main62", func_8009E278);
INCLUDE_ASM("asm/battle/nonmatchings/main62", func_8009E2EC);
#endif
#ifndef XENO_PC_PORT
extern u8* D_800C3E00;
#endif
#ifndef XENO_PC_PORT
extern u8* D_800C3DFC;
#endif
#ifndef XENO_PC_PORT
extern u8 D_800C3E50;
#endif
#ifndef XENO_PC_PORT
extern u8* D_800C34B0;
#endif
#ifdef XENO_PC_PORT
/* Coexistence: decompiled and logic-faithful, but not byte-exact yet, so the
 * matching build assembles retail bytes below and the port uses this body. */
/* func_8009E364.s: slot D_800C3E50 of the D_800C34B0 block: byte +0x5FA0 = 2,
 * word +0x5F6C = D_800C3E00[0x5B] * D_800C3DFC[0x11]. */
void func_8009E364(void) {
    u32 prod;

    prod = D_800C3E00[0x5B] * D_800C3DFC[0x11];
    (D_800C34B0 + D_800C3E50)[0x5FA0] = 2;
    ((u32*)(D_800C34B0 + 0x5F6C))[D_800C3E50] = prod;
}
#else
INCLUDE_ASM("asm/battle/nonmatchings/main62", func_8009E364);
#endif
#ifndef XENO_PC_PORT
extern u32 D_800C3D60;
#endif
#ifndef XENO_PC_PORT
extern u8 D_800C3E50;
#endif
#ifndef XENO_PC_PORT
extern u8 D_800CCE4A[];
#endif


/* func_8009E3C8.s */
void func_8009E3C8(void) {
    *(u8*)D_800C3D60 = 4;
    *(u8*)(D_800CCE4A + (u32)D_800C3E50 * 0x170) = 3;
}
