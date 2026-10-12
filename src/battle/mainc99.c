#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/mainc99", func_800B7424);
#include "psyq/libgpu.h"
#include "battle/spad_stack.h"
typedef struct {
    DRAWENV draw;
    DISPENV disp;
    u32 ot[0x1000];
} B7870Buf; /* 0x4070 */
typedef struct {
    u8 pad0[0xB70];
    B7870Buf db[2];
    B7870Buf* cur;
    u32* ot;
    u8 pad8C58[0x2C];
    s32 idx;
} B7870Glob;
extern u8 D_800C3EB0__b[] asm("D_800C3EB0");
static inline B7870Glob* B7870_Glob(void) {
    return (B7870Glob*)D_800C3EB0__b;
}
extern u32 D_800C3CB4__p[] asm("D_800C3CB4");
extern void WorkListsReset(void);
extern s32 ArchiveDataSync(void);
extern void ArchiveCdDataSync(s32 mode);
extern void GameCheckAndHandleSoftReset(void);
extern void func_8001BB0C(void);
extern void func_801E5840(u8 a0);
extern u8* func_800B73EC(void);
extern void func_800B6F0C__t(u8* task) asm("func_800B6F0C");
extern void func_800B7160(u8* task);
extern void func_800B7330(u8* p);
extern s32 func_80021AD8(s32 color, s32 value);
extern void Vsync(s32 mode);

/* Screen transition: copy the 320x224 front buffer to VRAM (0x2C0, 0x100)
 * with every pixel's STP bit set, flip and clear the double buffer once,
 * give both draw environments a white background, then run the transition
 * task (func_800B73EC) every frame (Vsync(2)) while the background colour
 * of db[idx] (idx at +0x8C84) fades by 0xC a step and the loader advances
 * (func_801E5840 for states 1, 3 and 4, func_8001BB0C for state 2), until
 * 0x56 frames have passed and the loader reaches state 5. */
void func_800B7870(void) {
    RECT rect;
    u16* buf;
    u16* q;
    s32 i;
    s32 wait;
    s32 state;
    s32 n;
    u8* p;
    B7870Buf* next;
    B7870Buf* first;
    B7870Glob* g;
    B7870Glob* h;
    B7870Glob* f;
    B7870Buf* db0;

    wait = 0x56;
    WorkListsReset();
    state = 1;
    n = 0;
    buf = HeapAlloc(0x30000, 1);
    q = buf;
    rect.x = 0;
    rect.y = 0;
    rect.w = 0x140;
    rect.h = 0xE0;
    StoreImage(&rect, (u_long*)buf);
    DrawSync(0);
    for (i = 0; i != 0x14000; i++) {
        *q++ |= 0x8000;
    }
    rect.x = 0x2C0;
    rect.y = 0x100;
    rect.w = 0x140;
    rect.h = 0xE0;
    LoadImage(&rect, (u_long*)buf);
    DrawSync(0);
    HeapFree(buf);
    g = B7870_Glob();
    first = &g->db[0];
    /* db0 is copied inside the compare, after the g->cur load, as retail */
    if (g->cur == (db0 = first)) {
        first = &g->db[1];
    }
    g->cur = first;
    g->ot = first->ot;
    ClearOTagR((u_long*)first->ot, 0x1000);
    g->idx = 0;
    g->cur = db0;
    g->db[0].draw.isbg = 1;
    g->db[1].draw.isbg = 1;
    g->db[0].draw.r0 = 0xFF;
    g->db[1].draw.r0 = 0xFF;
    g->db[0].draw.g0 = 0xFF;
    g->db[1].draw.g0 = 0xFF;
    g->db[0].draw.b0 = 0xFF;
    g->db[1].draw.b0 = 0xFF;
    p = func_800B73EC();
    while (wait != 0 || state != 5) {
        if (wait > 0) {
            wait--;
        }
        h = B7870_Glob();
        next = &h->db[0];
        if (h->cur == next) {
            next = &h->db[1];
        }
        h->cur = next;
        h->ot = next->ot;
        ClearOTagR((u_long*)next->ot, 0x1000);
        h->idx = 1 - h->idx;
        D_800C3CB4__p[0] = (u32)h->ot;
        if (ArchiveDataSync() == 0) {
            switch (state) {
            case 0:
                break;
            case 2:
                func_8001BB0C();
                state++;
                break;
            case 1:
            case 3:
            case 4:
                func_801E5840(n);
                n++;
                state++;
                break;
            }
        }
        GameCheckAndHandleSoftReset();
        BATTLE_SPAD_STACK_SET(0x1F8003FC);
        func_800B6F0C__t(p);
        func_800B7160(p);
        BATTLE_SPAD_STACK_RESET();
        DrawSync(0);
        Vsync(2);
        f = B7870_Glob();
        f->db[f->idx].draw.r0 = func_80021AD8(f->db[f->idx].draw.r0, -0xC);
        f->db[f->idx].draw.g0 = func_80021AD8(f->db[f->idx].draw.g0, -0xC);
        f->db[f->idx].draw.b0 = func_80021AD8(f->db[f->idx].draw.b0, -0xC);
        PutDispEnv(&f->cur->disp);
        PutDrawEnv(&f->cur->draw);
        DrawOTag((u_long*)&f->cur->ot[0xFFF]);
    }
    func_800B7330(p);
    SetDispMask(0);
    ArchiveCdDataSync(0);
    func_801E5840(3);
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


/* func_800B7C28.s */
#ifndef XENO_PC_PORT
extern u8 D_800D2FDC[];
#endif
void func_800B7C28(void) {
    D_800D2FDC[0] = 0;
}
