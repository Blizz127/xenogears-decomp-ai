#include "common.h"
/* Retail TU split: gcc 2.7.2 cc1 emits every file-scope INCLUDE_ASM block
 * before the first compiled body, so a mixed TU whose retail layout has a C
 * function ahead of an assembly function cannot be one object.  Each
 * contiguous [asm run][C run] of the retail layout is therefore its own
 * matching TU: src/battle/main40_p<N>.c defines BATTLE_TU_PART=<N> and
 * includes this file.  Host/port builds compile this file whole (part 0). */
#ifndef BATTLE_TU_PART
#define BATTLE_TU_PART 0
#endif
#define BATTLE_PART(n) (BATTLE_TU_PART == 0 || BATTLE_TU_PART == (n))


extern u8 D_800C33B0[];
extern u8 D_800CCB34;
extern u8 *D_800D2DB4;
extern u32 func_80076A10(u8 sel, u8 *p, u32 m, u32 n);
extern void func_80076B68(u8 *p);
extern void func_80076BF0(u8 *p);
extern u32 func_8001BD40(u32 a0, u32 a1, u32 a2, u32 a3);
extern u8 D_800C3A94;
extern u8 D_800C3A98;
extern u8 D_800C207C;
extern s32 D_800C3A7C;
extern s32 D_800C3A80;
extern s32 D_800C3A84;
extern s32 D_800C3A88;
extern s32 D_800C3A8C;
extern s32 D_800C3A90;
extern s32 D_800C3A9C;
extern s32 D_800C2080;
extern s32 D_800C2084;


#if BATTLE_PART(1)
/* func_8008860C.s */
/* Not yet byte-matching: the matching build assembles retail bytes for
 * this function from its own asm segment (see config/battle.yaml).  The
 * body below is kept for the port and the differential harnesses. */
#ifdef XENO_PC_PORT
void func_8008860C(void) {
    s32 i;
    s32 j;
    u8 *base;
    for (i = 0; i < 2; i++) {
        u8 v0;
        u8 r;
        base = D_800D2DB4;
        v0 = base[0x5D74];
        r = (u8)func_80076A10(D_800C33B0[i], base + (u32)v0 * 80u + 0x1720u, 0xA0u, 0x64u);
        base = D_800D2DB4;
        base[0x5D74] = (u8)(base[0x5D74] + r);
    }
    base = D_800D2DB4;
    base[0x5D83] = D_800CCB34;
    {
        u8 v0 = base[0x5D70];
        u8 r = (u8)func_80076A10(0xA8u, base + (u32)v0 * 80u, 0xA0u, 0x64u);
        base = D_800D2DB4;
        base[0x5D70] = r;
        base[0x5D92] = D_800CCB34;
    }
    base = D_800D2DB4;
    if (base[0x5D74] > 0) {
        j = 0;
        do {
            u8 v1 = base[0x5D83];
            func_80076B68(base + ((u32)j * 2u + (u32)v1) * 40u + 0x1720u);
            base = D_800D2DB4;
            j++;
        } while (j < base[0x5D74]);
    }
    base = D_800D2DB4;
    if (base[0x5D70] > 0) {
        j = 0;
        do {
            u8 v1 = base[0x5D92];
            func_80076B68(base + ((u32)j * 2u + (u32)v1) * 40u + 0x1720u);
            base = D_800D2DB4;
            j++;
        } while (j < base[0x5D70]);
    }
    for (i = 2; i < 4; i++) {
        u8 v0;
        u8 r;
        base = D_800D2DB4;
        v0 = base[0x5D7E];
        r = (u8)func_80076A10(D_800C33B0[i], base + (u32)v0 * 80u + 0x2530u, 0xA0u, 0x64u);
        base = D_800D2DB4;
        base[0x5D7E] = (u8)(base[0x5D7E] + r);
    }
    base = D_800D2DB4;
    base[0x5D8D] = D_800CCB34;
    base = D_800D2DB4;
    if (base[0x5D7E] > 0) {
        j = 0;
        do {
            u8 v1 = base[0x5D8D];
            func_80076BF0(base + ((u32)j * 2u + (u32)v1) * 40u + 0x2530u);
            base = D_800D2DB4;
            j++;
        } while (j < base[0x5D7E]);
    }
}
#endif /* XENO_PC_PORT */

/* func_8008887C.s */
/* Not yet byte-matching: the matching build assembles retail bytes for
 * this function from its own asm segment (see config/battle.yaml).  The
 * body below is kept for the port and the differential harnesses. */
#ifdef XENO_PC_PORT
void func_8008887C(s32 x0, s32 y0, s32 x1, s32 y1) {
    s32 dx;
    s32 dy;
    D_800C3A7C = x0;
    D_800C3A80 = y0;
    D_800C3A84 = x1;
    D_800C3A88 = y1;
    if (x1 == x0)
        return;
    if (y1 == y0)
        return;
    if (x1 < x0) {
        D_800C3A94 = 1;
        dx = (s32)((u32)x0 - (u32)x1);
    } else {
        D_800C3A94 = 0;
        dx = (s32)((u32)x1 - (u32)x0);
    }
    if (y1 < y0) {
        D_800C3A98 = 1;
        dy = (s32)((u32)y0 - (u32)y1);
    } else {
        D_800C3A98 = 0;
        dy = (s32)((u32)y1 - (u32)y0);
    }
    if (dx < dy) {
        D_800C3A90 = 0x100;
        D_800C3A8C = (s32)((u32)dx << 8) / dy;
    } else {
        D_800C3A8C = 0x100;
        D_800C3A90 = (s32)((u32)dy << 8) / dx;
    }
    D_800C2080 = 0;
    D_800C2084 = 0;
    D_800C3A9C = (s32)(func_8001BD40(1u, 8u, (u32)x1, (u32)y1) & 0xFFu);
    D_800C207C = 0;
}
#endif /* XENO_PC_PORT */

/* func_80088990.s: retail evaluates the rounding shift separately inside each
 * direction arm rather than hoisting it above the flag test. */
/* Not yet byte-matching: the matching build assembles retail bytes for
 * this function from its own asm segment (see config/battle.yaml).  The
 * body below is kept for the port and the differential harnesses. */
#ifdef XENO_PC_PORT
void func_80088990(void) {
    s32 n = D_800C3A9C;
    if (n > 0) {
        u8 dirx = D_800C3A94;
        s32 stepx = D_800C3A8C;
        u8 diry = D_800C3A98;
        s32 stepy = D_800C3A90;
        s32 i = 0;
        do {
            if (dirx != 0)
                D_800C2080 = (s32)((u32)D_800C2080 - (u32)stepx);
            else
                D_800C2080 = (s32)((u32)D_800C2080 + (u32)stepx);
            if (diry != 0)
                D_800C2084 = (s32)((u32)D_800C2084 - (u32)stepy);
            else
                D_800C2084 = (s32)((u32)D_800C2084 + (u32)stepy);
            i++;
        } while (i < n);
    }
    if (D_800C3A8C == 0x100) {
        if (D_800C3A94 != 0) {
            s32 v = D_800C2080;

            if (v < 0)
                v += 0xFF;
            v >>= 8;
            if ((s32)((u32)v + (u32)D_800C3A7C) < D_800C3A84)
                D_800C207C = 1;
        } else {
            s32 v = D_800C2080;

            if (v < 0)
                v += 0xFF;
            v >>= 8;
            if (D_800C3A84 < (s32)((u32)v + (u32)D_800C3A7C))
                D_800C207C = 1;
        }
    } else {
        if (D_800C3A98 != 0) {
            s32 v = D_800C2084;

            if (v < 0)
                v += 0xFF;
            v >>= 8;
            if ((s32)((u32)v + (u32)D_800C3A80) < D_800C3A88)
                D_800C207C = 1;
        } else {
            s32 v = D_800C2084;

            if (v < 0)
                v += 0xFF;
            v >>= 8;
            if (D_800C3A88 < (s32)((u32)v + (u32)D_800C3A80))
                D_800C207C = 1;
        }
    }
}
#endif /* XENO_PC_PORT */

#endif /* BATTLE_PART(1) */
#if BATTLE_PART(2)
#endif /* BATTLE_PART(2) */


extern void func_8008860C(void);
extern void func_80089038(void);
extern void func_80089110(void);
extern void func_800891E4(void);
extern void func_80089348(void);
extern void func_8008946C(void);
extern void func_8008963C(void);
extern void func_800897CC(void);


#if BATTLE_PART(2)
/* func_80089AF8.s */
void func_80089AF8(void) {
    func_8008860C();
    func_80089038();
    func_80089110();
    func_800891E4();
    func_80089348();
    func_8008946C();
    func_8008963C();
    func_800897CC();
}
#endif /* BATTLE_PART(2) */
