/*
 * LP64 proofs for the battle tick-watch callbacks
 * (func_800B5854 / func_800BF73C).
 *
 * TimerWorkListAllocateTask keeps a packed 0x1C header; the spawners then
 * store extra payload as u32:
 *   func_800B5924  +0x1C actor, +0x24 AF, +0x28 tick, +0x2C floor, +0x30 copy
 *   func_800BF7C8  +0x1C actor, +0x20 AF, +0x24 tick, +0x28 floor, +0x2C cb
 *
 * An 8-byte `*(u8**)` load of +0x1C concatenates the neighbouring word
 * (uninitialized leftover on 5924, the AF state byte on BF7C8). The first
 * TimerWorkListUpdate then follows a wild actor pointer.
 *
 * Trigger: battle overlay calls func_800B5924 / func_800BF7C8 to watch an
 * actor's tick, then the timer list pumps the host C callback.
 *
 * Build/run via pc_port/tests/run_battle_tick_watch_packed_slots_test.sh
 */
#include "common.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void func_800B5854(u8* task);
void func_800BF73C(u8* task);

static u8* g_seen_actor;
static u8* g_seen_task;
static u8* g_seen_floor_actor;
static int g_floor_calls;
static int g_free_calls;
static int g_tick_calls;
static s32 g_tick_return;

u32 func_800B57E4(u8* s)
{
    g_tick_calls++;
    g_seen_actor = s;
    return g_tick_return;
}

static void floor_cb(u8* actor)
{
    g_floor_calls++;
    g_seen_floor_actor = actor;
}

static void free_cb(u8* task)
{
    g_free_calls++;
    g_seen_task = task;
}

static int g_failures;

static void expect(int cond, const char* msg)
{
    if (!cond) {
        printf("FAIL: %s\n", msg);
        g_failures++;
    } else {
        printf("ok:   %s\n", msg);
    }
}

static void test_slot_concat(void)
{
    unsigned char task[0x40];
    void* as_ptr;
    const uint32_t actor = 0x00A10000u;
    const uint32_t af = 0x00000007u;

    memset(task, 0, sizeof(task));
    *(uint32_t*)(task + 0x1C) = actor;
    *(uint32_t*)(task + 0x20) = af;

    memcpy(&as_ptr, task + 0x1C, sizeof(as_ptr));
    expect((uintptr_t)as_ptr == (((uintptr_t)af << 32) | actor),
           "8-byte load of task +0x1C concatenates the AF/neighbour word");
    expect(as_ptr != (void*)(uintptr_t)actor,
           "concatenated pointer is not the actor slot");
    expect(*(uint32_t*)(task + 0x1C) == actor,
           "4-byte load of task +0x1C is just the actor");
}

static void reset_obs(void)
{
    g_seen_actor = NULL;
    g_seen_task = NULL;
    g_seen_floor_actor = NULL;
    g_floor_calls = 0;
    g_free_calls = 0;
    g_tick_calls = 0;
    g_tick_return = 5;
}

static unsigned char s_task[0x40];
static unsigned char s_actor[0xC0];

static void plant_bf7c8_task(void)
{
    memset(s_task, 0, sizeof(s_task));
    memset(s_actor, 0, sizeof(s_actor));
    assert((uintptr_t)s_actor <= UINT32_MAX);
    assert((uintptr_t)floor_cb <= UINT32_MAX);
    assert((uintptr_t)free_cb <= UINT32_MAX);

    *(uint32_t*)(s_task + 0x0C) = (uint32_t)(uintptr_t)free_cb;
    *(uint32_t*)(s_task + 0x10) = 0x12345678u; /* planted work-list flags */
    *(uint32_t*)(s_task + 0x1C) = (uint32_t)(uintptr_t)s_actor;
    *(uint32_t*)(s_task + 0x20) = 0x00000007u; /* planted AF state */
    *(uint32_t*)(s_task + 0x24) = 4;           /* prev tick */
    *(uint32_t*)(s_task + 0x28) = 0;           /* floor */
    *(uint32_t*)(s_task + 0x2C) = (uint32_t)(uintptr_t)floor_cb;
    *(uint32_t*)(s_task + 0x30) = 0xAABBCCDDu; /* past the 0x14 payload */
}

static void plant_b5924_task(void)
{
    memset(s_task, 0, sizeof(s_task));
    memset(s_actor, 0, sizeof(s_actor));
    assert((uintptr_t)s_actor <= UINT32_MAX);
    assert((uintptr_t)free_cb <= UINT32_MAX);

    *(uint32_t*)(s_task + 0x0C) = (uint32_t)(uintptr_t)free_cb;
    *(uint32_t*)(s_task + 0x10) = 0x12345678u;
    *(uint32_t*)(s_task + 0x1C) = (uint32_t)(uintptr_t)s_actor;
    *(uint32_t*)(s_task + 0x20) = 0xDEADBEEFu; /* leftover payload, not written */
    *(uint32_t*)(s_task + 0x24) = 0;           /* AF snapshot */
    *(uint32_t*)(s_task + 0x28) = 5;           /* prev tick */
    *(uint32_t*)(s_task + 0x2C) = 0;           /* floor */
    *(uint32_t*)(s_task + 0x30) = 0x11111111u;
    *(s16*)(s_actor + 0x9E) = 1; /* already armed, skip that dirty check */
    s_actor[0xAF] = 0;
}

static void test_func_800BF73C_live(void)
{
    reset_obs();
    plant_bf7c8_task();
    g_tick_return = 6; /* rose past prev=4 so the dirty path fires */
    func_800BF73C(s_task);

    expect(g_tick_calls == 1, "func_800BF73C reached func_800B57E4");
    expect(g_seen_actor == s_actor,
           "func_800BF73C sees the actor, not actor|AF");
    expect(g_floor_calls == 1, "func_800BF73C fired the +0x2C floor callback");
    expect(g_seen_floor_actor == s_actor,
           "floor callback received the 4-byte actor");
    expect(g_free_calls == 1, "func_800BF73C fired the +0xC callback");
    expect(g_seen_task == s_task, "+0xC callback received the task");
    expect(*(uint32_t*)(s_task + 0x20) == 0x00000007u, "AF canary intact");
    expect(*(uint32_t*)(s_task + 0x10) == 0x12345678u, "flags canary intact");
}

static void test_func_800B5854_live(void)
{
    reset_obs();
    plant_b5924_task();
    g_tick_return = 5; /* same as prev: only +0x9E would dirty, and it is 1 */
    func_800B5854(s_task);

    expect(g_tick_calls == 1, "func_800B5854 reached func_800B57E4");
    expect(g_seen_actor == s_actor,
           "func_800B5854 sees the actor, not actor|leftover");
    expect(g_free_calls == 0, "func_800B5854 does not fire when not dirty");
    expect(*(uint32_t*)(s_actor + 0x64) == 0,
           "actor +0x64 untouched when not dirty");
    expect(*(uint32_t*)(s_task + 0x20) == 0xDEADBEEFu,
           "leftover +0x20 canary intact");
}

static void test_func_800B5854_dirty(void)
{
    reset_obs();
    plant_b5924_task();
    g_tick_return = 9; /* rose past prev=5 */
    func_800B5854(s_task);

    expect(g_seen_actor == s_actor,
           "dirty func_800B5854 still sees the 4-byte actor");
    expect(*(s16*)(s_actor + 0x9E) == 1, "dirty path armed +0x9E");
    expect(*(uint32_t*)(s_actor + 0x64) == 0x11111111u,
           "dirty path copied +0x30 into actor +0x64");
    expect(g_free_calls == 1, "dirty func_800B5854 fired the +0xC callback");
    expect(g_seen_task == s_task, "dirty +0xC callback received the task");
}

int main(void)
{
    expect(sizeof(void*) == 8, "LP64 host");
    test_slot_concat();
    test_func_800BF73C_live();
    test_func_800B5854_live();
    test_func_800B5854_dirty();

    if (g_failures) {
        printf("%d failure(s)\n", g_failures);
        return 1;
    }
    printf("all battle tick-watch packed-slot proofs passed\n");
    return 0;
}
