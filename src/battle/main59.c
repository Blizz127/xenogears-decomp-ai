#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main59", func_8009C134);
INCLUDE_ASM("asm/battle/nonmatchings/main59", func_8009C198);
INCLUDE_ASM("asm/battle/nonmatchings/main59", func_8009C4B4);
INCLUDE_ASM("asm/battle/nonmatchings/main59", func_8009C9C4);
#endif
#ifndef XENO_PC_PORT
extern u8* D_800C34B0;
#endif
#ifndef XENO_PC_PORT
extern u8* D_800C3DFC;
#endif
#ifndef XENO_PC_PORT
extern u8 D_800C3E50;
#endif
#ifdef XENO_PC_PORT
/* Coexistence: decompiled and logic-faithful, but not byte-exact yet, so the
 * matching build assembles retail bytes below and the port uses this body. */
/* func_8009CA90.s: advance the D_800C3DFC cursor by 0x78 when the +0x5FC2 mode
 * is 0xC..0xE, or by 0x258 when it is 0..2; either way only while the active
 * actor's +0x15A flag has bit7 clear.  D_800C34B0 is loaded once. */
void func_8009CA90(void) {
    u8* p = D_800C34B0;

    /* D_800C3DFC is pointer-typed in the guest-RAM alias, so the cursor word
     * itself has to be advanced through PSX_ADDR; assigning to the alias is not
     * even an lvalue. Retail adds to the stored guest address, same as here. */
    if ((u32)(p[0x5FC2] - 0xC) < 3) {
        if (((p + D_800C3E50 * 368)[0x15A] & 0x80) == 0) {
            *(u32 *)PSX_ADDR(0x800C3DFC) += 0x78;
            return;
        }
    }
    if (p[0x5FC2] < 3) {
        if (((p + D_800C3E50 * 368)[0x15A] & 0x80) == 0) {
            *(u32 *)PSX_ADDR(0x800C3DFC) += 0x258;
        }
    }
}
#else
INCLUDE_ASM("asm/battle/nonmatchings/main59", func_8009CA90);
#endif
#ifndef XENO_PC_PORT
extern u8 D_800CCD8C[];
#endif


/* func_8009CB68.s */
void func_8009CB68(u8 index) {
    u32 off = ((u32)index & 0xFF) * 368;
    u8* base = D_800CCD8C;
    u16* p = (u16*)((u32)(unsigned int)base + off);
    u8* q = base;
    u16* r;
    u16 v;

    q -= 0xA4;
    r = (u16*)((u32)(unsigned int)q + off);
    if ((p[0x40] & 0x200) != 0) {
        v = r[0x42] | 0x20;
    } else {
        v = r[0x42] & 0xFFDF;
    }
    r[0x42] = v;
}
