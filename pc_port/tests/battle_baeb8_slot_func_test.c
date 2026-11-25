/*
 * Runs the real func_800BAEB8 port body against a planted D_800C3EB0 table
 * in guest RAM.  The u8** mutant (injected by the runner) must miss slot 0
 * or crash; the fixed body must pass actor 0 then actor 1 to the callees.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "battle_host_guest.h"

uint8_t g_PsxRam[PSX_RAM_SIZE];
uint8_t g_PsxScratchpad[0x1000];

extern void func_800BAEB8(s32 a0);

static u8* s_seen_dir;
static u8* s_seen_angle;
static s32 s_dir_flag;
static s32 s_angle_flag;
static int s_dir_calls;
static int s_angle_calls;

void func_800223B0(u8* p, s32 v)
{
    s_seen_dir = p;
    s_dir_flag = v;
    s_dir_calls++;
}

void func_80021FE0(u8* p, s32 v)
{
    s_seen_angle = p;
    s_angle_flag = v;
    s_angle_calls++;
}

static void reset_callees(void)
{
    s_seen_dir = NULL;
    s_seen_angle = NULL;
    s_dir_flag = -1;
    s_angle_flag = -1;
    s_dir_calls = 0;
    s_angle_calls = 0;
}

static u8* plant_actor(u32 guest, u8 state)
{
    u8* p = (u8*)PSX_ADDR(guest);

    memset(p, 0, 0xB8);
    p[0xAF] = state;
    return p;
}

int main(void)
{
    const u32 actor0 = 0x800D0000u;
    const u32 actor1 = 0x800D1000u;
    u8* host0;
    u8* host1;
    u8* table;
    u8* records;

    memset(g_PsxRam, 0, sizeof(g_PsxRam));
    host0 = plant_actor(actor0, 0);
    host1 = plant_actor(actor1, 0);

    table = (u8*)PSX_ADDR(0x800C3EB0u + 0x8C8C);
    *(u32*)(table + 0) = actor0;
    *(u32*)(table + 4) = actor1;
    *(u32*)(table + 8) = 0x800D2000u; /* decoy for an 8-byte-stride index 1 */

    records = (u8*)PSX_ADDR(0x800C3EB0u);
    records[0 * 28 + 0xA] = 1;
    records[1 * 28 + 0xA] = 1;

    reset_callees();
    func_800BAEB8(0);
    if (s_dir_calls != 1 || s_angle_calls != 1) {
        printf("FAIL: slot 0 did not call both callees (dir=%d angle=%d)\n",
               s_dir_calls, s_angle_calls);
        return 1;
    }
    if (s_seen_dir != host0 || s_seen_angle != host0) {
        printf("FAIL: slot 0 used %p/%p want %p (8-byte table load missed actor 0)\n",
               (void*)s_seen_dir, (void*)s_seen_angle, (void*)host0);
        return 1;
    }
    if (s_dir_flag != 0x800 || s_angle_flag != 0x800) {
        printf("FAIL: slot 0 flag=%d/%d want 0x800\n", s_dir_flag, s_angle_flag);
        return 1;
    }

    reset_callees();
    func_800BAEB8(1);
    if (s_dir_calls != 1 || s_angle_calls != 1) {
        printf("FAIL: slot 1 did not call both callees (dir=%d angle=%d)\n",
               s_dir_calls, s_angle_calls);
        return 1;
    }
    if (s_seen_dir != host1 || s_seen_angle != host1) {
        printf("FAIL: slot 1 used %p/%p want %p (u8** stride 8 skipped slot 1)\n",
               (void*)s_seen_dir, (void*)s_seen_angle, (void*)host1);
        return 1;
    }

    host0[0xAF] = 0x15;
    reset_callees();
    func_800BAEB8(0);
    if (s_dir_calls != 0 || s_angle_calls != 0) {
        printf("FAIL: state 0x15 should skip the flag apply\n");
        return 1;
    }

    printf("func_800BAEB8 live table walk used packed 4-byte slot pointers\n");
    return 0;
}
