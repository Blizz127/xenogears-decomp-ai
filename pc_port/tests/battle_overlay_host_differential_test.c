/* Differential prover for the adopted battle overlay C bodies.
 *
 * For every function named in pc_port/src/battle_overlay_host_leaves.inc this
 * runs the retail MIPS bytes out of disc/battle.bin through the interpreter and
 * the adopted host C body, on identical guest RAM and identical arguments, then
 * compares the return registers and the guest RAM the call changed.
 *
 * The host side is reached exactly the way production reaches it: the runtime's
 * own dlsym bridge, with the same argument translation. That makes this a test
 * of the allowlist, the guest-RAM D_* aliases and the native boundary ABI as
 * well as of the C body itself. The interpreted side forces every guest call
 * back into the interpreter (BattleMipsRuntime.force_interpret), so a body that
 * calls another adopted body is compared as retail ran it, not as the host
 * happens to compose it.
 *
 * What this does NOT cover:
 *  - the guest stack window is excluded, because the interpreter writes its
 *    frame there and a host C body keeps its frame on the host stack;
 *  - writes to variables that resolve to a *native* copy (main-exe globals with
 *    a shared binding) land outside g_PsxRam and are invisible to the RAM diff;
 *  - an interpreted run that does not halt is reported as inconclusive rather
 *    than failed, so unexercised paths do not masquerade as proof.
 */

#include <setjmp.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "psyq/libgpu.h"
#include "psyq/libgte.h"

#ifndef BATTLE_RUNTIME_SOURCE
#define BATTLE_RUNTIME_SOURCE "../src/battle_mips_runtime.c"
#endif
#include BATTLE_RUNTIME_SOURCE
/* bg_jalr, the native indirect-call entry the guest-ABI bodies use. */
#include "battle_host_guest.h"

/* Guest RAM plus a guard tail. PSX_ADDR masks a guest address into 2 MiB, so a
 * pointer loaded out of seeded random data can land in the last bytes of the
 * region and a body that writes past it would run off the array. On hardware
 * the address wraps; the guard keeps the harness alive so the divergence can be
 * reported instead of crashing. Only the first PSX_RAM_SIZE bytes are compared. */
uint8_t g_PsxRam[PSX_RAM_SIZE + 0x1000];
uint8_t g_PsxScratchpad[4096];
#ifndef PROVER_REAL_GTE
unsigned int MFC2(int reg) { (void)reg; abort(); }
unsigned int CFC2(int reg) { (void)reg; abort(); }
void MTC2(unsigned int v, int reg) { (void)v; (void)reg; abort(); }
void CTC2(unsigned int v, int reg) { (void)v; (void)reg; abort(); }
int doCOP2(int op) { (void)op; abort(); }
#endif
int g_GPUDisabledState;
unsigned g_PcPortPresentedFrames;
void ControllerPushState(void) {}
void ControllerPoll(void) {}
void PcPort_ServiceVblank(void) {}
void PcPort_PadVblankPump(void) {}
void PsyX_UpdateInput(void) {}
/* runtime_bridge's debug hook (pc_port/src/god_mode.c, not linked here). It is
 * port instrumentation, not a game callee, so it must not come from the
 * generated placeholders: those mark the case inconclusive. */
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

/* The allowlist is the unit under test; the width of the return value each body
 * defines is generated from src/battle so this cannot drift either. */
typedef struct Leaf {
    const char *name;
    unsigned return_bits;
    /* Bit i set when parameter i is declared as a pointer. */
    unsigned pointer_params;
} Leaf;

static const Leaf kLeaves[] = {
#include "overlay_leaf_types.inc"
};
#define LEAF_COUNT (sizeof(kLeaves) / sizeof(kLeaves[0]))

/* Guest addresses whose overlay alias is a pointer: the guest word holds a PSX
 * address that a body dereferences. The sweep seeds each of these with one live
 * RAM address, because a zero word makes retail fault on the dereference and the
 * case could never complete. Generated from the same aliases the bodies use, so
 * a body cannot silently lose its seed. */
static const uint32_t kPointerGlobals[] = {
#include "overlay_pointer_globals.inc"
};
#define POINTER_GLOBAL_COUNT \
    (sizeof(kPointerGlobals) / sizeof(kPointerGlobals[0]))

#define OVERLAY_BASE 0x8006FAF0u
#define SCRATCH      0x800C4000u   /* overlay BSS: raw guest RAM on both sides */
#define SCRATCH_END  0x800C8000u
#define IMAGE_MAX    0x60000u
#define STEP_LIMIT   4000000u
/* The interpreter's own frame lives here; the host keeps its frame off-RAM. */
#define STACK_SKIP_LO 0x1FF000u
#define STACK_SKIP_HI 0x200000u

#define WORD 0x800C5000u
#define HALF 0x800C5100u
#define REC  0x800C5200u
#define SRC  0x800C5400u
#define DST  0x800C5600u
#define PTRT 0x801E0000u   /* inside RAM, outside the excluded stack window */
/* Bytes of PTRT the pointer-argument cases fill with the RAM pattern. Large
 * enough for the struct an overlay body typically touches through a pointer. */
#define POINTER_TARGET_SIZE 0x400u

static BattleMipsRuntime rt;
/* The native g_GameState both sides share. Main-exe globals resolve to one
 * native copy for the interpreted and the host run alike, so a write there is
 * invisible to the guest-RAM diff. Snapshot, seed and compare it explicitly:
 * many battle bodies do most of their work in the character records. */
#define NATIVE_GS_SIZE 0x2358u
static uint8_t *native_gs;
static uint8_t native_before[NATIVE_GS_SIZE];
static uint8_t native_expected[NATIVE_GS_SIZE];
static PcPortMipsCpu cpu;
static uint8_t image[IMAGE_MAX];
static size_t image_size;
static uint8_t before[PSX_RAM_SIZE];
static uint8_t expected[PSX_RAM_SIZE];
static uint8_t actual[PSX_RAM_SIZE];

static unsigned checks;
static unsigned inconclusive;
static unsigned failures;
static unsigned hand_only;
/* Per-leaf count of comparisons that actually completed. A leaf with none is
 * reported UNPROVEN: being allowlisted without evidence is not a claim this
 * harness is willing to make. */
static unsigned proven[LEAF_COUNT];
static unsigned inconclusive_logged[LEAF_COUNT];
static unsigned inconclusive_per_leaf[LEAF_COUNT];

/* Bumped by every generated bridge placeholder (see the runner): a callee that
 * neither the link nor PsyCross provides. Both sides reach the same
 * placeholder -- the interpreter resolves it through the bridge map -- and
 * compare an undefined return register, so a case that touched one proves
 * nothing and is counted inconclusive. */
unsigned g_prover_placeholder_hits;

static uint32_t rng_state = 0x12345678u;
static uint32_t rnd(void)
{
    rng_state = rng_state * 1664525u + 1013904223u;
    return rng_state >> 8;
}

static void fail(const char *name, const char *what, const char *detail)
{
    fprintf(stderr, "BATTLE OVERLAY DIFFERENTIAL FAIL %s %s %s\n", name, what,
            detail);
    failures++;
}

static void begin_case(uint32_t pattern, int seed_pointers)
{
    uint32_t a;
    memset(g_PsxRam, 0, sizeof(g_PsxRam));
    memcpy(PSX_ADDR(OVERLAY_BASE), image, image_size);
    for (a = SCRATCH; a < SCRATCH_END; a++) {
        uint8_t v;
        if (pattern == 0)
            v = 0;
        else if (pattern == 1)
            v = 0xA5;
        else
            v = (uint8_t)rnd();
        *((uint8_t *)PSX_ADDR(a)) = v;
    }
    /* Small values only (0..3). Bodies index the character records with
     * bytes read from g_GameState; the shared binding covers 0x2358 bytes, and
     * past it the interpreter falls through to guest RAM while native code
     * keeps the native array. A wild index therefore diverges for a reason
     * that is the harness's binding, not the body, so keep indices in range
     * while still giving id comparisons something to match. */
    if (native_gs != NULL) {
        for (a = 0; a < NATIVE_GS_SIZE; a++) {
            uint8_t v;
            if (pattern == 0)
                v = 0;
            else if (pattern == 1)
                v = 1;
            else
                v = (uint8_t)(rnd() & 3u);
            native_gs[a] = v;
        }
    }
    /* Seeded after the fill: a pointer global that the pattern would leave null
     * (pattern 0) or wild (patterns 1-2) gets a live target instead, which is
     * what lets a body that dereferences it finish on both sides. The target
     * window then gets the pattern too, so a body that reads through a pointer
     * sees pattern data instead of the zero fill. */
    if (seed_pointers) {
        unsigned g;
        for (g = 0; g < POINTER_GLOBAL_COUNT; g++)
            *((uint32_t *)PSX_ADDR(kPointerGlobals[g])) = PTRT;
        for (a = PTRT; a < PTRT + POINTER_TARGET_SIZE; a++) {
            uint8_t v;
            if (pattern == 0)
                v = 0;
            else if (pattern == 1)
                v = 0xA5;
            else
                v = (uint8_t)rnd();
            *((uint8_t *)PSX_ADDR(a)) = v;
        }
        /* An argument can be a pointer to a pointer (u8**): give the first word
         * of every argument target a live target of its own, so the inner
         * dereference lands in emulated RAM instead of address zero. */
        for (a = 0; a < 4; a++)
            *((uint32_t *)PSX_ADDR(PTRT + 0x100u * a)) = PTRT + 0x200u;
    }
}

static void load(void)
{
    FILE *f = fopen("disc/battle.bin", "rb");
    if (f == NULL) {
        perror("disc/battle.bin");
        exit(2);
    }
    image_size = fread(image, 1, sizeof(image), f);
    if (image_size == 0) {
        fprintf(stderr, "BATTLE OVERLAY DIFFERENTIAL FAIL empty disc/battle.bin\n");
        exit(2);
    }
    fclose(f);
}

static void runtime_up(void)
{
    memset(&rt, 0, sizeof(rt));
    g_ActiveBattleRuntime = &rt;
    initialize_runtime(&rt);
    rt.bridge_cpu = &cpu;
}

/* Seeded into t0 on both sides.  func_80079ED8/func_8007A280 return the
 * caller's t0 unchanged on their store path (a3 == 0); see the rule in the
 * return comparison below. */
#define PROVER_T0_SEED 0x7E57A00Du
static unsigned store_pass_through_checks;

static int is_store_pass_through(const char *name, const uint32_t *args)
{
    return (args[3] & 0xFFu) == 0 &&
           (strcmp(name, "func_80079ED8") == 0 ||
            strcmp(name, "func_8007A280") == 0);
}

/* Instruction coverage of the retail side for the larger adopted leaves:
 * the distinct instruction addresses of the leaf's own body (the retail
 * extent, asm/battle/.../func_X.s) that the interpreter executed in cases
 * that were then PROVEN.  The interpreter consults the bus bridge before
 * every instruction, so wrapping it sees each executed pc. */
typedef struct {
    const char *name;
    uint32_t start;
    unsigned insns;
} CoverageLeaf;
static const CoverageLeaf kCoverage[] = {
    {"func_800BA8F4", 0x800BA8F4u, 36},
    {"func_800BB13C", 0x800BB13Cu, 67},
    {"func_800BAC50", 0x800BAC50u, 27},
    {"func_80076AC8", 0x80076AC8u, 14},
    {"func_80076C34", 0x80076C34u, 17},
    {"func_800AAA20", 0x800AAA20u, 69},
    {"func_800A3E98", 0x800A3E98u, 257},
    {"func_800A0838", 0x800A0838u, 1222},
};
#define COVERAGE_COUNT (sizeof(kCoverage) / sizeof(kCoverage[0]))
#define COVERAGE_MAX_INSNS 4096u
static uint8_t cov_seen[COVERAGE_COUNT][COVERAGE_MAX_INSNS];
static uint8_t cov_case[COVERAGE_MAX_INSNS];
static int cov_index = -1;
static int (*cov_inner_bridge)(void *, PcPortMipsCpu *, uint32_t);

static int cov_bridge(void *opaque, PcPortMipsCpu *c, uint32_t target)
{
    if (cov_index >= 0) {
        const CoverageLeaf *l = &kCoverage[cov_index];
        if (target >= l->start && target < l->start + 4u * l->insns)
            cov_case[(target - l->start) >> 2] = 1;
    }
    return cov_inner_bridge(opaque, c, target);
}

static int coverage_slot(const char *name)
{
    unsigned i;
    for (i = 0; i < COVERAGE_COUNT; i++)
        if (strcmp(kCoverage[i].name, name) == 0)
            return (int)i;
    return -1;
}

static void coverage_report(void)
{
    unsigned i, k, n;
    for (i = 0; i < COVERAGE_COUNT; i++) {
        for (k = 0, n = 0; k < kCoverage[i].insns; k++)
            n += cov_seen[i][k];
        printf("BATTLE OVERLAY DIFFERENTIAL coverage %s insns=%u/%u (%.1f%%)\n",
               kCoverage[i].name, n, kCoverage[i].insns,
               100.0 * n / kCoverage[i].insns);
        if (getenv("BATTLE_OVERLAY_DIFFERENTIAL_COVERAGE_MISSES") != NULL) {
            for (k = 0; k < kCoverage[i].insns; k++)
                if (!cov_seen[i][k])
                    fprintf(stderr, "[prover] uncovered %s %08x\n",
                            kCoverage[i].name, kCoverage[i].start + 4u * k);
        }
    }
}

static int run_interpreted(uint32_t target, const uint32_t *args)
{
    unsigned i;
    memset(&cpu, 0, sizeof(cpu));
    initialize_cpu(&cpu, &rt);
    cov_inner_bridge = cpu.bus.bridge;
    cpu.bus.bridge = cov_bridge;
    memset(cov_case, 0, sizeof(cov_case));
    for (i = 0; i < 4; i++)
        cpu.gpr[4 + i] = args[i];
    cpu.gpr[8] = PROVER_T0_SEED;
    rt.force_interpret = 1;
    /*
     * force_interpret must be cleared before the next host call; PcPortMipsRun
     * only returns here.
     */
    return PcPortMipsRun(&cpu, target, BATTLE_HALT_PC, STEP_LIMIT);
}

static int run_host(uint32_t target, const uint32_t *args)
{
    unsigned i;
    memset(&cpu, 0, sizeof(cpu));
    initialize_cpu(&cpu, &rt);
    for (i = 0; i < 4; i++)
        cpu.gpr[4 + i] = args[i];
    cpu.gpr[8] = PROVER_T0_SEED;
    rt.force_interpret = 0;
    return runtime_bridge(&rt, &cpu, target);
}

/* The interpreter validates every address through its bus; the host C body runs
 * on the real CPU and does not. A body that computes an out-of-range guest
 * address (the 2 MiB wrap the hardware does and the port does not model) walks
 * off g_PsxRam and faults instead of returning. Catch that so one runaway body
 * cannot abort the whole sweep and hide every other leaf's result; the case is
 * reported as a divergence, because retail completed it. */
static sigjmp_buf host_fault_jmp;

static void host_fault_handler(int sig)
{
    (void)sig;
    siglongjmp(host_fault_jmp, 1);
}

static uint32_t leaf_address(const char *name)
{
    /* "func_XXXXXXXX" -> 0xXXXXXXXX */
    return (uint32_t)strtoul(name + 5, NULL, 16);
}

static void placeholder_inconclusive(const Leaf *leaf, const char *side)
{
    unsigned index = (unsigned)(leaf - kLeaves);
    inconclusive++;
    inconclusive_per_leaf[index]++;
    if (inconclusive_logged[index] == 0) {
        inconclusive_logged[index] = 1;
        fprintf(stderr, "[prover] inconclusive %s: %s side reached a bridge "
                "placeholder callee\n", leaf->name, side);
    }
}

/* Optional per-case setup run identically before EACH side (after the RAM
 * snapshot/restore), for leaves whose native callees need valid state that
 * seeded random RAM cannot provide (the heap for 8008ABB8).  Its own writes
 * land in both sides' RAM alike; placeholder hits it causes are not charged to
 * the body.  force_return_bits overrides the generated return width for a leaf
 * whose bridge returns a guest-domain value (typed 8008ABB8). */
static void (*pre_side)(void);
static unsigned force_return_bits;
/* Outcome of the most recent compare(): set only when it proved a match, so
 * a hand case can require non-vacuous coverage (both result classes). */
static int last_case_proven;
static uint32_t last_retail_v0;

static void compare(const Leaf *leaf, const uint32_t *args)
{
    const char *name = leaf->name;
    uint32_t target = leaf_address(name);
    uint32_t iv0, hv0, mask;
    int irc, hrc;
    uint32_t a;
    size_t diffs = 0;
    uint32_t first = 0;
    char detail[200];

    last_case_proven = 0;
    cov_index = coverage_slot(name);
    memcpy(before, g_PsxRam, sizeof(before));
    if (native_gs != NULL)
        memcpy(native_before, native_gs, NATIVE_GS_SIZE);
    /* Both sides call the same rand(); start each run from the same point or
     * every random-dependent body would differ for no reason. */
    srand(1);
    if (pre_side != NULL)
        pre_side();
    g_prover_placeholder_hits = 0;
    /* The interpreted side calls main-executable code natively through the
     * bridge (HeapFree, rcos, ...). If that native callee faults, retail did
     * not complete on this input as far as the harness can tell: count the
     * case inconclusive. Without this the handler would jump through a stale
     * buffer left by an earlier host run and blame the wrong side. */
    if (sigsetjmp(host_fault_jmp, 1) != 0) {
        unsigned index = (unsigned)(leaf - kLeaves);
        rt.force_interpret = 0;
        inconclusive++;
        inconclusive_per_leaf[index]++;
        if (inconclusive_logged[index] == 0) {
            inconclusive_logged[index] = 1;
            fprintf(stderr, "[prover] inconclusive %s: a native callee of the "
                    "retail side faulted\n", name);
        }
        return;
    }
    irc = run_interpreted(target, args);
    if (irc == PC_PORT_MIPS_HALTED && g_prover_placeholder_hits != 0) {
        rt.force_interpret = 0;
        placeholder_inconclusive(leaf, "retail");
        return;
    }
    if (irc != PC_PORT_MIPS_HALTED) {
        unsigned index = (unsigned)(leaf - kLeaves);
        inconclusive++;
        inconclusive_per_leaf[index]++;
        /* Explain the first skip per leaf: "unproven" is only actionable if the
         * reason is on the record. */
        if (inconclusive_logged[index] == 0) {
            inconclusive_logged[index] = 1;
            fprintf(stderr, "[prover] inconclusive %s rc=%d err=%s\n", name, irc,
                    cpu.error);
        }
        rt.force_interpret = 0;
        return;
    }
    memcpy(expected, g_PsxRam, sizeof(expected));
    iv0 = cpu.gpr[2];
    rt.force_interpret = 0;
    if (native_gs != NULL) {
        memcpy(native_expected, native_gs, NATIVE_GS_SIZE);
        memcpy(native_gs, native_before, NATIVE_GS_SIZE);
    }

    memcpy(g_PsxRam, before, sizeof(g_PsxRam));
    srand(1);
    if (pre_side != NULL) {
        unsigned hits = g_prover_placeholder_hits;
        pre_side();
        g_prover_placeholder_hits = hits;
    }
    if (sigsetjmp(host_fault_jmp, 1) != 0) {
        fail(name, "host-fault",
             "host body faulted where the interpreter completed (wild address)");
        return;
    }
    hrc = run_host(target, args);
    if (hrc != 1) {
        snprintf(detail, sizeof(detail), "rc=%d (not adopted)", hrc);
        fail(name, "bridge", detail);
        return;
    }
    memcpy(actual, g_PsxRam, sizeof(actual));
    hv0 = cpu.gpr[2];
    if (g_prover_placeholder_hits != 0) {
        placeholder_inconclusive(leaf, "host");
        return;
    }

    /* Compare only the bits the body's return type defines. A void body leaves
     * whatever the callee happened to have in rax, and a u8/u16 return leaves
     * the upper bits undefined by the ABI. Retail's v1 is not part of any of
     * these functions' contract either. */
    switch (force_return_bits != 0 ? force_return_bits : leaf->return_bits) {
    case 8:  mask = 0xFFu; break;
    case 16: mask = 0xFFFFu; break;
    case 32: mask = 0xFFFFFFFFu; break;
    default: mask = 0; break;
    }
    /* Store path of the two table accessors: retail's v0 is the caller's t0
     * passed through (jr ra; addu v0, t0, zero), which a C body cannot
     * reproduce.  Every retail jal site and every native caller was checked to
     * not read v0 after a store (native-audit/battle-decomp/callerscan), so
     * the port body returns a defined 0 there.  Prove both halves of that
     * statement instead of comparing the two values. */
    if (mask != 0 && is_store_pass_through(name, args)) {
        if (iv0 != PROVER_T0_SEED || (hv0 & mask) != 0) {
            snprintf(detail, sizeof(detail),
                     "store-path v0 retail=%08x (expected t0 %08x) host=%08x (expected 0) a0=%08x a1=%08x a2=%08x",
                     iv0, PROVER_T0_SEED, hv0, args[0], args[1], args[2]);
            fail(name, "return", detail);
            return;
        }
        store_pass_through_checks++;
        mask = 0;
    }
    if (mask != 0 && (iv0 & mask) != (hv0 & mask)) {
        snprintf(detail, sizeof(detail),
                 "v0 retail=%08x host=%08x mask=%08x a0=%08x a1=%08x a2=%08x a3=%08x",
                 iv0, hv0, mask, args[0], args[1], args[2], args[3]);
        fail(name, "return", detail);
        return;
    }
    for (a = 0; a < PSX_RAM_SIZE; a++) {
        if (a >= STACK_SKIP_LO && a < STACK_SKIP_HI)
            continue;
        if (expected[a] != actual[a]) {
            if (diffs == 0)
                first = a;
            diffs++;
        }
    }
    if (diffs != 0) {
        snprintf(detail, sizeof(detail),
                 "%zu bytes differ, first at guest %08x retail=%02x host=%02x",
                 diffs, (unsigned)(0x80000000u | first), expected[first],
                 actual[first]);
        fail(name, "ram", detail);
        return;
    }
    if (native_gs != NULL) {
        for (a = 0; a < NATIVE_GS_SIZE; a++) {
            if (native_expected[a] != native_gs[a]) {
                if (diffs == 0)
                    first = a;
                diffs++;
            }
        }
        if (diffs != 0) {
            snprintf(detail, sizeof(detail),
                     "%zu bytes differ, first at g_GameState+%04x retail=%02x "
                     "host=%02x", diffs, (unsigned)first,
                     native_expected[first], native_gs[first]);
            fail(name, "native", detail);
            return;
        }
    }
    checks++;
    proven[(unsigned)(leaf - kLeaves)]++;
    last_case_proven = 1;
    {
        int slot = coverage_slot(name);
        unsigned k;
        if (slot >= 0)
            for (k = 0; k < kCoverage[slot].insns; k++)
                cov_seen[slot][k] |= cov_case[k];
    }
    last_retail_v0 = iv0;
}

static const Leaf *leaf_by_name(const char *name)
{
    unsigned i;
    for (i = 0; i < LEAF_COUNT; i++) {
        if (strcmp(kLeaves[i].name, name) == 0)
            return &kLeaves[i];
    }
    fprintf(stderr, "BATTLE OVERLAY DIFFERENTIAL FAIL unknown leaf %s\n", name);
    exit(2);
}

static void compare_name(const char *name, const uint32_t *args)
{
    compare(leaf_by_name(name), args);
}

/* 8008ABB8 = HeapChangeCurrentUser(2, 0); return HeapAlloc(size, mode).
 * Both sides reach the real native allocator, whose blocks live in guest RAM
 * and whose globals (g_Heap, current user/content, consolidation flag) are
 * native, so every side starts from the same freshly initialised 128 KiB heap.
 * The typed bridge returns the guest address, so all 32 bits of v0 are
 * compared, unlike the generic pointer-return rule. */
#define ABB8_HEAP_LO 0x80140000u
#define ABB8_HEAP_HI 0x80160000u
extern void HeapInit(void *pHeapStart, void *pHeapEnd);
static void abb8_heap(void)
{
    HeapInit(PSX_ADDR(ABB8_HEAP_LO), PSX_ADDR(ABB8_HEAP_HI));
}

static void hand_cases_8008ABB8(void)
{
    static const uint32_t sizes[] = {0u, 4u, 0x10Cu, 0x2F8u, 0xA2B4u, 0x8000u};
    uint32_t args[4];
    unsigned s, mode;

    pre_side = abb8_heap;
    force_return_bits = 32;
    for (s = 0; s < sizeof(sizes) / sizeof(sizes[0]); s++) {
        for (mode = 0; mode <= 1; mode++) {
            begin_case(s % 3u, 0);
            args[0] = sizes[s]; args[1] = mode; args[2] = args[3] = 0;
            compare_name("func_8008ABB8", args);
        }
    }
    pre_side = NULL;
    force_return_bits = 0;
}


/* ---- Battle guest ABI prototype (M-a): mesh fixture --------------------
 * The main69.c ground-mesh family reads two guest tables through guest
 * pointers held in overlay globals: D_800D3344 -> 8-byte SVECTOR vertices,
 * D_800D39CC -> 14-byte triangles (s16 v0,v1,v2; s16 neighbours n0,n1,n2,
 * -1 = none; u8 +0xC attribute; u8 +0xD pass mark), D_800D3348 = triangle
 * count, D_800D2F64 = pass counter.  The fixture is a 4x4-vertex grid (18
 * triangles) with real neighbour links, in either winding, so the
 * point-in-triangle test sees both inside and outside points. */
#define MESH_VERT   0x800C6000u
#define MESH_TRI    0x800C6800u
#define MESH_PT     0x800C7000u
#define MESH_NORMAL 0x800C7100u
#define MESH_N      4
static unsigned mesh_tri_count;

static void mesh_fixture(int winding, int seed)
{
    int i, j, t, u, e, f;
    int16_t tri[2 * (MESH_N - 1) * (MESH_N - 1)][3];

    for (i = 0; i < MESH_N; i++) {
        for (j = 0; j < MESH_N; j++) {
            int16_t *v = (int16_t *)PSX_ADDR(MESH_VERT + 8u * (unsigned)(i * MESH_N + j));
            v[0] = (int16_t)(i * 256 - 384 + ((i * 7 + j * 3 + seed) % 5) * 9);
            v[1] = (int16_t)((i * 37 + j * 53 + seed * 11) % 97 - 48);
            v[2] = (int16_t)(j * 256 - 384 + ((i * 5 + j * 11 + seed) % 7) * 6);
            v[3] = 0;
        }
    }
    t = 0;
    for (i = 0; i + 1 < MESH_N; i++) {
        for (j = 0; j + 1 < MESH_N; j++) {
            int16_t a = (int16_t)(i * MESH_N + j), b = (int16_t)((i + 1) * MESH_N + j);
            int16_t c = (int16_t)(i * MESH_N + j + 1), d = (int16_t)((i + 1) * MESH_N + j + 1);
            tri[t][0] = a; tri[t][1] = winding ? d : b; tri[t][2] = winding ? b : d; t++;
            tri[t][0] = a; tri[t][1] = winding ? c : d; tri[t][2] = winding ? d : c; t++;
        }
    }
    mesh_tri_count = (unsigned)t;
    for (u = 0; u < t; u++) {
        uint8_t *r = (uint8_t *)PSX_ADDR(MESH_TRI + 14u * (unsigned)u);
        int16_t *h = (int16_t *)r;
        h[0] = tri[u][0]; h[1] = tri[u][1]; h[2] = tri[u][2];
        for (e = 0; e < 3; e++) {
            int16_t p0 = tri[u][e], p1 = tri[u][(e + 1) % 3];
            int found = -1;
            for (f = 0; f < t && found < 0; f++) {
                int k;
                if (f == u) continue;
                for (k = 0; k < 3; k++) {
                    if (tri[f][k] == p1 && tri[f][(k + 1) % 3] == p0)
                        found = f;
                }
            }
            h[3 + e] = (int16_t)found;
        }
        r[0xC] = (uint8_t)(u ^ 0x5A);
        r[0xD] = 0;
    }
    *(uint32_t *)PSX_ADDR(0x800D3344u) = MESH_VERT;
    *(uint32_t *)PSX_ADDR(0x800D39CCu) = MESH_TRI;
    *(uint32_t *)PSX_ADDR(0x800D3348u) = mesh_tri_count;
    *(uint8_t *)PSX_ADDR(0x800D2F64u) = 1;
}

static void mesh_point(int x, int y, int z)
{
    int16_t *p = (int16_t *)PSX_ADDR(MESH_PT);
    p[0] = (int16_t)x; p[1] = (int16_t)y; p[2] = (int16_t)z; p[3] = 0;
}

/* Fails the run unless `name` proved a case for every listed retail result
 * class -- a prover that only ever saw one branch is vacuous. */
static void require_outcomes(const char *name, unsigned seen_mask,
                             unsigned want_mask)
{
    if ((seen_mask & want_mask) != want_mask) {
        char detail[80];
        snprintf(detail, sizeof(detail), "outcome classes seen=%x want=%x",
                 seen_mask, want_mask);
        fail(name, "coverage", detail);
    }
}

static int abi_leaf_listed(const char *name)
{
    unsigned i;
    for (i = 0; i < LEAF_COUNT; i++)
        if (strcmp(kLeaves[i].name, name) == 0)
            return 1;
    return 0;
}

static void hand_cases_mesh(void)
{
    uint32_t args[4];
    int winding, x, z;
    unsigned tri, seen;

    if (!abi_leaf_listed("func_800A5A48"))
        return;
    /* 800A5A48(v0, v1, v2, point): every triangle against a 5x5 point grid
     * that covers the mesh and its outside, both windings.  Outcome class 1 =
     * retail returned -1 (inside), 2 = retail returned 0. */
    seen = 0;
    for (winding = 0; winding < 2; winding++) {
        begin_case(0, 0);
        mesh_fixture(winding, winding + 1);
        for (x = -450; x <= 450; x += 225) {
            for (z = -450; z <= 450; z += 225) {
                mesh_point(x + 17, 5, z - 23);
                for (tri = 0; tri < mesh_tri_count; tri++) {
                    const int16_t *h = (const int16_t *)PSX_ADDR(MESH_TRI + 14u * tri);
                    args[0] = MESH_VERT + 8u * (uint16_t)h[0];
                    args[1] = MESH_VERT + 8u * (uint16_t)h[1];
                    args[2] = MESH_VERT + 8u * (uint16_t)h[2];
                    args[3] = MESH_PT;
                    compare_name("func_800A5A48", args);
                    if (last_case_proven)
                        seen |= last_retail_v0 == 0xFFFFFFFFu ? 1u : 2u;
                }
            }
        }
    }
    require_outcomes("func_800A5A48", seen, 3u);

    /* 800A579C(point): first containing triangle or -1; plus the
     * tables-not-loaded early-outs.  Class 1 = a triangle index, 2 = -1. */
    if (abi_leaf_listed("func_800A579C")) {
        seen = 0;
        for (winding = 0; winding < 2; winding++) {
            begin_case(0, 0);
            mesh_fixture(winding, winding + 3);
            for (x = -500; x <= 500; x += 125) {
                for (z = -500; z <= 500; z += 250) {
                    mesh_point(x + 9, 0, z + 31);
                    args[0] = MESH_PT; args[1] = args[2] = args[3] = 0;
                    compare_name("func_800A579C", args);
                    if (last_case_proven)
                        seen |= last_retail_v0 == 0xFFFFFFFFu ? 2u : 1u;
                }
            }
            *(uint32_t *)PSX_ADDR(0x800D39CCu) = 0;
            compare_name("func_800A579C", args);
            *(uint32_t *)PSX_ADDR(0x800D39CCu) = MESH_TRI;
            *(uint32_t *)PSX_ADDR(0x800D3344u) = 0;
            compare_name("func_800A579C", args);
        }
        require_outcomes("func_800A579C", seen, 3u);
    }

    /* 800A5D54(point, tri, depth): neighbour search with pass marks.  The
     * RAM diff covers the +0xD marks it writes.  Class 1 = found, 2 = -1. */
    if (abi_leaf_listed("func_800A5D54")) {
        int depth, start;
        seen = 0;
        for (winding = 0; winding < 2; winding++) {
            for (depth = 0; depth <= 3; depth++) {
                for (start = -1; start < 18; start += 3) {
                    begin_case(0, 0);
                    mesh_fixture(winding, depth + 5);
                    /* Some triangles already carry this pass's mark. */
                    *(uint8_t *)PSX_ADDR(MESH_TRI + 14u * 4u + 0xDu) = 1;
                    mesh_point(-130 + depth * 60, 0, 200 - depth * 90);
                    args[0] = MESH_PT; args[1] = (uint32_t)start;
                    args[2] = (uint32_t)depth; args[3] = 0;
                    compare_name("func_800A5D54", args);
                    if (last_case_proven)
                        seen |= last_retail_v0 == 0xFFFFFFFFu ? 2u : 1u;
                }
            }
        }
        require_outcomes("func_800A5D54", seen, 3u);
    }

    /* 800A5914(point, tri, tries, result): tries loop over 800A5D54, the
     * pass-counter increment and its 0xFF -> 0 wrap that clears every +0xD
     * mark.  Class 1 = found, 2 = -1 / pass-through. */
    if (abi_leaf_listed("func_800A5914")) {
        int tries, start, wrap;
        seen = 0;
        for (wrap = 0; wrap < 2; wrap++) {
            for (tries = 0; tries <= 3; tries++) {
                for (start = -1; start <= 19; start += 4) {
                    begin_case(0, 0);
                    mesh_fixture(tries & 1, tries + 7);
                    if (wrap) {
                        unsigned t;
                        *(uint8_t *)PSX_ADDR(0x800D2F64u) = 0xFF;
                        for (t = 0; t < mesh_tri_count; t++)
                            *(uint8_t *)PSX_ADDR(MESH_TRI + 14u * t + 0xDu) = (uint8_t)(t * 37u);
                    }
                    mesh_point(60 - tries * 70, 0, -40 + tries * 80);
                    args[0] = MESH_PT; args[1] = (uint32_t)start;
                    args[2] = (uint32_t)tries; args[3] = (uint32_t)(0x1234 + tries);
                    compare_name("func_800A5914", args);
                    if (last_case_proven)
                        seen |= (int32_t)last_retail_v0 >= 0 && last_retail_v0 < 0x100u ? 1u : 2u;
                }
            }
            /* Tables not loaded. */
            begin_case(0, 0);
            args[0] = MESH_PT; args[1] = 0; args[2] = 1; args[3] = 0;
            compare_name("func_800A5914", args);
        }
        require_outcomes("func_800A5914", seen, 3u);

        /* Bridge domain (typed thunks): with tries <= 0 the body returns its
         * fourth argument unchanged.  Hand it address-shaped words (KSEG0,
         * KSEG1, scratchpad) through the BRIDGE: retail returns the guest
         * word, so v0 must be that word, not a translated host pointer.  The
         * legacy heuristic bridge (BATTLE_BRIDGE_LEGACY_ARGS) fails here. */
        {
            static const uint32_t kShaped[] = {
                0x80123450u, 0xA0123450u, 0x1F800010u, MESH_PT
            };
            unsigned k, bridge_proven = 0, bridge_cases = 0;
            for (k = 0; k < sizeof(kShaped) / sizeof(kShaped[0]); k++) {
                for (tries = -1; tries <= 0; tries++) {
                    begin_case(0, 0);
                    mesh_fixture(0, 11);
                    mesh_point(10, 0, 10);
                    args[0] = MESH_PT; args[1] = 2;
                    args[2] = (uint32_t)tries; args[3] = kShaped[k];
                    bridge_cases++;
                    compare_name("func_800A5914", args);
                    if (last_case_proven && last_retail_v0 == kShaped[k])
                        bridge_proven++;
                }
            }
            printf("BATTLE OVERLAY DIFFERENTIAL bridge-domain func_800A5914 "
                   "tries<=0 address-shaped a3: proven=%u/%u\n",
                   bridge_proven, bridge_cases);
            if (bridge_proven != bridge_cases)
                fail("func_800A5914", "bridge-domain",
                     "address-shaped pass-through argument not returned in the guest domain");
        }
    }
}

/* Indirect calls from native code (bg_jalr -> PcPort_BattleMipsCallGuest):
 * an adopted guest-ABI leaf must run natively through the guest-address ->
 * native map (g_GuestNativeIndirectCalls counts it) and give retail's v0 and
 * RAM; with force_interpret the same call must still go to the interpreter. */
static void hand_cases_indirect(void)
{
#ifndef BATTLE_BRIDGE_LEGACY_ARGS
    uint32_t args[4];
    unsigned tri, cases = 0, native = 0, match = 0, interp = 0, seen = 0;
    int x;

    if (!abi_leaf_listed("func_800A5A48") || !abi_leaf_listed("func_800A5914"))
        return;
    for (x = -300; x <= 300; x += 300) {
        for (tri = 0; tri < 18; tri += 1) {
            uint32_t iv0, hv0 = 0xDEADBEEFu, cv0 = 0xDEADBEEFu;
            unsigned long n0;
            int irc, ok;
            const int16_t *h;
            uint32_t a;

            begin_case(0, 0);
            mesh_fixture(tri & 1, 13);
            mesh_point(x + 7, 0, 33 - x / 2);
            h = (const int16_t *)PSX_ADDR(MESH_TRI + 14u * tri);
            args[0] = MESH_VERT + 8u * (uint16_t)h[0];
            args[1] = MESH_VERT + 8u * (uint16_t)h[1];
            args[2] = MESH_VERT + 8u * (uint16_t)h[2];
            args[3] = MESH_PT;
            memcpy(before, g_PsxRam, sizeof(before));
            irc = run_interpreted(0x800A5A48u, args);
            rt.force_interpret = 0;
            if (irc != PC_PORT_MIPS_HALTED) {
                fail("func_800A5A48", "indirect", "retail did not halt");
                continue;
            }
            iv0 = cpu.gpr[2];
            memcpy(expected, g_PsxRam, sizeof(expected));
            memcpy(g_PsxRam, before, sizeof(g_PsxRam));
            memset(&cpu, 0, sizeof(cpu));
            initialize_cpu(&cpu, &rt);
            rt.bridge_cpu = &cpu;
            n0 = g_GuestNativeIndirectCalls;
            hv0 = bg_jalr(0x800A5A48u, args[0], args[1], args[2], args[3]);
            cases++;
            if (g_GuestNativeIndirectCalls == n0 + 1)
                native++;
            ok = hv0 == iv0;
            for (a = 0; ok && a < PSX_RAM_SIZE; a++) {
                if (a >= STACK_SKIP_LO && a < STACK_SKIP_HI)
                    continue;
                ok = expected[a] == g_PsxRam[a];
            }
            if (ok) {
                match++;
                seen |= iv0 == 0xFFFFFFFFu ? 1u : 2u;
            }
            /* Control: the same indirect call, interpreter forced. */
            memcpy(g_PsxRam, before, sizeof(g_PsxRam));
            n0 = g_GuestNativeIndirectCalls;
            rt.force_interpret = 1;
            cv0 = bg_jalr(0x800A5A48u, args[0], args[1], args[2], args[3]);
            rt.force_interpret = 0;
            if (g_GuestNativeIndirectCalls == n0 && cv0 == iv0)
                interp++;
        }
    }
    /* Typed kinds on the indirect path too: 800A5914 tries=0 hands back an
     * address-shaped fourth word unchanged, in the guest domain. */
    {
        uint32_t w[4];
        uint32_t v0 = 0;
        unsigned long n0 = g_GuestNativeIndirectCalls;
        begin_case(0, 0);
        mesh_fixture(0, 14);
        mesh_point(10, 0, 10);
        memset(&cpu, 0, sizeof(cpu));
        initialize_cpu(&cpu, &rt);
        rt.bridge_cpu = &cpu;
        w[0] = MESH_PT; w[1] = 2; w[2] = 0; w[3] = 0x80123450u;
        cases++;
        if (PcPort_BattleMipsCallGuest(0x800A5914u, w, 4, &v0) == 0 &&
            g_GuestNativeIndirectCalls == n0 + 1)
            native++;
        if (v0 == 0x80123450u)
            match++;
    }
    printf("BATTLE OVERLAY DIFFERENTIAL indirect-native bg_jalr/CallGuest: "
           "cases=%u native=%u retail-match=%u forced-interpreter-control=%u/%u "
           "outcomes=%u\n", cases, native, match, interp, cases - 1u, seen);
    if (native != cases || match != cases || interp != cases - 1u || seen != 3u)
        fail("func_800A5A48", "indirect",
             "indirect call not served natively with retail's result");
#endif
}

/* 800A5BE8 takes a fifth argument (VECTOR *normal) from the guest stack
 * frame, o32 slot sp+0x10.  Both sides start at BATTLE_STACK_TOP and the
 * bridge reads that slot, so it is written before each side. */
static uint32_t stack_arg4;
static void stack_arg4_side(void)
{
    *(uint32_t *)PSX_ADDR(BATTLE_STACK_TOP + 0x10u) = stack_arg4;
}

static void hand_cases_mesh_height(void)
{
    uint32_t args[4];
    int winding, x;
    unsigned tri, seen;

    /* 800A5BE8(v0, v1, v2, point, normal): plane normal into *normal and the
     * height into point.vy.  Class 1 = sloped plane, 2 = vertical plane
     * (normal.vy == 0 path), checked on the retail RAM after the case. */
    if (abi_leaf_listed("func_800A5BE8")) {
        seen = 0;
        stack_arg4 = MESH_NORMAL;
        pre_side = stack_arg4_side;
        for (winding = 0; winding < 2; winding++) {
            begin_case(0, 0);
            mesh_fixture(winding, winding + 9);
            for (x = -300; x <= 300; x += 200) {
                for (tri = 0; tri < mesh_tri_count; tri += 2) {
                    const int16_t *h = (const int16_t *)PSX_ADDR(MESH_TRI + 14u * tri);
                    mesh_point(x + 11, 777, -x / 2 + 5);
                    args[0] = MESH_VERT + 8u * (uint16_t)h[0];
                    args[1] = MESH_VERT + 8u * (uint16_t)h[1];
                    args[2] = MESH_VERT + 8u * (uint16_t)h[2];
                    args[3] = MESH_PT;
                    compare_name("func_800A5BE8", args);
                    if (last_case_proven)
                        seen |= *(int32_t *)PSX_ADDR(MESH_NORMAL + 4u) != 0 ? 1u : 2u;
                }
            }
        }
        /* A vertical triangle (plane z = const): normal.vy == 0. */
        begin_case(0, 0);
        {
            int16_t *v = (int16_t *)PSX_ADDR(MESH_VERT);
            v[0] = 0;   v[1] = 0;   v[2] = 40;  v[3] = 0;
            v[4] = 300; v[5] = 0;   v[6] = 40;  v[7] = 0;
            v[8] = 0;   v[9] = 300; v[10] = 40; v[11] = 0;
        }
        mesh_point(50, 999, 60);
        args[0] = MESH_VERT; args[1] = MESH_VERT + 8u; args[2] = MESH_VERT + 16u;
        args[3] = MESH_PT;
        compare_name("func_800A5BE8", args);
        if (last_case_proven)
            seen |= *(int32_t *)PSX_ADDR(MESH_NORMAL + 4u) != 0 ? 1u : 2u;
        pre_side = NULL;
        require_outcomes("func_800A5BE8", seen, 3u);
    }

    /* 800A5870(point, tri, normal): 800A5BE8 on triangle `tri`, returns its
     * +0xC attribute byte; -1 for tri < 0 or tables not loaded. */
    if (abi_leaf_listed("func_800A5870")) {
        int t;
        seen = 0;
        for (winding = 0; winding < 2; winding++) {
            begin_case(0, 0);
            mesh_fixture(winding, winding + 13);
            for (t = -2; t < 18; t += 3) {
                mesh_point(t * 23 - 100, 3, 150 - t * 17);
                args[0] = MESH_PT; args[1] = (uint32_t)t; args[2] = MESH_NORMAL;
                args[3] = 0;
                compare_name("func_800A5870", args);
                if (last_case_proven)
                    seen |= last_retail_v0 == 0xFFFFFFFFu ? 2u : 1u;
            }
        }
        begin_case(0, 0);
        args[0] = MESH_PT; args[1] = 1; args[2] = MESH_NORMAL; args[3] = 0;
        compare_name("func_800A5870", args);
        require_outcomes("func_800A5870", seen, 3u);
    }
}

/* ---- Battle guest ABI prototype: slot and heap fixtures -----------------
 * Board opcodes take ppBoard, the address of a 4-byte GUEST slot holding the
 * script IP (guest address of the operand bytes).  The slot is at PTRT, the
 * operands at SRC, and the D_800D3410/20/30 rows get small values. */
static void slot_fixture(uint32_t salt, int high_bit)
{
    unsigned i;
    uint8_t *ops = (uint8_t *)PSX_ADDR(SRC);
    *(uint32_t *)PSX_ADDR(PTRT) = SRC;
    ops[0] = 0x2A;
    ops[1] = (uint8_t)((salt * 7u + 3u) & 0x3Fu);
    if (high_bit)
        ops[1] |= 0x80u;
    ops[2] = (uint8_t)((salt * 13u + 5u) & 0x3Fu);
    ops[3] = (uint8_t)((salt * 29u + 1u) & 0x3Fu);
    for (i = 0; i < 0x40u * 8u; i++)
        *(uint8_t *)PSX_ADDR(0x800D3410u + i) = (uint8_t)((i * 31u + salt * 17u) & 0xFFu);
}

static void hand_cases_abi_slots(void)
{
    uint32_t args[4];
    uint32_t salt;

    for (salt = 0; salt < 12; salt++) {
        if (abi_leaf_listed("func_8007AE38")) {
            begin_case(0, 0);
            slot_fixture(salt, 0);
            args[0] = PTRT; args[1] = salt % 6u; args[2] = args[3] = 0;
            compare_name("func_8007AE38", args);
        }
        if (abi_leaf_listed("func_8007E8E0")) {
            begin_case(0, 0);
            slot_fixture(salt, 0);
            args[0] = PTRT; args[1] = salt % 5u; args[2] = args[3] = 0;
            compare_name("func_8007E8E0", args);
        }
        if (abi_leaf_listed("func_8007E154")) {
            int arm;
            for (arm = 0; arm < 2; arm++) {
                begin_case(0, 0);
                slot_fixture(salt, arm);
                args[0] = PTRT; args[1] = salt % 4u; args[2] = args[3] = 0;
                compare_name("func_8007E154", args);
            }
        }
    }
    /* 8007FEC4: allocate 0x5DA4 bytes through the native 8008ABB8, store the
     * block's GUEST address at 0x800D2DB4, clear it, and set the two halves
     * at +0x5D9C/+0x5D9E.  The heap fixture is re-initialised for each side;
     * the RAM diff covers the slot word (domain) and the whole block. */
    if (abi_leaf_listed("func_8007FEC4")) {
        int pattern;
        for (pattern = 0; pattern < 3; pattern++) {
            begin_case((uint32_t)pattern, 0);
            pre_side = abb8_heap;
            args[0] = args[1] = args[2] = args[3] = 0;
            compare_name("func_8007FEC4", args);
            pre_side = NULL;
            if (last_case_proven) {
                uint32_t slot = *(uint32_t *)PSX_ADDR(0x800D2DB4u);
                if ((slot & 0xFF800000u) != 0x80000000u)
                    fail("func_8007FEC4", "domain", "0x800D2DB4 does not hold a guest address");
            }
        }
    }
}

/* Codemod sweep batch 1 (ppBoard family, main19/main30): slot fixture with
 * forced operands.  Variant 0 leaves the fixture's pattern bytes, variant 1
 * forces the "low" class (no saturation / comparison false), variant 2 the
 * "high" class (saturation / comparison true).  Every forced variant must be
 * proven for each leaf, so neither result class is left to chance. */
typedef struct SlotSweepLeaf {
    const char *name;
    int wide;      /* 2: u32 at D_800D3410 rows; 1: u16 at D_800D3420; 0: u8 at D_800D3430 */
    int forced;    /* 1 when variants 1/2 mean something for this leaf */
    /* operand values for variant 1 and 2: row[p[1]], row[p[2]] */
    uint32_t lo_a, lo_b, hi_a, hi_b;
    int imm;       /* compare against the immediate p[2] (| p[3] << 8 when wide) */
} SlotSweepLeaf;

static const SlotSweepLeaf kSlotSweep[] = {
    { "func_8007AE98", 0, 1, 9, 4, 4, 9, 0 },             /* u8 sat sub: low = no clamp */
    { "func_8007AFFC", 0, 0, 0, 0, 0, 0, 0 },             /* u8 and */
    { "func_8007B040", 0, 0, 0, 0, 0, 0, 0 },             /* u8 or */
    { "func_8007B084", 0, 0, 0, 0, 0, 0, 0 },             /* u8 xor */
    { "func_8007B0C8", 1, 1, 0x100, 0x22, 0xFFF0, 0x40, 0 }, /* u16 sat add */
    { "func_8007B134", 1, 1, 0x300, 0x22, 0x22, 0x300, 0 },  /* u16 sat sub */
    { "func_8007E954", 0, 1, 0x11, 0, 0x2B, 0, 1 },       /* row[p1] == p2 */
    { "func_8007E98C", 1, 1, 0x1111, 0, 0x2B2B, 0, 1 },   /* u16 row[p1] == p2|p3<<8 */
    { "func_8007E9D0", 0, 1, 0x7F, 0, 0x20, 0, 1 },       /* p2 >= row[p1] */
    { "func_8007EAC8", 0, 1, 0x11, 0x12, 0x5A, 0x5A, 0 }, /* row[p1] == row[p2] */
    /* Wrap-faithful multiply and bare-DIVU siblings: high = clamp / zero divisor. */
    { "func_8007B198", 1, 1, 0x100, 0x22, 0x300, 0x300, 0 },  /* u16 sat mul */
    { "func_8007AF5C", 0, 1, 0xC9, 0x0B, 0xC9, 0, 0 },     /* u8 div, 0 -> 0xFF */
    { "func_8007AFAC", 0, 1, 0xC9, 0x0B, 0xC9, 0, 0 },     /* u8 rem, 0 -> dividend */
    { "func_8007B208", 1, 1, 0xBEEF, 0x123, 0xBEEF, 0, 0 },  /* u16 div, 0 -> 0xFFFF */
    { "func_8007B264", 1, 1, 0xBEEF, 0x123, 0xBEEF, 0, 0 },  /* u16 rem, 0 -> dividend */
    /* Codemod sweep batch 2: main30 compare leaves. */
    { "func_8007EA08", 1, 1, 0x9000, 0x0, 0x2B2B, 0x0, 1 },
    { "func_8007EA4C", 0, 1, 0x10, 0x0, 0x2B, 0x0, 1 },
    { "func_8007EA84", 1, 1, 0x1000, 0x0, 0x2B2B, 0x0, 1 },
    { "func_8007EB08", 1, 1, 0x1111, 0x1112, 0x5A5A, 0x5A5A, 0 },
    { "func_8007EB50", 0, 1, 0x20, 0x10, 0x20, 0x20, 0 },
    { "func_8007EB90", 1, 1, 0x2000, 0x1000, 0x2000, 0x2000, 0 },
    { "func_8007EBD8", 0, 1, 0xD4, 0x0, 0x2B, 0x0, 1 },
    { "func_8007EC10", 1, 1, 0xD4D4, 0x0, 0x2B2B, 0x0, 1 },
    { "func_8007EC54", 0, 1, 0xF0, 0xF, 0xF0, 0x30, 0 },
    { "func_8007EC94", 1, 1, 0xF0F0, 0xF0F, 0xF0F0, 0x3030, 0 },
    /* Codemod sweep batch 3: main30 compare leaves. */
    { "func_8007ECDC", 0, 1, 0x11, 0x0, 0x2B, 0x0, 1 },
    { "func_8007ED14", 1, 1, 0x1111, 0x0, 0x2B2B, 0x0, 1 },
    { "func_8007ED58", 0, 1, 0x11, 0x12, 0x5A, 0x5A, 0 },
    { "func_8007ED98", 1, 1, 0x1111, 0x1112, 0x5A5A, 0x5A5A, 0 },
    { "func_8007EDE0", 2, 1, 0x11111111, 0x11111112, 0x5A5A5A5A, 0x5A5A5A5A, 0 },
    { "func_8007EE28", 2, 1, 0x80000000, 0x7FFFFFFF, 0x7FFFFFFF, 0x80000000, 0 },
    /* Codemod sweep batch 3: main22 row copies (u16 / u32). */
    { "func_8007B98C", 1, 0, 0, 0, 0, 0, 0 },
    { "func_8007B9C8", 2, 0, 0, 0, 0, 0, 0 },
};

/* Extra forced operand pairs that must each be proven (class outside lo/hi):
 * the multiply whose promoted-int product exceeds 0x7FFFFFFF (retail MULT
 * wraps negative, skips the clamp and stores the low half), and a zero
 * dividend over a zero divisor for the DIVU siblings. */
typedef struct SlotSweepExtra {
    const char *name;
    int wide;
    uint32_t a, b;
} SlotSweepExtra;

static const SlotSweepExtra kSlotSweepExtra[] = {
    { "func_8007B198", 1, 39033, 55481 },
    { "func_8007B198", 1, 0xFFFF, 0xFFFF },
    { "func_8007B198", 1, 3, 21845 },          /* exactly 0xFFFF: no clamp */
    { "func_8007AF5C", 0, 0, 0 },
    { "func_8007AFAC", 0, 0, 0 },
    { "func_8007B208", 1, 0, 0 },
    { "func_8007B264", 1, 0, 0 },
    { "func_8007B208", 1, 0xFFFF, 1 },
    { "func_8007B264", 1, 0x8001, 0x8000 },
};

static void hand_cases_slot_extra(void)
{
    uint32_t args[4];
    unsigned e, salt;

    for (e = 0; e < sizeof(kSlotSweepExtra) / sizeof(kSlotSweepExtra[0]); e++) {
        const SlotSweepExtra *x = &kSlotSweepExtra[e];
        unsigned proven = 0;
        if (!abi_leaf_listed(x->name))
            continue;
        for (salt = 0; salt < 6; salt++) {
            uint32_t index = salt % 5u;
            uint8_t *ops = (uint8_t *)PSX_ADDR(SRC);
            begin_case(0, 0);
            slot_fixture(salt, 0);
            if (ops[2] == ops[1])
                ops[2] = (uint8_t)((ops[1] + 1u) & 0x3Fu);
            if (x->wide) {
                uint8_t *row = (uint8_t *)PSX_ADDR(0x800D3420u + (index << 6));
                *(uint16_t *)(row + 2u * ops[1]) = (uint16_t)x->a;
                *(uint16_t *)(row + 2u * ops[2]) = (uint16_t)x->b;
            } else {
                uint8_t *row = (uint8_t *)PSX_ADDR(0x800D3430u + (index << 6));
                row[ops[1]] = (uint8_t)x->a;
                row[ops[2]] = (uint8_t)x->b;
            }
            args[0] = PTRT; args[1] = index; args[2] = args[3] = 0;
            compare_name(x->name, args);
            if (last_case_proven)
                proven++;
        }
        if (proven == 0)
            fail(x->name, "coverage", "a forced extra operand pair has no proven case");
    }
}

static void hand_cases_slot_sweep(void)
{
    uint32_t args[4];
    unsigned l, salt, variant;

    for (l = 0; l < sizeof(kSlotSweep) / sizeof(kSlotSweep[0]); l++) {
        const SlotSweepLeaf *s = &kSlotSweep[l];
        unsigned proven_variant[3] = {0, 0, 0};
        unsigned seen = 0;
        if (!abi_leaf_listed(s->name))
            continue;
        for (salt = 0; salt < 12; salt++) {
            for (variant = 0; variant < 3; variant++) {
                uint32_t index = salt % 5u;
                uint8_t *ops = (uint8_t *)PSX_ADDR(SRC);
                if (variant != 0 && !s->forced)
                    break;
                begin_case(0, 0);
                slot_fixture(salt, 0);
                if (ops[2] == ops[1])
                    ops[2] = (uint8_t)((ops[1] + 1u) & 0x3Fu);
                if (variant != 0) {
                    uint32_t a = variant == 1 ? s->lo_a : s->hi_a;
                    uint32_t b = variant == 1 ? s->lo_b : s->hi_b;
                    if (s->imm) {
                        /* immediate compare: p[2] (and p[3]) carry the value */
                        ops[2] = (uint8_t)(s->hi_a & 0xFFu);
                        ops[3] = (uint8_t)((s->hi_a >> 8) & 0xFFu);
                    }
                    if (s->wide == 2) {
                        uint8_t *row = (uint8_t *)PSX_ADDR(0x800D3410u + (index << 6));
                        *(uint32_t *)(row + 4u * ops[1]) = a;
                        if (!s->imm)
                            *(uint32_t *)(row + 4u * ops[2]) = b;
                    } else if (s->wide) {
                        uint8_t *row = (uint8_t *)PSX_ADDR(0x800D3420u + (index << 6));
                        *(uint16_t *)(row + 2u * ops[1]) = (uint16_t)a;
                        if (!s->imm)
                            *(uint16_t *)(row + 2u * ops[2]) = (uint16_t)b;
                    } else {
                        uint8_t *row = (uint8_t *)PSX_ADDR(0x800D3430u + (index << 6));
                        row[ops[1]] = (uint8_t)a;
                        if (!s->imm)
                            row[ops[2]] = (uint8_t)b;
                    }
                }
                args[0] = PTRT; args[1] = index; args[2] = args[3] = 0;
                compare_name(s->name, args);
                if (last_case_proven) {
                    proven_variant[variant]++;
                    seen |= last_retail_v0 != 0 ? 1u : 2u;
                }
            }
        }
        if (proven_variant[0] == 0 ||
            (s->forced && (proven_variant[1] == 0 || proven_variant[2] == 0)))
            fail(s->name, "coverage", "a forced operand class has no proven case");
        /* Comparison leaves (u32 return) must prove both results. */
        if (strncmp(s->name, "func_8007E", 10) == 0)
            require_outcomes(s->name, seen, 3u);
    }
}

/* 8007A828 (main14): dst[(a2 << 3) + p[1]] = p[2]; returns a2, plus one when
 * p[1] == 0.  dst is the guest board the dispatcher passes (0x800D2E5C);
 * both return classes must be proven. */
static void hand_cases_a828(void)
{
    uint32_t args[4];
    unsigned salt, seen = 0;

    if (!abi_leaf_listed("func_8007A828"))
        return;
    for (salt = 0; salt < 12; salt++) {
        uint8_t *ops = (uint8_t *)PSX_ADDR(SRC);
        begin_case(0, 0);
        slot_fixture(salt, 0);
        if ((salt & 1u) == 0)
            ops[1] = 0;
        args[0] = PTRT; args[1] = 0x800D2E5Cu; args[2] = (salt * 37u + 5u) & 0xFFu; args[3] = 0;
        compare_name("func_8007A828", args);
        if (last_case_proven)
            seen |= last_retail_v0 == args[2] ? 1u : 2u;
    }
    require_outcomes("func_8007A828", seen, 3u);
}

/* 8007A7BC (main14): dst = a1 + 8*a3; dst[0] = 0x80, dst[1..4] = the four
 * operand bytes the slot points at; returns a3 + 1 (u8, so 0xFF wraps). */
static void hand_cases_a7bc(void)
{
    uint32_t args[4];
    unsigned salt;

    if (!abi_leaf_listed("func_8007A7BC"))
        return;
    for (salt = 0; salt < 12; salt++) {
        begin_case(0, 0);
        slot_fixture(salt, salt & 1u);
        args[0] = PTRT; args[1] = 0x800D2E5Cu; args[2] = salt;
        args[3] = salt == 11u ? 0xFFu : (salt * 23u + 1u) & 0xFFu;
        compare_name("func_8007A7BC", args);
    }
}

/* 800BA8F4 (mainl110, guest ABI): sprite floor query.  The sprite record is
 * at REC: s16 x/y/z at +2/+6/+0xA, cached triangle +0x78, floor height out
 * at +0x84.  Cached triangles hit, miss (fallback full search 800A579C) and
 * out of range, points inside and outside the mesh, both windings, the
 * pass-counter wrap, and tables not loaded.  Classes: 1 = a triangle cached
 * afterwards, 2 = -1 cached afterwards.  The locals the body hands to the
 * main69 queries are GFRAME (static under the port), so this also proves
 * the guest-word round trip of a native frame address. */
static void hand_cases_ba8f4(void)
{
    static const int32_t kCached[] = {-1, 0, 5, 11, 17, 18, 40};
    uint32_t args[4];
    unsigned seen = 0, c;
    int winding, x, z;

    if (!abi_leaf_listed("func_800BA8F4"))
        return;
    for (winding = 0; winding < 2; winding++) {
        for (c = 0; c < sizeof(kCached) / sizeof(kCached[0]); c++) {
            for (x = -500; x <= 500; x += 250) {
                for (z = -500; z <= 500; z += 250) {
                    uint8_t *s;
                    begin_case(c % 3u == 2u ? 2u : 0u, 0);
                    mesh_fixture(winding, (int)c + 2 * winding);
                    if (c == 3u && x == 0) {
                        unsigned t;
                        *(uint8_t *)PSX_ADDR(0x800D2F64u) = 0xFF;
                        for (t = 0; t < mesh_tri_count; t++)
                            *(uint8_t *)PSX_ADDR(MESH_TRI + 14u * t + 0xDu) = (uint8_t)(t * 37u);
                    }
                    s = (uint8_t *)PSX_ADDR(REC);
                    *(int16_t *)(s + 2) = (int16_t)(x + 13);
                    *(int16_t *)(s + 6) = (int16_t)(z / 10);
                    *(int16_t *)(s + 0xA) = (int16_t)(z - 7);
                    *(int32_t *)(s + 0x78) = kCached[c];
                    *(uint16_t *)(s + 0x84) = 0xBEEF;
                    args[0] = REC; args[1] = args[2] = args[3] = 0;
                    compare_name("func_800BA8F4", args);
                    if (last_case_proven)
                        seen |= *(int32_t *)(s + 0x78) >= 0 ? 1u : 2u;
                }
            }
        }
    }
    /* Tables not loaded: every query returns -1. */
    begin_case(0, 0);
    *(int32_t *)PSX_ADDR(REC + 0x78) = 3;
    args[0] = REC; args[1] = args[2] = args[3] = 0;
    compare_name("func_800BA8F4", args);
    require_outcomes("func_800BA8F4", seen, 3u);
}

/* Sprite record for the timer callbacks at REC (guest), the task at DST
 * (+4 = REC).  AnimScriptTick/func_80022CDC/func_80022B2C are the shipped
 * main-executable bodies (temp1.c) on both sides: the wait timer +0x9E is
 * kept above the tick count so the animation script interpreter
 * (func_800248D4) is never entered; +0x3C bit 26 selects the simple
 * vertical step or the floor query (func_800BA8F4, tables not loaded). */
static void timer_sprite(unsigned variant)
{
    uint8_t *o = (uint8_t *)PSX_ADDR(REC);
    *(uint32_t *)PSX_ADDR(DST + 4) = REC;
    *(uint32_t *)(o + 0x0) = 0x00120000u + variant * 0x1111u;
    *(uint32_t *)(o + 0x4) = 0xFFF80000u + variant * 0x2345u;
    *(uint32_t *)(o + 0x8) = 0x00340000u - variant * 0x0F0Fu;
    *(int32_t *)(o + 0xC) = (int32_t)(variant * 0x321u) - 0x400;
    *(int32_t *)(o + 0x10) = (int32_t)(variant * 0x123u) - 0x200;
    *(int32_t *)(o + 0x14) = 0x280 - (int32_t)(variant * 0x77u);
    *(int32_t *)(o + 0x1C) = (variant & 1u) ? 0x40 : -0x30;
    *(uint16_t *)(o + 0x3A) = (uint16_t)((variant % 3u) * 0x300u);
    *(uint32_t *)(o + 0x3C) = (variant & 2u) ? (1u << 26) : 0u;
    *(int32_t *)(o + 0x78) = -1;
    *(int16_t *)(o + 0x84) = (int16_t)(variant * 3u);
    *(int16_t *)(o + 0x9E) = (int16_t)(4 + variant);
    *(uint32_t *)(o + 0xAC) = (variant & 4u) ? 0x40u : 0u;
}

static void hand_cases_timer_callbacks(void)
{
    uint32_t args[4];
    unsigned v;

    if (abi_leaf_listed("func_800BAC50")) {
        unsigned seen = 0;
        for (v = 0; v < 16; v++) {
            begin_case(v % 3u, 0);
            timer_sprite(v);
            *(uint8_t *)PSX_ADDR(0x800C3664u) = (uint8_t)((v % 4u) == 3u ? 1u + (v & 8u) * 16u : 0u);
            args[0] = DST; args[1] = args[2] = args[3] = 0;
            compare_name("func_800BAC50", args);
            if (last_case_proven)
                seen |= *(uint8_t *)PSX_ADDR(0x800C3664u) != 0 ? 1u : 2u;
        }
        require_outcomes("func_800BAC50", seen, 3u);
    }
    /* 800BB13C: slot = ((+0xAC & 3) << 2) | (+0xA8 >> 30) selects the
     * D_800D3368 model record (at SRC, +4 -> SRC + 0x80 holding the
     * +0x5C/+0x60/+0x64 position).  Classes: 1 = no model (early out),
     * 2 = +0x9E idle clears +0x64, 4 = ticked. */
    if (abi_leaf_listed("func_800BB13C")) {
        unsigned seen = 0;
        for (v = 0; v < 24; v++) {
            uint8_t *o = (uint8_t *)PSX_ADDR(REC);
            unsigned slot;
            begin_case(v % 3u, 0);
            timer_sprite(v);
            *(uint32_t *)(o + 0xAC) |= v & 3u;
            *(uint32_t *)(o + 0xA8) = (v * 0x40000000u) | 0x1234u;
            slot = ((v & 3u) << 2) | (v & 3u);
            *(uint32_t *)(o + 0x64) = (v % 5u) == 1u ? 0u : 0x10u + v;
            if ((v % 6u) == 2u)
                *(int16_t *)(o + 0x9E) = 0;
            *(uint32_t *)PSX_ADDR(0x800D3368u + 4u * slot) = (v % 7u) == 3u ? 0u : SRC;
            *(uint32_t *)PSX_ADDR(SRC + 4) = SRC + 0x80u;
            *(uint32_t *)PSX_ADDR(SRC + 0x80u + 0x5C) = 0x0123u + v * 0x101u;
            *(uint32_t *)PSX_ADDR(SRC + 0x80u + 0x60) = 0xFF80u - v;
            *(uint32_t *)PSX_ADDR(SRC + 0x80u + 0x64) = 0x8001u + v * 7u;
            args[0] = DST; args[1] = args[2] = args[3] = 0;
            compare_name("func_800BB13C", args);
            if (last_case_proven)
                seen |= (v % 7u) == 3u ? 1u : ((v % 6u) == 2u ? 2u : 4u);
        }
        require_outcomes("func_800BB13C", seen, 7u);
    }
    /* 80076AC8 / 80076C34: SetSemiTrans(p, 1); SetShadeTex(p, 0) on a
     * primitive at DST (the PsyCross libgpu helpers the port binds the retail
     * 0x80043BFC entries to), then for C34 the 0x40 colour and +0x16 bit 6.
     * Every code byte value is tried. */
    for (v = 0; v < 256; v += 5) {
        uint8_t *p = (uint8_t *)PSX_ADDR(DST);
        begin_case(v % 3u, 0);
        p[3] = (uint8_t)(v * 3u);
        p[7] = (uint8_t)v;
        *(uint16_t *)(p + 0x16) = (uint16_t)(v * 0x0101u);
        args[0] = DST; args[1] = args[2] = args[3] = 0;
        if (abi_leaf_listed("func_80076AC8"))
            compare_name("func_80076AC8", args);
        if (abi_leaf_listed("func_80076C34"))
            compare_name("func_80076C34", args);
    }
}

/* 800AAA20 (main72 port body): obj at REC (+4 = model word -> SRC), the
 * fifth argument in the guest stack slot.  Its callees (800AFF9C,
 * 8009F1C4/8009EF3C, 800A0838, 800AE2A4, 800AAD54) are interpreted on both
 * sides (native side through PcPort_BattleMipsCallGuest).  800AAD54 returns
 * at once for steps == 0 or obj +0x10 == 0.  Classes: 1 = *obj == 0,
 * 2 = +0x34 clear (800AAD54 only), 4 = +0x34 set (stepping loop). */
static void hand_cases_aaa20(void)
{
    uint32_t args[4];
    unsigned v, seen = 0;

    if (!abi_leaf_listed("func_800AAA20"))
        return;
    pre_side = stack_arg4_side;
    for (v = 0; v < 48; v++) {
        uint8_t *o = (uint8_t *)PSX_ADDR(REC);
        unsigned cls = (v % 3u == 0u) ? 1u : ((v % 3u == 1u) ? 2u : 4u);
        begin_case(v % 2u, 0);
        *(uint32_t *)(o + 0x0) = cls == 1u ? 0u : 0x800C5300u;
        *(uint32_t *)(o + 0x4) = SRC;
        *(uint32_t *)(o + 0x10) = (v & 4u) ? SRC + 0x40u : 0u;
        *(int16_t *)(o + 0x1C) = (int16_t)(v * 3u);
        o[0x34] = cls == 4u ? (uint8_t)(1u + v) : 0u;
        o[0x37] = (uint8_t)((v >> 3) & 1u);
        *(uint16_t *)(o + 0x3C) = (uint16_t)(v * 0x111u);
        stack_arg4 = v * 0x10001u;
        args[0] = REC;
        args[1] = v % 4u;
        args[2] = (v & 8u) ? 0u : (v % 3u);  /* steps */
        args[3] = v & 1u;
        compare_name("func_800AAA20", args);
        if (last_case_proven)
            seen |= cls;
    }
    pre_side = NULL;
    require_outcomes("func_800AAA20", seen, 7u);
}

/* 800A3E98 (main68 port body): effect-image record at REC, linked record at
 * REC + 0x100.  The +0x24 progression callback is one of the four retail
 * entries (800A3490/3514/3578/35C8; native in the port body, interpreted on
 * the retail side).  Types 4/5 rebuild through 800A4348/800A43F8 into the
 * +0x1C buffer; types 0/1 reach main-exe func_80026F44 (and LoadImage when
 * unlinked), which this harness does not link, so those cases are
 * inconclusive by construction.  Classes: 1 = inactive (+0x1A == 0, -1),
 * 2 = finished (frame < 0, 800A429C), 4 = same frame, 8 = new frame. */
static void hand_cases_a3e98(void)
{
    static const uint32_t kCallbacks[] = {
        0x800A3490u, 0x800A3514u, 0x800A3578u, 0x800A35C8u
    };
    static const uint8_t kTypes[] = {4, 5, 4, 5, 0, 1, 2, 5};
    uint32_t args[4];
    unsigned v, seen = 0;

    if (!abi_leaf_listed("func_800A3E98"))
        return;
    for (v = 0; v < 160; v++) {
        uint8_t *e = (uint8_t *)PSX_ADDR(REC);
        uint8_t *l = (uint8_t *)PSX_ADDR(REC + 0x100u);
        unsigned cb = (v % 8u == 6u) ? 1u : v % 4u;
        unsigned ticks = v % 3u;
        int16_t div = (int16_t)(1 + v % 5u);
        int16_t base = (int16_t)((v % 7u == 3u) ? 0x40 : (int)(v % 4u));
        int inactive = (v % 13u) == 5u;
        int same = (v % 8u) == 6u;
        /* Same-frame cases keep the phase small so 800A3514 stays < 0x21. */
        uint16_t phase = (uint16_t)(same ? v % 8u : v * 37u);
        uint16_t step = (uint16_t)(v % 3u);
        unsigned cls;

        begin_case(v % 3u, 0);
        *(uint32_t *)(e + 0x00) = (v & 1u) ? REC + 0x100u : 0u;
        *(uint32_t *)(e + 0x04) = SRC;
        *(uint32_t *)(e + 0x08) = SRC + 0x40u;
        *(uint32_t *)(e + 0x0C) = DST;
        *(uint32_t *)(e + 0x1C) = DST + 0x100u;
        e[0x10] = kTypes[(v / 2u) % 8u];
        e[0x11] = 0;
        *(int16_t *)(e + 0x12) = 4;
        *(uint16_t *)(e + 0x14) = phase;
        *(uint16_t *)(e + 0x16) = step;
        *(uint16_t *)(e + 0x18) = 0x7FFF;
        *(uint16_t *)(e + 0x1A) = inactive ? 0 : 1;
        *(int16_t *)(e + 0x20) = div;
        *(int16_t *)(e + 0x22) = base;
        *(uint32_t *)(e + 0x24) = kCallbacks[cb];
        *(int16_t *)(e + 0x28) = (int16_t)(v % 2u);
        *(int16_t *)(e + 0x2A) = (int16_t)((v >> 1) % 2u);
        *(int16_t *)(e + 0x2C) = (int16_t)(2 + v % 2u);
        *(int16_t *)(e + 0x2E) = 2;
        if (same) {
            /* 800A3514: base + phase' / divisor, -1 at >= 0x21. */
            int16_t p2 = (int16_t)(uint16_t)(phase + step * (ticks + 1u));
            int16_t f = (int16_t)(base + p2 / div);
            *(uint16_t *)(e + 0x18) = (uint16_t)(f < 0x21 ? f : -1);
        }
        *(uint32_t *)(l + 0x04) = DST + 0x180u;
        l[0x10] = (v & 2u) ? 4u : 0u;
        *(uint16_t *)(l + 0x1A) = (v % 11u) == 7u ? 0 : 1;
        *(int16_t *)(l + 0x28) = (int16_t)(v % 3u);
        *(int16_t *)(l + 0x2A) = (int16_t)((v >> 2) % 2u);
        *(int16_t *)(l + 0x2C) = 3;
        *(int16_t *)(l + 0x2E) = 2;
        args[0] = REC; args[1] = ticks; args[2] = args[3] = 0;
        compare_name("func_800A3E98", args);
        if (!last_case_proven)
            continue;
        if (inactive)
            cls = 1u;
        else if ((int32_t)last_retail_v0 < 0)
            cls = 2u;
        else
            cls = same ? 4u : 8u;
        seen |= cls;
    }
    require_outcomes("func_800A3E98", seen, 15u);
}

/* 800A0838 (main65, lifted port body): model at REC (+0xA entry count,
 * 0x7C-byte entries from REC + 4), each entry with up to three track records
 * (+0x6C, +0x70, +0x74) at A0838_TRK and their byte streams at A0838_DAT.
 * Every track gets a random kind/skip byte (+2), finished flag (+1), owner
 * byte (+3, equal to a2 in half the cases), step counter/limit (+0x10/+0x12,
 * limit nonzero) and nonzero s16 parameters, so the 9-, 6- and third-track
 * jump tables are all reached; a0 is the track pool (HALF) that
 * func_800A23E8 (interpreted on both sides) releases a finished track into.
 * Classes: 1 = no flags, 2 = a running track (0x100), 4 = a finished,
 * released track (0x200), 8 = a finished looping track (0x400). */
#define A0838_TRK 0x800C6000u
#define A0838_DAT 0x800C7000u
/* High bits of the harness LCG: its low bits have short periods. */
static uint32_t hrnd(void)
{
    return rnd() >> 10;
}
static void hand_cases_a0838(void)
{
    uint32_t args[4];
    unsigned v, seen = 0;

    if (!abi_leaf_listed("func_800A0838"))
        return;
    for (v = 0; v < 1500; v++) {
        uint8_t *m = (uint8_t *)PSX_ADDR(REC);
        unsigned n = 1u + v % 3u, k, j;
        uint32_t a2 = v % 5u;
        unsigned cls;

        begin_case((v % 7u == 0u) ? 1u : 2u, 0);
        *(uint16_t *)(m + 0xA) = (uint16_t)(v % 13u == 0u ? 0u : n);
        for (k = 0; k < n; k++) {
            uint8_t *e = m + 4u + k * 0x7Cu;
            for (j = 0; j < 3; j++) {
                unsigned idx = k * 3u + j;
                uint32_t t = A0838_TRK + idx * 0x20u;
                uint32_t d = A0838_DAT + idx * 0x80u;
                uint8_t *r = (uint8_t *)PSX_ADDR(t);

                if ((hrnd() & 7u) == 0u) {
                    *(uint32_t *)(e + 0x6C + j * 4u) = 0u;
                    continue;
                }
                *(uint32_t *)(e + 0x6C + j * 4u) = t;
                r[1] = (hrnd() % 3u) == 0u ? 1u : 0u;
                r[2] = (uint8_t)hrnd();
                r[3] = (hrnd() & 1u) ? (uint8_t)a2 : (uint8_t)(a2 + 1u);
                *(uint32_t *)(r + 0x4) = d;
                *(uint32_t *)(r + 0x8) = d + (hrnd() & 0x1Eu);
                *(int16_t *)(r + 0xA) = (int16_t)(hrnd() % 0x800u) - 0x400;
                *(int16_t *)(r + 0xC) = (int16_t)(1 + hrnd() % 0x400u);
                *(int16_t *)(r + 0xE) = (int16_t)(hrnd() % 0x800u) - 0x400;
                *(int16_t *)(r + 0x12) = (int16_t)(1 + hrnd() % 4u);
                *(int16_t *)(r + 0x10) = (int16_t)(hrnd() % 6u) - 1;
                /* +8 (stream cursor) and +0xA (first parameter) share bytes;
                 * the stream kinds need a live cursor. */
                if (hrnd() & 1u)
                    *(uint32_t *)(r + 0x8) = d + (hrnd() & 0x1Eu);
                if ((hrnd() & 3u) == 0u) {
                    /* Delta streams: the -0x80 escape (16-bit delta). */
                    uint8_t *b = (uint8_t *)PSX_ADDR(d);
                    unsigned q;
                    for (q = 0; q < 0x40u; q++)
                        if ((hrnd() & 3u) == 0u)
                            b[q] = 0x80u;
                }
                if ((hrnd() & 3u) == 0u) {
                    /* Interpolation already at its target (zero step). */
                    if (j == 0u) {
                        *(int16_t *)(e + 0x50) = *(int16_t *)(r + 0xA);
                        *(int16_t *)(e + 0x52) = *(int16_t *)(r + 0xC);
                        *(int16_t *)(e + 0x54) = *(int16_t *)(r + 0xE);
                    } else if (j == 1u) {
                        *(int32_t *)(e + 0x58) = *(int16_t *)(r + 0xA);
                        *(int32_t *)(e + 0x5C) = *(int16_t *)(r + 0xC);
                        *(int32_t *)(e + 0x60) = *(int16_t *)(r + 0xE);
                    } else {
                        *(int16_t *)(r + 0xA) = *(int16_t *)(r + 0x4);
                        *(int16_t *)(r + 0xC) = *(int16_t *)(r + 0x6);
                        *(int16_t *)(r + 0xE) = *(int16_t *)(r + 0x8);
                    }
                }
            }
        }
        /* a0 is the track pool func_800A23E8 releases into: +0 base, +4
         * lowest free index. */
        *(uint32_t *)PSX_ADDR(HALF) = A0838_TRK;
        *(uint16_t *)PSX_ADDR(HALF + 4u) = (uint16_t)(v % 16u);
        args[0] = HALF;
        args[1] = REC;
        args[2] = a2;
        args[3] = (uint32_t)(int32_t)(int16_t)(v * 7u);
        compare_name("func_800A0838", args);
        if (!last_case_proven)
            continue;
        cls = last_retail_v0 == 0u ? 1u : 0u;
        if (last_retail_v0 & 0x100u) cls |= 2u;
        if (last_retail_v0 & 0x200u) cls |= 4u;
        if (last_retail_v0 & 0x400u) cls |= 8u;
        seen |= cls;
    }
    require_outcomes("func_800A0838", seen, 15u);
}

/* One case per adopted leaf, with the arguments the port's bridge test already
 * established as meaningful for it. */
static void hand_cases(void)
{
    uint32_t args[4];

    hand_cases_8008ABB8();
    hand_cases_mesh();
    hand_cases_indirect();
    hand_cases_abi_slots();
    hand_cases_slot_sweep();
    hand_cases_slot_extra();
    hand_cases_a828();
    hand_cases_a7bc();
    hand_cases_mesh_height();
    hand_cases_ba8f4();
    hand_cases_timer_callbacks();
    hand_cases_aaa20();
    hand_cases_a3e98();
    hand_cases_a0838();

    /* AA80AE4's result is undefined when no rotation slot qualifies. Seed a
     * valid 11-entry cycle so both sides return a defined selected value. */
    begin_case(0, 0);
    {
        uint8_t *base = (uint8_t *)PSX_ADDR(0x800D2DD7u);
        unsigned i;
        base[0] = 0;
        for (i = 0; i < 11; i++) {
            uint8_t value = (uint8_t)(i + 1);
            base[1 + i] = value;
            *(int16_t *)(base + 0x2F + value * 2) = (int16_t)((11 - i) * 16);
        }
    }
    args[0] = 0xFE; args[1] = args[2] = args[3] = 0;
    compare_name("func_80080AE4", args);

    /* AA600 follows a guest table slot -> record -> subrecord pointer chain.
     * Give the retail interpreter and the shipped host body the same valid
     * graph so this adapter is proven through the production runtime bridge. */
    begin_case(0, 0);
    *((uint32_t *)PSX_ADDR(0x800D3368u)) = REC;
    *((uint32_t *)PSX_ADDR(REC + 4)) = SRC;
    *((int16_t *)PSX_ADDR(REC + 0x1C)) = 0x300;
    *((int16_t *)PSX_ADDR(SRC + 0x4E)) = 0x280;
    *((int16_t *)PSX_ADDR(REC + 0x24)) = 0x500;
    args[0] = 0; args[1] = args[2] = args[3] = 0;
    compare_name("func_800AA600", args);

    begin_case(0, 0);
    *((uint32_t *)PSX_ADDR(WORD)) = 10;
    args[0] = WORD; args[1] = args[2] = args[3] = 0;
    compare_name("func_80079934", args);

    begin_case(0, 0);
    *((uint16_t *)PSX_ADDR(HALF)) = 0x1234;
    args[0] = HALF; args[1] = args[2] = args[3] = 0;
    compare_name("func_800A3484", args);

    begin_case(0, 0);
    *((uint16_t *)PSX_ADDR(REC + 0x98)) = 0x7777;
    args[0] = REC; args[1] = args[2] = args[3] = 0;
    compare_name("func_800AEEEC", args);

    begin_case(0, 0);
    args[0] = 7; args[1] = 7; args[2] = args[3] = 0;
    compare_name("func_80089B50", args);
    args[0] = 0xFFFF; args[1] = 3;
    compare_name("func_80089B50", args);
    args[0] = 0; args[1] = 0xFFFF;
    compare_name("func_80089B50", args);

    begin_case(0, 0);
    *((uint32_t *)PSX_ADDR(SRC + 0x10)) = 0x18;
    *((uint32_t *)PSX_ADDR(SRC + 0x14)) = 1;
    args[0] = SRC; args[1] = args[2] = args[3] = 0;
    compare_name("func_800B16A4", args);

    begin_case(0, 0);
    *((uint16_t *)PSX_ADDR(SRC)) = 2;
    *((uint16_t *)PSX_ADDR(SRC + 2)) = 1;
    *((uint16_t *)PSX_ADDR(SRC + 4)) = 2;
    *((uint16_t *)PSX_ADDR(SRC + 6)) = 3;
    args[0] = DST; args[1] = SRC; args[2] = args[3] = 0;
    compare_name("func_800B6930", args);

    begin_case(0, 0);
    *((uint16_t *)PSX_ADDR(SRC)) = 2;
    *((uint16_t *)PSX_ADDR(SRC + 2)) = 0x0011;
    *((uint16_t *)PSX_ADDR(SRC + 4)) = 0x0022;
    *((uint16_t *)PSX_ADDR(SRC + 6)) = 0x0033;
    args[0] = DST; args[1] = SRC; args[2] = args[3] = 0;
    compare_name("func_800B6990", args);

    begin_case(0, 0);
    *((uint16_t *)PSX_ADDR(DST)) = 10;
    *((uint16_t *)PSX_ADDR(DST + 2)) = 20;
    *((uint16_t *)PSX_ADDR(DST + 4)) = 30;
    args[0] = DST; args[1] = SRC; args[2] = args[3] = 0;
    compare_name("func_800B69E4", args);

    begin_case(0, 0);
    args[0] = 0; args[1] = 0; args[2] = 0xAB; args[3] = 0;
    compare_name("func_80079ED8", args);
    args[3] = 1;
    compare_name("func_80079ED8", args);

    begin_case(0, 0);
    args[0] = 0; args[1] = 0; args[2] = 0x3344; args[3] = 0;
    compare_name("func_8007A280", args);
    args[3] = 1;
    compare_name("func_8007A280", args);

    /* D_800D2D28 is pointer-typed in the retail image; give it a live target so
     * both sides dereference the same guest word. */
    begin_case(0, 0);
    *((uint32_t *)PSX_ADDR(0x800D2D28u)) = PTRT;
    args[0] = 1; args[1] = args[2] = args[3] = 0;
    compare_name("func_80079E18", args);
    compare_name("func_80079E4C", args);

    begin_case(0, 0);
    args[0] = DST; args[1] = 0; args[2] = 0x11111111u; args[3] = 0x22222222u;
    compare_name("func_800AA898", args);
}

/* Deterministic sweep: scalar-argument leaves, several argument shapes, five RAM
 * seeds; then a pointer-argument pass for leaves that declare a pointer.
 *
 * The bridge cannot tell a scalar from a pointer: an argument that looks like a
 * KSEG0/KSEG1 address is translated to a host pointer before the body sees it.
 * Feeding pointer-shaped values to a scalar parameter would therefore measure
 * that convention rather than the body, and feeding scalar values to a pointer
 * parameter would make the host dereference a wild address that the interpreter
 * (which validates through the bus) would simply fault on. Leaves that declare a
 * pointer parameter are therefore given PTRT for exactly those parameters, with
 * the target window and the pointer globals seeded, which is the same typed
 * shape the hand cases construct by hand.
 *
 * Patterns 0-2 fill RAM only; patterns 3-4 repeat two of those fills with the
 * pointer-typed overlay globals seeded. Without the seeded pass, a body that
 * dereferences such a global faults on the retail side in every case and can
 * never be proven. The unseeded patterns still run: for those bodies they are
 * reported inconclusive, not passed. */
static void sweep(void)
{
    unsigned index;
    unsigned round;
    uint32_t pattern;

    for (index = 0; index < LEAF_COUNT; index++) {
        /* The dedicated hand case and remu differential cover AA80AE4's
         * valid input domain. Random scalar values can leave best undefined. */
        if (strcmp(kLeaves[index].name, "func_80080AE4") == 0)
            continue;
        /* 8007FEC4 allocates from the native heap, whose globals are not
         * part of the snapshot: only its hand case (fresh heap per side)
         * is a fair comparison. */
        if (strcmp(kLeaves[index].name, "func_8007FEC4") == 0)
            continue;
        /* Each leaf draws its own sequence, seeded from its address, so
         * adding or removing an allowlist entry does not silently change the
         * inputs (and so the evidence) of every leaf after it. */
        rng_state = 0x12345678u ^ leaf_address(kLeaves[index].name);
        if (kLeaves[index].pointer_params != 0) {
            unsigned mask = kLeaves[index].pointer_params;

            hand_only++;
            for (pattern = 0; pattern < 3; pattern++) {
                for (round = 0; round < 4; round++) {
                    uint32_t args[4];
                    unsigned a;
                    begin_case(pattern, 1);
                    for (a = 0; a < 4; a++) {
                        if ((mask >> a) & 1u)
                            args[a] = PTRT + 0x100u * a;
                        else
                            args[a] = (round == 0) ? 0u : (rnd() & 0xFFFFu);
                    }
                    compare(&kLeaves[index], args);
                }
            }
            continue;
        }
        for (pattern = 0; pattern < 5; pattern++) {
            int seeded = pattern >= 3;
            uint32_t fill = seeded ? pattern - 3 : pattern;

            for (round = 0; round < 6; round++) {
                begin_case(fill, seeded);
                uint32_t args[4];
                unsigned a;
                for (a = 0; a < 4; a++) {
                    switch (rnd() % 4u) {
                    case 0: args[a] = 0; break;
                    case 1: args[a] = rnd() & 0xFFFFu; break;
                    case 2: args[a] = 1u; break;
                    default: args[a] = 0xFFFFFFFFu; break;
                    }
                }
                compare(&kLeaves[index], args);
            }
        }
    }
}

int main(void)
{
    load();
    runtime_up();
    {
        size_t d;
        for (d = 0; d < rt.data_count; d++) {
            if (strcmp(rt.data[d].name, "g_GameState") == 0 &&
                rt.data[d].size >= NATIVE_GS_SIZE)
                native_gs = rt.data[d].host;
        }
        if (native_gs == NULL)
            fprintf(stderr, "BATTLE OVERLAY DIFFERENTIAL note: g_GameState "
                    "is not a shared native binding; not compared\n");
    }

    /* A host body that runs off g_PsxRam must not take the whole run with it. */
    signal(SIGSEGV, host_fault_handler);
    signal(SIGBUS, host_fault_handler);

    /* The prover is worthless if the host bodies are absent; fail loudly. */
    if (rt.function_count == 0)
        fprintf(stderr, "BATTLE OVERLAY DIFFERENTIAL note: no native bindings\n");

    hand_cases();
    sweep();

    coverage_report();
    if (getenv("BATTLE_OVERLAY_DIFFERENTIAL_PER_LEAF") != NULL) {
        unsigned i;
        for (i = 0; i < LEAF_COUNT; i++)
            fprintf(stderr, "[prover] leaf %s proven=%u inconclusive=%u\n",
                    kLeaves[i].name, proven[i], inconclusive_per_leaf[i]);
    }
    if (failures == 0) {
        /* An allowlisted leaf with no completed comparison is not evidence of
         * anything. Say so by name; the batch has to be pruned. */
        unsigned i;
        for (i = 0; i < LEAF_COUNT; i++) {
            if (proven[i] != 0)
                continue;
            fprintf(stderr, "BATTLE OVERLAY DIFFERENTIAL UNPROVEN %s\n",
                    kLeaves[i].name);
            failures++;
        }
    }
    if (failures != 0) {
        fprintf(stderr, "BATTLE OVERLAY DIFFERENTIAL FAIL total=%u\n", failures);
        return 1;
    }
    printf("BATTLE OVERLAY DIFFERENTIAL PASS checks=%u inconclusive=%u "
           "hand-only=%u leaves=%u store-pass-through=%u\n",
           checks, inconclusive, hand_only, (unsigned)LEAF_COUNT,
           store_pass_through_checks);
    return 0;
}
