#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/mainc122", func_800BE1C4);
INCLUDE_ASM("asm/battle/nonmatchings/mainc122", func_800BE330);
INCLUDE_ASM("asm/battle/nonmatchings/mainc122", func_800BE538);
INCLUDE_ASM("asm/battle/nonmatchings/mainc122", func_800BE6A0);
INCLUDE_ASM("asm/battle/nonmatchings/mainc122", func_800BE6E8);
INCLUDE_ASM("asm/battle/nonmatchings/mainc122", func_800BE790);
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


/* func_800BEB04.s */
extern u8 D_800591B2[];
extern u8 D_800591B3[];
extern u8 D_800591B0[];
extern void func_800B8354(void);
extern void ArchiveGetArchiveOffsetIndices(u32* a, u32* b);
extern u32 ArchiveSetIndex(u32 dir, u32 entry);
extern void ArchiveReadFileToBuffer(u32 a0, void* buf, u32 a2, u32 a3);
extern void DrawSync(u32 v);
extern void Vsync(u32 v);
extern void EnterCriticalSection(void);
extern void FlushCache(void);
extern void ExitCriticalSection(void);
void func_800BEB04(void) {
    u8 cur = D_800591B2[0];
    u8 nxt = D_800591B3[0];
    u32 a, b;

    if (cur != nxt) {
        D_800591B2[0] = nxt;
        func_800B8354();
        ArchiveGetArchiveOffsetIndices(&a, &b);
        ArchiveSetIndex(0xC, 2);
        ArchiveReadFileToBuffer((u32)nxt + 2, (void*)0x801FC000u, 0, 0x80);
        func_800B8354();
        ArchiveSetIndex(a, b);
        DrawSync(0);
        Vsync(0);
        EnterCriticalSection();
        FlushCache();
        ExitCriticalSection();
    }
    D_800591B0[0] = 1;
}
/* func_800BEBC4.s */
#ifndef XENO_PC_PORT
extern u8 D_800C3EB0[];
#endif
extern u16 D_80059494[];
extern void func_800BEC18(void);
extern void Vsync(u32 v);
void func_800BEBC4(void) {
    u8* base;
    u8* q;

    func_800BEC18();
    base = (u8*)(u32)D_800C3EB0;
    q = base + 0x8000;
    if ((*(u16*)(q + 0xC58) & 0x100) != 0) {
        Vsync(8);
        D_80059494[0] = 0;
    }
}
