#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main12", func_8007916C);
INCLUDE_ASM("asm/battle/nonmatchings/main12", func_800791FC);
INCLUDE_ASM("asm/battle/nonmatchings/main12", func_80079270);
INCLUDE_ASM("asm/battle/nonmatchings/main12", func_800792F8);
INCLUDE_ASM("asm/battle/nonmatchings/main12", func_800793F0);
INCLUDE_ASM("asm/battle/nonmatchings/main12", func_80079674);
INCLUDE_ASM("asm/battle/nonmatchings/main12", func_80079778);
INCLUDE_ASM("asm/battle/nonmatchings/main12", func_80079840);
#endif


extern u32 D_800D3344;
extern u32 D_800D39CC;
extern u8 D_800C3B74;
extern u8 D_800C3D6C;
extern u8* D_800C3610;


/* func_80079934.s */
void func_80079934(u32* pValue) {
    *pValue += 4;
}
