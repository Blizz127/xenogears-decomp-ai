#include "common.h"
/* Retail TU split: gcc 2.7.2 cc1 emits every file-scope INCLUDE_ASM block
 * before the first compiled body, so a mixed TU whose retail layout has a C
 * function ahead of an assembly function cannot be one object.  Each
 * contiguous [asm run][C run] of the retail layout is therefore its own
 * matching TU: src/battle/main73_p<N>.c defines BATTLE_TU_PART=<N> and
 * includes this file.  Host/port builds compile this file whole (part 0). */
#ifndef BATTLE_TU_PART
#define BATTLE_TU_PART 0
#endif
#define BATTLE_PART(n) (BATTLE_TU_PART == 0 || BATTLE_TU_PART == (n))

extern u16 D_800C3E30;


#if BATTLE_PART(1)
/* func_800AF400.s: index of the lowest set bit in D_800C3E30
 * (13 when bits 0-12 are all clear). */
/* Not yet byte-matching: the matching build assembles retail bytes for
 * this function from its own asm segment (see config/battle.yaml).  The
 * body below is kept for the port and the differential harnesses. */
#ifdef XENO_PC_PORT
s32 func_800AF400(void) {
    s32 bit = 0;

    while (((D_800C3E30 >> bit) & 1) == 0) {
        if (++bit == 13) {
            break;
        }
    }
    return bit;
}
#endif /* XENO_PC_PORT */
#endif /* BATTLE_PART(1) */
#if BATTLE_PART(2)
#endif /* BATTLE_PART(2) */


extern u32 D_800D2D40;
extern u32 D_800D2D48;
extern u8 D_800C3BCC[];
extern u16 D_8005A3A0[];
extern u8 D_800D2E62[];
extern u16 D_800D39E0;
extern u8* D_800D3278;
extern void LoadImage(void* pRect, void* pData);
extern void DrawSync(s32 mode);


#if BATTLE_PART(2)
/* func_800B00D0.s: clear nine words counting down from D_800C3BCC. */
void func_800B00D0(void) {
    s32 i;

    for (i = 8; i >= 0; i--) {
        *(u32*)((u8*)D_800C3BCC + (i - 8) * 4) = 0;
    }
}
#endif /* BATTLE_PART(2) */
