#include "common.h"


#ifndef XENO_PC_PORT
void func_800A5BE8(s32, s32, s32, s32, s32);
extern s32 D_800D3344;
extern s32 D_800D39CC;
s32 func_800A5870(s32 arg0, s32 arg1, s32 arg2);
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main69", func_800A579C);

#ifndef XENO_PC_PORT
s32 func_800A5870(s32 arg0, s32 arg1, s32 arg2) {
    s32 temp_s0;
    void *temp_v0;

    if ((D_800D3344 == 0) || (D_800D39CC == 0) || (arg1 < 0)) {
        return -1;
    }
    temp_s0 = arg1 * 0xE;
    temp_v0 = temp_s0 + D_800D39CC;
    func_800A5BE8((*(s16 *)((s8*)(temp_v0) + 0) * 8) + D_800D3344, (*(s16 *)((s8*)(temp_v0) + 2) * 8) + D_800D3344, (*(s16 *)((s8*)(temp_v0) + 4) * 8) + D_800D3344, arg0, arg2);
    return *(u8 *)((s8*)((temp_s0 + D_800D39CC)) + 0xC);
}
#endif

INCLUDE_ASM("asm/battle/nonmatchings/main69", func_800A5914);
INCLUDE_ASM("asm/battle/nonmatchings/main69", func_800A5A48);
INCLUDE_ASM("asm/battle/nonmatchings/main69", func_800A5BE8);
INCLUDE_ASM("asm/battle/nonmatchings/main69", func_800A5D54);
#endif


#ifndef XENO_PC_PORT
extern u32 D_800D2D40;
#endif
#ifndef XENO_PC_PORT
extern u32 D_800D2D48;
#endif
#ifndef XENO_PC_PORT
extern u8 D_800C3BCC[];
#endif
extern u16 D_8005A3A0[];
#ifndef XENO_PC_PORT
extern u8 D_800D2E62[];
#endif
#ifndef XENO_PC_PORT
extern u16 D_800D39E0;
#endif
#ifndef XENO_PC_PORT
extern u8* D_800D3278;
#endif
extern void LoadImage(void* pRect, void* pData);
extern void DrawSync(s32 mode);


/* func_800A5E9C.s */
void func_800A5E9C(u32 a, u32 b) {
    D_800D2D40 = a;
    D_800D2D48 = b;
}
