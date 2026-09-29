/* battle mainasm_10FF4: the retail-asm run at file offset 0x10FF4, in retail order.
 * Functions still listed as INCLUDE_ASM are not byte-matching C yet. The whole
 * file is retail-build only; any port body for these functions lives in the
 * port's own TU. */
#include "common.h"

#ifndef XENO_PC_PORT
extern u8 D_800D2DD7[]; /* rotation start; s16 slot table at +0x2F */
extern u8 D_800D2DD8[]; /* 11-entry rotation order */

/* Walk the 11-slot rotation from D_800D2DD7[0] once; return the entry v
 * (!= target) with the smallest slot value below the running minimum
 * (starting 0xFF), the minimum being the slot's low byte.  If none
 * qualifies the result is whatever was left in the register, as retail. */
s32 func_80080AE4(u8 target) {
    s32 lim = 0xFF;
    s32 i = D_800D2DD7[0];
    s32 best;

    do {
        s32 v = D_800D2DD8[i];
        s16* slot = (s16*)(D_800D2DD7 + 0x2F) + v;

        i++;
        if (*slot < lim && v != target) {
            best = v;
            lim = *(u8*)slot;
        }
        if (i == 11) {
            i = 0;
        }
    } while (i != D_800D2DD7[0]);
    return best;
}

#endif
