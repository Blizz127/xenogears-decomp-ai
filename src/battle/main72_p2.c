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

#include "psyq/libgte.h"
#include "battle/gte_inline.h"
extern u32 D_800D3368[];

/* Attach actor to the record in slot +0x5C of D_800D3368 (its owner); with
 * no record there, detach it (+0x5C = 0xFF).  Otherwise copy the owner's
 * +0x34 unless +0x4A has 0x10, and while it is set:
 * - with +0x5D set, MulMatrix2 the actor model's +0xC and +0x2C matrices by
 *   the owner model's part matrix (part 0 at +0x2C, times part +0x5E's
 *   (0x7C bytes each) into the scratchpad when +0x5E is nonzero);
 * - load the owner model's +0xC matrix (CompMatrix'd with part +0x5E's into
 *   the scratchpad when nonzero) as the GTE rotation and translation,
 *   transform the actor's +0x6A/+0x6C/+0x6E offset and store the result
 *   at the actor model's +0x20 and +0x5C. */
void func_800AAB34(u8* actor) {
    MATRIX* m;
    SVECTOR v;

    m = (MATRIX*)0x1F800000;
    if (D_800D3368[actor[0x5C]] != 0) {
        if (!(*(u16*)(actor + 0x4A) & 0x10)) {
            actor[0x34] = ((u8*)D_800D3368[actor[0x5C]])[0x34];
        }
        if (actor[0x34] == 0) {
            return;
        }
        if (actor[0x5D] != 0) {
            if (*(s16*)(actor + 0x5E) != 0) {
                MulMatrix0((MATRIX*)(*(u8**)(D_800D3368[actor[0x5C]] + 4) + 0x2C),
                           (MATRIX*)(*(s16*)(actor + 0x5E) * 0x7C + *(s32*)(D_800D3368[actor[0x5C]] + 4) + 0x2C),
                           (MATRIX*)0x1F800000);
            } else {
                m = (MATRIX*)(*(u8**)(D_800D3368[actor[0x5C]] + 4) + 0x2C);
            }
            MulMatrix2(m, (MATRIX*)(*(u8**)(actor + 4) + 0xC));
            MulMatrix2(m, (MATRIX*)(*(u8**)(actor + 4) + 0x2C));
        }
        if (*(s16*)(actor + 0x5E) != 0) {
            CompMatrix((MATRIX*)(*(u8**)(D_800D3368[actor[0x5C]] + 4) + 0xC),
                       (MATRIX*)(*(s16*)(actor + 0x5E) * 0x7C + *(s32*)(D_800D3368[actor[0x5C]] + 4) + 0x2C), m);
        } else {
            m = (MATRIX*)(*(u8**)(D_800D3368[actor[0x5C]] + 4) + 0xC);
        }
        SetRotMatrix(m);
        SetTransMatrix(m);
        v.vx = *(s16*)(actor + 0x6A);
        v.vy = *(s16*)(actor + 0x6C);
        v.vz = *(s16*)(actor + 0x6E);
        BATTLE_GTE_LDV0(&v);
        BATTLE_GTE_RTV0TR();
        BATTLE_GTE_STLVNL(*(u8**)(actor + 4) + 0x20);
        BATTLE_GTE_STLVNL(*(u8**)(actor + 4) + 0x5C);
    } else {
        actor[0x5C] = 0xFF;
    }
}
INCLUDE_ASM("asm/battle/nonmatchings/main72_p2", func_800AAD54);
extern u8* func_800A2330(u8* pool);

/* Retarget the object's rotation (+0x54/+0x56/+0x58) to (rx, ry, rz): with
 * fewer than two steps set it at once (and raise +5); otherwise, when it
 * changes, fill the +0x70 pool record as a type-3 turn from the current angles
 * by the wrapped (-0x800..0x7FF) deltas over `steps`. */
void func_800ADF1C(u8* pool, u8* obj, s32 steps, s32 rx, s32 ry, s32 rz) {
    u8* e;

    if (steps < 2) {
        *(s16*)(obj + 0x54) = rx;
        *(s16*)(obj + 0x56) = ry;
        *(s16*)(obj + 0x58) = rz;
        obj[5] = 1;
        return;
    }
    if (*(s16*)(obj + 0x54) == rx && *(s16*)(obj + 0x56) == ry && *(s16*)(obj + 0x58) == rz) {
        return;
    }
    e = *(u8**)(obj + 0x70) ?: func_800A2330(pool);
    if (e != NULL) {
        e[0] = 1;
        e[2] = 3;
        e[1] = 0;
        e[3] = 0xFE;
        *(u16*)(e + 4) = *(u16*)(obj + 0x54);
        *(u16*)(e + 6) = *(u16*)(obj + 0x56);
        *(u16*)(e + 8) = *(u16*)(obj + 0x58);
        rx = (rx - *(s16*)(obj + 0x54)) & 0xFFF;
        if (rx >= 0x800) {
            rx -= 0x1000;
        }
        *(u16*)(e + 0xA) = rx;
        ry = (ry - *(s16*)(obj + 0x56)) & 0xFFF;
        if (ry >= 0x800) {
            ry -= 0x1000;
        }
        *(u16*)(e + 0xC) = ry;
        rz = (rz - *(s16*)(obj + 0x58)) & 0xFFF;
        if (rz >= 0x800) {
            rz -= 0x1000;
        }
        *(u16*)(e + 0xE) = rz;
        *(u16*)(e + 0x10) = 0;
        *(u16*)(e + 0x12) = steps;
        *(u8**)(obj + 0x70) = e;
    }
}
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
