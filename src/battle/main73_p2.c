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
INCLUDE_ASM("asm/battle/nonmatchings/main73_p2", func_800AFD98);
INCLUDE_ASM("asm/battle/nonmatchings/main73_p2", func_800AFF9C);
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
