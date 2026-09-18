#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main43", func_8008A3EC);
INCLUDE_ASM("asm/battle/nonmatchings/main43", func_8008A684);
#endif


extern u8 D_800C3E4C;
extern void func_8008A684(u8 v);
extern void func_8008A274(u8 v);
extern void func_8008A3EC(u8 v);
extern u32 D_8005919C;
extern void func_80039DB8(u32 v);
extern u8 D_800D366C;


/* func_8008A9C0.s */
void func_8008A9C0(u8 v) {
    switch (D_800C3E4C) {
    case 0:
        func_8008A684(v);
        break;
    case 1:
        func_8008A274(v);
        break;
    case 2:
        func_8008A3EC(v);
        break;
    default:
        break;
    }
}
/* func_8008AA40.s */
void func_8008AA40(u8 a) {
    u32 v = *(u16*)(D_8005919C + 0x14);

    func_80039DB8((v << 16) | (a & 0xFF));
}
/* Retail 8008AA74 accepts the argument word and masks it at 8008AA8C,
 * only when forwarding to 8008AA40. Keep caller declarations word-sized. */
void func_8008AA74(u32 v) {
    if (D_800D366C != 0) {
        func_8008AA40(v & 0xFF);
    }
}
