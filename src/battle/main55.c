#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main55", func_8009A7E4);
INCLUDE_ASM("asm/battle/nonmatchings/main55", func_8009A854);
INCLUDE_ASM("asm/battle/nonmatchings/main55", func_8009A9D0);
#endif
extern u8* D_800C34B0;
extern u8 D_800D2C34;
/* func_8009AA44.s: clear the D_800C34B0 mode byte, set bit0 of the actor's
 * +0x15A flag and, when that flag already had bit7 with +0x126 bit4, drop the
 * 0x1B0 bits of +0x120 and bit12 of +0x7C.  Finishes by stamping 0x3D into
 * +0x5FC7 while D_800D2C34 is 4.  D_800C34B0 is re-read for every access. */
void func_8009AA44(u8 idx) {
    u32 off = (u32)idx * 368;
    D_800C34B0[0x5FC2] = 0;
    (D_800C34B0 + off)[0x15A] |= 1;
    {
        u8* p = D_800C34B0 + off;

        if (p[0x15A] & 0x80) {
            if (*(u16*)(p + 0x126) & 0x10) {
                *(u16*)(p + 0x120) &= 0xFE4F;
                *(u16*)(p + 0x7C) &= 0xEFFF;
            }
        }
    }

    if (D_800D2C34 == 4) {
        D_800C34B0[0x5FC7] = 0x3D;
    }
}extern u8* D_800C34B0;


/* func_8009AB00.s */
void func_8009AB00(u8 index) {
    u8* p = D_800C34B0 + index * 0x170;

    p[0x15A] &= 0xFE;
}
