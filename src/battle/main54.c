#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main54", func_8009A0DC);
INCLUDE_ASM("asm/battle/nonmatchings/main54", func_8009A1AC);
INCLUDE_ASM("asm/battle/nonmatchings/main54", func_8009A258);
INCLUDE_ASM("asm/battle/nonmatchings/main54", func_8009A2D4);
#endif


extern u8 g_GameState[];


/* func_8009A7B8.s */
u32 func_8009A7B8(u8 index) {
    return *(u8*)(g_GameState + 0x271 + ((u32)index * 0xA4));
}
