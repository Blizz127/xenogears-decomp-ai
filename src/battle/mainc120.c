#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/mainc120", func_800BDD3C);
INCLUDE_ASM("asm/battle/nonmatchings/mainc120", func_800BDE58);
INCLUDE_ASM("asm/battle/nonmatchings/mainc120", func_800BDF1C);
#endif


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
extern u8 D_800C3EB0[];
extern u32 WorkListsAddTasks(u32 a0, u32 a1, void* a2, void* a3, void* a4);
extern void func_800B7424(u32 p);
extern void func_800B7364(void);
extern void func_800B6F0C(void);
extern void func_800B7134(void);
extern void WorkListSetTaskCallback(void* pTask, void* callback);
extern void D_80025A88(void);
extern u8 D_800D3420[];
extern u32 D_800C3CE8[];
extern void WorkListTaskSetOnFreeCallback(void* pTask, void* callback);
extern void func_800B3358(u8* p);
extern void func_800B3588(u8* p);
extern u32 D_800C3548[];


/* func_800BE0DC.s */
extern u32 D_800D2D68[];
extern void func_800BDCF8(u8* p);
void func_800BE0DC(void) {
    u8* p = (u8*)(u32)D_800D2D68[0];

    if (p != 0) {
        func_800BDCF8(p);
    }
}
/* func_800BE108.s */
extern u32 D_800D2D68[];
extern u32 D_800C374C[];
void func_800BE108(void) {
    D_800D2D68[0] = 0;
    D_800C374C[0] = 0;
}
