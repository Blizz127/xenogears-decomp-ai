/* Retail TU part 2 of main72.c (see the BATTLE_TU_PART note there): the
 * still-assembly run below, then that file's part-2 C bodies. */
#define BATTLE_TU_PART 2
#include "common.h"
#ifndef XENO_PC_PORT
extern u16 D_800C3E30;
extern void func_800AAD54(u8* obj, s32 id, s32 a2, s32 a3, s32 a4);

/* Record `src`'s +0x20 id and `slot` in obj's (up to five-entry) history when
 * it already has one; otherwise take slot's entry from src's tables, reset
 * obj's motion state and restart it through func_800AAD54. */
void func_800AA934(u8* obj, u8* src, s32 id, s32 slot) {
    if (obj == NULL || src == NULL) {
        return;
    }
    if (obj[0x2B] != 0) {
        if (obj[0x2B] < 5) {
            obj[0x2B]++;
        }
        (obj + obj[0x2B])[0x2A] = src[0x20];
        (obj + obj[0x2B])[0x2E] = slot;
        return;
    }
    if (slot < 0x50) {
        *(s32*)(obj + 0x10) = ((s32*)*(s32*)(src + 8))[slot];
    } else {
        *(s32*)(obj + 0x10) = *(s32*)(slot * 4 + *(s32*)(*(s32*)(src + 0xC) + 4) - 0x138);
    }
    {
        u16 now = D_800C3E30;

    *(s16*)(obj + 0x42) = 0;
    *(s16*)(obj + 0x40) = 0;
    *(s32*)(obj + 0x50) = 0;
    *(s32*)(obj + 0x54) = 0;
    *(s32*)(obj + 0x4C) = 0;
    obj[0x23] = 0;
    *(u16*)(obj + 0x10A) = now;
    }
    func_800AAD54(obj, id, -1, 1, 0);
}
extern void func_800AFF9C(void);
extern void func_8009F1C4(u8* model, s16 frame);
extern void func_8009EF3C(u8* model, s16 frame);
extern s32 func_800A0838(s32 id, u8* model, u16 a2, s16 frame);
extern void func_800AE2A4(u8* obj, s32 id, s32 a2);

/* Step obj's motion `steps` times (when +0x34 is set), OR-ing the per-step
 * flags, then hand them to func_800AAD54.  Returns the flags (unset when obj
 * has no +0 motion, as in retail). */
s32 func_800AAA20(u8* obj, s32 id, s32 steps, s32 a3, s32 a4) {
    s32 flags;
    s32 i;

    if (*(s32*)obj != 0) {
        flags = 0;
        if (obj[0x34] != 0) {
            func_800AFF9C();
            if (obj[0x37] != 0) {
                func_8009F1C4(*(u8**)(obj + 4), *(s16*)(obj + 0x1C));
            } else {
                func_8009EF3C(*(u8**)(obj + 4), *(s16*)(obj + 0x1C));
            }
            for (i = 0; i < steps; i++) {
                flags |= func_800A0838(id, *(u8**)(obj + 4), *(u16*)(obj + 0x3C), *(s16*)(obj + 0x1C));
                func_800AE2A4(obj, id, a3);
            }
        }
        func_800AAD54(obj, id, flags, steps, a4);
    }
    return flags;
}

INCLUDE_ASM("asm/battle/nonmatchings/main72_p2", func_800AAB34);
INCLUDE_ASM("asm/battle/nonmatchings/main72_p2", func_800AAD54);
INCLUDE_ASM("asm/battle/nonmatchings/main72_p2", func_800ADF1C);
extern u8* func_800A2330(u8* pool);

/* Take (once) the object's +0x70 pool record (func_800A2330 from `pool`) and
 * fill it like func_800B0164's: type 1, kind + 7, 0xFE, then the distance from
 * the object's +0x5C position to (x, y, z) + 1, a3, arg4, the target point
 * and arg5.  Retail used GNU C's `a ?: b` for the get-or-allocate. */
void func_800AE098(u8* pool, u8* obj, s32 kind, s32 a3, s32 arg4, s32 arg5, s32 x, s32 y, s32 z) {
    u8* e;
    s32 dx;
    s32 dy;
    s32 dz;

    e = *(u8**)(obj + 0x70) ?: func_800A2330(pool);
    if (e != NULL) {
        e[0] = 1;
        e[2] = kind + 7;
        e[1] = 0;
        e[3] = 0xFE;
        dx = x - *(s32*)(obj + 0x5C);
        dy = y - *(s32*)(obj + 0x60);
        dz = z - *(s32*)(obj + 0x64);
        *(u16*)(e + 4) = SquareRoot0(dx * dx + dy * dy + dz * dz) + 1;
        *(u16*)(e + 6) = a3;
        *(u16*)(e + 8) = arg4;
        *(u16*)(e + 0xA) = x;
        *(u16*)(e + 0xC) = y;
        *(u16*)(e + 0xE) = z;
        *(u16*)(e + 0x10) = 0;
        *(u16*)(e + 0x12) = arg5;
        *(u8**)(obj + 0x70) = e;
    }
}
#endif
#include "main72.c"
