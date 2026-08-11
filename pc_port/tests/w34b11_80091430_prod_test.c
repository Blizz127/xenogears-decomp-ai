/*
 * W34B11 production-linked retail certificate for callback 0x80091430.
 *
 * The expected access stream below is a declarative transcription of
 * world_map.bin [0x80091430,0x800914D0), not a clone of the production
 * implementation.  The compile-time-only production seam records semantic
 * guest reads and writes with their retail PC, kind, width, address, value,
 * and order.  This is essential at 0x80091494: omitting the fresh signed LH
 * from BD3A gives the same final bytes but is not retail-equivalent.
 */
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"

enum {
    TRACE_LHU = 1,
    TRACE_LH = 2,
    TRACE_LW = 3,
    TRACE_SH = 4,
    TRACE_SW = 5
};

typedef struct TraceEvent {
    u32 pc;
    u32 kind;
    u32 address;
    u32 width;
    u32 value;
} TraceEvent;

#define TRACE_CAPACITY 32u

static TraceEvent s_actual_trace[TRACE_CAPACITY];
static size_t s_actual_trace_count;
static bool s_actual_trace_overflow;

void wm_91430_test_trace(u32 pc, u32 kind, u32 address, u32 width,
                         u32 value)
{
    if (s_actual_trace_count < (size_t)TRACE_CAPACITY) {
        TraceEvent* event = &s_actual_trace[s_actual_trace_count];
        event->pc = pc;
        event->kind = kind;
        event->address = address;
        event->width = width;
        event->value = value;
    } else {
        s_actual_trace_overflow = true;
    }
    s_actual_trace_count++;
}

#define WM_91430_TEST_TRACE 1
#ifndef WM_91430_PRODUCTION_SOURCE
#define WM_91430_PRODUCTION_SOURCE "../src/world_map_callback_91430.c"
#endif
#include WM_91430_PRODUCTION_SOURCE

#define RAM_MASK       0x001FFFFFu
#define MAIN_RAM_BYTES 0x00200000u

#define POOL_POINTER 0x8009BE24u
#define MODE_GLOBAL  0x8009BE10u
#define VECTOR_X     0x8009D55Cu
#define VECTOR_Y     0x8009D560u
#define VECTOR_Z     0x8009D564u
#define VECTOR_W     0x8009D568u
#define HEADING_RAW  0x8009D52Cu
#define HEADING_X    0x8009BD3Au
#define HEADING_Z    0x8009BD3Cu

#define NATURAL_POOL 0x800D7538u
#define SLOT_STRIDE  0x80u
#define SLOT_STATE   0x00u
#define SLOT_CONTROL 0x20u
#define SLOT_X       0x28u
#define SLOT_Y       0x2Cu
#define SLOT_Z       0x30u
#define SLOT_W       0x34u
#define SLOT_HEADING 0x50u
#define SLOT_SHIFTED 0x58u

uint8_t g_PsxRam[PSX_RAM_SIZE];
uint8_t g_PsxScratchpad[4096];

typedef struct Fixture {
    const char* name;
    s32 slot_index;
    u32 pool;
    s32 mode;
    u16 heading;
} Fixture;

typedef struct Oracle {
    u8 memory[PSX_RAM_SIZE];
    TraceEvent trace[TRACE_CAPACITY];
    size_t trace_count;
} Oracle;

static u8 s_before_ram[PSX_RAM_SIZE];
static u8 s_before_scratch[sizeof(g_PsxScratchpad)];
static Oracle s_oracle;
static int s_pass_count;
static int s_total_count;
static int s_failure_count;

static size_t ram_index(u32 address)
{
    return (size_t)(address & RAM_MASK);
}

static u16 memory_load_u16(const u8* memory, u32 address)
{
    u16 value;
    memcpy(&value, memory + ram_index(address), sizeof(value));
    return value;
}

static u32 memory_load_u32(const u8* memory, u32 address)
{
    u32 value;
    memcpy(&value, memory + ram_index(address), sizeof(value));
    return value;
}

static void memory_store_u16(u8* memory, u32 address, u16 value)
{
    memcpy(memory + ram_index(address), &value, sizeof(value));
}

static void memory_store_u32(u8* memory, u32 address, u32 value)
{
    memcpy(memory + ram_index(address), &value, sizeof(value));
}

static void raw_store_u16(u32 address, u16 value)
{
    memory_store_u16(g_PsxRam, address, value);
}

static void raw_store_u32(u32 address, u32 value)
{
    memory_store_u32(g_PsxRam, address, value);
}

static u16 raw_load_u16(u32 address)
{
    return memory_load_u16(g_PsxRam, address);
}

static u32 raw_load_u32(u32 address)
{
    return memory_load_u32(g_PsxRam, address);
}

static u32 s32_bits(s32 value)
{
    u32 result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

static u32 sign_extend_u16(u16 value)
{
    u32 result = (u32)value;
    if ((result & 0x8000u) != 0u)
        result |= 0xFFFF0000u;
    return result;
}

static u32 shifted_heading(u16 value)
{
    return sign_extend_u16(value) << 12;
}

static bool mode_writes_control(s32 mode)
{
    /* Independent signed partitions from SLTI 8 then SLTI 6. */
    return mode >= 6 && mode < 8;
}

static void check_case(const char* fixture, const char* property, bool ok)
{
    s_total_count++;
    if (ok) {
        s_pass_count++;
    } else {
        s_failure_count++;
        printf("FAIL [%s]: %s\n", fixture, property);
    }
}

static const char* trace_kind_name(u32 kind)
{
    switch (kind) {
    case TRACE_LHU: return "LHU";
    case TRACE_LH: return "LH";
    case TRACE_LW: return "LW";
    case TRACE_SH: return "SH";
    case TRACE_SW: return "SW";
    default: return "UNKNOWN";
    }
}

static void oracle_event(u32 pc, u32 kind, u32 address, u32 width,
                         u32 value)
{
    TraceEvent* event = &s_oracle.trace[s_oracle.trace_count];
    event->pc = pc;
    event->kind = kind;
    event->address = address;
    event->width = width;
    event->value = value;
    s_oracle.trace_count++;
}

static u16 oracle_lhu(u32 pc, u32 address)
{
    u16 value = memory_load_u16(s_oracle.memory, address);
    oracle_event(pc, TRACE_LHU, address, 2u, (u32)value);
    return value;
}

static u16 oracle_lh_bits(u32 pc, u32 address)
{
    u16 value = memory_load_u16(s_oracle.memory, address);
    oracle_event(pc, TRACE_LH, address, 2u, sign_extend_u16(value));
    return value;
}

static u32 oracle_lw(u32 pc, u32 address)
{
    u32 value = memory_load_u32(s_oracle.memory, address);
    oracle_event(pc, TRACE_LW, address, 4u, value);
    return value;
}

static void oracle_sh(u32 pc, u32 address, u16 value)
{
    memory_store_u16(s_oracle.memory, address, value);
    oracle_event(pc, TRACE_SH, address, 2u, (u32)value);
}

static void oracle_sw(u32 pc, u32 address, u32 value)
{
    memory_store_u32(s_oracle.memory, address, value);
    oracle_event(pc, TRACE_SW, address, 4u, value);
}

static void seed_memory(u32 salt)
{
    size_t index;

    for (index = 0u; index < sizeof(g_PsxRam); index++) {
        u32 low = (u32)(index & 0xFFFFFFFFu);
        g_PsxRam[index] =
            (u8)((low * 37u + (low >> 9) + salt * 43u + 0x6Du) & 0xFFu);
    }
    for (index = 0u; index < sizeof(g_PsxScratchpad); index++) {
        u32 low = (u32)index;
        g_PsxScratchpad[index] =
            (u8)((low * 29u + salt * 17u + 0xB3u) & 0xFFu);
    }
}

static void reset_trace(void)
{
    memset(s_actual_trace, 0, sizeof(s_actual_trace));
    s_actual_trace_count = 0u;
    s_actual_trace_overflow = false;
}

static void build_oracle(const Fixture* fixture, u32 slot)
{
    u32 x;
    u32 y;
    u32 z;
    u32 w;
    u16 heading;
    u16 reloaded;

    memcpy(s_oracle.memory, g_PsxRam, sizeof(g_PsxRam));
    memset(s_oracle.trace, 0, sizeof(s_oracle.trace));
    s_oracle.trace_count = 0u;

    (void)oracle_lw(0x80091434u, POOL_POINTER);
    x = oracle_lw(0x80091448u, VECTOR_X);
    y = oracle_lw(0x8009144Cu, VECTOR_Y);
    z = oracle_lw(0x80091450u, VECTOR_Z);
    oracle_sw(0x80091454u, slot + SLOT_X, x);
    oracle_sw(0x80091458u, slot + SLOT_Y, y);
    oracle_sw(0x8009145Cu, slot + SLOT_Z, z);
    w = oracle_lw(0x80091460u, VECTOR_W);
    oracle_sw(0x80091468u, slot + SLOT_W, w);

    heading = oracle_lhu(0x80091470u, HEADING_RAW);
    /* Fresh raw words prove 0x91480 BD3C precedes 0x91484 BD3A. */
    oracle_sh(0x80091480u, HEADING_Z, 0u);
    oracle_sh(0x80091484u, HEADING_X, heading);
    oracle_sw(0x80091490u, slot + SLOT_HEADING,
              sign_extend_u16(heading));
    reloaded = oracle_lh_bits(0x80091494u, HEADING_X);
    (void)oracle_lw(0x8009149Cu, MODE_GLOBAL);
    oracle_sw(0x800914A4u, slot + SLOT_SHIFTED,
              shifted_heading(reloaded));

    if (mode_writes_control(fixture->mode))
        oracle_sh(0x800914C4u, slot + SLOT_CONTROL, 3u);
}

static bool trace_matches(const char* fixture)
{
    size_t index;

    if (s_actual_trace_overflow ||
        s_actual_trace_count != s_oracle.trace_count) {
        printf("TRACE [%s]: actual=%zu expected=%zu overflow=%d\n",
               fixture, s_actual_trace_count, s_oracle.trace_count,
               s_actual_trace_overflow ? 1 : 0);
        return false;
    }

    for (index = 0u; index < s_oracle.trace_count; index++) {
        const TraceEvent* actual = &s_actual_trace[index];
        const TraceEvent* expected = &s_oracle.trace[index];
        if (actual->pc != expected->pc ||
            actual->kind != expected->kind ||
            actual->address != expected->address ||
            actual->width != expected->width ||
            actual->value != expected->value) {
            printf("TRACE [%s] #%zu: actual %08x %s %08x/%u=%08x; "
                   "expected %08x %s %08x/%u=%08x\n",
                   fixture, index,
                   (unsigned int)actual->pc,
                   trace_kind_name(actual->kind),
                   (unsigned int)actual->address,
                   (unsigned int)actual->width,
                   (unsigned int)actual->value,
                   (unsigned int)expected->pc,
                   trace_kind_name(expected->kind),
                   (unsigned int)expected->address,
                   (unsigned int)expected->width,
                   (unsigned int)expected->value);
            return false;
        }
    }
    return true;
}

static bool fresh_bd3a_read_present(u32 slot)
{
    const size_t index = 13u;
    if (s_actual_trace_count <= index)
        return false;
    return s_actual_trace[index].pc == 0x80091494u &&
           s_actual_trace[index].kind == TRACE_LH &&
           s_actual_trace[index].address == HEADING_X &&
           s_actual_trace[index].width == 2u &&
           s_actual_trace[index].value ==
               sign_extend_u16(memory_load_u16(g_PsxRam, HEADING_X)) &&
           s_actual_trace[index - 1u].pc == 0x80091490u &&
           s_actual_trace[index - 1u].kind == TRACE_SW &&
           s_actual_trace[index - 1u].address == slot + SLOT_HEADING &&
           s_actual_trace[index + 1u].pc == 0x8009149Cu;
}

static bool slot_common_values_match(u32 slot, const Fixture* fixture)
{
    return raw_load_u32(slot + SLOT_X) == raw_load_u32(VECTOR_X) &&
           raw_load_u32(slot + SLOT_Y) == raw_load_u32(VECTOR_Y) &&
           raw_load_u32(slot + SLOT_Z) == raw_load_u32(VECTOR_Z) &&
           raw_load_u32(slot + SLOT_W) == raw_load_u32(VECTOR_W) &&
           raw_load_u32(slot + SLOT_HEADING) ==
               sign_extend_u16(fixture->heading) &&
           raw_load_u32(slot + SLOT_SHIFTED) ==
               shifted_heading(fixture->heading);
}

static void run_fixture(const Fixture* fixture, u32 salt)
{
    u32 slot = fixture->pool + (s32_bits(fixture->slot_index) << 7);
    u32 original_state;
    u16 original_control;
    u32 original_before;
    u32 original_after;
    u16 original_bd3a;
    u16 original_bd3c;
    s32 result;

    seed_memory(salt);
    raw_store_u32(POOL_POINTER, fixture->pool);
    raw_store_u32(MODE_GLOBAL, s32_bits(fixture->mode));
    raw_store_u32(VECTOR_X, 0x10213243u ^ salt);
    raw_store_u32(VECTOR_Y, 0x54657687u ^ (salt << 1));
    raw_store_u32(VECTOR_Z, 0x98A9BACBu ^ (salt << 2));
    raw_store_u32(VECTOR_W, 0xDCEDFE0Fu ^ (salt << 3));
    raw_store_u16(HEADING_RAW, fixture->heading);
    raw_store_u16(HEADING_X, (u16)(fixture->heading ^ 0x5A5Au));
    raw_store_u16(HEADING_Z, (u16)(fixture->heading ^ 0xA5A5u));

    original_state = raw_load_u32(slot + SLOT_STATE);
    original_control = raw_load_u16(slot + SLOT_CONTROL);
    original_before = raw_load_u32(slot - 4u);
    original_after = raw_load_u32(slot + SLOT_STRIDE);
    original_bd3a = raw_load_u16(HEADING_X);
    original_bd3c = raw_load_u16(HEADING_Z);

    memcpy(s_before_ram, g_PsxRam, sizeof(g_PsxRam));
    memcpy(s_before_scratch, g_PsxScratchpad, sizeof(g_PsxScratchpad));
    build_oracle(fixture, slot);
    reset_trace();

    result = wm_80091430(fixture->slot_index);

    check_case(fixture->name, "return is scheduler state 1", result == 1);
    check_case(fixture->name, "exact typed read/write trace",
               trace_matches(fixture->name));
    check_case(fixture->name, "exact path access count",
               s_actual_trace_count ==
                   (mode_writes_control(fixture->mode) ? 17u : 16u));
    check_case(fixture->name,
               "fresh signed BD3A read follows slot+50 publication",
               fresh_bd3a_read_present(slot));
    check_case(fixture->name, "asymmetric vector and heading publications",
               slot_common_values_match(slot, fixture));
    check_case(fixture->name, "BD3C cleared before BD3A publication",
               raw_load_u16(HEADING_Z) == 0u &&
                   raw_load_u16(HEADING_X) == fixture->heading &&
                   original_bd3a != fixture->heading &&
                   original_bd3c != 0u);
    check_case(fixture->name, "signed mode 6..7 control arm",
               mode_writes_control(fixture->mode)
                   ? raw_load_u16(slot + SLOT_CONTROL) == 3u
                   : raw_load_u16(slot + SLOT_CONTROL) == original_control);
    check_case(fixture->name, "scheduler-owned slot+00 untouched",
               raw_load_u32(slot + SLOT_STATE) == original_state);
    check_case(fixture->name, "adjacent slot canaries untouched",
               raw_load_u32(slot - 4u) == original_before &&
                   raw_load_u32(slot + SLOT_STRIDE) == original_after);
    check_case(fixture->name, "whole 3 MiB RAM exact footprint",
               memcmp(g_PsxRam, s_oracle.memory, sizeof(g_PsxRam)) == 0);
    check_case(fixture->name, "1 MiB guard canary untouched",
               memcmp(g_PsxRam + MAIN_RAM_BYTES,
                      s_before_ram + MAIN_RAM_BYTES,
                      sizeof(g_PsxRam) - MAIN_RAM_BYTES) == 0);
    check_case(fixture->name, "scratchpad untouched",
               memcmp(g_PsxScratchpad, s_before_scratch,
                      sizeof(g_PsxScratchpad)) == 0);
}

static void run_oracle_sentinels(void)
{
    check_case("oracle", "mode INT32_MIN common-only",
               !mode_writes_control(INT32_MIN));
    check_case("oracle", "mode 5 common-only", !mode_writes_control(5));
    check_case("oracle", "mode 6 conditional arm", mode_writes_control(6));
    check_case("oracle", "mode 7 conditional arm", mode_writes_control(7));
    check_case("oracle", "mode 8 common-only", !mode_writes_control(8));
    check_case("oracle", "heading 8001 sign extends",
               sign_extend_u16(0x8001u) == 0xFFFF8001u);
    check_case("oracle", "heading 8001 SLL12 wraps",
               shifted_heading(0x8001u) == 0xF8001000u);
    check_case("oracle", "heading FFFF SLL12 wraps",
               shifted_heading(0xFFFFu) == 0xFFFFF000u);
    check_case("oracle", "slot index -1 shift wraps",
               (s32_bits(-1) << 7) == 0xFFFFFF80u);
    check_case("oracle", "slot index INT32_MIN shift wraps",
               (s32_bits(INT32_MIN) << 7) == 0u);
}

int main(void)
{
    static const Fixture fixtures[] = {
        {"mode-int-min-negative-heading", 9, NATURAL_POOL,
         INT32_MIN, 0x8001u},
        {"mode-minus-one-all-ones", 9, NATURAL_POOL, -1, 0xFFFFu},
        {"mode-zero-zero-heading", 9, NATURAL_POOL, 0, 0x0000u},
        {"mode-one-natural", 9, NATURAL_POOL, 1, 0x1234u},
        {"mode-five-lower-edge", 9, NATURAL_POOL, 5, 0x7FFFu},
        {"mode-six-arm", 9, NATURAL_POOL, 6, 0xFEDCu},
        {"mode-seven-arm", 9, NATURAL_POOL, 7, 0x8000u},
        {"mode-eight-upper-edge", 9, NATURAL_POOL, 8, 0x0001u},
        {"mode-int-max", 9, NATURAL_POOL, INT32_MAX, 0xA55Au},
        {"slot-index-minus-one", -1, 0x800D8000u, 6, 0xF00Du},
        {"slot-index-int-min", INT32_MIN, 0x800D8100u, 7, 0x0FF0u},
        {"slot-addition-wrap", 1, 0xFFFFFFF0u, 8, 0x8123u}
    };
    size_t index;

    for (index = 0u; index < sizeof(fixtures) / sizeof(fixtures[0]);
         index++)
        run_fixture(&fixtures[index], (u32)index + 0x41u);

    run_oracle_sentinels();
    printf("=== Results: %d/%d PASS ===\n", s_pass_count, s_total_count);
    return s_failure_count == 0 ? 0 : 1;
}
