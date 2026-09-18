#include "common.h"


/* func_800AA79C.s */
extern u32 D_800D3368[];
void func_800AA79C(u32 arg0, u32 arg1) {
    u32 a;
    u32 b;
    *(u8*)(D_800D3368[arg0] + 0x34) = 0;
    *(u8*)(D_800D3368[arg1] + 0x34) = 1;
    a = D_800D3368[arg0];
    b = D_800D3368[arg1];
    D_800D3368[arg0] = b;
    D_800D3368[arg1] = a;
}


/* func_800AA7DC.s */
extern u8 D_800C402F[];
s32 func_800AA7DC(u32 index) {
    u32 match = 0xF7;
    u32 off = index * 0x48;
    u8 v;

    do {
        v = D_800C402F[off];
        off += 0x48;
    } while (v == match);
    if (v == 0xFF) {
        v = 0xFE;
    }
    return (s32)v;
}
