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
    u32 t = (u32)D_800C3EB4[index * 28] + 0x18;

    return *(u8*)((u32)D_800D301C + (t << 2));
}
