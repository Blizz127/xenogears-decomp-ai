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

/* PsyQ ReadGeomOffset: PsyCross implements SetGeomOffset (C2_OFX = ofx<<16)
 * but not the read-back. Used by the 0xBC sub-command 0x17 (screen-center
 * delta) in animation_scripts.c. */
#include <psx/gtereg.h>
void ReadGeomOffset(long* ofxp, long* ofyp)
{
    *ofxp = C2_OFX >> 16;
    *ofyp = C2_OFY >> 16;
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

/* Retail Enter/ExitCriticalSection masks ALL interrupts, and the game uses
 * it across many subsystems. The port keeps the GLOBAL mapping a no-op:
 * coupling every critical section to the sound-tick gate (tried in B5.2)
 * couples unrelated subsystems to the audio lock and destabilizes the
 * TSan build. The one load-bearing use for the port -- the SPU
 * transfer-queue writes racing the 240Hz tick -- is bracketed at its own
 * call sites in sound.c with PsyX_Sys_SoundGateEnterCritical/Exit. */
int EnterCriticalSection(void)
{
    return 0;
}

void ExitCriticalSection(void)
{
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

/*
 * Headless RESOURCED main-menu driver (no-op unless XENO_MENU_FORCE=1).
 * XENO_KERNEL_SEL=4 dispatches the menu but never loads its resources
 * (g_MenuDebugEnabled=0, D_8005945C=NULL), so no textured content can build.
 * This instead forces the FIELD menu-opener path: once the field is up and
 * idle, set D_800ADB64=0x80 (main menu; 0x80 & 0x7F -> D_80059460=0), mimicking
 * the menu button (field main.c:624). The field loop's trigger (main.c:618)
 * then calls func_800799D4, which streams the menu resources (D_8005945C =
 * archive file 1) and runs MenuMain -- the same resourced path member_change
 * was validated on (the map005 repro). A code-side write of the real global is
 * reliable, unlike gdb symbol-writes (which hit a native-layout phantom view).
 */
static int s_xenoMenuForceFired = 0;

static void PcPort_ForcedFieldMenu(void)
{
    extern int D_800ADB64;   /* menu request (0xFF = none); s32 in-game */
    extern int D_800ADB68;   /* playerCanRun -- field is up + idle; s32 in-game */
    static int armed = -2, frame = 0, delay = 0;
#define fired s_xenoMenuForceFired

    if (armed == -2) {  /* first call: read config */
        const char* e = getenv("XENO_MENU_FORCE");
        const char* d = getenv("XENO_MENU_FORCE_DELAY");
        armed = (e && e[0] == '1') ? 1 : 0;
        delay = (d && *d) ? atoi(d) : 90;
        frame = 0;
    }
    if (!armed || fired)
        return;
    if (++frame < delay)
        return;
    if (D_800ADB64 == 0xFF && D_800ADB68 == 1) {
        /* HARNESS-ONLY party seed: retail NEVER opens the menu with an empty
         * party (the roster is set by the intro/save before a menu is
         * reachable), and the menu-open animation's step/exit logic is driven
         * by the party slots -- with zero members it would spin forever (in
         * retail too; the state is unreachable there). The cold field-test
         * boot skips the intro, so seed the retail-guaranteed minimum: Fei
         * (char 0) in slot 0 + his bit in the party-available mask.
         * g_GameState offsets: 0x1D30 mask, 0x1D32 FrMask, 0x1D34 members[3]. */
        {
            extern unsigned char g_GameState[];  /* PSX-layout data blob */
            unsigned short* pMask = (unsigned short*)&g_GameState[0x1D30];
            unsigned short* pFrMask = (unsigned short*)&g_GameState[0x1D32];
            unsigned char* pMembers = &g_GameState[0x1D34];

            if ((*pMask & *pFrMask & 0x7FF) == 0) {
                pMembers[0] = 0;      /* Fei */
                pMembers[1] = 0xFF;   /* slots 1/2 empty (cold BSS zeros would
                                       * otherwise read as three Feis) */
                pMembers[2] = 0xFF;
                *pMask |= 0x1;
                *pFrMask |= 0x1;
                printf("[xeno-port][test] XENO_MENU_FORCE: seeded harness party "
                       "(Fei slot 0) -- cold boot had an empty roster\n");
            }
        }
        /* The same cold boot also bypasses the intro/save inventory setup.
         * Seed a single scrollable, retail-valid Items list only when every
         * item slot is empty.  IDs still resolve through the loaded game name
         * bank and quantities still use the real two-glyph renderer; this only
         * supplies the state a reachable retail menu would already have.
         * g_GameState offsets: 0x1F90 quantities[150], 0x2026 IDs[150]. */
        {
            extern unsigned char g_GameState[];  /* PSX-layout data blob */
            unsigned char* pQuantities = &g_GameState[0x1F90];
            unsigned char* pItemIds = &g_GameState[0x2026];
            int hasItem = 0;
            int i;

            for (i = 0; i < 150; i++) {
                if (pItemIds[i] != 0) {
                    hasItem = 1;
                    break;
                }
            }
            if (!hasItem) {
                for (i = 0; i < 18; i++) {
                    pItemIds[i] = (unsigned char)(i + 1);
                    pQuantities[i] = (unsigned char)(i + 11);
                }
                printf("[xeno-port][test] XENO_MENU_FORCE: seeded harness "
                       "inventory (18 real item IDs, quantities 11..28) -- "
                       "cold boot had no save inventory\n");
            }
        }
        D_800ADB64 = 0x80;   /* request the field main menu via the opener */
        fired = 1;
        printf("[xeno-port][test] XENO_MENU_FORCE: requesting field main menu "
               "(D_800ADB64=0x80) at frame %d\n", frame);
    }
#undef fired
}

static int s_xenoMenuNavActions = 0;
static int s_xenoMenuNavDowns = 0;
static int s_xenoMenuReorderActions = 0;
static int s_xenoMenuPromptActions = 0;
static int s_xenoMenuItemsOpenTick = -1;

/* Synthetic menu-nav edges (XENO_MENU_NAV_TEST=N): once the forced menu has
 * had time to open, hold DPAD-DOWN for a single frame, N times, ~30 hook-calls
 * apart, so an automated capture can verify the cursor moves.  The special
 * value XENO_MENU_NAV_TEST=items performs three DOWN taps (Exit -> Items), then
 * injects Circle/Cross release edges around a stable open-window interval.
 * XENO_MENU_NAV_TEST=items-reorder follows the same path, then selects row 0,
 * moves right to row 1, confirms the reorder, and closes the screen.
 * XENO_MENU_NAV_TEST=items-prompt confirms row 0 twice, holds the initial
 * target prompt without any direction input, then cancels prompt and Items.
 *
 * The inject is at the RAW BIOS pad buffer (g_C1Buffer), exactly where a real
 * keypress lands: PsyX_UpdateInput() refreshes the buffer from SDL each frame
 * (idle = 0xFF, active-low), then ControllerPoll() derives the pressed/edge/
 * repeat state the menu reads.  DPAD-DOWN is bit 0x40 of buttons byte
 * g_C1Buffer[CONTROLLER_BUTTONS_1] (== PsyX pad->buttons[0], `ret &= ~0x40`).
 * So we MUST run after PsyX_UpdateInput and before ControllerPoll; clearing the
 * bit for one frame makes ControllerPoll compute a genuine rising edge that
 * flows through ControllerPushState → the queue → the menu reader, identical to
 * a physical DOWN tap.  (An earlier attempt OR-ing the derived edge var after
 * ControllerPoll failed: the reader drains the queue, not the live var.) */
static void PcPort_ForcedMenuNav(void)
{
    extern unsigned char g_C1Buffer[];
    extern int g_XenoMenuNavReaderTicks;   /* menu reader executions (misc.c) */
    static int armed = -2, injected = 0;

    if (armed == -2) {
        const char* e = getenv("XENO_MENU_NAV_TEST");
        if (e && (strcmp(e, "items") == 0 ||
                  strcmp(e, "items-reorder") == 0 ||
                  strcmp(e, "items-prompt") == 0)) {
            armed = 3;
            s_xenoMenuNavActions = 1;
            s_xenoMenuReorderActions = strcmp(e, "items-reorder") == 0;
            s_xenoMenuPromptActions = strcmp(e, "items-prompt") == 0;
        } else {
            armed = (e && *e) ? atoi(e) : 0;
        }
        s_xenoMenuNavDowns = armed;
    }
    if (armed <= 0)
        return;
    /* Clock the injection off READER TICKS, not Vsync frames: the field's
     * open/close phases reset the pad queue at a different cadence, so frame
     * counting races it.  A tick == one execution of the menu's interactive
     * input reader, so a hold spanning >=4 ticks is guaranteed to be polled,
     * edged, queued, and drained while the menu is actually listening.
     * Schedule: settle 40 ticks, then press k = hold 4 ticks / release 10. */
    {
        int t = g_XenoMenuNavReaderTicks;
        int k, ph;
        if (t < 40)
            return;
        k = (t - 40) / 14;
        ph = (t - 40) % 14;
        if (k < armed && ph < 4) {
            g_C1Buffer[0x2] &= (unsigned char)~0x40;  /* hold DPAD-DOWN */
            if (k + 1 > injected) {
                injected = k + 1;
                printf("[xeno-port][test] XENO_MENU_NAV_TEST: press DOWN %d/%d "
                       "(reader tick %d)\n", injected, armed, t);
                fflush(stdout);
            }
        }

        /* N2c-1 proof: after Circle selects the first item, inject a genuine
         * RIGHT pad hold so the retail Items reader advances from row 0 to
         * row 1 before the second Circle confirms the exchange. */
        if (s_xenoMenuReorderActions && s_xenoMenuItemsOpenTick >= 0 &&
            t >= s_xenoMenuItemsOpenTick + 18 &&
            t < s_xenoMenuItemsOpenTick + 22) {
            g_C1Buffer[0x2] &= (unsigned char)~0x20;  /* hold DPAD-RIGHT */
            if (t == s_xenoMenuItemsOpenTick + 18) {
                printf("[xeno-port][test] N2C1 REORDER: press RIGHT "
                       "row 0 -> row 1 at reader tick %d\n", t);
                fflush(stdout);
            }
        }

        /* After the Items common-exit tail has rebuilt the main highlight,
         * issue one more genuine DOWN tap.  This is the nav-alive proof. */
        if (s_xenoMenuNavActions) {
            extern int g_XenoMenuN2Phase;
            static int closedTick = -1, finalLogged = 0;
            if (g_XenoMenuN2Phase >= 2 && closedTick < 0)
                closedTick = t;
            if (closedTick >= 0 && t >= closedTick + 20 && t < closedTick + 24) {
                g_C1Buffer[0x2] &= (unsigned char)~0x40;
                if (!finalLogged) {
                    finalLogged = 1;
                    printf("[xeno-port][test] XENO_MENU_NAV_TEST=items: "
                           "post-close DOWN (nav-alive) at reader tick %d\n", t);
                    fflush(stdout);
                }
            }
        }
    }
}

/* Circle/Cross must be inserted after ControllerPoll recomputes the derived
 * edge globals but before ControllerPushState snapshots them for the queue
 * drained by func_801C7D78. */
static void PcPort_ForcedMenuActionEdges(void)
{
    extern unsigned short g_C1ButtonStateReleased;
    extern int g_XenoMenuNavReaderTicks;
    extern int g_XenoMenuN2Phase;
    extern int g_XenoMenuN2c2Phase;
    extern unsigned int PcPort_N2c1ReadItemPair(void);
    static int confirmInjected = 0, selectInjected = 0;
    static int reorderInjected = 0, cancelInjected = 0;
    static int promptInjected = 0, promptCancelInjected = 0;
    static int promptObserved = 0, promptTick = -1;
    static int reorderObserved = 0;
    static unsigned int inventoryHashBefore = 0;
    static unsigned int characterHashBefore = 0;
    unsigned int itemPair;
    int confirmTick;
    int t;

    /* FNV-1a over the actual save-backed regions touched by item effects. */
    #define HASH_REGION(dst, start, length) do { \
        extern unsigned char g_GameState[]; \
        unsigned int _h = 2166136261U; \
        int _i; \
        for (_i = 0; _i < (length); _i++) { \
            _h ^= g_GameState[(start) + _i]; \
            _h *= 16777619U; \
        } \
        (dst) = _h; \
    } while (0)

    if (!s_xenoMenuNavActions)
        return;

    t = g_XenoMenuNavReaderTicks;
    confirmTick = 40 + 14 * s_xenoMenuNavDowns + 14;
    if (!confirmInjected && t >= confirmTick) {
        confirmInjected = 1;
        g_C1ButtonStateReleased |= 0x20;  /* CTRL_BTN_CIRCLE */
        printf("[xeno-port][test] XENO_MENU_NAV_TEST=items: Circle confirm "
               "at reader tick %d\n", t);
        fflush(stdout);
    }

    if (g_XenoMenuN2Phase == 1 && s_xenoMenuItemsOpenTick < 0) {
        s_xenoMenuItemsOpenTick = t;
        if (s_xenoMenuReorderActions) {
            itemPair = PcPort_N2c1ReadItemPair();
            printf("[xeno-port][test] N2C1 STATE-BEFORE: "
                   "row0=(ID%u,qty%u) row1=(ID%u,qty%u)\n",
                   itemPair & 0xFF, (itemPair >> 8) & 0xFF,
                   (itemPair >> 16) & 0xFF, (itemPair >> 24) & 0xFF);
            fflush(stdout);
        }
        if (s_xenoMenuPromptActions) {
            HASH_REGION(characterHashBefore, 0x26C, 0x70C);
            HASH_REGION(inventoryHashBefore, 0x1F90, 0x12C);
            printf("[xeno-port][test] N2C2A STATE-BEFORE: "
                   "inventory=%08x characters=%08x\n",
                   inventoryHashBefore, characterHashBefore);
            fflush(stdout);
        }
    }

    if (s_xenoMenuReorderActions && s_xenoMenuItemsOpenTick >= 0) {
        if (!selectInjected && t >= s_xenoMenuItemsOpenTick + 8) {
            selectInjected = 1;
            g_C1ButtonStateReleased |= 0x20;  /* CTRL_BTN_CIRCLE */
            printf("[xeno-port][test] N2C1 REORDER: Circle select row 0 "
                   "at reader tick %d\n", t);
            fflush(stdout);
        }
        if (!reorderInjected && t >= s_xenoMenuItemsOpenTick + 32) {
            reorderInjected = 1;
            g_C1ButtonStateReleased |= 0x20;  /* CTRL_BTN_CIRCLE */
            printf("[xeno-port][test] N2C1 REORDER: Circle confirm row 1 "
                   "at reader tick %d\n", t);
            fflush(stdout);
        }
        if (reorderInjected && !reorderObserved) {
            itemPair = PcPort_N2c1ReadItemPair();
            if (itemPair == 0x0B010C02U) {
                reorderObserved = 1;
                printf("[xeno-port][test] N2C1 STATE-AFTER: "
                       "row0=(ID%u,qty%u) row1=(ID%u,qty%u)\n",
                       itemPair & 0xFF, (itemPair >> 8) & 0xFF,
                       (itemPair >> 16) & 0xFF,
                       (itemPair >> 24) & 0xFF);
                fflush(stdout);
            }
        }
    }

    if (s_xenoMenuPromptActions && s_xenoMenuItemsOpenTick >= 0 &&
        !selectInjected && t >= s_xenoMenuItemsOpenTick + 8) {
        selectInjected = 1;
        g_C1ButtonStateReleased |= 0x20;
        printf("[xeno-port][test] N2C2A PROMPT: Circle select row 0 "
               "at reader tick %d\n", t);
        fflush(stdout);
    }
    if (s_xenoMenuPromptActions && selectInjected && !promptInjected &&
        t >= s_xenoMenuItemsOpenTick + 24) {
        promptInjected = 1;
        g_C1ButtonStateReleased |= 0x20;
        printf("[xeno-port][test] N2C2A PROMPT: Circle confirm same row "
               "at reader tick %d\n", t);
        fflush(stdout);
    }
    if (s_xenoMenuPromptActions && g_XenoMenuN2c2Phase == 1 &&
        promptTick < 0) {
        promptTick = t;
        printf("[xeno-port][test] N2C2A PROMPT-OPEN: default target stable "
               "at reader tick %d\n", t);
        fflush(stdout);
    }
    if (s_xenoMenuPromptActions && promptTick >= 0 &&
        !promptCancelInjected && t >= promptTick + 45) {
        promptCancelInjected = 1;
        g_C1ButtonStateReleased |= 0x40;
        printf("[xeno-port][test] N2C2A PROMPT: Cross cancel "
               "at reader tick %d\n", t);
        fflush(stdout);
    }
    if (s_xenoMenuPromptActions && g_XenoMenuN2c2Phase == 2 &&
        !promptObserved) {
        unsigned int inventoryHashAfter;
        unsigned int characterHashAfter;
        HASH_REGION(characterHashAfter, 0x26C, 0x70C);
        HASH_REGION(inventoryHashAfter, 0x1F90, 0x12C);
        promptObserved = 1;
        printf("[xeno-port][test] N2C2A STATE-AFTER: "
               "inventory=%08x characters=%08x unchanged=%s\n",
               inventoryHashAfter, characterHashAfter,
               (inventoryHashAfter == inventoryHashBefore &&
                characterHashAfter == characterHashBefore) ? "yes" : "NO");
        fflush(stdout);
    }

    if (!cancelInjected && s_xenoMenuItemsOpenTick >= 0 &&
        ((!s_xenoMenuReorderActions && !s_xenoMenuPromptActions &&
          t >= s_xenoMenuItemsOpenTick + 45) ||
         (s_xenoMenuReorderActions && reorderObserved &&
          t >= s_xenoMenuItemsOpenTick + 60) ||
         (s_xenoMenuPromptActions && promptObserved &&
          t >= promptTick + 75))) {
        cancelInjected = 1;
        g_C1ButtonStateReleased |= 0x40;  /* CTRL_BTN_CROSS */
        printf("[xeno-port][test] XENO_MENU_NAV_TEST=%s: Cross cancel "
               "at reader tick %d\n",
               s_xenoMenuReorderActions ? "items-reorder" :
               (s_xenoMenuPromptActions ? "items-prompt" : "items"), t);
        fflush(stdout);
    }
    #undef HASH_REGION
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

    /* Synthetic menu-nav edges (no-op unless XENO_MENU_NAV_TEST=N). MUST run
     * after the pad buffer is refreshed and before ControllerPoll derives edges
     * from it -- it pokes DPAD-DOWN into g_C1Buffer for one frame. */
    PcPort_ForcedMenuNav();

    { extern void ControllerPoll(void);   ControllerPoll();   }
    PcPort_ForcedMenuActionEdges();
    /* Retail's per-vblank handler func_8003634C pairs ControllerPoll with
     * ControllerPushState (asm 80036368/80036370). FieldPollControllers reads
     * input only by draining that queue via ControllerPopState, so without the
     * push the field never sees any input. */
    { extern void ControllerPushState(void); ControllerPushState(); }

    /* Headless menu driver (no-op unless XENO_KERNEL_SEL is set). Must run after
     * ControllerPoll, which recomputes g_C1ButtonState* each frame -- we OR the
     * synthetic Circle in afterwards so it survives to the next KernelMenuUpdate. */
    PcPort_ForcedKernelSelect();

    /* Headless resourced main-menu driver (no-op unless XENO_MENU_FORCE=1). */
    PcPort_ForcedFieldMenu();

    /* Temporary interactive camera+cull logger (XENO_CULL_CAM_LOG=1). */
    { extern void PcPort_CullCamLogOnVsync(void); PcPort_CullCamLogOnVsync(); }

    return VSync(mode);   /* then pace to the next vblank */
}
