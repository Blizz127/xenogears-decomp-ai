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
extern u16 D_800CCD64__h[][184] asm("D_800CCD64");
extern u16 D_800CCD1C__h[][184] asm("D_800CCD1C");
extern u8 D_800D2E5C[];
extern u8 D_800C3D18[];
extern u8 D_800D3408[];
extern u8 func_8007F8C0(u8** pp, u8 e);
extern u8 func_8007EF6C(u8** pp, u8 e, u8 state);
extern void func_800793F0(u8 actor);

/* func_80079AB0.s, byte-exact: run enemy `actor`'s AI script (e = actor
 * - 3).  Unless its D_800CCD64 halfword (0x170-byte records) has bit
 * 0x8000 without bit 0x800 of its D_800CCD1C halfword: D_800D2E5C[0] = 0;
 * when the enemy's D_800C3D18 byte is set, the script pointer comes from
 * its 64-byte D_800D3408 record, the 0x100-byte D_800D2E5C buffer is
 * cleared, and opcodes run until 0xFD or 0xFF: opcodes >= 0x80 go to
 * func_8007F8C0 (and func_80079948 skips on when it returns 0), others
 * to func_8007EF6C (which returns the next state; opcode 0x62 marks the
 * result).  A set D_800D2E5C[0] then calls func_800793F0(actor).
 * Returns 1 when opcode 0x62 ran.
 * Codegen: the guard wraps the body (`!A || B`), which keeps e & 0xFF in
 * one register across it; the clear loop is a pointer loop with an end
 * pointer taken from it before it moves (retail reuses the `la` register
 * as the pointer); opcodes >= 0x80 are tested first, as in retail's block
 * order.  Matches under gcc 2.7.2 (main13's preset) and 2.6.x. */
u8 func_80079AB0(u8 actor) {
    u8 e;
    u8 ended;
    u8 state;
    u8* pc;
    u8* p;
    u8* end;

    ended = 0;
    e = actor - 3;
    state = 0;
    if (!(D_800CCD64__h[e + 3][0] & 0x8000) || (D_800CCD1C__h[e + 3][0] & 0x800)) {
        D_800D2E5C[0] = 0;
        if (D_800C3D18[e * 4] != 0) {
            pc = *(u8**)(D_800D3408 + (e << 6));
            p = D_800D2E5C;
            end = p + 0x100;
            *p = 0;
            while (++p < end) {
                *p = 0;
            }
            while (*pc != 0xFD && *pc != 0xFF) {
                if (*pc >= 0x80) {
                    if (func_8007F8C0(&pc, e) == 0) {
                        func_80079948(&pc);
                    }
                } else {
                    if (*pc == 0x62) {
                        ended = 1;
                    }
                    state = func_8007EF6C(&pc, e, state);
                }
            }
        }
        if (D_800D2E5C[0] != 0) {
            func_800793F0(e + 3);
        }
    }
    return ended;
}
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
