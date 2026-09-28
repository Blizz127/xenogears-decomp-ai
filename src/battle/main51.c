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
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80090B90);
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
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_8009413C);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_800941A4);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_800946F4);
INCLUDE_ASM("asm/battle/nonmatchings/main51", func_80094C78);
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
