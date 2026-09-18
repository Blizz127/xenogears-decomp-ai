#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main42", func_80089CCC);
INCLUDE_ASM("asm/battle/nonmatchings/main42", func_8008A144);
#endif


extern u8 D_800CCC58;
extern u8* D_800D2D28;
extern u8* D_800C3EA4;
extern u8 D_800D39D4;
extern s32 D_800D3288;
extern void func_8007171C(void);
extern void func_80089CCC(u8 v);
extern void func_8008A144(void);


/* func_8008A274.s */
void func_8008A274(u8 v) {
    if (D_800CCC58 != 0) {
        func_8007171C();
    }
    if ((v & 0xFF) == 0) {
        func_80089CCC(0);
    }
    D_800D2D28[0xA9] += 6;
    D_800D2D28[0xAB] += 1;
    if (*(u8*)(D_800C3EA4 + 0x6415) != 0) {
        if (*(u8*)(D_800C3EA4 + 0x6416) == 0) {
            *(s32*)(D_800C3EA4 + 0x6410) += 4;
            if (*(s32*)(D_800C3EA4 + 0x6410) >= 0x81) {
                *(s32*)(D_800C3EA4 + 0x6410) = 0x7C;
                *(u8*)(D_800C3EA4 + 0x6416) = 1;
            }
        } else {
            *(s32*)(D_800C3EA4 + 0x6410) -= 4;
            if (*(s32*)(D_800C3EA4 + 0x6410) < 0) {
                *(s32*)(D_800C3EA4 + 0x6410) = 4;
                *(u8*)(D_800C3EA4 + 0x6416) = 0;
            }
        }
    }
    switch (D_800D39D4) {
    case 1:
    case 3:
        D_800D3288 += 1;
        break;
    case 2:
    case 4:
        D_800D3288 -= 1;
        break;
    default:
        break;
    }
    func_8008A144();
}
