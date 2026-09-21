#include "common.h"


#ifndef XENO_PC_PORT
extern u8* D_800D2F5C;
#endif
#ifndef XENO_PC_PORT
extern u32 D_800CCB34;
#endif
extern s32 func_8002675C(u8*, u8*, u8*, u32, s32, s32, s32);
/* func_80076A10.s: as func_80076A6C with scale 0x1000. */
s32 func_80076A10(u8* a0, u8* a1, s16 a2, s16 a3) {
    return func_8002675C(D_800D2F5C, a0, a1, D_800CCB34, a2, a3, 0x1000);
}
/* func_80076A6C.s */
s32 func_80076A6C(u8* a0, u8* a1, s16 a2, s16 a3) {
    return func_8002675C(D_800D2F5C, a0, a1, D_800CCB34, a2, a3, 0x800);
}


extern void SetSemiTrans(void* p, s32 v);
extern void SetShadeTex(void* p, s32 v);


/* func_80076AC8.s */
void func_80076AC8(u8* p) {
    SetSemiTrans(p, 1);
    SetShadeTex(p, 0);
}
