#include "common.h"


/* func_800AA79C.s */
#ifndef XENO_PC_PORT
extern u32 D_800D3368[];
#endif
void func_800AA79C(u32 arg0, u32 arg1) {
    u32 a;
    u32 b;
#ifdef XENO_PC_PORT
    /* Two separate 64-bit-host problems, both caught by the prover.
     *
     * The index is unbounded, so `D_800D3368[arg0]` runs off the end of the
     * host RAM buffer, where retail's `lw` and the interpreter both wrap
     * inside the 2 MB of RAM. Address each slot through PSX_ADDR so the host
     * wraps the same way.
     *
     * And the slot holds a *guest* address, which is a real pointer on the
     * PSX but a wild one here, so the +0x34 store needs translating too. */
    u32* pa = (u32*)PSX_ADDR(0x800D3368 + arg0 * 4);
    u32* pb = (u32*)PSX_ADDR(0x800D3368 + arg1 * 4);

    *(u8*)PSX_ADDR(*pa + 0x34) = 0;
    *(u8*)PSX_ADDR(*pb + 0x34) = 1;
    a = *pa;
    b = *pb;
    *pa = b;
    *pb = a;
#else
    *(u8*)(D_800D3368[arg0] + 0x34) = 0;
    *(u8*)(D_800D3368[arg1] + 0x34) = 1;
    a = D_800D3368[arg0];
    b = D_800D3368[arg1];
    D_800D3368[arg0] = b;
    D_800D3368[arg1] = a;
#endif
}


/* func_800AA7DC.s */
#ifndef XENO_PC_PORT
extern u8 D_800C402F[];
#endif
s32 func_800AA7DC(u32 index) {
    u32 match = 0xF7;
    u32 off = index * 0x48;
    u8 v;

    do {
#ifdef XENO_PC_PORT
        /* The walk is unbounded: it runs until it finds a byte that is not
         * 0xF7. Retail's `lbu` wraps inside the 2 MB of RAM, and the
         * interpreter masks every access the same way, but `D_800C402F[off]`
         * on the host indexes straight off the end of the RAM buffer and
         * faults. Mask per access so the host wraps exactly as the hardware
         * and the interpreter do. */
        v = *(u8*)PSX_ADDR(0x800C402F + off);
#else
        v = D_800C402F[off];
#endif
        off += 0x48;
    } while (v == match);
    if (v == 0xFF) {
        v = 0xFE;
    }
    return (s32)v;
}
