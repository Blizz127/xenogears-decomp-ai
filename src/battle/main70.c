#include "common.h"
#ifndef XENO_PC_PORT
#include "system/memory.h"
#endif


#ifndef XENO_PC_PORT
void func_800A22A8(s32 *);
void func_800A2D1C(s32 *);
void func_800A9FF0(s32);
extern s32 D_800C3D04;
extern s32 D_800C3D0C;
void func_800A8A88(void *arg0);
void func_800A9F94(void);
s32 func_800AA514(s16 arg0, s16 arg1, s32 arg2);
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A5EB4);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A6444);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A64E4);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A6884);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A6AE8);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A6F98);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A7064);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A7948);

#ifndef XENO_PC_PORT
void func_800A8A88(void *arg0) {
    s32 *temp_a0;
    s32 *temp_a0_2;

    temp_a0 = *(s32 **)((s8*)(arg0) + 0x14);
    if (temp_a0 != NULL) {
        HeapFree(temp_a0);
        HeapFree(**(s32 **)((s8*)(arg0) + 0x1C));
        HeapFree(*(s32 **)((s8*)(arg0) + 0x1C));
        HeapFree(*(s32 **)((s8*)(arg0) + 0x20));
        temp_a0_2 = *(s32 **)((s8*)(arg0) + 0x18);
        if (temp_a0_2 != NULL) {
            HeapFree(temp_a0_2);
        }
        *(s32 **)((s8*)(arg0) + 0x14) = NULL;
    }
}
#endif

INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A8B0C);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A8BF0);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A9540);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A96B4);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A979C);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A9A50);

#ifndef XENO_PC_PORT
void func_800A9F94(void) {
    s32 var_s0;

    var_s0 = 0;
    do {
        func_800A9FF0(var_s0);
        var_s0 += 1;
    } while (var_s0 < 0x1F);
    func_800A22A8(&D_800C3D0C);
    func_800A2D1C(&D_800C3D04);
}
#endif

INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A9FF0);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800AA320);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800AA384);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800AA454);

#ifndef XENO_PC_PORT
s32 func_800AA514(s16 arg0, s16 arg1, s32 arg2) {
    s16 temp_v0;
    s16 var_v1;

    temp_v0 = arg2 + ((s32) (arg1 * arg0) / 256);
    var_v1 = temp_v0;
    if (temp_v0 >= 0x100) {
        var_v1 = 0xFF;
    }
    return var_v1 & 0xFF;
}
#endif

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
}
#ifndef XENO_PC_PORT
extern u32 D_800D3368[];
#endif
#ifndef XENO_PC_PORT
extern u8 D_800C3B74;
#endif


/* func_800AA760.s */
void func_800AA760(u32 index, u8 value) {
#ifdef XENO_PC_PORT
    /* The index is unbounded and the slot holds a guest address, so both the
     * table read and the +0x2A store need PSX_ADDR; see func_800AA79C in
     * main71.c for the same pair of fixes. */
    u32 p = *(u32*)PSX_ADDR(0x800D3368 + index * 4);

    if (p != 0) {
        *(u8*)PSX_ADDR(p + 0x2A) = value;
    }
#else
    u32 p = D_800D3368[index];

    if (p != 0) {
        *(u8*)(p + 0x2A) = value;
    }
#endif
}
/* func_800AA788.s */
void func_800AA788(u32 value) {
    D_800C3B74 = (u8)(value & 1);
}
