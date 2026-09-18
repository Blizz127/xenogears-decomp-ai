#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/mainc131", func_800BF85C);
INCLUDE_ASM("asm/battle/nonmatchings/mainc131", func_800BF8CC);
#endif
#ifndef XENO_PC_PORT
extern s16 D_800D3678;
#endif
#ifndef XENO_PC_PORT
extern u32 D_800D363C[];
#endif
/* func_800BF954.s: index of a0 in the D_800D363C table (D_800D3678 entries);
 * returns the count when absent or when the table is empty. */
s32 func_800BF954(u32 a0) {
    s32 i = 0;
    s16 h = D_800D3678;

    if (h != 0) {
        s32 n = h;
        u32* p = D_800D363C;
        for (i = 0; i != n; i++, p++) {
            if (*p == a0) {
                break;
            }
        }
    }
    return i;
}

extern u8* TimerWorkListAllocateTask(u32 owner, u32 size);
extern void TimerWorkListSetTaskCallback(void* pTask, void* callback);
extern u32 func_800B57E4(u8* s);
extern void func_800B5B3C(u8* task);
extern void func_800B5854(void);
extern void func_800BF73C(void);
extern void func_800B5CC0(void);
extern void func_800BDC14(void);
extern void func_800BDF1C(void);
extern void func_8001E148(u32 v);
extern void func_800C08CC(u32 a0, void* a1, void* a2);
extern void func_800B51B0(void);
extern void func_800245D8(u32 a0, u32 a1);
#ifndef XENO_PC_PORT
extern u8 D_800C3EB0[];
#endif
extern u32 WorkListsAddTasks(u32 a0, u32 a1, void* a2, void* a3, void* a4);
extern void func_800B7424(u32 p);
extern void func_800B7364(void);
extern void func_800B6F0C(void);
extern void func_800B7134(void);
extern void WorkListSetTaskCallback(void* pTask, void* callback);
extern void D_80025A88(void);
#ifndef XENO_PC_PORT
extern u8 D_800D3420[];
#endif
#ifndef XENO_PC_PORT
extern u32 D_800C3CE8[];
#endif
extern void WorkListTaskSetOnFreeCallback(void* pTask, void* callback);
extern void func_800B3358(u8* p);
extern void func_800B3588(u8* p);
#ifndef XENO_PC_PORT
extern u32 D_800C3548[];
#endif


/* func_800BF998.s */
#ifndef XENO_PC_PORT
extern u16 D_800D2D4C[];
#endif
#ifndef XENO_PC_PORT
extern u16 D_800D36BC[];
#endif
extern void func_800A9FF0(u32 v);
void func_800BF998(void) {
    s16 a = (s16)(D_800D2D4C[0] + 1);

    D_800D2D4C[0] = (u16)a;
    D_800D36BC[0] = D_800D36BC[0] + 1;
    if (a == 2) {
        func_800A9FF0(0xB);
    }
}
/* func_800BF9EC.s */
#ifndef XENO_PC_PORT
extern u8 D_800C3621[];
#endif
extern u32 ArchiveSetIndex(u32 dir, u32 entry);
extern u32 ArchiveDecodeAlignedSize(u32 index);
extern u32 HeapAlloc(u32 size, u32 flag);
extern void ArchiveReadFileToBuffer(u32 a0, void* buf, u32 a2, u32 a3);
extern void func_800B8354(void);
extern void func_8002DDE4(void* a0, u32 a1, u32 a2, u32 a3, u32 s0, u32 s1, u32 s2);
extern void func_800BE790(void);
extern void HeapFree(u32 p);
void func_800BF9EC(void) {
    u8* s0;

    if (D_800C3621[0] != 0) {
        func_800B8354();
        ArchiveSetIndex(0x2C, 0);
        s0 = (u8*)(u32)HeapAlloc(ArchiveDecodeAlignedSize(1), 0);
        ArchiveReadFileToBuffer(1, s0, 0, 0x80);
        func_800B8354();
        func_8002DDE4(s0, 0, 0, 0, 0, 0, 0);
        func_800BE790();
        HeapFree((u32)s0);
        D_800C3621[0] = 0;
    }
}
