#include "common.h"


#ifndef XENO_PC_PORT
extern u8* D_800D2F5C;
#endif
#ifndef XENO_PC_PORT
extern u32 D_800CCB34;
#endif
extern s32 func_8002675C(u8*, s32, u8*, u32, s32, s32, s32);
/* func_80076A10.s: draw sprite-table entry `index` of D_800D2F5C into the
 * POLY_FT4 run at `polys` (slot D_800CCB34) at (x, y), scale 0x1000.  The
 * first argument is an entry index, not a pointer: func_8002675C forwards it
 * as its `index` (see main37.c / main40.c callers). */
s32 func_80076A10(s32 index, u8* polys, s16 x, s16 y) {
    return func_8002675C(D_800D2F5C, index, polys, D_800CCB34, x, y, 0x1000);
}
/* func_80076A6C.s: as func_80076A10 with scale 0x800. */
s32 func_80076A6C(s32 index, u8* polys, s16 x, s16 y) {
    return func_8002675C(D_800D2F5C, index, polys, D_800CCB34, x, y, 0x800);
}


extern void SetSemiTrans(void* p, s32 v);
extern void SetShadeTex(void* p, s32 v);


/* func_80076AC8.s */
void func_80076AC8(u8* p) {
    SetSemiTrans(p, 1);
    SetShadeTex(p, 0);
}
