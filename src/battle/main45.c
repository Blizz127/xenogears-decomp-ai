#include "common.h"


#ifndef XENO_PC_PORT
extern u8* D_800C3EAC;
extern u8 D_800CCD1C[];
extern u8 D_800C400B[];
extern u8 D_800C402F[];
extern u8 D_800C4022[];
extern u16 func_80089C9C(s32 mask, u8 idx);
extern u16 func_80089C48(u8 idx);

/* func_8008AC88.s, byte-exact: queue a 0xF3 command for the enemies in
 * `mask`.  The party bits (0-2) are dropped; each enemy slot k = 3..10
 * that func_80089C9C selects from the mask is cleared from it
 * (func_80089C48) unless the D_800CCD1C halfword of its 0x170-byte
 * record has bit 0x800.  If any slot is left, the command row at
 * D_800C3EAC +0x2DA gets D_800C400B = v, D_800C402F = 0xF3 and the
 * D_800C4022 halfword = mask, and the row cursor advances.
 * Codegen: the record offset is its own counter (`off`, from 0x450 in
 * steps of 0x170) and the address is the symbol plus that offset, as in
 * retail; `(i + 3) * 0x170` lets loop.c reduce the whole address to a
 * pointer.  Matches only under gcc 2.6.x; 2.7.2 schedules i++ at the
 * loop end, where retail fills the first branch's delay slot with it. */
void func_8008AC88(u16 mask, u8 v) {
    s32 i;
    s32 off;
    u8 k;

    mask &= 0xFFF8;
    for (i = 0, off = 0x450; i < 8; i++, off += 0x170) {
        k = i + 3;
        if (func_80089C9C(mask, k) != 0) {
            if (!(*(u16*)(D_800CCD1C + off) & 0x800)) {
                mask &= func_80089C48(k);
            }
        }
    }
    if (mask != 0) {
        D_800C400B[D_800C3EAC[0x2DA] * 0x48] = v;
        D_800C402F[D_800C3EAC[0x2DA] * 0x48] = 0xF3;
        ((u16*)D_800C4022)[D_800C3EAC[0x2DA] * 0x24] = mask;
        D_800C3EAC[0x2DA]++;
    }
}
INCLUDE_ASM("asm/battle/nonmatchings/main45", func_8008ADD0);
#endif


#ifndef XENO_PC_PORT
extern u8* D_800D2D28;
#endif
#ifndef XENO_PC_PORT
extern u8* D_800C3EAC;
#endif
extern u32 func_80089C08(u32 v);
extern void func_800BC404(u32 v);
extern void func_800BCD98(u32 v);


/* func_8008B108.s */
void func_8008B108(u8 flag) {
    u8* p;
    u8* q;
    u8* r;

    p = D_800D2D28;
    p[0x9E] = 0;
    p[0x9D] = 0;
    p[0x9C] = 0;
    q = D_800D2D28;
    q[0xB3] = 0;
    q[0xB2] = 0;
    q[0xB1] = 0;
    q[0xB0] = 0;
    r = D_800D2D28;
    r[0xB7] = 0;
    if (flag == 0) {
        D_800D2D28[0xCB] = 1;
    }
}
/* func_8008B168.s */
void func_8008B168(u8 index) {
    u8* p;
    u8* q;
    u8* r;
    u32 v;
    u32 off;

    p = D_800D2D28;
    p[0x9E] = 1;
    p[0x9D] = 1;
    p[0x9C] = 1;
    q = D_800D2D28;
    q[0xB3] = 1;
    q[0xB2] = 1;
    q[0xB1] = 1;
    q[0xB0] = 1;
    r = D_800D2D28;
    r[0xB7] = 1;
    v = func_80089C08(index & 0xFF);
    off = (u32)index << 6;
    v |= func_80089C08(*(u8*)(D_800C3EAC + off + 0x3C));
    func_800BC404(v & 0xFFFF);
    func_800BCD98(func_80089C08(*(u8*)(D_800C3EAC + off + 0x3C)) & 0xFFFF);
}
