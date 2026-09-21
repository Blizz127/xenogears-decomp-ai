#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main53", func_80097D5C);
INCLUDE_ASM("asm/battle/nonmatchings/main53", func_8009892C);
INCLUDE_ASM("asm/battle/nonmatchings/main53", func_80098AF8);
INCLUDE_ASM("asm/battle/nonmatchings/main53", func_80098C6C);
INCLUDE_ASM("asm/battle/nonmatchings/main53", func_80098D2C);
INCLUDE_ASM("asm/battle/nonmatchings/main53", func_80099498);
INCLUDE_ASM("asm/battle/nonmatchings/main53", func_800995A0);
INCLUDE_ASM("asm/battle/nonmatchings/main53", func_80099890);
INCLUDE_ASM("asm/battle/nonmatchings/main53", func_80099CF0);
#endif
#ifndef XENO_PC_PORT
extern u8* D_800C34B0;
#endif
#ifndef XENO_PC_PORT
extern u8* D_800C3E00;
#endif
#ifndef XENO_PC_PORT
extern u8* D_800C3DFC;
#endif
#ifdef XENO_PC_PORT
/* Coexistence: decompiled and logic-faithful, but not byte-exact yet, so the
 * matching build assembles retail bytes below and the port uses this body. */
/* func_80099FB0.s: copy the four D_800C3DFC status bytes into the
 * D_800C34B0 +0x5FBC block, mirror +0x5FC2 into +0x5FC0, and when the
 * copied +0x5FBE byte has no low-6 bits, fold in the top nibble of the
 * combined D_800C3E00 +0x8C/+0x8E halfwords. */
void func_80099FB0(void) {
    u32 m = *(u16*)(D_800C3E00 + 0x8C) | *(u16*)(D_800C3E00 + 0x8E);
    u8* p;
    u8 v;

    D_800C34B0[0x5FBC] = D_800C3DFC[0x20];
    D_800C34B0[0x5FBD] = D_800C3DFC[0x21];
    D_800C34B0[0x5FBE] = D_800C3DFC[0x22];
    D_800C34B0[0x5FBF] = D_800C3DFC[0x23];
    D_800C34B0[0x5FC0] = D_800C34B0[0x5FC2];
    p = D_800C34B0;
    v = p[0x5FBE];
    if ((v & 0x3F) == 0) {
        p[0x5FBE] = (m >> 12) | v;
    }
}
#else
INCLUDE_ASM("asm/battle/nonmatchings/main53", func_80099FB0);
#endif
#ifndef XENO_PC_PORT
extern u8* D_800C34B0;
#endif


/* func_8009A074.s */
void func_8009A074(void) {
    if ((*(u16*)(D_800C34B0 + 0x5FAC) & 7) != 0) {
        *(u16*)(D_800C34B0 + 0x5FAC) = *(u16*)(D_800C34B0 + 0x5FAE) & 7;
    }
    if ((*(u16*)(D_800C34B0 + 0x5FAC) & 0x7F8) != 0) {
        *(u16*)(D_800C34B0 + 0x5FAC) = *(u16*)(D_800C34B0 + 0x5FAE) & 0x7F8;
    }
}
