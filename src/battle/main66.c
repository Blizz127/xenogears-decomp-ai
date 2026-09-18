#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main66", func_800A22E8);
INCLUDE_ASM("asm/battle/nonmatchings/main66", func_800A2330);
INCLUDE_ASM("asm/battle/nonmatchings/main66", func_800A23E8);
INCLUDE_ASM("asm/battle/nonmatchings/main66", func_800A2434);
INCLUDE_ASM("asm/battle/nonmatchings/main66", func_800A2704);
INCLUDE_ASM("asm/battle/nonmatchings/main66", func_800A2ACC);
INCLUDE_ASM("asm/battle/nonmatchings/main66", func_800A2BB8);
#endif
extern void HeapChangeCurrentUser(u32 user, u32 arg);
extern void* HeapAlloc(u32 size, u32 flags);
extern void func_800A2D5C(u8* p);
/* func_800A2CA4.s: allocate (n + 1) 124-byte entries for the list at p
 * (count at +4, cursor +6 zeroed, buffer at +0) and initialise it with
 * func_800A2D5C; NULL when the allocation fails. */
u8* func_800A2CA4(u8* p, s32 n) {
    u8* r;

    HeapChangeCurrentUser(4, 0);
    *(s16*)(p + 4) = n;
    *(s16*)(p + 6) = 0;
    *(u8**)(p + 0) = HeapAlloc((n + 1) * 124, 0);
    if (*(u8**)(p + 0) == NULL) {
        r = NULL;
    } else {
        func_800A2D5C(p);
        r = p;
    }
    return r;
}


extern void HeapFree(u32 p);


/* func_800A2D1C.s */
void func_800A2D1C(u8* p) {
    u32 q = *(u32*)p;

    *(u16*)(p + 4) = 0;
    *(u16*)(p + 6) = 0;
    if (q != 0) {
        HeapFree(q);
    }
    *(u32*)p = 0;
}
