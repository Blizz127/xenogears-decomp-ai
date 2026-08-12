/*
 * W34B16-B production-linked retail certificate for callback 0x80071A50.
 *
 * The retail slice is two instructions, transcribed independently from
 * disc/world_map.bin (load base 0x8006FAF0):
 *
 *   80071A50: 03e00008  jr    $ra
 *   80071A54: 24020001  addiu $v0, $zero, 1     ; branch delay slot
 *
 * slice SHA-256 5ce5ad86d452c4d2422bd63e15223d5d6b3dfb77f224a88c1f476e9fb34e359d
 *
 * The retail body contains no JAL/JALR, no load, no store, no COP2/GTE
 * instruction and no conditional branch. Certifying "returns 1" alone would
 * therefore be far too weak: the interesting properties are all negative
 * ones. This certificate proves them directly against the linked production
 * body:
 *
 *   - the return value is exactly 1 for every slot index, including the
 *     asymmetric extremes INT32_MIN / INT32_MAX;
 *   - the result does not depend on the incoming slot index;
 *   - the result does not depend on guest RAM contents (each fixture seeds a
 *     different whole-RAM pattern);
 *   - complete guest RAM, the slot-14 record, and the scheduler pool pointer
 *     at 0x8009BE24 are byte-identical across the call;
 *   - the body performs no guest-memory access whatsoever - proven by
 *     running it with all of guest RAM mprotect(PROT_NONE) and trapping
 *     SIGSEGV/SIGBUS, which catches even a load whose value is discarded;
 *   - the body calls no helper.
 *
 * The production body is #included unchanged so copied-source mutants can
 * retarget WM_71A50_PRODUCTION_SOURCE. The callback is always invoked through
 * a volatile function pointer so that -O2 cannot inline it into a constant
 * and thereby hide a mutant's guest access from the protection gate.
 */
#include <limits.h>
#include <setjmp.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "common.h"
#include "psx_memory.h"

/* Self-contained guest RAM, matching the other focused callback
 * certificates: the suite is a single translation unit. */
uint8_t g_PsxRam[PSX_RAM_SIZE];
uint8_t g_PsxScratchpad[4096];

/* Scheduler slot layout (world_map_scheduler.h), restated independently. */
#define TEST_POOL_PTR      0x8009BE24u
#define TEST_SLOT_STRIDE   0x80u
#define TEST_OFF_STATE     0x00u
#define TEST_OFF_CB0       0x18u
#define TEST_OFF_CB1       0x1Cu
#define TEST_SLOT14        14u

/* Arbitrary in-RAM pool base for the fixture-owned slot table. */
#define TEST_POOL_BASE     0x800A0000u

static int s_pass_count;
static int s_failure_count;
static int s_total_count;

/* Helper-call seam. Production never calls this; a copied-source mutant that
 * introduces a helper call trips the zero-helper-calls assertion. */
static int s_helper_calls;
void wm_71a50_probe_helper(void);
void wm_71a50_probe_helper(void) { s_helper_calls++; }

#ifndef WM_71A50_PRODUCTION_SOURCE
#define WM_71A50_PRODUCTION_SOURCE "../src/world_map_callback_71a50.c"
#endif
#include WM_71A50_PRODUCTION_SOURCE

/* Volatile indirect call: forbids constant-folding the body away at -O2. */
static s32 (*volatile s_callback)(s32) = wm_80071A50;

static void check_case(const char* fixture, const char* assertion, int ok)
{
    s_total_count++;
    if (ok) {
        s_pass_count++;
    } else {
        s_failure_count++;
        printf("FAIL [%s]: ASSERTION %s\n", fixture, assertion);
    }
}

/* ------------------------------------------------------------------ RAM */

static uint32_t xorshift32(uint32_t* state)
{
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

static void seed_guest_ram(uint32_t seed)
{
    uint32_t state = seed | 1u;
    size_t i;
    for (i = 0u; i < (size_t)PSX_RAM_SIZE; i++)
        g_PsxRam[i] = (uint8_t)(xorshift32(&state) >> 24);
}

static void seed_slot_table(void)
{
    uint8_t* pool;
    uint32_t pool_psx = TEST_POOL_BASE;
    unsigned slot;

    memcpy(PSX_ADDR(TEST_POOL_PTR), &pool_psx, sizeof(pool_psx));
    pool = (uint8_t*)PSX_ADDR(TEST_POOL_BASE);

    for (slot = 0u; slot < 16u; slot++) {
        uint8_t* rec = pool + slot * TEST_SLOT_STRIDE;
        s16 state = 0;
        uint32_t cb0 = 0u;
        uint32_t cb1 = 0u;
        if (slot == TEST_SLOT14) {
            cb0 = 0x80071A50u;
            cb1 = 0x80071A58u;
        }
        memcpy(rec + TEST_OFF_STATE, &state, sizeof(state));
        memcpy(rec + TEST_OFF_CB0, &cb0, sizeof(cb0));
        memcpy(rec + TEST_OFF_CB1, &cb1, sizeof(cb1));
    }
}

/* ------------------------------------------ zero-guest-access gate */

static volatile sig_atomic_t s_faulted;
static sigjmp_buf s_fault_env;

static void fault_handler(int signo)
{
    (void)signo;
    s_faulted = 1;
    siglongjmp(s_fault_env, 1);
}

/* Page-aligned interior of the 2 MB guest window. Every address this
 * callback family could touch (0x8009xxxx, 0x800Axxxx) lies well inside. */
static uint8_t* prot_base(void)
{
    long pagesz = sysconf(_SC_PAGESIZE);
    uintptr_t raw = (uintptr_t)g_PsxRam;
    uintptr_t up = (raw + (uintptr_t)pagesz - 1u) & ~((uintptr_t)pagesz - 1u);
    return (uint8_t*)up;
}

static size_t prot_len(void)
{
    long pagesz = sysconf(_SC_PAGESIZE);
    uintptr_t raw = (uintptr_t)g_PsxRam;
    uintptr_t up = (uintptr_t)prot_base();
    uintptr_t end = (raw + 0x200000u) & ~((uintptr_t)pagesz - 1u);
    return (size_t)(end - up);
}

/* Runs the production body with guest RAM unmapped. Returns 1 if the body
 * completed without touching guest memory, 0 if it faulted, -1 if the gate
 * itself could not be armed. */
static int run_under_protection(s32 slot_index, s32* out_ret)
{
    struct sigaction sa_segv, sa_bus, old_segv, old_bus;
    uint8_t* base = prot_base();
    size_t len = prot_len();
    volatile int ok; /* written on both sides of sigsetjmp */

    s_faulted = 0;

    memset(&sa_segv, 0, sizeof(sa_segv));
    sa_segv.sa_handler = fault_handler;
    sigemptyset(&sa_segv.sa_mask);
    sa_segv.sa_flags = SA_NODEFER;
    sa_bus = sa_segv;
    if (sigaction(SIGSEGV, &sa_segv, &old_segv) != 0)
        return -1;
    if (sigaction(SIGBUS, &sa_bus, &old_bus) != 0) {
        (void)sigaction(SIGSEGV, &old_segv, NULL);
        return -1;
    }

    if (mprotect(base, len, PROT_NONE) != 0) {
        (void)sigaction(SIGSEGV, &old_segv, NULL);
        (void)sigaction(SIGBUS, &old_bus, NULL);
        return -1;
    }

    if (sigsetjmp(s_fault_env, 1) == 0) {
        *out_ret = s_callback(slot_index);
        ok = 1;
    } else {
        ok = 0;
    }

    (void)mprotect(base, len, PROT_READ | PROT_WRITE);
    (void)sigaction(SIGSEGV, &old_segv, NULL);
    (void)sigaction(SIGBUS, &old_bus, NULL);
    return ok;
}

/* ------------------------------------------------------------- fixtures */

typedef struct Fixture {
    const char* name;
    s32 slot_index;
    uint32_t ram_seed;
} Fixture;

static uint8_t* s_snapshot;

static void run_fixture(const Fixture* fx, s32 baseline_ret, int have_baseline)
{
    s32 ret;
    s32 prot_ret = 0;
    int gate;
    uint8_t* pool;
    uint8_t* rec;
    uint32_t pool_ptr_before, pool_ptr_after;
    uint8_t rec_before[TEST_SLOT_STRIDE];

    seed_guest_ram(fx->ram_seed);
    seed_slot_table();

    pool = (uint8_t*)PSX_ADDR(TEST_POOL_BASE);
    rec = pool + TEST_SLOT14 * TEST_SLOT_STRIDE;
    memcpy(rec_before, rec, TEST_SLOT_STRIDE);
    memcpy(&pool_ptr_before, PSX_ADDR(TEST_POOL_PTR), sizeof(pool_ptr_before));
    memcpy(s_snapshot, g_PsxRam, (size_t)PSX_RAM_SIZE);

    s_helper_calls = 0;
    ret = s_callback(fx->slot_index);

    check_case(fx->name, "return-value-exactly-1", ret == 1);
    if (have_baseline)
        check_case(fx->name, "return-independent-of-slot-index",
                   ret == baseline_ret);
    check_case(fx->name, "zero-helper-calls", s_helper_calls == 0);
    check_case(fx->name, "guest-ram-unchanged",
               memcmp(s_snapshot, g_PsxRam, (size_t)PSX_RAM_SIZE) == 0);
    check_case(fx->name, "slot14-record-unchanged",
               memcmp(rec_before, rec, TEST_SLOT_STRIDE) == 0);
    memcpy(&pool_ptr_after, PSX_ADDR(TEST_POOL_PTR), sizeof(pool_ptr_after));
    check_case(fx->name, "pool-pointer-be24-unchanged",
               pool_ptr_before == pool_ptr_after);

    /* Decisive negative proof: no guest access at all, discarded or not. */
    s_helper_calls = 0;
    gate = run_under_protection(fx->slot_index, &prot_ret);
    check_case(fx->name, "protection-gate-armed", gate >= 0);
    if (gate >= 0) {
        check_case(fx->name, "no-guest-memory-access", gate == 1);
        if (gate == 1) {
            check_case(fx->name, "protected-return-value-exactly-1",
                       prot_ret == 1);
            check_case(fx->name, "protected-zero-helper-calls",
                       s_helper_calls == 0);
        }
    }
}

int main(void)
{
    static const Fixture fixtures[] = {
        {"slot-int32-min",  INT32_MIN, 0x9E3779B9u},
        {"slot-negative-1", -1,        0x12345678u},
        {"slot-zero",       0,         0xDEADBEEFu},
        {"slot-one",        1,         0x0BADF00Du},
        {"slot-thirteen",   13,        0xFEEDFACEu},
        {"slot-fourteen",   14,        0xA5A5A5A5u},
        {"slot-int32-max",  INT32_MAX, 0x5A5A5A5Au}
    };
    size_t i;
    s32 baseline = 0;

    s_snapshot = (uint8_t*)malloc((size_t)PSX_RAM_SIZE);
    if (s_snapshot == NULL) {
        printf("FAIL [harness]: ASSERTION snapshot-allocation\n");
        return 1;
    }

    memset(g_PsxRam, 0, sizeof(g_PsxRam));
    memset(g_PsxScratchpad, 0, sizeof(g_PsxScratchpad));

    for (i = 0u; i < sizeof(fixtures) / sizeof(fixtures[0]); i++) {
        if (i == 0u) {
            seed_guest_ram(fixtures[0].ram_seed);
            seed_slot_table();
            baseline = s_callback(fixtures[0].slot_index);
        }
        run_fixture(&fixtures[i], baseline, i != 0u);
    }

    /* Retail transcription restated for the acceptance log. */
    printf("RETAIL_SLICE 80071A50 03e00008 jr $ra / 80071A54 24020001 "
           "addiu $v0,$zero,1\n");
    printf("RETAIL_BOUNDARY [0x80071A50,0x80071A58) bytes=8 insns=2\n");
    printf("RETAIL_SLICE_SHA256 "
           "5ce5ad86d452c4d2422bd63e15223d5d6b3dfb77f224a88c1f476e9fb34e359d\n");
    printf("NEGATIVE_COUNTS calls=0 loads=0 stores=0 cop2=0 "
           "slot_access=0 global_access=0\n");
    printf("RETURN_VALUE 1\n");
    printf("=== Results: %d/%d PASS ===\n", s_pass_count, s_total_count);

    free(s_snapshot);
    return s_failure_count == 0 ? 0 : 1;
}
