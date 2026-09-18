#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main58", func_8009B104);
INCLUDE_ASM("asm/battle/nonmatchings/main58", func_8009B1E4);
INCLUDE_ASM("asm/battle/nonmatchings/main58", func_8009B46C);
INCLUDE_ASM("asm/battle/nonmatchings/main58", func_8009B684);
INCLUDE_ASM("asm/battle/nonmatchings/main58", func_8009BAC4);
INCLUDE_ASM("asm/battle/nonmatchings/main58", func_8009BD94);
INCLUDE_ASM("asm/battle/nonmatchings/main58", func_8009BE0C);
#endif
#ifndef XENO_PC_PORT
extern u8 D_800CCCE8[];
#endif
#ifdef XENO_PC_PORT
/* Coexistence: decompiled and logic-faithful, but not byte-exact yet, so the
 * matching build assembles retail bytes below and the port uses this body. */
/* func_8009C050.s: status flags for actor idx (368-byte stride over
 * D_800CCCE8): bit0 = +0x120 & 0x400, bit1 = +0x104 < (+0x108 >> 3) with
 * +0x36 bit0 clear, bit2 = +0x148 == 4. */
u8 func_8009C050(u8 idx) {
    u32 off = idx * 368;
    u8* base = D_800CCCE8;
    u8* a = off + (base + 0xA4);
    u8* b = off + base;
    u8* c = off + (base + 0x148);
    u32 t = (*(u16*)(a + 0x7C) & 0x400) != 0;
    u32 r = t;

    if (*(u32*)(a + 0x60) < (*(u32*)(a + 0x64) >> 3) && !(*(u16*)(b + 0x36) & 1)) {
        r = t | 2;
    }
    if (c[0] == 4) {
        r |= 4;
    }
    return r;
}
#else
INCLUDE_ASM("asm/battle/nonmatchings/main58", func_8009C050);
#endif
#ifndef XENO_PC_PORT
extern u8 D_800CCE30[];
#endif
#ifndef XENO_PC_PORT
extern u8 D_800CCE4A[];
#endif
extern u8 g_GameState[];


/* func_8009C0E0.s */
void func_8009C0E0(u8 index) {
    u32 off = (u32)index * 368;
    u16* p = (u16*)(g_GameState + 0x16DA);

    D_800CCE30[off] = 4;
    D_800CCE4A[off] = 3;
    *p |= 0x4000;
}
