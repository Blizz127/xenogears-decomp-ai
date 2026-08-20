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

void SetDrawMove(DR_MOVE* move, RECT* rect, int x, int y) {
    register u8* out asm("$8") = (u8*)move;
    register s32 code asm("$4") = 5;
    u32 rectPos;

    if (rect->w == 0 || rect->h == 0) {
        code = 0;
    }
    *(u32*)(out + 4) = 0x01000000;
    *(u32*)(out + 8) = 0x80000000;
    out[3] = code;
    rectPos = *(u32*)((u8*)rect + 0);
    *(u32*)(out + 0x10) = (y << 16) | (x & 0xFFFF);
    *(u32*)(out + 0x0C) = rectPos;
    *(u32*)(out + 0x14) = *(u32*)((u8*)rect + 4);
}

void func_80043EAC(void* pPrim, void* pRect) {
    s32 w = *(s16*)((u8*)pRect + 4);
    s32 h = *(s16*)((u8*)pRect + 6);
    s32 area = (w * h + 1) / 2;
    u8 tagCount;
    if (area < 13) {
        tagCount = (u8)(area + 4);
    } else {
        tagCount = 0;
    }
    *(u8*)((u8*)pPrim + 3) = tagCount;
    *(u32*)((u8*)pPrim + 4) = (area < 13) ? (u32)area : 0xA0000000;
    *(u32*)((u8*)pPrim + 8) = *(u32*)pRect;
    *(u32*)((u8*)pPrim + 0xC) = *(u32*)((u8*)pRect + 4);
    *(u32*)((u8*)pPrim + tagCount * 4) = 0x1000000;
}

s32 func_80043F18(void* pOt1, void* pOt2) {
    s32 total = *(u8*)((u8*)pOt1 + 3) + *(u8*)((u8*)pOt2 + 3) + 1;
    if (total < 0x11) {
        *(u8*)((u8*)pOt1 + 3) = (u8)total;
        *(u32*)pOt2 = 0;
        return 0;
    }
    return -1;
}

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

int SetGraphDebug(int level) {
    register u8* state asm("$2") = (u8*)&g_GraphDebugLevel;
    register int previous asm("$16");
    u8 newLevel = level;

    previous = state[0];
    state[0] = level;
    if (newLevel != 0) {
        extern u8 D_800568D0;
        extern u8 D_800568D3;
        extern char D_800190D8[];
        g_GpuPrintf(D_800190D8, newLevel, D_800568D0, D_800568D3);
    }

    return previous;
}

// SetGrapQue ?
extern u8 D_800568D1;
extern void* D_800568C8;
extern char D_80019104[];
extern char D_80019134[];
extern char D_80019148[];
extern char D_80019180[];
extern char D_8001918C[];
extern char D_80019198[];
extern char D_800191C8[];
extern char D_800191E0[];
extern char D_8005698C[];
extern void DMACallback(s32, void*);
extern void func_80047178(void*, s32, s32);
extern void func_8004463C(char*, RECT*);

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

void SetDispMask(int mask) {
    u8* state = (u8*)&g_GraphDebugLevel;
    void* dispEnv;
    void* dispatch;
    void (*setMask)(u32);
    u32 command;

    if (state[0] >= 2) {
        g_GpuPrintf(D_80019134, mask);
    }
    dispEnv = state + 0x6A;
    if (mask == 0) {
        func_80047178(dispEnv, -1, 0x14);
    }

    command = 0x03000001;
    dispatch = D_800568C8;
    if (mask != 0) {
        command = 0x03000000;
    }
    setMask = *(void (**)(u32))((u8*)dispatch + 0x10);
    setMask(command);
}

int DrawSync(int mode) {
    int (*sync)(int);

    if ((u8)g_GraphDebugLevel >= 2) {
        g_GpuPrintf(D_80019148, mode);
    }

    sync = *(int (**)(int))((u8*)D_800568C8 + 0x3C);
    return sync(mode);
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", func_8004463C);

int ClearImage(RECT* rect, u_char r, u_char g, u_char b) {
    void* dispatch;
    int (*clear)(void*, RECT*, s32, u32);
    u32 color;

    func_8004463C(D_80019180, rect);
    color = ((b & 0xFF) << 16) | ((g & 0xFF) << 8) | (r & 0xFF);
    dispatch = D_800568C8;
    clear = *(int (**)(void*, RECT*, s32, u32))((u8*)dispatch + 8);
    return clear(*(void**)((u8*)dispatch + 0x0C), rect, 8, color);
}

int ClearImage2(RECT* rect, u_char r, u_char g, u_char b) {
    void* dispatch;
    int (*clear)(void*, RECT*, s32, u32);
    u32 color;

    func_8004463C(D_80019180, rect);
    color = 0x80000000 | ((b & 0xFF) << 16) | ((g & 0xFF) << 8) |
            (r & 0xFF);
    dispatch = D_800568C8;
    clear = *(int (**)(void*, RECT*, s32, u32))((u8*)dispatch + 8);
    return clear(*(void**)((u8*)dispatch + 0x0C), rect, 8, color);
}

int LoadImage(RECT* rect, u_long* data) {
    void* dispatch;
    int (*load)(void*, RECT*, s32, u_long*);

    func_8004463C(D_8001918C, rect);
    dispatch = D_800568C8;
    load = *(int (**)(void*, RECT*, s32, u_long*))((u8*)dispatch + 8);
    return load(*(void**)((u8*)dispatch + 0x20), rect, 8, data);
}

int StoreImage(RECT* rect, u_long* data) {
    void* dispatch;
    int (*store)(void*, RECT*, s32, u_long*);

    func_8004463C(D_80019198, rect);
    dispatch = D_800568C8;
    store = *(int (**)(void*, RECT*, s32, u_long*))((u8*)dispatch + 8);
    return store(*(void**)((u8*)dispatch + 0x1C), rect, 8, data);
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", MoveImage);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgpu", ClearOTag);

u_long* ClearOTagR(u_long* ot, int count) {
    void (*clear)(u_long*, int);

    if ((u8)g_GraphDebugLevel >= 2) {
        g_GpuPrintf(D_800191C8, ot, count);
    }

    clear = *(void (**)(u_long*, int))((u8*)D_800568C8 + 0x2C);
    clear(ot, count);
    *ot = (u_long)D_8005698C & 0xFFFFFF;
    return ot;
}

void DrawPrim(void* prim) {
    void (*sync)(s32);
    void (*send)(void*, s32);
    s32 length;

    sync = *(void (**)(s32))((u8*)D_800568C8 + 0x3C);
    length = *((u8*)prim + 3);
    sync(0);
    send = *(void (**)(void*, s32))((u8*)D_800568C8 + 0x14);
    send((u8*)prim + 4, length);
}

void DrawOTag(u_long* ot) {
    void* dispatch;
    void (*draw)(void*, u_long*, s32, s32);

    if ((u8)g_GraphDebugLevel >= 2) {
        g_GpuPrintf(D_800191E0, ot);
    }

    dispatch = D_800568C8;
    draw = *(void (**)(void*, u_long*, s32, s32))((u8*)dispatch + 8);
    draw(*(void**)((u8*)dispatch + 0x18), ot, 0, 0);
}

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
