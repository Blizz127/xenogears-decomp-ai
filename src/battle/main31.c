#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main31", func_8007EF6C);
INCLUDE_ASM("asm/battle/nonmatchings/main31", func_8007F8C0);
INCLUDE_ASM("asm/battle/nonmatchings/main31", func_8007FB70);
INCLUDE_ASM("asm/battle/nonmatchings/main31", func_8007FBE0);
#endif


extern u8* D_800D2D28;
extern u32 D_800D367C;
extern void HeapFree(u32 p);


/* func_8007FCE8.s */
void func_8007FCE8(void) {
    if (D_800D2D28[0xAE] != 0) {
        HeapFree(D_800D367C);
        D_800D2D28[0xAE] = 0;
    }
}
