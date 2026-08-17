/*
 * World-map heading/pose transform helper 0x80096F18.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x80096F18, 0x80097070).  See world_map_helper_96f18.h.
 *
 * Scratch (guest):
 *   0x1F8000A0  RotMatrixYXZ / ApplyMatrix SVECTOR
 *   0x1F8000F0  MATRIX
 *   0x1F800000  ApplyMatrixLV VECTOR in  (0, 0, -(scale>>12))
 *   0x1F800010  ApplyMatrixLV VECTOR out
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_96f18.h"

#define W618_SC_VIN     0x1F800000u
#define W618_SC_VOUT    0x1F800010u
#define W618_SC_SVEC    0x1F8000A0u
#define W618_SC_MAT     0x1F8000F0u

typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} Wm96f18Svector;

typedef struct {
    s32 vx;
    s32 vy;
    s32 vz;
    s32 pad;
} Wm96f18Vector;

typedef struct {
    s16 m[3][3];
    s32 t[3];
} Wm96f18Matrix;

extern Wm96f18Matrix *RotMatrixYXZ(Wm96f18Svector *r, Wm96f18Matrix *m);
extern Wm96f18Vector *ApplyMatrixLV(Wm96f18Matrix *m, Wm96f18Vector *v0,
                                    Wm96f18Vector *v1);
extern Wm96f18Vector *ApplyMatrix(Wm96f18Matrix *m, Wm96f18Svector *v0,
                                  Wm96f18Vector *v1);

#if defined(WM_96F18_TEST_TRACE)
extern void wm_96f18_test_store(u32 address, u32 value);
#define W618_TRACE_STORE(a, v) wm_96f18_test_store((a), (v))
#else
#define W618_TRACE_STORE(a, v) ((void)0)
#endif

static u32 w618_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static u32 w618_lhu(u32 a)
{
    u16 h;

    memcpy(&h, PSX_ADDR(a), 2);
    return (u32)h;
}

static void w618_sh(u32 a, u32 v)
{
    u16 h = (u16)(v & 0xFFFFu);

    memcpy(PSX_ADDR(a), &h, 2);
    W618_TRACE_STORE(a, (u32)h);
}

static void w618_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    W618_TRACE_STORE(a, v);
}

static s32 w618_s32(u32 v)
{
    s32 r;

    memcpy(&r, &v, 4);
    return r;
}

void wm_80096F18(u32 dest_addr, u32 pose_addr, s32 scale, u32 angles_addr)
{
    Wm96f18Svector *r = (Wm96f18Svector *)PSX_ADDR(W618_SC_SVEC);
    Wm96f18Matrix *m = (Wm96f18Matrix *)PSX_ADDR(W618_SC_MAT);
    Wm96f18Vector *vin = (Wm96f18Vector *)PSX_ADDR(W618_SC_VIN);
    Wm96f18Vector *vout = (Wm96f18Vector *)PSX_ADDR(W618_SC_VOUT);
    s32 pose_y;
    s32 z_in;

#if defined(WM_96F18_MUTANT_SKIP_PAD)
    (void)dest_addr;
#else
    w618_sh(dest_addr + 8u, 0u);
    w618_sh(dest_addr + 0xCu, 0u);
#endif

#if defined(WM_96F18_MUTANT_WRONG_POSE_OFF)
    pose_y = w618_s32(w618_lw(pose_addr));
#else
    pose_y = w618_s32(w618_lw(pose_addr + 4u));
#endif
#if defined(WM_96F18_MUTANT_POSE_SRA11)
    pose_y >>= 11;
#else
    pose_y >>= 12;
#endif
    w618_sh(dest_addr + 0xAu, (u32)pose_y);

    w618_sh(W618_SC_SVEC, w618_lhu(angles_addr));
#if defined(WM_96F18_MUTANT_FIRST_VZ)
    w618_sh(W618_SC_SVEC + 4u, w618_lhu(angles_addr + 4u));
#else
    w618_sh(W618_SC_SVEC + 4u, 0u);
#endif
    w618_sh(W618_SC_SVEC + 2u, w618_lhu(angles_addr + 2u));

#if !defined(WM_96F18_MUTANT_SKIP_ROT1)
    (void)RotMatrixYXZ(r, m);
#endif

#if defined(WM_96F18_MUTANT_SCALE_SRA11)
    z_in = scale >> 11;
#else
    z_in = scale >> 12;
#endif
#if !defined(WM_96F18_MUTANT_SKIP_NEGU)
    z_in = -z_in;
#endif
    w618_sw(W618_SC_VIN, 0u);
    w618_sw(W618_SC_VIN + 4u, 0u);
    w618_sw(W618_SC_VIN + 8u, (u32)z_in);

#if !defined(WM_96F18_MUTANT_SKIP_AMLV)
    (void)ApplyMatrixLV(m, vin, vout);
#else
    (void)vin;
#endif

#if !defined(WM_96F18_MUTANT_SKIP_DEST_XZ)
    w618_sh(dest_addr, (u32)vout->vx);
    w618_sh(dest_addr + 4u, (u32)vout->vz);
#endif
#if !defined(WM_96F18_MUTANT_SKIP_DEST_Y)
    w618_sh(dest_addr + 2u, w618_lhu(dest_addr + 0xAu) + (u32)vout->vy);
#endif

    w618_sh(W618_SC_SVEC, 0u);
    w618_sh(W618_SC_SVEC + 2u, w618_lhu(angles_addr + 2u));
    w618_sh(W618_SC_SVEC + 4u, w618_lhu(angles_addr + 4u));
#if defined(WM_96F18_MUTANT_SECOND_VX)
    w618_sh(W618_SC_SVEC, w618_lhu(angles_addr));
#endif

#if !defined(WM_96F18_MUTANT_SKIP_ROT2)
    (void)RotMatrixYXZ(r, m);
#endif

    w618_sh(W618_SC_SVEC, 0u);
#if defined(WM_96F18_MUTANT_APPLY_VY)
    w618_sh(W618_SC_SVEC + 2u, 0x1000u);
#else
    w618_sh(W618_SC_SVEC + 2u, (u32)(u16)(s16)-0x1000);
#endif
    w618_sh(W618_SC_SVEC + 4u, 0u);

#if !defined(WM_96F18_MUTANT_SKIP_APPLY)
#if defined(WM_96F18_MUTANT_WRONG_OUT_OFF)
    (void)ApplyMatrix(m, r, (Wm96f18Vector *)PSX_ADDR(dest_addr + 8u));
#else
    (void)ApplyMatrix(m, r, (Wm96f18Vector *)PSX_ADDR(dest_addr + 0x10u));
#endif
#endif
}
