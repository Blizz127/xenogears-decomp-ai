#include "common.h"
#include "psyq/libetc.h"
#include "psyq/libgpu.h"

extern void memcpy(u8*, u8*, int);

extern char g_GraphDebugLevel;
extern void (*g_DrawSyncCallbackFn)();
extern int (*g_GpuPrintf)(char*, ...);

extern DRAWENV g_GpuDrawEnv;
extern DISPENV g_GpuDispEnv;

// D_800568C8 : Sys func ptrs

DRAWENV* SetDefDrawEnv(DRAWENV* env, int x, int y, int w, int h) {
    int nVideoMode;

    nVideoMode = GetVideoMode();
    env->clip.x = x;
    env->clip.y = y;
    env->clip.w = w;
    env->clip.h = h;
    env->tw.x = 0;
    env->tw.y = 0;
    env->tw.w = 0;
    env->tw.h = 0;
    env->r0 = 0;
    env->g0 = 0;
    env->b0 = 0;
    env->dtd = 1;
    
    if (nVideoMode != MODE_NTSC)
        env->dfe  = h < 0x121;
    else
        env->dfe  = h < 0x101;

    env->ofs[0] = x;
    env->ofs[1] = y;
    env->tpage = 0xA;
    env->isbg = 0;
    return env;
}

DISPENV* SetDefDispEnv(DISPENV* env, int x, int y, int w, int h) {
    env->disp.x = x;
    env->disp.y = y;
    env->disp.w = w;
    env->disp.h = h;
    env->screen.x = 0;
    env->screen.y = 0;
    env->screen.w = 0;
    env->screen.h = 0;
    env->isrgb24 = 0;
    env->isinter = 0;
    env->pad1 = 0;
    env->pad0 = 0;
    return env;
}

u_short GetTPage(int tp, int abr, int x, int y) {
    return getTPage(tp, abr, x, y);
}

u_short GetClut(int x, int y) {
    return getClut(x, y);
}

void DumpTPage(u_short tpage) {
    dumpTPage(tpage);
}

void DumpClut(u_short clut) {
    dumpClut(clut);
}

void* NextPrim(void *p) {
    return nextPrim(p);
}

int IsEndPrim(void *p) {
    return isendprim(p);
}

void AddPrim(void *ot, void *p) {
    addPrim(ot, p);
}

void AddPrims(void *ot, void *p0, void *p1) {
    addPrims(ot, p0, p1);
}

void CatPrim(void *p0, void *p1) {
    setaddr(p0, p1);
}

void TermPrim(void *p) {
    termPrim(p);
}

void SetSemiTrans(void *p, int abe) {
    setSemiTrans(p, abe);
}

void SetShadeTex(void *p, int tge) {
    setShadeTex(p, tge);
}

void SetPolyF3(POLY_F3 *p) {
    setlen(p, 4);
    setcode(p, 0x20);
}

void SetPolyFT3(POLY_FT3 *p) {
    setlen(p, 7);
    setcode(p, 0x24);
}

void SetPolyG3(POLY_G3 *p) {
    setlen(p, 6);
    setcode(p, 0x30);
}

void SetPolyGT3(POLY_GT3 *p) {
    setlen(p, 9);
    setcode(p, 0x34);
}

void SetPolyF4(POLY_F4 *p) {
    setlen(p, 5);
    setcode(p, 0x28);
}

void SetPolyFT4(POLY_FT4 *p) {
    setlen(p, 9);
    setcode(p, 0x2C);
}

void SetPolyG4(POLY_G4 *p) {
    setlen(p, 8);
    setcode(p, 0x38);
}

void SetPolyGT4(POLY_GT4 *p) {
    setlen(p, 12);
    setcode(p, 0x3C);
}

void SetSprt8(SPRT_8 *p) {
    setlen(p, 3);
    setcode(p, 0x74);
}

void SetSprt16(SPRT_16 *p) {
    setlen(p, 3);
    setcode(p, 0x7C);
}

void SetSprt(SPRT *p) {
    setlen(p, 4);
    setcode(p, 0x64);
}

void SetTile1(TILE_1 *p) {
    setlen(p, 2);
    setcode(p, 0x68);
}

void SetTile8(TILE_8 *p) {
    setlen(p, 2);
    setcode(p, 0x70);
}

void SetTile16(TILE_16 *p) {
    setlen(p, 2);
    setcode(p, 0x78);
}

void SetTile(TILE *p) {
    setlen(p, 3);
    setcode(p, 0x60);
}

void SetLineF2(LINE_F2 *p) {
    setlen(p, 3);
    setcode(p, 0x40);
}

void SetLineG2(LINE_G2 *p) {
    setlen(p, 4);
    setcode(p, 0x50);
}

void SetLineF3(LINE_F3 *p) {
    setlen(p, 5);
    setcode(p, 0x48);
    p->pad = 0x55555555;
}

void SetLineG3(LINE_G3 *p) {
    setlen(p, 7);
    setcode(p, 0x58);
    p->pad = 0x55555555;
}

void SetLineF4(LINE_F4 *p) {
    setlen(p, 6);
    setcode(p, 0x4c);
    p->pad = 0x55555555;
}

void SetLineG4(LINE_G4 *p) {
    setlen(p, 9);
    setcode(p, 0x5c);
    p->pad = 0x55555555;
}

void SetDrawTPage(DR_TPAGE *p, int dfe, int dtd, int tpage) {
    setlen(p, 1);
    ((u_long *)(p))[1] = _get_mode(dfe, dtd, tpage);
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", SetDrawMove);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", func_80043EAC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", func_80043F18);

void DumpDrawEnv(DRAWENV *env) {
    g_GpuPrintf("clip (%3d,%3d)-(%d,%d)\n", env->clip.x, env->clip.y, env->clip.w, env->clip.h);
    g_GpuPrintf("ofs  (%3d,%3d)\n", env->ofs[0], env->ofs[1]);
    g_GpuPrintf("tw   (%d,%d)-(%d,%d)\n", env->tw.x, env->tw.y, env->tw.w, env->tw.h);
    g_GpuPrintf("dtd   %d\n", env->dtd);
    g_GpuPrintf("dfe   %d\n", env->dfe);
    dumpTPage(env->tpage);
}

void DumpDispEnv(DISPENV *env) {
    g_GpuPrintf("disp   (%3d,%3d)-(%d,%d)\n", env->disp.x, env->disp.y, env->disp.w, env->disp.h);
    g_GpuPrintf("screen (%3d,%3d)-(%d,%d)\n", env->screen.x, env->screen.y, env->screen.w, env->screen.h);
    g_GpuPrintf("isinter %d\n", env->isinter);
    g_GpuPrintf("isrgb24 %d\n", env->isrgb24);
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", ResetGraph);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", SetGraphReverse);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", SetGraphDebug);
/*
int SetGraphDebug(int level) {
    int nPrev = g_GraphDebugLevel;

    g_GraphDebugLevel = level;
    if (level) {
        g_GpuPrintf("SetGraphDebug:level:%d,type:%d reverse:%d\n", level, DAT_800568d0, DAT_800568d3);
    }

    return nPrev;
}
*/

// SetGrapQue ?
extern u8 D_800568D1;
extern void* D_800568C8;
extern char D_80019104[];
extern void DMACallback(s32, void*);

u8 func_8004440C(u8 arg0) {
    u8 old = D_800568D1;
    if (g_GraphDebugLevel >= 2) {
        g_GpuPrintf(D_80019104, arg0);
    }
    if (arg0 != D_800568D1) {
        void (*pFunc)(s32) = *(void (**)(s32))((u8*)D_800568C8 + 0x34);
        pFunc(1);
        DMACallback(2, NULL);
        D_800568D1 = arg0;
    }
    return old;
}

extern u8 D_800568D0;

u8 func_800444B8(void) {
    return D_800568D0;
}

int GetGraphDebug(void) {
    return g_GraphDebugLevel;
}

extern char D_80019118; // "DrawSyncCallback(%08x)...\n"
u_long DrawSyncCallback(void (*pCallbackFn)()) {
    void (*pPrevCallbackFn)();

    if (g_GraphDebugLevel >= 2)
        g_GpuPrintf(&D_80019118, pCallbackFn);

    pPrevCallbackFn = g_DrawSyncCallbackFn;
    g_DrawSyncCallbackFn = pCallbackFn;
    return (u_long) pPrevCallbackFn;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", SetDispMask);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", DrawSync);
/*
int DrawSync(int mode) {
    if (g_GraphDebugLevel >= 2)
        g_GpuPrintf("DrawSync(%d)...\n", mode);

    return func_80046DB4(mode);
}
*/

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", func_8004463C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", ClearImage);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", ClearImage2);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", LoadImage);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", StoreImage);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", MoveImage);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", ClearOTag);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", ClearOTagR);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", DrawPrim);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", DrawOTag);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", PutDrawEnv);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", DrawOTagEnv);

DRAWENV* GetDrawEnv(DRAWENV* env) {
    memcpy(env, &g_GpuDrawEnv, sizeof(DRAWENV));
    return env;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", PutDispEnv);

DISPENV* GetDispEnv(DISPENV* env) {
    memcpy(env, &g_GpuDispEnv, sizeof(DISPENV));
    return env;
}
