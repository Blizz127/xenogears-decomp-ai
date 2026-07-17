/*
 * Phase 0 entry point for the Xenogears native PC port.
 *
 * Right now this only proves the integration boundary: it links the port
 * executable against PsyCross (the PSX hardware abstraction layer) and brings
 * the runtime up and down. In Phase 1 this will hand control to the game's own
 * entry point after asset/disc setup, with PsyCross standing in for the PSX
 * GPU/SPU/GTE/CD hardware.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdatomic.h>

#include "xeno_pc.h"
#include "psx_memory.h"
#include "PsyX/PsyX_public.h"
#include "psx/libspu.h"   /* Phase-2 sound-SDK primitive probe: SpuReverbAttr/SpuCommonAttr + prims */

/* Forward-declared to avoid pulling the full PsyQ headers (libgpu needs libgte
 * first, etc.). Signatures match PsyCross. */
extern int ResetCallback(void);
extern int ResetGraph(int mode);
extern void SpuInit(void);            /* PsyCross LIBSPU: wakes the OpenAL SPU backend */

/* Phase-1 sound-pump synthetic probe (env XENO_SOUND_PUMP_PROBE=1). Registers a
 * counter callback via the real OpenEvent on the RCnt2 (counter-2) event,
 * enables it, measures the dispatch rate, disables it, and confirms dispatch
 * stops -- validating the pump architecture independent of SoundInitialize. */
extern int OpenEvent(unsigned int event, int spec, int mode, long(*func)());
extern int EnableEvent(unsigned int event);
extern int DisableEvent(unsigned int event);
extern int CloseEvent(unsigned int event);
#define PORT_RCntCNT2 0xF2000002u   /* DescRC|0x02 */
#define PORT_EvSpINT  0x0002
#define PORT_EvMdINTR 0x1000
extern int PsyX_SPUAL_IsInit(void);
/* _Atomic: incremented on the pump thread, read on main -- keeps the probe
 * counter itself out of TSan reports (the gate validation regime). */
static _Atomic long s_soundPumpProbeCount = 0;
static long PortSoundPumpProbe(void) { s_soundPumpProbeCount++; return 0; }
static void PortSleepMs(long ms) {
    struct timespec ts;
    ts.tv_sec  = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}
/* Phase-1 sound-pump synthetic probe: proves the counter-2 pump dispatches an
 * OpenEvent-registered callback at ~240Hz when enabled and stops when disabled,
 * independent of SoundInitialize (which registers the real func_8003C020 in a
 * later phase). Env-gated diagnostic; runs before MainLoop. */
static void PortRunSoundPumpProbe(void) {
    int handle;
    long c_enabled, c_disabled_start, c_disabled_end;
    int rate_ok, stop_ok;
    printf("[sound-probe] g_spuInit=%d (Phase 0, want 1)\n", PsyX_SPUAL_IsInit());
    handle = OpenEvent(PORT_RCntCNT2, PORT_EvSpINT, PORT_EvMdINTR, PortSoundPumpProbe);
    printf("[sound-probe] OpenEvent(RCntCNT2) handle=%d (want >0)\n", handle);
    s_soundPumpProbeCount = 0;
    EnableEvent(handle);
    PortSleepMs(500);
    c_enabled = s_soundPumpProbeCount;         /* ~120 ticks @ 240Hz over 0.5s */
    DisableEvent(handle);
    PortSleepMs(50);                            /* let any in-flight tick settle */
    c_disabled_start = s_soundPumpProbeCount;
    PortSleepMs(200);
    c_disabled_end = s_soundPumpProbeCount;
    CloseEvent(handle);
    rate_ok = (c_enabled > 90 && c_enabled < 150);            /* 240Hz, ~25% band */
    stop_ok = (c_disabled_end == c_disabled_start);
    printf("[sound-probe] enabled 500ms -> %ld ticks (~%.0f Hz, want ~240)\n",
           c_enabled, c_enabled / 0.5);
    printf("[sound-probe] disabled 200ms -> +%ld ticks (want 0)\n",
           c_disabled_end - c_disabled_start);
    printf("[sound-probe] RESULT: rate_ok=%d stop_ok=%d handle_ok=%d -> %s\n",
           rate_ok, stop_ok, (handle > 0),
           (rate_ok && stop_ok && handle > 0) ? "PASS" : "FAIL");
}

/* Phase-2 sound-SDK primitive probe (env XENO_SOUND_PRIM_PROBE=1). Behavioral,
 * NO oracle: exercises the 5 init-reached SDK primitives (SpuSetReverbModeType/
 * Depth, SpuSetCommonAttr, SpuSetIRQ, SpuSetIRQCallback) plus the reverb-param
 * round-trip against the awake backend and reads the state back. This validates
 * the primitives in isolation (as the pump probe validates the pump); it is NOT
 * the init happy path itself -- SoundInitialize drives them for real. */
extern float PsyX_SPUAL_GetMasterVolume(void);
static void PortSoundIrqCb1(void) {}
static void PortSoundIrqCb2(void) {}
static void PortRunSoundPrimProbe(void) {
    SpuReverbAttr rv;
    SpuCommonAttr cm;
    int t_mode, t_depthL, t_depthR, t_master, t_irq, t_cb;
    float mg;
    SpuIRQCallbackProc old1, old2;

    /* 1. reverb type + depth -> shared state -> read back via SpuGetReverbModeParam */
    SpuSetReverbModeType(SPU_REV_MODE_HALL);
    SpuSetReverbModeDepth((short)0x4000, (short)0x5000);
    memset(&rv, 0, sizeof(rv));
    SpuGetReverbModeParam(&rv);
    t_mode   = (rv.mode == SPU_REV_MODE_HALL);
    t_depthL = (rv.depth.left  == (short)0x4000);
    t_depthR = (rv.depth.right == (short)0x5000);

    /* 2. common-attr master volume -> backend listener gain (0x2000/16384 = 0.5) */
    memset(&cm, 0, sizeof(cm));
    cm.mask = SPU_COMMON_MVOLL | SPU_COMMON_MVOLR;
    cm.mvol.left  = (short)0x2000;
    cm.mvol.right = (short)0x2000;
    SpuSetCommonAttr(&cm);
    mg = PsyX_SPUAL_GetMasterVolume();
    t_master = (mg > 0.45f && mg < 0.55f);

    /* 3. IRQ enable state + callback register round-trip (registered, never fired) */
    t_irq = (SpuSetIRQ(SPU_ON) == SPU_ON);
    old1 = SpuSetIRQCallback(PortSoundIrqCb1);   /* returns previous (NULL) */
    old2 = SpuSetIRQCallback(PortSoundIrqCb2);   /* returns PortSoundIrqCb1 */
    t_cb = (old1 == NULL && old2 == PortSoundIrqCb1);
    SpuSetIRQCallback(NULL);
    SpuSetIRQ(SPU_OFF);

    printf("[sound-prim] reverb round-trip: mode=%d(HALL ok=%d) depthL=0x%04x(ok=%d) depthR=0x%04x(ok=%d)\n",
           rv.mode, t_mode, (unsigned short)rv.depth.left, t_depthL,
           (unsigned short)rv.depth.right, t_depthR);
    printf("[sound-prim] mixer master vol -> listener gain=%.3f (want ~0.5, ok=%d)\n", mg, t_master);
    printf("[sound-prim] IRQ set=%d cb round-trip ok=%d\n", t_irq, t_cb);
    printf("[sound-prim] RESULT: %s\n",
           (t_mode && t_depthL && t_depthR && t_master && t_irq && t_cb) ? "PASS" : "FAIL");
}

/* Init-proof milestone: the game's own SoundInitialize(0) routed into boot, plus
 * a coherence probe. Reads BACK audio-manager fields (not just non-null) to
 * defeat the hollow-init trap: after init, D_800595D8 must point at a manager
 * whose func_8003B32C / SoundInitializeAudioManager fields are set. */
extern void SoundInitialize(int mode);
extern unsigned int D_800595D8;         /* packed audio-manager address */
extern unsigned long g_unk_SoundEvent;  /* RCnt2 tick event handle */
static void PortRunSoundInitProbe(void) {
    unsigned int mgrAddr = D_800595D8;
    unsigned char* mgr = (unsigned char*)(uintptr_t)mgrAddr;
    int ok_addr = (mgrAddr != 0);
    int ec = -1, flags = -1, f32 = -1, f38 = -1, f18 = -1;
    int coherent;

    if (ok_addr) {
        ec    = mgr[0x14];                 /* elementCount u8, want 0x10 */
        flags = *(short*)(mgr + 0x10);     /* unk_Flags, want 2 (func_8003B32C) */
        f18   = mgr[0x18];                 /* unk_0x18 u8, want 0x7F (func_8003B32C) */
        f32   = *(short*)(mgr + 0x32);     /* unk_0x32, want 1 (SoundInitializeAudioManager) */
        f38   = *(short*)(mgr + 0x38);     /* unk_0x38, want 4 */
    }
    printf("[sound-init] D_800595D8=0x%08x (want !=0, ok=%d)\n", mgrAddr, ok_addr);
    printf("[sound-init] manager fields: elementCount=0x%x(want 0x10) flags=%d(want 2) "
           "unk18=0x%x(want 0x7F) unk32=%d(want 1) unk38=%d(want 4)\n",
           ec, flags, f18, f32, f38);
    printf("[sound-init] tick event g_unk_SoundEvent=0x%lx (want !=0)\n",
           (unsigned long)g_unk_SoundEvent);
    coherent = ok_addr && ec == 0x10 && flags == 2 && f18 == 0x7F && f32 == 1 && f38 == 4;
    printf("[sound-init] RESULT: %s\n", coherent ? "PASS (manager coherent)" : "FAIL");

    /* Post-init pump liveness: the real func_8003C020 is now registered on
     * counter-2/EvSpINT and enabled (SoundAddAudioManagerToList). Register a
     * shadow counter on the same event and measure 500ms -- the rate func_8003C020
     * rides. Confirms the pump still dispatches at 240Hz with the real tick live. */
    {
        int h = OpenEvent(PORT_RCntCNT2, PORT_EvSpINT, PORT_EvMdINTR, PortSoundPumpProbe);
        long n;
        s_soundPumpProbeCount = 0;
        EnableEvent(h);
        PortSleepMs(500);
        n = s_soundPumpProbeCount;
        DisableEvent(h);
        CloseEvent(h);
        printf("[sound-init] post-init pump: %ld ticks/500ms (~%.0f Hz, want ~240) -> tick %s\n",
               n, n / 0.5, (n > 90 && n < 150) ? "FIRING" : "NOT firing");
    }
}

/* Gate-stress probe (env XENO_SOUND_GATE_STRESS=1): the concurrency regime for
 * the sound tick gate (g_SoundTickMutex). Validates, with the REAL tick event
 * registered and its stub dispatching at 240Hz:
 *   1. the gated pump still dispatches at ~240Hz (TryLock does not starve);
 *   2. an open DisableEvent bracket freezes tick dispatch while the pump
 *      THREAD stays alive (vblank counter advances -- no pump stall), and
 *      EnableEvent resumes dispatch (retail toggle func_80037F44/F88 shape);
 *   3. under main-thread-vs-tick contention, multi-word invariants written
 *      inside brackets are never observed torn by the tick callback (and vice
 *      versa), the callback's func_8003AE84-style re-entrant bracket does not
 *      self-deadlock, and nested main-thread brackets recurse safely.
 * All shared fields are accessed ONLY under the bracket/mutex -- so a gate
 * regression shows up BOTH as an invariant violation here and as a TSan data
 * race in the XENO_TSAN=1 build. Completion of the probe is itself the
 * no-deadlock proof (a lost lock level or self-deadlock hangs it). */
extern int PsyX_Sys_GetVBlankCount(void);
/* Initialised to a tuple SATISFYING the invariants (y=2x+1, z=x^y; q=3p) so
 * ticks that run before the first main-thread write don't count as torn. */
static struct { long x, y, z; } s_gateA = { 0, 1, 1 };   /* main writes (bracket), tick reads */
static struct { long p, q; }    s_gateB = { 0, 0 };      /* tick writes (re-entrant bracket), main reads (bracket) */
static _Atomic long s_gateTicks       = 0; /* tick callback invocations */
static _Atomic long s_gateTickTorn    = 0; /* tick saw a torn s_gateA */
static _Atomic long s_gateReentries   = 0; /* completed re-entrant brackets on the tick path */
static int s_gateHandle = 0;

static long PortGateStressTick(void) {
    /* Runs on the pump thread, dispatched under g_SoundTickMutex. */
    long x = s_gateA.x, y = s_gateA.y, z = s_gateA.z;
    if (y != 2 * x + 1 || z != (x ^ y))
        s_gateTickTorn++;
    /* func_8003AE84 shape: the tick path itself re-enters the
     * DisableEvent/EnableEvent bracket. Non-recursive would self-deadlock. */
    DisableEvent(s_gateHandle);
    s_gateB.p++;
    s_gateB.q = s_gateB.p * 3;
    EnableEvent(s_gateHandle);
    s_gateReentries++;
    s_gateTicks++;
    return 0;
}

static double PortMonotonicSeconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

static void PortRunSoundGateStress(void) {
    long rate, frozen0, frozen1, resumed, iters = 0, mainTorn = 0;
    long ticksBefore, ticksDuring;
    int vbl0, vbl1;
    int rate_ok, freeze_ok, alive_ok, resume_ok, torn_ok, reentry_ok, ticked_ok;
    double t0;

    s_gateHandle = OpenEvent(PORT_RCntCNT2, PORT_EvSpINT, PORT_EvMdINTR,
                             PortGateStressTick);
    printf("[gate-stress] OpenEvent handle=%d (want >0)\n", s_gateHandle);

    /* 1. gated pump rate (no contention) */
    s_gateTicks = 0;
    EnableEvent(s_gateHandle);          /* unpaired enable: flag flip only */
    PortSleepMs(500);
    rate = s_gateTicks;
    rate_ok = (rate > 90 && rate < 150);
    printf("[gate-stress] gated pump: %ld ticks/500ms (~%.0f Hz, want ~240, ok=%d)\n",
           rate, rate / 0.5, rate_ok);

    /* 2. held bracket freezes dispatch but not the pump thread */
    DisableEvent(s_gateHandle);         /* bracket open: mutex HELD */
    frozen0 = s_gateTicks;
    vbl0 = PsyX_Sys_GetVBlankCount();
    PortSleepMs(200);
    frozen1 = s_gateTicks;
    vbl1 = PsyX_Sys_GetVBlankCount();
    EnableEvent(s_gateHandle);          /* bracket close */
    PortSleepMs(200);
    resumed = s_gateTicks;
    freeze_ok = (frozen1 == frozen0);
    alive_ok  = (vbl1 > vbl0);
    resume_ok = (resumed > frozen1 + 20);
    printf("[gate-stress] bracket held 200ms: +%ld ticks (want 0, ok=%d); "
           "vblank advanced %d (want >0: pump thread alive, ok=%d); "
           "resumed +%ld after release (ok=%d)\n",
           frozen1 - frozen0, freeze_ok, vbl1 - vbl0, alive_ok,
           resumed - frozen1, resume_ok);

    /* 3. contention: hammer brackets + invariants from main for ~2s */
    ticksBefore = s_gateTicks;
    t0 = PortMonotonicSeconds();
    while (PortMonotonicSeconds() - t0 < 2.0) {
        long i = ++iters, p, q;
        DisableEvent(s_gateHandle);     /* bracket open */
        s_gateA.x = i;                  /* multi-word write the tick must */
        s_gateA.y = 2 * i + 1;          /* never observe half-done       */
        s_gateA.z = i ^ (2 * i + 1);
        DisableEvent(s_gateHandle);     /* nested bracket (recursion) */
        p = s_gateB.p; q = s_gateB.q;
        if (q != p * 3)
            mainTorn++;
        EnableEvent(s_gateHandle);      /* close nested */
        EnableEvent(s_gateHandle);      /* close outer */
        if ((iters & 0x3FF) == 0)
            PortSleepMs(1);             /* windows for the tick to win the lock */
    }
    ticksDuring = s_gateTicks - ticksBefore;
    torn_ok    = (s_gateTickTorn == 0 && mainTorn == 0);
    reentry_ok = (s_gateReentries > 0);
    ticked_ok  = (ticksDuring > 50);
    printf("[gate-stress] contention 2s: %ld bracket pairs, %ld ticks ran (ok=%d), "
           "%ld re-entrant brackets (ok=%d)\n",
           iters, ticksDuring, ticked_ok, (long)s_gateReentries, reentry_ok);
    printf("[gate-stress] torn reads: tick=%ld main=%ld (want 0/0, ok=%d)\n",
           (long)s_gateTickTorn, mainTorn, torn_ok);

    DisableEvent(s_gateHandle);
    CloseEvent(s_gateHandle);           /* dissolves the open bracket (probe shape) */

    /* 4. post-close: the CloseEvent bracket-dissolve must have released the
     * mutex -- the REAL tick (func_8003C020 stub, enabled by SoundInitialize)
     * must still be dispatching or every later tick is silenced. */
    s_soundPumpProbeCount = 0;
    {
        int h = OpenEvent(PORT_RCntCNT2, PORT_EvSpINT, PORT_EvMdINTR,
                          PortSoundPumpProbe);
        long n;
        EnableEvent(h);
        PortSleepMs(300);
        n = s_soundPumpProbeCount;
        DisableEvent(h);
        CloseEvent(h);
        printf("[gate-stress] post-close pump: %ld ticks/300ms (want >40: close "
               "released the held bracket, ok=%d)\n", n, n > 40);
        rate_ok = rate_ok && (n > 40);
    }

    printf("[gate-stress] RESULT: %s (rate=%d freeze=%d alive=%d resume=%d "
           "torn=%d reentry=%d ticked=%d)\n",
           (rate_ok && freeze_ok && alive_ok && resume_ok && torn_ok &&
            reentry_ok && ticked_ok) ? "PASS" : "FAIL",
           rate_ok, freeze_ok, alive_ok, resume_ok, torn_ok, reentry_ok,
           ticked_ok);
}

/* Synthetic sequence-command probe (env XENO_SOUND_SEQ_PROBE=1): tick-leg
 * step 3 validation. Real sequence data needs WDS/B5 (deferred), so this
 * feeds a hand-built command stream to the live 240Hz interpreter instead:
 * element 0 of the initialized manager is pointed at the stream and activated
 * under the DisableEvent/EnableEvent bracket, the tick's func_8003C6E8 then
 * dispatches through the host g_SoundScriptHandlers table. Five opcodes with
 * five distinct observable effects prove TABLE ROUTING (a mis-routed opcode
 * writes the wrong field and/or desynchronizes the stream so the final IP
 * check fails):
 *   0x97 func_8003CE68  time signature   -> manager unk_0x3a/3c/38
 *   0xA0 func_8003D0E8  volume immediate -> manager unk_0x58
 *   0xE9 func_8003DEB4  pan nudge        -> element +0x74
 *   0xC1 func_8003D60C  raw ADSR         -> element +0x54..56
 *   0xA9 SoundScriptSetUnk62 (pre-batch {}) -> element +0x62
 *   0x80 func_8003CD08  rest             -> fermata + REST exit
 * SYNTHETIC data, labeled as such: full live exercise with real sequence
 * banks awaits the WDS/B5 leg. Diagnostic only; does not run in normal boot. */
static const unsigned char s_seqProbeStream[] = {
    0x97, 0x03, 0x04,       /* time signature (func_8003CE68)          */
    0xA0, 0x55,             /* volume immediate (func_8003D0E8)        */
    0x98, 0x02,             /* loop start, 2 iterations (func_8003CEF0) */
    0xA1, 0x03,             /* volume nudge +3<<16 in-loop (func_8003D110) */
    0x99,                   /* loop continue (func_8003CF38); exhausts + pops
                             * after 2 iterations. NOTE: 0x9A is an early-exit-
                             * INSIDE-loop construct, not a terminator (3d) */
    0xE9, 0x10,             /* pan nudge -> 0x1000 (func_8003DEB4)     */
    0xC1, 0x11, 0x22, 0x33, /* raw ADSR (func_8003D60C)                */
    0xA9, 0x44,             /* unk62 (SoundScriptSetUnk62)             */
    0xE1, 0x05,             /* vib accumulator nudge (func_8003DB58)   */
    0xA7, 0x02, 0x60,       /* channel fade: cnt 0x40, tgt 0x6000 (func_8003D1BC) */
    0xD4, 0x04, 0x20,       /* pitch slide arm (func_8003D7FC)         */
    0xEA, 0x08, 0x30,       /* pan fade, completes at scaled 0x2000 (func_8003DEE4 + C4C4 fade path) */
    0xFD, 0x02,             /* master level (func_8003E4BC): restores the
                             * tempo product the in-loop 0xA1 zeroed      */
    0x80, 0xFF,             /* rest (func_8003CD08) -- terminates; 0xFF
                             * outlasts the probe window (guard stays unread
                             * by the interpreter)                        */
    0x00,                   /* scan-safe epilogue: in-bounds <0x80 byte for
                             * C6E8's post-pass tie-scan (3c finding)    */
};
static void PortRunSoundSeqProbe(void) {
    unsigned char* mgr = (unsigned char*)(uintptr_t)D_800595D8;
    unsigned char* el = mgr + 0x94;
    int save58, save54, save50;
    unsigned short save3a, save3c, save38, save36;
        int ok_route, ok_mgr, ok_el, ok_ip, ok_ticked;

    if (mgr == NULL) {
        printf("[seq-probe] no manager (D_800595D8==0) -> SKIP\n");
        return;
    }
    /* Arm under the bracket: script IP -> stream, element active, manager
     * sequencing enabled at one step per tick. */
    DisableEvent(g_unk_SoundEvent);
    save58 = *(int*)(mgr + 0x58); save54 = *(int*)(mgr + 0x54);
    save50 = *(int*)(mgr + 0x50);
    save3a = *(unsigned short*)(mgr + 0x3A); save3c = *(unsigned short*)(mgr + 0x3C);
    save38 = *(unsigned short*)(mgr + 0x38); save36 = *(unsigned short*)(mgr + 0x36);
    *(unsigned short*)(mgr + 0x36) = 4;
    *(int*)(mgr + 0x48) = 1;
    *(int*)(mgr + 0x50) = 0;
    *(int*)(mgr + 0x54) = 0x10000;
    *(unsigned int*)(el + 0x14) = (unsigned int)(uintptr_t)s_seqProbeStream;  /* unk14 script IP */
    *(unsigned short*)(el + 0x02) = 0;                                  /* status */
    *(int*)(el + 0x5C) = 0;                                  /* fermata pair */
    *(unsigned short*)(el + 0x00) = 0x1;                                /* active */
    *(short*)(mgr + 0x10) |= 0x8000;                           /* manager active */
    EnableEvent(g_unk_SoundEvent);

    s_soundPumpProbeCount = 0;
    PortSleepMs(100);   /* ~24 ticks; the stream completes on the first step */
    DisableEvent(g_unk_SoundEvent);

    ok_mgr = (*(unsigned short*)(mgr + 0x3A) == 0x30) && (*(unsigned short*)(mgr + 0x3C) == 4) &&
             (*(unsigned short*)(mgr + 0x38) == 3) && (*(int*)(mgr + 0x58) == 0x5B0000)   /* 0xA0 + loop 2x 0xA1 (+3<<16): CEF0/CF38/CFA4 live */;
    ok_el = (*(unsigned short*)(el + 0x74) == 0x2000) &&   /* pan fade completed (retail sets scaled delta on final step) */
            (*(unsigned char*)(el + 0x54) == 0x11) && (*(unsigned char*)(el + 0x55) == 0x22) &&
            (*(unsigned char*)(el + 0x56) == 0x33) && (*(unsigned short*)(el + 0x62) == 0x44) &&
            (*(int*)(el + 0x78) == 0x05000000) &&               /* vib acc (0xE1) */
            (*(int*)(el + 0x84) == 0x08000000) &&               /* slide step (0xD4) */
            (*(unsigned short*)(mgr + 0x7A) == 0x6000);          /* interp70 target (0xA7) */
    ok_ip = (*(unsigned int*)(el + 0x14) ==
             (unsigned int)(uintptr_t)(s_seqProbeStream + sizeof(s_seqProbeStream) - 1));
    {
        unsigned short fermata = *(unsigned short*)(el + 0x5C);
        ok_ticked = (fermata > 0 && fermata <= 0xFF);
    }
    ok_route = ok_mgr && ok_el && ok_ip;
    printf("[seq-probe] mgr: sig=%x/%x beats=%x vol58=%08x (ok=%d)\n",
           *(unsigned short*)(mgr + 0x3A), *(unsigned short*)(mgr + 0x3C), *(unsigned short*)(mgr + 0x38),
           *(int*)(mgr + 0x58), ok_mgr);
    printf("[seq-probe] el: pan=%04x adsr=%02x/%02x/%02x unk62=%04x (ok=%d)\n",
           *(unsigned short*)(el + 0x74), *(unsigned char*)(el + 0x54), *(unsigned char*)(el + 0x55),
           *(unsigned char*)(el + 0x56), *(unsigned short*)(el + 0x62), ok_el);
    printf("[seq-probe] ip advanced to end=%d fermata=0x%x counting=%d\n",
           ok_ip, *(unsigned short*)(el + 0x5C), ok_ticked);
    printf("[seq-probe] RESULT: %s (routing=%d, SYNTHETIC stream; real "
           "sequence data awaits WDS/B5)\n",
           (ok_route && ok_ticked) ? "PASS" : "FAIL", ok_route);

    /* Teardown: park the element + manager back to the pre-probe state. */
    *(unsigned short*)(el + 0x00) = 0;
    *(unsigned short*)(el + 0x02) = 0;
    *(int*)(el + 0x5C) = 0;
    *(unsigned int*)(el + 0x14) = 0;
    *(short*)(mgr + 0x10) &= 0x7FFF;
    *(int*)(mgr + 0x48) = 0;
    *(int*)(mgr + 0x58) = save58; *(int*)(mgr + 0x54) = save54;
    *(int*)(mgr + 0x50) = save50;
    *(unsigned short*)(mgr + 0x3A) = save3a; *(unsigned short*)(mgr + 0x3C) = save3c;
    *(unsigned short*)(mgr + 0x38) = save38; *(unsigned short*)(mgr + 0x36) = save36;
    EnableEvent(g_unk_SoundEvent);
}

/* B5.1 pass 2 -- the register->backend translator (FIRST AUDIBLE wiring).
 * The tick's voice-register flush (func_8003E900/EB5C, objdiff {}) writes
 * key-on/off + voice params into the static SpuUnion backing page; the real
 * backend is driven by SpuSetVoiceAttr/SpuSetKey (-> alSourcePlay). This
 * translator is registered as a second counter-2 event (OpenEvent slot AFTER
 * the tick's), so the pump dispatches it at 240Hz right after func_8003C020,
 * under the same g_SoundTickMutex bracket -- gate-serialized by construction.
 * Edge semantics: real KON/KOFF registers are write-triggered; the translator
 * consumes (clears) the page words after driving the backend. PsyX SpuVoiceAttr
 * supports VOLL/VOLR/PITCH/WDSA (no raw ADSR -- envelope shaping is absent
 * until the backend grows it; volume/pitch/addr suffice for audible). */
static _Atomic long s_regFlushKeyOns = 0;
static _Atomic long s_regFlushKeyOffs = 0;
static long PcPort_SpuRegFlushTick(void) {
    extern void* g_pSoundSpuRegisters;
    unsigned char* spu = (unsigned char*)g_pSoundSpuRegisters;
    unsigned int kon = *(unsigned short*)(spu + 0x188) |
                       (*(unsigned short*)(spu + 0x18A) << 16);
    unsigned int koff = *(unsigned short*)(spu + 0x18C) |
                        (*(unsigned short*)(spu + 0x18E) << 16);
    if (kon) {
        int i;
        for (i = 0; i < 24; i++) {
            if (kon & (1u << i)) {
                unsigned char* v = spu + i * 0x10;
                SpuVoiceAttr attr;
                memset(&attr, 0, sizeof(attr));
                attr.voice = 1u << i;
                attr.mask = SPU_VOICE_VOLL | SPU_VOICE_VOLR | SPU_VOICE_PITCH |
                            SPU_VOICE_WDSA;
                attr.volume.left = *(short*)(v + 0x0);
                attr.volume.right = *(short*)(v + 0x2);
                attr.pitch = *(unsigned short*)(v + 0x4);
                attr.addr = *(unsigned short*)(v + 0x6) << 3;
                if (s_regFlushKeyOns < 4)
                    printf("[reg-flush] KON v%d voll=%d volr=%d pitch=0x%x "
                           "addr=0x%x\n", i, attr.volume.left,
                           attr.volume.right, (unsigned)attr.pitch,
                           (unsigned)attr.addr);
                SpuSetVoiceAttr(&attr);
            }
        }
        SpuSetKey(SPU_ON, kon);
        s_regFlushKeyOns += 1;
        *(unsigned short*)(spu + 0x188) = 0;
        *(unsigned short*)(spu + 0x18A) = 0;
    }
    if (koff) {
        SpuSetKey(SPU_OFF, koff);
        s_regFlushKeyOffs += 1;
        *(unsigned short*)(spu + 0x18C) = 0;
        *(unsigned short*)(spu + 0x18E) = 0;
    }
    return 0;
}

/* B5.1 play probe (env XENO_SOUND_PLAY_PROBE=1): with the real WDS bank
 * loaded (pass 1), arm element 0 with a note-on stream (bank+instrument via
 * 0xFC, note 0x30 vel 0x05, long rest). The tick interprets, keys the voice
 * on, the translator drives the backend -> alSourcePlay. Proof tiers:
 * translator counters here; the WAV-capture RMS check is the runner's job
 * (ALSOFT wave backend). SYNTHETIC note; real sequences await field music. */
static const unsigned char s_playProbeStream[] = {
    0xFC, 0x00, 0x00,   /* bank (list head fallback) + instrument 0 */
    0xE0, 0x7F,         /* element volume accumulator = 0x7F<<24 (el+0x78;
                         * its hi16 feeds the voll chain -- 0 == silence) */
    0x30, 0x05,         /* note 0x30, velocity 5 (duration table: 64 steps) */
    0x80, 0xFF,         /* rest outlasting the window */
    0x00,               /* scan-safe guard */
};
static void PortRunSoundPlayProbe(void) {
    unsigned char* mgr = (unsigned char*)(uintptr_t)D_800595D8;
    unsigned char* el = mgr + 0x94;
    long kons0 = s_regFlushKeyOns;
    int ok_keyed, ok_chan, ok_playing;
    extern void* g_SoundChannels[24];

    if (mgr == NULL) { printf("[play-probe] no manager -> SKIP\n"); return; }
    DisableEvent(g_unk_SoundEvent);
    *(unsigned short*)(mgr + 0x36) = 4;
    *(int*)(mgr + 0x48) = 1;
    *(int*)(mgr + 0x50) = 0;
    *(int*)(mgr + 0x54) = 0x4000;                 /* gentle tempo */
    *(int*)(mgr + 0x70) = 0x7FFF0000;             /* channel level interp
                                                   * (hi16 scales every voll/
                                                   * volr; 0 == silence) */
    *(unsigned int*)(el + 0x14) = (unsigned int)(uintptr_t)s_playProbeStream;
    *(unsigned short*)(el + 0x02) = 0;
    *(int*)(el + 0x5C) = 0;
    *(unsigned char*)(el + 0x27) = 2;             /* voice_number = 2 */
    *(short*)(el + 0x76) = 0x7FFF;                /* expression factor (st[0x3A]):
                                                   * set by the unported song-
                                                   * start path; the probe stands
                                                   * in for it (0 == silence) */
    *(unsigned short*)(el + 0x00) = 0x401;        /* active + RESTING: the note
                                                   * path keys on via status|=1
                                                   * only from the rest state */
    *(short*)(mgr + 0x10) |= 0x8000;
    EnableEvent(g_unk_SoundEvent);
    {
        /* AL source-state tier: poll the backend during the window; the voice
         * must be observed AL_PLAYING at least once. */
        extern int SpuGetKeyStatus(unsigned int voice_bit);
        int t;
        ok_playing = 0;
        for (t = 0; t < 30; t++) {
            PortSleepMs(10);
            if (SpuGetKeyStatus(1u << 2))
                ok_playing = 1;
        }
    }
    DisableEvent(g_unk_SoundEvent);
    ok_keyed = (s_regFlushKeyOns > kons0);
    ok_chan = (g_SoundChannels[2] != NULL);
    printf("[play-probe] keyons=%ld (delta ok=%d) chan2=%d vd.start=0x%x pitch=0x%x\n",
           (long)s_regFlushKeyOns, ok_keyed, ok_chan,
           *(unsigned int*)(el + 0x4C), *(unsigned short*)(el + 0x44));
    printf("[play-probe] vol-chain: el76=%d el7A=%d elD2=%d mgr70hi=%d "
           "vd.voll=%d vd.volr=%d\n",
           *(short*)(el + 0x76), *(short*)(el + 0x7A), *(short*)(el + 0xD2),
           ((short*)(mgr + 0x70))[1], *(short*)(el + 0x38), *(short*)(el + 0x3A));
    printf("[play-probe] al-source-state: observed playing=%d (want 1)\n",
           ok_playing);
    printf("[play-probe] RESULT: %s (voice keyed + AL source PLAYING; WAV RMS "
           "is the output-tier proof -- see runner)\n",
           (ok_keyed && ok_chan && ok_playing) ? "PASS" : "FAIL");
    /* teardown */
    *(unsigned short*)(el + 0x00) = 0;
    *(unsigned short*)(el + 0x02) = 0;
    *(int*)(el + 0x5C) = 0;
    *(unsigned int*)(el + 0x14) = 0;
    *(short*)(mgr + 0x10) &= 0x7FFF;
    *(int*)(mgr + 0x48) = 0;
    *(int*)(mgr + 0x54) = 0;
    EnableEvent(g_unk_SoundEvent);
}

/* Song-start M2 probe (env XENO_SOUND_SONG_PROBE=1): buffer-read a REAL
 * song pair from archive dir 0x1C -- WDS bank (default file 0x13, 'wds ')
 * via the proven SoundLoadWdsFile path, then the 'smds' song file (default
 * 0x14) -- and drive the M1 arming chain exactly as retail field code does:
 * func_80039850 (create manager from the song file) -> func_80039A80
 * (start: header init + element arm + level 0x7F + running flag). The
 * already-ported tick then interprets the real sequence at 240Hz and the
 * register->backend translator keys voices. Observation: per-second voice/
 * key-on sampling here; the WAV-capture RMS profile is the runner's proof
 * tier. Teardown via func_80039C4C + func_800399D4 (the real destroy path).
 * Env overrides: XENO_SOUND_SONG_FILE / XENO_SOUND_BANK_FILE (hex). */
static void PortRunSoundSongProbe(void) {
    extern void* func_80039850(void* pSongFile);
    extern void func_80039A80(void* manager, int level, int steps);
    extern void func_80039C4C(void* manager);
    extern void func_800399D4(void* manager);
    extern void* SoundLoadWdsFile(void* pWdsFile, int mode);
    extern int SpuGetKeyStatus(unsigned int voice_bit);
    extern int ArchiveDecodeAlignedSize(int fileIndex);
    extern void* HeapAlloc(int size, int flags);
    extern void HeapFree(void* pMemory);
    extern int ArchiveSetIndex(int directoryIndex, int entryIndex);
    extern void ArchiveReadFileToBuffer(int fileIndex, void* pBuffer, int arg2,
                                        int arg3);
    int bankFile = 0x13, songFile = 0x14;
    unsigned char* bankEntry;
    unsigned char* mgr;
    void* bankBuf;
    unsigned char* songBuf;
    int sz, t;
    long kons0, koffs0;
    int ok_mgr, ok_run, ok_played, ok_seq;
    int maxVoices = 0, samplesWithSound = 0;

    if (getenv("XENO_SOUND_BANK_FILE")) bankFile = (int)strtol(getenv("XENO_SOUND_BANK_FILE"), NULL, 16);
    if (getenv("XENO_SOUND_SONG_FILE")) songFile = (int)strtol(getenv("XENO_SOUND_SONG_FILE"), NULL, 16);

    ArchiveSetIndex(0x1C, 0);
    sz = ArchiveDecodeAlignedSize(bankFile);
    bankBuf = HeapAlloc(sz, 1);
    ArchiveReadFileToBuffer(bankFile, bankBuf, 0, 0x80);
    bankEntry = (unsigned char*)SoundLoadWdsFile(bankBuf, 0);
    printf("[song-probe] bank file 0x%02x: size=%d entry=%d id=0x%x spuAddr=0x%x\n",
           bankFile, sz, bankEntry != NULL,
           bankEntry ? *(unsigned short*)(bankEntry + 0x20) : 0,
           bankEntry ? *(unsigned int*)(bankEntry + 0x28) : 0);
    HeapFree(bankBuf);
    if (bankEntry == NULL) {
        printf("[song-probe] RESULT: FAIL (bank load)\n");
        return;
    }

    sz = ArchiveDecodeAlignedSize(songFile);
    songBuf = HeapAlloc(sz, 1);
    ArchiveReadFileToBuffer(songFile, songBuf, 0, 0x80);
    printf("[song-probe] song file 0x%02x: size=%d magic=%.4s elemCnt=%d wdsId=0x%x\n",
           songFile, sz, (char*)songBuf, songBuf[0x14],
           *(unsigned short*)(songBuf + 0x16));

    kons0 = s_regFlushKeyOns;
    koffs0 = s_regFlushKeyOffs;
    mgr = (unsigned char*)func_80039850(songBuf);
    ok_mgr = (mgr != NULL);
    printf("[song-probe] manager=%d elemCnt=%d flags=0x%x (create)\n",
           ok_mgr, ok_mgr ? mgr[0x14] : 0,
           ok_mgr ? *(unsigned short*)(mgr + 0x10) : 0);
    if (!ok_mgr) {
        printf("[song-probe] RESULT: FAIL (manager create -- M1 guard territory)\n");
        HeapFree(songBuf);
        ArchiveSetIndex(4, 0);
        return;
    }
    if (getenv("XENO_SOUND_SONG_CONTROL")) {
        /* Silent-control tier: manager created, song NEVER started -- the
         * wave capture must be digitally silent. */
        printf("[song-probe] CONTROL: created but not started\n");
        PortSleepMs(8000);
        printf("[song-probe] CONTROL keyons=%ld (want 0)\n",
               (long)(s_regFlushKeyOns - kons0));
        func_800399D4(mgr);
        HeapFree(songBuf);
        ArchiveSetIndex(4, 0);
        return;
    }
    func_80039A80(mgr, 0x7F, 0);
    ok_run = ((*(unsigned short*)(mgr + 0x10) & 0x8000) != 0);
    printf("[song-probe] started: flags=0x%x run=%d mgr70hi=%d\n",
           *(unsigned short*)(mgr + 0x10), ok_run, ((short*)(mgr + 0x70))[1]);

    /* 8 seconds of real playback, sampled twice a second. */
    for (t = 0; t < 16; t++) {
        int v, playing = 0;
        PortSleepMs(500);
        for (v = 0; v < 24; v++) {
            playing += (SpuGetKeyStatus(1u << v) != 0);
        }
        if (playing > maxVoices) maxVoices = playing;
        if (playing > 0) samplesWithSound++;
        printf("[song-probe] t=%2d.%ds voices=%2d keyons=%ld keyoffs=%ld\n",
               (t + 1) / 2, ((t + 1) % 2) * 5, playing,
               (long)(s_regFlushKeyOns - kons0),
               (long)(s_regFlushKeyOffs - koffs0));
    }
    ok_played = (s_regFlushKeyOns - kons0) > 4 && maxVoices > 0;
    ok_seq = (s_regFlushKeyOns - kons0) > 16 && samplesWithSound >= 8 && maxVoices >= 2;
    printf("[song-probe] totals: keyons=%ld keyoffs=%ld maxVoices=%d "
           "samplesWithSound=%d/16\n",
           (long)(s_regFlushKeyOns - kons0), (long)(s_regFlushKeyOffs - koffs0),
           maxVoices, samplesWithSound);
    printf("[song-probe] RESULT: %s (create=%d run=%d played=%d sequence=%d; "
           "WAV RMS profile is the output-tier proof -- see runner)\n",
           (ok_mgr && ok_run && ok_played && ok_seq) ? "PASS" : "FAIL",
           ok_mgr, ok_run, ok_played, ok_seq);

    func_80039C4C(mgr);
    func_800399D4(mgr);
    HeapFree(songBuf);
    ArchiveSetIndex(4, 0);
}

#define WINDOW_TITLE  "Xenogears (PC port)"
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480

/* Decompiled game entry (src/slus_006.64/main/main_loop.c). */
extern void MainLoop(int errorCode);
/* Port-side runtime build of the game-state dispatch table (game_overrides.c). */
extern void PcPort_InitGameStates(void);
/* Port-side one-time HeapInit the asm boot would have done (game_overrides.c). */
extern void PcPort_HeapBoot(void);
/* Original boot global-state initializer called by func_80019578 before MainLoop. */
extern void func_8001AADC(void);
/* Field dialog-box interior darkening color (subtractive-blend TILE color), set in
 * .bss at retail boot by func_8001BB50 (also called from func_80019578). */
extern unsigned char D_800594D4;
extern unsigned char D_800594D5;
extern unsigned char D_800594D6;
/* "Published by Square" splash, decompressed + drawn from the migrated EXE data. */
extern void GameShowSplashScreen(void);

/* Input wiring. The game reads its BIOS controller buffer g_C1Buffer directly
 * (system/controller.c: ControllerGetButtonState reads [status,type,btn,btn] at
 * stride 0x22). On PSX the BIOS auto-fills it each vblank after InitPAD/StartPAD;
 * those are asm (bypassed boot) and PsyCross's InitPAD/PadRead are unimplemented.
 * PsyCross's PADRAW layout (status,id,buttons[2],analog[4]) matches the game's
 * buffer byte-for-byte, so we register g_C1Buffer's two pad slots with PsyX_Pad
 * and enable pad comms here. The per-frame refresh (PsyX_UpdateInput +
 * ControllerPoll) is driven from the Vsync shim in psyq_compat.c. */
extern unsigned char g_C1Buffer[];
extern void PsyX_Pad_InitPad(int slot, unsigned char* padData);
extern int g_padCommEnable;
#define PORT_CONTROLLER_BUFFER_SIZE 0x22

/* Disc / archive bring-up. The asm boot (func_80019578) calls
 * ArchiveInit(&D_80010004 [table buf], &D_80018004 [header buf], 0 [CD path])
 * after HeapInit; it CdInit()s and reads the archive index off the disc (sectors
 * 0x18/0x28). The port replaces the async CD path with synchronous PsyCross-libcd
 * reads (archive_port.c), so we just point PsyCross at the disc image and make the
 * same call. ArchiveReadFileToBuffer (used by the menu/field overlay loads) then
 * pulls real file data straight from disc1.bin. Gated on the image being present
 * so a disc-less run still boots to the menu as before. */
extern void PsyX_CDFS_Init(const char* imageFileName, int track, int sectorSize);
extern void ArchiveInit(unsigned int pArchiveTable, unsigned int pHeaderTable,
                        unsigned int pDebugTable);
extern int ArchiveSetIndex(int directoryIndex, int entryIndex);
extern int ArchiveDecodeSize(int fileIndex);
extern void ArchiveReadFileToBuffer(int fileIndex, void* pBuffer, int arg2, int arg3);
extern int ArchiveCdDataSync(int mode);
extern void* HeapAlloc(int size, int flags);
extern void HeapFree(void* pMemory);
extern void HeapSetCurrentContentType(int contentType);
extern void* LZSSHeapDecompress(void* pCompressed, int flags);
extern void SystemInitializeFont(void* pSystemFont);
extern void SystemInitializeData(void* pSystemData);
extern void func_8001ACA4(void);
extern unsigned short D_8006F954;    /* field entrance/spawn index (sister of D_8006F94E) */
extern unsigned short D_8006F950;    /* field transition approach angle (-> var 8 camera octant) */
extern unsigned char D_80010000[];  /* build/mode flag: -1 in retail ROM        */
extern unsigned char D_80010004[];  /* archive table buffer  (g_ArchiveTable)  */
extern unsigned char D_80018004[];  /* archive header buffer (g_ArchiveHeader) */
extern unsigned short D_8006F94E;    /* field map selected by FieldMain         */

/* MODE2/2352 image; PsyCross extracts the 2048-byte data payload per sector. */
#define PORT_CD_SECTOR_SIZE 2352

/* Return the first readable disc-image path, or NULL. XENO_DISC overrides; the
 * defaults assume the binary is run from pc_port/build_native (repo disc/ dir). */
static const char* PcPort_FindDiscImage(void) {
    static const char* defaults[] = {
        "../../disc/disc1.bin", "disc/disc1.bin", "../disc/disc1.bin",
    };
    const char* env = getenv("XENO_DISC");
    unsigned i;
    FILE* f;
    if (env && *env) {
        f = fopen(env, "rb");
        if (f) { fclose(f); return env; }
    }
    for (i = 0; i < sizeof(defaults) / sizeof(defaults[0]); i++) {
        f = fopen(defaults[i], "rb");
        if (f) { fclose(f); return defaults[i]; }
    }
    return NULL;
}

static void PcPort_LoadSystemTextData(void) {
    void* pCompressed;
    void* pDecoded;

    ArchiveSetIndex(0, 1);

    pCompressed = HeapAlloc(ArchiveDecodeSize(6), 0);
    ArchiveReadFileToBuffer(6, pCompressed, 0, 0);
    ArchiveCdDataSync(0);
    HeapSetCurrentContentType(0x30);
    pDecoded = LZSSHeapDecompress(pCompressed, 1);
    SystemInitializeFont(pDecoded);
    HeapFree(pCompressed);

    pCompressed = HeapAlloc(ArchiveDecodeSize(7), 0);
    ArchiveReadFileToBuffer(7, pCompressed, 0, 0);
    ArchiveCdDataSync(0);
    HeapSetCurrentContentType(0x31);
    pDecoded = LZSSHeapDecompress(pCompressed, 1);
    SystemInitializeData(pDecoded);
    HeapFree(pCompressed);
}

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    printf("[xeno-port] booting (Silent-Hill-style: PSX RAM emu + runtime dispatch table)\n");

    /* 1. PSX main-RAM emulation must come first (PSX_ADDR targets live here). */
    PsxMemory_Init();

    /* 2. Data migration: build the game-state dispatch table at runtime. */
    PcPort_InitGameStates();

    /* 3. Bring up PsyCross (SDL2 window + OpenGL context). */
    PsyX_Initialise(WINDOW_TITLE, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    /* XENO_PC_PORT: PsyCross's default keyboard map puts Circle (the field
     * confirm/talk button) on V and Triangle (the menu button) on Z. Players
     * expect Z to confirm, so swap them: Z = Circle (confirm/talk), V = Triangle
     * (menu). This runs after PsyX_Initialise, which installs the defaults.
     * Raw SDL scancodes (Z=29, V=25) avoid pulling an SDL header in here; kc_* are
     * plain ints. Arrows (d-pad), Enter (Start), Space (Select), C (Cross),
     * X (Square) are unchanged. */
    g_cfg_keyboardMapping.kc_circle   = 29; /* SDL_SCANCODE_Z */
    g_cfg_keyboardMapping.kc_triangle = 25; /* SDL_SCANCODE_V */

    /* 4. PsyQ subsystem init normally done by the asm `start` before MainLoop. */
    ResetCallback();
    ResetGraph(0);

    /* 4a-sound (Phase 0, sound SDK): wake the OpenAL SPU backend. Retail's sound
     * cold-init reaches SpuInit() -> PsyX_SPUAL_InitSound(), which sets
     * g_spuInit=1; until then every PsyX_SPUAL_* call early-returns and the SPU
     * is dormant. Must follow PsyX_Initialise (the OpenAL context). Idempotent
     * (PsyX_SPUAL_InitSound guards on g_spuInit/g_SpuMutex). This is ONLY the
     * backend wake; the game-side SoundInitialize(0) routing is a later phase
     * and is intentionally NOT done here. */
    SpuInit();

    /* 4a-probe: Phase-1 sound-pump synthetic validation (env XENO_SOUND_PUMP_PROBE).
     * The PsyX interrupt thread is already running (started by PsyX_Initialise),
     * so the pump is live here. Diagnostic only; does not run in normal boot. */
    if (getenv("XENO_SOUND_PUMP_PROBE")) {
        PortRunSoundPumpProbe();
    }

    /* 4a-probe2: Phase-2 sound-SDK primitive validation (env XENO_SOUND_PRIM_PROBE).
     * Exercises the wired init-reached primitives + reverb round-trip against the
     * awake backend. Diagnostic only; does not run in normal boot. */
    if (getenv("XENO_SOUND_PRIM_PROBE")) {
        PortRunSoundPrimProbe();
    }

    /* 4c (init-proof milestone): route the game's own SoundInitialize(0) into
     * boot -- the func_80019578 duty port_main did not replicate. Wakes the sound
     * manager: allocates the audio-manager (D_800595D8), sets up its sound heap,
     * registers the 240Hz tick func_8003C020 on the RCnt2 pump (body still
     * STUBBED), configures reverb/mixer via the wired Phase-2 primitives.
     * Cross-thread safe while the tick body is stubbed -- the dispatched stub
     * touches no shared state; SPU-IRQ / real tick body remain the gate before
     * the tick leg. WDS/playback (SoundLoadWdsFile) intentionally NOT routed. */
    SoundInitialize(0);

    /* B5.1: register the register->backend translator on the pump (slot after
     * the tick's; dispatched at 240Hz under the same gate bracket). Always-on
     * port wiring -- this is what makes key-ons audible. */
    {
        int hFlush = OpenEvent(PORT_RCntCNT2, PORT_EvSpINT, PORT_EvMdINTR,
                               PcPort_SpuRegFlushTick);
        EnableEvent(hFlush);
    }
    if (getenv("XENO_SOUND_INIT_PROBE")) {
        PortRunSoundInitProbe();
    }

    /* 4d-probe (tick-leg step 1, gate-first): concurrency validation of the
     * sound tick gate (g_SoundTickMutex; psycross_sound_gate.patch). Runs with
     * the real func_8003C020 registered + enabled (its stub dispatches under
     * the gate throughout). Diagnostic only; does not run in normal boot. */
    if (getenv("XENO_SOUND_GATE_STRESS")) {
        PortRunSoundGateStress();
    }

    /* 4e-probe (tick-leg step 3): synthetic sequence-command stream through
     * the live dispatch table. Diagnostic only; does not run in normal boot. */
    if (getenv("XENO_SOUND_SEQ_PROBE")) {
        PortRunSoundSeqProbe();
    }

    /* 4b. Wire controller input into the game's BIOS pad buffer (see note above). */
    PsyX_Pad_InitPad(0, &g_C1Buffer[0]);
    PsyX_Pad_InitPad(1, &g_C1Buffer[PORT_CONTROLLER_BUFFER_SIZE]);
    g_padCommEnable = 1;

    /* 5. One-time HeapInit the asm boot (func_80019578) runs before MainLoop;
     * MainLoop only HeapRelocate()s and would crash on an uninitialised heap. */
    PcPort_HeapBoot();

    /* 5a. Original boot state reset normally performed by func_80019578 before
     * entering MainLoop. The native oracle bypasses that raw asm entry point. */
    func_8001AADC();

    /* 5a-bis. func_80019578 also calls func_8001BB50, which writes the field
     * dialog-box interior darkening color into .bss. That color is applied to a
     * semi-transparent flat TILE drawn under the text with a *subtractive*
     * blend (background.drawModes use GetTPage abr=2), so the box interior =
     * framebuffer - (D4,D5,D6). func_8001BB50 is stubbed on the port and never
     * runs, leaving the bytes 0 -> the TILE subtracts (0,0,0) and no darkening
     * shows. Restore the exact retail values (func_8001BB50 asm 8001BB74-8001BB90:
     * D4=0x88, D5=0x76, D6=0x54). Consumed once by FieldTextBoxInitializePrimitives
     * (setRGB0 on background.tiles) at field enter, so set before MainLoop. */
    D_800594D4 = 0x88;
    D_800594D5 = 0x76;
    D_800594D6 = 0x54;

    /* 5b. Disc / archive init (see notes above). Only when the image is found,
     * so a disc-less run still reaches the menu instead of hanging in
     * ArchiveInit's `while (CdInit() == 0)`. */
    {
        const char* disc = PcPort_FindDiscImage();
        /* D_80010000 is static read-only ROM data (asm/.../data/800.rodata.s) with
         * value 0xFFFFFFFF; nothing ever writes it. The auto-generated stub leaves it
         * zeroed, which makes FieldMain compute g_FieldSystemMode = SYSTEM_MODE_PC_HDD
         * (0) and hit a `break 1` trap meant only for the PC-HDD dev path. Restoring
         * the real value (-1) lets the original control flow pick SYSTEM_MODE_CD_ROM
         * (1) -- not a forced mode, just the correct constant. ArchiveInit treats the
         * value as pDebugTable: -1, like 0, selects the CD path (g_ArchiveDebugTable
         * = NULL), so disc loading is unchanged. */
        *(int*)D_80010000 = -1;
        if (disc) {
            printf("[xeno-port] CD image: %s\n", disc);
            PsyX_CDFS_Init(disc, 0, PORT_CD_SECTOR_SIZE);
            /* pDebugTable MUST be 0 here, not D_80010000's -1. Retail passes -1, but
             * ArchiveInit only reads the archive table/header from CD when
             * pDebugTable == 0 (`if (!pDebugTable)`); with -1 it relies on the table
             * being statically baked into the EXE at D_80010004/D_80018004, which the
             * port's data migration does not provide. So the port reads the index
             * from disc (pDebugTable = 0); g_ArchiveDebugTable still ends up NULL. */
            ArchiveInit((unsigned int)D_80010004, (unsigned int)D_80018004, 0);
            printf("[xeno-port] ArchiveInit done (archive index loaded from disc).\n");

            /* B5.1 WDS-load probe (env XENO_SOUND_WDS_PROBE=1): replicate the
             * retail loader pair (func_80085FB8 + func_80085F30 core): read the
             * REAL WDS sample bank (archive dir 0x1C, file 3) through the
             * working archive path, SoundLoadWdsFile it, and PROVE the samples
             * landed in the backend SPU-RAM image by SpuRead-back comparison.
             * Samples LOAD only -- key-on translation (audible) is B5.1 pass 2. */
            if (getenv("XENO_SOUND_WDS_PROBE")) {
                extern int ArchiveDecodeAlignedSize(int fileIndex);
                extern unsigned int SpuSetTransferStartAddr(unsigned int addr);
                extern unsigned int SpuRead(unsigned char* addr, unsigned int size);
                extern int SpuIsTransferCompleted(int flag);
                extern unsigned short g_SoundTransferQueueReadIndex;
                extern unsigned short g_SoundTransferQueueWriteIndex;
                extern void* SoundLoadWdsFile(void* pWdsFile, int mode);
                void* buf;
                unsigned char* entry;
                int size;
                ArchiveSetIndex(0x1C, 0x0);
                size = ArchiveDecodeAlignedSize(3);
                buf = HeapAlloc(size, 1);
                ArchiveReadFileToBuffer(3, buf, 0, 0x80);
                {
                    unsigned int dataOff = *(unsigned int*)((unsigned char*)buf + 0x18);
                    unsigned int dataSize = *(unsigned int*)((unsigned char*)buf + 0x14);
                    unsigned char first[16];
                    unsigned char rb[16];
                    unsigned int probeOff = 0;
                    int ok_entry, ok_spuaddr, ok_list, ok_queue, ok_bytes;
                    /* ADPCM banks open with silent blocks; verify at the first
                     * nonzero 16-byte window instead of offset 0. */
                    while (probeOff + 16 < dataSize &&
                           *(unsigned int*)((unsigned char*)buf + dataOff + probeOff) == 0) {
                        probeOff += 16;
                    }
                    memcpy(first, (unsigned char*)buf + dataOff + probeOff, 16);
                    entry = (unsigned char*)SoundLoadWdsFile(buf, 0);
                    ok_entry = (entry != NULL);
                    ok_spuaddr = ok_entry && (*(int*)(entry + 0x28) != 0);
                    {
                        extern void* g_SoundWdsLinkedList;
                        ok_list = (g_SoundWdsLinkedList == (void*)entry);
                    }
                    {
                        /* The transfer queue drains on the 240Hz tick thread;
                         * poll (TSan slows the pump ~15x -- a fixed-order
                         * readback races the drain and trips the backend's
                         * bounds assert on a not-yet-written address). */
                        int w = 0;
                        while (g_SoundTransferQueueReadIndex !=
                                   g_SoundTransferQueueWriteIndex && w < 500) {
                            PortSleepMs(10);
                            w++;
                        }
                    }
                    ok_queue = (g_SoundTransferQueueReadIndex == g_SoundTransferQueueWriteIndex);
                    memset(rb, 0, 16);
                    if (ok_spuaddr && ok_queue &&
                        (unsigned int)*(int*)(entry + 0x28) + probeOff + 16 < 0x80000) {
                        SpuSetTransferStartAddr(*(int*)(entry + 0x28) + probeOff);
                        SpuRead(rb, 16);
                    }
                    ok_bytes = (memcmp(rb, first, 16) == 0) &&
                               !(first[0]==0 && first[1]==0 && first[2]==0 && first[3]==0 &&
                                 first[4]==0 && first[5]==0 && first[6]==0 && first[7]==0);
                    printf("[wds-probe] file: size=%d dataOff=0x%x dataSize=0x%x\n",
                           size, dataOff, dataSize);
                    printf("[wds-probe] entry=%d spuAddr=0x%x list=%d queueDrained=%d\n",
                           ok_entry, ok_entry ? *(int*)(entry + 0x28) : 0, ok_list, ok_queue);
                    printf("[wds-probe] SPU-RAM readback vs source @+0x%x: match=%d (src %02x%02x%02x%02x rb %02x%02x%02x%02x)\n",
                           probeOff, ok_bytes, first[0], first[1], first[2], first[3],
                           rb[0], rb[1], rb[2], rb[3]);
                    printf("[wds-probe] RESULT: %s (samples LOADED to SPU-RAM; audibility = play probe)\n",
                           (ok_entry && ok_spuaddr && ok_list && ok_queue && ok_bytes) ? "PASS" : "FAIL");
                }
                HeapFree(buf);
                ArchiveSetIndex(4, 0);
            }

            /* B5.1 pass 2: first-audible probe (needs the WDS bank loaded --
             * run with XENO_SOUND_WDS_PROBE=1 too, and skip its HeapFree side
             * effects by design: the loader copied the header to the sound
             * heap and the samples to SPU-RAM; the file buffer is free). */
            if (getenv("XENO_SOUND_PLAY_PROBE")) {
                PortRunSoundPlayProbe();
            }

            /* Song-start M2 scan (env XENO_SOUND_SONG_SCAN=1): enumerate dir
             * 0x1C candidates -- header bytes decide which files are songs
             * (elementCount at +0x14, wdsId at +0x16) vs WDS banks -- and
             * print the loaded bank's id so the song/bank pairing is chosen
             * from measured data, not the scoping map alone. Read-only. */
            if (getenv("XENO_SOUND_SONG_SCAN")) {
                int fi;
                ArchiveSetIndex(0x1C, 0);
                for (fi = 0x0; fi <= 0x24; fi++) {
                    int sz = ArchiveDecodeAlignedSize(fi);
                    unsigned char hdr[0x40];
                    void* b;
                    if (sz <= 0 || sz > 0x300000) {
                        printf("[song-scan] file %02x: size=%d (skip)\n", fi, sz);
                        continue;
                    }
                    b = HeapAlloc(sz, 1);
                    if (b == NULL) { printf("[song-scan] file %02x: alloc fail (%d)\n", fi, sz); continue; }
                    ArchiveReadFileToBuffer(fi, b, 0, 0x80);
                    memcpy(hdr, b, 0x40);
                    printf("[song-scan] file %02x: size=%-7d magic=%02x%02x%02x%02x "
                           "id10=%04x elemCnt=%02x extra=%02x wdsId=%04x off18=%04x "
                           "tempo=%02x%02x%02x%02x\n",
                           fi, sz, hdr[0], hdr[1], hdr[2], hdr[3],
                           *(unsigned short*)(hdr + 0x10), hdr[0x14], hdr[0x15],
                           *(unsigned short*)(hdr + 0x16), *(unsigned short*)(hdr + 0x18),
                           hdr[0x1A], hdr[0x1B], hdr[0x1C], hdr[0x1D]);
                    HeapFree(b);
                }
                ArchiveSetIndex(4, 0);
            }

            /* Song-start M2: a real sequence through the real arming chain. */
            if (getenv("XENO_SOUND_SONG_PROBE")) {
                PortRunSoundSongProbe();
            }

            /* The KernelMenu "Field" option jumps straight into the field without
             * the new-game / worldmap setup that normally (a) fills
             * g_GameState.partyMembers and (b) selects the party-skin archive
             * directory (#4, as the skin loader func_8001ACA4 does) before field
             * entry. Without (b), GamePartyCharactersInitializeSkins resolves
             * ArchiveDecodeAlignedSize against the wrong directory -> bogus ~6MB ->
             * HeapAlloc fail. Setting the real directory here lets the field's
             * party-skin init proceed. This is a stand-in for the not-yet-ported
             * new-game init, not a permanent solution. The LoadGameStateOverlay
             * save/restore preserves this index through the field overlay load.
             *
             * These initializations are needed for BOTH the XENO_FIELD_TEST
             * harness AND the normal KernelMenu path, so they run unconditionally
             * once the archive is available. */
            PcPort_LoadSystemTextData();
            func_8001ACA4();
            {
                const char* fieldMap = getenv("XENO_FIELD_MAP");
                if (fieldMap != NULL && fieldMap[0] != '\0') {
                    D_8006F94E = (unsigned short)strtoul(fieldMap, NULL, 0);
                    printf("[xeno-port][field] XENO_FIELD_MAP=%u\n",
                           (unsigned int)D_8006F94E);
                }
                /* Field entrance/spawn index. D_8006F954 is the real field
                 * entrance/spawn transition input, the sister global of the map
                 * selector D_8006F94E (set just above the same way). The value
                 * propagates: FieldMain copies D_8006F954 -> g_GameState+0x1932
                 * (main.c:408); FieldLoad copies g_GameState+0x1930 ->
                 * g_FieldScriptMemory (misc3.c:390-396); the field-load script
                 * (func_800A08B8 -> func_8009FA54) reads field-script variable 2
                 * at g_FieldScriptMemory+2 to pick a spawn-table entry. Direct
                 * XENO_FIELD_MAP entry skips the transition, leaving var 2 = 0 ->
                 * entrance 0, which on some maps is an edge spawn outside the
                 * walkmesh (camera can't frame the player). Writing D_8006F954
                 * here -- the top of that copy chain, not an intermediate buffer
                 * that gets overwritten -- stands in for the missing transition.
                 * Coordinates still come from the game's own spawn table; only
                 * the index is selected. */
                const char* fieldEntrance = getenv("XENO_FIELD_ENTRANCE");
                if (fieldEntrance != NULL && fieldEntrance[0] != '\0') {
                    char* end = NULL;
                    long entrance = strtol(fieldEntrance, &end, 0);
                    if (end != fieldEntrance && entrance >= 0 &&
                        entrance <= 0xFFFF) {
                        D_8006F954 = (unsigned short)entrance;
                        printf("[xeno-port][field] XENO_FIELD_ENTRANCE=%ld "
                               "(D_8006F954 -> field-script var 2)\n", entrance);
                    } else {
                        printf("[xeno-port][field] ignoring invalid "
                               "XENO_FIELD_ENTRANCE=%s\n", fieldEntrance);
                    }
                    /* Camera approach direction. D_8006F950 is the retail
                     * transition's approach-angle input: FieldMain stores it
                     * as an octant (>>9) to g_GameState+0x1938 (main.c:409),
                     * which FieldLoad copies to field-script var 8; spawn
                     * entries whose camera rotByte is 0xFF (= "inherit from
                     * the transition") read var 8 in func_8009FA54. A cold
                     * harness boot leaves it 0, which degenerates to an
                     * axis-aligned camera yaw (0x800) that the d-pad->walk
                     * LUT is not authored for (retail field cameras are
                     * diagonal; Up walks away from the camera only there).
                     * Default octant 7 = scene yaw 0x600, empirically
                     * validated: Up walks exactly away from the camera, and
                     * the Map1 ent6 spawn is unoccluded from that side (the
                     * other consistent diagonal, octant 3, puts the camera
                     * behind the spawn-adjacent house). Override with
                     * XENO_FIELD_CAMDIR=0..7; 0 reproduces the legacy
                     * axis-aligned probes. */
                    {
                        const char* camDir = getenv("XENO_FIELD_CAMDIR");
                        long octant = 7;
                        if (camDir != NULL && camDir[0] != '\0') {
                            octant = strtol(camDir, NULL, 0) & 7;
                        }
                        D_8006F950 = (unsigned short)(octant << 9);
                        printf("[xeno-port][field] camera approach octant=%ld "
                               "(D_8006F950 -> field-script var 8)\n", octant);
                    }
                }
            }
            /* Normal-boot field target for the port-side title/new-game flow
             * (PcPort_BootMain in game_overrides.c). When this is NOT a field-test
             * run (XENO_FIELD_TEST != "1"), default to Lahan (map 1) entrance 6,
             * camera octant 7 -- the same spawn the harness validates -- so New
             * Game lands in the playable town instead of the cold-default map 0 /
             * axis-aligned camera. Any explicit XENO_FIELD_* override set above
             * still wins; a field-test run is left untouched so the smokes keep
             * their own map/entrance. */
            {
                const char* fieldTest = getenv("XENO_FIELD_TEST");
                if (!(fieldTest && fieldTest[0] == '1')) {
                    if (getenv("XENO_FIELD_MAP") == NULL)
                        D_8006F94E = 1;
                    if (getenv("XENO_FIELD_ENTRANCE") == NULL)
                        D_8006F954 = 6;
                    if (getenv("XENO_FIELD_CAMDIR") == NULL)
                        D_8006F950 = (unsigned short)(7 << 9);
                    printf("[xeno-port][field] normal boot -> Lahan default "
                           "(map=%u ent=%u camoct=7)\n",
                           (unsigned int)D_8006F94E, (unsigned int)D_8006F954);
                }
            }
            printf("[xeno-port][field] font + party-skin init done\n");
        } else {
            printf("[xeno-port] WARNING: no disc image found "
                   "(set XENO_DISC or place disc/disc1.bin); archive reads disabled.\n");
        }
    }

    /* 6. Boot splash: the asm boot shows the "Published by Square" logo before
     * handing off to the game. It is fully self-contained (LZSS-decompress the
     * migrated EXE blob, LoadImage CLUT+texture, DrawPrim a sprite with a
     * fade-in/hold/fade-out via Vsync), and bypasses the game ordering table. */
    GameShowSplashScreen();

    /* Oracle bootstrap: the real entry `start` (0x80019524) is still raw MIPS
     * asm, so we call the decompiled MainLoop() directly. It will run real game
     * code until it reaches the first not-yet-decompiled function on the live
     * path, which the stub layer logs as "[stub] <name>". That name is the next
     * thing to decompile. Expect crashes/loops until the boot chain is filled in. */
    printf("[xeno-port] entering decompiled MainLoop() (oracle)...\n");
    MainLoop(0);

    PsyX_Shutdown();
    printf("[xeno-port] Clean shutdown.\n");
    return 0;
}
