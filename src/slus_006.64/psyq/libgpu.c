#include "common.h"
#include "psyq/libetc.h"
#include "psyq/libgpu.h"

extern void memcpy(u8*, u8*, int);

extern char g_GraphDebugLevel;
extern void (*g_DrawSyncCallbackFn)();
extern int (*g_GpuPrintf)(char*, ...);

extern DRAWENV g_GpuDrawEnv;
extern volatile u16 g_GpuDispEnv;

/* Dump* format strings. TU-owned bytes (yaml [0x9788, .rodata, psyq/libgpu])
 * kept in retail order; the D_ names match the disassembly labels so relocs
 * read the same. Retail addresses them absolutely (lui+addiu), so each call
 * site pins the arg regs and materializes the address explicitly (same idiom
 * as system/memory.c's HEAP_FMT_PTR). g_GpuPrintf itself stays a natural
 * indirect call (absolute lui+lw under -G0, as in retail). */
const char D_80018F88[] = "tpage: (%d,%d,%d,%d)\n";
const char D_80018FA0[] = "clut: (%d,%d)\n";
const char D_80018FB0[] = "clip (%3d,%3d)-(%d,%d)\n";
const char D_80018FC8[] = "ofs  (%3d,%3d)\n";
const char D_80018FD8[] = "tw   (%d,%d)-(%d,%d)\n";
const char D_80018FF0[] = "dtd   %d\n";
const char D_80018FFC[] = "dfe   %d\n";
const char D_80019008[] = "disp   (%3d,%3d)-(%d,%d)\n";
const char D_80019024[] = "screen (%3d,%3d)-(%d,%d)\n";
const char D_80019040[] = "isinter %d\n";
const char D_8001904C[] = "isrgb24 %d\n";

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
    /* The format string is a TU-owned global (top of file) so the -G0
     * assembler addresses it absolutely with a named reloc, exactly as in
     * retail; the arg computations are untouched from the matching build. */
    g_GpuPrintf(D_80018F88, ((tpage) >> 7) & 0x003, ((tpage) >> 5) & 0x003,
                ((tpage) << 6) & 0x7c0, (((tpage) << 4) & 0x100) + (((tpage) >> 2) & 0x200));
}

void DumpClut(u_short clut) {
    g_GpuPrintf(D_80018FA0, (clut & 0x3f) << 4, (clut >> 6));
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

/* MIPS register-pinned packet fill; the host port links PsyCross's own. */
#ifndef XENO_PC_PORT
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
#endif /* XENO_PC_PORT */

void func_80043EAC(void* pPrim, void* pRect) {
    s32 w = *(s16*)((u8*)pRect + 4);
    s32 h = *(s16*)((u8*)pRect + 6);
    s32 area = (w * h + 1) / 2;
    s32 tagCount = area + 4;
    if ((u32)area >= 13) {
        tagCount = 0;
    }
    *(u8*)((u8*)pPrim + 3) = (u8)tagCount;
    *(u32*)((u8*)pPrim + 4) = 0xA0000000;
    *(u32*)((u8*)pPrim + 8) = *(u32*)pRect;
    *(u32*)((u8*)pPrim + 0xC) = *(u32*)((u8*)pRect + 4);
    {
        register u32 off asm("$2");
        off = (u32)tagCount * 4;
        *(u32*)((u8*)pPrim + off) = 0x1000000;
    }
}

s32 func_80043F18(void* pOt1, void* pOt2) {
    s32 total = *(u8*)((u8*)pOt1 + 3) + *(u8*)((u8*)pOt2 + 3) + 1;
    if (total >= 0x11) {
        return -1;
    }
    *(u8*)((u8*)pOt1 + 3) = (u8)total;
    *(u32*)pOt2 = 0;
    return 0;
}

void DumpDrawEnv(DRAWENV *env) {
    g_GpuPrintf(D_80018FB0, env->clip.x, env->clip.y, env->clip.w, env->clip.h);
    g_GpuPrintf(D_80018FC8, env->ofs[0], env->ofs[1]);
    g_GpuPrintf(D_80018FD8, env->tw.x, env->tw.y, env->tw.w, env->tw.h);
    g_GpuPrintf(D_80018FF0, env->dtd);
    g_GpuPrintf(D_80018FFC, env->dfe);
    g_GpuPrintf(D_80018F88, ((env->tpage) >> 7) & 0x003, ((env->tpage) >> 5) & 0x003,
                ((env->tpage) << 6) & 0x7c0, (((env->tpage) << 4) & 0x100) + (((env->tpage) >> 2) & 0x200));
}

void DumpDispEnv(DISPENV *env) {
    g_GpuPrintf(D_80019008, env->disp.x, env->disp.y, env->disp.w, env->disp.h);
    g_GpuPrintf(D_80019024, env->screen.x, env->screen.y, env->screen.w, env->screen.h);
    g_GpuPrintf(D_80019040, env->isinter);
    g_GpuPrintf(D_8001904C, env->isrgb24);
}

/* MIPS register-pinned dispatch; the host port links PsyCross's own. */
#ifndef XENO_PC_PORT
int ResetGraph(int mode) {
    extern char D_8001908C[];
    extern char D_800190AC[];
    extern char D_80056888[];
    extern u8 D_800568D0;
    extern u8 D_800568D1;
    extern s16 D_800568D4;
    extern s16 D_800568D6;
    extern void* D_800568C8;
    extern s32 D_80056950[];
    extern s32 D_80056964[];
    extern void func_80047178(void*, s32, s32);
    extern void func_800471A4(u32);
    extern int func_80046C58(int);
    register int arg asm("$17") = mode;
    register u8* state asm("$16");
    int index;

    switch (arg & 7) {
    case 0:
    case 3:
        printf(D_8001908C, D_80056888, &D_800568D0);
    case 5:
        state = &D_800568D0;
        func_80047178(state, 0, 0x80);
        ResetCallback();
        func_800471A4((u32)D_800568C8 & 0xFFFFFF);
        index = func_80046C58(arg);
        state[0] = index;
        D_800568D1 = 1;
        index = (u8)index * 4;
        D_800568D4 = *(s32*)((u8*)D_80056950 + index);
        D_800568D6 = *(s32*)((u8*)D_80056964 + index);
        func_80047178(state + 0x10, -1, 0x5C);
        func_80047178(state + 0x6C, -1, 0x14);
        return state[0];
    default:
        if ((u8)g_GraphDebugLevel >= 2) {
            g_GpuPrintf(D_800190AC, arg);
        }
        return (*(int (**)(int))((u8*)D_800568C8 + 0x34))(1);
    }
}
#endif /* XENO_PC_PORT */

extern char D_800190C0[];

s32 SetGraphReverse(s32 arg) {
    extern u8 D_800568D3;
    extern void* D_800568C8;
    extern u8 D_800568D0;
    register s32 a asm("$17");
    register u8* rev asm("$16");
    register s32 old asm("$18");
    s32 r;
    u32 mode;

    a = arg;
    rev = &D_800568D3;
    old = *rev;
    if (((u8*)&g_GraphDebugLevel)[0] >= 2) {
        g_GpuPrintf(D_800190C0, a);
    }
    {
        void* dispatch = D_800568C8;
        *rev = (u8)a;
        r = (*(s32 (**)(u32))((u8*)dispatch + 0x28))(8);
    }
    if (*rev != 0) {
        r |= 0x8000080;
    } else {
        r |= 0x8000000;
    }
    (*(void (**)(u32))((u8*)D_800568C8 + 0x10))(r);
    if (D_800568D0 == 2) {
        register s32 flag asm("$2");
        register void* dsp asm("$3");
        mode = 0x20000000;
        __asm__ volatile("lui %0,%%hi(D_800568D3)\n\tlbu %0,%%lo(D_800568D3)(%0)" : "=r"(flag) : "r"(mode) : "memory");
        __asm__ volatile("lui %0,%%hi(D_800568C8)\n\tlw %0,%%lo(D_800568C8)(%0)" : "=r"(dsp) :: "memory");
        mode |= 0x504;
        if (flag != 0) {
            mode = 0x20000501;
        }
        (*(void (**)(u32))((u8*)dsp + 0x10))(mode);
    }
    return old;
}

/* MIPS register-pinned debug level; the host port links PsyCross's own. */
#ifndef XENO_PC_PORT
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
#endif /* XENO_PC_PORT */

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
extern char D_8001920C[];
extern char D_8005698C[];
extern void DMACallback(s32, void*);
extern void func_80047178(void*, s32, s32);
extern void func_8004463C(char*, RECT*);
extern void func_8004574C(void*, DRAWENV*);

u8 func_8004440C(int arg0) {
    register int av asm("$16");
    register u8* pD asm("$17");
    register int oldv asm("$18");
    void* dispatch;
    void (*pFunc)(s32);

    av = arg0;
    pD = &D_800568D1;
    oldv = *pD;
    if (g_GraphDebugLevel >= 2) {
        register void* pf asm("$2");
        register char* fmt asm("$4");
        __asm__ volatile(
            "lui %1,%%hi(D_80019104)\n\t"
            "addiu %1,%1,%%lo(D_80019104)\n\t"
            "lui %0,%%hi(g_GpuPrintf)\n\t"
            "lw %0,%%lo(g_GpuPrintf)(%0)"
            : "=r"(pf), "=r"(fmt) :: "memory");
        ((void (*)(char*, int))pf)(fmt, av);
    }
    if (av != *pD) {
        __asm__ volatile(
            "lui %0,%%hi(D_800568C8)\n\t"
            "lw %0,%%lo(D_800568C8)(%0)"
            : "=r"(dispatch) :: "memory");
        pFunc = *(void (**)(s32))((u8*)dispatch + 0x34);
        pFunc(1);
        *pD = av;
        DMACallback(2, (void*)0);
    }
    return oldv;
}

extern u8 D_800568D0;

u8 func_800444B8(void) {
    return D_800568D0;
}

int GetGraphDebug(void) {
    return g_GraphDebugLevel;
}

extern char D_80019118; // "DrawSyncCallback(%08x)...\n"
/* LP64-conflicting u_long return (PsyCross declares u_int); the host port
 * links PsyCross's own DrawSyncCallback. */
#ifndef XENO_PC_PORT
u_long DrawSyncCallback(void (*pCallbackFn)()) {
    void (*pPrevCallbackFn)();

    if (g_GraphDebugLevel >= 2)
        g_GpuPrintf(&D_80019118, pCallbackFn);

    pPrevCallbackFn = g_DrawSyncCallbackFn;
    g_DrawSyncCallbackFn = pCallbackFn;
    return (u_long) pPrevCallbackFn;
}
#endif /* XENO_PC_PORT */

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

extern char D_8001915C[];
extern char D_8001917C[];
extern char D_80019168[];

void func_8004463C(char* template, RECT* rect) {
    extern s16 D_800568D4;
    extern s16 D_800568D6;
    register char* t0 asm("$8") = template;
    register RECT* s0 asm("$16");
    u8 level;
    s32 w, x, y, h;
    s32 boundX, boundY;
    char* fmt;

    __asm__("addu $16,$5,$0" : "=r"(s0) : "r"(rect));
    level = g_GraphDebugLevel;
    if (level == 1)
        goto clip;
    if (level == 2)
        goto level2;
    goto end;
clip:
    w = s0->w;
    boundX = D_800568D4;
    if (boundX < w)
        goto fail;
    x = s0->x;
    if (boundX < w + x)
        goto fail;
    y = s0->y;
    boundY = D_800568D6;
    if (boundY < y)
        goto fail;
    h = s0->h;
    if (boundY < y + h)
        goto fail;
    if (w <= 0)
        goto fail;
    if (x < 0)
        goto fail;
    if (y < 0)
        goto fail;
    if (h > 0)
        goto end;
fail:
    fmt = D_8001915C;
    goto doprint;
level2:
    fmt = D_8001917C;
doprint:
    g_GpuPrintf(fmt, t0);
    {
        s32 px, py, pw;
        register void (*pf)(char*, s32, s32, s32, s32) asm("$3");
        px = s0->x;
        __asm__ volatile("" ::: "memory");
        py = s0->y;
        __asm__ volatile("" ::: "memory");
        pw = s0->w;
        __asm__ volatile("" ::: "memory");
        pf = (void (*)(char*, s32, s32, s32, s32))g_GpuPrintf;
        pf(D_80019168, px, py, pw, s0->h);
    }
end: ;
}

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

/* Transcribed from asm/slus_006.64/nonmatchings/psyq/libgpu/MoveImage.s
 * (0x8004495C-0x80044A20, 49 instructions). Queues a 20-byte image-move packet:
 * func_8004463C fills the command header for the D_800191A4 template and a zero
 * result returns -1 (retail's -1 lives in the delay slot); the packet then gets
 * (y << 16) | (x & 0xFFFF) at +4 and the rect's two words at +0/+8, and the
 * dispatch hook at D_800568C8+8 is called with (*(D_800568C8+0x18),
 * &D_80056978, 0x14, 0). Weak under XENO_PC_PORT (PsyCross provides MoveImage). */
extern char D_800191A4[];
extern u32 D_80056980[];
extern u32 D_80056984;
extern u32 D_80056988;

#ifdef XENO_PC_PORT
__attribute__((weak))
#endif
int MoveImage(RECT* pRect, int x, int y) {
    register RECT* rp asm("$16");
    register int xx asm("$18");
    register int yy asm("$17");
    register char* tmpl asm("$4");
    int (*move)(void*, void*, s32, u32);

    __asm__("addu $16,$4,$0" : "=r"(rp) : "r"(pRect));
    __asm__("addu $18,$5,$0" : "=r"(xx) : "r"(x));
    __asm__("addu $17,$6,$0" : "=r"(yy) : "r"(y));
    __asm__("lui $4,%%hi(D_800191A4)\n\taddiu $4,$4,%%lo(D_800191A4)" : "=r"(tmpl) : "r"(rp), "r"(xx), "r"(yy));
    func_8004463C(tmpl, rp);
    if (rp->w == 0) {
        return -1;
    }
    if (rp->h != 0) {
        register u32 shy asm("$2");
        register u32 lo asm("$3");
        register u32* base asm("$5");
        register void* dsp asm("$3");
        register u32 z asm("$7");
        u32 xy;
        shy = (u32)yy << 16;
        lo = (u32)xx & 0xFFFF;
        shy |= lo;
        xy = *(u32*)&rp->x;
        base = D_80056980;
        __asm__ volatile("lui %0,%%hi(D_800568C8)\n\tlw %0,%%lo(D_800568C8)(%0)" : "=r"(dsp) :: "memory");
        D_80056984 = shy;
        D_80056980[0] = xy;
        D_80056988 = *(u32*)&rp->w;
        z = 0;
        move = *(int (**)(void*, void*, s32, u32))((u8*)dsp + 8);
        return move(*(void**)((u8*)dsp + 0x18), (void*)&base[-2], 0x14, z);
    }
    return -1;
}

/* Transcribed from asm/slus_006.64/nonmatchings/psyq/libgpu/ClearOTag.s
 * (0x80044A20-0x80044AD8). Builds a linked OT of n entries: each entry keeps its
 * length byte and gets the low 24 bits of the next entry's address, byte 3 is
 * explicitly zeroed first (which is why the preserved top byte is always 0), and
 * the last entry gets D_8005698C & 0xFFFFFF as the terminator. With
 * g_GraphDebugLevel >= 2 the call is traced through the g_GpuPrintf hook. The
 * port links PsyCross's own ClearOTag, so this definition is weak there. */
extern char D_800191B0[];

#ifdef XENO_PC_PORT
__attribute__((weak))
#endif
u_long* ClearOTag(u_long* ot, int n) {
    register u_long* p asm("$16");
    register int nn asm("$17");

    __asm__("addu $16,$4,$0" : "=r"(p) : "r"(ot));
    __asm__("addu $17,$5,$0" : "=r"(nn) : "r"(n), "r"(p));
    if (g_GraphDebugLevel >= 2) {
        register void* pf asm("$2");
        register char* fmt asm("$4");
        __asm__ volatile(
            "lui %0,%%hi(g_GpuPrintf)\n\t"
            "lw %0,%%lo(g_GpuPrintf)(%0)\n\t"
            "lui %1,%%hi(D_800191B0)\n\t"
            "addiu %1,%1,%%lo(D_800191B0)"
            : "=r"(pf), "=r"(fmt) :: "memory");
        ((void (*)(char*, u_long*, int))pf)(fmt, p, nn);
    }
    nn--;
    if (nn != 0) {
        do {
            register u32 p1 asm("$4");
            register u32 orv asm("$2");
            u32 lowPart, highPart;
            nn--;
            __asm__("addiu $4,$16,4" : "=r"(p1) : "r"(p));
            *(u8*)((u8*)p + 3) = 0;
            __asm__ volatile("" ::: "memory");
            lowPart = (p1 & 0x00FFFFFFu);
            highPart = (*p & 0xFF000000u);
            orv = (highPart | lowPart);
            *p = orv;
            /* Element step: +4 bytes on retail ILP32 (same addiu as the asm),
             * one padded host u_long under XENO_PC_PORT LP64. */
            p = (u_long*)p1;
        } while (nn != 0);
    }
    *p = ((u32)(uintptr_t)D_8005698C) & 0x00FFFFFFu;
    return p;
}

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

extern char D_800191F4[];

DRAWENV* PutDrawEnv(DRAWENV* envInput) {
    register u8* state asm("$18") = (u8*)&g_GraphDebugLevel;
    register DRAWENV* env asm("$17") = envInput;
    register void* packet asm("$16");
    void* dispatch;
    void (*submit)(void*, void*, s32, s32);

    if (state[0] >= 2) {
        g_GpuPrintf(D_800191F4, env);
    }
    packet = (u8*)env + 0x1C;
    func_8004574C(packet, env);
    {
        register u32 mask asm("$4") = 0xFFFFFF;
        register void* packetArg asm("$5") = packet;
        register s32 size asm("$6") = 0x40;
        u32 word = *(u32*)((u8*)env + 0x1C);
        word |= mask;
        dispatch = D_800568C8;
        *(u32*)((u8*)env + 0x1C) = word;
        submit = *(void (**)(void*, void*, s32, s32))((u8*)dispatch + 8);
        submit(*(void**)((u8*)dispatch + 0x18), packetArg, size, 0);
    }
    {
        /* Unused; folds the copy loop's end computation into env+$50. */
        register u8* end asm("$8") = (u8*)env + 0x50;
        *(DRAWENV*)(state + 0x0E) = *env;
        __asm__("" : : "r"(state), "r"(env));
    }
    return env;
}

/* MIPS register-pinned OT submit; the host port links PsyCross's own. */
#ifndef XENO_PC_PORT
void DrawOTagEnv(u_long* otInput, DRAWENV* envInput) {
    register u_long* ot asm("$18") = otInput;
    register u8* state asm("$19") = (u8*)&g_GraphDebugLevel;
    register DRAWENV* env asm("$17") = envInput;
    register void* packet asm("$16");
    void* dispatch;
    void (*submit)(void*, void*, s32, s32);

    if (state[0] >= 2) {
        g_GpuPrintf(D_8001920C, ot, env);
    }
    packet = (u8*)env + 0x1C;
    func_8004574C(packet, env);
    {
        register u32 mask asm("$4") = 0xFFFFFF;
        register void* packetArg asm("$5") = packet;
        register s32 size asm("$6") = 0x40;
        register u32 highMask asm("$3") = 0xFF000000;
        u32 word = *(u32*)((u8*)env + 0x1C);
        u32 link = (u32)ot & mask;
        word &= highMask;
        dispatch = D_800568C8;
        *(u32*)((u8*)env + 0x1C) = word | link;
        submit = *(void (**)(void*, void*, s32, s32))((u8*)dispatch + 8);
        submit(*(void**)((u8*)dispatch + 0x18), packetArg, size, 0);
    }
    *(DRAWENV*)(state + 0x0E) = *env;
    __asm__("" : : "r"(state), "r"(env));
}
#endif /* XENO_PC_PORT */

DRAWENV* GetDrawEnv(DRAWENV* env) {
    memcpy(env, &g_GpuDrawEnv, sizeof(DRAWENV));
    return env;
}

extern char D_80019228[];
extern s32 func_80045C94(DISPENV*);
extern volatile u16 D_80056944;
extern volatile u16 D_80056946;
extern volatile u16 D_80056948;
extern volatile u16 D_8005694A;
extern u32 D_8005694C;
extern volatile u16 D_8005693E;
extern volatile u16 D_80056940;
extern volatile u16 D_80056942;
extern u8 D_800568D3;
extern u8 D_800568D0;
extern void* D_800568C8;

DISPENV* PutDispEnv(DISPENV* envInput) {
    register DISPENV* env asm("$16");
    register u32 cmd asm("$19");
    register s32 dispX asm("$17");
    register s32 dispY asm("$18");
    void* dispatch;
    void (*submit1)(u32);
    void (*submit4)(void*, u32);
    s32 ret;

    __asm__("addu $16,$4,$0" : "=r"(env) : "r"(envInput));
    __asm__("lui $19,0x800" : "=r"(cmd) : "r"(env));
    if (((u8*)&g_GraphDebugLevel)[0] >= 2) {
        g_GpuPrintf(D_80019228, env);
    }
    if ((D_800568D0 - 1u) < 2u) {
        ret = func_80045C94(env);
        ret = ((((u16)env->disp.y & 0xFFF) << 12) | (ret & 0xFFF)) | 0x5000000;
    } else {
        ret = ((((u16)env->disp.y & 0x3FF) << 10) | ((u16)env->disp.x & 0x3FF)) | 0x5000000;
    }
    submit1 = *(void (**)(u32))((u8*)D_800568C8 + 0x10);
    submit1(ret);
    if (!(((s16)*(volatile u16*)&D_80056944 == env->screen.x) &&
          ((s16)D_80056946 == env->screen.y) &&
          ((s16)D_80056948 == env->screen.w) &&
          ((s16)D_8005694A == env->screen.h))) {
        register s32 v1 asm("$3");
        register s32 a2 asm("$6");
        register s32 a1 asm("$5");
        register s32 w10 asm("$2");
        s32 vm = GetVideoMode();
        __asm__ volatile("" ::: "memory");
        {
            register s32 x asm("$4");
            register s32 y asm("$4");
            x = env->screen.x;
            env->pad0 = (u8)vm;
            vm &= 0xFF;
            v1 = x * 10;
            __asm__ volatile("" ::: "memory");
            y = env->screen.y;
            v1 += 0x260;
            if (vm != 0) {
                dispX = y + 0x13;
            } else {
                dispX = y + 0x10;
            }
            __asm__ volatile("lh %0,%2(%1)\n\tnop" : "=r"(a1) : "r"(env), "i"(12) : "memory");
            if (a1 != 0) {
                w10 = a1 * 10;
                a2 = v1 + w10;
            } else {
                a2 = v1 + 0xA00;
            }
        }
        if ((s32)env->screen.h != 0) {
            dispY = dispX + (s32)env->screen.h;
        } else {
            dispY = dispX + 0xF0;
        }
        {
            if (v1 >= 0x1F4) {
                if (v1 < 0xCDB) {
                    a1 = v1;
                } else {
                    a1 = 0xCDA;
                }
            } else {
                a1 = 0x1F4;
            }
            v1 = a1;
            a1 = v1 + 0x50;
            if (a2 < a1) {
                goto skipArea;
            }
            if (a2 < 0xCDB) {
                a1 = a2;
            } else {
                a1 = 0xCDA;
            }
        skipArea:
            a2 = a1;
        {
            register s32 A4 asm("$4");
            if (dispX >= 0x10) {
                if (env->pad0) {
                    if (dispX < 0x137) {
                        A4 = dispX;
                        goto endPadXa;
                    }
                } else {
                    if (dispX < 0x101) {
                        goto takePadXb;
                    }
                }
            c4PadX:
                if (env->pad0) {
                    A4 = 0x136;
                } else {
                    A4 = 0x100;
                }
                goto endPadXb;
            takePadXb:
                A4 = dispX;
                goto endPadXb;
            } else {
                A4 = 0x10;
            }
            endPadXa:
                __asm__ volatile("" ::: "memory");
            endPadXb: ;
            dispX = A4;
        }
        a1 = dispX + 2;
        if (dispY >= a1) {
            if (env->pad0) {
                if (dispY < 0x139) {
                    a1 = dispY;
                    goto endPadY;
                }
            } else {
                if (dispY < 0x103) {
                    goto takePadY;
                }
            }
        c4PadY:
            if (env->pad0) {
                a1 = 0x138;
            } else {
                a1 = 0x102;
            }
            goto endPadY;
        takePadY:
            a1 = dispY;
        endPadY: ;
        }
        dispY = a1;
        {
            register s32 x12 asm("$2");
            register u32 v1p asm("$4");
            register u32 c6 asm("$3");
            register u32 or1 asm("$4");
            register void* db asm("$5");
            x12 = ((u16)a2 & 0xFFF) << 12;
            v1p = ((u16)v1 & 0xFFF);
            c6 = 0x6000000;
            db = D_800568C8;
            submit4 = *(void (**)(void*, u32))((u8*)db + 0x10);
            or1 = v1p | c6;
            __asm__ volatile("" ::: "memory");
            submit4((void*)(x12 | or1),
                    db);
        }
        {
            register s32 y10 asm("$2");
            register u32 xpart asm("$4");
            register u32 c7 asm("$3");
            register u32 or1b asm("$4");
            y10 = ((u16)dispY & 0x3FF) << 10;
            xpart = ((u16)dispX & 0x3FF);
            c7 = 0x7000000;
            submit4 = *(void (**)(void*, u32))((u8*)D_800568C8 + 0x10);
            or1b = xpart | c7;
            submit4((void*)(y10 | or1b),
                    D_800568C8);
        }
        }
    }
    if (D_8005694C != *(s32*)((u8*)env + 0x10))
        goto doG;
    if ((s16)g_GpuDispEnv != env->disp.x)
        goto doG;
    if ((s16)D_8005693E != *(s16*)((u8*)env + 2))
        goto doG;
    if ((s16)D_80056940 != *(s16*)((u8*)env + 4))
        goto doG;
    if ((s16)D_80056942 == *(s16*)((u8*)env + 6))
        goto skipG;
doG: {
        s32 vm = GetVideoMode();
        env->pad0 = (u8)vm;
        if ((vm & 0xFF) == 1) {
            cmd |= 8;
        }
        if (env->isrgb24) {
            cmd |= 0x10;
        }
        if (env->isinter) {
            cmd |= 0x20;
        }
        if (D_800568D3) {
            cmd |= 0x80;
        }
        if ((s32)env->disp.w < 0x119) {
        } else if ((s32)env->disp.w < 0x161) {
            cmd |= 1;
        } else if ((s32)env->disp.w < 0x191) {
            cmd |= 0x40;
        } else if ((s32)env->disp.w < 0x231) {
            cmd |= 2;
        } else {
            cmd |= 3;
        }
        {
            s32 padtmp = env->pad0;
            s32 vv = env->disp.h;
            s32 isUnder;
            if (padtmp) {
                isUnder = (vv < 0x121);
            } else {
                isUnder = (vv < 0x101);
            }
            if (isUnder) {
                goto skipV;
            }
            cmd |= 0x24;
        skipV: ;
        }
        dispatch = D_800568C8;
        submit1 = *(void (**)(u32))((u8*)dispatch + 0x10);
        submit1(cmd);
    }
skipG: ;
    memcpy(&g_GpuDispEnv, env, 0x14);
    return env;
}

DISPENV* GetDispEnv(DISPENV* env) {
    memcpy(env, &g_GpuDispEnv, sizeof(DISPENV));
    return env;
}
