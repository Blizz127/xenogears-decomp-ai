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
#include <stdio.h>
#include <stdlib.h>   /* getenv/atoi for the headless test hook below */
#include <libgte.h>   /* PsyCross: pull in before libgpu.h (it uses SVECTOR) */
#include <libgpu.h>   /* PsyCross: POLY_F3, setPolyF3 macro (setlen/setcode) */

/* --- PsyCross internals / exports used below (extern "C") --- */
extern void PsyX_EndScene(void);  /* GR_EndScene + GR_StoreFrameBuffer + GR_SwapWindow (SDL_GL_SwapWindow) */
extern int  VSync(int mode);      /* PsyCross frame pacing; returns vblank count. Does NOT present. */
extern void DrawAllSplits(void);  /* flush queued primitives to the GL framebuffer; no-op when none */

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
 * LZSSDecompress) without a human at the keyboard. No-op unless the env var is set.
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
        sel = (e && *e) ? atoi(e) : -1;
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

    /* Headless menu driver (no-op unless XENO_KERNEL_SEL is set). Must run after
     * ControllerPoll, which recomputes g_C1ButtonState* each frame -- we OR the
     * synthetic Circle in afterwards so it survives to the next KernelMenuUpdate. */
    PcPort_ForcedKernelSelect();

    return VSync(mode);   /* then pace to the next vblank */
}
