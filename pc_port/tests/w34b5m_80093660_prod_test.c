/*
 * W34B5-M — Production-linked test for world terrain cell lookup 0x80093660.
 *
 * Links the ACTUAL production helper (pc_port/src/world_map_terrain_cell.c).
 * Uses an independent oracle that computes expected guest pointers from the
 * retail formula WITHOUT calling the production helper.
 *
 * Build:
 *   gcc -std=gnu17 -O0 -g -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C \
 *     -Ipc_port/include -Iinclude -Ipc_port/src \
 *     pc_port/tests/w34b5m_80093660_prod_test.c \
 *     pc_port/src/world_map_terrain_cell.c \
 *     -o pc_port/build_native/w34b5m_80093660_prod_test
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "psx_memory.h"
#include "world_map_terrain_cell.h"

uint8_t g_PsxRam[PSX_RAM_SIZE];

void PsxMemory_Init(void)
{
    memset(g_PsxRam, 0, sizeof(g_PsxRam));
}

/* ---- Independent oracle (never calls wm_80093660) ---- */

#define TERRAIN_STRIDE_ADDR  0x8009D160u
#define TERRAIN_TABLE_ADDR   0x8009C184u

/* Mirrors the retail formula exactly, including MIPS branch-delay ordering.
 * Key: mult v0,v1 is in delay slot of bgez a2, so the multiply uses
 * coarse_z (÷8'd) but raw coarse_x (not yet adjusted or ÷8'd).
 * The addu that combines product + coarse_x comes AFTER coarse_x is
 * adjusted and ÷8'd. */
static u32 oracle_80093660(s32 x, s32 z)
{
    s32 coarse_z = z >> 20;
    if (coarse_z < 0) coarse_z += 7;
    coarse_z >>= 3;

    s32 coarse_x = x >> 20;

    s32 stride = *(s32*)PSX_ADDR(TERRAIN_STRIDE_ADDR);
    s64 prod = (s64)coarse_z * (s64)stride;
    s32 product_lo = (s32)(u32)prod;

    if (coarse_x < 0) coarse_x += 7;
    coarse_x >>= 3;

    s32 tile_idx = (s32)((u32)product_lo + (u32)coarse_x);

    s32 local_x = (x >> 12) & 0x7FF;
    s32 local_z = (z >> 12) & 0x7FF;

    s32 quadrant = 0;
    if (local_x >= 1024) { quadrant = 1; local_x -= 1024; }
    if (local_z >= 1024) { quadrant |= 2; local_z -= 1024; }

    s32 cell_z = local_z >> 4;
    if (cell_z < 0) cell_z += 7;
    cell_z >>= 3;

    s32 cell_x = local_x >> 4;
    if (cell_x < 0) cell_x += 7;
    cell_x >>= 3;

    s32 cell_idx = cell_z * 9 + cell_x;
    s32 tile_idx_s16 = (s32)(s16)(u16)(u32)tile_idx;
    u32 quadrant_base = *(u32*)PSX_ADDR(TERRAIN_TABLE_ADDR + (u32)(tile_idx_s16 << 2));

    return quadrant_base + (u32)(cell_idx * 4) + (u32)(quadrant * 0x144);
}

/* ---- Fixture setup ---- */

/* Terrain patch: 4 quadrants × 9×9 cells × 4 bytes = 0x510 bytes.
 * Place at a high guest address to test guest-pointer behavior. */
#define PATCH_GUEST_ADDR  0x801C0000u
#define PATCH_SIZE        0x510  /* 4 * 0x144 */

static void setup_fixture(s32 stride, u32 patch_guest_addr, s32 table_index)
{
    memset(g_PsxRam, 0, PSX_RAM_SIZE);
    *(s32*)PSX_ADDR(TERRAIN_STRIDE_ADDR) = stride;

    /* Place patch guest address in table at given index. */
    u32 entry_addr = TERRAIN_TABLE_ADDR + (u32)(table_index * 4);
    *(u32*)PSX_ADDR(entry_addr) = patch_guest_addr;

    /* Fill patch with known pattern. */
    u8* patch = (u8*)PSX_ADDR(patch_guest_addr);
    for (int i = 0; i < PATCH_SIZE; i++)
        patch[i] = (u8)(i & 0xFF);
}

static int verify_no_writes(u32 patch_guest_addr)
{
    u8* patch = (u8*)PSX_ADDR(patch_guest_addr);
    for (int i = 0; i < PATCH_SIZE; i++) {
        if (patch[i] != (u8)(i & 0xFF))
            return 0;
    }
    if (*(s32*)PSX_ADDR(TERRAIN_STRIDE_ADDR) == 0)
        return 0;
    return 1;
}

/* ---- Test infrastructure ---- */

static int s_pass, s_fail;

#define CHECK(desc, expected, actual) do { \
    if ((expected) == (actual)) { \
        s_pass++; \
    } else { \
        s_fail++; \
        fprintf(stderr, "FAIL: %s\n  expected: 0x%08X\n  actual:   0x%08X\n", \
                desc, (u32)(expected), (u32)(actual)); \
    } \
} while(0)

int main(void)
{
    PsxMemory_Init();

    fprintf(stderr, "=== W34B5-M: wm_80093660 production-linked test ===\n\n");

    /*
     * Coordinate guide:
     *   local = (coord >> 12) & 0x7FF
     *   cell  = (local >> 4) >> 3 = local / 128
     *   Each cell is 128 local-units wide.
     *   Quadrant boundary at local = 1024.
     *   To set local=L, use coord = L * 4096.
     */

    /* Case 1: Interior cell (2,2) in quadrant 0.
     * x = 257*4096 = 0x101000 → local_x=257, cell_x=2
     * z = 258*4096 = 0x102000 → local_z=258, cell_z=2
     * cell_idx = 2*9+2 = 20 */
    {
        setup_fixture(32, PATCH_GUEST_ADDR, 0);
        s32 x = 257 * 4096, z = 258 * 4096;
        u32 expected = PATCH_GUEST_ADDR + 20 * 4;
        u32 actual = wm_80093660(x, z);
        CHECK("Case 1: interior cell (2,2) quad 0", expected, actual);
        CHECK("Case 1: oracle match", expected, oracle_80093660(x, z));
        CHECK("Case 1: no-write proof", 1, verify_no_writes(PATCH_GUEST_ADDR));
    }

    /* Case 2: Adjacent X cell → cell (3,2).
     * x = 384*4096 = 0x180000 → local_x=384, cell_x=3 */
    {
        setup_fixture(32, PATCH_GUEST_ADDR, 0);
        s32 x = 384 * 4096, z = 258 * 4096;
        u32 expected = PATCH_GUEST_ADDR + (2 * 9 + 3) * 4;
        u32 actual = wm_80093660(x, z);
        CHECK("Case 2: adjacent X cell (3,2) quad 0", expected, actual);
        CHECK("Case 2: oracle match", expected, oracle_80093660(x, z));
    }

    /* Case 3: Adjacent Z cell → cell (2,3).
     * z = 384*4096 = 0x180000 → local_z=384, cell_z=3 */
    {
        setup_fixture(32, PATCH_GUEST_ADDR, 0);
        s32 x = 257 * 4096, z = 384 * 4096;
        u32 expected = PATCH_GUEST_ADDR + (3 * 9 + 2) * 4;
        u32 actual = wm_80093660(x, z);
        CHECK("Case 3: adjacent Z cell (2,3) quad 0", expected, actual);
        CHECK("Case 3: oracle match", expected, oracle_80093660(x, z));
    }

    /* Case 4: Exact X cell boundary (127→128). */
    {
        setup_fixture(32, PATCH_GUEST_ADDR, 0);
        /* local_x = 127 → cell_x = (127>>4)>>3 = 7>>3 = 0 */
        s32 x = 127 * 4096, z = 0;
        u32 actual4a = wm_80093660(x, z);
        CHECK("Case 4a: local_x=127 → cell_x=0", PATCH_GUEST_ADDR + 0, actual4a);

        /* local_x = 128 → cell_x = (128>>4)>>3 = 8>>3 = 1 */
        x = 128 * 4096;
        u32 actual4b = wm_80093660(x, z);
        CHECK("Case 4b: local_x=128 → cell_x=1", PATCH_GUEST_ADDR + 1 * 4, actual4b);
    }

    /* Case 5: Exact Z cell boundary. */
    {
        setup_fixture(32, PATCH_GUEST_ADDR, 0);
        s32 x = 0, z = 127 * 4096;
        u32 actual5a = wm_80093660(x, z);
        CHECK("Case 5a: local_z=127 → cell_z=0", PATCH_GUEST_ADDR + 0, actual5a);

        z = 128 * 4096;
        u32 actual5b = wm_80093660(x, z);
        CHECK("Case 5b: local_z=128 → cell_z=1", PATCH_GUEST_ADDR + 9 * 4, actual5b);
    }

    /* Case 6: Quadrant X transition (1023→1024). */
    {
        setup_fixture(32, PATCH_GUEST_ADDR, 0);
        /* local_x = 1023: quad=0, cell_x = (1023>>4)>>3 = 63>>3 = 7 */
        s32 x = 1023 * 4096, z = 0;
        u32 actual6a = wm_80093660(x, z);
        CHECK("Case 6a: local_x=1023 quad=0 cell_x=7",
              PATCH_GUEST_ADDR + 7 * 4, actual6a);

        /* local_x = 1024: quad=1, cell_x = 0 */
        x = 1024 * 4096;
        u32 actual6b = wm_80093660(x, z);
        CHECK("Case 6b: local_x=1024 quad=1 cell_x=0",
              PATCH_GUEST_ADDR + 0x144 + 0, actual6b);
    }

    /* Case 7: Quadrant Z transition (1023→1024). */
    {
        setup_fixture(32, PATCH_GUEST_ADDR, 0);
        s32 x = 0, z = 1023 * 4096;
        u32 actual7a = wm_80093660(x, z);
        CHECK("Case 7a: local_z=1023 quad=0 cell_z=7",
              PATCH_GUEST_ADDR + 7 * 9 * 4, actual7a);

        z = 1024 * 4096;
        u32 actual7b = wm_80093660(x, z);
        CHECK("Case 7b: local_z=1024 quad=2 cell_z=0",
              PATCH_GUEST_ADDR + 2 * 0x144, actual7b);
    }

    /* Case 8: Negative X (-4096).
     * x>>20 = -1, +7 = 6, >>3 = 0 → coarse_x = 0
     * local_x = (-4096>>12)&0x7FF = (-1)&0x7FF = 2047
     * quad: 2047>=1024 → quad=1, local=1023, cell_x=7 */
    {
        setup_fixture(32, PATCH_GUEST_ADDR, 0);
        s32 x = -4096, z = 0;
        u32 expected = PATCH_GUEST_ADDR + 0x144 + 7 * 4;
        u32 actual = wm_80093660(x, z);
        CHECK("Case 8: negative X (quad=1, cell_x=7)", expected, actual);
        CHECK("Case 8: oracle match", expected, oracle_80093660(x, z));
    }

    /* Case 9: Negative Z (-4096). */
    {
        setup_fixture(32, PATCH_GUEST_ADDR, 0);
        s32 x = 0, z = -4096;
        u32 expected = PATCH_GUEST_ADDR + 2 * 0x144 + 7 * 9 * 4;
        u32 actual = wm_80093660(x, z);
        CHECK("Case 9: negative Z (quad=2, cell_z=7)", expected, actual);
        CHECK("Case 9: oracle match", expected, oracle_80093660(x, z));
    }

    /* Case 10: Both negative. */
    {
        setup_fixture(32, PATCH_GUEST_ADDR, 0);
        s32 x = -4096, z = -4096;
        u32 expected = PATCH_GUEST_ADDR + 3 * 0x144 + (7 * 9 + 7) * 4;
        u32 actual = wm_80093660(x, z);
        CHECK("Case 10: both negative (quad=3)", expected, actual);
        CHECK("Case 10: oracle match", expected, oracle_80093660(x, z));
    }

    /* Case 11: Signed16 tile index (bit 15 set).
     * stride=32768, coarse_z=1, coarse_x=0 → tile_idx=32768=0x8000.
     * (s16)0x8000 = -32768 → table at 0x8009C184 + (-32768*4) = 0x8007C184.
     *
     * coarse = (coord>>20 + [7 if negative]) >> 3.
     * For coarse_z=1: need z>>20 = 8 → z = 8<<20 = 0x800000.
     * local_z = (0x800000>>12)&0x7FF = 0x800&0x7FF = 0. cell_z=0.
     * For coarse_x=0: x=0. local_x=0, cell_x=0.
     * cell_idx = 0*9+0 = 0. */
    {
        memset(g_PsxRam, 0, PSX_RAM_SIZE);
        *(s32*)PSX_ADDR(TERRAIN_STRIDE_ADDR) = 32768;
        *(u32*)PSX_ADDR(0x8007C184u) = PATCH_GUEST_ADDR;
        u8* patch = (u8*)PSX_ADDR(PATCH_GUEST_ADDR);
        for (int i = 0; i < PATCH_SIZE; i++) patch[i] = (u8)(i & 0xFF);

        s32 cx = 0, cz = 8 << 20;
        u32 expected = PATCH_GUEST_ADDR + 0;
        u32 actual = wm_80093660(cx, cz);
        CHECK("Case 11: signed16 tile index (bit15 set)", expected, actual);
        CHECK("Case 11: oracle match", expected, oracle_80093660(cx, cz));
    }

    /* Case 12: Zero terrain table entry. */
    {
        memset(g_PsxRam, 0, PSX_RAM_SIZE);
        *(s32*)PSX_ADDR(TERRAIN_STRIDE_ADDR) = 32;
        s32 x = 257 * 4096, z = 258 * 4096;
        u32 expected = 0 + 20 * 4;
        u32 actual = wm_80093660(x, z);
        CHECK("Case 12: zero table entry → offset only", expected, actual);
    }

    /* Case 13: Smallest coordinate (x=1, z=1). */
    {
        setup_fixture(32, PATCH_GUEST_ADDR, 0);
        s32 x = 1, z = 1;
        /* local = (1>>12)&0x7FF = 0, cell = 0 */
        u32 expected = PATCH_GUEST_ADDR + 0;
        u32 actual = wm_80093660(x, z);
        CHECK("Case 13: smallest coordinate (1,1)", expected, actual);
    }

    /* Case 14: Max tile-0 coordinate.
     * local_x = 2047, local_z = 2047 → quad=3, cell (7,7) */
    {
        setup_fixture(32, PATCH_GUEST_ADDR, 0);
        s32 x = 2047 * 4096, z = 2047 * 4096;
        /* But coarse_x = (2047*4096)>>20 = 0x7FF000>>20 = 0 (tile 0). */
        u32 expected = PATCH_GUEST_ADDR + 3 * 0x144 + (7 * 9 + 7) * 4;
        u32 actual = wm_80093660(x, z);
        CHECK("Case 14: max tile-0 (quad=3, cell 7,7)", expected, actual);
        CHECK("Case 14: oracle match", expected, oracle_80093660(x, z));
    }

    /* Case 15: Non-unit stride. stride=100.
     * With delay-slot ordering: mult uses raw coarse_x=50 (before ÷8).
     * product = 0*100 = 0. Then coarse_x = 50>>3 = 6.
     * tile_idx = 0 + 6 = 6.
     * Store patch at table index 6.
     * local_x = ((50<<20)>>12)&0x7FF = 512. 512<1024 → quad=0.
     * cell_x = (512>>4)>>3 = 4.
     * cell_idx = 0*9+4 = 4. */
    {
        setup_fixture(100, PATCH_GUEST_ADDR, 6);
        s32 x = 50 << 20, z = 0;
        u32 expected = PATCH_GUEST_ADDR + 4 * 4;
        u32 actual = wm_80093660(x, z);
        CHECK("Case 15: stride=100, tile 6, cell_x=4", expected, actual);
        CHECK("Case 15: oracle match", expected, oracle_80093660(x, z));
    }

    /* Case 16: Negative coarse_z → 0 after +7>>3.
     * z = -(1<<20): z>>20 = -1, +7=6, >>3=0 */
    {
        setup_fixture(32, PATCH_GUEST_ADDR, 0);
        s32 x = 0, z = -(1 << 20);
        /* local_z = (z>>12)&0x7FF = (-256)&0x7FF = 0x700 = 1792.
         * 1792>=1024 → quad|=2, local=768, cell_z = (768>>4)>>3 = 48>>3 = 6 */
        u32 expected = PATCH_GUEST_ADDR + 2 * 0x144 + (6 * 9 + 0) * 4;
        u32 actual = wm_80093660(x, z);
        CHECK("Case 16: negative coarse_z (quad=2, cell_z=6)", expected, actual);
        CHECK("Case 16: oracle match", expected, oracle_80093660(x, z));
    }

    /* Case 17: Quadrant 3 (both upper). */
    {
        setup_fixture(32, PATCH_GUEST_ADDR, 0);
        s32 x = 1500 * 4096, z = 1500 * 4096;
        /* local_x = 1500, 1500>=1024 → quad=1, local=476, cell_x=(476>>4)>>3=29>>3=3
         * local_z = 1500, 1500>=1024 → quad=3, local=476, cell_z=3 */
        u32 expected = PATCH_GUEST_ADDR + 3 * 0x144 + (3 * 9 + 3) * 4;
        u32 actual = wm_80093660(x, z);
        CHECK("Case 17: quadrant 3 (both upper)", expected, actual);
        CHECK("Case 17: oracle match", expected, oracle_80093660(x, z));
    }

    /* Case 18: Negative coarse_x → tile_idx=-1.
     * x = -(9<<20): x>>20 = -9, +7 = -2, >>3 = -1 → coarse_x = -1.
     * tile_idx = 0*32 + (-1) = -1.
     * (u16)(-1) = 0xFFFF, (s16)0xFFFF = -1.
     * table at 0x8009C184 + (-1*4) = 0x8009C180.
     * local_x = (x>>12)&0x7FF = (-2304)&0x7FF = 1792.
     * 1792>=1024 → quad=1, local=768, cell_x = (768>>4)>>3 = 6.
     * cell_idx = 0*9+6 = 6. */
    {
        memset(g_PsxRam, 0, PSX_RAM_SIZE);
        *(s32*)PSX_ADDR(TERRAIN_STRIDE_ADDR) = 32;
        *(u32*)PSX_ADDR(0x8009C180u) = PATCH_GUEST_ADDR;
        u8* patch = (u8*)PSX_ADDR(PATCH_GUEST_ADDR);
        for (int i = 0; i < PATCH_SIZE; i++) patch[i] = (u8)(i & 0xFF);

        s32 x = -(9 << 20), z = 0;
        u32 expected = PATCH_GUEST_ADDR + 0x144 + 6 * 4;
        u32 actual = wm_80093660(x, z);
        CHECK("Case 18: negative coarse_x=-1 (quad=1, cell_x=6)", expected, actual);
        CHECK("Case 18: oracle match", expected, oracle_80093660(x, z));
    }

    /* ---- Results ---- */
    fprintf(stderr, "\n=== Results: %d PASS / %d TOTAL ===\n", s_pass, s_pass + s_fail);
    if (s_fail > 0)
        fprintf(stderr, "*** %d FAILURE(S) ***\n", s_fail);

    return s_fail > 0 ? 1 : 0;
}
