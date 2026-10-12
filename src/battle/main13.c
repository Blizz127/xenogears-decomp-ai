#include "common.h"


#ifndef XENO_PC_PORT
extern void func_80079934(u8** pp);

/* func_80079948.s: advance the script cursor *pp 4 bytes at a time
 * (func_80079934) past records whose lead byte is >= 0x80, then on to the
 * next record whose lead byte is in [0x80, 0xF0).
 * The second test is written in unsigned arithmetic, which keeps both of
 * retail's `addiu -0x80` and unsigned `sltiu 0x70`: the `x < 0x80 ||
 * x >= 0xF0` and (u8)(x - 0x80) spellings fold to `addiu 0x80`, and an int
 * (x - 0x80) & 0xFF compares signed (`slti`). */
void func_80079948(u8** pp) {
    while (**pp >= 0x80) {
        func_80079934(pp);
    }
    while (((**pp - 0x80U) & 0xFF) >= 0x70) {
        func_80079934(pp);
    }
}

INCLUDE_ASM("asm/battle/nonmatchings/main13", func_800799C8);
INCLUDE_ASM("asm/battle/nonmatchings/main13", func_80079AB0);
INCLUDE_ASM("asm/battle/nonmatchings/main13", func_80079C24);
#endif


#ifndef XENO_PC_PORT
extern u8* D_800D2D28;
#endif
#ifndef XENO_PC_PORT
extern u8 D_800D3725[];
#endif
extern u32 func_80089C9C(u32 a, u32 b);


/* func_80079E18.s */
void func_80079E18(u8 index) {
    D_800D2D28[0xB4] = 1;
    *(u8*)(D_800D3725 + (((u32)index * 3) << 5)) = 1;
}
/* func_80079E4C.s */
void func_80079E4C(u8 index) {
    D_800D2D28[0xB4] = 0;
    *(u8*)(D_800D3725 + (((u32)index * 3) << 5)) = 0;
}
/* func_80079E7C.s */
u32 func_80079E7C(u16 v) {
    s32 i;

    for (i = 0; i < 0xB; i++) {
        if ((func_80089C9C(v & 0xFFFF, i & 0xFF) & 0xFFFF) != 0) {
            break;
        }
    }
    return i & 0xFF;
}
