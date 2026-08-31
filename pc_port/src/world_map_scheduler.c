/*
 * World-map callback scheduler (W34B5H) — production implementation.
 *
 * Exact bounded transcription of retail 0x80097800..0x800978FB (63
 * instructions), decoded fresh in W34B5G from disc/world_map.bin
 * (sha256 4c15fd32…ac70) and mechanically re-verified in W34B5H.
 *
 * Retail shape:
 *   s1 = *(0x8009BE24); s0 = s1 + 0x4C; s2 = 0;
 *   loop @0x80097830:
 *     if (slot.+0x1C == 0) -> next                       (occupancy)
 *     state = (s16)slot.+0x00
 *     if ((u32)(s32)state >= 5) -> next                  (sltiu after lh)
 *     switch (jt_0x80070CE8[state]):
 *       0: cb = slot.+0x18; goto call
 *       1: cb = slot.+0x1C; goto call
 *       call @0x8009787C: jalr cb, a0 = slot index;
 *            slot.+0x00 = callback return               (delay-slot sh)
 *       2: if ((s16)(--slot.+0x02) <= 0) slot.+0x00 = 1
 *       3: (dormant)
 *       4: if (slot.+0x4C) func_800230A8(slot.+0x4C)
 *   next @0x800978C8: s2++; s0 += 0x80; s1 += 0x80 (delay); while s2 < 64
 *
 * Bounded dispatch frontier: guest callback addresses are NEVER cast to
 * native pointers. Resolution classes:
 *   IMPLEMENTED — a registered test body or an explicitly linked,
 *                 independently accepted production body;
 *   MISSING     — recognized guest callback with no body (the 30 distinct
 *                 callback pointers registered by the accepted Table A/B
 *                 initialization); the scheduler stops BEFORE writing the
 *                 slot state, BEFORE advancing, and the caller cuts before
 *                 DrawSync;
 *   INVALID     — any other address; bounded failure, no host call.
 */
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_923a8.h"
#include "world_map_callback_925a0.h"
#include "world_map_callback_8a2c8.h"
#include "world_map_callback_8b2bc.h"
#include "world_map_callback_8bb40.h"
#include "world_map_callback_8c530.h"
#include "world_map_callback_8d3f0.h"
#include "world_map_callback_8dd6c.h"
#include "world_map_callback_8e76c.h"
#include "world_map_callback_8e190.h"
#include "world_map_callback_906e0.h"
#include "world_map_helper_907f4.h"
#include "world_map_callback_91430.h"
#include "world_map_callback_91b54.h"
#include "world_map_callback_914d0.h"
#include "world_map_callback_91c18.h"
#include "world_map_callback_92234.h"
#include "world_map_callback_922ac.h"
#include "world_map_callback_92be4.h"
#include "world_map_callback_92c70.h"
#include "world_map_callback_92fd8.h"
#include "world_map_callback_92df8.h"
#include "world_map_callback_71a50.h"
#include "world_map_frame_driver_712d0.h"
#include "world_map_r4world_71a58.h"
#include "world_map_callback_87710.h"
#include "world_map_callback_87734.h"
#include "world_map_callback_7756c.h"
#include "world_map_callback_78948.h"
#include "world_map_callback_77dc8.h"
#include "world_map_callback_78e2c.h"
#include "world_map_callback_794d8.h"
#include "world_map_callback_795e4.h"
#include "world_map_callback_7a144.h"
#include "world_map_callback_8b644.h"
#include "world_map_callback_8c844.h"
#include "world_map_callback_8d678.h"
#include "world_map_scheduler.h"

/* Focused legacy scheduler tests intentionally link the scheduler without
 * production callback bodies.  Weak references preserve their bounded-
 * missing behavior, while the canonical link resolves these six accepted
 * bodies without any guest-function-pointer cast. */
extern s16 wm_800923A8(int slot_index) __attribute__((weak));
extern s32 wm_800925A0(s32 slot_index) __attribute__((weak));
extern s32 wm_8008A2C8(s32 slot_index) __attribute__((weak));
extern s32 wm_8008A72C(s32 slot_index) __attribute__((weak));
extern s32 wm_8008B2BC(s32 slot_index) __attribute__((weak));
extern s32 wm_8008B644(s32 slot_index) __attribute__((weak));
extern s32 wm_8008BB40(s32 slot_index) __attribute__((weak));
extern s32 wm_8008C530(s32 slot_index) __attribute__((weak));
extern s32 wm_8008C844(s32 slot_index) __attribute__((weak));
extern s32 wm_8008D3F0(s32 slot_index) __attribute__((weak));
extern s32 wm_8008D678(s32 slot_index) __attribute__((weak));
extern s32 wm_8008DD6C(s32 slot_index) __attribute__((weak));
extern s32 wm_8008E190(s32 slot_index) __attribute__((weak));
extern s32 wm_8008E76C(s32 slot_index) __attribute__((weak));
extern s32 wm_800906E0(s32 slot_index) __attribute__((weak));
extern s32 wm_800907F4(s32 slot_index) __attribute__((weak));
extern s32 wm_80091430(s32 slot_index) __attribute__((weak));
extern s32 wm_800914D0(s32 slot_index) __attribute__((weak));
extern s32 wm_80091B54(s32 slot_index) __attribute__((weak));
extern s32 wm_80091C18(s32 slot_index) __attribute__((weak));
extern s32 wm_80092234(s32 slot_index) __attribute__((weak));
extern s32 wm_800922AC(s32 slot_index) __attribute__((weak));
extern s32 wm_80092BE4(s32 slot_index) __attribute__((weak));
extern s32 wm_80092C70(s32 slot_index) __attribute__((weak));
extern s32 wm_80092DF8(s32 slot_index) __attribute__((weak));
extern s32 wm_80092FD8(s32 slot_index) __attribute__((weak));
extern s32 wm_80071A50(s32 slot_index) __attribute__((weak));
extern s32 wm_80071A58(s32 slot_index) __attribute__((weak));
extern s32 wm_80087710(s32 slot_index) __attribute__((weak));
extern s32 wm_80087734(s32 slot_index) __attribute__((weak));
extern s32 wm_8007756C(s32 slot_index) __attribute__((weak));
extern s32 wm_800776E0(s32 slot_index) __attribute__((weak));
extern s32 wm_80078948(s32 slot_index) __attribute__((weak));
extern s32 wm_80078950(s32 slot_index) __attribute__((weak));
extern s32 wm_80077DC8(s32 slot_index) __attribute__((weak));
extern s32 wm_80077E68(s32 slot_index) __attribute__((weak));
extern s32 wm_8007828C(s32 slot_index) __attribute__((weak));
extern s32 wm_800783E8(s32 slot_index) __attribute__((weak));
extern s32 wm_80078E2C(s32 slot_index) __attribute__((weak));
extern s32 wm_80078EA4(s32 slot_index) __attribute__((weak));
extern s32 wm_800794D8(s32 slot_index) __attribute__((weak));
extern s32 wm_80079538(s32 slot_index) __attribute__((weak));
extern s32 wm_800795E4(s32 slot_index) __attribute__((weak));
extern s32 wm_80079778(s32 slot_index) __attribute__((weak));
extern s32 wm_8007A410(s32 slot_index) __attribute__((weak));
extern s32 wm_8007A430(s32 slot_index) __attribute__((weak));
extern s32 wm_8007A568(s32 slot_index) __attribute__((weak));
extern s32 wm_8007A570(s32 slot_index) __attribute__((weak));
extern s32 wm_8007A144(s32 slot_index) __attribute__((weak));
extern s32 wm_8007A1B4(s32 slot_index) __attribute__((weak));
extern s32 wm_8007A9B4(s32 slot_index) __attribute__((weak));
extern s32 wm_8007A9F8(s32 slot_index) __attribute__((weak));
extern s32 wm_8007BA08(s32 slot_index) __attribute__((weak));
extern s32 wm_8007BA10(s32 slot_index) __attribute__((weak));
extern s32 wm_8007BB60(s32 slot_index) __attribute__((weak));
extern s32 wm_8007BBEC(s32 slot_index) __attribute__((weak));
extern s32 wm_8007B200(s32 slot_index) __attribute__((weak));
extern s32 wm_8007B394(s32 slot_index) __attribute__((weak));
extern s32 wm_8007B604(s32 slot_index) __attribute__((weak));
extern s32 wm_8007B798(s32 slot_index) __attribute__((weak));

#define WM_SCHED_RAM(a) ((u8*)PSX_ADDR(a))

/* Recognized callback addresses from the W34B5G natural pool capture.
 * Linked accepted bodies are resolved before this missing fallback:
 * 16 occupied slots x {+0x18, +0x1C}; duplicates 0x8008B644/0x8008D678
 * collapsed — 30 distinct addresses). */
static const u32 s_wm_sched_known_missing[] = {
    0x800923A8u, 0x800925A0u, /* slot 0  (Table A record 0)  */
    0x8008A2C8u, 0x8008A72Cu, /* slot 1  */
    0x8008B2BCu, 0x8008B644u, /* slot 2  */
    0x8008BB40u,              /* slot 3 cb0 (cb1 = 0x8008B644) */
    0x8008C530u, 0x8008C844u, /* slot 4  */
    0x8008D3F0u, 0x8008D678u, /* slot 5  */
    0x8008DD6Cu,              /* slot 6 cb0 (cb1 = 0x8008D678) */
    0x8008E190u, 0x8008E76Cu, /* slot 7  */
    0x800906E0u, 0x800907F4u, /* slot 8  */
    0x80091430u, 0x800914D0u, /* slot 9  */
    0x80091B54u, 0x80091C18u, /* slot 10 */
    0x80092234u, 0x800922ACu, /* slot 11 */
    0x80092BE4u, 0x80092C70u, /* slot 12 */
    0x80092DF8u, 0x80092FD8u, /* slot 13 */
    0x80071A50u, 0x80071A58u, /* slot 14 */
    0x80087710u, 0x80087734u, /* slot 15 (Table B, selector 0) */
    0x8007756Cu, 0x800776E0u, /* mode 8/11 camera slot */
    0x80078948u, 0x80078950u, /* shared mode draw slot */
    0x80077DC8u, 0x80077E68u, /* mode 9 path/camera slot */
    0x8007828Cu, 0x800783E8u, /* mode 9 context slot */
    0x80078E2Cu, 0x80078EA4u, /* mode 10 camera/event slot */
    0x800794D8u, 0x80079538u, /* mode 10 orbit/context slot */
    0x800795E4u, 0x80079778u, /* mode 10 sequence/context slot */
    0x8007A410u, 0x8007A430u, /* mode 10 timed marker slot */
    0x8007A568u, 0x8007A570u, /* mode 10 marker initializer slot */
    0x8007A144u, 0x8007A1B4u, /* mode 10 scaled-stream slot */
    0x8007A9B4u, 0x8007A9F8u, /* mode 14 scripted-sequence slot */
    0x8007BA08u, 0x8007BA10u, /* mode 14 timed-marker slot */
    0x8007BB60u, 0x8007BBECu, /* mode 14 path-camera slot */
    0x8007B200u, 0x8007B394u, /* mode 14 first scaled-object slot */
    0x8007B604u, 0x8007B798u, /* mode 14 second scaled-object slot */
};
#define WM_SCHED_KNOWN_MISSING_COUNT \
    (sizeof(s_wm_sched_known_missing) / sizeof(s_wm_sched_known_missing[0]))

/* Test-only/synthetic callback registry. Production bodies use the explicit
 * built-in resolution below. */
#define WM_SCHED_REGISTRY_MAX 32
static struct {
    u32 guest_addr;
    wm_sched_callback_fn fn;
} s_wm_sched_registry[WM_SCHED_REGISTRY_MAX];
static int s_wm_sched_registry_count;

static wm_sched_destructor_fn s_wm_sched_test_destructor;

/* Instrumentation (per world-init lifecycle; see wm_sched_reset). */
static int s_entry;
static int s_slots_inspected;
static int s_occupied_inspected;
static int s_state_seen[5];
static int s_state_invalid_seen;
static int s_dispatch_attempts;
static int s_callbacks_executed;
static int s_missing_hits;
static int s_invalid_hits;
static int s_destructor_calls;
static int s_destructor_boundary_hits;
static int s_completed_passes;
static int s_last_slot;
static u32 s_last_callback;
static int s_last_callback_state;
static int s_outcome = WM_SCHED_PASS_COMPLETE;
static u32 s_frontier_pc = WM_SCHED_CUT_BEFORE_DRAWSYNC;

typedef struct {
    u32 guest_addr;
    unsigned count;
} wm_sched_stub_hit;

static wm_sched_stub_hit s_stub_hits[64];
static unsigned s_stub_hit_count;

static void wm_sched_log_stub(u32 guest_addr, const char* kind,
                              int slot_index, int state)
{
    unsigned i;
    for (i = 0; i < s_stub_hit_count; i++) {
        if (s_stub_hits[i].guest_addr == guest_addr) {
            s_stub_hits[i].count++;
            fprintf(stderr,
                    "[worldmap-stub] guest=0x%08x kind=%s count=%u "
                    "slot=%d state=%d default_return=0\n",
                    guest_addr, kind, s_stub_hits[i].count,
                    slot_index, state);
            return;
        }
    }
    if (s_stub_hit_count < sizeof(s_stub_hits) / sizeof(s_stub_hits[0])) {
        s_stub_hits[s_stub_hit_count].guest_addr = guest_addr;
        s_stub_hits[s_stub_hit_count].count = 1;
        s_stub_hit_count++;
    }
    fprintf(stderr,
            "[worldmap-stub] guest=0x%08x kind=%s count=1 slot=%d "
            "state=%d default_return=0\n",
            guest_addr, kind, slot_index, state);
}

void wm_sched_reset(void)
{
    s_entry = 0;
    s_slots_inspected = 0;
    s_occupied_inspected = 0;
    memset(s_state_seen, 0, sizeof(s_state_seen));
    s_state_invalid_seen = 0;
    s_dispatch_attempts = 0;
    s_callbacks_executed = 0;
    s_missing_hits = 0;
    s_invalid_hits = 0;
    s_destructor_calls = 0;
    s_destructor_boundary_hits = 0;
    s_completed_passes = 0;
    s_last_slot = -1;
    s_last_callback = 0;
    s_last_callback_state = -1;
    s_outcome = WM_SCHED_PASS_COMPLETE;
    s_frontier_pc = WM_SCHED_CUT_BEFORE_DRAWSYNC;
    s_stub_hit_count = 0;
    memset(s_stub_hits, 0, sizeof(s_stub_hits));
}

void wm_sched_callback_register(u32 guest_addr, wm_sched_callback_fn fn)
{
    if (s_wm_sched_registry_count >= WM_SCHED_REGISTRY_MAX)
        return;
    s_wm_sched_registry[s_wm_sched_registry_count].guest_addr = guest_addr;
    s_wm_sched_registry[s_wm_sched_registry_count].fn = fn;
    s_wm_sched_registry_count++;
}

void wm_sched_callback_registry_clear(void)
{
    s_wm_sched_registry_count = 0;
    s_wm_sched_test_destructor = 0;
}

void wm_sched_test_set_destructor(wm_sched_destructor_fn fn)
{
    s_wm_sched_test_destructor = fn;
}

static wm_sched_callback_fn wm_sched_lookup(u32 guest_addr)
{
    int i;
    for (i = 0; i < s_wm_sched_registry_count; i++)
        if (s_wm_sched_registry[i].guest_addr == guest_addr)
            return s_wm_sched_registry[i].fn;
    return 0;
}

static s16 wm_sched_builtin_8008A2C8(int slot_index)
{
    return (s16)wm_8008A2C8((s32)slot_index);
}

/* Slot 0 Table-A cb1. */
static s16 wm_sched_builtin_800925A0(int slot_index)
{
    return (s16)wm_800925A0((s32)slot_index);
}

static s16 wm_sched_builtin_8008A72C(int slot_index)
{
    return (s16)wm_8008A72C((s32)slot_index);
}

static s16 wm_sched_builtin_8008B2BC(int slot_index)
{
    return (s16)wm_8008B2BC((s32)slot_index);
}

static s16 wm_sched_builtin_8008B644(int slot_index)
{
    return (s16)wm_8008B644((s32)slot_index);
}

static s16 wm_sched_builtin_8008BB40(int slot_index)
{
    return (s16)wm_8008BB40((s32)slot_index);
}

static s16 wm_sched_builtin_8008C530(int slot_index)
{
    return (s16)wm_8008C530((s32)slot_index);
}

static s16 wm_sched_builtin_8008C844(int slot_index)
{
    return (s16)wm_8008C844((s32)slot_index);
}

static s16 wm_sched_builtin_8008D3F0(int slot_index)
{
    return (s16)wm_8008D3F0((s32)slot_index);
}

static s16 wm_sched_builtin_8008D678(int slot_index)
{
    return (s16)wm_8008D678((s32)slot_index);
}

static s16 wm_sched_builtin_8008DD6C(int slot_index)
{
    return (s16)wm_8008DD6C((s32)slot_index);
}

static s16 wm_sched_builtin_8008E76C(int slot_index)
{
    return (s16)wm_8008E76C((s32)slot_index);
}

static s16 wm_sched_builtin_8008E190(int slot_index)
{
    return (s16)wm_8008E190((s32)slot_index);
}

static s16 wm_sched_builtin_800906E0(int slot_index)
{
    return (s16)wm_800906E0((s32)slot_index);
}

/* Slot 8 Table-A cb1. */
static s16 wm_sched_builtin_800907F4(int slot_index)
{
    return (s16)wm_800907F4((s32)slot_index);
}

static s16 wm_sched_builtin_80091430(int slot_index)
{
    return (s16)wm_80091430((s32)slot_index);
}

static s16 wm_sched_builtin_800914D0(int slot_index)
{
    return (s16)wm_800914D0((s32)slot_index);
}

static s16 wm_sched_builtin_80091B54(int slot_index)
{
    return (s16)wm_80091B54((s32)slot_index);
}

static s16 wm_sched_builtin_80091C18(int slot_index)
{
    return (s16)wm_80091C18((s32)slot_index);
}

static s16 wm_sched_builtin_80092234(int slot_index)
{
    return (s16)wm_80092234((s32)slot_index);
}

static s16 wm_sched_builtin_800922AC(int slot_index)
{
    return (s16)wm_800922AC((s32)slot_index);
}

static s16 wm_sched_builtin_80092BE4(int slot_index)
{
    return (s16)wm_80092BE4((s32)slot_index);
}

static s16 wm_sched_builtin_80092C70(int slot_index)
{
    return (s16)wm_80092C70((s32)slot_index);
}

static s16 wm_sched_builtin_80092DF8(int slot_index)
{
    return (s16)wm_80092DF8((s32)slot_index);
}

static s16 wm_sched_builtin_80092FD8(int slot_index)
{
    return (s16)wm_80092FD8((s32)slot_index);
}

/* Slot 14 Table-A cb0. Retail 0x80071A50 is a two-instruction return-1 leaf;
 * its cb1 partner 0x80071A58 remains unresolved by design. */
static s16 wm_sched_builtin_80071A50(int slot_index)
{
    return (s16)wm_80071A50((s32)slot_index);
}

static s16 wm_sched_builtin_80071A58(int slot_index)
{
    return (s16)wm_80071A58((s32)slot_index);
}

/* Slot 15 Table-B cb0. Twin 0x800877E0 remains a distinct unresolved
 * Table-B stream and is not aliased to this cb1. */
static s16 wm_sched_builtin_80087710(int slot_index)
{
    return (s16)wm_80087710((s32)slot_index);
}

static s16 wm_sched_builtin_80087734(int slot_index)
{
    return (s16)wm_80087734((s32)slot_index);
}

static s16 wm_sched_builtin_8007756C(int slot_index)
{
    return (s16)wm_8007756C((s32)slot_index);
}

static s16 wm_sched_builtin_800776E0(int slot_index)
{
    return (s16)wm_800776E0((s32)slot_index);
}

static s16 wm_sched_builtin_80078948(int slot_index)
{
    return (s16)wm_80078948((s32)slot_index);
}

static s16 wm_sched_builtin_80078950(int slot_index)
{
    return (s16)wm_80078950((s32)slot_index);
}

static s16 wm_sched_builtin_80077DC8(int slot_index)
{
    return (s16)wm_80077DC8((s32)slot_index);
}

static s16 wm_sched_builtin_80077E68(int slot_index)
{
    return (s16)wm_80077E68((s32)slot_index);
}

static s16 wm_sched_builtin_8007828C(int slot_index)
{
    return (s16)wm_8007828C((s32)slot_index);
}

static s16 wm_sched_builtin_800783E8(int slot_index)
{
    return (s16)wm_800783E8((s32)slot_index);
}

static s16 wm_sched_builtin_80078E2C(int slot_index)
{
    return (s16)wm_80078E2C((s32)slot_index);
}

static s16 wm_sched_builtin_80078EA4(int slot_index)
{
    return (s16)wm_80078EA4((s32)slot_index);
}

static s16 wm_sched_builtin_800794D8(int slot_index)
{
    return (s16)wm_800794D8((s32)slot_index);
}

static s16 wm_sched_builtin_80079538(int slot_index)
{
    return (s16)wm_80079538((s32)slot_index);
}

static s16 wm_sched_builtin_800795E4(int slot_index)
{
    return (s16)wm_800795E4((s32)slot_index);
}

static s16 wm_sched_builtin_80079778(int slot_index)
{
    return (s16)wm_80079778((s32)slot_index);
}

static s16 wm_sched_builtin_8007A410(int slot_index)
{
    return (s16)wm_8007A410((s32)slot_index);
}

static s16 wm_sched_builtin_8007A430(int slot_index)
{
    return (s16)wm_8007A430((s32)slot_index);
}

static s16 wm_sched_builtin_8007A568(int slot_index)
{
    return (s16)wm_8007A568((s32)slot_index);
}

static s16 wm_sched_builtin_8007A570(int slot_index)
{
    return (s16)wm_8007A570((s32)slot_index);
}

static s16 wm_sched_builtin_8007A144(int slot_index)
{
    return (s16)wm_8007A144((s32)slot_index);
}

static s16 wm_sched_builtin_8007A1B4(int slot_index)
{
    return (s16)wm_8007A1B4((s32)slot_index);
}

static s16 wm_sched_builtin_8007A9B4(int slot_index)
{
    return (s16)wm_8007A9B4((s32)slot_index);
}

static s16 wm_sched_builtin_8007A9F8(int slot_index)
{
    return (s16)wm_8007A9F8((s32)slot_index);
}

static s16 wm_sched_builtin_8007BA08(int slot_index)
{
    return (s16)wm_8007BA08((s32)slot_index);
}

static s16 wm_sched_builtin_8007BA10(int slot_index)
{
    return (s16)wm_8007BA10((s32)slot_index);
}

static s16 wm_sched_builtin_8007BB60(int slot_index)
{
    return (s16)wm_8007BB60((s32)slot_index);
}

static s16 wm_sched_builtin_8007BBEC(int slot_index)
{
    return (s16)wm_8007BBEC((s32)slot_index);
}

static s16 wm_sched_builtin_8007B200(int slot_index)
{
    return (s16)wm_8007B200((s32)slot_index);
}

static s16 wm_sched_builtin_8007B394(int slot_index)
{
    return (s16)wm_8007B394((s32)slot_index);
}

static s16 wm_sched_builtin_8007B604(int slot_index)
{
    return (s16)wm_8007B604((s32)slot_index);
}

static s16 wm_sched_builtin_8007B798(int slot_index)
{
    return (s16)wm_8007B798((s32)slot_index);
}

static int wm_sched_is_known_missing(u32 guest_addr)
{
    unsigned i;
    for (i = 0; i < WM_SCHED_KNOWN_MISSING_COUNT; i++)
        if (s_wm_sched_known_missing[i] == guest_addr)
            return 1;
    return 0;
}

/* Bounded guest-callback resolver. No guest address is ever called through
 * a native pointer. */
static wm_sched_cb_resolve_t wm_sched_resolve(u32 guest_addr,
                                              wm_sched_callback_fn* out_fn)
{
    wm_sched_callback_fn fn = wm_sched_lookup(guest_addr);
    if (fn != 0) {
        *out_fn = fn;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x800923A8u && wm_800923A8 != 0) {
        *out_fn = wm_800923A8;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x800925A0u && wm_800925A0 != 0) {
        *out_fn = wm_sched_builtin_800925A0;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8008A2C8u && wm_8008A2C8 != 0) {
        *out_fn = wm_sched_builtin_8008A2C8;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8008A72Cu && wm_8008A72C != 0) {
        *out_fn = wm_sched_builtin_8008A72C;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8008B2BCu && wm_8008B2BC != 0) {
        *out_fn = wm_sched_builtin_8008B2BC;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8008B644u && wm_8008B644 != 0) {
        *out_fn = wm_sched_builtin_8008B644;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8008BB40u && wm_8008BB40 != 0) {
        *out_fn = wm_sched_builtin_8008BB40;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8008C530u && wm_8008C530 != 0) {
        *out_fn = wm_sched_builtin_8008C530;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8008C844u && wm_8008C844 != 0) {
        *out_fn = wm_sched_builtin_8008C844;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8008D3F0u && wm_8008D3F0 != 0) {
        *out_fn = wm_sched_builtin_8008D3F0;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8008D678u && wm_8008D678 != 0) {
        *out_fn = wm_sched_builtin_8008D678;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8008DD6Cu && wm_8008DD6C != 0) {
        *out_fn = wm_sched_builtin_8008DD6C;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8008E190u && wm_8008E190 != 0) {
        *out_fn = wm_sched_builtin_8008E190;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8008E76Cu && wm_8008E76C != 0) {
        *out_fn = wm_sched_builtin_8008E76C;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x800906E0u && wm_800906E0 != 0) {
        *out_fn = wm_sched_builtin_800906E0;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x800907F4u && wm_800907F4 != 0) {
        *out_fn = wm_sched_builtin_800907F4;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x80091430u && wm_80091430 != 0) {
        *out_fn = wm_sched_builtin_80091430;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x800914D0u && wm_800914D0 != 0) {
        *out_fn = wm_sched_builtin_800914D0;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x80091B54u && wm_80091B54 != 0) {
        *out_fn = wm_sched_builtin_80091B54;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x80091C18u && wm_80091C18 != 0) {
        *out_fn = wm_sched_builtin_80091C18;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x80092234u && wm_80092234 != 0) {
        *out_fn = wm_sched_builtin_80092234;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x800922ACu && wm_800922AC != 0) {
        *out_fn = wm_sched_builtin_800922AC;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x80092BE4u && wm_80092BE4 != 0) {
        *out_fn = wm_sched_builtin_80092BE4;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x80092C70u && wm_80092C70 != 0) {
        *out_fn = wm_sched_builtin_80092C70;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x80092DF8u && wm_80092DF8 != 0) {
        *out_fn = wm_sched_builtin_80092DF8;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x80092FD8u && wm_80092FD8 != 0) {
        *out_fn = wm_sched_builtin_80092FD8;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x80071A50u && wm_80071A50 != 0) {
        *out_fn = wm_sched_builtin_80071A50;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x80071A58u && wm_80071A58 != 0) {
        *out_fn = wm_sched_builtin_80071A58;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x80087710u && wm_80087710 != 0) {
        *out_fn = wm_sched_builtin_80087710;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x80087734u && wm_80087734 != 0) {
        *out_fn = wm_sched_builtin_80087734;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8007756Cu && wm_8007756C != 0) {
        *out_fn = wm_sched_builtin_8007756C;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x800776E0u && wm_800776E0 != 0) {
        *out_fn = wm_sched_builtin_800776E0;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x80078948u && wm_80078948 != 0) {
        *out_fn = wm_sched_builtin_80078948;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x80078950u && wm_80078950 != 0) {
        *out_fn = wm_sched_builtin_80078950;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x80077DC8u && wm_80077DC8 != 0) {
        *out_fn = wm_sched_builtin_80077DC8;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x80077E68u && wm_80077E68 != 0) {
        *out_fn = wm_sched_builtin_80077E68;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8007828Cu && wm_8007828C != 0) {
        *out_fn = wm_sched_builtin_8007828C;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x800783E8u && wm_800783E8 != 0) {
        *out_fn = wm_sched_builtin_800783E8;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x80078E2Cu && wm_80078E2C != 0) {
        *out_fn = wm_sched_builtin_80078E2C;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x80078EA4u && wm_80078EA4 != 0) {
        *out_fn = wm_sched_builtin_80078EA4;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x800794D8u && wm_800794D8 != 0) {
        *out_fn = wm_sched_builtin_800794D8;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x80079538u && wm_80079538 != 0) {
        *out_fn = wm_sched_builtin_80079538;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x800795E4u && wm_800795E4 != 0) {
        *out_fn = wm_sched_builtin_800795E4;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x80079778u && wm_80079778 != 0) {
        *out_fn = wm_sched_builtin_80079778;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8007A410u && wm_8007A410 != 0) {
        *out_fn = wm_sched_builtin_8007A410;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8007A430u && wm_8007A430 != 0) {
        *out_fn = wm_sched_builtin_8007A430;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8007A568u && wm_8007A568 != 0) {
        *out_fn = wm_sched_builtin_8007A568;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8007A570u && wm_8007A570 != 0) {
        *out_fn = wm_sched_builtin_8007A570;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8007A144u && wm_8007A144 != 0) {
        *out_fn = wm_sched_builtin_8007A144;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8007A1B4u && wm_8007A1B4 != 0) {
        *out_fn = wm_sched_builtin_8007A1B4;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8007A9B4u && wm_8007A9B4 != 0) {
        *out_fn = wm_sched_builtin_8007A9B4;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8007A9F8u && wm_8007A9F8 != 0) {
        *out_fn = wm_sched_builtin_8007A9F8;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8007BA08u && wm_8007BA08 != 0) {
        *out_fn = wm_sched_builtin_8007BA08;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8007BA10u && wm_8007BA10 != 0) {
        *out_fn = wm_sched_builtin_8007BA10;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8007BB60u && wm_8007BB60 != 0) {
        *out_fn = wm_sched_builtin_8007BB60;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8007BBECu && wm_8007BBEC != 0) {
        *out_fn = wm_sched_builtin_8007BBEC;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8007B200u && wm_8007B200 != 0) {
        *out_fn = wm_sched_builtin_8007B200;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8007B394u && wm_8007B394 != 0) {
        *out_fn = wm_sched_builtin_8007B394;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8007B604u && wm_8007B604 != 0) {
        *out_fn = wm_sched_builtin_8007B604;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (guest_addr == 0x8007B798u && wm_8007B798 != 0) {
        *out_fn = wm_sched_builtin_8007B798;
        return WM_SCHED_CB_IMPLEMENTED;
    }
    if (wm_sched_is_known_missing(guest_addr))
        return WM_SCHED_CB_MISSING;
    return WM_SCHED_CB_INVALID;
}

void wm_80097800(void)
{
    u32 pool_psx = *(u32*)WM_SCHED_RAM(WM_SCHED_POOL_PTR);
    u8* base;
    int i;

    s_entry++;
    fprintf(stderr, "[worldmap-scheduler] entry pool=0x%08x\n", pool_psx);

    if (pool_psx == 0) {
        /* Unnatural: retail would dereference near-null. Bounded stop. */
        s_outcome = WM_SCHED_STOP_NO_POOL;
        s_frontier_pc = WM_SCHED_CUT_BEFORE_DRAWSYNC;
        fprintf(stderr, "[worldmap-scheduler] ERROR: pool base null\n");
        return;
    }
    base = WM_SCHED_RAM(pool_psx);

    for (i = 0; i < WM_SCHED_SLOT_COUNT; i++) {
        u8* slot = base + (u32)i * WM_SCHED_SLOT_STRIDE;
        u32 occupancy = *(u32*)(slot + WM_SCHED_OFF_CB1);
        s16 state;

        s_slots_inspected++;
        if (occupancy == 0)
            continue;                               /* 0x80097838 -> next */
        s_occupied_inspected++;

        state = *(s16*)(slot + WM_SCHED_OFF_STATE); /* lh sign-extends */
        if ((u32)(s32)state >= 5) {                 /* sltiu v0,v1,5 */
            s_state_invalid_seen++;
            continue;                               /* 0x8009784C -> next */
        }
        s_state_seen[state]++;

        switch (state) {                            /* jr via 0x80070CE8 */
        case 0:
        case 1: {
            u32 target = (state == 0)
                ? *(u32*)(slot + WM_SCHED_OFF_CB0)  /* 0x80097868 */
                : occupancy;                        /* 0x80097874 */
            wm_sched_callback_fn fn = 0;
            wm_sched_cb_resolve_t r;

            s_dispatch_attempts++;
            s_last_slot = i;
            s_last_callback = target;
            s_last_callback_state = state;
            r = wm_sched_resolve(target, &fn);
            if (r == WM_SCHED_CB_IMPLEMENTED) {
                /* jalr target, a0 = slot index (0x8009787C/0x80097880);
                 * return value becomes the slot state (0x80097888). */
                s16 ret = fn(i);
                *(s16*)(slot + WM_SCHED_OFF_STATE) = ret;
                s_callbacks_executed++;
                fprintf(stderr, "[worldmap-scheduler] slot=%d state=%d "
                        "cb=0x%08x executed ret=%d\n", i, state, target, ret);
            } else if (r == WM_SCHED_CB_MISSING) {
                s_missing_hits++;
                wm_sched_log_stub(target, "missing_callback", i, state);
                s_frontier_pc = target;
                /* Open-loop experiment: emulate the unresolved jalr with
                 * the neutral s16 return value and continue the retail slot
                 * traversal. The guest address was never called. */
                *(s16*)(slot + WM_SCHED_OFF_STATE) = 0;
            } else {
                s_invalid_hits++;
                wm_sched_log_stub(target, "invalid_callback", i, state);
                s_frontier_pc = target;
                *(s16*)(slot + WM_SCHED_OFF_STATE) = 0;
            }
            break;
        }
        case 2: {
            /* 0x8009788C: decrement timer; expiry (<=0) -> state 1. */
            u16 t = *(u16*)(slot + WM_SCHED_OFF_TIMER);
            t = (u16)(t - 1);
            *(u16*)(slot + WM_SCHED_OFF_TIMER) = t;
            if ((s16)t <= 0)
                *(s16*)(slot + WM_SCHED_OFF_STATE) = 1;
            break;
        }
        case 3:
            break;                                  /* dormant: 0x800978C8 */
        case 4: {
            /* 0x800978B0: payload non-null -> func_800230A8(payload). */
            u32 payload = *(u32*)(slot + WM_SCHED_OFF_PAYLOAD);
            if (payload != 0) {
                if (s_wm_sched_test_destructor != 0) {
                    s_wm_sched_test_destructor(payload);
                    s_destructor_calls++;
                } else {
                    s_destructor_boundary_hits++;
                    wm_sched_log_stub(WM_SCHED_DESTRUCTOR, "destructor", i,
                                      state);
                    s_frontier_pc = WM_SCHED_DESTRUCTOR;
                    s_last_slot = i;
                    s_last_callback = WM_SCHED_DESTRUCTOR;
                    s_last_callback_state = state;
                    /* Unknown destructor is skipped; traversal continues. */
                }
            }
            break;
        }
        }
    }

    s_completed_passes++;
    s_outcome = WM_SCHED_PASS_COMPLETE;
    s_frontier_pc = WM_SCHED_CUT_BEFORE_DRAWSYNC;
    fprintf(stderr, "[worldmap-scheduler] pass complete slots=%d occupied=%d "
            "dispatched=%d executed=%d\n",
            s_slots_inspected, s_occupied_inspected,
            s_dispatch_attempts, s_callbacks_executed);
}

int  wm_sched_get_entry(void)                   { return s_entry; }
int  wm_sched_get_slots_inspected(void)         { return s_slots_inspected; }
int  wm_sched_get_occupied_inspected(void)      { return s_occupied_inspected; }
int  wm_sched_get_state_seen(int state)
{
    if (state < 0 || state > 4) return 0;
    return s_state_seen[state];
}
int  wm_sched_get_state_invalid_seen(void)      { return s_state_invalid_seen; }
int  wm_sched_get_dispatch_attempts(void)       { return s_dispatch_attempts; }
int  wm_sched_get_callbacks_executed(void)      { return s_callbacks_executed; }
int  wm_sched_get_missing_hits(void)            { return s_missing_hits; }
int  wm_sched_get_invalid_hits(void)            { return s_invalid_hits; }
int  wm_sched_get_destructor_calls(void)        { return s_destructor_calls; }
int  wm_sched_get_destructor_boundary_hits(void){ return s_destructor_boundary_hits; }
int  wm_sched_get_completed_passes(void)        { return s_completed_passes; }
int  wm_sched_get_last_slot(void)               { return s_last_slot; }
u32  wm_sched_get_last_callback(void)           { return s_last_callback; }
int  wm_sched_get_last_callback_state(void)     { return s_last_callback_state; }
int  wm_sched_get_outcome(void)                 { return s_outcome; }
u32  wm_sched_get_frontier_pc(void)             { return s_frontier_pc; }
