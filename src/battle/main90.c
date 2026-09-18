#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main90", func_800B6518);
INCLUDE_ASM("asm/battle/nonmatchings/main90", func_800B65B0);
INCLUDE_ASM("asm/battle/nonmatchings/main90", func_800B6808);
#endif


/* func_800B6930.s */
void func_800B6930(u32* dst, u8* p) {
    u32 n = (u32)((*(s8*)(p + 1) << 8) | p[0]);

    p += n;
    dst[0] = (u32)((*(s8*)(p + 1) << 8) | p[0]) << 16;
    dst[1] = (u32)((*(s8*)(p + 3) << 8) | p[2]) << 16;
    dst[2] = (u32)((*(s8*)(p + 5) << 8) | p[4]) << 16;
}
/* func_800B6990.s */
void func_800B6990(u16* dst, u8* p) {
    s32 hi;
    u32 lo;
    u32 n = (u32)((*(s8*)(p + 1) << 8) | p[0]);

    p += n;
    hi = *(s8*)(p + 1);
    lo = p[0];
    *(u16*)((u8*)dst + 0) = (u16)((hi << 8) | lo);
    hi = *(s8*)(p + 3);
    lo = p[2];
    *(u16*)((u8*)dst + 2) = (u16)((hi << 8) | lo);
    hi = *(s8*)(p + 5);
    lo = p[4];
    *(u16*)((u8*)dst + 4) = (u16)((hi << 8) | lo);
}
