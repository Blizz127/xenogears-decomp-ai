#include "common.h"
#include "battle_guest_abi.h"


#ifndef XENO_PC_PORT
void bzero(void *, s32);
s32 func_8008ABB8(s32, s32);
extern u32 D_800D2DB4;
void func_8007FEC4(void);
#else
/* The port's allocator wrapper returns a host pointer (src/battle/main44.c). */
void bzero(void *, s32);
void *func_8008ABB8(s32, s32);
#endif

#ifndef XENO_PC_PORT
s32 ArchiveDecodeAlignedSize(s32);
void ArchiveReadFileToBuffer(s32, s32, s32, s32);
void func_8008AB94();
s32 func_8008ABB8(s32, s32);
void func_8008AC50();
extern s32 D_800C3DE8;
extern u8* D_800D2D28;
void func_8007FE3C(void);
#endif

#ifndef XENO_PC_PORT

#ifndef XENO_PC_PORT
void func_8007FE3C(void) {
    s32 temp_v0;

    if (*(u8 *)((s8*)((*(void **)&D_800D2D28)) + 0x96) == 0) {
        func_8008AC50();
        func_8008AB94();
        temp_v0 = func_8008ABB8(ArchiveDecodeAlignedSize(3), 0);
        D_800C3DE8 = temp_v0;
        ArchiveReadFileToBuffer(3, temp_v0, 0, 0x80);
        func_8008AC50();
        *(u8 *)((s8*)((*(void **)&D_800D2D28)) + 0x96) = 1U;
    }
}
#endif


#endif /* !XENO_PC_PORT */
/* Native under the port (battle guest ABI): the slot at 0x800D2DB4 receives
 * the GUEST address of the block, as retail stores it, and the two halfwords
 * are written back through that guest word. */
void func_8007FEC4(void) {
    void *temp_v0;

    temp_v0 = func_8008ABB8(0x5DA4, 0);
    GSTORE(D_800D2DB4, temp_v0);
    bzero(temp_v0, 0x5DA4);
    *(s16 *)((s8*)(GGET(D_800D2DB4)) + 0x5D9C) = 0xA0;
    *(s16 *)((s8*)(GGET(D_800D2DB4)) + 0x5D9E) = 0x64;
}
#ifndef XENO_PC_PORT

INCLUDE_ASM("asm/battle/nonmatchings/main33", func_8007FF14);
#endif


#ifndef XENO_PC_PORT
extern u8* D_800D2D28;
#endif
#ifndef XENO_PC_PORT
extern u8 D_800D32A1[];
#endif
#ifndef XENO_PC_PORT
extern u32 D_800D2DB4;
#endif
#ifndef XENO_PC_PORT
extern void HeapFree(u32 p);
#else
extern void HeapFree(void* p); /* the port allocator takes the host block */
#endif


/* func_800800E8.s */
void func_800800E8(u8 index) {
    D_800D2D28[0xAD] = 0;
    D_800D2D28[0xC7] = 0;
    D_800D2D28[0xA8] = 0;
    if (D_800D32A1[((u32)index & 0xFF) << 3] == 2) {
        D_800D32A1[((u32)index & 0xFF) << 3] = 1;
    }
    HeapFree(D_800D2DB4);
}
