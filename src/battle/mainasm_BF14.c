/* battle mainasm_BF14: the retail-asm run at file offset 0xBF14, in retail order.
 * Functions still listed as INCLUDE_ASM are not byte-matching C yet. The whole
 * file is retail-build only; any port body for these functions lives in the
 * port's own TU. */
#include "common.h"

#ifndef XENO_PC_PORT
extern u8 D_800D3420[];

/* func_8007BA04: row u8->u16: T[p[2]] = B16[p[1]]  (T = D_800D3420 row,
 * B16 = row base + 0x10). Same shape as func_8007B98C/func_8007B9C8 in
 * src/battle/main22.c, but byte-exact reg alloc needs the row-base ($6)
 * and shifted-index ($3) pinned via register asm; without the pins gcc's
 * local-alloc picks the opposite v1/a2 pair (or moves p off $4) and the
 * tail scheduling/operand order drifts. p is pinned to $4, reusing the
 * incoming pp argument register exactly as retail does (lw a0,0(a0)). */
void func_8007BA04(u8 **pp, s32 idx) {
    register u8 *p asm("$4");
    register u32 t_base asm("$6");
    register u32 idx2 asm("$3");
    u32 row;
    u8 *base;
    u8 *temp;
    u8 *b16;
    u32 idx1;

    row = (idx & 0xFF) << 6;
    base = D_800D3420;
    t_base = (u32)base + row;
    temp = base + 0x10;
    p = *pp;
    b16 = row + temp;
    idx2 = p[2];
    idx1 = p[1];
    idx2 <<= 1;
    *(u16 *)(idx2 + t_base) = b16[idx1];
}

extern u8 D_800D3410[];

/* func_8007BA44: row u32 = u16: T[p[2]] = B16[p[1]]  (T = D_800D3410 row,
 * B16 = row base + 0x10, p[2] scaled by 4, p[1] scaled by 2). Sibling of
 * func_8007BA04 (src/battle/main22.c has the not-yet-matching port body
 * for both). Byte-exact reg alloc needs base/$3, the row+base pointer/
 * $6, and both scaled indices/$4,$2 pinned; the address adds must stay
 * plain u32 math (not pointer types) or gcc's commutative-operand
 * canonicalization picks the wrong rs/rt order for the final adds. */
void func_8007BA44(u8 **pp, s32 idx) {
    register u8 *base asm("$3");
    register u32 t_base asm("$6");
    register u32 f2 asm("$4");
    register u32 f1 asm("$2");
    u32 row;
    u32 temp;
    u8 *p;
    u32 b16;
    u16 val;

    row = (idx & 0xFF) << 6;
    base = D_800D3410;
    t_base = (u32)base + row;
    temp = (u32)base + 0x10;
    p = *pp;
    b16 = row + temp;
    f2 = p[2];
    f1 = p[1];
    f2 <<= 2;
    f1 <<= 1;
    val = *(u16 *)(f1 + b16);
    *(u32 *)(f2 + t_base) = val;
}

#endif
