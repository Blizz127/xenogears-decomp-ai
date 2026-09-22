#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main39", func_80087A38);
INCLUDE_ASM("asm/battle/nonmatchings/main39", func_80087AF0);
INCLUDE_ASM("asm/battle/nonmatchings/main39", func_80087EDC);
INCLUDE_ASM("asm/battle/nonmatchings/main39", func_800881B8);
INCLUDE_ASM("asm/battle/nonmatchings/main39", func_800883AC);
INCLUDE_ASM("asm/battle/nonmatchings/main39", func_80088490);
#endif


#ifndef XENO_PC_PORT
extern u8 D_800C3EB4[];
#endif
#ifndef XENO_PC_PORT
extern u8 D_800D301C[];
#endif


/* func_800885D0.s */
u8 func_800885D0(u8 index) {
    /* Retail: lbu at D_800C3EB4 + 28*index, +0x18, <<2, then lbu at
     * D_800D301C + that. Both are byte loads relative to the symbol's
     * ADDRESS.
     *
     * Two bugs the differential prover caught. `(u32)D_800D301C` truncated a
     * 64-bit host pointer to 32 bits and then used it as an address, which is
     * a 32-bit-host assumption and faulted here. And D_800C3EB4 is declared
     * `u8 D_800C3EB4[][28]` in main35.c, so the shared guest-RAM alias is
     * `u8 (*)[28]`; `D_800C3EB4[index * 28]` therefore scaled by 28 twice.
     * Index both through a plain byte pointer, which is what retail does and
     * what this TU's own `extern u8 D_800C3EB4[]` means. */
    const u8* rows = (const u8*)D_800C3EB4;
    const u8* table = (const u8*)D_800D301C;
    u32 t = (u32)rows[index * 28] + 0x18;

    return table[t << 2];
}
