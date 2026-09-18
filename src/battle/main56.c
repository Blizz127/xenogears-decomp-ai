#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main56", func_8009AB38);
INCLUDE_ASM("asm/battle/nonmatchings/main56", func_8009AC48);
INCLUDE_ASM("asm/battle/nonmatchings/main56", func_8009ADA0);
#endif


extern u8* D_800C34B0;
extern u8 D_800CCD68[];
extern void func_8009B104(u32 a0, u8* a1);


/* func_8009AEFC.s */
void func_8009AEFC(u8 index) {
    u32 off;
    u8* p;
    u32 v;
    u32 t;

    D_800C34B0[0x5FC2] = 0;
    off = ((u32)index & 0xFF) * 368;
    p = (u8*)(off + (u32)(unsigned int)D_800C34B0);
    v = *(u16*)(p + 0x80);
    t = p[0x56];
    *(u16*)(p + 0x7C) = 0;
    *(u16*)(p + 0x84) = 0;
    *(u16*)(p + 0x88) = 0;
    *(u16*)(p + 0x8C) = 0;
    *(u16*)(p + 0x80) = (u16)(v & 0x2000);
    if (t == 7) {
        func_8009B104((u32)index & 0xFF, p);
    }
    if (p[0x56] == 3) {
        u32 i;

        for (i = 3; (u32)(i & 0xFF) < 0xB; i++) {
            u32 o2 = (i & 0xFF) * 368;

            *(u16*)((u8*)D_800CCD68 + o2) &= 0xFFDF;
        }
    }
}
