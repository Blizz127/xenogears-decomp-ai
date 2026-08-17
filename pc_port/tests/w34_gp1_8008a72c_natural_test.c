/*
 * Controller-natural measurement for live 0x8008A72C.
 *
 * Plants the retail Table-A callback addresses on slots 0-2 only.  No
 * guest PC is forced; the scheduler walks state 0 then state 1.  Slot-1
 * cb1 is the production 8A72C body.  The next unresolved target is
 * printed, not asserted.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_8a72c.h"
#include "world_map_scheduler.h"

#define POOL   0x80020000u
#define BE24   0x8009BE24u
#define F8E5   0x8006F8E5u

#define CB0_0  0x800923A8u
#define CB1_0  0x800925A0u
#define CB0_1  0x8008A2C8u
#define CB1_1  0x8008A72Cu
#define CB0_2  0x8008B2BCu
#define CB1_2  0x8008B644u

static int s_stub_calls;

static s16 stub_return_1(int slot_index)
{
    (void)slot_index;
    s_stub_calls++;
    return 1;
}

static void plant(int index, s16 state, u32 cb0, u32 cb1)
{
    u32 slot = POOL + (u32)index * WM_SCHED_SLOT_STRIDE;
    memset(PSX_ADDR(slot), 0, WM_SCHED_SLOT_STRIDE);
    *(s16 *)PSX_ADDR(slot + WM_SCHED_OFF_STATE) = state;
    *(u32 *)PSX_ADDR(slot + WM_SCHED_OFF_CB0) = cb0;
    *(u32 *)PSX_ADDR(slot + WM_SCHED_OFF_CB1) = cb1;
}

s32 wm_8a72c_test_90a84(u32 slot_addr) { (void)slot_addr; return 2; }
s32 wm_8a72c_test_95414(u32 a, u32 b, u32 c, s32 d, s32 e)
{ (void)a; (void)b; (void)c; (void)d; (void)e; return 1; }
void wm_8a72c_test_894c8(u32 i) { (void)i; }
void wm_8a72c_test_8c1dc(u32 a, u32 b, u32 c) { (void)a; (void)b; (void)c; }
void wm_8a72c_test_8c040(u32 a, s32 b, s32 c, u32 d, u32 e)
{ (void)a; (void)b; (void)c; (void)d; (void)e; }
s32 wm_8a72c_test_94238(u32 a, u32 b) { (void)a; (void)b; return 0; }
void wm_8a72c_test_7528c(void) {}
void wm_8a72c_test_74794(s32 a, u32 b) { (void)a; (void)b; }
s32 wm_8a72c_test_97770(u32 a, s32 b) { (void)a; (void)b; return 1; }
s32 wm_8a72c_test_941c4(u32 a, u32 b, u32 c, u32 d)
{ (void)a; (void)b; (void)c; (void)d; return 0; }
s32 wm_8a72c_test_8bec8(u32 a) { (void)a; return 0; }
s32 wm_8a72c_test_93978(s32 a, s32 b) { (void)a; (void)b; return 0; }
void wm_8a72c_test_245d8(void *o, s16 a) { (void)o; (void)a; }
long wm_8a72c_test_rcos(long a) { (void)a; return 0; }
long wm_8a72c_test_rsin(long a) { (void)a; return 0; }

int main(void)
{
    u32 slot1 = POOL + WM_SCHED_SLOT_STRIDE;
    s16 state_before;
    s16 state_after;
    int passes_before;
    int executed_before;
    s32 ret;

    PsxMemory_Init();
    memset(PSX_ADDR(POOL), 0, 16u * WM_SCHED_SLOT_STRIDE);
    *(u32 *)PSX_ADDR(BE24) = POOL;
    *(u8 *)PSX_ADDR(F8E5) = 0;

    plant(0, 0, CB0_0, CB1_0);
    plant(1, 0, CB0_1, CB1_1);
    plant(2, 0, CB0_2, CB1_2);

    wm_sched_callback_registry_clear();
    wm_sched_callback_register(CB0_0, stub_return_1);
    wm_sched_callback_register(CB1_0, stub_return_1);
    wm_sched_callback_register(CB0_1, stub_return_1);
    wm_sched_callback_register(CB0_2, stub_return_1);
    wm_sched_reset();

    wm_80097800();
    passes_before = wm_sched_get_completed_passes();
    executed_before = wm_sched_get_callbacks_executed();
    state_before = *(s16 *)PSX_ADDR(slot1 + WM_SCHED_OFF_STATE);

    wm_80097800();
    state_after = *(s16 *)PSX_ADDR(slot1 + WM_SCHED_OFF_STATE);
    ret = state_after;

    printf("8A72C_BODY_EXECUTED=%s\n",
           (wm_sched_get_callbacks_executed() > executed_before &&
            state_after == 1 &&
            wm_sched_get_last_slot() >= 1)
               ? "YES"
               : "NO");
    printf("8A72C_RETURN=%d\n", (int)ret);
    printf("SLOT1_STATE_BEFORE=%d\n", (int)state_before);
    printf("SLOT1_STATE_AFTER=%d\n", (int)state_after);
    printf("SCHEDULER_COMPLETED_PASSES_BEFORE=%d\n", passes_before);
    printf("SCHEDULER_COMPLETED_PASSES_AFTER=%d\n",
           wm_sched_get_completed_passes());
    printf("NEXT_CALLBACK_TARGET=0x%08X\n", wm_sched_get_last_callback());
    printf("NEXT_CALLBACK_SLOT=%d\n", wm_sched_get_last_slot());
    printf("NEXT_CALLBACK_STATE=%d\n", wm_sched_get_last_callback_state());
    printf("NEXT_CALLBACK_CLASS=%s\n",
           wm_sched_get_outcome() == WM_SCHED_STOP_MISSING_CALLBACK
               ? "MISSING"
               : (wm_sched_get_outcome() == WM_SCHED_PASS_COMPLETE
                      ? "PASS_COMPLETE"
                      : "OTHER"));
    printf("FRAME_FRONTIER=0x%08X\n", wm_sched_get_frontier_pc());
    printf("CRASH_OR_CUT_PC=0x%08X\n", wm_sched_get_frontier_pc());
    printf("NATURAL_NOTE first_pass_executed=%d stubs=%d\n",
           executed_before, s_stub_calls);
    (void)ret;
    return 0;
}
