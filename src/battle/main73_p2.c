/* Retail TU part 2 of main73.c (see the BATTLE_TU_PART note there): the
 * still-assembly run below, then that file's part-2 C bodies. */
#define BATTLE_TU_PART 2
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main73_p2", func_800AF438);
INCLUDE_ASM("asm/battle/nonmatchings/main73_p2", func_800AF518);
INCLUDE_ASM("asm/battle/nonmatchings/main73_p2", func_800AF678);
void func_800AFA98(u8 *arg0, u8 *arg1, s32 arg2) {
    u8 *var_s0;
    s32 var_s1;

    arg1[7] = arg2 & 1;
    if (arg2 & 0x80) {
        var_s0 = *(u8 **)(arg0 + 4);
        var_s1 = 1;
        if (var_s1 < *(u16 *)(var_s0 + 0xA)) {
            do {
                var_s0 += 0x7C;
                var_s1++;
                if (*(u8 **)var_s0 == arg1) {
                    func_800AFA98(arg0, var_s0, arg2);
                }
            } while (var_s1 < *(u16 *)(*(u8 **)(arg0 + 4) + 0xA));
        }
    }
}

void func_800AFC68();
void SpriteSetScale(s32, s16);                   /* extern */
void TimerWorkListSetTaskCallback(void *, s32 *); /* extern */
s32 WorkListTaskGetTaskCallback(void *);            /* extern */
void func_80021FE0(s32, s16);                    /* extern */
void func_800223B0(s32, s16);                    /* extern */
void *func_80023FD8(s32, s32, s32, s32);         /* extern */

void func_800AFB4C(s32 arg0, s32 arg1, s32 arg2, s16 arg3, s32 arg4, void *arg5, s32 arg6) {
    s32 temp_s1;
    void *temp_s0;
    void *temp_v0;

    temp_v0 = func_80023FD8(arg1, arg0, arg2, 0x18);
    temp_s1 = (s32)temp_v0 + 0x38;
    func_80021FE0(temp_s1, arg3);
    func_800223B0(temp_s1, arg3);
    SpriteSetScale(temp_s1, (s16) arg4);
    temp_s0 = temp_v0 + *(s16 *)((s8*)(temp_v0) + 0xBE);
    *(s32 *)((s8*)(temp_s0) + 8) = arg6;
    *(s16 *)((s8*)(temp_s0) + 0xC) = (s16) *(u8 *)((s8*)(arg5) + 5);
    if (*(u8 *)((s8*)(arg5) + 0x13) != 0) {
        *(s32 *)((s8*)(temp_s0) + 4) = WorkListTaskGetTaskCallback(temp_v0);
        TimerWorkListSetTaskCallback(temp_v0, (s32 *)&func_800AFC68);
        *(u16 *)((s8*)(temp_s0) + 0x10) = (u16) *(u16 *)((s8*)(arg5) + 6);
        *(u16 *)((s8*)(temp_s0) + 0x12) = (u16) *(u16 *)((s8*)(arg5) + 8);
        *(u16 *)((s8*)(temp_s0) + 0x14) = (u16) *(u16 *)((s8*)(arg5) + 0xA);
        *(s16 *)((s8*)(temp_s0) + 0xE) = (s16) *(u8 *)((s8*)(arg5) + 0xC);
    }
}

INCLUDE_ASM("asm/battle/nonmatchings/main73_p2", func_800AFC68);
INCLUDE_ASM("asm/battle/nonmatchings/main73_p2", func_800AFD98);
INCLUDE_ASM("asm/battle/nonmatchings/main73_p2", func_800AFF9C);
#endif
/* func_800B0060.s */
extern void func_8003852C(u32 p);
extern void HeapFree(u32 p);
void func_800B0060(u8* p) {
    if (*(u32*)(p + 0xC) != 0) {
        if (p[0x63] != 0) {
            func_8003852C(*(u32*)(*(u32*)(p + 0xB4) + 8));
            p[0x63] = 0;
        }
        HeapFree(*(u32*)(p + 0xC));
        *(u32*)(p + 0xC) = 0;
        *(u32*)(p + 0x18) = 0;
    }
}
#include "main73.c"
