#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main60", func_8009CBC4);
#endif


extern u8* D_800C34B0;
extern u8 D_800C3E50;
extern u32 func_8009DBFC(u32 a0);


/* func_8009D354.s */
void func_8009D354(void) {
    if ((func_8009DBFC(0) << 24) == 0) {
        u8* p = D_800C34B0;

        *(u8*)(p + D_800C3E50 + 0x5FA0) = 6;
    }
}
