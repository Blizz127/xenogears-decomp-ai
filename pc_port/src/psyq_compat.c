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

/* --- PsyCross internals / exports used below (extern "C") --- */
extern void PsyX_EndScene(void);  /* GR_EndScene + GR_StoreFrameBuffer + GR_SwapWindow (SDL_GL_SwapWindow) */
extern int  VSync(int mode);      /* PsyCross frame pacing; returns vblank count. Does NOT present. */
extern void DrawAllSplits(void);  /* flush queued primitives to the GL framebuffer; no-op when none */

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
    return VSync(mode);   /* then pace to the next vblank */
}
