/*
 * Vsync present regression test (behavioral, production-linked).
 *
 * Compiles the real pc_port/src/psyq_compat.c and drives its exported Vsync
 * wrapper through instrumented fakes for the PsyCross/game externs in the
 * wrapper's call closure.  The double-present flicker path is:
 *
 *   field frame N ends with DrawOTag -> scene left open
 *   frame N+1 calls Vsync(1) (query) -> old wrapper ends/presents the scene
 *   same frame calls Vsync(0) -> scene already closed -> VRAM fallback,
 *     which swaps a second time
 *
 * The assertions below pin the intended behavior:
 *   - Vsync(1) / Vsync(<0) are pure timing queries: no flush, present, VRAM
 *     fallback, input poll, or controller push.
 *   - A blocking Vsync(0) with an open rendered scene ends/presents exactly
 *     once and never takes the VRAM fallback.
 *   - A blocking Vsync(0) with no scene may take the VRAM fallback exactly
 *     once (the movie/LoadImage display path).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int Vsync(int mode);

/* ---- instrumented fakes for the Vsync closure ---- */

int g_fakeSceneOpen;

static int s_endSceneCalls;
static int s_sceneEnds;          /* PsyX_EndScene actually closed an open scene */
static int s_presentVRAMCalls;
static int s_swapCalls;
static int s_pollEventCalls;     /* modeled SDL event drains */
static int s_drawAllSplitsCalls;
static int s_updateInputCalls;
static int s_controllerPollCalls;
static int s_controllerPushCalls;
static int s_vsyncCalls;
static const int s_vsyncBase = 0x12340000;

int PsyX_IsSceneOpen(void)
{
    return g_fakeSceneOpen;
}

void GR_SwapWindow(void)
{
    s_swapCalls++;
}

void PsyX_EndScene(void)
{
    s_endSceneCalls++;
    if (g_fakeSceneOpen) {
        g_fakeSceneOpen = 0;
        s_sceneEnds++;
        GR_SwapWindow();
    }
}

int PsyX_PresentDisplayFromVRAM(void)
{
    s_presentVRAMCalls++;
    s_pollEventCalls++;   /* real presenter drains SDL events before the swap */
    GR_SwapWindow();
    return 1;
}

void DrawAllSplits(void)
{
    s_drawAllSplitsCalls++;
}

void PsyX_UpdateInput(void)
{
    s_updateInputCalls++;
    s_pollEventCalls++;
}

void ControllerPoll(void)
{
    s_controllerPollCalls++;
}

void ControllerPushState(void)
{
    s_controllerPushCalls++;
}

/* Env-gated test-tooling paths in psyq_compat.c (ForcedFieldMenu equip seed,
 * PadOnControl tracing); never armed in this test. */
unsigned char D_800594CC;
unsigned char D_800B21D0;
int g_GameSceneMapNum;
void func_8001B970(void) {}

void PcPort_FieldCaptureOnVsync(void)
{
}

void PcPort_CullCamLogOnVsync(void)
{
}

void PcPort_FieldPosDiag(void)
{
}

static int s_vblankCount;
int PsyX_Sys_GetVBlankCount(void)
{
    return ++s_vblankCount;
}

int VSync(int mode)
{
    s_vsyncCalls++;
    return s_vsyncBase + mode;
}

/* Game globals touched by the headless menu hooks in psyq_compat.c.  The
 * hooks are env-disabled in this test; the definitions exist only to link. */
int g_KernelMenuCurChoice;
int g_KernelMenuIsRunning;
unsigned short g_C1ButtonStateReleased;
int D_800ADB64;
int D_800ADB68;
unsigned char g_GameState[0x8000];
unsigned char g_C1Buffer[4];
int g_XenoMenuNavReaderTicks;
int g_XenoMenuN2Phase;
int g_XenoMenuN2c2Phase;
int g_XenoMenuN3aPhase;
unsigned char g_XenoMenuN3aRowFlags[32];
int g_XenoMenuN2c3SpecialHits;

unsigned int PcPort_N2c1ReadItemPair(void)
{
    return 0;
}

/* ---- harness ---- */

static int s_failures;

static void reset(void)
{
    g_fakeSceneOpen = 0;
    s_endSceneCalls = 0;
    s_sceneEnds = 0;
    s_presentVRAMCalls = 0;
    s_swapCalls = 0;
    s_pollEventCalls = 0;
    s_drawAllSplitsCalls = 0;
    s_updateInputCalls = 0;
    s_controllerPollCalls = 0;
    s_controllerPushCalls = 0;
    s_vsyncCalls = 0;
}

static void check(const char* tag, int cond)
{
    if (!cond) {
        fprintf(stderr, "FAIL: %s\n", tag);
        s_failures++;
    }
}

static int single_frame_present_ok(void)
{
    return s_sceneEnds == 1 && s_swapCalls == 1 && s_presentVRAMCalls == 0;
}

static void run_query_case(int mode)
{
    char tag[64];
    int before;
    int ret;

    reset();
    g_fakeSceneOpen = 1;   /* a rendered scene is open, as after DrawOTag */
    before = s_failures;
    ret = Vsync(mode);

    snprintf(tag, sizeof(tag), "Vsync(%d) returns the timing sample", mode);
    check(tag, ret == s_vsyncBase + mode);
    check("query does not end/present a scene", s_endSceneCalls == 0 && s_sceneEnds == 0);
    check("query performs no swap", s_swapCalls == 0);
    check("query does not flush splits", s_drawAllSplitsCalls == 0);
    check("query does not invoke VRAM fallback", s_presentVRAMCalls == 0);
    check("query does not poll input", s_updateInputCalls == 0 && s_pollEventCalls == 0);
    check("query does not poll/push controllers", s_controllerPollCalls == 0 && s_controllerPushCalls == 0);
    check("query still reads the vblank counter", s_vsyncCalls == 1);

    printf("Vsync(%d) query side-effect free: %s\n", mode,
           s_failures == before ? "PASS" : "FAIL");
}

static void run_field_sequence(void)
{
    int before = s_failures;

    /* DrawOTag just finished frame N and left the PsyCross scene open.
     * Frame N+1 starts with Vsync(1) (query) and hits its blocking Vsync(0)
     * at the vblank boundary. */
    reset();
    g_fakeSceneOpen = 1;
    Vsync(1);
    Vsync(0);

    check("field frame ends/presents the rendered scene exactly once",
          s_sceneEnds == 1 && s_endSceneCalls == 1);
    check("field frame performs exactly one swap", s_swapCalls == 1);
    check("field frame takes no VRAM fallback", s_presentVRAMCalls == 0);
    check("field frame flushes splits once", s_drawAllSplitsCalls == 1);
    check("field frame has a single input drain",
          s_updateInputCalls == 1 && s_controllerPollCalls == 1 &&
          s_controllerPushCalls == 1 && s_pollEventCalls == 1);
    check("field frame keeps both Vsync reads", s_vsyncCalls == 2);
    check("single-present predicate holds after fix", single_frame_present_ok());

    printf("Field rendered-frame sequence: %s\n",
           s_failures == before ? "PASS" : "FAIL");
}

static void run_blocking_open_scene(void)
{
    int before = s_failures;

    reset();
    g_fakeSceneOpen = 1;
    Vsync(0);

    check("blocking Vsync(0) with open scene ends it once",
          s_sceneEnds == 1 && s_endSceneCalls == 1);
    check("blocking Vsync(0) with open scene presents once", s_swapCalls == 1);
    check("blocking Vsync(0) with open scene takes no VRAM fallback",
          s_presentVRAMCalls == 0);
    check("blocking Vsync(0) drains input once",
          s_updateInputCalls == 1 && s_controllerPollCalls == 1 &&
          s_controllerPushCalls == 1);

    printf("Blocking Vsync(0), open rendered scene: %s\n",
           s_failures == before ? "PASS" : "FAIL");
}

static void run_blocking_no_scene(void)
{
    int before = s_failures;

    reset();
    g_fakeSceneOpen = 0;
    Vsync(0);

    check("no-scene blocking Vsync(0) may use VRAM fallback once",
          s_presentVRAMCalls == 1 && s_swapCalls == 1);
    check("no-scene blocking Vsync(0) ends no scene", s_sceneEnds == 0);

    printf("Blocking Vsync(0), no scene (movie/LoadImage): %s\n",
           s_failures == before ? "PASS" : "FAIL");
}

static void run_negative_control(void)
{
    int before = s_failures;

    /* Replay the old defect by hand: rendered EndScene plus the VRAM fallback
     * within one frame.  The single-present predicate must reject it. */
    reset();
    g_fakeSceneOpen = 1;
    PsyX_EndScene();
    PsyX_PresentDisplayFromVRAM();
    check("negative control reproduces double present",
          s_sceneEnds == 1 && s_swapCalls == 2 && s_presentVRAMCalls == 1);
    check("single-present predicate kills old double-present mutant",
          !single_frame_present_ok());

    printf("Negative control (old double present detected): %s\n",
           s_failures == before ? "PASS" : "FAIL");
}

int main(void)
{
    static const char* envs[] = {
        "XENO_MENU_FORCE", "XENO_MENU_NAV_TEST", "XENO_KERNEL_SEL",
        "XENO_FIELD_TEST", "XENO_MENU_FORCE_DELAY", "XENO_KERNEL_DELAY",
        "XENO_FIELD_CAPTURE_DIR", "XENO_CULL_CAM_LOG"
    };
    size_t i;

    for (i = 0; i < sizeof(envs) / sizeof(envs[0]); i++)
        unsetenv(envs[i]);

    run_query_case(1);
    run_query_case(-1);
    run_query_case(-2);  /* negative-mode class, not just the observed caller */
    run_blocking_open_scene();
    run_field_sequence();
    run_blocking_no_scene();
    run_negative_control();

    if (s_failures != 0) {
        fprintf(stderr, "Vsync present regression: FAIL (%d check(s))\n",
                s_failures);
        return 1;
    }
    puts("Vsync present regression: PASS");
    return 0;
}
