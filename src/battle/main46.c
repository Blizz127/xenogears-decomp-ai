#include "common.h"


#ifndef XENO_PC_PORT
extern u8 D_800D32A1[];
extern u8 D_800D2D24[];
extern u8 g_GameState[];
extern u16 D_800CDD40__rows[][38][20] asm("D_800CDD40");
extern u8 D_800CDD53__b[][38][40] asm("D_800CDD53");
extern u16 D_800CEF10__rows[][42][20] asm("D_800CEF10");
extern u8 D_800CEF23__b[][42][40] asm("D_800CEF23");
extern u16 D_800CCD38__rows[][0xB8] asm("D_800CCD38");
extern u8* D_800D2D28;
extern u16 func_80089C6C(u16 bits, u8 b);
extern void func_8008B108(u8 flag);
extern void func_8008B168(u8 index);
extern u8 func_80085084(u16 arg0, u8 arg1, u8 arg2);
extern void func_8008AA74(u32 v);

/* func_8008B224.s, byte-exact: use the costed entry at (col, row) of
 * party actor `actor`, the func_8008CDE4 pattern.  An actor whose
 * D_800D32A1 byte (8-byte rows) is clear takes entry row * 2 + col + 0x16
 * of its 38 x 40-byte D_800CDD40 table and asks func_80089C6C about the
 * g_GameState + 0x16C2 halfword; otherwise entry + 0x15 of its 42 x
 * 40-byte D_800CEF10 table and the + 0x16C6 halfword (both indexed by
 * the actor's D_800D2D24 byte * 32).  The entry's halfword +0 is the
 * target word and its byte +0x13 the cost.  When the actor's D_800CCD38
 * halfword covers the cost and the check passed: func_8008B108(1),
 * D_800D2D28 +0xC6 = 1, func_80085084(target, actor, 0); on commit the
 * cost is paid and it returns 1, else +0xC6 is cleared and
 * func_8008B168(actor) runs.  Otherwise func_8008AA74(0x4F).
 * First draft; it matches under gcc 2.7.2 (main46's preset) and 2.6.x. */
u8 func_8008B224(u8 actor, u8 col, u8 row) {
    u16 target;
    u8 cost;
    u8 ok;
    u8 fail;
    u8 ret;

    ret = 0;
    fail = 1;
    ok = 0;
    if (D_800D32A1[actor * 8] == 0) {
        target = D_800CDD40__rows[actor][row * 2 + col + 0x16][0];
        cost = D_800CDD53__b[actor][row * 2 + col + 0x16][0];
        ok = func_80089C6C(*(u16*)(g_GameState + 0x16C2 + D_800D2D24[actor] * 32), row * 2 + col) != 0;
    } else {
        target = D_800CEF10__rows[actor][row * 2 + col + 0x15][0];
        cost = D_800CEF23__b[actor][row * 2 + col + 0x15][0];
        if (func_80089C6C(*(u16*)(g_GameState + 0x16C6 + D_800D2D24[actor] * 32), row * 2 + col)) {
            ok = 1;
        }
    }
    if (D_800CCD38__rows[actor][0] >= cost && ok) {
        func_8008B108(1);
        D_800D2D28[0xC6] = 1;
        if (func_80085084(target, actor, 0)) {
            D_800CCD38__rows[actor][0] -= cost;
            ret = 1;
        } else {
            D_800D2D28[0xC6] = 0;
            func_8008B168(actor);
        }
        fail = 0;
    }
    if (fail) {
        func_8008AA74(0x4F);
    }
    return ret;
}
INCLUDE_ASM("asm/battle/nonmatchings/main46", func_8008B478);
INCLUDE_ASM("asm/battle/nonmatchings/main46", func_8008B908);
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


/* func_8008BC40.s */
void func_8008BC40(u8 flag) {
    u8* p;
    u8* q;
    u8* r;

    p = D_800D2D28;
    p[0x9E] = 0;
    p[0x9D] = 0;
    p[0x9C] = 0;
    q = D_800D2D28;
    q[0xB1] = 0;
    q[0xB0] = 0;
    r = D_800D2D28;
    r[0xB7] = 0;
    if (flag == 0) {
        D_800D2D28[0xCB] = 1;
    }
}
/* func_8008BC98.s */
void func_8008BC98(u8 index) {
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
    q[0xB1] = 1;
    q[0xB0] = 1;
    r = D_800D2D28;
    r[0xB7] = 2;
    v = func_80089C08(index & 0xFF);
    off = (u32)index << 6;
    v |= func_80089C08(*(u8*)(D_800C3EAC + off + 0x3C));
    func_800BC404(v & 0xFFFF);
    func_800BCD98(func_80089C08(*(u8*)(D_800C3EAC + off + 0x3C)) & 0xFFFF);
}
