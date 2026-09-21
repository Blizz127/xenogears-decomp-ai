#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main74", func_800B00F4);
INCLUDE_ASM("asm/battle/nonmatchings/main74", func_800B0164);
INCLUDE_ASM("asm/battle/nonmatchings/main74", func_800B026C);
INCLUDE_ASM("asm/battle/nonmatchings/main74", func_800B0AB4);
INCLUDE_ASM("asm/battle/nonmatchings/main74", func_800B0B14);
INCLUDE_ASM("asm/battle/nonmatchings/main74", func_800B0D70);
INCLUDE_ASM("asm/battle/nonmatchings/main74", func_800B0FF4);
INCLUDE_ASM("asm/battle/nonmatchings/main74", func_800B10EC);
INCLUDE_ASM("asm/battle/nonmatchings/main74", func_800B12D0);
INCLUDE_ASM("asm/battle/nonmatchings/main74", func_800B136C);
#endif


#ifndef XENO_PC_PORT
extern u32 D_800D3344;
#endif
#ifndef XENO_PC_PORT
extern u32 D_800D39CC;
#endif
#ifndef XENO_PC_PORT
extern u8 D_800C3B74;
#endif
#ifndef XENO_PC_PORT
extern u8 D_800C3D6C;
#endif
#ifndef XENO_PC_PORT
extern u8* D_800C3610;
#endif


/* func_800B14B8.s */
void func_800B14B8(void) {
    D_800C3D6C = 1;
}
