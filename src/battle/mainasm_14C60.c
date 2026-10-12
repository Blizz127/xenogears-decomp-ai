/* battle mainasm_14C60: the retail-asm run at file offset 0x14C60, in retail order.
 * Functions still listed as INCLUDE_ASM are not byte-matching C yet. The whole
 * file is retail-build only; any port body for these functions lives in the
 * port's own TU. */
#include "common.h"

#ifndef XENO_PC_PORT
extern u16 D_800C3D64;
extern u8 D_800C3E90[];
extern u8 D_800D3274;
extern u32 func_80083FF4(u32 actor, u32 target);
extern u32 func_80089C08(u32 v);

/* Byte-exact under gcc 2.6.3 (BattleGcc263; the port body is in main35.c).
 * Fill the 12-byte target list D_800C3E90 with 0xFF (from the end), then
 * for targets 3..10 keep those func_80083FF4(arg0, target) accepts:
 * append each to the list, OR func_80089C08(target) into D_800C3D64 and
 * count them in D_800D3274.  Returns the first list entry. */
u32 func_80084750(u32 arg0) {
    s32 remaining = 8;
    s32 fill = 0xFF;
    s32 i = 11;
    s32 output;
    u8* clear = &D_800C3E90[11];

    for (; i >= 0; i--) {
        *clear = fill;
        clear--;
    }
    output = 0;
    remaining--;
    D_800D3274 = 0;
    D_800C3D64 = 0;
    for (i = 3; remaining >= 0; remaining--) {
        u8 target = i;
        u32 eligible = func_80083FF4(arg0 & 0xFF, target);

        if (eligible & 0xFF) {
            u32 mask;

            D_800C3E90[output] = target;
            mask = func_80089C08(target);
            output++;
            D_800C3D64 |= mask;
            D_800D3274++;
        }
        i++;
    }
    return D_800C3E90[0];
}
INCLUDE_ASM("asm/battle/nonmatchings/mainasm_14C60", func_80084854);
#endif
