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
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_8009023C);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80090310);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_800904A0);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_8009070C);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_8009080C);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_8009093C);
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

INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80090C44);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80090E7C);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80091064);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80091604);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_800916D4);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_8009187C);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80091B38);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80091D38);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80091EC4);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_8009209C);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80092298);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80092784);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80092B74);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_800930AC);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80093578);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_8009382C);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_800939CC);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80093B08);
extern void func_8007FB70(s32);
extern u8 D_800D2D24[];
extern u8* D_800D2D28;

void func_8009413C(u8 idx, u8 flag) {
    if (D_800D2D24[idx] == 4) {
        D_800D2D28[0xB7] = 0;
        D_800D2D28[0xB0] = 0;
        if (flag) {
            func_8007FB70(idx);
        }
    }
}

INCLUDE_ASM("asm/battle/nonmatchings/main51", func_800941A4);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_800946F4);
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

INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80094D24);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80094EE4);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80095690);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_800957D8);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_800958D8);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80095A78);

#ifndef XENO_PC_PORT
void func_80095B44(void) {
    if (func_80097964(*(u8 *)((s8*)(D_800C3E00) + 2), *(u8 *)((s8*)(D_800C3E00) + 3), *(u16 *)((s8*)(D_800C3E00) + 0)) == 1) {
        func_800995A0(D_800C3E50, *(u8 *)((s8*)(D_800C3DFC) + 0x1D), *(u16 *)((s8*)(D_800C3DFC) + 0x1E), 5);
    }
}
#endif

INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80095BAC);
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

INCLUDE_ASM("asm/battle/nonmatchings/main51", func_800968C0);
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
