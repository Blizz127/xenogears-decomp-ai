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
extern u8 D_800D3014, D_800C3E29;
extern void func_8008AA74(u32);
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
extern u8 D_800D366C;
extern void func_80087A38(u32);
extern void func_80084A7C(u32);
extern void func_80077698(void);

/* func_800829F4.s, byte-exact: a command-ring step on D_800D3014 (the
 * func_8008115C family).  Events 4/6/7: the reject call func_8008AA74(0x4F)
 * when the actor row's +0x3C target is 0xFF, else func_80087A38(actor),
 * func_80084A7C(actor), state 0x19, func_80077698() and D_800D366C = 0.
 * Events 1 and 0 reject when the row's +0x34/+0x36 halfword is set, else
 * state 0x11/0x15; event 2 -> state 0x12; event 3 follows
 * func_8008115C's case 3 with +0x28/+0x24 and states 0x13/0x18.  The
 * switch table is this object's .rodata (jtbl_80070110).  The target row
 * is addressed with `actor << 6`: as `actor * 64` the base and offset
 * registers swap (3/111). */
void func_800829F4(u32 arg0) {
    u32 actor;
    switch (D_800D3014) {
    case 4:
    case 6:
    case 7:
        actor = arg0 & 0xFF;
        if (((u8 *)D_800C3EAC + (actor << 6))[0x3C] == 0xFF) {
            arg0 = 0x4F;
            goto reject;
        }
        func_80087A38(actor);
        func_80084A7C(actor);
        ((u8 *)D_800C3EAC)[0x2DD] = 0x19;
        func_80077698();
        D_800D366C = 0;
        return;
    case 1: {
        u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

        if (*(u16 *)(row + 0x34)) {
            arg0 = 0x4F;
            goto reject;
        }
    }
        ((u8 *)D_800C3EAC)[0x2DD] = 0x11;
        return;
    case 2:
        ((u8 *)D_800C3EAC)[0x2DD] = 0x12;
        return;
    case 3: {
        u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

        if (!*(u16 *)(row + 0x28)) {
            ((u8 *)D_800C3EAC)[0x2DD] = 0x13;
            return;
        }
        if (((u8 *)D_800C3EAC)[0x2F6] && D_800C3E29 == 3) {
            if (*(u16 *)(row + 0x24)) func_8008AA74(0x4F);
            else ((u8 *)D_800C3EAC)[0x2DD] = 0x18;
            ((u8 *)D_800C3EAC)[0x2F6] = 0;
            return;
        }
        arg0 = 0x4F;
        ((u8 *)D_800C3EAC)[0x2F6] = 1;
        goto reject;
    }
    case 0: {
        u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

        if (*(u16 *)(row + 0x36)) {
            arg0 = 0x4F;
            goto reject;
        }
    }
        ((u8 *)D_800C3EAC)[0x2DD] = 0x15;
        return;
    default:
        return;
    }
reject:
    func_8008AA74(arg0);
}

extern u8 func_8008BED8(u32);
extern void func_8008B908(u32);

/* func_80082BB0.s, byte-exact: a command-ring step on D_800D3014 (the
 * func_8008115C family).  Events 4/6/7: func_8008B908(actor) if
 * func_8008BED8(actor) is nonzero, else state 0x11.  Events 0 and 1 make
 * the reject call func_8008AA74(0x4F) when the row's +0x2E/+0x34 halfword
 * is set, else state 0x10/0x16; event 2 -> state 0x12; event 3 follows
 * func_8008115C's case 3 with +0x28/+0x24 and states 0x13/0x18.  The
 * switch table is this object's .rodata (jtbl_80070130).  Each event
 * keeps its own row local, as in func_80082D4C. */
void func_80082BB0(u32 arg0) {
    u32 actor;
    switch (D_800D3014) {
    case 4:
    case 6:
    case 7:
        actor = arg0 & 0xFF;
        if (func_8008BED8(actor)) {
            func_8008B908(actor);
            return;
        }
        ((u8 *)D_800C3EAC)[0x2DD] = 0x11;
        return;
    case 0: {
        u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

        if (*(u16 *)(row + 0x2E)) {
            arg0 = 0x4F;
            goto reject;
        }
    }
        ((u8 *)D_800C3EAC)[0x2DD] = 0x10;
        return;
    case 2:
        ((u8 *)D_800C3EAC)[0x2DD] = 0x12;
        return;
    case 3: {
        u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

        if (!*(u16 *)(row + 0x28)) {
            ((u8 *)D_800C3EAC)[0x2DD] = 0x13;
            return;
        }
        if (((u8 *)D_800C3EAC)[0x2F6] && D_800C3E29 == 3) {
            if (*(u16 *)(row + 0x24)) func_8008AA74(0x4F);
            else ((u8 *)D_800C3EAC)[0x2DD] = 0x18;
            ((u8 *)D_800C3EAC)[0x2F6] = 0;
            return;
        }
        arg0 = 0x4F;
        ((u8 *)D_800C3EAC)[0x2F6] = 1;
        goto reject;
    }
    case 1: {
        u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

        if (*(u16 *)(row + 0x34)) {
            arg0 = 0x4F;
            goto reject;
        }
    }
        ((u8 *)D_800C3EAC)[0x2DD] = 0x16;
        return;
    default:
        return;
    }
reject:
    func_8008AA74(arg0);
}

extern void func_8009AA44(u32);
extern volatile u16 D_800CCDC4[][184];
extern u16 D_800CCDC6[][184];
extern u16 D_800D2C32;

/* func_80082D4C.s, byte-exact WITH A VOLATILE VIEW (see below): a
 * command-ring step on D_800D3014 (the func_8008115C family).  Events
 * 4/6/7: func_8009AA44(actor), add D_800D2C32 to the actor's u16 at
 * D_800CCDC4 (0x170 stride), clamp it to the u16 at D_800CCDC6, then set
 * context +0x2EB + actor to 1, +0x2EA to 0 and +0x2DE to 1.  Events 0, 1
 * and 2 make the reject call func_8008AA74(0x4F) when the actor row's
 * +0x2E/+0x34/+0x2C halfword is set and otherwise move to state
 * 0x10/0x11/0x17.  Event 3 follows func_8008115C's case 3 with +0x28/+0x24
 * and states 0x13/0x18.  The switch table is this object's .rodata
 * (jtbl_80070150, placed by config/battle.yaml).
 * Retail re-reads the sum from memory before the clamp; with a plain u16
 * array CSE forwards the stored value instead (548 vs 560 bytes), and
 * every non-volatile spelling tried keeps that or reorders the loads.
 * D_800CCDC4 is therefore declared volatile here -- a construct, not a
 * natural match; count it apart and revisit.  Each event keeps its own
 * row local: one shared local stays live across event 3's branches and
 * moves every row into $a0. */
void func_80082D4C(u32 arg0) {
    u32 actor;
    switch (D_800D3014) {
    case 4:
    case 6:
    case 7:
        actor = arg0 & 0xFF;
        func_8009AA44(actor);
        D_800CCDC4[actor][0] += D_800D2C32;
        if (D_800CCDC4[actor][0] > D_800CCDC6[actor][0]) {
            D_800CCDC4[actor][0] = D_800CCDC6[actor][0];
        }
        *((u8 *)D_800C3EAC + actor + 0x2EB) = 1;
        ((u8 *)D_800C3EAC)[0x2EA] = 0;
        ((u8 *)D_800C3EAC)[0x2DE] = 1;
        return;
    case 0:
        {
            u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

            if (*(u16 *)(row + 0x2E)) {
                arg0 = 0x4F;
                goto reject;
            }
        }
        ((u8 *)D_800C3EAC)[0x2DD] = 0x10;
        return;
    case 1:
        {
            u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

            if (*(u16 *)(row + 0x34)) {
                arg0 = 0x4F;
                goto reject;
            }
        }
        ((u8 *)D_800C3EAC)[0x2DD] = 0x11;
        return;
    case 3: {
        u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

        if (!*(u16 *)(row + 0x28)) {
            ((u8 *)D_800C3EAC)[0x2DD] = 0x13;
            return;
        }
        if (((u8 *)D_800C3EAC)[0x2F6] && D_800C3E29 == 3) {
            if (*(u16 *)(row + 0x24)) func_8008AA74(0x4F);
            else ((u8 *)D_800C3EAC)[0x2DD] = 0x18;
            ((u8 *)D_800C3EAC)[0x2F6] = 0;
            return;
        }
        arg0 = 0x4F;
        ((u8 *)D_800C3EAC)[0x2F6] = 1;
        goto reject;
    }
    case 2:
        {
            u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

            if (*(u16 *)(row + 0x2C)) {
                arg0 = 0x4F;
                goto reject;
            }
        }
        ((u8 *)D_800C3EAC)[0x2DD] = 0x17;
        return;
    default:
        return;
    }
reject:
    func_8008AA74(arg0);
}

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
extern u16 D_800CCD64[][184], D_800CCD6C[][184], D_800CCE08[][184], D_800CCE0C[][184];

/* func_800830A8.s, byte-exact WITH THE FAMILY'S CROSS-JUMP CONSTRUCT (see
 * below): a command-ring step on D_800D3014 (the func_8008115C family).
 * Events 4/6/7 toggle the actor's 0x8000 status bit: when the u16 at
 * D_800CCE0C (0x170 stride) has it, clear it there and at D_800CCD6C;
 * otherwise clear bit 0x20 at D_800CCE08 and bit 0x1000 at D_800CCD64 and
 * set 0x8000 at D_800CCE0C and D_800CCD6C; then context +0x2DE = 1.
 * Events 1 and 0 make the reject call func_8008AA74(0x4F) when the row's
 * +0x34/+0x2E halfword is set, else state 0x16/0x10.  Event 2 checks
 * +0x2C (state 0x17), then with +0x2F6 set and D_800C3E29 == 2 moves to
 * state 0x12; event 3 follows func_8008115C's case 3 with +0x24/+0x28 and
 * states 0x18/0x13.  The switch table is this object's .rodata
 * (jtbl_80070190).  The four status arrays are separate symbols: retail
 * schedules their loads across each other's stores.
 * Construct: retail keeps events 2 and 3's identical "+0x2F6 = 1, reject"
 * blocks apart, but gcc's final jump pass cross-jumps them in every
 * natural spelling tried.  Storing the first one through an s32 local
 * keeps them distinct (decomp-permuter found this on func_80081318).  It
 * is not a natural match; count it apart. */
void func_800830A8(u32 arg0) {
    u32 actor;
    switch (D_800D3014) {
    case 4:
    case 6:
    case 7:
        actor = arg0 & 0xFF;
        if (D_800CCE0C[actor][0] & 0x8000) {
            D_800CCE0C[actor][0] &= 0x7FFF;
            D_800CCD6C[actor][0] &= 0x7FFF;
        } else {
            D_800CCE08[actor][0] &= 0xFFDF;
            D_800CCD64[actor][0] &= 0xEFFF;
            D_800CCE0C[actor][0] |= 0x8000;
            D_800CCD6C[actor][0] |= 0x8000;
        }
        ((u8 *)D_800C3EAC)[0x2DE] = 1;
        return;
    case 1: {
        u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

        if (*(u16 *)(row + 0x34)) {
            arg0 = 0x4F;
            goto reject;
        }
    }
        ((u8 *)D_800C3EAC)[0x2DD] = 0x16;
        return;
    case 2: {
        u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

        if (!*(u16 *)(row + 0x2C)) {
            ((u8 *)D_800C3EAC)[0x2DD] = 0x17;
            return;
        }
        if (((u8 *)D_800C3EAC)[0x2F6] && D_800C3E29 == 2) {
            ((u8 *)D_800C3EAC)[0x2DD] = 0x12;
            ((u8 *)D_800C3EAC)[0x2F6] = 0;
            return;
        }
        arg0 = 0x4F;
        {
            s32 on = 1;

            ((u8 *)D_800C3EAC)[0x2F6] = on;
        }
        goto reject;
    }
    case 3: {
        u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

        if (!*(u16 *)(row + 0x24)) {
            ((u8 *)D_800C3EAC)[0x2DD] = 0x18;
            return;
        }
        if (((u8 *)D_800C3EAC)[0x2F6] && D_800C3E29 == 3) {
            if (*(u16 *)(row + 0x28)) func_8008AA74(0x4F);
            else ((u8 *)D_800C3EAC)[0x2DD] = 0x13;
            ((u8 *)D_800C3EAC)[0x2F6] = 0;
            return;
        }
        arg0 = 0x4F;
        ((u8 *)D_800C3EAC)[0x2F6] = 1;
        goto reject;
    }
    case 0: {
        u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

        if (*(u16 *)(row + 0x2E)) {
            arg0 = 0x4F;
            goto reject;
        }
    }
        ((u8 *)D_800C3EAC)[0x2DD] = 0x10;
        return;
    default:
        return;
    }
reject:
    func_8008AA74(arg0);
}

extern u8 func_8008BED8(u32);
extern void func_8008B908(u32);

/* func_80083340.s, byte-exact WITH THE CROSS-JUMP CONSTRUCT (twice): a
 * command-ring step on D_800D3014 (the func_8008115C family).  Events
 * 4/6/7: func_8008B908(actor) if func_8008BED8(actor) is nonzero, else
 * state 0x16.  Event 0 checks +0x36 (state 0x15), then with +0x2F6 set and
 * D_800C3E29 == 0 rejects or moves to state 0x10 by +0x2E; event 2 checks
 * +0x2C (state 0x17), then moves to state 0x12; event 3 follows
 * func_8008115C's case 3 with +0x24/+0x28 and states 0x18/0x13; event 1
 * rejects when +0x34 is set, else state 0x11.  The switch table is this
 * object's .rodata (jtbl_800701B0).  Construct, as in func_800830A8:
 * events 0, 2 and 3 end in identical "+0x2F6 = 1, reject" blocks that
 * retail keeps apart; the first stores through an s32 local and the second
 * through an s16 local, so the final jump pass cannot merge any two of
 * them.  Not a natural match; count it apart. */
void func_80083340(u32 arg0) {
    u32 actor;
    switch (D_800D3014) {
    case 4:
    case 6:
    case 7:
        actor = arg0 & 0xFF;
        if (func_8008BED8(actor)) {
            func_8008B908(actor);
            return;
        }
        ((u8 *)D_800C3EAC)[0x2DD] = 0x16;
        return;
    case 0: {
        u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

        if (!*(u16 *)(row + 0x36)) {
            ((u8 *)D_800C3EAC)[0x2DD] = 0x15;
            return;
        }
        if (((u8 *)D_800C3EAC)[0x2F6] && D_800C3E29 == 0) {
            if (*(u16 *)(row + 0x2E)) func_8008AA74(0x4F);
            else ((u8 *)D_800C3EAC)[0x2DD] = 0x10;
            ((u8 *)D_800C3EAC)[0x2F6] = 0;
            return;
        }
        arg0 = 0x4F;
        {
            s32 on = 1;

            ((u8 *)D_800C3EAC)[0x2F6] = on;
        }
        goto reject;
    }
    case 2: {
        u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

        if (!*(u16 *)(row + 0x2C)) {
            ((u8 *)D_800C3EAC)[0x2DD] = 0x17;
            return;
        }
        if (((u8 *)D_800C3EAC)[0x2F6] && D_800C3E29 == 2) {
            ((u8 *)D_800C3EAC)[0x2DD] = 0x12;
            ((u8 *)D_800C3EAC)[0x2F6] = 0;
            return;
        }
        arg0 = 0x4F;
        {
            s16 on = 1;

            ((u8 *)D_800C3EAC)[0x2F6] = on;
        }
        goto reject;
    }
    case 3: {
        u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

        if (!*(u16 *)(row + 0x24)) {
            ((u8 *)D_800C3EAC)[0x2DD] = 0x18;
            return;
        }
        if (((u8 *)D_800C3EAC)[0x2F6] && D_800C3E29 == 3) {
            if (*(u16 *)(row + 0x28)) func_8008AA74(0x4F);
            else ((u8 *)D_800C3EAC)[0x2DD] = 0x13;
            ((u8 *)D_800C3EAC)[0x2F6] = 0;
            return;
        }
        arg0 = 0x4F;
        ((u8 *)D_800C3EAC)[0x2F6] = 1;
        goto reject;
    }
    case 1: {
        u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

        if (*(u16 *)(row + 0x34)) {
            arg0 = 0x4F;
            goto reject;
        }
    }
        ((u8 *)D_800C3EAC)[0x2DD] = 0x11;
        return;
    default:
        return;
    }
reject:
    func_8008AA74(arg0);
}

extern u8 D_800C48EA;
extern u32 func_8009A9D0(u8);

/* func_80083580.s, byte-exact: a command-ring step on D_800D3014 (the
 * func_8008115C family).  Events 4/6/7: D_800C48EA = 0x40 if
 * func_8009A9D0(context +0x2D3) is nonzero, then context +0x2DE = 1.
 * Event 0 checks +0x36 (state 0x15), then with +0x2F6 set and
 * D_800C3E29 == 0 rejects or moves to state 0x10 by +0x2E, else sets
 * +0x2F6 and rejects; event 1 rejects on +0x34, else state 0x16; event 3
 * is event 0 with +0x24/+0x28, states 0x18/0x13 and D_800C3E29 == 3;
 * event 2 moves to state 0x12.  Unlike its siblings this one calls
 * func_8008AA74(0x4F) directly at each reject, and retail cross-jumps
 * the identical tails (events 0 and 3 share one "+0x2F6 = 1, reject"
 * block), so no construct is needed.  The switch table is this object's
 * .rodata (jtbl_800701D0).  func_8009A9D0 (main55) is defined (void);
 * retail passes the context byte anyway. */
void func_80083580(u32 arg0) {
    switch (D_800D3014) {
    case 4:
    case 6:
    case 7:
        if (func_8009A9D0(((u8 *)D_800C3EAC)[0x2D3])) {
            D_800C48EA = 0x40;
        }
        ((u8 *)D_800C3EAC)[0x2DE] = 1;
        return;
    case 0: {
        u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

        if (!*(u16 *)(row + 0x36)) {
            ((u8 *)D_800C3EAC)[0x2DD] = 0x15;
            return;
        }
        if (((u8 *)D_800C3EAC)[0x2F6] && D_800C3E29 == 0) {
            if (*(u16 *)(row + 0x2E)) func_8008AA74(0x4F);
            else ((u8 *)D_800C3EAC)[0x2DD] = 0x10;
            ((u8 *)D_800C3EAC)[0x2F6] = 0;
            return;
        }
        ((u8 *)D_800C3EAC)[0x2F6] = 1;
        func_8008AA74(0x4F);
        return;
    }
    case 1: {
        u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

        if (*(u16 *)(row + 0x34)) {
            func_8008AA74(0x4F);
            return;
        }
    }
        ((u8 *)D_800C3EAC)[0x2DD] = 0x16;
        return;
    case 3: {
        u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

        if (!*(u16 *)(row + 0x24)) {
            ((u8 *)D_800C3EAC)[0x2DD] = 0x18;
            return;
        }
        if (((u8 *)D_800C3EAC)[0x2F6] && D_800C3E29 == 3) {
            if (*(u16 *)(row + 0x28)) func_8008AA74(0x4F);
            else ((u8 *)D_800C3EAC)[0x2DD] = 0x13;
            ((u8 *)D_800C3EAC)[0x2F6] = 0;
            return;
        }
        ((u8 *)D_800C3EAC)[0x2F6] = 1;
        func_8008AA74(0x4F);
        return;
    }
    case 2:
        ((u8 *)D_800C3EAC)[0x2DD] = 0x12;
        return;
    }
}

extern void func_8007FE3C(u32);
extern u8 func_8008CFB8(u32);
extern void func_8008ADD0(u32);

/* func_80083748.s, byte-exact WITH THE CROSS-JUMP CONSTRUCT (see
 * func_800830A8): a command-ring step on D_800D3014 (the func_8008115C
 * family).  Events 4/6/7: func_8007FE3C(actor), then func_8008ADD0(actor)
 * if func_8008CFB8(actor) is nonzero, else state 0x18.  Event 0 checks
 * +0x36 (state 0x15), then with +0x2F6 set and D_800C3E29 == 0 rejects or
 * moves to state 0x10 by +0x2E; event 1 rejects on +0x34, else state
 * 0x16; event 2 checks +0x2C (state 0x17), then moves to state 0x12;
 * event 3 rejects on +0x28, else state 0x13.  Events 0 and 2 end in
 * identical "+0x2F6 = 1, reject" blocks that retail keeps apart; the
 * first stores through an s32 local.  Not a natural match; count it
 * apart.  The switch table is this object's .rodata (jtbl_800701F0).
 * func_8007FE3C (main33) is defined (void); retail passes the actor. */
void func_80083748(u32 arg0) {
    u32 actor;
    switch (D_800D3014) {
    case 4:
    case 6:
    case 7:
        actor = arg0 & 0xFF;
        func_8007FE3C(actor);
        if (func_8008CFB8(actor)) {
            func_8008ADD0(actor);
            return;
        }
        ((u8 *)D_800C3EAC)[0x2DD] = 0x18;
        return;
    case 0: {
        u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

        if (!*(u16 *)(row + 0x36)) {
            ((u8 *)D_800C3EAC)[0x2DD] = 0x15;
            return;
        }
        if (((u8 *)D_800C3EAC)[0x2F6] && D_800C3E29 == 0) {
            if (*(u16 *)(row + 0x2E)) func_8008AA74(0x4F);
            else ((u8 *)D_800C3EAC)[0x2DD] = 0x10;
            ((u8 *)D_800C3EAC)[0x2F6] = 0;
            return;
        }
        arg0 = 0x4F;
        {
            s32 on = 1;

            ((u8 *)D_800C3EAC)[0x2F6] = on;
        }
        goto reject;
    }
    case 1: {
        u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

        if (*(u16 *)(row + 0x34)) {
            arg0 = 0x4F;
            goto reject;
        }
    }
        ((u8 *)D_800C3EAC)[0x2DD] = 0x16;
        return;
    case 2: {
        u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

        if (!*(u16 *)(row + 0x2C)) {
            ((u8 *)D_800C3EAC)[0x2DD] = 0x17;
            return;
        }
        if (((u8 *)D_800C3EAC)[0x2F6] && D_800C3E29 == 2) {
            ((u8 *)D_800C3EAC)[0x2DD] = 0x12;
            ((u8 *)D_800C3EAC)[0x2F6] = 0;
            return;
        }
        arg0 = 0x4F;
        ((u8 *)D_800C3EAC)[0x2F6] = 1;
        goto reject;
    }
    case 3: {
        u8 *row = (u8 *)D_800C3EAC + (arg0 & 0xFF) * 64;

        if (*(u16 *)(row + 0x28)) {
            arg0 = 0x4F;
            goto reject;
        }
    }
        ((u8 *)D_800C3EAC)[0x2DD] = 0x13;
        return;
    default:
        return;
    }
reject:
    func_8008AA74(arg0);
}

INCLUDE_ASM("asm/battle/nonmatchings/main35_p2", func_80083948);
#endif
#include "main35.c"
