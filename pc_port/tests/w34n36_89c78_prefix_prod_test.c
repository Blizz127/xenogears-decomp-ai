/* W34N36 — production-linked certificate for retail wm_80089C78's prefix.
 *
 * The retail loop reads 256 x 0x4c records through the pointer at 0x8009BDF4,
 * skips zero state halfwords at record+6, builds a camera-relative VECTOR in
 * scratchpad at 0x1f800088, and passes that scratch address to wm_80093534.
 * The fixture makes the obsolete fixed 0x8009B040 interpretation live at the
 * exact record whose +0x30 aliases allocator global 0x8009D7EC. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "psyq/libgte.h"
#include "world_map_helper_89c78.h"

#define SCRATCH       0x1F800000u
#define CAMERA_MATRIX 0x8009C808u
#define MODEL_MATRIX  0x8009A180u
#define FIXED_TABLE   0x8009B040u
#define POOL_GLOBAL   0x8009BDF4u
#define CAMERA_X      0x8009BE28u
#define CAMERA_Z      0x8009BE30u
#define WRAP_X        0x8009D160u
#define WRAP_Z        0x8009D2B4u
#define ALLOC_A       0x8009D7E8u
#define ALLOC_B       0x8009D7ECu
#define POOL           0x80110000u
#define COUNT          256u
#define STRIDE         0x4Cu

static int s_pass;
static int s_total;
static int s_rot_calls;
static int s_scale_calls;
static int s_load_count;
static int s_store_count;
static int s_asserted;
static int s_rot_angle;
static u32 s_load_addr[32];
static u32 s_store_addr[16];
static u32 s_store_value[16];
static VECTOR s_scales[4];
static u8 s_matrix_at_rot[32];
static u8 s_matrix_at_scale[4][32];

static void st8(u32 address, u8 value)
{
    memcpy(PSX_ADDR(address), &value, sizeof(value));
}

static void st16(u32 address, u16 value)
{
    memcpy(PSX_ADDR(address), &value, sizeof(value));
}

static void st32(u32 address, u32 value)
{
    memcpy(PSX_ADDR(address), &value, sizeof(value));
}

static u32 ld32(u32 address)
{
    u32 value;
    memcpy(&value, PSX_ADDR(address), sizeof(value));
    return value;
}

static void check(const char *name, int ok)
{
    s_total++;
    if (ok) {
        s_pass++;
        printf("PASS %s\n", name);
    } else {
        fprintf(stderr, "ASSERTION %s\n", name);
        s_asserted = 1;
    }
}

MATRIX *RotMatrixZ(int angle, MATRIX *matrix)
{
    s_rot_calls++;
    s_rot_angle = angle;
    memcpy(s_matrix_at_rot, matrix, sizeof(s_matrix_at_rot));
    return matrix;
}

MATRIX *ScaleMatrix(MATRIX *matrix, VECTOR *scale)
{
    int slot = s_scale_calls;
    if (slot < 4) {
        s_scales[slot] = *scale;
        memcpy(s_matrix_at_scale[slot], matrix,
               sizeof(s_matrix_at_scale[slot]));
    }
    s_scale_calls++;
    return matrix;
}

void wm_93534_test_load(u32 address, u32 value)
{
    (void)value;
    if (s_load_count < 32)
        s_load_addr[s_load_count] = address;
    s_load_count++;
}

void wm_93534_test_store(u32 address, u32 value)
{
    if (s_store_count < 16) {
        s_store_addr[s_store_count] = address;
        s_store_value[s_store_count] = value;
    }
    s_store_count++;
}

static void init_record(u32 index, u16 state, s16 heading, u8 flags,
                        u16 scale_x, u16 scale_y,
                        s32 x, s32 y, s32 z)
{
    u32 record = POOL + index * STRIDE;
    st16(record + 2u, (u16)heading);
    st16(record + 6u, state);
    st32(record + 8u, (u32)x);
    st32(record + 0x0Cu, (u32)y);
    st32(record + 0x10u, (u32)z);
    st16(record + 0x38u, scale_x);
    st16(record + 0x3Au, scale_y);
    st8(record + 0x47u, flags);
}

static void reset_fixture(void)
{
    u8 camera[32];
    u8 model[32];
    u32 wrong_record = FIXED_TABLE + 133u * STRIDE;
    u32 i;

    memset(g_PsxRam, 0, PSX_RAM_SIZE);
    memset(g_PsxScratchpad, 0, 4096u);
    memset(s_load_addr, 0, sizeof(s_load_addr));
    memset(s_store_addr, 0, sizeof(s_store_addr));
    memset(s_store_value, 0, sizeof(s_store_value));
    memset(s_scales, 0, sizeof(s_scales));
    memset(s_matrix_at_rot, 0, sizeof(s_matrix_at_rot));
    memset(s_matrix_at_scale, 0, sizeof(s_matrix_at_scale));
    s_rot_calls = 0;
    s_scale_calls = 0;
    s_load_count = 0;
    s_store_count = 0;
    s_asserted = 0;
    s_rot_angle = 0;

    for (i = 0u; i < 32u; i++) {
        camera[i] = (u8)(0x40u + i);
        model[i] = (u8)(0x80u + i);
    }
    memcpy(PSX_ADDR(CAMERA_MATRIX), camera, sizeof(camera));
    memcpy(PSX_ADDR(MODEL_MATRIX), model, sizeof(model));
    st32(POOL_GLOBAL, POOL);
    st32(CAMERA_X, (u32)(1000 * 4096));
    st32(CAMERA_Z, (u32)(-500 * 4096));
    st32(WRAP_X, 10u); /* period 20480 */
    st32(WRAP_Z, 12u); /* period 24576 */
    st32(ALLOC_A, 0x801C3A7Cu);
    st32(ALLOC_B, 0x801BEA74u);

    /* Two live records and one deliberately tempting inactive record. */
    init_record(7u, 3u, (s16)0x0123, 1u, 0x1111u, 0x2222u,
                (s32)(20000 * 4096), (s32)(7 * 4096),
                (s32)(-20000 * 4096));
    init_record(8u, 0u, (s16)0x0555, 1u, 0x3333u, 0x4444u,
                (s32)(25000 * 4096), (s32)(99 * 4096),
                (s32)(25000 * 4096));
    init_record(9u, 5u, (s16)0x0666, 0u, 0x5555u, 0x6666u,
                (s32)(-20000 * 4096), (s32)(-9 * 4096),
                (s32)(20000 * 4096));

    /* Old fixed-base record 133 is the exact alias witness:
     * wrong_record+0x30 == 0x8009D7EC. */
    st16(wrong_record + 6u, 1u);
}

static int address_sequence_is_scratch(void)
{
    int i;
    if (s_load_count != 8)
        return 0;
    for (i = 0; i < 8; i += 4) {
        if (s_load_addr[i] != SCRATCH + 0x88u ||
                s_load_addr[i + 1] != WRAP_X ||
                s_load_addr[i + 2] != SCRATCH + 0x90u ||
                s_load_addr[i + 3] != WRAP_Z)
            return 0;
    }
    return 1;
}

static void run_certificate(void)
{
    u8 pool_before[COUNT * STRIDE];
    u8 camera[32];
    u8 model[32];
    u8 scratch_camera[32];
    u8 scratch_model[32];

    reset_fixture();
    memcpy(pool_before, PSX_ADDR(POOL), sizeof(pool_before));
    memcpy(camera, PSX_ADDR(CAMERA_MATRIX), sizeof(camera));
    memcpy(model, PSX_ADDR(MODEL_MATRIX), sizeof(model));

    wm_80089C78(0xDEADBEEFu);

    memcpy(scratch_camera, PSX_ADDR(SCRATCH + 0x28u),
           sizeof(scratch_camera));
    memcpy(scratch_model, PSX_ADDR(SCRATCH + 0x68u),
           sizeof(scratch_model));
    check("camera-matrix-copied-to-retail-scratch",
          memcmp(camera, scratch_camera, sizeof(camera)) == 0);
    check("model-matrix-copied-to-retail-scratch",
          memcmp(model, scratch_model, sizeof(model)) == 0);
    check("dynamic-BDF4-pool-and-nonzero-state-guard",
          s_scale_calls == 2 && s_rot_calls == 1);
    check("rotation-angle-and-order",
          s_rot_angle == 0x0123 &&
          memcmp(s_matrix_at_rot, model, sizeof(model)) == 0 &&
          memcmp(s_matrix_at_scale[0], model, sizeof(model)) == 0 &&
          memcmp(s_matrix_at_scale[1], model, sizeof(model)) == 0);
    check("scale-vector-is-retail-32-bit-layout",
          s_scales[0].vx == 0x1111 && s_scales[0].vy == 0x2222 &&
          s_scales[0].vz == 0x1000 && s_scales[0].pad == 0 &&
          s_scales[1].vx == 0x5555 && s_scales[1].vy == 0x6666 &&
          s_scales[1].vz == 0x1000 && s_scales[1].pad == 0);
    check("wrap-loads-only-retail-scratch-vector",
          address_sequence_is_scratch());
    check("camera-relative-fields-and-wrap-results",
          s_store_count == 4 &&
          s_store_addr[0] == SCRATCH + 0x88u &&
          s_store_value[0] == (u32)-1480 &&
          s_store_addr[1] == SCRATCH + 0x90u &&
          s_store_value[1] == (u32)5076 &&
          s_store_addr[2] == SCRATCH + 0x88u &&
          s_store_value[2] == (u32)-520 &&
          s_store_addr[3] == SCRATCH + 0x90u &&
          s_store_value[3] == (u32)-4076 &&
          ld32(SCRATCH + 0x8Cu) == (u32)-9);
    check("object-pool-is-read-only",
          memcmp(pool_before, PSX_ADDR(POOL), sizeof(pool_before)) == 0);
    check("allocator-globals-not-aliased",
          ld32(ALLOC_A) == 0x801C3A7Cu &&
          ld32(ALLOC_B) == 0x801BEA74u);
}

int main(void)
{
    run_certificate();
    if (s_asserted != 0 || s_pass != s_total)
        return EXIT_FAILURE;
    printf("W34N36 0x80089C78 retail-prefix certificate PASS\n");
    return EXIT_SUCCESS;
}
