#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main28", func_8007D8C0);
INCLUDE_ASM("asm/battle/nonmatchings/main28", func_8007DA1C);
INCLUDE_ASM("asm/battle/nonmatchings/main28", func_8007DB78);
INCLUDE_ASM("asm/battle/nonmatchings/main28", func_8007DCF8);
INCLUDE_ASM("asm/battle/nonmatchings/main28", func_8007DE78);
INCLUDE_ASM("asm/battle/nonmatchings/main28", func_8007DFD4);
INCLUDE_ASM("asm/battle/nonmatchings/main28", func_8007E154);
INCLUDE_ASM("asm/battle/nonmatchings/main28", func_8007E1D0);
INCLUDE_ASM("asm/battle/nonmatchings/main28", func_8007E234);
INCLUDE_ASM("asm/battle/nonmatchings/main28", func_8007E334);
INCLUDE_ASM("asm/battle/nonmatchings/main28", func_8007E438);
INCLUDE_ASM("asm/battle/nonmatchings/main28", func_8007E554);
#endif


extern u32 D_800D2C60[];
extern u8 D_800D2C8B[];


/* func_8007E674.s */
void func_8007E674(u8 idx) {
    D_800D2C60[idx] = 0;
    D_800D2C8B[idx] = 4;
}
