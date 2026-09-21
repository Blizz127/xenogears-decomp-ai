#include "common.h"
#ifndef BATTLE_TU_SUB
#define BATTLE_TU_SUB 0
#endif
/* Selects one run of byte-exact C bodies when a retail-asm gap splits them;
 * 0 (host/port/tests) compiles every run. */
#define BATTLE_SUB(n) (BATTLE_TU_SUB == 0 || BATTLE_TU_SUB == (n))

#ifndef XENO_PC_PORT
extern u8 D_800D3420[];
#endif
#ifndef XENO_PC_PORT
extern u8 D_800D3410[];
#endif
#ifndef XENO_PC_PORT
extern u8 D_800D3430[];
#endif


/* func_8007B98C.s: row copy u16: T[p[2]] = T[p[1]] in D_800D3420 row. */
#if BATTLE_SUB(1)
void func_8007B98C(u8 **pp, s32 idx) {
    u8 *p = *pp;
#if !defined(XENO_PC_PORT)
    register u16 *base asm("$4");
    u16 *t;
    u32 offset = (idx & 0xFF) << 6;
    base = (u16 *)D_800D3420;
    t = (u16 *)((u8 *)base + offset);
#else
    u16 *t = (u16 *)(D_800D3420 + ((idx & 0xFF) << 6));
#endif

    t[p[2]] = t[p[1]];
}
#endif /* BATTLE_SUB(1) */

/* func_8007B9C8.s: row copy u32: T[p[2]] = T[p[1]] in D_800D3410 row. */
#if BATTLE_SUB(1)
void func_8007B9C8(u8 **pp, s32 idx) {
    u8 *p = *pp;
#if !defined(XENO_PC_PORT)
    register u32 *base asm("$4");
    u32 *t;
    u32 offset = (idx & 0xFF) << 6;
    base = (u32 *)D_800D3410;
    t = (u32 *)((u8 *)base + offset);
#else
    u32 *t = (u32 *)(D_800D3410 + ((idx & 0xFF) << 6));
#endif

    t[p[2]] = t[p[1]];
}
#endif /* BATTLE_SUB(1) */

/* func_8007BA04.s: row u8->u16: T[p[2]] = B16[p[1]] (B16 = row base + 0x10). */
/* Not yet byte-matching: the matching build assembles retail bytes for
 * this function from its own asm segment (see config/battle.yaml).  The
 * body below is kept for the port and the differential harnesses. */
#ifdef XENO_PC_PORT
void func_8007BA04(u8 **pp, s32 idx) {
    u32 row = (idx & 0xFF) << 6;
    u8 *base = D_800D3420;
    u16 *t = (u16 *)(base + row);
    u8 *b16 = base + 0x10 + row;
    u8 *p = *pp;

    t[p[2]] = b16[p[1]];
}
#endif /* XENO_PC_PORT */

/* func_8007BA44.s: row u32 = u16: T[p[2]] = B16[p[1]] (T = D_800D3410 row,
 * B16 = row base + 0x10, p[1] scaled by 2, p[2] scaled by 4). */
/* Not yet byte-matching: the matching build assembles retail bytes for
 * this function from its own asm segment (see config/battle.yaml).  The
 * body below is kept for the port and the differential harnesses. */
#ifdef XENO_PC_PORT
void func_8007BA44(u8 **pp, s32 idx) {
    u32 row = (idx & 0xFF) << 6;
    u8 *base = D_800D3410;
    u32 *t = (u32 *)(base + row);
    u16 *b16 = (u16 *)(base + 0x10 + row);
    u8 *p = *pp;

    t[p[2]] = b16[p[1]];
}
#endif /* XENO_PC_PORT */



#ifndef XENO_PC_PORT
extern u8 D_800D343F;
#endif
#ifndef XENO_PC_PORT
extern u8 D_800D342E;
#endif


/* func_8007BA88.s */
#if BATTLE_SUB(2)
void func_8007BA88(u8 index) {
    s32 i = 0xF;
    u8* p = (u8*)((u32)&D_800D343F + ((u32)index << 6));

    for (; i >= 0; i--) {
        *p-- = 0;
    }
}
#endif /* BATTLE_SUB(2) */
/* func_8007BAB8.s */
#if BATTLE_SUB(2)
void func_8007BAB8(u8 index) {
    s32 i = 7;
    u16* p = (u16*)((u32)&D_800D342E + ((u32)index << 6));

    for (; i >= 0; i--) {
        *p-- = 0;
    }
}
#endif /* BATTLE_SUB(2) */
