#include "common.h"


#ifndef XENO_PC_PORT
extern u8 D_800CCE34[];
#endif

/* func_8007BAE8.s: slot = (a1 & 0xFF) + 3; store (u32)(p[1] | (p[2] << 8))
 * (p = *a0) to D_800CCE34 + slot * 368. Retail leaves the offset in v0,
 * but the only caller (main31 func_8007EF6C) ignores it, so void like the
 * sibling slot stores below. */
/* Not yet byte-matching: the matching build assembles retail bytes for
 * this function from its own asm segment (see config/battle.yaml).  The
 * body below is kept for the port and the differential harnesses. */
#ifdef XENO_PC_PORT
void func_8007BAE8(u8** a0, u8 a1) {
    u8* p = *a0;
    u32 slot = ((u32)a1 & 0xFFu) + 3;
    u32 off = slot * 368;

    *(u32*)(D_800CCE34 + off) = (u32)p[1] | ((u32)p[2] << 8);
}
#endif /* XENO_PC_PORT */


#ifndef XENO_PC_PORT
extern u8 D_800CCE3D[];
#endif
#ifndef XENO_PC_PORT
extern u8 D_800CCE3B[];
#endif
#ifndef XENO_PC_PORT
extern u8 D_800CCE39[];
#endif
#ifndef XENO_PC_PORT
extern u8 D_800CCE3C[];
#endif
#ifndef XENO_PC_PORT
extern u8 D_800CCE3A[];
#endif
#ifndef XENO_PC_PORT
extern u8 D_800CCE38[];
#endif
#ifndef XENO_PC_PORT
extern u8 D_800CCE3E[];
#endif


/* func_8007BB2C.s */
void func_8007BB2C(u8** ppBoard, u8 index) {
    u8* p = *ppBoard;
    u16 v = (u16)(p[1] | (p[2] << 8));
    u32 slot = index + 3;

    *(u16*)(D_800CCE3E + slot * 368) = v;
}
/* func_8007BB70.s */
void func_8007BB70(u8** ppBoard, u8 index) {
    u32 slot = index + 3;
    u32 off = slot * 368;

    D_800CCE3D[off] = (*ppBoard)[1];
    D_800CCE3B[off] = (*ppBoard)[2];
    D_800CCE39[off] = (*ppBoard)[3];
}
/* func_8007BBD8.s */
void func_8007BBD8(u8** ppBoard, u8 index) {
    u32 slot = index + 3;
    u32 off = slot * 368;

    D_800CCE3C[off] = (*ppBoard)[1];
    D_800CCE3A[off] = (*ppBoard)[2];
    D_800CCE38[off] = (*ppBoard)[3];
}
/* func_8007BC40.s */
void func_8007BC40(u8** ppBoard, u8* dst, u8 index) {
    u8* p = *ppBoard;
    u32 off = ((u32)index & 0xFF) << 3;

    dst[off + p[1]] = p[2];
    p = *ppBoard;
    dst += off + p[1];
    dst[1] = p[3];
}
