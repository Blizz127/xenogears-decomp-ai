/* Retail TU part 2 of main40.c (see the BATTLE_TU_PART note there): the
 * still-assembly run below, then that file's part-2 C bodies. */
#define BATTLE_TU_PART 2
#include "common.h"
#ifndef XENO_PC_PORT
u32 func_80076A10(u32 sel, u8 *p, s32 m, s32 n);
u32 func_80089C6C(u32 mask, u8 index);
extern u8 D_800CCB34;
extern u16 D_800D2C30;
extern u8 * D_800D2DB4;
void func_80089038(void);
#endif

#ifndef XENO_PC_PORT
extern u8* D_800D2D28;
extern u8 D_800C207C;
/* D_800C2054 is read as two rows of five words: func_8008887C(x0, y0, x1, y1)
 * gets a random target x from row 0 and a random target y from row 1.
 * Indexing row 1 of this 2-D view gives retail's base-first add for `pb`;
 * widening `r` where it is assigned and loading the row-0 base once into
 * `tbl` give retail's andi/la/sll order for `pa`. */
extern s32 D_800C2054__2d[2][5] asm("D_800C2054");
extern s32 D_800C2080;
extern s32 D_800C2084;
extern s32 D_800C3A7C;
extern s32 D_800C3A80;
extern s32 D_800CCB34__w asm("D_800CCB34");
extern u8 D_800CCB34__b[] asm("D_800CCB34");
extern u8 func_8001BD40(u8 min, u8 max);
extern void func_8008887C(u16 x, u16 y, s32 a2, s32 a3);
extern void func_80088990(void);
extern void func_80076B68(u8* p);
extern void func_80076BF0(u8* p);

void func_80088B80(void) {
    s32 i;
    s32 j;
    s32 k;
    s32* pa;
    s32* pb;

    if (D_800D2D28[0xAD] == 0) {
        return;
    }
    if (D_800C207C != 0) {
        if (func_8001BD40(0, 0x63) >= 0x60) {
            s32 r = func_8001BD40(0, 4);
            s32* tbl = D_800C2054__2d[0];

            pa = &tbl[r];
            pb = &D_800C2054__2d[1][func_8001BD40(0, 4)];
            func_8008887C(*(u16*)(D_800D2DB4 + 0x5D9C), *(u16*)(D_800D2DB4 + 0x5D9E), *pa, *pb);
        }
        if (D_800C207C != 0) {
            goto draw;
        }
    }
    func_80088990();
    ((u16*)D_800D2DB4)[0x5D9C / 2] = D_800C3A7C + D_800C2080 / 256;
    ((u16*)D_800D2DB4)[0x5D9E / 2] = D_800C3A80 + D_800C2084 / 256;
draw:
    D_800D2DB4[0x5D72] =
        func_80076A10(0xB9, D_800D2DB4 + 0xD20, *(s16*)(D_800D2DB4 + 0x5D9C), *(s16*)(D_800D2DB4 + 0x5D9E));
    D_800D2DB4[0x5D94] = D_800CCB34__b[0];
    D_800D2DB4[0x5D71] = func_80076A10((*(u16*)(D_800D2DB4 + 0x5D9C) & 0xF) + 0xA9, D_800D2DB4 + 0x3C0, 0xA0, 0x64);
    D_800D2DB4[0x5D93] = D_800CCB34__b[0];
    D_800D2DB4[0x5D73] =
        func_80076A10(0x82, D_800D2DB4 + 0xFA0, *(s16*)(D_800D2DB4 + 0x5D9C), *(s16*)(D_800D2DB4 + 0x5D9E));
    D_800D2DB4[0x5D95] = D_800CCB34__b[0];
    for (i = 0; i < D_800D2DB4[0x5D72]; i++) {
        func_80076B68(D_800D2DB4 + 0xD20 + (i * 2 + D_800D2DB4[0x5D94]) * 0x28);
    }
    for (i = 0; i < D_800D2DB4[0x5D73]; i++) {
        func_80076B68(D_800D2DB4 + 0xFA0 + (i * 2 + D_800D2DB4[0x5D95]) * 0x28);
    }
    for (i = 0; i < D_800D2DB4[0x5D71]; i++) {
        func_80076B68(D_800D2DB4 + 0x3C0 + (i * 2 + D_800D2DB4[0x5D93]) * 0x28);
    }
    D_800D2DB4[0x5D96] = func_80076A10((*(u16*)(D_800D2DB4 + 0x5D9E) & 0xF) + 0xC9, D_800D2DB4 + 0x32A0,
                                       *(s16*)(D_800D2DB4 + 0x5D9C), *(s16*)(D_800D2DB4 + 0x5D9E));
    D_800D2DB4[0x5D97] = D_800CCB34;
    for (i = 0; i < D_800D2DB4[0x5D96]; i++) {
        func_80076BF0(D_800D2DB4 + 0x32A0 + (i * 2 + D_800D2DB4[0x5D97]) * 0x28);
    }
    if (D_800C207C == 0) {
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 10; j++) {
                k = i * 10 + j;
                func_80076A10(func_8001BD40(0, 9) + 0xBA, D_800D2DB4 + 0x46A0 + k * 0x50, (s16)(0x82 + j * 6),
                              (s16)(0xA + i * 0xBD));
                func_80076B68(D_800D2DB4 + 0x46A0 + (k * 2 + D_800CCB34__w) * 0x28);
            }
        }
        D_800D2DB4[0x5D98] = D_800CCB34;
    }
}

#ifndef XENO_PC_PORT
void func_80089038(void) {
    s32 temp_v0;
    s32 var_s0;
    s32 var_s1;

    var_s0 = 0;
    var_s1 = 0x6E0000;
    *(u8 *)((s8*)((*(void **)&D_800D2DB4)) + 0x5DA1) = 0U;
    do {
        if (func_80089C6C(D_800D2C30, var_s0 & 0xFF) & 0xFFFF) {
            temp_v0 = func_80076A10(var_s0 + 0xC4, (*(u8 *)((s8*)((*(void **)&D_800D2DB4)) + 0x5DA1) * 0x50) + 0x4CE0 + (*(void **)&D_800D2DB4), 0xE0, var_s1 >> 0x10);
            var_s1 += 0xA0000;
            *(u8 *)((s8*)((*(void **)&D_800D2DB4)) + 0x5DA1) = (u8) (*(u8 *)((s8*)((*(void **)&D_800D2DB4)) + 0x5DA1) + temp_v0);
        }
        var_s0 += 1;
    } while (var_s0 < 5);
    *(u8 *)((s8*)((*(void **)&D_800D2DB4)) + 0x5DA0) = (u8) D_800CCB34;
    *(s16 *)((s8*)((*(void **)&D_800D2DB4)) + 0x5DA2) = 0;
}
#endif

extern u8 D_800D2C38;

void func_80089110(void) {
    u8 pad[1];
    s32 i;
    s32 id = 0xA0;

    if (D_800D2C38 != 0) {
        id = 0xA1;
    }
    D_800D2DB4[0x5D75] = func_80076A10(id, D_800D2DB4 + 0x3AC0, 0xA0, 0x64);
    D_800D2DB4[0x5D84] = D_800CCB34;
    for (i = 0; i < D_800D2DB4[0x5D75]; i++) {
        func_80076B68(D_800D2DB4 + 0x3AC0 + (i * 2 + D_800D2DB4[0x5D84]) * 0x28);
    }
}

INCLUDE_ASM("asm/battle/nonmatchings/main40_p2", func_800891E4);
extern u16 D_800D2C2A;
extern void func_8008AAA0(u32 value);
extern u8 D_800C3CF8[];

/* Lay out the (up to five) D_800C3CF8 item names from y = 0x11A in steps of
 * 6 (kept as y << 16), counting sprites in +0x5D78, then register those sprites. */
void func_80089348(void) {
    u8 pad[1];
    s32 i;
    s32 y;

    func_8008AAA0(D_800D2C2A);
    for (i = 0, y = 0x11A << 16; i < 5; i++) {
        if (D_800C3CF8[i] != 0xFF) {
            D_800D2DB4[0x5D78] += func_80076A10(D_800C3CF8[i] + 0x92, D_800D2DB4 + 0x4E70 + D_800D2DB4[0x5D78] * 0x50, y >> 16, 0x46);
            y += 6 << 16;
        }
    }
    D_800D2DB4[0x5D87] = D_800CCB34;
    for (i = 0; i < D_800D2DB4[0x5D78]; i++) {
        func_80076B68(D_800D2DB4 + 0x4E70 + (i * 2 + D_800D2DB4[0x5D87]) * 0x28);
    }
}

INCLUDE_ASM("asm/battle/nonmatchings/main40_p2", func_8008946C);
INCLUDE_ASM("asm/battle/nonmatchings/main40_p2", func_8008963C);
extern u8 D_800D2C35;
extern void func_8008AAA0(u32 value);
extern u8 D_800C3CFB[];

/* Lay out the (up to two) D_800C3CFB item names from y = 0x11A in steps of
 * 6 (kept as y << 16), counting sprites in +0x5D7B, then register those sprites. */
void func_800897CC(void) {
    u8 pad[1];
    s32 i;
    s32 y;

    func_8008AAA0(D_800D2C35);
    for (i = 0, y = 0x11A << 16; i < 2; i++) {
        if (D_800C3CFB[i] != 0xFF) {
            D_800D2DB4[0x5D7B] += func_80076A10(D_800C3CFB[i] + 0x92, D_800D2DB4 + 0x51E0 + D_800D2DB4[0x5D7B] * 0x50, y >> 16, 0x5E);
            y += 6 << 16;
        }
    }
    D_800D2DB4[0x5D8A] = D_800CCB34;
    for (i = 0; i < D_800D2DB4[0x5D7B]; i++) {
        func_80076B68(D_800D2DB4 + 0x51E0 + (i * 2 + D_800D2DB4[0x5D8A]) * 0x28);
    }
}

INCLUDE_ASM("asm/battle/nonmatchings/main40_p2", func_800898F0);
#endif
#include "main40.c"
