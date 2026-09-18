#include "common.h"
#ifndef BATTLE_TU_SUB
#define BATTLE_TU_SUB 0
#endif
/* Selects one run of byte-exact C bodies when a retail-asm gap splits them;
 * 0 (host/port/tests) compiles every run. */
#define BATTLE_SUB(n) (BATTLE_TU_SUB == 0 || BATTLE_TU_SUB == (n))

extern u8 D_800D3430[];
extern u8 D_800D3420[];


/* func_8007AE38.s: u8 saturating add row[p[3]] = min(row[p[1]] + row[p[2]], 0xFF) */
#if BATTLE_SUB(1)
void func_8007AE38(u8** ppBoard, u8 index) {
    u8* p = *ppBoard;
    u8* pRow = D_800D3430 + ((u32)index << 6);
    s32 v = pRow[p[1]] + pRow[p[2]];
    u8 value = v;

    /* Retail keeps the saturated result in its own variable (a register copy of
     * the arithmetic temporary) and tests the temporary with a signed compare. */
    if (v >= 0x100) {
        value = 0xFF;
    }
    pRow[p[3]] = value;
}
#endif /* BATTLE_SUB(1) */
/* func_8007AE98.s: u8 saturating sub row[p[3]] = max(row[p[1]] - row[p[2]], 0) */
#if BATTLE_SUB(1)
void func_8007AE98(u8** ppBoard, u8 index) {
    u8* p = *ppBoard;
    u8* pRow = D_800D3430 + ((u32)index << 6);
    s32 v = pRow[p[1]] - pRow[p[2]];
    u8 value = v;

    if (v < 0) {
        value = 0;
    }
    pRow[p[3]] = value;
}
#endif /* BATTLE_SUB(1) */
/* func_8007AEF0.s: u8 multiply of two row cells, saturate at 0xFF (16-bit sign-extended compare) */
/* Not yet byte-matching: the matching build assembles retail bytes for
 * this function from its own asm segment (see config/battle.yaml).  The
 * body below is kept for the port and the differential harnesses. */
#ifdef XENO_PC_PORT
void func_8007AEF0(u8** ppBoard, u8 index) {
    u8* p = *ppBoard;
    u8* pRow = D_800D3430 + ((u32)index << 6);
    /* NOT YET EXACT (1 word, size exact): retail emits `mult cell1,cell2`
     * keeping p in $a2; this shape gives `mult cell2,cell1` with p in $a0.
     * Same open register-allocation artifact as main17/func_8007AA60. */
    s32 x = pRow[p[1]];
    u8 y = pRow[p[2]];
    s32 prod = y * x;
    u8 value = prod;

    if ((s32)(s16)prod >= 0x100) {
        value = 0xFF;
    }
    pRow[p[3]] = value;
}
#endif /* XENO_PC_PORT */

/* func_8007AF5C.s */
#if BATTLE_SUB(2)
void func_8007AF5C(u8** ppBoard, u8 index) {
    u8* p = *ppBoard;
    u8* pRow = D_800D3430 + ((u32)index << 6);

    pRow[p[3]] = pRow[p[1]] / pRow[p[2]];
}
#endif /* BATTLE_SUB(2) */
/* func_8007AFAC.s */
#if BATTLE_SUB(2)
void func_8007AFAC(u8** ppBoard, u8 index) {
    u8* p = *ppBoard;
    u8* pRow = D_800D3430 + ((u32)index << 6);

    pRow[p[3]] = pRow[p[1]] % pRow[p[2]];
}
#endif /* BATTLE_SUB(2) */
/* func_8007AFFC.s */
#if BATTLE_SUB(2)
void func_8007AFFC(u8** ppBoard, u8 index) {
    u8* p = *ppBoard;
    u8* row = D_800D3430 + ((u32)index << 6);

    row[p[3]] = row[p[1]] & row[p[2]];
}
#endif /* BATTLE_SUB(2) */
/* func_8007B040.s */
#if BATTLE_SUB(2)
void func_8007B040(u8** ppBoard, u8 index) {
    u8* p = *ppBoard;
    u8* row = D_800D3430 + ((u32)index << 6);

    row[p[3]] = row[p[1]] | row[p[2]];
}
#endif /* BATTLE_SUB(2) */
/* func_8007B084.s */
#if BATTLE_SUB(2)
void func_8007B084(u8** ppBoard, u8 index) {
    u8* p = *ppBoard;
    u8* row = D_800D3430 + ((u32)index << 6);

    row[p[3]] = row[p[1]] ^ row[p[2]];
}
#endif /* BATTLE_SUB(2) */
/* func_8007B0C8.s */
#if BATTLE_SUB(2)
void func_8007B0C8(u8** ppBoard, u8 index) {
    u8* p = *ppBoard;
    u16* row = (u16*)(D_800D3420 + ((u32)index << 6));
    s32 v = row[p[1]] + row[p[2]];

    if (v > 0xFFFF) {
        v = 0xFFFF;
    }
    row[p[3]] = (u16)v;
}
#endif /* BATTLE_SUB(2) */
/* func_8007B134.s */
#if BATTLE_SUB(2)
void func_8007B134(u8** ppBoard, u8 index) {
    u8* p = *ppBoard;
    u16* row = (u16*)(D_800D3420 + ((u32)index << 6));
    s32 v = row[p[1]] - row[p[2]];

    if (v < 0) {
        v = 0;
    }
    row[p[3]] = (u16)v;
}
#endif /* BATTLE_SUB(2) */
/* func_8007B198.s */
#if BATTLE_SUB(2)
void func_8007B198(u8** ppBoard, u8 index) {
    u8* p = *ppBoard;
    u16* row = (u16*)(D_800D3420 + ((u32)index << 6));
    s32 v = row[p[1]] * row[p[2]];

    if (v > 0xFFFF) {
        v = 0xFFFF;
    }
    row[p[3]] = (u16)v;
}
#endif /* BATTLE_SUB(2) */
/* func_8007B208.s */
#if BATTLE_SUB(2)
void func_8007B208(u8** ppBoard, u8 index) {
    u8* p = *ppBoard;
    u16* row = (u16*)(D_800D3420 + ((u32)index << 6));

    row[p[3]] = row[p[1]] / row[p[2]];
}
#endif /* BATTLE_SUB(2) */
/* func_8007B264.s */
#if BATTLE_SUB(2)
void func_8007B264(u8** ppBoard, u8 index) {
    u8* p = *ppBoard;
    u16* row = (u16*)(D_800D3420 + ((u32)index << 6));

    row[p[3]] = row[p[1]] % row[p[2]];
}
#endif /* BATTLE_SUB(2) */
