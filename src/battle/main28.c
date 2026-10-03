#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main28", func_8007D8C0);
INCLUDE_ASM("asm/battle/nonmatchings/main28", func_8007DA1C);
INCLUDE_ASM("asm/battle/nonmatchings/main28", func_8007DB78);
INCLUDE_ASM("asm/battle/nonmatchings/main28", func_8007DCF8);
INCLUDE_ASM("asm/battle/nonmatchings/main28", func_8007DE78);
INCLUDE_ASM("asm/battle/nonmatchings/main28", func_8007DFD4);
void func_80087EDC(s32, s32, s32);               /* extern */
void func_800883AC(s32, void *, s32);            /* extern */
extern u8 D_800C3EB7[];

void func_8007E154(void **arg0, s32 arg1) {
    s32 temp_a0;
    s32 temp_a0_2;
    s32 idx;
    void *temp_a1;

    temp_a1 = *arg0;
    idx = (arg1 & 0xFF) + 3;
    D_800C3EB7[idx * 0x1C] = *(u8 *)((s8*)(temp_a1) + 1);
    temp_a0 = arg1 + 3;
    if (*(u8 *)((s8*)(*arg0) + 1) & 0x80) {
        temp_a0_2 = temp_a0 & 0xFF;
        func_80087EDC(temp_a0_2, temp_a0_2, arg1);
        return;
    }
    func_800883AC(temp_a0 & 0xFF, temp_a1, arg1);
}

extern u8 D_800D3420[];
extern u32 func_80089C08(u8 idx);

/* func_8007E1D0: T[p[1]] = (u16)func_80089C08((idx+3)&0xFF), T = D_800D3420
 * row base (idx&0xFF)<<6. func_80089C08 is declared with varying return
 * width across TUs (u16 in main35.c/main38.c/main36.c, u32 in main27.c/
 * main46.c); u32 matches src/battle/main27.c's declaration and the call
 * result is truncated to u16 at the store, same as that TU's pattern.
 * Straightforward translation matched first try, no register pins needed. */
void func_8007E1D0(u8 **pp, s32 idx) {
    u16 result;
    u32 row;
    u8 *p;
    u32 off;

    result = (u16)func_80089C08((idx + 3) & 0xFF);
    row = (idx & 0xFF) << 6;
    p = *pp;
    off = p[1];
    row += (u32)D_800D3420;
    off <<= 1;
    *(u16 *)(off + row) = result;
}

INCLUDE_ASM("asm/battle/nonmatchings/main28", func_8007E234);
INCLUDE_ASM("asm/battle/nonmatchings/main28", func_8007E334);
INCLUDE_ASM("asm/battle/nonmatchings/main28", func_8007E438);
INCLUDE_ASM("asm/battle/nonmatchings/main28", func_8007E554);
#endif


#ifndef XENO_PC_PORT
extern u32 D_800D2C60[];
#endif
#ifndef XENO_PC_PORT
extern u8 D_800D2C8B[];
#endif


/* func_8007E674.s */
void func_8007E674(u8 idx) {
    D_800D2C60[idx] = 0;
    D_800D2C8B[idx] = 4;
}
