#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main3", func_80076544);
INCLUDE_ASM("asm/battle/nonmatchings/main3", func_800765C4);
INCLUDE_ASM("asm/battle/nonmatchings/main3", func_80076710);
#endif


extern u32 D_800D2D40;
extern u32 D_800D2D48;
extern u8 D_800C3BCC[];
extern u16 D_8005A3A0[];
extern u8 D_800D2E62[];
extern u16 D_800D39E0;
extern u8* D_800D3278;
extern void LoadImage(void* pRect, void* pData);
extern void DrawSync(s32 mode);


/* func_800769E8.s */
void func_800769E8(void* pRect, void* pData) {
    LoadImage(pRect, pData);
    DrawSync(0);
}
