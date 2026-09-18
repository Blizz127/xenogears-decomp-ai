#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A5EB4);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A6444);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A64E4);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A6884);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A6AE8);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A6F98);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A7064);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A7948);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A8A88);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A8B0C);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A8BF0);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A9540);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A96B4);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A979C);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A9A50);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A9F94);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A9FF0);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800AA320);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800AA384);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800AA454);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800AA514);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800AA564);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800AA600);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800AA650);
#endif
extern void* HeapAlloc(u32 size, u32 flags);
/* func_800AA6E0.s: allocate p[0x10C] 0x70-byte records into *(p + 0x110),
 * each with halfword +0 = -1 and word +8 = 0; nothing when the count is 0.
 * The count is re-read every iteration. */
void func_800AA6E0(u8* p) {
    u8 n = p[0x10C];

    if (n != 0) {
        u8* buf = HeapAlloc(n * 0x70, 0);
        s32 i;

        for (i = 0; i < p[0x10C]; i++) {
            *(s16*)(buf + i * 0x70) = -1;
            *(u32*)(buf + i * 0x70 + 8) = 0;
        }
        *(u8**)(p + 0x110) = buf;
    }
}extern u32 D_800D3368[];
extern u8 D_800C3B74;


/* func_800AA760.s */
void func_800AA760(u32 index, u8 value) {
    u32 p = D_800D3368[index];

    if (p != 0) {
        *(u8*)(p + 0x2A) = value;
    }
}
/* func_800AA788.s */
void func_800AA788(u32 value) {
    D_800C3B74 = (u8)(value & 1);
}
