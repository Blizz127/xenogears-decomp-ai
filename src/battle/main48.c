#include "common.h"


#ifndef XENO_PC_PORT
extern u8* D_800D2D28;
extern u8* D_800C3EAC;
extern u8 D_800D366C;
extern u8 D_800C3E2C;
extern u8 D_800C4928;
extern u16 D_800D2C94;
extern u8 D_800CCD1C[];
extern u8 D_800C400B__rows[][0x48] asm("D_800C400B");
extern u8 D_800C402F__rows[][0x48] asm("D_800C402F");
extern u16 D_800C4022__rows[][0x24] asm("D_800C4022");
extern u16 D_800C3FFE__rows[][0x24] asm("D_800C3FFE");
extern u8 func_8008C81C(u8 actor);
extern void func_800BAF40();
extern void func_8008C360(u8 a);
extern void func_800BCD98(u32 v);
extern u32 func_800877E0(u8 a, u8 b);
extern void func_80087A38(u8 index);
extern u32 func_80089C08(u32 v);
extern void func_80085CCC(u8 a0, u16 a1, u16 a2);
extern void func_80085C88(u8 index);
extern void func_80080B64(u8 v);
extern void func_80087EDC(u8 index, u8 target);
extern void func_800716D8(void);

/* func_8008C4A8.s, byte-exact: run actor `actor`'s target menu
 * (func_8008C81C).  On cancel: func_800BAF40(actor, 0x100),
 * func_8008C360(1), D_800D2D28 +0xCB = 1, return 0.  Otherwise:
 * func_800BAF40(actor, 0x80), D_800D2D28 +0xAF = 0, func_8008C360(1),
 * D_800D366C = 0, func_800BCD98(0); the actor's D_800C3EAC row byte
 * +0x3C (64-byte rows) takes the selection D_800C3E2C;
 * func_800877E0(actor, target), D_800C4928 = 1, func_80087A38(actor).
 * A target whose D_800CCD1C halfword (0x170-byte records) has bit 0x800
 * gets a 0xF3 command row (D_800C400B = actor, D_800C4022 =
 * func_80089C08(target)).  Then for each of the seven D_800C3EAC +0x2C4
 * bytes that is not 0xFF: +0x2DC = byte + 8, func_80085CCC(actor,
 * func_80089C08(target), +0x2DC - 1), func_80085C88(row), and a command
 * row with D_800C402F = +0x2DC - 1, D_800C400B = actor and D_800C3FFE =
 * D_800D2C94.  Finally func_80080B64(actor), func_80087EDC(actor,
 * target), wait for D_800C3EAC +0x2DB (func_800716D8), return 1.
 * Codegen: the command rows go through asm-label row views, which keep
 * the symbol in each address (u8 arrays let loop.c hoist three `la`s
 * into s-registers); the actor row is `(D_800C3EAC + (actor << 6))`,
 * base first, as in func_80079840; the cancel path falls through to the
 * shared `return ret`.  Matches only under gcc 2.6.x (retail's u8
 * `+0x2DC - 1` is `addiu 0xFF`). */
u8 func_8008C4A8(u8 actor) {
    s32 i;
    u8 ret;

    ret = 1;
    if (func_8008C81C(actor)) {
        func_800BAF40(actor, 0x100);
        func_8008C360(1);
        D_800D2D28[0xCB] = 1;
        ret = 0;
    } else {
        func_800BAF40(actor, 0x80);
        D_800D2D28[0xAF] = 0;
        func_8008C360(1);
        D_800D366C = 0;
        func_800BCD98(0);
        (D_800C3EAC + (actor << 6))[0x3C] = D_800C3E2C;
        func_800877E0(actor, (D_800C3EAC + (actor << 6))[0x3C]);
        D_800C4928 = 1;
        func_80087A38(actor);
        if (*(u16*)(D_800CCD1C + (D_800C3EAC + (actor << 6))[0x3C] * 0x170) & 0x800) {
            D_800C400B__rows[D_800C3EAC[0x2DA]][0] = actor;
            D_800C402F__rows[D_800C3EAC[0x2DA]][0] = 0xF3;
            D_800C4022__rows[D_800C3EAC[0x2DA]][0] = func_80089C08((D_800C3EAC + (actor << 6))[0x3C]);
            D_800C3EAC[0x2DA]++;
        }
        for (i = 0; i < 7; i++) {
            if ((D_800C3EAC + i)[0x2C4] != 0xFF) {
                D_800C3EAC[0x2DC] = (D_800C3EAC + i)[0x2C4] + 8;
                func_80085CCC(actor, func_80089C08((D_800C3EAC + (actor << 6))[0x3C]), D_800C3EAC[0x2DC] - 1);
                func_80085C88(D_800C3EAC[0x2DA]);
                D_800C402F__rows[D_800C3EAC[0x2DA]][0] = D_800C3EAC[0x2DC] - 1;
                D_800C400B__rows[D_800C3EAC[0x2DA]][0] = actor;
                D_800C3FFE__rows[D_800C3EAC[0x2DA]][0] = D_800D2C94;
                D_800C3EAC[0x2DA]++;
            }
        }
        func_80080B64(actor);
        func_80087EDC(actor, (D_800C3EAC + (actor << 6))[0x3C]);
        while (D_800C3EAC[0x2DB] == 0) {
            func_800716D8();
        }
    }
    return ret;
}
INCLUDE_ASM("asm/battle/nonmatchings/main48", func_8008C81C);
#endif


#ifndef XENO_PC_PORT
extern u8* D_800D2D28;
#endif
#ifndef XENO_PC_PORT
extern u8 D_800D3420[];
#endif
#ifndef XENO_PC_PORT
extern u8 D_800D3430[];
#endif
#ifndef XENO_PC_PORT
extern u32 D_800D3410[];
#endif
extern u16 D_8005A3A0[];
#ifndef XENO_PC_PORT
extern u8 D_800D366C;
#endif
#ifndef XENO_PC_PORT
extern u8* D_800C3EAC;
#endif
extern u32 func_80089C08(u32 v);
extern void func_800BC404(u32 v);
extern void func_800BCD98(u32 v);
extern void func_80085454(s32);
extern void func_80085618(s32);
extern void func_8008AA40(u8 v);
#ifndef XENO_PC_PORT
extern u32 D_800C3EA4;
#endif
extern void* func_8008ABB8(s32 size, s32 flag);
extern void* bzero(unsigned char* p, int size);
extern void func_80077074(void);


/* func_8008CCCC.s */
void func_8008CCCC(u8 flag) {
    u8* p;
    u8* q;
    u8* r;

    p = D_800D2D28;
    p[0x9E] = 0;
    p[0x9D] = 0;
    p[0x9C] = 0;
    q = D_800D2D28;
    q[0xB2] = 0;
    q[0xB1] = 0;
    q[0xB0] = 0;
    r = D_800D2D28;
    r[0xB7] = 0;
    if (flag == 0) {
        D_800D2D28[0xCB] = 1;
    }
}
/* func_8008CD28.s */
void func_8008CD28(u8 index) {
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
    q[0xB2] = 1;
    q[0xB1] = 1;
    q[0xB0] = 1;
    r = D_800D2D28;
    r[0xB7] = 4;
    v = func_80089C08(index & 0xFF);
    off = (u32)index << 6;
    v |= func_80089C08(*(u8*)(D_800C3EAC + off + 0x3C));
    func_800BC404(v & 0xFFFF);
    func_800BCD98(func_80089C08(*(u8*)(D_800C3EAC + off + 0x3C)) & 0xFFFF);
}
