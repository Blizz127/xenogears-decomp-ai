/* Focused production-linked certificate for retail slot 1, 0x80072238. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_convergence.h"
#include "world_map_session_setup_72238.h"

uint8_t g_PsxRam[PSX_RAM_SIZE];
uint8_t g_PsxScratchpad[4096];

enum Event {
    EV_FRAMEBUFFER = 1, EV_MOVE, EV_DRAWSYNC, EV_TRANSITION,
    EV_CDSYNC, EV_SECOND, EV_POOL, EV_TEMPLATE, EV_MODE, EV_CROSS,
    EV_WDS_CLEAN, EV_ENTRY, EV_GPU_A, EV_GPU_B, EV_OBJECT_MATRIX,
    EV_THIRD, EV_BSS, EV_PRIMS, EV_CLUT, EV_GFX, EV_FT4, EV_HEAP,
    EV_UPLOAD_A, EV_UPLOAD_B, EV_DRAW_PACKETS, EV_88F64, EV_ARCH_POLL,
    EV_FIRST_WDS, EV_ARCH_INDEX, EV_TERRAIN, EV_CD_WORK, EV_VSYNC,
    EV_DISTANCE, EV_READY, EV_AUDIO, EV_CONV_P1, EV_CONV_P2,
    EV_TAIL_P0, EV_TAIL_P1, EV_TAIL_P2, EV_TAIL_P3, EV_TAIL_P4,
    EV_TAIL_P5
};

typedef struct Rect4s { s16 x, y, w, h; } Rect4s;

static int events[128];
static int event_count;
static int failures;
static int distance_calls;

static void event(int value)
{
    if (event_count < (int)(sizeof(events) / sizeof(events[0])))
        events[event_count++] = value;
}

static void check(int condition, const char* name)
{
    if (!condition) {
        fprintf(stderr, "ASSERTION %s FAILED\n", name);
        failures++;
    }
}

static void write32(u32 address, u32 value)
{
    memcpy(PSX_ADDR(address), &value, sizeof(value));
}

static void write16(u32 address, u16 value)
{
    memcpy(PSX_ADDR(address), &value, sizeof(value));
}

#define STAGE(name, id) int name(void) { event(id); return 0; }
STAGE(wm_72238_stage_second_wave, EV_SECOND)
STAGE(wm_72238_stage_object_pool, EV_POOL)
STAGE(wm_72238_stage_state_template, EV_TEMPLATE)
STAGE(wm_72238_stage_mode_enter, EV_MODE)
STAGE(wm_72238_stage_cross_products, EV_CROSS)
STAGE(wm_72238_stage_wds_cleanup, EV_WDS_CLEAN)
STAGE(wm_72238_stage_entry_placement, EV_ENTRY)
STAGE(wm_72238_stage_gpu_asset_a, EV_GPU_A)
STAGE(wm_72238_stage_gpu_asset_b, EV_GPU_B)
STAGE(wm_72238_stage_object_matrix, EV_OBJECT_MATRIX)
STAGE(wm_72238_stage_third_wave, EV_THIRD)
STAGE(wm_72238_stage_bss_constants, EV_BSS)
STAGE(wm_72238_stage_primitive_templates, EV_PRIMS)
STAGE(wm_72238_stage_record_clut, EV_CLUT)
STAGE(wm_72238_stage_gfx_work_buffers, EV_GFX)
STAGE(wm_72238_stage_ft4_pools, EV_FT4)
STAGE(wm_72238_stage_heap_table, EV_HEAP)
STAGE(wm_72238_stage_upload_a, EV_UPLOAD_A)
STAGE(wm_72238_stage_upload_b, EV_UPLOAD_B)
STAGE(wm_72238_stage_draw_packets, EV_DRAW_PACKETS)
STAGE(wm_72238_stage_88f64, EV_88F64)
STAGE(wm_72238_stage_archive_poll, EV_ARCH_POLL)
STAGE(wm_72238_stage_first_wds, EV_FIRST_WDS)
STAGE(wm_72238_stage_archive_index, EV_ARCH_INDEX)

void wm_80072BB0(void) { event(EV_FRAMEBUFFER); }

int MoveImage(void* rect, int x, int y)
{
    const Rect4s* r = (const Rect4s*)rect;
    event(EV_MOVE);
    check(r->x == 0 && r->y == 0 && r->w == 320 && r->h == 216,
          "move_rect");
    check(x == 704 && y == 256, "move_destination");
    return 0;
}

int DrawSync(int mode)
{
    event(EV_DRAWSYNC);
    check(mode == 0, "draw_sync_mode");
    return 0;
}

int wm_80072DB4(s32 frames, s32 initial, s32 step, s32 abr)
{
    event(EV_TRANSITION);
    check(frames == 64 && initial == 0 && step == 4 && abr == 2,
          "transition_arguments");
    return 0;
}

void ArchiveCdDataSync(int mode)
{
    event(EV_CDSYNC);
    check(mode == 0, "cd_sync_mode");
}

void wm_80097BC0(u32 pos)
{
    event(EV_TERRAIN);
    check(pos == 0x8009C5ACu, "terrain_position_source");
}

u32 wm_800967E4(void)
{
    event(EV_CD_WORK);
    return 0u;
}

int Vsync(int mode)
{
    event(EV_VSYNC);
    check(mode == 0, "vsync_mode");
    return 0;
}

u32 wm_80096668_circular_distance(void)
{
    event(EV_DISTANCE);
    distance_calls++;
    return distance_calls == 1 ? 3u : 1u;
}

void wm_ready_buffer_consume(void) { event(EV_READY); }
void wm_mode_audio_setup(void) { event(EV_AUDIO); }
wm_conv_p1_next_t wm_800726C0_convergence_p1(void)
{
    event(EV_CONV_P1);
    return WM_CONV_P1_CUT_SECOND_TABLE;
}
u32 wm_8007272C_convergence_p2(void)
{
    event(EV_CONV_P2);
    return WM_CONV_P1_CUT_COMMON_TAIL;
}
u32 wm_8007290C_common_tail_p0(void) { event(EV_TAIL_P0); return 0u; }
u32 wm_8007293C_common_tail_p1(void) { event(EV_TAIL_P1); return 0u; }
u32 wm_80072944_common_tail_p2(void) { event(EV_TAIL_P2); return 0u; }
u32 wm_8007294C_common_tail_p3(void) { event(EV_TAIL_P3); return 0u; }
u32 wm_80072954_common_tail_p4(void) { event(EV_TAIL_P4); return 0u; }
u32 wm_8007295C_common_tail_p5(void) { event(EV_TAIL_P5); return 0u; }

static void check_order(void)
{
    static const int expected[] = {
        EV_FRAMEBUFFER, EV_MOVE, EV_DRAWSYNC, EV_TRANSITION,
        EV_CDSYNC, EV_SECOND, EV_POOL, EV_TEMPLATE, EV_MODE, EV_CROSS,
        EV_WDS_CLEAN, EV_ENTRY, EV_CDSYNC, EV_GPU_A, EV_GPU_B,
        EV_OBJECT_MATRIX, EV_THIRD, EV_BSS, EV_PRIMS, EV_CLUT, EV_GFX,
        EV_FT4, EV_HEAP, EV_UPLOAD_A, EV_UPLOAD_B, EV_DRAW_PACKETS,
        EV_88F64, EV_ARCH_POLL, EV_FIRST_WDS, EV_ARCH_INDEX, EV_TERRAIN,
        EV_CD_WORK, EV_VSYNC, EV_DISTANCE, EV_CD_WORK, EV_VSYNC,
        EV_DISTANCE, EV_READY, EV_AUDIO, EV_CONV_P1, EV_CONV_P2,
        EV_TAIL_P0, EV_TAIL_P1, EV_TAIL_P2, EV_TAIL_P3, EV_TAIL_P4,
        EV_TAIL_P5
    };
    int i;
    check(event_count == (int)(sizeof(expected) / sizeof(expected[0])),
          "event_count");
    for (i = 0; i < event_count && i < (int)(sizeof(expected) / sizeof(expected[0])); i++) {
        if (events[i] != expected[i]) {
            fprintf(stderr, "ASSERTION retail_order FAILED index=%d got=%d expected=%d\n",
                    i, events[i], expected[i]);
            failures++;
            break;
        }
    }
}

int main(void)
{
    memset(g_PsxRam, 0, sizeof(g_PsxRam));
    write32(0x8009C894u, 0u);
    write16(0x8006EE6Au, 0u);
    check(wm_80072238() == 0, "fresh_session_return");
    check_order();
    check(distance_calls == 2, "cd_drain_repeats");

    event_count = 0;
    distance_calls = 0;
    write32(0x8009C894u, 1u);
    check(wm_80072238() == -2, "restore_branch_fails_loud");
    check(event_count == 10, "restore_branch_stops_before_fresh_cleanup");

    if (failures != 0)
        return 1;
    puts("W34N9 SLOT1 OWNER CERTIFICATE PASS");
    return 0;
}
