#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/mainl133", func_800BFC80);
#endif


extern void func_800BFC80(u32 a0, u32 a1, u32 a2);


/* func_800BFD88.s */
void func_800BFD88(u32 a0, u32 a1) {
    func_800BFC80(a0, a1, 1);
}
