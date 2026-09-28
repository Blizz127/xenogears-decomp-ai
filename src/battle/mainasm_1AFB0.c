/* battle mainasm_1AFB0: the retail-asm run at file offset 0x1AFB0, in retail order.
 * Functions still listed as INCLUDE_ASM are not byte-matching C yet. The whole
 * file is retail-build only; any port body for these functions lives in the
 * port's own TU. */
#include "common.h"

#ifndef XENO_PC_PORT

extern u8 D_800C3CF4[];

/* Split `value` into nine decimal digits (most significant first) and blank
 * the leading zeros with 0xFF, keeping at least the last digit. */
void func_8008AAA0(u32 value) {
    s32 i;
    u32 div = 100000000;

    for (i = 0; i < 9; i++) {
        D_800C3CF4[i] = value / div;
        value %= div;
        div /= 10;
    }
    for (i = 1; i < 9; i++) {
        if (D_800C3CF4[i] != 0) {
            if (D_800C3CF4[i - 1] == 0) {
                D_800C3CF4[i - 1] = 0xFF;
            }
            break;
        }
        D_800C3CF4[i - 1] = 0xFF;
    }
}

#endif
