/* Differential prover for the adopted battle RESULTS module routines.
 *
 * For every routine in pc_port/src/battle_results_host_leaves.inc this loads
 * the retail results module (extracted from ./disc at test time; argv[1]) at
 * guest 0x801DE000 and runs the routine twice from identical state:
 *
 *   retail  the MIPS bytes through the interpreter, force_interpret set;
 *   native  the same guest call through the production bridge with
 *           force_interpret clear, i.e. results_try_host(): identity gate,
 *           allowlist, dlsym-bound battle_results_func_XXXXXXXX body and the
 *           PcPort_BattleMipsResolve accessor.
 *
 * Both runs enter through PcPortMipsRun at the routine's jal target, so the
 * native side is reached exactly the way the interpreted caller reaches it.
 * Compared after each pair: the full v0, every other general register (the
 * scratch ones are seeded per case, so a body must leave exactly the retail
 * exit set and preserve the rest), all of
 * guest RAM (minus the interpreter's own frame below the entry SP), the whole
 * native g_GameState object and the scratchpad.  Every store either side makes
 * is also recorded by host address and must land inside one of those compared
 * windows, so a write into some other native copy cannot pass unseen.  The
 * native run must execute zero guest instructions and must have gone through
 * the accessor.
 *
 * Then gate controls: corrupting one immutable module byte (unobserved, or
 * through a KSEG1 alias store), a different module resident, a pending
 * delay-slot load, force_interpret, a foreign runtime and a non-allowlisted
 * results PC must all refuse native dispatch; restoring the bytes, an
 * identical-value store and writes to the excluded mutable module data must
 * not.  The accessor's fail-closed paths are checked too.
 *
 * Not covered: HI/LO and timing.
 */

#include <setjmp.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "psyq/libgpu.h"
#include "psyq/libgte.h"

#ifndef BATTLE_RUNTIME_SOURCE
#define BATTLE_RUNTIME_SOURCE "../src/battle_mips_runtime.c"
#endif
#include BATTLE_RUNTIME_SOURCE

/* Size of the production g_GameState object (pc_port/src/data_game_state.c);
 * the runner reads it from the linked object with nm -S. */
#ifndef RESULTS_GS_BYTES
#error "RESULTS_GS_BYTES must be the size of the linked g_GameState"
#endif

/* Guest RAM plus a guard tail, as in the overlay prover. */
uint8_t g_PsxRam[PSX_RAM_SIZE + 0x1000];
uint8_t g_PsxScratchpad[4096];
unsigned int MFC2(int reg) { (void)reg; abort(); }
unsigned int CFC2(int reg) { (void)reg; abort(); }
void MTC2(unsigned int v, int reg) { (void)v; (void)reg; abort(); }
void CTC2(unsigned int v, int reg) { (void)v; (void)reg; abort(); }
int doCOP2(int op) { (void)op; abort(); }
int g_GPUDisabledState;
unsigned g_PcPortPresentedFrames;
void ControllerPushState(void) {}
void ControllerPoll(void) {}
void PcPort_ServiceVblank(void) {}
void PcPort_PadVblankPump(void) {}
void PsyX_UpdateInput(void) {}
void PcPort_GodModeBeforeGuest(PcPortMipsCpu *cpu, uint32_t target);
void PcPort_GodModeBeforeGuest(PcPortMipsCpu *cpu, uint32_t target)
{
    (void)cpu;
    (void)target;
}
int PsyX_Sys_GetVBlankCount(void) { return 0; }
char PsyX_BeginScene(void) { return 1; }
void ClearSplits(void) { abort(); }
void DrawAllSplits(void) { abort(); }
void ParsePrimitivesLinkedList(u_long *p, int s) { (void)p; (void)s; abort(); }

/* Bumped by the runner's generated placeholders for callees nothing linked
 * provides.  An adopted results routine must never reach one. */
unsigned g_prover_placeholder_hits;

/* The production accessor, observed through -Wl,--wrap: the host TU's
 * references bind here, the definition stays the runtime's. */
uint8_t *__real_PcPort_BattleMipsResolve(uint32_t address, unsigned width,
                                         int write);
uint8_t *__wrap_PcPort_BattleMipsResolve(uint32_t address, unsigned width,
                                         int write);

#define MODULE_ADDR   0x801DE000u
#define MODULE_BYTES  0x6500u
#define MODULE_OFFSET (MODULE_ADDR - 0x80000000u)
#define STEP_LIMIT    20000000u
/* The interpreter's frame for a callee entered at BATTLE_STACK_TOP lives
 * below it; a native body keeps its frame on the host stack.  Only that
 * window is excluded -- the caller's argument slots at/above SP are compared. */
#define FRAME_SKIP_HI (BATTLE_STACK_TOP - 0x80000000u)
#define FRAME_SKIP_LO (FRAME_SKIP_HI - 0x1000u)
#define RANDOM_CASES  96u

static BattleMipsRuntime rt;
static PcPortMipsCpu cpu;
static uint8_t module_image[MODULE_BYTES];
static uint8_t *native_gs;

static uint8_t before_ram[PSX_RAM_SIZE];
static uint8_t before_gs[RESULTS_GS_BYTES];
static uint8_t before_pad[4096];
static uint8_t retail_ram[PSX_RAM_SIZE];
static uint8_t retail_gs[RESULTS_GS_BYTES];
static uint8_t retail_pad[4096];

static unsigned failures;
static unsigned checks;
static unsigned controls;
static unsigned accessor_calls;
static unsigned stray_writes;
static uintptr_t stray_first;

static uint32_t rng_state;
static uint32_t rnd(void)
{
    rng_state = rng_state * 1664525u + 1013904223u;
    return rng_state >> 8;
}

static void fail(const char *name, const char *what, const char *detail)
{
    fprintf(stderr, "BATTLE RESULTS DIFFERENTIAL FAIL %s %s %s\n", name, what,
            detail);
    failures++;
}

/* Every store either side makes must land in a compared window. */
static void note_write(const uint8_t *p, unsigned width)
{
    uintptr_t a = (uintptr_t)p;
    if (a >= (uintptr_t)g_PsxRam && a + width <= (uintptr_t)g_PsxRam + PSX_RAM_SIZE)
        return;
    if (a >= (uintptr_t)native_gs && a + width <= (uintptr_t)native_gs + RESULTS_GS_BYTES)
        return;
    if (a >= (uintptr_t)g_PsxScratchpad &&
        a + width <= (uintptr_t)g_PsxScratchpad + sizeof(g_PsxScratchpad))
        return;
    if (stray_writes++ == 0)
        stray_first = a;
}

uint8_t *__wrap_PcPort_BattleMipsResolve(uint32_t address, unsigned width,
                                         int write)
{
    uint8_t *p = __real_PcPort_BattleMipsResolve(address, width, write);
    accessor_calls++;
    if (write)
        note_write(p, width);
    return p;
}

static int recording_write(void *opaque, uint32_t address, unsigned width,
                           uint32_t value)
{
    uint8_t *p = resolve_memory(opaque, address, width, 1);
    if (p != NULL)
        note_write(p, width);
    return runtime_write(opaque, address, width, value);
}

/* Shape test state through the production mapping, like the code would. */
static void poke8(uint32_t address, uint8_t value)
{
    uint8_t *p = resolve_memory(&rt, address, 1, 0);
    if (p == NULL) {
        fprintf(stderr, "BATTLE RESULTS DIFFERENTIAL FAIL harness poke 0x%08x\n",
                address);
        exit(2);
    }
    *p = value;
}

static void load_module_image(void)
{
    memcpy(g_PsxRam + MODULE_OFFSET, module_image, MODULE_BYTES);
}

/* Fresh random world, then the retail module on top. */
static void begin_case(uint32_t seed)
{
    size_t i;
    rng_state = seed;
    for (i = 0; i < PSX_RAM_SIZE; i++)
        g_PsxRam[i] = (uint8_t)rnd();
    for (i = 0; i < RESULTS_GS_BYTES; i++)
        native_gs[i] = (uint8_t)rnd();
    for (i = 0; i < sizeof(g_PsxScratchpad); i++)
        g_PsxScratchpad[i] = (uint8_t)rnd();
    load_module_image();
}

/* ---- per-routine case shapers ------------------------------------------ */

/* func_801E24B0: ids D_800D2FE4[48], values D_800D2CB0[48], table
 * g_GameState+0x2026 (0x8006F65A)[150], destination g_GameState+0x1F90
 * (0x8006F5C4)[150].  Edge cases first, then random alphabets small enough
 * to produce many matches and duplicate ids (a later id must win). */
static void shape_801E24B0(unsigned index, uint32_t args[4])
{
    unsigned j, i, alphabet;

    for (j = 0; j < 4; j++)
        args[j] = rnd();
    switch (index) {
    case 0: /* all ids zero: nothing may be written */
        for (j = 0; j < 48; j++) poke8(0x800D2FE4u + j, 0);
        return;
    case 1: /* every id matches every entry: all 150 get values[47] */
        for (j = 0; j < 48; j++) poke8(0x800D2FE4u + j, 1);
        for (i = 0; i < 150; i++) poke8(0x8006F65Au + i, 1);
        return;
    case 2: /* 0xFF ids against a 0xFF table */
        for (j = 0; j < 48; j++) poke8(0x800D2FE4u + j, 0xFF);
        for (i = 0; i < 150; i++) poke8(0x8006F65Au + i, 0xFF);
        return;
    case 3: /* only j=0/i=0 and j=47/i=149 match */
        for (j = 0; j < 48; j++) poke8(0x800D2FE4u + j, (uint8_t)(0x40 + j));
        for (i = 0; i < 150; i++) poke8(0x8006F65Au + i, 0x10);
        poke8(0x8006F65Au + 0, 0x40);
        poke8(0x8006F65Au + 149, 0x40 + 47);
        return;
    case 4: /* duplicate id at j=3, 20, 47: the last one wins */
        for (j = 0; j < 48; j++) poke8(0x800D2FE4u + j, 0);
        poke8(0x800D2FE4u + 3, 5);
        poke8(0x800D2FE4u + 20, 5);
        poke8(0x800D2FE4u + 47, 5);
        for (i = 0; i < 150; i += 7) poke8(0x8006F65Au + i, 5);
        return;
    case 5: /* zero table entries never match a zero id (skipped) */
        for (j = 0; j < 48; j++) poke8(0x800D2FE4u + j, (uint8_t)(j & 1 ? 0 : 9));
        for (i = 0; i < 150; i++) poke8(0x8006F65Au + i, 0);
        return;
    case 6: /* the guest bytes behind the native table hold the ids, the
             * native table does not: only a correct mapping stays silent */
        for (j = 0; j < 48; j++) poke8(0x800D2FE4u + j, (uint8_t)(j + 1));
        for (i = 0; i < 150; i++) {
            poke8(0x8006F65Au + i, 0xEE);
            g_PsxRam[0x6F65Au + i] = (uint8_t)(i % 48 + 1);
        }
        return;
    default:
        break;
    }
    alphabet = (unsigned[]){2, 4, 8, 16, 256}[index % 5u];
    for (j = 0; j < 48; j++)
        poke8(0x800D2FE4u + j, (uint8_t)(rnd() % alphabet));
    for (i = 0; i < 150; i++)
        poke8(0x8006F65Au + i, (uint8_t)(rnd() % alphabet));
}

typedef struct LeafSpec {
    uint32_t address;
    void (*shape)(unsigned index, uint32_t args[4]);
    unsigned edge_cases;
} LeafSpec;

static const LeafSpec kSpecs[] = {
    {0x801E24B0u, shape_801E24B0, 7},
};

/* ---- running one side --------------------------------------------------- */

typedef struct SideResult {
    int rc;
    uint32_t v0;
    uint32_t saved[11]; /* s0-s7, gp, sp, fp */
    uint32_t gpr[32];
    uint64_t steps;
    uint64_t native_calls;
    unsigned accessor_calls;
    unsigned placeholder_hits;
} SideResult;

static sigjmp_buf fault_jmp;
static void fault_handler(int sig)
{
    (void)sig;
    siglongjmp(fault_jmp, 1);
}

static void prepare_cpu(const uint32_t args[4], uint32_t saved_seed)
{
    unsigned i;
    memset(&cpu, 0, sizeof(cpu));
    initialize_cpu(&cpu, &rt);
    cpu.bus.write = recording_write;
    for (i = 0; i < 4; i++)
        cpu.gpr[4 + i] = args[i];
    for (i = 16; i <= 23; i++)
        cpu.gpr[i] = saved_seed * (i + 1u);
    /* Scratch registers too: an exit set that misses a register the retail
     * bytes change, or clobbers one they keep, shows up as a difference. */
    for (i = 1; i <= 25; i++) {
        if (i == 2 || (i >= 4 && i <= 7) || (i >= 16 && i <= 23))
            continue;
        cpu.gpr[i] = (saved_seed ^ 0xC3A5u) * (i + 7u);
    }
    cpu.gpr[28] = 0x800C0000u + (saved_seed & 0xFFF0u);
    cpu.gpr[30] = saved_seed ^ 0x5A5A5A5Au;
}

static int run_side(uint32_t target, const uint32_t args[4], int force,
                    uint32_t saved_seed, SideResult *out)
{
    unsigned i;
    uint64_t calls = rt.results.native_calls;
    unsigned accessed = accessor_calls;

    prepare_cpu(args, saved_seed);
    g_prover_placeholder_hits = 0;
    rt.force_interpret = force;
    if (sigsetjmp(fault_jmp, 1) != 0) {
        rt.force_interpret = 0;
        return -1;
    }
    out->rc = PcPortMipsRun(&cpu, target, BATTLE_HALT_PC, STEP_LIMIT);
    rt.force_interpret = 0;
    out->v0 = cpu.gpr[2];
    for (i = 0; i < 8; i++)
        out->saved[i] = cpu.gpr[16 + i];
    out->saved[8] = cpu.gpr[28];
    out->saved[9] = cpu.gpr[29];
    out->saved[10] = cpu.gpr[30];
    memcpy(out->gpr, cpu.gpr, sizeof(out->gpr));
    out->steps = cpu.steps;
    out->native_calls = rt.results.native_calls - calls;
    out->accessor_calls = accessor_calls - accessed;
    out->placeholder_hits = g_prover_placeholder_hits;
    return 0;
}

static void count_diffs(const uint8_t *a, const uint8_t *b, size_t n,
                        size_t skip_lo, size_t skip_hi, size_t *diffs,
                        size_t *first)
{
    size_t i;
    *diffs = 0;
    for (i = 0; i < n; i++) {
        if (i >= skip_lo && i < skip_hi)
            continue;
        if (a[i] != b[i]) {
            if (*diffs == 0)
                *first = i;
            (*diffs)++;
        }
    }
}

static unsigned compare_case(const char *name, uint32_t target,
                             const uint32_t args[4], uint32_t seed)
{
    SideResult retail, native;
    size_t diffs, first = 0;
    char detail[256];

    memcpy(before_ram, g_PsxRam, sizeof(before_ram));
    memcpy(before_gs, native_gs, RESULTS_GS_BYTES);
    memcpy(before_pad, g_PsxScratchpad, sizeof(before_pad));
    stray_writes = 0;

    if (run_side(target, args, 1, seed, &retail) != 0) {
        fail(name, "retail-fault", "the interpreted side faulted natively");
        return 0;
    }
    if (retail.rc != PC_PORT_MIPS_HALTED) {
        snprintf(detail, sizeof(detail), "rc=%d err=%s seed=%08x", retail.rc,
                 cpu.error, seed);
        fail(name, "retail-incomplete", detail);
        return 0;
    }
    if (retail.native_calls != 0 || retail.steps == 0) {
        fail(name, "retail-not-interpreted", "force_interpret did not interpret");
        return 0;
    }
    memcpy(retail_ram, g_PsxRam, sizeof(retail_ram));
    memcpy(retail_gs, native_gs, RESULTS_GS_BYTES);
    memcpy(retail_pad, g_PsxScratchpad, sizeof(retail_pad));

    memcpy(g_PsxRam, before_ram, sizeof(before_ram));
    memcpy(native_gs, before_gs, RESULTS_GS_BYTES);
    memcpy(g_PsxScratchpad, before_pad, sizeof(before_pad));

    if (run_side(target, args, 0, seed, &native) != 0) {
        fail(name, "native-fault", "native body faulted where retail completed");
        return 0;
    }
    if (native.rc != PC_PORT_MIPS_HALTED) {
        snprintf(detail, sizeof(detail), "rc=%d err=%s", native.rc, cpu.error);
        fail(name, "native-incomplete", detail);
        return 0;
    }
    if (native.native_calls != 1 || native.steps != 0) {
        snprintf(detail, sizeof(detail),
                 "native_calls=%llu guest_steps=%llu (identity/allowlist/bind refused?)",
                 (unsigned long long)native.native_calls,
                 (unsigned long long)native.steps);
        fail(name, "not-adopted", detail);
        return 0;
    }
    if (retail.placeholder_hits != 0 || native.placeholder_hits != 0) {
        fail(name, "placeholder", "a side reached a bridge placeholder callee");
        return 0;
    }
    if (retail.v0 != native.v0) {
        snprintf(detail, sizeof(detail), "v0 retail=%08x native=%08x seed=%08x",
                 retail.v0, native.v0, seed);
        fail(name, "return", detail);
        return 0;
    }
    if (memcmp(retail.saved, native.saved, sizeof(retail.saved)) != 0) {
        fail(name, "callee-saved", "s0-s7/gp/sp/fp differ");
        return 0;
    }
    {
        unsigned reg;
        for (reg = 1; reg < 32; reg++) {
            if (retail.gpr[reg] != native.gpr[reg]) {
                snprintf(detail, sizeof(detail),
                         "r%u retail=%08x native=%08x seed=%08x", reg,
                         retail.gpr[reg], native.gpr[reg], seed);
                fail(name, "exit-registers", detail);
                return 0;
            }
        }
    }
    count_diffs(retail_ram, g_PsxRam, PSX_RAM_SIZE, FRAME_SKIP_LO,
                FRAME_SKIP_HI, &diffs, &first);
    if (diffs != 0) {
        snprintf(detail, sizeof(detail),
                 "%zu bytes differ, first at guest %08x retail=%02x native=%02x seed=%08x",
                 diffs, (unsigned)(0x80000000u | first), retail_ram[first],
                 g_PsxRam[first], seed);
        fail(name, "ram", detail);
        return 0;
    }
    count_diffs(retail_gs, native_gs, RESULTS_GS_BYTES, 0, 0, &diffs, &first);
    if (diffs != 0) {
        snprintf(detail, sizeof(detail),
                 "%zu bytes differ, first at g_GameState+%04zx (guest %08x) "
                 "retail=%02x native=%02x seed=%08x",
                 diffs, first, (unsigned)(0x8006D634u + first), retail_gs[first],
                 native_gs[first], seed);
        fail(name, "native-copy", detail);
        return 0;
    }
    count_diffs(retail_pad, g_PsxScratchpad, sizeof(retail_pad), 0, 0, &diffs,
                &first);
    if (diffs != 0) {
        fail(name, "scratchpad", "scratchpad differs");
        return 0;
    }
    if (stray_writes != 0) {
        snprintf(detail, sizeof(detail),
                 "%u stores outside the compared windows, first host %p",
                 stray_writes, (void *)stray_first);
        fail(name, "uncompared-write", detail);
        return 0;
    }
    if (native.accessor_calls == 0) {
        fail(name, "accessor", "native body never used PcPort_BattleMipsResolve");
        return 0;
    }
    checks++;
    return 1;
}

/* ---- gate controls ------------------------------------------------------ */

/* 1 native, 0 interpret, as production's bridge decides for this PC. */
static int dispatch(uint32_t target)
{
    static const uint32_t args[4] = {0, 0, 0, 0};
    prepare_cpu(args, 1);
    rt.bridge_cpu = &cpu;
    return runtime_bridge(&rt, &cpu, target);
}

#define CONTROL(label, condition) do { \
    controls++; \
    if (!(condition)) { \
        fprintf(stderr, "BATTLE RESULTS DIFFERENTIAL FAIL control %s (line %d)\n", \
                label, __LINE__); \
        failures++; \
    } \
} while (0)

static void accessor_controls(void)
{
    ResultsCall call;
    volatile int stopped = 0;
    pid_t child;
    int status = 0;

    /* Inside an adopted call: unaligned and unmapped accesses unwind. */
    call.previous = NULL;
    results_call = &call;
    if (setjmp(call.failure) == 0)
        (void)PcPort_BattleMipsResolve(0x800D2FE5u, 2, 0);
    else
        stopped = 1;
    CONTROL("accessor unaligned stops the call", stopped);
    stopped = 0;
    if (setjmp(call.failure) == 0)
        (void)PcPort_BattleMipsResolve(0x00001000u, 4, 1);
    else
        stopped = 1;
    CONTROL("accessor unmapped stops the call", stopped);
    results_call = NULL;

    /* Outside one it aborts rather than hand back NULL. */
    child = fork();
    if (child == 0) {
        (void)PcPort_BattleMipsResolve(0x800D2FE6u, 4, 0);
        _exit(0);
    }
    CONTROL("accessor outside a call aborts",
            child > 0 && waitpid(child, &status, 0) == child &&
            WIFSIGNALED(status) && WTERMSIG(status) == SIGABRT);

    /* Exactly the interpreter's mapping, including the native g_GameState. */
    CONTROL("accessor maps g_GameState natively",
            PcPort_BattleMipsResolve(0x8006F65Au, 1, 0) == native_gs + 0x2026u &&
            PcPort_BattleMipsResolve(0x8006F5C4u, 1, 1) == native_gs + 0x1F90u);
    CONTROL("accessor maps battle BSS to guest RAM",
            PcPort_BattleMipsResolve(0x800D2FE4u, 1, 0) == g_PsxRam + 0xD2FE4u);
    CONTROL("accessor canonicalizes KSEG1",
            PcPort_BattleMipsResolve(0xA006F65Au, 1, 0) == native_gs + 0x2026u);
}

static void gate_controls(uint32_t target)
{
    static const uint32_t probes[] = {0x0000u, 0x44B0u, RESULTS_IMMUTABLE_BYTES - 1u};
    uint64_t generation;
    unsigned k;
    uint8_t saved;

    begin_case(0xC0FFEEu);
    CONTROL("retail module verifies", results_identity_current(&rt, target));
    CONTROL("baseline native", dispatch(target) == 1);

    for (k = 0; k < sizeof(probes) / sizeof(probes[0]); k++) {
        uint8_t *byte = g_PsxRam + MODULE_OFFSET + probes[k];
        int probed = probes[k] < RESULTS_REJECT_PROBE ||
                     (probes[k] & ~(RESULTS_REJECT_PROBE - 1u)) ==
                         ((target - MODULE_ADDR) & ~(RESULTS_REJECT_PROBE - 1u));
        *byte ^= 0x01u; /* unobserved: no hook sees this */
        CONTROL("corrupt immutable byte refuses native", dispatch(target) == 0);
        *byte ^= 0x01u;
        if (probed) {
            CONTROL("restored module adopts again", dispatch(target) == 1);
        } else {
            /* A rejection is re-examined only when its probe windows change
             * or a hook invalidates it (battle_results_gate.inc): an
             * unobserved repair elsewhere keeps interpreting -- fail closed. */
            CONTROL("unobserved far repair stays interpreted", dispatch(target) == 0);
            results_note_archive(&rt, 0x80029690u);
            CONTROL("invalidated repair adopts again", dispatch(target) == 1);
        }
    }

    generation = rt.results.generation;
    saved = g_PsxRam[MODULE_OFFSET + 0x44B4u];
    CONTROL("observed store", runtime_write(&rt, 0x801E24B4u, 1, saved) == 0);
    CONTROL("observed store invalidates",
            rt.results.state == RESULTS_UNKNOWN && rt.results.generation != generation);
    CONTROL("identical bytes re-verify", dispatch(target) == 1);

    CONTROL("KSEG1 alias store", runtime_write(&rt, 0xA01E24B4u, 1, saved ^ 0x80u) == 0);
    CONTROL("KSEG1 alias store invalidates", rt.results.state == RESULTS_UNKNOWN);
    CONTROL("KSEG1-corrupted module refuses native", dispatch(target) == 0);
    CONTROL("KSEG1 restore", runtime_write(&rt, 0xA01E24B4u, 1, saved) == 0);
    CONTROL("KSEG1-restored module adopts", dispatch(target) == 1);

    generation = rt.results.generation;
    CONTROL("mutable flag store", runtime_write(&rt, 0x801E44C0u, 1, 1) == 0);
    CONTROL("mutable word store", runtime_write(&rt, 0x801E44FCu, 4, 0x12345678u) == 0);
    CONTROL("mutable data keeps the verdict",
            rt.results.state == RESULTS_VERIFIED && rt.results.generation == generation);
    CONTROL("mutable data still adopts", dispatch(target) == 1);

    results_note_archive(&rt, 0x800295d8u);
    CONTROL("archive read invalidates", rt.results.state == RESULTS_UNKNOWN);
    CONTROL("after archive read re-verifies", dispatch(target) == 1);

    memset(g_PsxRam + MODULE_OFFSET, 0, MODULE_BYTES);
    CONTROL("empty range refuses native", dispatch(target) == 0);
    memcpy(g_PsxRam + MODULE_OFFSET, module_image + 0x2000u, MODULE_BYTES - 0x2000u);
    CONTROL("other code resident refuses native", dispatch(target) == 0);
    load_module_image();
    CONTROL("reloaded module adopts", dispatch(target) == 1);

    prepare_cpu((const uint32_t[4]){0, 0, 0, 0}, 1);
    cpu.load_valid = 1;
    cpu.load_reg = 4;
    rt.bridge_cpu = &cpu;
    CONTROL("pending delay-slot load interprets", runtime_bridge(&rt, &cpu, target) == 0);
    prepare_cpu((const uint32_t[4]){0, 0, 0, 0}, 1);
    cpu.load_valid = 1;
    cpu.load_reg = 0;
    CONTROL("load into $zero does not block", runtime_bridge(&rt, &cpu, target) == 1);

    rt.force_interpret = 1;
    CONTROL("force_interpret interprets", dispatch(target) == 0);
    rt.force_interpret = 0;

    g_ActiveBattleRuntime = NULL;
    CONTROL("no active runtime interprets", dispatch(target) == 0);
    g_ActiveBattleRuntime = &rt;

    CONTROL("non-allowlisted results entry interprets",
            results_try_host(&rt, &cpu, 0x801E252Cu) == 0);
    CONTROL("still native afterwards", dispatch(target) == 1);
}

/* An adopted address is dispatched whenever the guest PC equals it, so it
 * must be reachable only as a call: no branch, J, or data word (jump table)
 * in the module may target it. */
static void entry_reachability(uint32_t entry, const char *name)
{
    uint32_t offset;
    for (offset = 0; offset < RESULTS_IMMUTABLE_BYTES; offset += 4) {
        uint32_t word = load_le(module_image + offset, 4);
        uint32_t pc = MODULE_ADDR + offset;
        uint32_t op = word >> 26;
        char detail[96];
        if (word == entry) {
            snprintf(detail, sizeof(detail), "data word at %08x", pc);
            fail(name, "entry-reached-not-as-call", detail);
        }
        if (op == 2u && (((pc + 4u) & 0xF0000000u) | ((word & 0x03FFFFFFu) << 2)) == entry) {
            snprintf(detail, sizeof(detail), "J at %08x", pc);
            fail(name, "entry-reached-not-as-call", detail);
        }
        if (op == 1u || (op >= 4u && op <= 7u) || (op >= 0x14u && op <= 0x17u)) {
            uint32_t t = pc + 4u + (uint32_t)((int32_t)(int16_t)(word & 0xFFFFu) * 4);
            if (t == entry) {
                snprintf(detail, sizeof(detail), "branch at %08x", pc);
                fail(name, "entry-reached-not-as-call", detail);
            }
        }
    }
    controls++;
}

int main(int argc, char **argv)
{
    FILE *f;
    size_t i;
    unsigned spec_count = (unsigned)(sizeof(kSpecs) / sizeof(kSpecs[0]));

    if (argc != 2) {
        fprintf(stderr, "usage: %s results_module.bin\n", argv[0]);
        return 2;
    }
    f = fopen(argv[1], "rb");
    if (f == NULL || fread(module_image, 1, MODULE_BYTES, f) != MODULE_BYTES) {
        fprintf(stderr, "BATTLE RESULTS DIFFERENTIAL FAIL cannot read %s\n", argv[1]);
        return 2;
    }
    fclose(f);

    memset(&rt, 0, sizeof(rt));
    g_ActiveBattleRuntime = &rt;
    initialize_runtime(&rt);
    for (i = 0; i < rt.data_count; i++) {
        if (strcmp(rt.data[i].name, "g_GameState") == 0)
            native_gs = rt.data[i].host;
    }
    if (native_gs == NULL || native_gs != dlsym(RTLD_DEFAULT, "g_GameState")) {
        fprintf(stderr, "BATTLE RESULTS DIFFERENTIAL FAIL g_GameState is not "
                "bound to the linked production object\n");
        return 1;
    }
    signal(SIGSEGV, fault_handler);
    signal(SIGBUS, fault_handler);

    begin_case(1);
    if (!results_identity_current(&rt, MODULE_ADDR)) {
        fprintf(stderr, "BATTLE RESULTS DIFFERENTIAL FAIL loaded module does not "
                "match the pinned identity\n");
        return 1;
    }

    accessor_controls();
    for (i = 0; kResultsLeaves[i].name != NULL; i++) {
        const BattleResultsLeaf *leaf = &kResultsLeaves[i];
        const LeafSpec *spec = NULL;
        unsigned s, c, proven = 0;

        for (s = 0; s < spec_count; s++) {
            if (kSpecs[s].address == leaf->address)
                spec = &kSpecs[s];
        }
        if (spec == NULL) {
            fprintf(stderr, "BATTLE RESULTS DIFFERENTIAL UNPROVEN %s (no case "
                    "shaper in this test)\n", leaf->name);
            failures++;
            continue;
        }
        entry_reachability(leaf->address, leaf->name);
        for (c = 0; c < spec->edge_cases + RANDOM_CASES; c++) {
            uint32_t seed = leaf->address ^ (0x9E3779B9u * (c + 1u));
            uint32_t args[4];
            begin_case(seed);
            spec->shape(c, args);
            proven += compare_case(leaf->name, leaf->address, args, seed);
        }
        if (proven != spec->edge_cases + RANDOM_CASES) {
            fprintf(stderr, "BATTLE RESULTS DIFFERENTIAL FAIL %s proven=%u of %u\n",
                    leaf->name, proven, spec->edge_cases + RANDOM_CASES);
            failures++;
        } else {
            printf("BATTLE RESULTS DIFFERENTIAL leaf %s cases=%u\n", leaf->name,
                   proven);
        }
        gate_controls(leaf->address);
    }
    if (kResultsLeaves[0].name == NULL)
        fprintf(stderr, "BATTLE RESULTS DIFFERENTIAL note: the allowlist is empty\n");

    if (failures != 0) {
        fprintf(stderr, "BATTLE RESULTS DIFFERENTIAL FAIL total=%u\n", failures);
        return 1;
    }
    printf("BATTLE RESULTS DIFFERENTIAL PASS cases=%u controls=%u leaves=%u\n",
           checks, controls, (unsigned)(RESULTS_LEAF_SLOTS - 1u));
    return 0;
}
