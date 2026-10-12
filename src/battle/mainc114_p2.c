/* Retail TU part 2 of mainc114.c (see the BATTLE_TU_PART note there): the
 * still-assembly run below, then that file's part-2 C bodies. */
#define BATTLE_TU_PART 2
#include "common.h"
#ifndef XENO_PC_PORT
extern u8 D_800D30BC[];
extern u8 D_800D3354[];
extern u8 D_800D335C[];
extern u8 D_800C3730[];
extern s16 D_800C354C[];
extern u8 D_800C372C[];
extern u8 D_800C3EB0[];
extern void func_800BB844(u8* m, u8* eye, u8* at, u8* p);
extern void func_800A4654(u8* view, u8* light, u32 a2, u8* ot, s32 bufIndex, u8* eye, u8* at, s32 otCount);

/* Per-frame camera + effects: rebuild the view matrix D_800D30BC from the
 * eye/target pair (func_800BB844), add the D_800C354C shake to its
 * translation (the record that holds the matrix starts 0x20 earlier), then,
 * unless D_800C372C is set, render the battle effects (func_800A4654) into
 * the current draw buffer's OT. */
void func_800BB9D4(void) {
    u8* cam;
    u8* base;

    func_800BB844(D_800D30BC, D_800D3354, D_800D335C, D_800C3730);
    cam = D_800D30BC - 0x20;
    *(s32*)(cam + 0x34) += D_800C354C[0];
    *(s32*)(cam + 0x38) += D_800C354C[1];
    *(s32*)(cam + 0x3C) += D_800C354C[2];
    if (D_800C372C[0] == 0) {
        base = (u8*)(u32)D_800C3EB0;
        func_800A4654(D_800D30BC, 0, 0, *(u8**)(base + 0x8000 + 0xC54), *(s32*)(base + 0x8000 + 0xC84),
                      D_800D3354, D_800D335C, 0x1000);
    }
}
INCLUDE_ASM("asm/battle/nonmatchings/mainc114_p2", func_800BBAB8);
INCLUDE_ASM("asm/battle/nonmatchings/mainc114_p2", func_800BBEE0);
INCLUDE_ASM("asm/battle/nonmatchings/mainc114_p2", func_800BC018);
INCLUDE_ASM("asm/battle/nonmatchings/mainc114_p2", func_800BC158);
#endif
#include "mainc114.c"
