/* Retail run 1 of mainc25: the byte-exact C bodies on one side of a
 * retail-asm gap; see the BATTLE_SUB note in mainc25.c. */
#define BATTLE_TU_SUB 1
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/mainc25_q1", func_8007C840);

extern u32 func_8007A628(u32 a0, u32 a1);
extern u8 func_8001BD40(u8 a0, u8 a1);
extern u32 func_80089C08(u8 idx);
extern u8 D_800D32A1[];
extern u8 D_800D3420[];

/* func_8007C9D4.s: clear u16 slot (*pp)[1] of row idx of D_800D3420, then
 * draw party actors with func_8001BD40(0, 2) until one not tried yet passes
 * func_8007A628(r, (*pp)[2]) with D_800D32A1[r * 8] set; store
 * func_80089C08(r) in the slot.  Each failed draw marks tried[r]; stop when
 * all three are marked.  The found branch recomputes the row address into
 * dst: loop.c hoists that invariant into the preheader, where it becomes
 * retail's copy of the first row ($a1 -> $s4); reusing row is 3 insns
 * longer.  k holds r * 8 so D_800D32A1 is indexed as D_800D32A1($at)
 * instead of a hoisted la.  Found with a decomp-permuter hint. */
void func_8007C9D4(u8** pp, u8 idx) {
    u8 tried[3];
    u8* row;
    u8* dst;
    u8 r;
    u32 k;

    tried[2] = 0;
    tried[1] = 0;
    tried[0] = 0;
    row = D_800D3420 + (idx << 6);
    *(u16*)(row + (*pp)[1] * 2) = 0;
    while (!(tried[0] & tried[1] & tried[2])) {
        r = func_8001BD40(0, 2);
        if (tried[r] == 0) {
            if ((func_8007A628(r, (*pp)[2]) & 0xFF) != 0) {
                k = r * 8;
                if (D_800D32A1[k] != 0) {
                    dst = D_800D3420 + (idx << 6);
                    *(u16*)(dst + (*pp)[1] * 2) = func_80089C08(r);
                    return;
                }
            }
            tried[r] = 1;
        }
    }
}

INCLUDE_ASM("asm/battle/nonmatchings/mainc25_q1", func_8007CB20);

extern u32 func_8007A628(u32 a0, u32 a1);
extern u8 D_800C3EB4[];
extern u8 D_800D3430[];

/* func_8007CC50.s: count the party actors 0..2 that pass func_8007A628(i, 0)
 * and whose 28-byte D_800C3EB4 record starts with (*pp)[2]; store the count
 * in byte (*pp)[1] of row idx of D_800D3430.  The record offset is its own
 * counter, as in main27's func_8007D6A8: gcc 2.6.3 then reduces the offset
 * and indexes the table as D_800C3EB4($at).  n is set first, as retail
 * clears it before i and off. */
void func_8007CC50(u8** pp, s32 idx) {
    u8 n = 0;
    s32 i = 0;
    u32 off = 0;
    u8* row;

    for (; i < 3; i++) {
        if ((func_8007A628((u32)i & 0xFF, 0) & 0xFF) != 0) {
            if (D_800C3EB4[off] == (*pp)[2]) {
                n++;
            }
        }
        off += 28;
    }
    row = D_800D3430 + ((idx & 0xFF) << 6);
    row[(*pp)[1]] = n;
}

/* func_8007CD10.s: func_8007CC50 for the enemy actors 3..10. */
void func_8007CD10(u8** pp, s32 idx) {
    u8 n = 0;
    s32 i = 3;
    u32 off = 0x54;
    u8* row;

    for (; i < 0xB; i++) {
        if ((func_8007A628((u32)i & 0xFF, 0) & 0xFF) != 0) {
            if (D_800C3EB4[off] == (*pp)[2]) {
                n++;
            }
        }
        off += 28;
    }
    row = D_800D3430 + ((idx & 0xFF) << 6);
    row[(*pp)[1]] = n;
}

INCLUDE_ASM("asm/battle/nonmatchings/mainc25_q1", func_8007CDD0);

extern u32 func_80079E7C(u16 value);
extern u8 D_800D3420[];

/* func_8007CEA4.s: func_8007CC50 with the record byte taken from an actor:
 * count the party actors 0..2 that pass func_8007A628(i, 0) and whose
 * D_800C3EB4 record starts with the same byte as the record of actor
 * func_80079E7C(mask), mask being u16 slot (*pp)[2] of row idx of
 * D_800D3420.  The mask is re-read for every actor, as retail does.  k
 * holds v * 28 so the table index is a register (a bare v * 28 index makes
 * expand load D_800C3EB4 into a register, which loop.c hoists); idx is u8
 * (an s32 idx copies a1 to s5 before n and i are cleared). */
void func_8007CEA4(u8** pp, u8 idx) {
    u8 n = 0;
    s32 i = 0;
    u8* src = D_800D3420 + (idx << 6);
    u32 off = 0;
    u8* row;

    for (; i < 3; i++) {
        if ((func_8007A628((u32)i & 0xFF, 0) & 0xFF) != 0) {
            u8 v = func_80079E7C(*(u16*)(src + (*pp)[2] * 2));
            u32 k = v * 28;

            if (D_800C3EB4[off] == D_800C3EB4[k]) {
                n++;
            }
        }
        off += 28;
    }
    row = D_800D3430 + (idx << 6);
    row[(*pp)[1]] = n;
}

/* func_8007CFB8.s: func_8007CEA4 for the enemy actors 3..10. */
void func_8007CFB8(u8** pp, u8 idx) {
    u8 n = 0;
    s32 i = 3;
    u8* src = D_800D3420 + (idx << 6);
    u32 off = 0x54;
    u8* row;

    for (; i < 0xB; i++) {
        if ((func_8007A628((u32)i & 0xFF, 0) & 0xFF) != 0) {
            u8 v = func_80079E7C(*(u16*)(src + (*pp)[2] * 2));
            u32 k = v * 28;

            if (D_800C3EB4[off] == D_800C3EB4[k]) {
                n++;
            }
        }
        off += 28;
    }
    row = D_800D3430 + (idx << 6);
    row[(*pp)[1]] = n;
}
#endif
#include "mainc25.c"
