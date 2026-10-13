/* Retail TU part 2 of main38.c (see the BATTLE_TU_PART note there): the
 * still-assembly run below, then that file's part-2 C bodies. */
#define BATTLE_TU_PART 2
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main38_p2", func_80086028);
INCLUDE_ASM("asm/battle/nonmatchings/main38_p2", func_800861D0);
extern u8* D_800C3EAC;
extern u8 D_800D2C34[];
extern u8 D_800D2D24[];
extern u8 D_800C34CC[];
extern u8 g_GameState[];
extern u32 func_80089C6C(u32 a0, u8 a1);

/* func_80086B88.s, byte-exact: 1 when entry `index` may be used in the
 * current command context (D_800C3EAC).  With +0x2D6 clear it always may.
 * Otherwise its byte of D_800C34CC (at index + (+0x2CC + 1) * 3, or
 * index + 12 when D_800D2C34 is 4) must be allowed by func_80089C6C in
 * the g_GameState +0x16C4 mask of character D_800D2D24[slot], and unless
 * +0x2CC is 0xFF or D_800D2C34 is 4, D_800D2C34 must exceed +0x2CC. */
s32 func_80086B88(u8 index, u8 slot) {
    u8 i = index + (D_800C3EAC[0x2CC] + 1) * 3;
    s32 ok = 1;

    if (D_800C3EAC[0x2D6] != 0) {
        if (D_800D2C34[0] == 4) {
            i = index + 12;
        }
        if ((u16)func_80089C6C(*(u16*)(g_GameState + 0x16C4 + D_800D2D24[slot] * 32), D_800C34CC[i]) == 0) {
            ok = 0;
        } else if (D_800C3EAC[0x2CC] != 0xFF && D_800D2C34[0] != 4 && D_800D2C34[0] < D_800C3EAC[0x2CC] + 1) {
            ok = 0;
        }
    }
    return ok;
}
INCLUDE_ASM("asm/battle/nonmatchings/main38_p2", func_80086C88);
INCLUDE_ASM("asm/battle/nonmatchings/main38_p2", func_80086F98);
INCLUDE_ASM("asm/battle/nonmatchings/main38_p2", func_800877E0);
#endif
#include "main38.c"
