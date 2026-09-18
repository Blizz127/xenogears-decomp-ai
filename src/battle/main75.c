#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main75", func_800B14CC);
#endif
typedef struct { u32 w[7]; } BattleOtRecord;
extern BattleOtRecord D_800C3BD0;
extern u32 D_800C3BEC;
extern u32 D_800C3BF0;
#ifdef XENO_PC_PORT
/* Coexistence: decompiled and logic-faithful, but not byte-exact yet, so the
 * matching build assembles retail bytes below and the port uses this body. */
/* func_800B15D8.s: copy entry idx of the 28-byte record array at a0 + 0xC into
 * D_800C3BD0; unless bit0 of a0[+4] is set, relocate the record's three
 * pointer words by the entry address.  Publishes the +0x10 word in D_800C3BEC,
 * clears D_800C3BF0 and returns the entry's +0x14 word. */
u32 func_800B15D8(u8* a0, s32 idx) {
    u8* e = a0 + idx * 28 + 0xC;

    D_800C3BD0 = *(BattleOtRecord*)e;
    if ((*(u32*)(a0 + 4) & 1) == 0) {
        D_800C3BD0.w[0] += (u32)e;
        D_800C3BD0.w[2] += (u32)e;
        D_800C3BD0.w[4] += (u32)e;
    }
    D_800C3BF0 = 0;
    D_800C3BEC = D_800C3BD0.w[4];
    return *(u32*)(e + 0x14);
}
#else
INCLUDE_ASM("asm/battle/nonmatchings/main75", func_800B15D8);
#endif
extern u32 D_800D2D40;
extern u32 D_800D2D48;
extern u8 D_800C3BCC[];
extern u16 D_8005A3A0[];
extern u8 D_800D2E62[];
extern u16 D_800D39E0;
extern u8* D_800D3278;
extern void LoadImage(void* pRect, void* pData);
extern void DrawSync(s32 mode);


/* func_800B168C.s */
u8* func_800B168C(u8* pArg, u32 iValue) {
    return pArg + (iValue * 0x1C + 0xC);
}
