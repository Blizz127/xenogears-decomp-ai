/* Retail run 1 of mainc25: the byte-exact C bodies on one side of a
 * retail-asm gap; see the BATTLE_SUB note in mainc25.c. */
#define BATTLE_TU_SUB 1
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/mainc25_q1", func_8007C840);
INCLUDE_ASM("asm/battle/nonmatchings/mainc25_q1", func_8007C9D4);
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
