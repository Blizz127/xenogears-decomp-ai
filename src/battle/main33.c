#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main33", func_8007FE3C);
INCLUDE_ASM("asm/battle/nonmatchings/main33", func_8007FEC4);
INCLUDE_ASM("asm/battle/nonmatchings/main33", func_8007FF14);
#endif


#ifndef XENO_PC_PORT
extern u8* D_800D2D28;
#endif
#ifndef XENO_PC_PORT
extern u8 D_800D32A1[];
#endif
#ifndef XENO_PC_PORT
extern u32 D_800D2DB4;
#endif
extern void HeapFree(u32 p);


/* func_800800E8.s */
void func_800800E8(u8 index) {
    D_800D2D28[0xAD] = 0;
    D_800D2D28[0xC7] = 0;
    D_800D2D28[0xA8] = 0;
    if (D_800D32A1[((u32)index & 0xFF) << 3] == 2) {
        D_800D32A1[((u32)index & 0xFF) << 3] = 1;
    }
    HeapFree(D_800D2DB4);
}
