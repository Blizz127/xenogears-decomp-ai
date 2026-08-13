/*
 * W34B17-B production-linked certificate for world callback 0x80087710.
 *
 * The callback object is linked from pc_port/src/world_map_callback_87710.c;
 * this file is an independent oracle and event-order witness.  The oracle
 * does not call the production function and does not reuse its address/offset
 * macros.  The test also links the real bounded scheduler and exercises the
 * explicit resolver, the natural 92DF8 -> 71A50 -> 87710 chain, first-pass
 * completion, and the second-pass 925A0 frontier.
 *
 * Focused build (from the repository root):
 *
 *   gcc -std=gnu17 -O0 -g -Wall -Wextra -Wconversion -Wsign-conversion -Werror \
 *     -DXENO_PC_PORT -DWM_87710_TEST_TRACE -fno-pie -no-pie \
 *     -Ipc_port/include_shim -Iinclude -Ipc_port/src \
 *     pc_port/tests/w34b17b_80087710_prod_test.c \
 *     pc_port/src/world_map_scheduler.c \
 *     pc_port/src/world_map_callback_87710.c \
 *     pc_port/src/world_map_callback_92df8.c \
 *     pc_port/src/world_map_callback_71a50.c \
 *     -o pc_port/build_native/w34b17b_80087710_prod_test
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_71a50.h"
#include "world_map_callback_87710.h"
#include "world_map_callback_92df8.h"
#include "world_map_scheduler.h"

uint8_t g_PsxRam[PSX_RAM_SIZE];
uint8_t g_PsxScratchpad[4096];

enum {
    TEST_POOL_PTR       = 0x8009BE24u,
    TEST_SLOT_STRIDE    = 0x80u,
    TEST_FIELD_50       = 0x50u,
    TEST_FIELD_54       = 0x54u,
    TEST_SLOT_STATE     = 0x00u,
    TEST_SLOT_CB0       = 0x18u,
    TEST_SLOT_CB1       = 0x1Cu,
    TEST_POOL_A         = 0x80040000u,
    TEST_POOL_B         = 0x800A0000u,
    TEST_POOL_C         = 0x80130000u,
    TEST_CB_925A0       = 0x800925A0u,
    TEST_CB_87734       = 0x80087734u,
    TEST_CB_877E0       = 0x800877E0u,
    TEST_CB_87804       = 0x80087804u,
    TEST_CB_92DF8       = 0x80092DF8u,
    TEST_CB_92FD8       = 0x80092FD8u,
    TEST_CB_71A50       = 0x80071A50u,
    TEST_CB_71A58       = 0x80071A58u,
    TEST_TRACE_CAPACITY = 16u
};

typedef struct TraceEvent {
    u32 pc;
    u32 kind;
    u32 address;
    u32 width;
    u32 value;
} TraceEvent;

typedef struct Fixture {
    const char* name;
    u32 pool;
    s32 slot;
    u32 seed;
} Fixture;

static TraceEvent s_trace[TEST_TRACE_CAPACITY];
static size_t s_trace_count;
static int s_alias_calls;
static int s_total;
static int s_pass;
static int s_fail;

static bool s_natural_mode;
static int s_natural_chain_count;
static u32 s_natural_chain[3];
static int s_natural_87710_entries;
static s32 s_natural_entry_state = -1;
static s32 s_natural_callback_state = -1;
static u32 s_natural_pool;

/* Defined only for the copied scheduler mutant M12.  The production link
 * never exports or references this different retail symbol. */
s32 wm_test_alias_877e0(s32 slot_index);
#if defined(WM_87710_MUTANT_M12)
s32 wm_800877E0(s32 slot_index)
{
    return wm_test_alias_877e0(slot_index);
}
#endif

static size_t ram_index(u32 address)
{
    return (size_t)(address & 0x001FFFFFu);
}

static void* raw_address(u32 address)
{
    return (void*)(g_PsxRam + ram_index(address));
}

static u32 raw_load_u32(u32 address)
{
    u32 value;
    memcpy(&value, raw_address(address), sizeof(value));
    return value;
}

static s16 raw_load_s16(u32 address)
{
    s16 value;
    memcpy(&value, raw_address(address), sizeof(value));
    return value;
}

static void raw_store_u16(u32 address, u16 value)
{
    memcpy(raw_address(address), &value, sizeof(value));
}

static void raw_store_u32(u32 address, u32 value)
{
    memcpy(raw_address(address), &value, sizeof(value));
}

/* This is the only observer used by the production callback.  It receives a
 * typed event after each actual volatile guest access, so the oracle can
 * distinguish one u32 load from the two ordered u32 stores. */
void wm_87710_test_trace(u32 pc, u32 kind, u32 address,
                         u32 width, u32 value)
{
    if (s_trace_count < TEST_TRACE_CAPACITY) {
        s_trace[s_trace_count].pc = pc;
        s_trace[s_trace_count].kind = kind;
        s_trace[s_trace_count].address = address;
        s_trace[s_trace_count].width = width;
        s_trace[s_trace_count].value = value;
    }
    s_trace_count++;

    if (s_natural_mode && pc == 0x80087718u) {
        u32 slot = s_natural_pool + (15u * TEST_SLOT_STRIDE);
        s_natural_87710_entries++;
        s_natural_entry_state = (s32)raw_load_s16(slot + TEST_SLOT_STATE);
    }
    if (s_natural_mode && pc == 0x80087728u) {
        u32 slot = s_natural_pool + (15u * TEST_SLOT_STRIDE);
        s_natural_callback_state = (s32)raw_load_s16(slot + TEST_SLOT_STATE);
        if (s_natural_chain_count < 3) {
            s_natural_chain[s_natural_chain_count] = 0x80087710u;
            s_natural_chain_count++;
        }
    }
}

static u32 xorshift32(u32* state)
{
    u32 x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

static void seed_ram(u32 seed)
{
    u32 state = seed | 1u;
    size_t i;
    for (i = 0u; i < (size_t)PSX_RAM_SIZE; i++)
        g_PsxRam[i] = (u8)(xorshift32(&state) >> 24);
}

static void check_case(const char* case_name, const char* assertion,
                       bool condition)
{
    s_total++;
    if (condition) {
        s_pass++;
    } else {
        s_fail++;
        printf("FAIL [%s]: ASSERTION %s\n", case_name, assertion);
    }
}

static void trace_reset(void)
{
    memset(s_trace, 0, sizeof(s_trace));
    s_trace_count = 0u;
}

static u32 expected_slot(u32 pool, s32 slot_index)
{
    return pool + ((u32)slot_index << 7);
}

/* Independent retail oracle.  It uses separate constants and raw memcpy
 * accessors, and never calls a production function or observer. */
static void oracle_run(u8* expected, u32 pool, s32 slot_index)
{
    u32 base;
    u32 slot;

    memcpy(&base, expected + ram_index(0x8009BE24u), sizeof(base));
    slot = base + ((u32)slot_index << 7);
    {
        u32 zero = 0u;
        u32 eight = 8u;
        memcpy(expected + ram_index(slot + 0x50u), &zero, sizeof(zero));
        memcpy(expected + ram_index(slot + 0x54u), &eight, sizeof(eight));
    }
    (void)pool;
}

static bool trace_matches(u32 pool, s32 slot_index)
{
    u32 slot = expected_slot(pool, slot_index);
    const TraceEvent expected[3] = {
        {0x80087718u, WM_87710_TRACE_LW, TEST_POOL_PTR, 4u, pool},
        {0x80087724u, WM_87710_TRACE_SW, slot + 0x50u, 4u, 0u},
        {0x80087728u, WM_87710_TRACE_SW, slot + 0x54u, 4u, 8u}
    };
    size_t i;

    if (s_trace_count != 3u)
        return false;
    for (i = 0u; i < 3u; i++) {
        if (s_trace[i].pc != expected[i].pc ||
            s_trace[i].kind != expected[i].kind ||
            s_trace[i].address != expected[i].address ||
            s_trace[i].width != expected[i].width ||
            s_trace[i].value != expected[i].value)
            return false;
    }
    return true;
}

static bool trace_has_exact_store_address_set(u32 pool, s32 slot_index)
{
    u32 slot = expected_slot(pool, slot_index);
    bool saw_field0 = false;
    bool saw_field1 = false;
    size_t i;

    if (s_trace_count != 3u)
        return false;
    for (i = 1u; i < 3u; i++) {
        if (s_trace[i].kind != WM_87710_TRACE_SW ||
            s_trace[i].width != 4u)
            return false;
        if (s_trace[i].address == slot + 0x50u)
            saw_field0 = true;
        if (s_trace[i].address == slot + 0x54u)
            saw_field1 = true;
    }
    return saw_field0 && saw_field1;
}

static size_t changed_byte_count(const u8* before, const u8* after,
                                 size_t byte_count)
{
    size_t changed = 0u;
    size_t i;

    for (i = 0u; i < byte_count; i++) {
        if (before[i] != after[i])
            changed++;
    }
    return changed;
}

static void run_direct_fixture(const Fixture* fixture)
{
    static u8 before[PSX_RAM_SIZE];
    static u8 expected[PSX_RAM_SIZE];
    u32 slot;
    s32 result;

    seed_ram(fixture->seed);
    raw_store_u32(TEST_POOL_PTR, fixture->pool);
    slot = expected_slot(fixture->pool, fixture->slot);
    /* Every source byte differs from the retail destinations, making the
     * exact eight-byte change requirement observable in final memory too. */
    raw_store_u32(slot + TEST_FIELD_50, 0xA5B6C7D8u);
    raw_store_u32(slot + TEST_FIELD_54, 0x1A2B3C4Du);
    memcpy(before, g_PsxRam, sizeof(before));
    memcpy(expected, before, sizeof(expected));
    oracle_run(expected, fixture->pool, fixture->slot);

    trace_reset();
    s_alias_calls = 0;
    result = wm_80087710(fixture->slot);

    check_case(fixture->name, "return-value-exactly-1", result == 1);
    check_case(fixture->name, "exactly-one-u32-read-0x8009BE24",
               s_trace_count >= 1u && s_trace[0].kind == WM_87710_TRACE_LW &&
               s_trace[0].address == TEST_POOL_PTR && s_trace[0].width == 4u);
    check_case(fixture->name, "exact-derived-slot-base-plus-index-times-0x80",
               trace_has_exact_store_address_set(fixture->pool,
                                                 fixture->slot));
    check_case(fixture->name, "exact-store-sequence-u32-0-then-8",
               trace_matches(fixture->pool, fixture->slot));
    check_case(fixture->name, "independent-oracle-final-ram-image",
               memcmp(expected, g_PsxRam, sizeof(expected)) == 0);
    check_case(fixture->name, "exactly-eight-intended-bytes-changed",
               changed_byte_count(before, g_PsxRam, sizeof(before)) == 8u);
    check_case(fixture->name, "slot14-unchanged",
               memcmp(before + ram_index(fixture->pool + 14u * TEST_SLOT_STRIDE),
                      g_PsxRam + ram_index(fixture->pool + 14u * TEST_SLOT_STRIDE),
                      TEST_SLOT_STRIDE) == 0);
    check_case(fixture->name, "slot16-unchanged",
               memcmp(before + ram_index(fixture->pool + 16u * TEST_SLOT_STRIDE),
                      g_PsxRam + ram_index(fixture->pool + 16u * TEST_SLOT_STRIDE),
                      TEST_SLOT_STRIDE) == 0);
    check_case(fixture->name, "all-neighboring-fields-unchanged",
               memcmp(before, expected, sizeof(before)) != 0 &&
               memcmp(expected, g_PsxRam, sizeof(expected)) == 0);
    check_case(fixture->name, "zero-helper-gpu-ot-gte-calls",
               s_alias_calls == 0);
}

static void set_slot(u32 pool, unsigned slot, s16 state,
                     u32 cb0, u32 cb1)
{
    u32 address = pool + (u32)slot * TEST_SLOT_STRIDE;
    raw_store_u16(address + TEST_SLOT_STATE, (u16)state);
    raw_store_u32(address + TEST_SLOT_CB0, cb0);
    raw_store_u32(address + TEST_SLOT_CB1, cb1);
}

static void prepare_single_slot(u32 pool, u32 callback)
{
    seed_ram(0xC001D00Du);
    raw_store_u32(TEST_POOL_PTR, pool);
    set_slot(pool, 15u, 0, callback, TEST_CB_87734);
    raw_store_u32(pool + 15u * TEST_SLOT_STRIDE + TEST_FIELD_50, 0x11111111u);
    raw_store_u32(pool + 15u * TEST_SLOT_STRIDE + TEST_FIELD_54, 0x22222222u);
}

static s16 early_natural_callback(int slot_index)
{
    (void)slot_index;
    return 1;
}

static s16 natural_92df8_callback(int slot_index)
{
    if (s_natural_chain_count < 3)
        s_natural_chain[s_natural_chain_count++] = TEST_CB_92DF8;
    return (s16)wm_80092DF8(slot_index);
}

static s16 natural_71a50_callback(int slot_index)
{
    if (s_natural_chain_count < 3)
        s_natural_chain[s_natural_chain_count++] = TEST_CB_71A50;
    return (s16)wm_80071A50(slot_index);
}

static void run_resolver_cases(void)
{
    static const struct {
        const char* name;
        u32 callback;
        int outcome;
        u32 frontier;
    } cases[] = {
        {"resolver-87710-implemented", 0x80087710u,
         WM_SCHED_PASS_COMPLETE, WM_SCHED_CUT_BEFORE_DRAWSYNC},
        {"resolver-87734-unresolved", TEST_CB_87734,
         WM_SCHED_STOP_MISSING_CALLBACK, TEST_CB_87734},
        {"resolver-877E0-unresolved-distinct-symbol", TEST_CB_877E0,
         WM_SCHED_STOP_INVALID_CALLBACK, TEST_CB_877E0},
        {"resolver-87804-unresolved-distinct-symbol", TEST_CB_87804,
         WM_SCHED_STOP_INVALID_CALLBACK, TEST_CB_87804},
        {"resolver-925A0-unresolved", TEST_CB_925A0,
         WM_SCHED_STOP_MISSING_CALLBACK, TEST_CB_925A0}
    };
    size_t i;

    for (i = 0u; i < sizeof(cases) / sizeof(cases[0]); i++) {
        prepare_single_slot(TEST_POOL_A, cases[i].callback);
        trace_reset();
        wm_sched_callback_registry_clear();
        wm_sched_reset();
        wm_80097800();
        check_case(cases[i].name, "resolver-outcome-exact",
                   wm_sched_get_outcome() == cases[i].outcome);
        check_case(cases[i].name, "resolver-frontier-exact",
                   wm_sched_get_frontier_pc() == cases[i].frontier);
        if (cases[i].callback == 0x80087710u) {
            check_case(cases[i].name, "implemented-callback-executed-once",
                       wm_sched_get_callbacks_executed() == 1 &&
                       raw_load_u32(TEST_POOL_A + 15u * TEST_SLOT_STRIDE +
                                    TEST_FIELD_50) == 0u &&
                       raw_load_u32(TEST_POOL_A + 15u * TEST_SLOT_STRIDE +
                                    TEST_FIELD_54) == 8u);
        } else {
            check_case(cases[i].name, "unresolved-body-not-executed",
                       wm_sched_get_callbacks_executed() == 0);
        }
    }
}

static void run_natural_scheduler_chain(void)
{
    static const u32 early_callbacks[13] = {
        0x800923A8u, 0x8008A2C8u, 0x8008B2BCu, 0x8008BB40u,
        0x8008C530u, 0x8008D3F0u, 0x8008DD6Cu, 0x8008E190u,
        0x800906E0u, 0x80091430u, 0x80091B54u, 0x80092234u,
        0x80092BE4u
    };
    u32 pool = TEST_POOL_B;
    u32 slot15;
    unsigned i;
    int completed_before;

    seed_ram(0x71A50877u);
    raw_store_u32(TEST_POOL_PTR, pool);
    for (i = 0u; i < 13u; i++)
        set_slot(pool, i, 0, early_callbacks[i],
                 (i == 0u) ? TEST_CB_925A0 : 0x8008A72Cu);
    set_slot(pool, 13u, 0, TEST_CB_92DF8, TEST_CB_92FD8);
    set_slot(pool, 14u, 0, TEST_CB_71A50, TEST_CB_71A58);
    set_slot(pool, 15u, 0, 0x80087710u, TEST_CB_87734);
    slot15 = pool + 15u * TEST_SLOT_STRIDE;
    raw_store_u32(slot15 + TEST_FIELD_50, 0xCAFEBABEu);
    raw_store_u32(slot15 + TEST_FIELD_54, 0x13572468u);

    wm_sched_callback_registry_clear();
    for (i = 0u; i < 13u; i++)
        wm_sched_callback_register(early_callbacks[i], early_natural_callback);
    wm_sched_callback_register(TEST_CB_92DF8, natural_92df8_callback);
    wm_sched_callback_register(TEST_CB_71A50, natural_71a50_callback);
    wm_sched_reset();
    s_natural_mode = true;
    s_natural_chain_count = 0;
    s_natural_87710_entries = 0;
    s_natural_entry_state = -1;
    s_natural_callback_state = -1;
    s_natural_pool = pool;
    trace_reset();

    completed_before = wm_sched_get_completed_passes();
    wm_80097800();

    check_case("natural-first-pass", "callback-chain-92DF8-71A50-87710",
               s_natural_chain_count == 3 &&
               s_natural_chain[0] == TEST_CB_92DF8 &&
               s_natural_chain[1] == TEST_CB_71A50 &&
               s_natural_chain[2] == 0x80087710u);
    check_case("natural-first-pass", "real-87710-enters-exactly-once",
               s_natural_87710_entries == 1);
    check_case("natural-first-pass", "slot15-entry-state-zero",
               s_natural_entry_state == 0);
    check_case("natural-first-pass", "slot15-callback-time-state-zero",
               s_natural_callback_state == 0);
    check_case("natural-first-pass", "slot15-exact-writes",
               raw_load_u32(slot15 + TEST_FIELD_50) == 0u &&
               raw_load_u32(slot15 + TEST_FIELD_54) == 8u);
    check_case("natural-first-pass", "scheduler-publishes-slot15-0-to-1",
               raw_load_s16(slot15 + TEST_SLOT_STATE) == 1);
    check_case("natural-first-pass", "completed-passes-0-to-1",
               completed_before == 0 && wm_sched_get_completed_passes() == 1);
    check_case("natural-first-pass", "first-pass-counts",
               wm_sched_get_slots_inspected() == 64 &&
               wm_sched_get_dispatch_attempts() == 16 &&
               wm_sched_get_callbacks_executed() == 16);

    /* The first unresolved target on the next natural pass is slot 0 cb1;
     * do not execute its body. */
    wm_80097800();
    s_natural_mode = false;
    check_case("natural-second-pass", "second-pass-frontier-925A0",
               wm_sched_get_frontier_pc() == TEST_CB_925A0 &&
               wm_sched_get_last_slot() == 0 &&
               wm_sched_get_last_callback_state() == 1 &&
               wm_sched_get_outcome() == WM_SCHED_STOP_MISSING_CALLBACK);
    check_case("natural-second-pass", "925A0-body-not-executed",
               wm_sched_get_callbacks_executed() == 16 &&
               wm_sched_get_completed_passes() == 1);

    printf("NATURAL_CHAIN 0x80092DF8 -> 0x80071A50 -> 0x80087710\n");
    printf("NATURAL_87710_ENTRY count=%d slot15_entry_state=%d "
           "callback_time_state=%d slot15_final_state=%d\n",
           s_natural_87710_entries, s_natural_entry_state,
           s_natural_callback_state, (int)raw_load_s16(slot15 + TEST_SLOT_STATE));
    printf("FIRST_PASS slots_inspected=64 callbacks_executed=16 "
           "dispatch_attempts=16 completed_passes=1\n");
    printf("SECOND_PASS target=0x800925A0 body=not-executed\n");
}

/* The alias mutant calls a test-only seam representing the different symbol.
 * It is not part of the production link and only exists to certify M12. */
s32 wm_test_alias_877e0(s32 slot_index)
{
    (void)slot_index;
    s_alias_calls++;
    return 1;
}

/* Minimal dependencies for the linked 0x80092DF8 body.  They record no GPU
 * work; the natural-chain test only needs the callback's real memory body to
 * return 1 and the scheduler's real resolver/publish behavior. */
void func_80032F54(void* object, s32 tpage_x, s32 tpage_y,
                   s32 x, s32 y, s32 width, s32 host_dead_mode, s32 height)
{
    (void)object; (void)tpage_x; (void)tpage_y; (void)x; (void)y;
    (void)width; (void)host_dead_mode; (void)height;
}

void func_80034614(void* object) { (void)object; }
u16 GetTPage(int tp, int abr, int x, int y)
{
    (void)tp; (void)abr; (void)x; (void)y;
    return 0x001Eu;
}
u16 GetClut(int x, int y)
{
    (void)x; (void)y;
    return 0x0020u;
}
void SetSemiTrans(void* primitive, int enabled)
{
    (void)primitive; (void)enabled;
}

int main(void)
{
    static const Fixture fixtures[] = {
        {"base-A-slot-0",  TEST_POOL_A,  0, 0x11112222u},
        {"base-A-slot-3",  TEST_POOL_A,  3, 0x33334444u},
        {"base-B-slot-15", TEST_POOL_B, 15, 0x55556666u},
        {"base-C-slot-17", TEST_POOL_C, 17, 0x77778888u},
        {"base-C-slot-neg1", TEST_POOL_C, -1, 0x9999AAAAu}
    };
    size_t i;

    memset(g_PsxRam, 0, sizeof(g_PsxRam));
    memset(g_PsxScratchpad, 0, sizeof(g_PsxScratchpad));
    for (i = 0u; i < sizeof(fixtures) / sizeof(fixtures[0]); i++)
        run_direct_fixture(&fixtures[i]);
    run_resolver_cases();
    run_natural_scheduler_chain();

    printf("RETAIL_BOUNDARY [0x80087710,0x80087734) bytes=36 insns=9\n");
    printf("RETAIL_SLICE_SHA256 a3f62bc691ef9e4fecea300f902b7be0a48beb84b8ac5406f3e439ea95909e78\n");
    printf("EXPECTED_ACCESS read=u32:0x8009BE24 stores=u32:+0x50=0,+0x54=8 order=read,store,store\n");
    printf("=== Results: %d/%d PASS ===\n", s_pass, s_total);
    return s_fail == 0 ? 0 : 1;
}
