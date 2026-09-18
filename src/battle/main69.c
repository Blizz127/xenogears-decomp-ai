#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main69", func_800A579C);
INCLUDE_ASM("asm/battle/nonmatchings/main69", func_800A5870);
INCLUDE_ASM("asm/battle/nonmatchings/main69", func_800A5914);
INCLUDE_ASM("asm/battle/nonmatchings/main69", func_800A5A48);
INCLUDE_ASM("asm/battle/nonmatchings/main69", func_800A5BE8);
INCLUDE_ASM("asm/battle/nonmatchings/main69", func_800A5D54);
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


/* func_800A5E9C.s */
void func_800A5E9C(u32 a, u32 b) {
    D_800D2D40 = a;
    D_800D2D48 = b;
}
