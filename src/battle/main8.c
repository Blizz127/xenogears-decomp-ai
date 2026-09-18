#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main8", func_80077990);
INCLUDE_ASM("asm/battle/nonmatchings/main8", func_800780A8);
INCLUDE_ASM("asm/battle/nonmatchings/main8", func_8007819C);
INCLUDE_ASM("asm/battle/nonmatchings/main8", func_80078310);
INCLUDE_ASM("asm/battle/nonmatchings/main8", func_80078508);
INCLUDE_ASM("asm/battle/nonmatchings/main8", func_800785D4);
INCLUDE_ASM("asm/battle/nonmatchings/main8", func_80078658);
INCLUDE_ASM("asm/battle/nonmatchings/main8", func_800787E0);
#endif
#ifndef XENO_PC_PORT
extern u8 D_800C3E8C;
#endif
#ifndef XENO_PC_PORT
extern u8* D_800C3EAC;
#endif
#ifndef XENO_PC_PORT
extern u8 D_800C400B[];
#endif
#ifndef XENO_PC_PORT
extern u8 D_800C402F[];
#endif
#ifndef XENO_PC_PORT
extern u8 D_800C4022[];
#endif
/* func_8007887C.s: append one 72-byte entry at the D_800C3EAC+0x2DA cursor --
 * colour a0, marker 0xF8 and the halfword D_800C3E8C - 1 -- then bump the
 * cursor.  Skipped entirely while D_800C3E8C is 0. */
void func_8007887C(u8 a0) {
    if (D_800C3E8C != 0) {
        u8* p;

        D_800C400B[D_800C3EAC[0x2DA] * 72] = a0;
        D_800C402F[D_800C3EAC[0x2DA] * 72] = 0xF8;
        p = D_800C3EAC;
        *(u16*)(D_800C4022 + p[0x2DA] * 72) = D_800C3E8C - 1;
        p[0x2DA] = p[0x2DA] + 1;
    }
}


#ifndef XENO_PC_PORT
extern u8 D_800D2E5F[];
#endif
extern void func_80078658(u8 a, u8 b);
extern void func_800787E0(u8 a, u8 b);
extern void func_8007887C(u8 a);


/* func_8007893C.s */
void func_8007893C(u8 a, u8 b) {
    if (*(u8*)(D_800D2E5F + ((u32)a << 3)) == 0) {
        return;
    }
    func_80078658(a & 0xFF, b & 0xFF);
    func_800787E0(0x1E, b & 0xFF);
    func_8007887C(b & 0xFF);
}
