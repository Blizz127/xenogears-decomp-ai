/* Retail TU part 2 of main73.c (see the BATTLE_TU_PART note there): the
 * still-assembly run below, then that file's part-2 C bodies. */
#define BATTLE_TU_PART 2
#include "common.h"
#ifndef XENO_PC_PORT
extern u8 D_800C3D40;
extern s32 func_800AF400(void);

/* Decode target code `code` into a bit index and store *out = 1 << index:
 * 0xFF -> func_800AF400(), 0xFE -> D_800C3D40, 0xFD/0xF9 -> obj[0x20],
 * 0xFC -> obj[0x21], 0xFA -> 31, 0xF8/0xF7 -> obj[0x20] * 2 + 13/14, any
 * other code is the index itself.  Returns the index byte, which
 * func_800AAD54 reads at 7 of its 10 call sites; that second use is why
 * retail keeps the `andi` ahead of the SLLV. */
u8 func_800AF438(u8* obj, s32 code, u16* out) {
    u8 c = code;

    if (c == 0xFF) {
        code = func_800AF400();
    } else if (c == 0xFE) {
        code = D_800C3D40;
    } else if (c == 0xFD || c == 0xF9) {
        code = obj[0x20];
    } else if (c == 0xFC) {
        code = obj[0x21];
    } else if (c == 0xFA) {
        code = 0x1F;
    } else if (c == 0xF8) {
        code = obj[0x20] * 2 + 0xD;
    } else if (c == 0xF7) {
        code = obj[0x20] * 2 + 0xE;
    }
    *out = 1 << (u8)code;
    return code;
}

extern s32 func_8009C050(u8 idx);
extern u8 D_800CCD1E[];

/* Resolve slot `code` of the actor's +0x14/+0x18 pointer tables.  0xFF first
 * refreshes the +0x2A slot byte from func_8009C050's flags and the +0x4A
 * status bits (D_800CCD1E: 0x170-byte records); 0xFE and 0xFF then take the
 * slot from +0x2A (bit 7 goes to *flag).  Slots below 0x40 read
 * (+0x14)[slot + 1], others (+0x18)[slot - 0x3F]. */
u32 func_800AF518(u8* obj, u8 code, u32* flag) {
    u32 bits;

    *flag = 0;
    if (code >= 0xFE) {
        if (code == 0xFF) {
            bits = func_8009C050(obj[0x20]);
            if ((bits & 4) && (*(u16*)(obj + 0x4A) & 0x100)) {
                obj[0x2A] = 0x1B;
            } else if ((bits & 2) && (*(u16*)(obj + 0x4A) & 0x80)) {
                if (!(*(u16*)(D_800CCD1E + obj[0x20] * 0x170) & 1)) {
                    obj[0x2A] = 6;
                }
            } else if (!(*(u16*)(obj + 0x4A) & 0x400)) {
                obj[0x2A] = 1;
            }
            if (bits & 1) {
                obj[0x2A] |= 0x80;
            }
        }
        code = obj[0x2A] & 0x7F;
        *flag = obj[0x2A] & 0x80;
    }
    if (code < 0x40) {
        return (*(u32**)(obj + 0x14))[code + 1];
    }
    return (*(u32**)(obj + 0x18))[code - 0x3F];
}
INCLUDE_ASM("asm/battle/nonmatchings/main73_p2", func_800AF678);
void func_800AFA98(u8 *arg0, u8 *arg1, s32 arg2) {
    u8 *var_s0;
    s32 var_s1;

    arg1[7] = arg2 & 1;
    if (arg2 & 0x80) {
        var_s0 = *(u8 **)(arg0 + 4);
        var_s1 = 1;
        if (var_s1 < *(u16 *)(var_s0 + 0xA)) {
            do {
                var_s0 += 0x7C;
                var_s1++;
                if (*(u8 **)var_s0 == arg1) {
                    func_800AFA98(arg0, var_s0, arg2);
                }
            } while (var_s1 < *(u16 *)(*(u8 **)(arg0 + 4) + 0xA));
        }
    }
}

void func_800AFC68();
void SpriteSetScale(s32, s16);                   /* extern */
void TimerWorkListSetTaskCallback(void *, s32 *); /* extern */
s32 WorkListTaskGetTaskCallback(void *);            /* extern */
void func_80021FE0(s32, s16);                    /* extern */
void func_800223B0(s32, s16);                    /* extern */
void *func_80023FD8(s32, s32, s32, s32);         /* extern */

void func_800AFB4C(s32 arg0, s32 arg1, s32 arg2, s16 arg3, s32 arg4, void *arg5, s32 arg6) {
    s32 temp_s1;
    void *temp_s0;
    void *temp_v0;

    temp_v0 = func_80023FD8(arg1, arg0, arg2, 0x18);
    temp_s1 = (s32)temp_v0 + 0x38;
    func_80021FE0(temp_s1, arg3);
    func_800223B0(temp_s1, arg3);
    SpriteSetScale(temp_s1, (s16) arg4);
    temp_s0 = temp_v0 + *(s16 *)((s8*)(temp_v0) + 0xBE);
    *(s32 *)((s8*)(temp_s0) + 8) = arg6;
    *(s16 *)((s8*)(temp_s0) + 0xC) = (s16) *(u8 *)((s8*)(arg5) + 5);
    if (*(u8 *)((s8*)(arg5) + 0x13) != 0) {
        *(s32 *)((s8*)(temp_s0) + 4) = WorkListTaskGetTaskCallback(temp_v0);
        TimerWorkListSetTaskCallback(temp_v0, (s32 *)&func_800AFC68);
        *(u16 *)((s8*)(temp_s0) + 0x10) = (u16) *(u16 *)((s8*)(arg5) + 6);
        *(u16 *)((s8*)(temp_s0) + 0x12) = (u16) *(u16 *)((s8*)(arg5) + 8);
        *(u16 *)((s8*)(temp_s0) + 0x14) = (u16) *(u16 *)((s8*)(arg5) + 0xA);
        *(s16 *)((s8*)(temp_s0) + 0xE) = (s16) *(u8 *)((s8*)(arg5) + 0xC);
    }
}

#include "psyq/libgte.h"
#include "battle/gte_inline.h"

/* The 0x18-byte block func_800AFB4C appends to the sprite task (at
 * task + (s16)task[0xBE]). */
typedef struct {
    u32 unk0;
    void (*next)(u8* task); /* the task's previous callback */
    u8* owner;              /* model whose body carries the parts */
    s16 part;               /* 0 = the body matrix itself */
    s16 pinY;               /* nonzero: y comes from owner +0x60 */
    SVECTOR offset;
} BattleSpriteAnchor;

/* Task callback installed by func_800AFB4C: place the sprite at the anchor
 * offset transformed by the owner's body matrix (composed with part `part`'s
 * matrix in the scratchpad when part is nonzero), optionally pinning y to
 * the owner's +0x60, store it as 16.16 at task +0x38/+0x3C/+0x40, then run
 * the chained callback. */
void func_800AFC68(u8* task) {
    BattleSpriteAnchor* a;
    MATRIX* m;
    u8* body;
    VECTOR out;

    a = (BattleSpriteAnchor*)(task + *(s16*)(task + 0xBE));
    m = (MATRIX*)0x1F800000;
    if (a->part != 0) {
        body = *(u8**)(a->owner + 4);
        CompMatrix((MATRIX*)(body + 0xC), (MATRIX*)(a->part * 0x7C + (s32)body + 0x2C), (MATRIX*)0x1F800000);
    } else {
        m = (MATRIX*)(*(u8**)(a->owner + 4) + 0xC);
    }
    SetRotMatrix(m);
    SetTransMatrix(m);
    BATTLE_GTE_LDV0(&a->offset);
    BATTLE_GTE_RTV0TR();
    BATTLE_GTE_STLVNL(&out);
    if (a->pinY != 0) {
        out.vy = *(s16*)(a->owner + 0x60);
    }
    *(s32*)(task + 0x38) = out.vx << 16;
    *(s32*)(task + 0x3C) = out.vy << 16;
    *(s32*)(task + 0x40) = out.vz << 16;
    a->next(task);
}
/* func_800AFD98.s, byte-exact: set (flags & 0x20: add to) one of node's
 * three xyz triples chosen by flags & 7 (0: halfwords +0x54, 1: words
 * +0x5C, else halfwords +0x4C), mark +4 / +5, and with flags bit 7 recurse
 * into the entries of the (ctx + 4) table (0x7C bytes each, count at +0xA)
 * whose first word is node.  The port arm is the XENO_PC_PORT body in
 * main73.c.  Codegen: the entry pointer starts at the table and is
 * advanced at the top of the loop body, as retail keeps the table pointer
 * in that register.  Matches only under gcc 2.6.x (2.7.2: 512 vs 516
 * bytes). */
void func_800AFD98(u8* ctx, u8* node, u8 flags, s16 x, s16 y, s16 z) {
    s32 i;
    u8* e;

    if ((flags & 7) == 0) {
        if (flags & 0x20) {
            *(u16*)(node + 0x54) += x;
            *(u16*)(node + 0x56) += y;
            *(u16*)(node + 0x58) += z;
        } else {
            *(u16*)(node + 0x54) = x;
            *(u16*)(node + 0x56) = y;
            *(u16*)(node + 0x58) = z;
        }
    } else if ((flags & 7) == 1) {
        if (flags & 0x20) {
            *(s32*)(node + 0x5C) += x;
            *(s32*)(node + 0x60) += y;
            *(s32*)(node + 0x64) += z;
        } else {
            *(s32*)(node + 0x5C) = x;
            *(s32*)(node + 0x60) = y;
            *(s32*)(node + 0x64) = z;
        }
    } else {
        if (flags & 0x20) {
            *(u16*)(node + 0x4C) += x;
            *(u16*)(node + 0x4E) += y;
            *(u16*)(node + 0x50) += z;
        } else {
            *(u16*)(node + 0x4C) = x;
            *(u16*)(node + 0x4E) = y;
            *(u16*)(node + 0x50) = z;
        }
    }
    node[4] = 1;
    node[5] = 1;
    if (flags & 0x80) {
        e = *(u8**)(ctx + 4);
        for (i = 1; i < *(u16*)(*(u8**)(ctx + 4) + 0xA); i++) {
            e += 0x7C;
            if (*(u8**)e == node) {
                func_800AFD98(ctx, e, flags, x, y, z);
            }
        }
    }
}
extern s32 func_800A5914(SVECTOR* pos, s32 tri, s32 tries);
extern s32 func_800A579C(SVECTOR* pos);
extern s32 func_800A5870(SVECTOR* pos, s32 tri, VECTOR* normal);

/* func_800AFF9C.s, byte-exact: walk-mesh floor query for obj.  The model's
 * (obj +4) position +0x5C/+0x60/+0x64 goes into a short vector; the cached
 * triangle obj +0x1E is refreshed with func_800A5914(pos, tri, 4), or with
 * func_800A579C(pos) when that is negative; func_800A5870 then sets pos.vy
 * to the floor height, which goes to obj +0x60 and, unless obj +0x36 is set,
 * back into the model's +0x60.  The port runs the lifted body in main73.c.
 * The model coordinates are read as s16 halves (lhu); s32 reads are lw. */
void func_800AFF9C(u8* obj) {
    VECTOR normal;
    SVECTOR pos;
    s16 tri;

    pos.vx = *(s16*)(*(u8**)(obj + 4) + 0x5C);
    pos.vy = *(s16*)(*(u8**)(obj + 4) + 0x60);
    pos.vz = *(s16*)(*(u8**)(obj + 4) + 0x64);
    tri = func_800A5914(&pos, *(s16*)(obj + 0x1E), 4);
    *(s16*)(obj + 0x1E) = tri;
    if (tri < 0) {
        *(s16*)(obj + 0x1E) = func_800A579C(&pos);
    }
    func_800A5870(&pos, *(s16*)(obj + 0x1E), &normal);
    *(s16*)(obj + 0x60) = pos.vy;
    if (obj[0x36] == 0) {
        *(s32*)(*(u8**)(obj + 4) + 0x60) = pos.vy;
    }
}
#endif
/* func_800B0060.s */
extern void func_8003852C(u32 p);
extern void HeapFree(u32 p);
void func_800B0060(u8* p) {
    if (*(u32*)(p + 0xC) != 0) {
        if (p[0x63] != 0) {
            func_8003852C(*(u32*)(*(u32*)(p + 0xB4) + 8));
            p[0x63] = 0;
        }
        HeapFree(*(u32*)(p + 0xC));
        *(u32*)(p + 0xC) = 0;
        *(u32*)(p + 0x18) = 0;
    }
}
#include "main73.c"
