#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main87", func_800B5DF4);
INCLUDE_ASM("asm/battle/nonmatchings/main87", func_800B5FBC);
INCLUDE_ASM("asm/battle/nonmatchings/main87", func_800B6004);
INCLUDE_ASM("asm/battle/nonmatchings/main87", func_800B61B0);
INCLUDE_ASM("asm/battle/nonmatchings/main87", func_800B61F8);
INCLUDE_ASM("asm/battle/nonmatchings/main87", func_800B626C);
INCLUDE_ASM("asm/battle/nonmatchings/main87", func_800B62C8);
#endif


extern void func_800B3CD4(u32 a0, u32 a1, u32 a2, s32 a3,
                           s32 a4, s32 a5);


/* func_800B639C.s */
void func_800B639C(u32 a0, u8* p) {
    u32 n = (u32)((*(s8*)(p + 1) << 8) | p[0]);

    p += n;
    func_800B3CD4(p[5], p[3], p[4], *(s8*)(p + 0), *(s8*)(p + 1),
                  *(s8*)(p + 2));
}
