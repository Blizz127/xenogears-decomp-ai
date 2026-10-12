#include "common.h"


#ifndef XENO_PC_PORT
extern u8* func_800B35C0(void);
extern void func_800B3358(u8* p);

void func_800B3658(s16* pos, s32 v) {
    u8* p = func_800B35C0();

    *(s16*)(p + 0x2C) = pos[0];
    *(s16*)(p + 0x2E) = pos[1];
    *(s16*)(p + 0x30) = pos[2];
    *(s32*)(p + 0x38) = v * 2;
    *(s32*)(p + 0x3C) = v * 2;
    func_800B3358(p);
}

#include "psyq/libgte.h"
#include "battle/gte_inline.h"
extern u32 D_800C3558[];

/* Per-tick colour fade of fx (+0x38 total ticks, +0x3C ticks left): while
 * ticks remain, the current colour +0x48..+0x4A steps from the start colour
 * +0x45..+0x47 toward the target +0x42..+0x44 (delta * left / 2 through GTE
 * GPF, divided by total / 2).  Once done, the colour is the target; a black
 * target also calls the D_800C3558 object's +0xC handler with that object. */
void func_800B36BC(u8* fx) {
    u8* w;
    VECTOR v;

    w = fx;
    if (*(s32*)(fx + 0x3C) == 0) {
        fx[0x48] = fx[0x42];
        fx[0x49] = fx[0x43];
        fx[0x4A] = fx[0x44];
        if ((fx[0x48] | fx[0x49] | fx[0x4A]) == 0) {
            (*(void (**)(u32*))(D_800C3558[0] + 0xC))((u32*)D_800C3558[0]);
        }
    } else {
        *(s32*)(w + 0x3C) -= 1;
        v.vx = w[0x42] - w[0x45];
        v.vy = w[0x43] - w[0x46];
        v.vz = w[0x44] - w[0x47];
        BATTLE_GTE_LDDP(*(s32*)(w + 0x3C) >> 1);
        BATTLE_GTE_LDLVL(&v);
        BATTLE_GTE_GPF0();
        BATTLE_GTE_STLVL(&v);
        v.vx /= *(s32*)(w + 0x38) >> 1;
        v.vy /= *(s32*)(w + 0x38) >> 1;
        v.vz /= *(s32*)(w + 0x38) >> 1;
        w[0x48] = w[0x42] - v.vx;
        w[0x49] = w[0x43] - v.vy;
        w[0x4A] = w[0x44] - v.vz;
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


/* func_800B383C.s */
#ifndef XENO_PC_PORT
extern u32 D_800C3558[];
#endif
extern void WorkListRemoveTask(u32 p);
extern void TimerWorkListRemoveTask(u32 p);
void func_800B383C(u8* p) {
    WorkListRemoveTask((u32)(p + 0x1C));
    TimerWorkListRemoveTask((u32)p);
    D_800C3558[0] = 0;
}
