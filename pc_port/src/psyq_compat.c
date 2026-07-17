/*
 * psyq_compat.c - PsyQ SDK compatibility shims for the Xenogears PC port.
 *
 * build_port.sh compiles every game TU EXCEPT the decompiled PsyQ tree
 * (src/slus_006.64/psyq/* is grep -v'd out), so the game's PsyQ calls resolve
 * to PsyCross instead of the on-hardware reimplementations. Where the game's
 * symbol name/signature doesn't line up 1:1 with a PsyCross export, this file
 * provides the thin forwarding shim. Defining a symbol here removes it from the
 * auto-generated no-op stub set.
 *
 * PsyCross's public entry points (PsyX_*) and PsyQ exports (VSync, ...) are all
 * extern "C" (unmangled T symbols in libpsycross.a), so they're callable from C.
 */

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>   /* getenv/atoi for the headless test hook below */
#include <string.h>   /* memcpy for TIM parsing */
#include <libgte.h>   /* PsyCross: pull in before libgpu.h (it uses SVECTOR) */
#include <inline_c.h>
#include <libgpu.h>   /* PsyCross: POLY_F3, setPolyF3 macro (setlen/setcode) */

/* --- PsyCross internals / exports used below (extern "C") --- */
extern void PsyX_EndScene(void);  /* GR_EndScene + GR_StoreFrameBuffer + GR_SwapWindow (SDL_GL_SwapWindow) */
extern int  VSync(int mode);      /* PsyCross frame pacing; returns vblank count. Does NOT present. */
extern void DrawAllSplits(void);  /* flush queued primitives to the GL framebuffer; no-op when none */

/* build_port.sh excludes src/slus_006.64/psyq, so an unqualified rand() would
 * otherwise bind to the host libc and produce a different range and sequence.
 * Retail rand (0x8003FA38) uses this 32-bit LCG and returns bits 16..30.  Keep
 * the seed explicitly 32-bit: PsyQ u_long is 32-bit, while host unsigned long
 * is 64-bit on the native port. */
uint32_t g_RandomSeed;

int rand(void)
{
    uint32_t next = g_RandomSeed * UINT32_C(0x41C64E6D) + UINT32_C(0x3039);
    g_RandomSeed = next;
    return (int)((next >> 16) & UINT32_C(0x7FFF));
}

/* Retail func_80048AB0 (asm 80048AB0) is SetFogNearFar: compute DQA/DQB from
 * near Z, far Z, and projection H. PsyCross exports SetFogNearFar under that
 * name; the game calls the Xenogears symbol, so forward here. Matching-tree
 * ownership of 80048AB0 is the pre-InitGeom asm blob (not libgte.yaml yet). */
void func_80048AB0(long a, long b, long h)
{
    SetFogNearFar(a, b, h);
}

/*
 * OpenTIM / ReadTIM: PsyCross declares these in libgpu.h but provides NO
 * implementation — they fall through to auto-generated no-op stubs that return
 * 0/NULL. FieldLoadTIMWithClut calls OpenTIM(pTimData) then ReadTIM(&tim);
 * the stubbed ReadTIM returns NULL, so the `if (pTIM)` guard fails and
 * LoadImage is never called — no CLUT or pixel data reaches host VRAM (vram[]),
 * and StoreImage/GR_ReadVRAM reads zeros.
 *
 * Implementation follows PsyCross's own GetTimInfo (src/gpu/font.h) exactly:
 * TIM format is [0]=u32 header (ID=0x10, version=0x00), [1]=u32 mode,
 * then if mode&8: [2]=u32 blocklen, [3..4]=RECT16 clut rect, [5..]=clut pixels,
 * then [N]=u32 blocklen, [N+1..N+2]=RECT16 pixel rect, [N+3..]=pixel data.
 *
 * OpenTIM stores the data pointer; ReadTIM parses it into the caller's
 * TIM_IMAGE struct and returns a pointer to it (or NULL on bad ID/version).
 */
static u_long* s_pTimData = NULL;

int OpenTIM(u_long* addr)
{
    s_pTimData = addr;
    return 0;
}

TIM_IMAGE* ReadTIM(TIM_IMAGE* timimg)
{
    u_int* rtim = (u_int*)s_pTimData;

    if (!rtim) {
        printf("[psyq_compat] ReadTIM: no TIM data (OpenTIM not called)\n");
        return NULL;
    }

    /* Check ID */
    if ((rtim[0] & 0xff) != 0x10) {
        printf("[psyq_compat] ReadTIM: bad TIM ID 0x%02x (expected 0x10)\n",
               (unsigned)(rtim[0] & 0xff));
        return NULL;
    }

    /* Check version */
    if (((rtim[0] >> 8) & 0xff) != 0x00) {
        printf("[psyq_compat] ReadTIM: bad TIM version 0x%02x\n",
               (unsigned)((rtim[0] >> 8) & 0xff));
        return NULL;
    }

    timimg->mode = rtim[1];
    rtim += 2;

    /* Clut present? */
    if (timimg->mode & 0x8) {
        timimg->cRECT16 = (RECT16*)&rtim[1];
        timimg->caddr = (u_int*)&rtim[3];
        rtim += rtim[0] >> 2;  /* advance by block length (in words) */
    } else {
        timimg->caddr = 0;
        timimg->cRECT16 = 0;
    }

    timimg->pRECT16 = (RECT16*)&rtim[1];
    timimg->paddr = (u_int*)&rtim[3];

    return timimg;
}

/*
 * SetPolyF3: PsyCross declares it (libgpu.h) but ships only the setPolyF3 macro,
 * not the function -- so the game's call falls through to a no-op auto-stub and
 * the KernelMenu cursor (a flat triangle) never gets a valid primitive tag. This
 * is the exact body of the game's own decompiled SetPolyF3 (psyq/libgpu.c).
 */
void SetPolyF3(POLY_F3* p)
{
    setPolyF3(p);   /* setlen(p, 4), setcode(p, 0x20) */
}

VECTOR* Square0(VECTOR* v0, VECTOR* v1)
{
    v1->vx = v0->vx * v0->vx;
    v1->vy = v0->vy * v0->vy;
    v1->vz = v0->vz * v0->vz;
    return v1;
}

MATRIX* ScaleMatrixL(MATRIX* m, VECTOR* v)
{
    m->m[0][0] = (m->m[0][0] * v->vx) >> 12;
    m->m[0][1] = (m->m[0][1] * v->vx) >> 12;
    m->m[0][2] = (m->m[0][2] * v->vx) >> 12;
    m->m[1][0] = (m->m[1][0] * v->vy) >> 12;
    m->m[1][1] = (m->m[1][1] * v->vy) >> 12;
    m->m[1][2] = (m->m[1][2] * v->vy) >> 12;
    m->m[2][0] = (m->m[2][0] * v->vz) >> 12;
    m->m[2][1] = (m->m[2][1] * v->vz) >> 12;
    m->m[2][2] = (m->m[2][2] * v->vz) >> 12;

    return m;
}

static int32_t GteClampS32(int64_t value)
{
    if (value > 0x7FFFFFFFLL)
        return 0x7FFFFFFF;
    if (value < -0x80000000LL)
        return (int32_t)0x80000000u;
    return (int32_t)value;
}

void OuterProduct12(VECTOR* v0, VECTOR* v1, VECTOR* v2)
{
    int32_t x0 = (int16_t)v0->vx;
    int32_t y0 = (int16_t)v0->vy;
    int32_t z0 = (int16_t)v0->vz;
    int32_t x1 = (int16_t)v1->vx;
    int32_t y1 = (int16_t)v1->vy;
    int32_t z1 = (int16_t)v1->vz;

    v2->vx = GteClampS32(((int64_t)y0 * z1 - (int64_t)z0 * y1) >> 12);
    v2->vy = GteClampS32(((int64_t)z0 * x1 - (int64_t)x0 * z1) >> 12);
    v2->vz = GteClampS32(((int64_t)x0 * y1 - (int64_t)y0 * x1) >> 12);
}

static const int16_t s_InvSqrtTable[] = {
    0x1000, 0x0FE0, 0x0FC1, 0x0FA3, 0x0F85, 0x0F68, 0x0F4C, 0x0F30,
    0x0F15, 0x0EFB, 0x0EE1, 0x0EC7, 0x0EAE, 0x0E96, 0x0E7E, 0x0E66,
    0x0E4F, 0x0E38, 0x0E22, 0x0E0C, 0x0DF7, 0x0DE2, 0x0DCD, 0x0DB9,
    0x0DA5, 0x0D91, 0x0D7E, 0x0D6B, 0x0D58, 0x0D45, 0x0D33, 0x0D21,
    0x0D10, 0x0CFF, 0x0CEE, 0x0CDD, 0x0CCC, 0x0CBC, 0x0CAC, 0x0C9C,
    0x0C8D, 0x0C7D, 0x0C6E, 0x0C5F, 0x0C51, 0x0C42, 0x0C34, 0x0C26,
    0x0C18, 0x0C0A, 0x0BFD, 0x0BEF, 0x0BE2, 0x0BD5, 0x0BC8, 0x0BBB,
    0x0BAF, 0x0BA2, 0x0B96, 0x0B8A, 0x0B7E, 0x0B72, 0x0B67, 0x0B5B,
    0x0B50, 0x0B45, 0x0B39, 0x0B2E, 0x0B24, 0x0B19, 0x0B0E, 0x0B04,
    0x0AF9, 0x0AEF, 0x0AE5, 0x0ADB, 0x0AD1, 0x0AC7, 0x0ABD, 0x0AB4,
    0x0AAA, 0x0AA1, 0x0A97, 0x0A8E, 0x0A85, 0x0A7C, 0x0A73, 0x0A6A,
    0x0A61, 0x0A59, 0x0A50, 0x0A47, 0x0A3F, 0x0A37, 0x0A2E, 0x0A26,
    0x0A1E, 0x0A16, 0x0A0E, 0x0A06, 0x09FE, 0x09F6, 0x09EF, 0x09E7,
    0x09E0, 0x09D8, 0x09D1, 0x09C9, 0x09C2, 0x09BB, 0x09B4, 0x09AD,
    0x09A5, 0x099E, 0x0998, 0x0991, 0x098A, 0x0983, 0x097C, 0x0976,
    0x096F, 0x0969, 0x0962, 0x095C, 0x0955, 0x094F, 0x0949, 0x0943,
    0x093C, 0x0936, 0x0930, 0x092A, 0x0924, 0x091E, 0x0918, 0x0912,
    0x090D, 0x0907, 0x0901, 0x08FB, 0x08F6, 0x08F0, 0x08EB, 0x08E5,
    0x08E0, 0x08DA, 0x08D5, 0x08CF, 0x08CA, 0x08C5, 0x08BF, 0x08BA,
    0x08B5, 0x08B0, 0x08AB, 0x08A6, 0x08A1, 0x089C, 0x0897, 0x0892,
    0x088D, 0x0888, 0x0883, 0x087E, 0x087A, 0x0875, 0x0870, 0x086B,
    0x0867, 0x0862, 0x085E, 0x0859, 0x0855, 0x0850, 0x084C, 0x0847,
    0x0843, 0x083E, 0x083A, 0x0836, 0x0831, 0x082D, 0x0829, 0x0824,
    0x0820, 0x081C, 0x0818, 0x0814, 0x0810, 0x080C, 0x0808, 0x0804,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

static int32_t VectorNormalWork(int32_t x, int32_t y, int32_t z, int32_t* outX, int32_t* outY, int32_t* outZ)
{
    int32_t sx = (int16_t)x;
    int32_t sy = (int16_t)y;
    int32_t sz = (int16_t)z;
    uint32_t squared = (uint32_t)(sx * sx + sy * sy + sz * sz);
    int lzc;
    int lzcEven;
    int shift;
    int index;
    int32_t scale;

    if (squared == 0) {
        *outX = 0;
        *outY = 0;
        *outZ = 0;
        return 0;
    }

    lzc = __builtin_clz(squared);
    lzcEven = lzc & ~1;
    shift = (31 - lzcEven) >> 1;

    if (lzcEven - 24 >= 0)
        index = (int)(squared << (lzcEven - 24));
    else
        index = (int)(squared >> (24 - lzcEven));

    index -= 0x40;
    if (index < 0)
        index = 0;
    if (index >= (int)(sizeof(s_InvSqrtTable) / sizeof(s_InvSqrtTable[0])))
        index = (sizeof(s_InvSqrtTable) / sizeof(s_InvSqrtTable[0])) - 1;

    scale = s_InvSqrtTable[index];
    *outX = GteClampS32(((int64_t)scale * sx) >> shift);
    *outY = GteClampS32(((int64_t)scale * sy) >> shift);
    *outZ = GteClampS32(((int64_t)scale * sz) >> shift);
    return (int32_t)squared;
}

long VectorNormal(VECTOR* v0, VECTOR* v1)
{
    int32_t x, y, z;
    int32_t squared = VectorNormalWork(v0->vx, v0->vy, v0->vz, &x, &y, &z);
    v1->vx = x;
    v1->vy = y;
    v1->vz = z;
    return squared;
}

long VectorNormalS(VECTOR* v0, SVECTOR* v1)
{
    int32_t x, y, z;
    int32_t squared = VectorNormalWork(v0->vx, v0->vy, v0->vz, &x, &y, &z);
    v1->vx = x;
    v1->vy = y;
    v1->vz = z;
    return squared;
}

long RotAverage4(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, SVECTOR* v3,
                 long* sxy0, long* sxy1, long* sxy2, long* sxy3,
                 long* p, long* flag)
{
    long flag0;

    gte_ldv3(v0, v1, v2);
    gte_rtpt();
    gte_stsxy3(sxy0, sxy1, sxy2);
    gte_stflg(&flag0);

    gte_ldv0(v3);
    gte_rtps();
    gte_stsxy(sxy3);
    gte_stflg(flag);
    gte_stdp(p);
    *flag |= flag0;
    /* XENO_PC_PORT: gte_stflg writes only the low 32 bits of a long*; sign-extend
     * so `flag < 0` matches retail bltz on GTE FLAG bit 31 (see build_port.sh
     * _xeno_gte_flag_sx*). Callers must pass a real long* — (long*)&int smashes
     * the stack on LP64 (see FieldZoomFadeEffectUpdate). */
    *flag = (long)(int)(unsigned int)*flag;

    gte_avsz4();
    gte_stotz(p);

    return *p;
}

/* Retail Enter/ExitCriticalSection masks interrupts; the effect the sound
 * code depends on is that the 240Hz tick cannot preempt the section (the
 * SPU transfer-queue writes in SoundQueueTransferCommand). Map onto the
 * PsyCross sound-tick gate -- caught as a real TSan race when the field
 * music path (func_80085F30 -> SoundLoadWdsFile) went live in M3. */
extern void PsyX_Sys_SoundGateEnterCritical(void);
extern void PsyX_Sys_SoundGateExitCritical(void);

int EnterCriticalSection(void)
{
    PsyX_Sys_SoundGateEnterCritical();
    return 0;
}

void ExitCriticalSection(void)
{
    PsyX_Sys_SoundGateExitCritical();
}

int CdDataSync(int mode)
{
    (void)mode;
    return 0;
}

/*
 * Sprintf (PsyQ): the game's variadic string formatter (KernelMenu builds its
 * menu text with it). Forward to libc vsprintf. Signature matches the game's
 * include/system/memory.h declaration.
 */
int Sprintf(char* dest, char* fmt, ...)
{
    va_list ap;
    int n;
    va_start(ap, fmt);
    n = vsprintf(dest, fmt, ap);
    va_end(ap);
    return n;
}

/*
 * Vsync (game spelling, lowercase 's'; ELF symbol `Vsync` @ 0x8004b54c) vs PsyQ
 * VSync. On PSX, VSync(0) waits for vblank, at which point the GPU has finished
 * and the displayed framebuffer flips. PsyCross's VSync only paces/returns the
 * count -- the actual GL backbuffer swap lives in PsyX_EndScene, which nothing
 * on the boot path calls per frame (only ResetGraph does, and the game state
 * loops don't re-enter it). So the screen never updates after the first frame.
 *
 * Presenting here makes Vsync the once-per-frame swap point, which is exactly
 * the PSX display-flip semantics and robust against game code that issues
 * multiple DrawOTag()s per frame (presenting in DrawOTag would swap mid-frame).
 * PsyX_EndScene() is a no-op when no scene is open (begin_scene_flag == 0), so
 * the early boot Vsync(2) before any DrawOTag is safe.
 */
/*
 * Headless test hook for the KernelMenu. Synthetic X11/SDL key events can't reach
 * the focused XWayland window in automated runs, so the only way to drive the menu
 * from a script is to inject the game's button state in code. With XENO_KERNEL_SEL
 * set to an option index (Field=0, Battle=1, Worldmap=2, Battling=3, Menu=4,
 * Movie=5) this presses Circle on that option once, XENO_KERNEL_DELAY frames (def
 * 60) after the KernelMenu becomes active -- exercising ChangeGameState ->
 * MainLoop's overlay load (LoadGameStateOverlay -> ArchiveReadFileToBuffer ->
 * LZSSDecompress) without a human at the keyboard.
 *
 * Normal (non-field-test) boots no longer use KernelMenu -- PcPort_BootMain owns
 * title/movie-skip/new-game and enters Field itself -- so this hook stays idle
 * unless XENO_FIELD_TEST=1 (or an explicit XENO_KERNEL_SEL) is set.
 */
static void PcPort_ForcedKernelSelect(void)
{
    extern int g_KernelMenuCurChoice;
    extern int g_KernelMenuIsRunning;
    extern unsigned short g_C1ButtonStateReleased;
    static int sel = -2, delay, frame;

    if (sel == -2) {  /* first call: read config */
        const char* e = getenv("XENO_KERNEL_SEL");
        const char* d = getenv("XENO_KERNEL_DELAY");
        const char* ft = getenv("XENO_FIELD_TEST");
        if (e && *e) {
            sel = atoi(e);        /* explicit menu drive (field-test or manual) */
        } else if (ft && ft[0] == '1') {
            sel = -1;             /* field-test w/o an explicit choice: leave the
                                   * debug KernelMenu up for interactive use */
        } else {
            sel = -1;             /* NORMAL BOOT: KernelMenu is not the boot state
                                   * (PcPort_BootMain handles Field entry). */
        }
        delay = (d && *d) ? atoi(d) : 60;
        frame = 0;
    }
    if (sel < 0 || !g_KernelMenuIsRunning)
        return;  /* disabled, or menu not up yet -- don't start the countdown */

    if (++frame == delay) {
        g_KernelMenuCurChoice = sel;
        g_C1ButtonStateReleased |= 0x20;  /* CTRL_BTN_CIRCLE */
        printf("[xeno-port][test] forcing KernelMenu select %d (Circle)\n", sel);
    }
}

int Vsync(int mode)
{
    /* Flush any primitives queued this frame before presenting. DrawOTag flushes
     * its own ordering table via DrawAllSplits, but immediate-mode DrawPrim (used
     * by GameShowSplashScreen's fade loops) does NOT -- it only queues into the
     * split list, relying on a later DrawSync/DrawOTag to flush. The splash issues
     * DrawPrim + Vsync with no such flush in between, so its sprite would never
     * reach the framebuffer. Flushing here (idempotent when empty, like DrawSync)
     * makes the present show everything drawn since the last frame. */
    DrawAllSplits();

    PsyX_EndScene();      /* present the frame the game just finished building */

    /* Per-frame input, normally driven by the BIOS vblank IRQ + the game's main
     * loop, both of which live in bypassed asm. PsyX_UpdateInput() polls SDL
     * events and refreshes the registered pad buffer (g_C1Buffer) from the
     * keyboard/gamepad; ControllerPoll() then folds that buffer into the game's
     * g_C1ButtonState* edge/repeat state that the menus/field read. */
    { extern void PsyX_UpdateInput(void); PsyX_UpdateInput(); }
    { extern void ControllerPoll(void);   ControllerPoll();   }
    /* Retail's per-vblank handler func_8003634C pairs ControllerPoll with
     * ControllerPushState (asm 80036368/80036370). FieldPollControllers reads
     * input only by draining that queue via ControllerPopState, so without the
     * push the field never sees any input. */
    { extern void ControllerPushState(void); ControllerPushState(); }

    /* Headless menu driver (no-op unless XENO_KERNEL_SEL is set). Must run after
     * ControllerPoll, which recomputes g_C1ButtonState* each frame -- we OR the
     * synthetic Circle in afterwards so it survives to the next KernelMenuUpdate. */
    PcPort_ForcedKernelSelect();

    /* Temporary interactive camera+cull logger (XENO_CULL_CAM_LOG=1). */
    { extern void PcPort_CullCamLogOnVsync(void); PcPort_CullCamLogOnVsync(); }

    return VSync(mode);   /* then pace to the next vblank */
}
