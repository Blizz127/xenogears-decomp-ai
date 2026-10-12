#include "common.h"


#ifndef XENO_PC_PORT
#include "psyq/libgte.h"
#include "battle/gte_inline.h"
extern s16 D_800C354C[];
extern s16 D_800C354E[];
extern s16 D_800C3550[];

/* Per-tick shake fade of s (+0x38 ticks left, +0x3C total): the current
 * offset +0x1C..+0x20 steps from the start +0x24..+0x28 toward the target
 * +0x2C..+0x30 (delta * left / 2 through GTE GPF, divided by total / 2) and
 * is the target once done.  It is published to D_800C354C/E/D_800C3550,
 * negated on alternate pairs of ticks (+0x34 counter).  A finished zero
 * offset runs the +0xC callback. */
void func_800B3358(u8* s) {
    VECTOR v;

    if (*(s32*)(s + 0x38) == 0) {
        *(u16*)(s + 0x1C) = *(u16*)(s + 0x2C);
        *(u16*)(s + 0x1E) = *(u16*)(s + 0x2E);
        *(u16*)(s + 0x20) = *(u16*)(s + 0x30);
    } else {
        *(s32*)(s + 0x38) -= 1;
        v.vx = *(s16*)(s + 0x2C) - *(s16*)(s + 0x24);
        v.vy = *(s16*)(s + 0x2E) - *(s16*)(s + 0x26);
        v.vz = *(s16*)(s + 0x30) - *(s16*)(s + 0x28);
        BATTLE_GTE_LDDP(*(s32*)(s + 0x38) >> 1);
        BATTLE_GTE_LDLVL(&v);
        BATTLE_GTE_GPF0();
        BATTLE_GTE_STLVL(&v);
        v.vx /= *(s32*)(s + 0x3C) >> 1;
        v.vy /= *(s32*)(s + 0x3C) >> 1;
        v.vz /= *(s32*)(s + 0x3C) >> 1;
        *(u16*)(s + 0x1C) = *(u16*)(s + 0x2C) - v.vx;
        *(u16*)(s + 0x1E) = *(u16*)(s + 0x2E) - v.vy;
        *(u16*)(s + 0x20) = *(u16*)(s + 0x30) - v.vz;
    }
    *(s32*)(s + 0x34) += 1;
    if (*(s32*)(s + 0x34) & 2) {
        D_800C354C[0] = -*(u16*)(s + 0x1C);
    } else {
        D_800C354C[0] = *(u16*)(s + 0x1C);
    }
    if (*(s32*)(s + 0x34) & 2) {
        D_800C354E[0] = -*(u16*)(s + 0x1E);
    } else {
        D_800C354E[0] = *(u16*)(s + 0x1E);
    }
    if (*(s32*)(s + 0x34) & 2) {
        D_800C3550[0] = -*(u16*)(s + 0x20);
    } else {
        D_800C3550[0] = *(u16*)(s + 0x20);
    }
    if (*(s32*)(s + 0x1C) == 0 && *(s16*)(s + 0x20) == 0 && *(s32*)(s + 0x38) == 0) {
        (*(void (**)(u8*))(s + 0xC))(s);
    }
}
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


/* func_800B3588.s */
extern void TimerWorkListRemoveTask(u32 p);
extern void HeapFree(u32 p);
#ifndef XENO_PC_PORT
extern u32 D_800C3548[];
#endif
void func_800B3588(u8* p) {
    TimerWorkListRemoveTask((u32)p);
    HeapFree((u32)p);
    D_800C3548[0] = 0;
}
/* func_800B35C0.s */
u8* func_800B35C0(void) {
    u8* s0;

    if (D_800C3548[0] == 0) {
        s0 = TimerWorkListAllocateTask(0, 0x24);
        TimerWorkListSetTaskCallback(s0, func_800B3358);
        WorkListTaskSetOnFreeCallback(s0, func_800B3588);
        *(u16*)(s0 + 0x24) = 0;
        *(u16*)(s0 + 0x26) = 0;
        *(u16*)(s0 + 0x28) = 0;
        D_800C3548[0] = (u32)s0;
    } else {
        s0 = (u8*)(u32)D_800C3548[0];
        *(u16*)(s0 + 0x24) = *(u16*)(s0 + 0x1C);
        *(u16*)(s0 + 0x26) = *(u16*)(s0 + 0x1E);
        *(u16*)(s0 + 0x28) = *(u16*)(s0 + 0x20);
    }
    return s0;
}
