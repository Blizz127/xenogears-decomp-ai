#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main57", func_8009AFD8);
#endif


extern u8* D_800C34B0;


/* func_8009B098.s */
void func_8009B098(void) {
    u8 i;

    for (i = 1; (u8)i < 3; i++) {
        u8* p = (u8*)(((u32)i) * 368 + (u32)(unsigned int)D_800C34B0);

        *(u16*)(p + 0x4C) = 0x64;
        *(u16*)(p + 0x4E) = 0x64;
        p[0x5E] = 0x14;
        p[0x5F] = 0xF;
        *(u16*)(p + 0x7A) = 0x1FBF;
        p[0x149] = 0;
    }
}
