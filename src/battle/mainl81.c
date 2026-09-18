#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/mainl81", func_800B39C0);
#endif


extern u32 D_800C3558;


/* func_800B3B6C.s */
u32 func_800B3B6C(void) {
    if (D_800C3558 != 0) {
        return *(u8*)(D_800C3558 + 0x41);
    }
    return 1;
}
