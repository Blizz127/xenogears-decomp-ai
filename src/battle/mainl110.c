#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/mainl110", func_800B9C00);
INCLUDE_ASM("asm/battle/nonmatchings/mainl110", func_800B9C78);
INCLUDE_ASM("asm/battle/nonmatchings/mainl110", func_800B9F78);
INCLUDE_ASM("asm/battle/nonmatchings/mainl110", func_800BA4E0);
INCLUDE_ASM("asm/battle/nonmatchings/mainl110", func_800BA59C);
INCLUDE_ASM("asm/battle/nonmatchings/mainl110", func_800BA614);
INCLUDE_ASM("asm/battle/nonmatchings/mainl110", func_800BA768);
#endif


typedef struct { long vx, vy; long vz, pad; } VECTOR;
typedef struct { short vx, vy; short vz, pad; } SVECTOR;
extern s32 func_800A5914(SVECTOR* v, u32 arg1, s32 arg2);
extern s32 func_800A579C(SVECTOR* v);
extern void func_800A5870(SVECTOR* v, s32 r, VECTOR* out);


/* func_800BA8F4.s */
void func_800BA8F4(u8* s) {
    SVECTOR v;
    VECTOR out;
    s32 r;
    s32 t;

    t = *(s16*)(s + 2);
    *(s16*)((u8*)&v + 0) = (s16)t;
    t = *(s16*)(s + 6);
    *(s16*)((u8*)&v + 2) = (s16)t;
    t = *(s16*)(s + 0xA);
    *(s16*)((u8*)&v + 4) = (s16)t;
    r = func_800A5914(&v, *(u32*)(s + 0x78), 4);
    if (r < 0) {
        r = func_800A579C(&v);
    }
    func_800A5870(&v, r, &out);
    *(u16*)(s + 0x84) = *(u16*)((u8*)&v + 2);
    *(u32*)(s + 0x78) = (u32)r;
}
