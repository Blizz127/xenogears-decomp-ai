#include "common.h"
#ifndef XENO_PC_PORT
s32 rand();
extern void *D_800C34B0;
extern void * D_800C3DFC;
extern void * D_800C3E00;
extern s8 D_800D2DC4;
void func_80096824(void);
#endif

#ifndef XENO_PC_PORT
#include "system/memory.h"
#endif


#ifndef XENO_PC_PORT
s8 func_80097964(u8, u8, u16);
void func_800995A0(u8, u8, u16, s32);
extern void *D_800C3DFC;
extern void *D_800C3E00;
extern u8 D_800C3E50;
void func_80095B44(void);
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_8008FE18);
extern s32 func_80076A10(s32, s32, s16, s16);
extern s32 D_800C3EA4;
extern u8 D_800CCB34;
extern u8* D_800D2D28;

void func_8009023C(s32 arg0) {
    s32 var_s0;
    s32 var_s1;

    var_s1 = 0;
    var_s0 = 0x68;
    *(s32 *)((s8*)(D_800D2D28) + 0xF8) = 0;
    do {
        *(s32 *)((s8*)(D_800D2D28) + 0xF8) = (s32) (func_80076A10(0x66, D_800C3EA4 + 0xBA8 + (*(s32 *)((s8*)(D_800D2D28) + 0xF8) * 0x50), 0x20, (s16) (var_s0 - 4 + arg0)) + *(s32 *)((s8*)(D_800D2D28) + 0xF8));
        var_s0 += 8;
    } while (++var_s1 < 9);
    *(u8 *)((s8*)(D_800D2D28) + 0xA5) = (u8) D_800CCB34;
    *(s8 *)((s8*)(D_800D2D28) + 0x9C) = 1;
}

extern void func_80076C78(u8* p, s32 x, s32 y, s32 a3, s32 a4, s32 n);
extern u16 g_SystemPalette1;
extern u16 g_SystemPalette2;
extern s32 D_800CCB34__w[] asm("D_800CCB34");

/* func_80090310.s, byte-exact: with k = b * 2 + a, for k > 16 (then
 * k -= 16, and the second sprite's fourth argument gains 0x10) set the CLUT
 * halfword +0x14E (sprite +0x140) at D_800CCB34 * 40 of the record pointed
 * to by the word at D_800C3EA4 + 0xA230 to g_SystemPalette2, otherwise to
 * g_SystemPalette1; then set up the func_80076C78 sprites at +0x140 (0x18,
 * 0x33, (u8)(k % 2 * 120), (u8)(k / 2 * 13), 0x60) and +0x190 (0x84, 0x33,
 * (u8)(k % 2 << 6 | off), (u8)(k / 2 * 13), 0x10).  CLUT store as in
 * func_8009093C (block-local `rec`, written in each branch). */
void func_80090310(u8 a, u8 b) {
    s32 k = b * 2 + a;
    s32 off = 0;
    s32 q;
    s32 r;

    if (k > 16) {
        k -= 16;
        {
            s32 base = D_800C3EA4;
            u16 pal = g_SystemPalette2;
            {
                u8* rec = *(u8**)(base + 0xA230);
                u8* p = rec + D_800CCB34__w[0] * 40;
                *(u16*)(p + 0x14E) = pal;
            }
        }
        off = 0x10;
    } else {
        {
            s32 base = D_800C3EA4;
            u16 pal = g_SystemPalette1;
            {
                u8* rec = *(u8**)(base + 0xA230);
                u8* p = rec + D_800CCB34__w[0] * 40;
                *(u16*)(p + 0x14E) = pal;
            }
        }
    }
    q = k / 2;
    r = k - q * 2;
    func_80076C78(*(u8**)(D_800C3EA4 + 0xA230) + 0x140 + D_800CCB34__w[0] * 40, 0x18, 0x33, (u8)(r * 120), (u8)(q * 13), 0x60);
    func_80076C78(*(u8**)(D_800C3EA4 + 0xA230) + 0x190 + D_800CCB34__w[0] * 40, 0x84, 0x33, (u8)((r << 6) | off), (u8)(q * 13), 0x10);
}

extern u8 D_800D2CE0[];
extern u8 D_800D2204[];
extern u8 D_800D2F68[][4];

/* func_800904A0.s, byte-exact: func_8009187C's sprite pair for the halfword
 * v of 16-byte entry D_800D2CE0[b * 2 + a] of D_800D2204, at +0x320 (0xA8,
 * 0x33) and +0x370 (0xD0, 0x33).  Its switch has no default: for
 * v & 0xF > 2 retail indexes D_800D2F68 with whatever the caller left in the
 * saved register, and the C reads the unset u8 `j`; that read is undefined
 * in a native build, which must define the case or show it unreachable. */
void func_800904A0(u8 a, u8 b) {
    u16 v;
    s32 i;
    u8 j;

    v = *(u16*)(D_800D2204 + D_800D2CE0[b * 2 + a] * 16);
    if (v & 0x4000) {
        i = 9;
    } else if (v & 0x1000) {
        i = 7;
    } else {
        i = 8;
    }
    switch (v & 0xF) {
    case 0:
        j = 0xC;
        break;
    case 1:
        j = 0xD;
        break;
    case 2:
        j = 0x15;
        break;
    }
    func_80076C78(*(u8**)(D_800C3EA4 + 0xA230) + 0x320 + D_800CCB34__w[0] * 40, 0xA8, 0x33, D_800D2F68[i][2], D_800D2F68[i][3], D_800D2F68[i][0]);
    {
        u8* rec = *(u8**)(D_800C3EA4 + 0xA230);
        u8* p = rec + D_800CCB34__w[0] * 40;
        *(u16*)(p + 0x32E) = D_800D2F68[i][1] ? g_SystemPalette2 : g_SystemPalette1;
    }
    func_80076C78(*(u8**)(D_800C3EA4 + 0xA230) + 0x370 + D_800CCB34__w[0] * 40, 0xD0, 0x33, D_800D2F68[j][2], D_800D2F68[j][3], D_800D2F68[j][0]);
    {
        u8* rec = *(u8**)(D_800C3EA4 + 0xA230);
        u8* p = rec + D_800CCB34__w[0] * 40;
        *(u16*)(p + 0x37E) = D_800D2F68[j][1] ? g_SystemPalette2 : g_SystemPalette1;
    }
}
extern u8 D_800D2CE0[];
extern u8* func_8008AC00(s32 size);
extern void bzero(void* p, s32 n);
extern s32 D_800D329C;
extern u8* GetStringEntry(s32 table, s32 id);
extern s32 SystemRenderStringEntry(u8* str, u8* buf, s32 w, s32 flags);
extern void func_800769E8(s16* rect, u8* buf);
extern s32 D_800C3EA4;
extern u8 D_800CCB34;
extern void func_80076C78(u8* p, s32 x, s32 y, s32 a3, s32 a4, s32 n);
extern u_int HeapFree(void* pMem);

/* Render string D_800D2CE0[b * 2 + a] into a 0x39-wide buffer, upload it at
 * (0x3C0, 0) and set up the text sprite. */
void func_8009070C(u8 a, u8 b) {
    s16 r[4];
    u8 id = D_800D2CE0[b * 2 + a];
    u8* buf = func_8008AC00(0x39);
    s32 n;

    bzero(buf, 0x618);
    n = SystemRenderStringEntry(GetStringEntry(D_800D329C, id), buf, 0x39, 0);
    r[0] = 0x3C0;
    r[2] = 0x3C;
    r[1] = 0;
    r[3] = 0xD;
    func_800769E8(r, buf);
    func_80076C78(*(u8**)(D_800C3EA4 + 0xA230) + 0x280 + *(s32*)&D_800CCB34 * 40, 0x30, 0x42, 0, 0, n & 0xFF);
    HeapFree(buf);
}

extern u8 D_800D2CE0[];
extern u8 D_800C3D70[];
extern s32 D_800C3EA4;
extern u8 D_800CCB34;
extern void func_80090310(u8 a, u8 b);
extern void func_8009070C(u8 a, u8 b);
extern void func_800904A0(u8 a, u8 b);

/* Show text slot (a, b): its string id comes from D_800D2CE0 (mode 0) or
 * D_800C3D70; mode 0 with a non-zero id also renders it and flags +0x66D. */
void func_8009080C(u8 a, u8 b, u8 mode) {
    u8 id;
    u8* w;

    if (mode == 0) {
        id = D_800D2CE0[b * 2 + a];
    } else {
        id = D_800C3D70[b * 2 + a];
    }
    (*(u8**)(D_800C3EA4 + 0xA230))[0x66D] = 0;
    func_80090310(a, b);
    if (id != 0 && mode == 0) {
        func_8009070C(a, b);
        func_800904A0(a, b);
        (*(u8**)(D_800C3EA4 + 0xA230))[0x66D] = 1;
    }
    (*(u8**)(D_800C3EA4 + 0xA230))[0x66A] = D_800CCB34;
    (*(u8**)(D_800C3EA4 + 0xA230))[0x66B] = 1;
}

extern void func_80076CE8(u8* p, s32 x, s32 y, s32 a3, s32 a4, s32 a5, s32 a6);
extern u16 g_SystemPalette1;
extern u16 g_SystemPalette2;
extern s32 D_800CCB34__w[] asm("D_800CCB34");

/* func_8009093C.s, byte-exact: for n >= 0x68 (then n -= 0x68, and the last
 * two sprites' fourth argument gains 0x10) set the CLUT halfwords (+0xE,
 * +0x5E) of the first two sprites at D_800CCB34 * 40 of the record pointed
 * to by the word at D_800C3EA4 + 0xA230 to g_SystemPalette2, otherwise to
 * g_SystemPalette1; then set up four func_80076CE8 sprites at +0, +0x50,
 * +0xA0 and +0xF0 with (u8)n as fifth argument and store D_800CCB34 at
 * +0x668.  Codegen: each CLUT store loads the record pointer into its own
 * block-local `rec` (retail adds it as the first operand), and both stores
 * are written in each branch, as a store-pair macro invoked per branch
 * would give (func_80091EC4 and func_8009209C open with the same pair);
 * retail's shared second store is the two copies merged. */
void func_8009093C(s32 n) {
    s32 off = 0;

    if (n >= 0x68) {
        n -= 0x68;
        {
            s32 base = D_800C3EA4;
            u16 pal = g_SystemPalette2;
            {
                u8* rec = *(u8**)(base + 0xA230);
                u8* p = rec + D_800CCB34__w[0] * 40;
                *(u16*)(p + 0xE) = pal;
            }
            {
                u8* rec = *(u8**)(base + 0xA230);
                u8* p = rec + D_800CCB34__w[0] * 40;
                *(u16*)(p + 0x5E) = pal;
            }
        }
        off = 0x10;
    } else {
        s32 base = D_800C3EA4;
        u16 pal = g_SystemPalette1;
        {
            u8* rec = *(u8**)(base + 0xA230);
            u8* p = rec + D_800CCB34__w[0] * 40;
            *(u16*)(p + 0xE) = pal;
        }
        {
            u8* rec = *(u8**)(base + 0xA230);
            u8* p = rec + D_800CCB34__w[0] * 40;
            *(u16*)(p + 0x5E) = pal;
        }
    }
    func_80076CE8(*(u8**)(D_800C3EA4 + 0xA230) + D_800CCB34__w[0] * 40, 0x30, 0x60, 0, (u8)n, 0x60, 0x68);
    func_80076CE8(*(u8**)(D_800C3EA4 + 0xA230) + 0x50 + D_800CCB34__w[0] * 40, 0xB4, 0x60, 0x78, (u8)n, 0x60, 0x68);
    func_80076CE8(*(u8**)(D_800C3EA4 + 0xA230) + 0xA0 + D_800CCB34__w[0] * 40, 0x98, 0x60, off, (u8)n, 0x10, 0x68);
    func_80076CE8(*(u8**)(D_800C3EA4 + 0xA230) + 0xF0 + D_800CCB34__w[0] * 40, 0x11C, 0x60, off | 0x40, (u8)n, 0x10, 0x68);
    (*(u8**)(D_800C3EA4 + 0xA230))[0x668] = *(u8*)D_800CCB34__w;
}

extern s32 func_80076A10(s32, s32, s16, s16);
extern s32 D_800C3EA4;
extern u8 D_800CCB34;
extern u8* D_800D2D28;

void func_80090B90(s32 x, s32 y, s32* frame, u8* timer) {
    if (++*timer >= 3) {
        if (--*frame < 0) {
            *frame = 4;
        }
        *timer = 0;
    }
    *(s32*)(D_800D2D28 + 0x100) = func_80076A10(*frame + 0xE0, D_800C3EA4 + 0x27C8, x, y);
    D_800D2D28[0xA7] = D_800CCB34;
    D_800D2D28[0x9E] = 1;
}

extern u16 D_800CCD38__rows[][0xB8] asm("D_800CCD38");

/* func_80090C44.s, byte-exact: two-digit readouts of actor a's halfwords at
 * D_800CCD38 and D_800CCD3A (0x170-byte records): for each value v, a
 * func_80076C78 sprite for the tens digit (only when v / 10 is non-zero) and
 * one for the ones digit, fourth argument (u8)(digit * 8 + 0x78), at
 * +0x460/+0x4B0 and +0x500/+0x550 + D_800CCB34 * 40 of the record pointed to
 * by the word at D_800C3EA4 + 0xA230 (x 0x104/0x10C and 0x11C/0x124,
 * y 0xC6).  Codegen: the halfwords are read through a [][0xB8] row view of
 * D_800CCD38, so the saved register holds only a * 0x170 as in retail
 * (pointer arithmetic on the u8 array keeps the whole address there). */
void func_80090C44(u8 a) {
    if (D_800CCD38__rows[a][0] / 10 != 0) {
        func_80076C78(*(u8**)(D_800C3EA4 + 0xA230) + 0x460 + *(s32*)&D_800CCB34 * 40, 0x104, 0xC6, (u8)(D_800CCD38__rows[a][0] / 10 * 8 + 0x78), 0, 8);
    }
    func_80076C78(*(u8**)(D_800C3EA4 + 0xA230) + 0x4B0 + D_800CCB34__w[0] * 40, 0x10C, 0xC6, (u8)(D_800CCD38__rows[a][0] % 10 * 8 + 0x78), 0, 8);
    if (D_800CCD38__rows[a][1] / 10 != 0) {
        func_80076C78(*(u8**)(D_800C3EA4 + 0xA230) + 0x500 + D_800CCB34__w[0] * 40, 0x11C, 0xC6, (u8)(D_800CCD38__rows[a][1] / 10 * 8 + 0x78), 0, 8);
    }
    func_80076C78(*(u8**)(D_800C3EA4 + 0xA230) + 0x550 + D_800CCB34__w[0] * 40, 0x124, 0xC6, (u8)(D_800CCD38__rows[a][1] % 10 * 8 + 0x78), 0, 8);
}

extern u8 D_800D2F7C[];
extern u8 D_800D2F80[];

/* func_80090E7C.s, byte-exact: set up the func_80076C78 sprites at +0x3C0
 * (0xA8, 0xA6) and +0x410 (0xEC, 0xC6) + D_800CCB34 * 40 from the 4-byte
 * entries D_800D2F80 and D_800D2F7C (bytes 2, 3 and 0 as arguments), set
 * each sprite's CLUT halfword (+0xE) to g_SystemPalette2 when the entry's
 * byte 1 is set, else g_SystemPalette1, then run func_80090C44(a), set up
 * func_80076A10(0x71, record + 0x5A0, 0x118, 0xD1) and store D_800CCB34 at
 * +0x66C.  CLUT stores as in func_8009093C (block-local `rec`). */
void func_80090E7C(u8 a) {
    func_80076C78(*(u8**)(D_800C3EA4 + 0xA230) + 0x3C0 + D_800CCB34__w[0] * 40, 0xA8, 0xA6, D_800D2F80[2], D_800D2F80[3], D_800D2F80[0]);
    {
        u8* rec = *(u8**)(D_800C3EA4 + 0xA230);
        u8* p = rec + D_800CCB34__w[0] * 40;
        *(u16*)(p + 0x3CE) = D_800D2F80[1] ? g_SystemPalette2 : g_SystemPalette1;
    }
    func_80076C78(*(u8**)(D_800C3EA4 + 0xA230) + 0x410 + D_800CCB34__w[0] * 40, 0xEC, 0xC6, D_800D2F7C[2], D_800D2F7C[3], D_800D2F7C[0]);
    {
        u8* rec = *(u8**)(D_800C3EA4 + 0xA230);
        u8* p = rec + D_800CCB34__w[0] * 40;
        *(u16*)(p + 0x41E) = D_800D2F7C[1] ? g_SystemPalette2 : g_SystemPalette1;
    }
    func_80090C44(a);
    func_80076A10(0x71, (s32)(*(u8**)(D_800C3EA4 + 0xA230) + 0x5A0), 0x118, 0xD1);
    (*(u8**)(D_800C3EA4 + 0xA230))[0x66C] = D_800CCB34;
}
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80091064);
/* func_80091604.s, byte-exact: func_8009023C's 8-row twin.  The running
 * count at D_800D2D28 +0xF8 starts at 0; each of 8 rows adds
 * func_80076A10(0x66, D_800C3EA4 + 0xBA8 + count * 0x50, 0x20, y + 0x38 +
 * row * 8) to it.  Then +0xA5 = D_800CCB34 and +0x9C = 1. */
void func_80091604(s32 y) {
    s32 dy;
    s32 i;

    i = 0;
    dy = 0x38;
    *(s32*)(D_800D2D28 + 0xF8) = 0;
    do {
        *(s32*)(D_800D2D28 + 0xF8) = func_80076A10(0x66, D_800C3EA4 + 0xBA8 + *(s32*)(D_800D2D28 + 0xF8) * 0x50, 0x20, (s16)(dy + y)) + *(s32*)(D_800D2D28 + 0xF8);
        dy += 8;
    } while (++i < 8);
    D_800D2D28[0xA5] = D_800CCB34;
    D_800D2D28[0x9C] = 1;
}
extern u8 D_800D2D24[];
extern u8 D_800C3CFA[];
extern void func_80076CE8(u8* p, s32 x, s32 y, s32 a3, s32 a4, s32 a5, s32 a6);
extern u32 func_8009A258(s32 a, s32 b);
extern void func_8008AAA0(u32 value);

/* func_800916D4.s, byte-exact: with k = c * 2 + b, set up the
 * func_80076CE8 sprite at +0x140 + D_800CCB34 * 40 of the record pointed to
 * by the word at D_800C3EA4 + 0xA230 (0x20, 0xA4, (u8)(k % 2 * 120),
 * (u8)(k / 2 * 16), 0x60, 0x10) and clear its +0x66E count; for an actor a
 * whose D_800D2D24 byte is 1, pass func_8009A258(a, k + 0x16) to
 * func_8008AAA0 and add one func_80076C78 sprite per D_800C3CFA entry
 * (three, 0xFF = none) at x 0x8C, 0x94, 0x9C, counting them in +0x66E.
 * gcc 2.6.x code; func_8009A258 takes int arguments here (its second one is
 * passed unmasked), and the loop's sprite address reads the record pointer
 * through the D_800C3EA4 word, not a local (retail adds it last). */
void func_800916D4(u8 b, u8 c, u8 a) {
    u32 k = c * 2 + b;
    u32 q = k / 2;
    u32 r = k - q * 2;
    s32 i;
    u16 x;

    func_80076CE8(*(u8**)(D_800C3EA4 + 0xA230) + 0x140 + *(s32*)&D_800CCB34 * 40, 0x20, 0xA4, (u8)(r * 120), (u8)(q * 16), 0x60, 0x10);
    (*(u8**)(D_800C3EA4 + 0xA230))[0x66E] = 0;
    if (D_800D2D24[a] == 1) {
        func_8008AAA0(func_8009A258(a, k + 0x16));
        for (i = 0, x = 0x8C; i < 3; i++, x += 8) {
            if (D_800C3CFA[i] != 0xFF) {
                func_80076C78(*(u8**)(D_800C3EA4 + 0xA230) + 0x190 + ((*(u8**)(D_800C3EA4 + 0xA230))[0x66E] * 2 + *(s32*)&D_800CCB34) * 40, x, 0xA6, (u8)(D_800C3CFA[i] * 8 + 0x78), 0, 8);
                (*(u8**)(D_800C3EA4 + 0xA230))[0x66E]++;
            }
        }
    }
}
extern u16 D_800CDD40__rows[][38][20] asm("D_800CDD40");
extern u16 D_800CEF10__rows[][42][20] asm("D_800CEF10");
extern u8 D_800D2F68[][4];
extern u8 D_800D32A1[];

/* func_8009187C.s, byte-exact: read the halfword v of entry c * 2 + b + 0x16
 * of actor a's D_800CDD40 row (38 40-byte entries per actor) when the
 * D_800D32A1 byte of a is clear, else of entry c * 2 + b + 0x15 of its
 * D_800CEF10 row (42 per actor); pick D_800D2F68 entry 9 (v & 0x4000), 7
 * (v & 0x1000) or 8, and entry 0xD, 0x15 or 0xC for v & 0xF = 1, 2 or other,
 * and draw each as in func_80090E7C at +0x320 (0xDA, 0xAA) and +0x370 (0xFE,
 * 0xAA).  Codegen: row views of the two tables and a [][4] view of
 * D_800D2F68 (index arithmetic on the flat arrays gives another address
 * form and unused stack slots); the switch's explicit case 0 gives retail's
 * extra "< 2" test. */
void func_8009187C(u8 a, u8 b, u8 c) {
    u16 v;
    s32 i;
    s32 j;

    if (D_800D32A1[a * 8] == 0) {
        v = D_800CDD40__rows[a][c * 2 + b + 0x16][0];
    } else {
        v = D_800CEF10__rows[a][c * 2 + b + 0x15][0];
    }
    if (v & 0x4000) {
        i = 9;
    } else if (v & 0x1000) {
        i = 7;
    } else {
        i = 8;
    }
    switch (v & 0xF) {
    case 0:
        j = 0xC;
        break;
    case 1:
        j = 0xD;
        break;
    case 2:
        j = 0x15;
        break;
    default:
        j = 0xC;
        break;
    }
    func_80076C78(*(u8**)(D_800C3EA4 + 0xA230) + 0x320 + D_800CCB34__w[0] * 40, 0xDA, 0xAA, D_800D2F68[i][2], D_800D2F68[i][3], D_800D2F68[i][0]);
    {
        u8* rec = *(u8**)(D_800C3EA4 + 0xA230);
        u8* p = rec + D_800CCB34__w[0] * 40;
        *(u16*)(p + 0x32E) = D_800D2F68[i][1] ? g_SystemPalette2 : g_SystemPalette1;
    }
    func_80076C78(*(u8**)(D_800C3EA4 + 0xA230) + 0x370 + D_800CCB34__w[0] * 40, 0xFE, 0xAA, D_800D2F68[j][2], D_800D2F68[j][3], D_800D2F68[j][0]);
    {
        u8* rec = *(u8**)(D_800C3EA4 + 0xA230);
        u8* p = rec + D_800CCB34__w[0] * 40;
        *(u16*)(p + 0x37E) = D_800D2F68[j][1] ? g_SystemPalette2 : g_SystemPalette1;
    }
}
extern u8 g_GameState[];
extern u8 D_800D2D24[];
extern u8 D_800D32A1[];
extern s32 D_800D367C;
extern s32 D_800CCB34__w[] asm("D_800CCB34");

/* func_80091B38.s, byte-exact: func_8009382C's two-line string sprite for
 * table D_800D367C at x 0x30, with id = D_800D2D24[a] * 32 + (c * 2 + b) * 2
 * (D_800D32A1 byte of a clear) or g_GameState[0x30C + D_800D2D24[a] * 0xA4]
 * * 32 + (c * 2 + b) * 2.  Same codegen as func_8009382C (u32 `str` copy of
 * the u16 id after bzero, word view of D_800CCB34); first draft. */
void func_80091B38(u8 a, u8 b, u8 c) {
    s16 r[4];
    u16 id;
    u8* buf;
    s32 n1;
    s32 n2;
    u32 str;

    if (D_800D32A1[a * 8] == 0) {
        id = D_800D2D24[a] * 32 + (c * 2 + b) * 2;
    } else {
        id = g_GameState[0x30C + D_800D2D24[a] * 0xA4] * 32 + (c * 2 + b) * 2;
    }
    buf = func_8008AC00(0x39);
    bzero(buf, 0x618);
    str = id;
    n1 = SystemRenderStringEntry(GetStringEntry(D_800D367C, str), buf, 0x39, 0);
    n2 = SystemRenderStringEntry(GetStringEntry(D_800D367C, str | 1), buf, 0x39, 1);
    r[0] = 0x3C0;
    r[2] = 0x3C;
    r[1] = 0;
    r[3] = 0xD;
    func_800769E8(r, buf);
    func_80076C78(*(u8**)(D_800C3EA4 + 0xA230) + 0x280 + D_800CCB34__w[0] * 40, 0x30, 0xB6, 0, 0, n1 & 0xFF);
    func_80076C78(*(u8**)(D_800C3EA4 + 0xA230) + 0x2D0 + D_800CCB34__w[0] * 40, 0x30, 0xC6, 0, 0, n2 & 0xFF);
    HeapFree(buf);
}
extern u8 g_GameState[];
extern u8 D_800D2D24[];
extern u8 D_800D32A1[];
extern u16 func_80089C6C(u16 bits, u8 b);
extern void func_800916D4(u8 b, u8 c, u8 a);
extern void func_8009187C(u8 a, u8 b, u8 c);
extern void func_80091B38(u8 a, u8 b, u8 c);

/* func_80091D38.s, byte-exact: when func_80089C6C accepts the halfword at
 * g_GameState + 0x16C2 (D_800D32A1 byte of a clear) or + 0x16C6 (set) +
 * D_800D2D24[a] * 32 for b + c * 2, run func_800916D4(b, c, a),
 * func_8009187C(a, b, c) and func_80091B38(a, b, c) and set bytes +0x66A
 * (D_800CCB34) and +0x66B (1) of the record pointed to by the word at
 * D_800C3EA4 + 0xA230; otherwise clear its +0x66B.  First draft. */
void func_80091D38(u8 a, u8 b, u8 c) {
    u8 ok = 0;

    if (D_800D32A1[a * 8] == 0) {
        ok = func_80089C6C(*(u16*)(g_GameState + 0x16C2 + D_800D2D24[a] * 32), b + c * 2) != 0;
    } else if (func_80089C6C(*(u16*)(g_GameState + 0x16C6 + D_800D2D24[a] * 32), b + c * 2) != 0) {
        ok = 1;
    }
    if (ok) {
        func_800916D4(b, c, a);
        func_8009187C(a, b, c);
        func_80091B38(a, b, c);
        (*(u8**)(D_800C3EA4 + 0xA230))[0x66A] = D_800CCB34;
        (*(u8**)(D_800C3EA4 + 0xA230))[0x66B] = 1;
    } else {
        (*(u8**)(D_800C3EA4 + 0xA230))[0x66B] = 0;
    }
}

/* func_80091EC4.s, byte-exact: set the CLUT halfwords (+0xE, +0x5E) of the
 * first two sprites at D_800CCB34 * 40 of the record pointed to by the word
 * at D_800C3EA4 + 0xA230 to g_SystemPalette1, set up four func_80076CE8
 * sprites at +0, +0x50, +0xA0 and +0xF0 (y 0x34, last argument 0x60) with
 * a as fifth argument and store D_800CCB34 at +0x668.  CLUT stores as in
 * func_8009093C. */
void func_80091EC4(u8 a) {
    s32 base = D_800C3EA4;
    u16 pal = g_SystemPalette1;

    {
        u8* rec = *(u8**)(base + 0xA230);
        u8* p = rec + D_800CCB34__w[0] * 40;
        *(u16*)(p + 0xE) = pal;
    }
    {
        u8* rec = *(u8**)(base + 0xA230);
        u8* p = rec + D_800CCB34__w[0] * 40;
        *(u16*)(p + 0x5E) = pal;
    }
    func_80076CE8(*(u8**)(base + 0xA230) + D_800CCB34__w[0] * 40, 0x34, 0x34, 0, a, 0x60, 0x60);
    func_80076CE8(*(u8**)(D_800C3EA4 + 0xA230) + 0x50 + D_800CCB34__w[0] * 40, 0xB8, 0x34, 0x78, a, 0x60, 0x60);
    func_80076CE8(*(u8**)(D_800C3EA4 + 0xA230) + 0xA0 + D_800CCB34__w[0] * 40, 0x9C, 0x34, 0, a, 0x10, 0x60);
    func_80076CE8(*(u8**)(D_800C3EA4 + 0xA230) + 0xF0 + D_800CCB34__w[0] * 40, 0x120, 0x34, 0x40, a, 0x10, 0x60);
    (*(u8**)(D_800C3EA4 + 0xA230))[0x668] = *(u8*)D_800CCB34__w;
}

/* func_8009209C.s, byte-exact: func_80091EC4's twin at y 0x62 with last
 * argument 0x40 (x 0x36, 0xC2, 0x96 and 0x122, fifth argument 0), then
 * +0x669 = 1 and D_800D2D28[0xB7] = 3. */
void func_8009209C(void) {
    s32 base = D_800C3EA4;
    u16 pal = g_SystemPalette1;

    {
        u8* rec = *(u8**)(base + 0xA230);
        u8* p = rec + D_800CCB34__w[0] * 40;
        *(u16*)(p + 0xE) = pal;
    }
    {
        u8* rec = *(u8**)(base + 0xA230);
        u8* p = rec + D_800CCB34__w[0] * 40;
        *(u16*)(p + 0x5E) = pal;
    }
    func_80076CE8(*(u8**)(base + 0xA230) + D_800CCB34__w[0] * 40, 0x36, 0x62, 0, 0, 0x60, 0x40);
    func_80076CE8(*(u8**)(D_800C3EA4 + 0xA230) + 0x50 + D_800CCB34__w[0] * 40, 0xC2, 0x62, 0x78, 0, 0x60, 0x40);
    func_80076CE8(*(u8**)(D_800C3EA4 + 0xA230) + 0xA0 + D_800CCB34__w[0] * 40, 0x96, 0x62, 0, 0, 0x10, 0x40);
    func_80076CE8(*(u8**)(D_800C3EA4 + 0xA230) + 0xF0 + D_800CCB34__w[0] * 40, 0x122, 0x62, 0x40, 0, 0x10, 0x40);
    (*(u8**)(D_800C3EA4 + 0xA230))[0x668] = *(u8*)D_800CCB34__w;
    (*(u8**)(D_800C3EA4 + 0xA230))[0x669] = 1;
    D_800D2D28[0xB7] = 3;
}
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80092298);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80092784);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80092B74);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_800930AC);

/* func_80093578.s, byte-exact: set up the func_80076CE8 sprite at +0x140 +
 * D_800CCB34 * 40 (0x7C, 0xA4, 0, (u8)(b * 16), 0x60, 0x10), then
 * func_8009187C's sprite pair for entry b + 0x25 of actor a's D_800CEF10 row
 * at +0x320 (0x24, 0xA6) and +0x370 (0x48, 0xA6). */
void func_80093578(u8 a, u8 b) {
    u16 v;
    s32 i;
    s32 j;

    func_80076CE8(*(u8**)(D_800C3EA4 + 0xA230) + 0x140 + D_800CCB34__w[0] * 40, 0x7C, 0xA4, 0, (u8)(b * 16), 0x60, 0x10);
    v = D_800CEF10__rows[a][b + 0x25][0];
    if (v & 0x4000) {
        i = 9;
    } else if (v & 0x1000) {
        i = 7;
    } else {
        i = 8;
    }
    switch (v & 0xF) {
    case 0:
        j = 0xC;
        break;
    case 1:
        j = 0xD;
        break;
    case 2:
        j = 0x15;
        break;
    default:
        j = 0xC;
        break;
    }
    func_80076C78(*(u8**)(D_800C3EA4 + 0xA230) + 0x320 + D_800CCB34__w[0] * 40, 0x24, 0xA6, D_800D2F68[i][2], D_800D2F68[i][3], D_800D2F68[i][0]);
    {
        u8* rec = *(u8**)(D_800C3EA4 + 0xA230);
        u8* p = rec + D_800CCB34__w[0] * 40;
        *(u16*)(p + 0x32E) = D_800D2F68[i][1] ? g_SystemPalette2 : g_SystemPalette1;
    }
    func_80076C78(*(u8**)(D_800C3EA4 + 0xA230) + 0x370 + D_800CCB34__w[0] * 40, 0x48, 0xA6, D_800D2F68[j][2], D_800D2F68[j][3], D_800D2F68[j][0]);
    {
        u8* rec = *(u8**)(D_800C3EA4 + 0xA230);
        u8* p = rec + D_800CCB34__w[0] * 40;
        *(u16*)(p + 0x37E) = D_800D2F68[j][1] ? g_SystemPalette2 : g_SystemPalette1;
    }
}
extern u8 D_800CCD88[];
extern s32 D_800C3DE8;
extern s32 D_800CCB34__w[] asm("D_800CCB34");

/* func_8009382C.s, byte-exact: render strings id and id | 1 of table
 * D_800C3DE8 (id = (D_800CCD88[a * 0x170] * 4 + b) * 2) into a 0x39-wide
 * buffer, upload it at (0x3C0, 0) and set up two text sprites at
 * +0x280 / +0x2D0 + D_800CCB34 * 40 of the record pointed to by the word at
 * D_800C3EA4 + 0xA230 (func_8009070C's shape with two lines).  Codegen:
 * D_800CCB34 is read as a word through an unsized-array view (retail keeps
 * its address in a saved register across the first func_80076C78 call);
 * the u16 id is copied to a u32 `str` after bzero (retail's late
 * andi 0xFFFF; a decomp-permuter hint), and GetStringEntry takes an s32
 * index, as its definition does. */
void func_8009382C(u8 a, u8 b) {
    s16 r[4];
    u16 id = (D_800CCD88[a * 0x170] * 4 + b) * 2;
    u8* buf = func_8008AC00(0x39);
    s32 n1;
    s32 n2;
    u32 str;

    bzero(buf, 0x618);
    str = id;
    n1 = SystemRenderStringEntry(GetStringEntry(D_800C3DE8, str), buf, 0x39, 0);
    n2 = SystemRenderStringEntry(GetStringEntry(D_800C3DE8, str | 1), buf, 0x39, 1);
    r[0] = 0x3C0;
    r[2] = 0x3C;
    r[1] = 0;
    r[3] = 0xD;
    func_800769E8(r, buf);
    func_80076C78(*(u8**)(D_800C3EA4 + 0xA230) + 0x280 + D_800CCB34__w[0] * 40, 0x88, 0xB6, 0, 0, n1 & 0xFF);
    func_80076C78(*(u8**)(D_800C3EA4 + 0xA230) + 0x2D0 + D_800CCB34__w[0] * 40, 0x88, 0xC6, 0, 0, n2 & 0xFF);
    HeapFree(buf);
}
extern u16 D_800C324E;
extern u16 D_800C3250;
extern u16 D_800C3252;
extern u16 D_800C323A;
extern u8 g_GameState[];
extern u8 D_800D2D24[];
extern u8 D_800CCD62[];
extern u16 func_80089C6C(u16 bits, u8 b);
extern void func_80093578(u8 a, u8 b);
extern void func_8009382C(u8 a, u8 b);

/* func_800939CC.s, byte-exact: when func_80089C6C accepts the halfword at
 * g_GameState + 0x16DA + D_800D2D24[a] * 32 for b and actor a's D_800CCD62
 * halfword (0x170-byte records) has none of the bits of mask b
 * (D_800C324E, D_800C3250, D_800C3252, D_800C323A), run
 * func_80093578(a, b) and func_8009382C(a, b) and set bytes +0x66A
 * (D_800CCB34) and +0x66B (1) of the record pointed to by the word at
 * D_800C3EA4 + 0xA230; otherwise clear its +0x66B.  gcc 2.6.x code (main51
 * is BattleGcc263); first draft. */
void func_800939CC(u8 a, u8 b) {
    u16 masks[4];

    masks[0] = D_800C324E;
    masks[1] = D_800C3250;
    masks[2] = D_800C3252;
    masks[3] = D_800C323A;
    if (func_80089C6C(*(u16*)(g_GameState + 0x16DA + D_800D2D24[a] * 32), b) != 0
        && !(*(u16*)(D_800CCD62 + a * 0x170) & masks[b])) {
        func_80093578(a, b);
        func_8009382C(a, b);
        (*(u8**)(D_800C3EA4 + 0xA230))[0x66A] = D_800CCB34;
        (*(u8**)(D_800C3EA4 + 0xA230))[0x66B] = 1;
    } else {
        (*(u8**)(D_800C3EA4 + 0xA230))[0x66B] = 0;
    }
}
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80093B08);
#endif

extern void func_8007FB70(s32);
#ifndef XENO_PC_PORT
extern u8 D_800D2D24[];
extern u8* D_800D2D28;
#endif

void func_8009413C(u8 idx, u8 flag) {
    if (D_800D2D24[idx] == 4) {
        D_800D2D28[0xB7] = 0;
        D_800D2D28[0xB0] = 0;
        if (flag) {
            func_8007FB70(idx);
        }
    }
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_800941A4);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_800946F4);
#endif
typedef struct { u8 pad[0x15A]; u8 flags; u8 pad2[0x170 - 0x15B]; } Actor170;
typedef struct {
    Actor170 actors[0x42];
    u8 pad[0x5F54 - 0x42 * 0x170];
    s32 f5F54[3];
    s32 f5F60[3];
    s32 f5F6C[8];
    u8 pad2[0x5FA0 - 0x5F8C];
    u8 f5FA0[8];
} Battle;
void func_80094C78(void) {
    u8 i;
    Battle* b = D_800C34B0;
    u32 cur = D_800C3E50;

    for (i = 0; i < 3; i++) {
        if (b->f5FA0[i] == 0) {
            if (b->actors[i].flags & 0x80) {
                b->f5F60[i] += b->f5F6C[cur];
            } else {
                b->f5F54[i] += b->f5F6C[cur];
            }
        }
    }
}

#ifndef XENO_PC_PORT
extern u16 D_800C3AA4[];

void func_80094D24(void) {
    u8 i;
    u8 cnt1;
    u8 cnt2;
    u16 f;
    u8 *p;

    cnt1 = 0;
    for (i = 0; i < 3; i++) {
        if (*(u16 *)((u8 *)((s32)D_800C34B0 - (-(i * 0x170))) + 0x7C) & 0xC000) {
            cnt1++;
        }
    }
    cnt2 = 0;
    for (i = 0; i < 3; i++) {
        if (*(u16 *)((u8 *)((s32)D_800C34B0 - (-(i * 0x170))) + 0x7C) & 2) {
            cnt2++;
        }
    }
    if (cnt1 == 1 && cnt2 == 2) {
        for (i = 0; i < 3; i++) {
            p = (u8 *)((s32)D_800C34B0 - (-(i * 0x170)));
            f = ((u16 *)p)[0x3E];
            if (f & 2) {
                ((u16 *)p)[0x3E] = f & 0xFFFD;
                ((u16 *)p)[0x3D] = D_800C3AA4[i];
            }
        }
    }
    if (cnt1 == 2 && cnt2 == 1) {
        for (i = 0; i < 3; i++) {
            p = (u8 *)((s32)D_800C34B0 - (-(i * 0x170)));
            f = ((u16 *)p)[0x3E];
            if (f & 2) {
                ((u16 *)p)[0x3E] = f & 0xFFFD;
                ((u16 *)p)[0x3D] = D_800C3AA4[i];
            }
        }
    }
}

INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80094EE4);
extern u8* D_800C3E34;
extern u8 D_800CCE42[];

void func_80095690(void) {
    s16 temp_lo;
    s16 var_a1;
    u16 temp_a2;

    temp_lo = *(u8 *)((s8*)(D_800C3E00) + 0x5B) * *(u8 *)((s8*)(D_800C3DFC) + 0x11);
    var_a1 = temp_lo;
    if (*(u16 *)((s8*)(D_800C3E00) + 0x8A) & 0x2000) {
        var_a1 = temp_lo * 2;
    }
    temp_a2 = *(u16 *)((s8*)(D_800C3E34) + 0x8C) | *(u16 *)((s8*)(D_800C3E34) + 0x8E);
    if (temp_a2 & 0x100) {
        var_a1 = (var_a1 * 7) / 10;
    }
    if (temp_a2 & 0x200) {
        var_a1 = (var_a1 * 0xD) / 10;
    }
    if (D_800CCE42[D_800C3E50 * 0x170] & 0x80) {
        var_a1 = 0;
    }
    *(s8 *)((s8*)((D_800C34B0 + D_800C3E50)) + 0x5FA0) = 2;
    *(s32 *)((s8*)(((D_800C3E50 * 4) + D_800C34B0)) + 0x5F6C) = (s32) var_a1;
}

#endif
#ifndef XENO_PC_PORT
extern u8 D_800CCD3E[];
extern u8* D_800C3E34;
extern u8 D_800D2C88[];
extern s32 D_800D2C54[];
extern u16 D_800D2C9E;
#endif
extern void func_8009AC48(u8 actor, s32 a1);

typedef struct {
    u8 pad0[0x4E];
    u16 unk4E;
    u8 pad50[0x2C];
    s16 unk7C;
    s16 pad7E;
    s16 unk80;
    s16 pad82;
    s16 unk84;
    s16 pad86;
    s16 unk88;
    s16 pad8A;
    s16 unk8C;
} BattleActorC34;

void func_800957D8(void) {
    u8 i;

    if (D_800CCD3E[D_800C3E50 * 0x170] == 2) {
        func_8009AC48(D_800C3E50, 1);
    }
    ((BattleActorC34*)D_800C3E34)->unk7C = 0;
    ((BattleActorC34*)D_800C3E34)->unk80 = 0;
    ((BattleActorC34*)D_800C3E34)->unk84 = 0;
    ((BattleActorC34*)D_800C3E34)->unk88 = 0;
    ((BattleActorC34*)D_800C3E34)->unk8C = 0;
    D_800D2C88[D_800C3E50] = 2;
    i = D_800C3E50;
    *(s32*)((u8*)D_800D2C54 + i * 4) = ((BattleActorC34*)D_800C3E34)->unk4E * ((u8*)D_800C3DFC)[0x11] / 10;
    D_800D2C9E |= 1 << i;
}

#ifndef XENO_PC_PORT
/* func_800958D8.s, byte-exact: when rand() % 100 is at most byte +0x1C of
 * D_800C3DFC and its byte +0x1D is 0x6E, clear each halfword +0x84 ... +0x8E
 * of D_800C3E34 whose bit (0x8000 down to 0x400) is set in D_800C3DFC's
 * halfword +0x1E, and set byte +0x5FC7 of D_800C34B0 to 0x3A. */
void func_800958D8(void) {
    if (rand() % 100 <= *(u8*)((s8*)D_800C3DFC + 0x1C) && *(u8*)((s8*)D_800C3DFC + 0x1D) == 0x6E) {
        if (*(u16*)((s8*)D_800C3DFC + 0x1E) & 0x8000) {
            *(s16*)((s8*)D_800C3E34 + 0x84) = 0;
        }
        if (*(u16*)((s8*)D_800C3DFC + 0x1E) & 0x4000) {
            *(s16*)((s8*)D_800C3E34 + 0x86) = 0;
        }
        if (*(u16*)((s8*)D_800C3DFC + 0x1E) & 0x2000) {
            *(s16*)((s8*)D_800C3E34 + 0x88) = 0;
        }
        if (*(u16*)((s8*)D_800C3DFC + 0x1E) & 0x1000) {
            *(s16*)((s8*)D_800C3E34 + 0x8A) = 0;
        }
        if (*(u16*)((s8*)D_800C3DFC + 0x1E) & 0x800) {
            *(s16*)((s8*)D_800C3E34 + 0x8C) = 0;
        }
        if (*(u16*)((s8*)D_800C3DFC + 0x1E) & 0x400) {
            *(s16*)((s8*)D_800C3E34 + 0x8E) = 0;
        }
        ((u8*)D_800C34B0)[0x5FC7] = 0x3A;
    }
}
void func_80095A78(void) {
    u8* act = D_800C3DFC;
    s8 r = func_80097964(act[0x1C], act[0x1D], *(u16*)(act + 0x1E));

    if (*(u16*)((u8*)D_800C3DFC + 0xA) & 0x4000) {
        func_800995A0(D_800C3E50, ((u8*)D_800C3DFC)[0x1D], *(u16*)((u8*)D_800C3DFC + 0x1E), 5);
    } else {
        func_800995A0(D_800C3E50, ((u8*)D_800C3DFC)[0x1D], *(u16*)((u8*)D_800C3DFC + 0x1E), ((u8*)D_800C3DFC)[0x11]);
        if (r != 1) {
            u8* b = D_800C34B0;

            (b + D_800C3E50)[0x5FA0] = 6;
        }
    }
}


#ifndef XENO_PC_PORT
void func_80095B44(void) {
    if (func_80097964(*(u8 *)((s8*)(D_800C3E00) + 2), *(u8 *)((s8*)(D_800C3E00) + 3), *(u16 *)((s8*)(D_800C3E00) + 0)) == 1) {
        func_800995A0(D_800C3E50, *(u8 *)((s8*)(D_800C3DFC) + 0x1D), *(u16 *)((s8*)(D_800C3DFC) + 0x1E), 5);
    }
}
#endif

#ifndef XENO_PC_PORT

void func_80095BAC(void) {
    u8 temp_s0;

    temp_s0 = *(u8 *)((s8*)(D_800C3DFC) + 0x1C);
    if ((s32) temp_s0 < (rand() % 100)) {
        u8* b = D_800C34B0;

        (b + D_800C3E50)[0x5FA0] = 6;
        return;
    }
    if (*(u8 *)((s8*)(D_800C3DFC) + 0x1D) & 0x80) {
        *(u16 *)((s8*)(D_800C3E34) + 0x7C) = (u16) (*(u16 *)((s8*)(D_800C3E34) + 0x7C) & 0xC000);
    }
    if (*(u8 *)((s8*)(D_800C3DFC) + 0x1D) & 0x40) {
        *(s16 *)((s8*)(D_800C3E34) + 0x80) = 0;
        *(u16 *)((s8*)(D_800C3E34) + 0x7A) = (u16) (*(u16 *)((s8*)(D_800C3E34) + 0x7A) & 0xFFDF);
    }
    if (*(u8 *)((s8*)(D_800C3DFC) + 0x1D) & 0x20) {
        *(s16 *)((s8*)(D_800C3E34) + 0x84) = 0;
    }
    if (*(u8 *)((s8*)(D_800C3DFC) + 0x1D) & 0x10) {
        *(s16 *)((s8*)(D_800C3E34) + 0x88) = 0;
    }
    if (*(u8 *)((s8*)(D_800C3DFC) + 0x1D) & 8) {
        *(s16 *)((s8*)(D_800C3E34) + 0x8C) = 0;
    }
}
#endif

INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80095D4C);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80096018);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80096494);

#ifndef XENO_PC_PORT
void func_80096824(void) {
    s32 temp_s0;

    temp_s0 = *(u8 *)((s8*)(D_800C3E00) + 0x5B) + *(u8 *)((s8*)(D_800C3DFC) + 0x14);
    if ((rand() % 100) >= temp_s0) {
        *(s8 *)((s8*)(D_800C34B0) + 0x5FC7) = 0x38;
        D_800D2DC4 = 1;
    }
}
#endif

#ifndef XENO_PC_PORT
extern u8 D_800C3E04;

void func_800968C0(void) {
    s32 var_s0;
    u16 temp_v1;
    u16 temp_v1_2;
    u8 temp_a1;
    void *temp_v0;

    if (!(*(u16 *)((s8*)(D_800C3E34) + 0x7C) & 0xA000)) {
        temp_v1 = *(u16 *)((s8*)(D_800C3E34) + 0x80);
        if (!(temp_v1 & 0x1000) && (((u8) D_800C3E04 >= 3U) || (temp_v1 & 0x2000)) && (*(u16 *)((s8*)(D_800C3E34) + 0x88) & 0x1000)) {
            temp_v1_2 = *(u16 *)((s8*)(D_800C3DFC) + 0xA);
            if (!(temp_v1_2 & 0x2000) && !(temp_v1_2 & 0x100) && !(((Battle *)D_800C34B0)->actors[D_800C3E04].flags & 0x80)) {
                var_s0 = 0x32;
                if (*(u8 *)((s8*)(D_800C3E34) + 0x56) == 0) {
                    var_s0 = 0x3C;
                }
                if ((var_s0 >= (rand() % 100)) && (*(u8 *)((s8*)(D_800C3DFC) + 0x16) != 2)) {
                    temp_a1 = D_800C3E04;
                    D_800C3E04 = D_800C3E50;
                    temp_v0 = D_800C3E00;
                    D_800C3E00 = D_800C3E34;
                    D_800C3E50 = temp_a1;
                    D_800C3E34 = temp_v0;
                    D_800C3DFC = (D_800C3E04 * 0x5F0) + D_800C34B0 + 0x1378;
                    *(s8 *)((s8*)((D_800C34B0 + D_800C3E04)) + 0x5FA0) = 7;
                    *(s32 *)((s8*)(((D_800C3E04 * 4) + D_800C34B0)) + 0x5F6C) = 0;
                    *(s8 *)((s8*)(D_800C34B0) + 0x5FC7) = 0x34;
                }
            }
        }
    }
}
#endif

INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80096AB8);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80096FBC);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80097610);
#endif


#ifndef XENO_PC_PORT
extern u8* D_800D2D28;
#endif
#ifndef XENO_PC_PORT
extern u8* D_800D2DC8;
#endif


/* func_8009795C.s */
void func_8009795C(void) {
}


/* ==== fo/bat2 port bodies, part A (func_8008FE18..func_80093578) ==== */
/* ==== end part A ==== */

/* ==== fo/bat2 port bodies, part B (func_8009382C..func_80097610) ==== */
/* ==== end part B ==== */
