#include "common.h"
#ifndef BATTLE_TU_SUB
#define BATTLE_TU_SUB 0
#endif
/* Selects one run of byte-exact C bodies when a retail-asm gap splits them;
 * 0 (host/port/tests) compiles every run. */
#define BATTLE_SUB(n) (BATTLE_TU_SUB == 0 || BATTLE_TU_SUB == (n))

extern u8 D_800D3420[];
extern u8 D_800D3410[];
extern u8 D_800D343F;
extern u8 D_800D342E;

#ifndef XENO_PC_PORT
static __inline__ u16* BattleCopyRow16(s32 idx) {
    register u16* base asm("$4");
    u32 offset = (idx & 0xFF) << 6;
    base = (u16*)D_800D3420;
    return (u16*)((u8*)base + offset);
}
static __inline__ u32* BattleCopyRow32(s32 idx) {
    register u32* base asm("$4");
    u32 offset = (idx & 0xFF) << 6;
    base = (u32*)D_800D3410;
    return (u32*)((u8*)base + offset);
}
#else
static __inline__ u16* BattleCopyRow16(s32 idx) {
    return (u16*)(D_800D3420 + ((idx & 0xFF) << 6));
}
static __inline__ u32* BattleCopyRow32(s32 idx) {
    return (u32*)(D_800D3410 + ((idx & 0xFF) << 6));
}
#endif


/* func_8007B98C.s: row copy u16: T[p[2]] = T[p[1]] in D_800D3420 row. */
#if BATTLE_SUB(1)
void func_8007B98C(u8 **pp, s32 idx) {
    u8 *p = *pp;
    u16 *t = BattleCopyRow16(idx);

    t[p[2]] = t[p[1]];
}
#endif /* BATTLE_SUB(1) */

/* func_8007B9C8.s: row copy u32: T[p[2]] = T[p[1]] in D_800D3410 row. */
#if BATTLE_SUB(1)
void func_8007B9C8(u8 **pp, s32 idx) {
    u8 *p = *pp;
    u32 *t = BattleCopyRow32(idx);

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
