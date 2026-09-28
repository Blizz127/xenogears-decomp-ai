/* Retail TU part 2 of main40.c (see the BATTLE_TU_PART note there): the
 * still-assembly run below, then that file's part-2 C bodies. */
#define BATTLE_TU_PART 2
#include "common.h"
#ifndef XENO_PC_PORT
u32 func_80076A10(u32 sel, u8 *p, s32 m, s32 n);
u32 func_80089C6C(u32 mask, u8 index);
extern u8 D_800CCB34;
extern u16 D_800D2C30;
extern u8 * D_800D2DB4;
void func_80089038(void);
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main40_p2", func_80088B80);

#ifndef XENO_PC_PORT
void func_80089038(void) {
    s32 temp_v0;
    s32 var_s0;
    s32 var_s1;

    var_s0 = 0;
    var_s1 = 0x6E0000;
    *(u8 *)((s8*)((*(void **)&D_800D2DB4)) + 0x5DA1) = 0U;
    do {
        if (func_80089C6C(D_800D2C30, var_s0 & 0xFF) & 0xFFFF) {
            temp_v0 = func_80076A10(var_s0 + 0xC4, (*(u8 *)((s8*)((*(void **)&D_800D2DB4)) + 0x5DA1) * 0x50) + 0x4CE0 + (*(void **)&D_800D2DB4), 0xE0, var_s1 >> 0x10);
            var_s1 += 0xA0000;
            *(u8 *)((s8*)((*(void **)&D_800D2DB4)) + 0x5DA1) = (u8) (*(u8 *)((s8*)((*(void **)&D_800D2DB4)) + 0x5DA1) + temp_v0);
        }
        var_s0 += 1;
    } while (var_s0 < 5);
    *(u8 *)((s8*)((*(void **)&D_800D2DB4)) + 0x5DA0) = (u8) D_800CCB34;
    *(s16 *)((s8*)((*(void **)&D_800D2DB4)) + 0x5DA2) = 0;
}
#endif

INCLUDE_ASM("asm/battle/nonmatchings/main40_p2", func_80089110);
INCLUDE_ASM("asm/battle/nonmatchings/main40_p2", func_800891E4);
INCLUDE_ASM("asm/battle/nonmatchings/main40_p2", func_80089348);
INCLUDE_ASM("asm/battle/nonmatchings/main40_p2", func_8008946C);
INCLUDE_ASM("asm/battle/nonmatchings/main40_p2", func_8008963C);
INCLUDE_ASM("asm/battle/nonmatchings/main40_p2", func_800897CC);
INCLUDE_ASM("asm/battle/nonmatchings/main40_p2", func_800898F0);
#endif
#include "main40.c"
