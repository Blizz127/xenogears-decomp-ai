#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main77", func_800B1720);
INCLUDE_ASM("asm/battle/nonmatchings/main77", func_800B1EA0);
INCLUDE_ASM("asm/battle/nonmatchings/main77", func_800B1F0C);
INCLUDE_ASM("asm/battle/nonmatchings/main77", func_800B1F6C);
INCLUDE_ASM("asm/battle/nonmatchings/main77", func_800B2AEC);
#endif


#ifndef XENO_PC_PORT
extern u8* D_800D2D28;
#endif
#ifndef XENO_PC_PORT
extern u8* D_800D2DC8;
#endif


/* func_800B3348.s */
void func_800B3348(void) {
}
/* func_800B3350.s */
void func_800B3350(void) {
}


#ifdef XENO_PC_PORT
#include "battle_host_guest.h"
/* func_800B1EA0.s: one-shot scale of a vertex block.  If flag 0x8000 of the
 * +0x18 word is clear, set it and shift every s16 x/y/z of the `count`
 * (+0x4) 8-byte vertices at header + header[0] left by `shift` (SLLV: low
 * five bits). */
void func_800B1EA0(u8* header, u32 shift) {
    u32 h = BG_ADDR(header);
    u32 flags = BG_U32(h + 0x18);
    u32 count;
    u32 v;
    u32 i;

    if ((flags & 0x8000) != 0) {
        return;
    }
    BG_U32(h + 0x18) = flags | 0x8000;
    v = BG_U32(h) + h;
    count = BG_U32(h + 4);
    shift &= 31;
    for (i = 0; i != count; i++, v += 8) {
        BG_U16(v + 0) = (u16)((u32)(s32)BG_S16(v + 0) << shift);
        BG_U16(v + 4) = (u16)((u32)(s32)BG_S16(v + 4) << shift);
        BG_U16(v + 2) = (u16)((u32)(s32)BG_S16(v + 2) << shift);
    }
}
#endif
