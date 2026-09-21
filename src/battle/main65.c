#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main65", func_8009F5B8);
INCLUDE_ASM("asm/battle/nonmatchings/main65", func_8009F708);
INCLUDE_ASM("asm/battle/nonmatchings/main65", func_8009F794);
INCLUDE_ASM("asm/battle/nonmatchings/main65", func_8009F844);
INCLUDE_ASM("asm/battle/nonmatchings/main65", func_800A0838);
INCLUDE_ASM("asm/battle/nonmatchings/main65", func_800A1B50);
INCLUDE_ASM("asm/battle/nonmatchings/main65", func_800A1CF4);
#endif
extern void func_800A23E8(u8* a0, u32 p);
/* func_800A216C.s: release up to three slot pointers (+0x70/+0x74/+0x78) of
 * entry idx in the 124-byte array at a1, each gated by its own mask bit, when
 * idx is below the +0xA count. */
void func_800A216C(u8* a0, u8* a1, s32 idx, u32 mask) {
    u8* e = a1;

    if (idx < *(u16*)(e + 0xA)) {
        e += idx * 124;

        if (*(u32*)(e + 0x70) != 0 && (mask & 1)) {
            func_800A23E8(a0, *(u32*)(e + 0x70));
            *(u32*)(e + 0x70) = 0;
        }
        if (*(u32*)(e + 0x74) != 0 && (mask & 2)) {
            func_800A23E8(a0, *(u32*)(e + 0x74));
            *(u32*)(e + 0x74) = 0;
        }
        if (*(u32*)(e + 0x78) != 0 && (mask & 4)) {
            func_800A23E8(a0, *(u32*)(e + 0x78));
            *(u32*)(e + 0x78) = 0;
        }
    }
}
/* func_800A2234.s: the halfword store precedes the HeapChangeCurrentUser call
 * in C; sched sinks it below the arg setups into the jal delay slot. */
extern void HeapChangeCurrentUser(u32 a0, u32 a1);
extern void* HeapAlloc(u32 size, u32 flag);
extern void func_800A22E8(u8* p);
u8* func_800A2234(u8* a0, int a1) {
    void* p;
    int size;
    if (a1 <= 0)
        return 0;
    *(u16*)(a0 + 6) = a1;
    HeapChangeCurrentUser(4, 0);
    size = a1 * 20;
    p = HeapAlloc(size, 0);
    *(void**)a0 = p;
    if (p != 0) {
        func_800A22E8(a0);
        return a0;
    }
    return 0;
}


extern void HeapFree(u32 p);


/* func_800A22A8.s */
void func_800A22A8(u8* p) {
    u32 q = *(u32*)p;

    *(u16*)(p + 4) = 0;
    if (q != 0) {
        HeapFree(q);
    }
    *(u32*)p = 0;
}
