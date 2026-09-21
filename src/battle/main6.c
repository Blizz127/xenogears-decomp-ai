#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main6", func_80076C78);
INCLUDE_ASM("asm/battle/nonmatchings/main6", func_80076CE8);
INCLUDE_ASM("asm/battle/nonmatchings/main6", func_80076D58);
INCLUDE_ASM("asm/battle/nonmatchings/main6", func_80076EA4);
INCLUDE_ASM("asm/battle/nonmatchings/main6", func_80077074);
INCLUDE_ASM("asm/battle/nonmatchings/main6", func_80077364);
INCLUDE_ASM("asm/battle/nonmatchings/main6", func_80077454);
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


/* func_80077610.s */
void func_80077610(void) {
    void* p = func_8008ABB8(0x670, 0);

    *(u32*)((u8*)D_800C3EA4 + 0xA230) = (u32)p;
    bzero(p, 0x670);
    func_80077074();
}
/* func_8007765C.s */
void func_8007765C(void) {
    func_800716D8();
    HeapFree(*(u32*)((u8*)D_800C3EA4 + 0xA230));
}
