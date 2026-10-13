/* Retail TU part 2 of main35.c (see the BATTLE_TU_PART note there): the
 * still-assembly run below, then that file's part-2 C bodies. */
#define BATTLE_TU_PART 2
#include "common.h"
#ifndef XENO_PC_PORT
u32 func_80084854(u32 arg0, u32 arg1);
u16 func_80089C08(u8);
void func_80093B08(s32);
void func_800BC404(u32);
void func_800BCD98(u32);
extern s32 D_800C3E24;
extern struct BattleCommandContext * D_800C3EAC;
extern u8 D_800D2C34;
void func_8008189C(s32 arg0);
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main35_p2", func_80081318);
INCLUDE_ASM("asm/battle/nonmatchings/main35_p2", func_80081504);
INCLUDE_ASM("asm/battle/nonmatchings/main35_p2", func_800816F8);

#ifndef XENO_PC_PORT
void func_8008189C(s32 arg0) {
    s32 var_s0;

    if (*(u8 *)((s8*)((*(void **)&D_800C3EAC)) + 0x2E9) == 0) {
        var_s0 = 0;
        func_800BC404(func_80089C08(*(u8 *)((s8*)((*(void **)&D_800C3EAC)) + 0x2E8)) & 0xFFFF);
        func_800BCD98(func_80089C08(*(u8 *)((s8*)((*(void **)&D_800C3EAC)) + 0x2E8)) & 0xFFFF);
        do {
            if ((func_80084854(*(u8 *)((s8*)((*(void **)&D_800C3EAC)) + 0x2E8), var_s0 & 0xFF) & 0xFF) != *(u8 *)((s8*)((*(void **)&D_800C3EAC)) + 0x2E8)) {
                *(s8 *)((s8*)((D_800C3E24 + var_s0)) + 0xE6) = 1;
            } else {
                *(s8 *)((s8*)((D_800C3E24 + var_s0)) + 0xE6) = 0;
            }
            var_s0 += 1;
        } while (var_s0 < 4);
        if (D_800D2C34 != 4) {
            func_80093B08(arg0 & 0xFF);
        }
    }
}
#endif

INCLUDE_ASM("asm/battle/nonmatchings/main35_p2", func_800819A4);
INCLUDE_ASM("asm/battle/nonmatchings/main35_p2", func_80081B58);
INCLUDE_ASM("asm/battle/nonmatchings/main35_p2", func_800820A4);
INCLUDE_ASM("asm/battle/nonmatchings/main35_p2", func_800822C4);
INCLUDE_ASM("asm/battle/nonmatchings/main35_p2", func_80082504);
INCLUDE_ASM("asm/battle/nonmatchings/main35_p2", func_800826CC);
INCLUDE_ASM("asm/battle/nonmatchings/main35_p2", func_80082820);
INCLUDE_ASM("asm/battle/nonmatchings/main35_p2", func_800829F4);
INCLUDE_ASM("asm/battle/nonmatchings/main35_p2", func_80082BB0);
INCLUDE_ASM("asm/battle/nonmatchings/main35_p2", func_80082D4C);
extern u8 D_800D3014;
extern void func_8008AA74(u32);
extern void func_8007FD38(u8);
extern u8 func_8008B478(u32);
extern void func_8008ADD0(u32);

/* func_80082F7C.s, byte-exact: a command-ring step on D_800D3014 (the
 * main35_p1 func_8008115C family).  Events 4/6/7: func_8007FD38(actor), then
 * func_8008ADD0(actor) if func_8008B478(actor) accepts, else state 0x13;
 * events 0, 1 and 3 make the reject call func_8008AA74(0x4F) when the actor
 * row's +0x2E/+0x34/+0x24 halfword is set and otherwise move to state
 * 0x10/0x11/0x18;
 * event 2 moves to state 0x12.  The switch table is this object's .rodata
 * (jtbl_80070170, placed by config/battle.yaml).  Each test goes through a
 * `row` local: indexing the context directly puts the row offset first in
 * the address add. */
void func_80082F7C(u32 arg0) {
    u32 actor;
    u8 *row;
    switch (D_800D3014) {
    case 4:
    case 6:
    case 7:
        actor = arg0 & 0xFF;
        func_8007FD38(actor);
        if (func_8008B478(actor)) {
            func_8008ADD0(actor);
            return;
        }
        ((u8 *)D_800C3EAC)[0x2DD] = 0x13;
        return;
    case 0:
        row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;
        if (*(u16 *)(row + 0x2E)) {
            arg0 = 0x4F;
            goto reject;
        }
        ((u8 *)D_800C3EAC)[0x2DD] = 0x10;
        return;
    case 1:
        row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;
        if (*(u16 *)(row + 0x34)) {
            arg0 = 0x4F;
            goto reject;
        }
        ((u8 *)D_800C3EAC)[0x2DD] = 0x11;
        return;
    case 2:
        ((u8 *)D_800C3EAC)[0x2DD] = 0x12;
        return;
    case 3:
        row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;
        if (*(u16 *)(row + 0x24)) {
            arg0 = 0x4F;
            goto reject;
        }
        ((u8 *)D_800C3EAC)[0x2DD] = 0x18;
        return;
    default:
        return;
    }
reject:
    func_8008AA74(arg0);
}
INCLUDE_ASM("asm/battle/nonmatchings/main35_p2", func_800830A8);
INCLUDE_ASM("asm/battle/nonmatchings/main35_p2", func_80083340);
INCLUDE_ASM("asm/battle/nonmatchings/main35_p2", func_80083580);
INCLUDE_ASM("asm/battle/nonmatchings/main35_p2", func_80083748);
INCLUDE_ASM("asm/battle/nonmatchings/main35_p2", func_80083948);
#endif
#include "main35.c"
