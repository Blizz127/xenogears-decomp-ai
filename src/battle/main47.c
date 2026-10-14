#include "common.h"


#ifndef XENO_PC_PORT
extern u8* D_800D2D28;
extern u8 D_800D2CE0[];
extern u8 D_800D2204[];
extern u8 D_800C3D70[];
extern u8 D_800D3688[];
extern void func_8008BC40(u8 flag);
extern void func_8008BC98(u8 index);
extern u32 func_8009A7E4();
extern void func_8009A854();
extern void func_8008AA74(u32 v);
extern u8 func_80085084(u16 arg0, u8 arg1, u8 arg2);

/* func_8008BD50.s, byte-exact: use the command-menu entry at (col, row),
 * index row * 2 + col.  The item is the D_800D2CE0 entry (D_800C3D70 when
 * mode is set); the target word is the halfword at D_800D2204 + 16 * the
 * D_800D2CE0 entry.  An empty item calls func_8008AA74(0x4F) and returns
 * 0.  Mode 0: func_8008BC40(1) and D_800D2D28 +0xC6 = 1, then
 * func_80085084(target, actor, 0); it returns 1 when that commits, else
 * it clears +0xC6, calls func_8008BC98(actor) and returns 0.  Otherwise,
 * when func_8009A7E4(item - 0x32) accepts it, func_8009A854(item) runs,
 * the entry's D_800D3688 count drops by one (clearing its D_800C3D70
 * entry at 0) and it returns 2; else 0.
 * Codegen: func_8009A7E4 and func_8009A854 are declared without
 * prototypes, because retail passes item - 0x32 unmasked and leaves $a1
 * as it is for func_8009A854 (its main55 definition takes two
 * arguments).  The success path falls through to the shared
 * `return ret`, as in retail. */
u8 func_8008BD50(u8 actor, u8 col, u8 row, u8 mode) {
    u16 target;
    u8 item;
    u8 ret;

    target = *(u16*)(D_800D2204 + D_800D2CE0[row * 2 + col] * 16);
    ret = 0;
    if (mode == 0) {
        item = D_800D2CE0[row * 2 + col];
    } else {
        item = D_800C3D70[row * 2 + col];
    }
    if (item != 0) {
        if (mode == 0) {
            func_8008BC40(1);
            D_800D2D28[0xC6] = 1;
            if (func_80085084(target, actor, 0)) {
                ret = 1;
            } else {
                D_800D2D28[0xC6] = 0;
                func_8008BC98(actor);
                return ret;
            }
        } else {
            if (func_8009A7E4(item - 0x32)) {
                func_8009A854(item);
                if (--D_800D3688[row * 2 + col] == 0) {
                    D_800C3D70[row * 2 + col] = 0;
                }
                ret = 2;
            }
            return ret;
        }
    } else {
        func_8008AA74(0x4F);
    }
    return ret;
}
INCLUDE_ASM("asm/battle/nonmatchings/main47", func_8008BED8);
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
extern void func_8008FA60(u32 v);
extern void func_800716D8(void);
extern void func_8007765C(void);
extern void func_80077980(void);


/* func_8008C360.s */
void func_8008C360(u8 a) {
    u8* p;
    u8* q;

    p = D_800D2D28;
    p[0x9E] = 0;
    p[0x9D] = 0;
    p[0x9C] = 0;
    q = D_800D2D28;
    q[0xB7] = 0;
    if (a != 0) {
        D_800D2D28[0xC6] = 0;
        func_8008FA60(0);
        func_8008FA60(1);
        func_800716D8();
        func_8007765C();
        func_80077980();
    } else {
        q = D_800D2D28;
        q[0xB1] = 0;
        q[0xB0] = 0;
    }
}
/* func_8008C3F0.s */
void func_8008C3F0(u8 index) {
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
    q[0xB7] = 3;
    r = D_800D2D28;
    r[0xB1] = 1;
    r[0xB0] = 1;
    v = func_80089C08(index & 0xFF);
    off = (u32)index << 6;
    v |= func_80089C08(*(u8*)(D_800C3EAC + off + 0x3C));
    func_800BC404(v & 0xFFFF);
    func_800BCD98(func_80089C08(*(u8*)(D_800C3EAC + off + 0x3C)) & 0xFFFF);
}
