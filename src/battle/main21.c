#include "common.h"

#ifdef XENO_PC_PORT
extern u8 D_800D3420[];
extern u32 func_80079E7C(u16 value);
extern u32 func_80079ED8(u8 a0, u8 a1, u8 a2, s32 a3);
extern u32 func_8007A280(u8 a0, u8 a1, u32 a2, u32 a3);
#endif

#ifdef XENO_PC_PORT
/* Port body for the retail row-to-party helper wrapper. */
void func_8007B424(u8** ppBoard, u8 index) {
    u8* p = *ppBoard;
    u8* row = D_800D3420 + ((u32)index << 6);
    u8 slot = p[3];
    u8 value = (u8)func_80079E7C(*(u16*)(row + ((u32)slot << 1)));

    value = (u8)func_80079ED8(value, p[2], 0, 1);
    row += 0x10;
    row[p[1]] = value;
}
#else
INCLUDE_ASM("asm/battle/nonmatchings/main21", func_8007B424);
#endif
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main21", func_8007B4B8);
#endif
#ifdef XENO_PC_PORT
/* Port body for the row-halfword conversion wrapper. */
void func_8007B578(u8** ppBoard, u8 index) {
    u8* p = *ppBoard;
    u8* row = D_800D3420 + ((u32)index << 6);
    u8 source = p[3];
    u8 mapped = (u8)func_80079E7C(*(u16*)(row + ((u32)source << 1)));
    u16 result = (u16)func_8007A280(mapped, p[2], 0, 1);

    *(u16*)(row + ((u32)p[1] << 1)) = result;
}
#else
INCLUDE_ASM("asm/battle/nonmatchings/main21", func_8007B578);
#endif
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main21", func_8007B608);
INCLUDE_ASM("asm/battle/nonmatchings/main21", func_8007B6C0);
INCLUDE_ASM("asm/battle/nonmatchings/main21", func_8007B7B0);
#endif
extern u8 D_800D3420[];
extern u16 D_8005A3A0[];
/* func_8007B8D4.s: board-to-party halfword setter: row[p[1]] = D_8005A3A0[p[2]]. */
void func_8007B8D4(u8** ppBoard, u8 index) {
    u32 off = (u32)index << 6;
    u8* p = *ppBoard;
    u8* base;

    base = D_800D3420;
    ((u16*)(base + off))[p[1]] = D_8005A3A0[p[2]];
}
extern u8 D_800D3420[];
extern u8 D_800D3430[];
extern u32 D_800D3410[];
extern u16 D_8005A3A0[];
extern u8 D_800CCE34[];
extern u8 D_800CCE3E[];
extern u8 D_800D343F;
extern u8 D_800D342E;


/* func_8007B914.s */
void func_8007B914(u8** ppBoard, u8 index) {
    u8* p = *ppBoard;
    u32 i1 = p[1];
    u32 i2 = p[2];
    u32 row = (u32)D_800D3420 + ((u32)index << 6);

    D_8005A3A0[i2] = *(u16*)((i1 << 1) + row);
}
/* func_8007B958.s */
void func_8007B958(u8** ppBoard, u8 index) {
    u8* p = *ppBoard;
    u8* pRow = D_800D3430 + ((u32)index << 6);

    pRow[p[2]] = pRow[p[1]];
}
