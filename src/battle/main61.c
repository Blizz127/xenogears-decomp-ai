#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main61", func_8009D3A0);
INCLUDE_ASM("asm/battle/nonmatchings/main61", func_8009D948);
INCLUDE_ASM("asm/battle/nonmatchings/main61", func_8009DA04);
INCLUDE_ASM("asm/battle/nonmatchings/main61", func_8009DB54);
INCLUDE_ASM("asm/battle/nonmatchings/main61", func_8009DBFC);
#endif


extern u8* D_800D2D28;
extern u8* D_800D2DC8;


/* func_8009E268.s */
void func_8009E268(void) {
    D_800D2DC8[0x99] = 0;
}
