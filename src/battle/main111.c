#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main111", func_800BA984);
INCLUDE_ASM("asm/battle/nonmatchings/main111", func_800BAB0C);
INCLUDE_ASM("asm/battle/nonmatchings/main111", func_800BABDC);
INCLUDE_ASM("asm/battle/nonmatchings/main111", func_800BAC50);
INCLUDE_ASM("asm/battle/nonmatchings/main111", func_800BACBC);
INCLUDE_ASM("asm/battle/nonmatchings/main111", func_800BADD4);
#endif
#ifndef XENO_PC_PORT
extern u8 D_800C3EB0[];
#endif
extern void func_800223B0(u8* p, s32 v);
extern void func_80021FE0(u8* p, s32 v);
#ifdef XENO_PC_PORT
/* Coexistence: decompiled and logic-faithful, but not byte-exact yet, so the
 * matching build assembles retail bytes below and the port uses this body. */
/* func_800BAEB8.s: for party slot a0 (pointer table at D_800C3EB0 + 0x8C8C),
 * unless its +0xAF state byte is 0x15, apply flag 0x800 (set when the slot's
 * 28-byte record byte +0xA is nonzero) through func_800223B0/func_80021FE0. */
void func_800BAEB8(s32 a0) {
    u8* s1 = ((u8**)(D_800C3EB0 + 0x8C8C))[a0];

    if (*(s8*)(s1 + 0xAF) != 0x15) {
        s32 f = (D_800C3EB0[a0 * 28 + 0xA] != 0) << 11;

        func_800223B0(s1, f);
        func_80021FE0(s1, f);
    }
}
#else
INCLUDE_ASM("asm/battle/nonmatchings/main111", func_800BAEB8);
#endif
#ifndef XENO_PC_PORT
extern u8* D_800D2D28;
#endif
#ifndef XENO_PC_PORT
extern u8* D_800D2DC8;
#endif


/* func_800BAF40.s */
void func_800BAF40(void) {
}
