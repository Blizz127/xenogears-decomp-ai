/*
 * World-map camera-from-angles helper 0x80097440.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x80097440, 0x8009766C).  See world_map_helper_97440.h.
 *
 * Scratch (guest):
 *   0x1F8000A0  negated eye SVECTOR
 *   0x1F8000F0  RotMatrixX / ApplyMatrix MATRIX
 *   0x1F800110  RotMatrixY MATRIX
 *   0x1F800130  RotMatrixZ MATRIX
 *   0x1F800150  MulMatrix0 temp MATRIX
 *   0x1F800000  ApplyMatrix VECTOR out / TransMatrix in
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_97440.h"

#define W440_SRC_MATRIX  0x8009A180u
#define W440_DST_MATRIX  0x8009C808u
#define W440_ANG_X       0x8009BD38u
#define W440_ANG_Y       0x8009BD3Au
#define W440_ANG_Z       0x8009BD3Cu
#define W440_SC_VOUT     0x1F800000u
#define W440_SC_SVEC     0x1F8000A0u
#define W440_SC_MX       0x1F8000F0u
#define W440_SC_MY       0x1F800110u
#define W440_SC_MZ       0x1F800130u
#define W440_SC_MT       0x1F800150u

typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} Wm97440Svector;

typedef struct {
    s32 vx;
    s32 vy;
    s32 vz;
    s32 pad;
} Wm97440Vector;

typedef struct {
    s16 m[3][3];
    s32 t[3];
} Wm97440Matrix;

extern Wm97440Matrix *RotMatrixX(s32 r, Wm97440Matrix *m);
extern Wm97440Matrix *RotMatrixY(s32 r, Wm97440Matrix *m);
extern Wm97440Matrix *RotMatrixZ(s32 r, Wm97440Matrix *m);
extern Wm97440Matrix *MulMatrix0(Wm97440Matrix *m0, Wm97440Matrix *m1,
                                 Wm97440Matrix *m2);
extern Wm97440Vector *ApplyMatrix(Wm97440Matrix *m, Wm97440Svector *v0,
                                  Wm97440Vector *v1);
extern Wm97440Matrix *TransMatrix(Wm97440Matrix *m, Wm97440Vector *v);

#if defined(WM_97440_TEST_TRACE)
extern void wm_97440_test_store(u32 address, u32 value);
#define W440_TRACE_STORE(a, v) wm_97440_test_store((a), (v))
#else
#define W440_TRACE_STORE(a, v) ((void)0)
#endif

static u32 w440_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static u32 w440_lhu(u32 a)
{
    u16 h;

    memcpy(&h, PSX_ADDR(a), 2);
    return (u32)h;
}

static s32 w440_lh(u32 a)
{
    s16 h;

    memcpy(&h, PSX_ADDR(a), 2);
    return (s32)h;
}

static void w440_sh(u32 a, u32 v)
{
    u16 h = (u16)(v & 0xFFFFu);

    memcpy(PSX_ADDR(a), &h, 2);
    W440_TRACE_STORE(a, (u32)h);
}

static void w440_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    W440_TRACE_STORE(a, v);
}

static void w440_copy_matrix(u32 dst, u32 src)
{
    u32 i;

#if defined(WM_97440_MUTANT_COPY7)
    for (i = 0; i < 7u; i++)
#else
    for (i = 0; i < 8u; i++)
#endif
        w440_sw(dst + i * 4u, w440_lw(src + i * 4u));
}

void wm_80097440(u32 eye_addr)
{
    Wm97440Matrix *mx = (Wm97440Matrix *)PSX_ADDR(W440_SC_MX);
    Wm97440Matrix *my = (Wm97440Matrix *)PSX_ADDR(W440_SC_MY);
    Wm97440Matrix *mz = (Wm97440Matrix *)PSX_ADDR(W440_SC_MZ);
    Wm97440Matrix *mt = (Wm97440Matrix *)PSX_ADDR(W440_SC_MT);
    Wm97440Matrix *dst = (Wm97440Matrix *)PSX_ADDR(W440_DST_MATRIX);
    s32 ang_x;
    u32 neg_x;
    u32 neg_y;
    u32 neg_z;

#if !defined(WM_97440_MUTANT_SKIP_SRC_COPY)
    w440_copy_matrix(W440_SC_MX, W440_SRC_MATRIX);
    w440_copy_matrix(W440_SC_MY, W440_SRC_MATRIX);
    w440_copy_matrix(W440_SC_MZ, W440_SRC_MATRIX);
#endif

#if defined(WM_97440_MUTANT_WRONG_ANG_X)
    ang_x = w440_lh(W440_ANG_Y);
#else
    ang_x = w440_lh(W440_ANG_X);
#endif
#if defined(WM_97440_MUTANT_SKIP_NEGU_X)
    (void)RotMatrixX(ang_x, mx);
#else
    (void)RotMatrixX(-ang_x, mx);
#endif

#if !defined(WM_97440_MUTANT_SKIP_ROTY)
    {
        s32 ang_y = w440_lh(W440_ANG_Y);

        (void)RotMatrixY(-ang_y, my);
    }
#endif

#if !defined(WM_97440_MUTANT_SKIP_ROTZ)
    {
        s32 ang_z = w440_lh(W440_ANG_Z);

        (void)RotMatrixZ(-ang_z, mz);
    }
#endif

#if defined(WM_97440_MUTANT_SWAP_MUL0)
    (void)MulMatrix0(my, mx, mt);
#else
    (void)MulMatrix0(mx, my, mt);
#endif
#if !defined(WM_97440_MUTANT_SKIP_MUL1)
#if defined(WM_97440_MUTANT_WRONG_DST)
    (void)MulMatrix0(mz, mt, mx);
#else
    (void)MulMatrix0(mz, mt, dst);
#endif
#endif

#if defined(WM_97440_MUTANT_SKIP_NEGU_EYE)
    neg_x = w440_lhu(eye_addr);
    neg_y = w440_lhu(eye_addr + 2u);
    neg_z = w440_lhu(eye_addr + 4u);
#else
    /* lhu then negu; sh keeps the low 16 bits. */
    neg_x = 0u - w440_lhu(eye_addr);
    neg_y = 0u - w440_lhu(eye_addr + 2u);
    neg_z = 0u - w440_lhu(eye_addr + 4u);
#endif
    w440_sh(W440_SC_SVEC, neg_x);
    w440_sh(W440_SC_SVEC + 2u, neg_y);
    w440_sh(W440_SC_SVEC + 4u, neg_z);

#if !defined(WM_97440_MUTANT_SKIP_DST_COPY)
    w440_copy_matrix(W440_SC_MX, W440_DST_MATRIX);
#endif

#if !defined(WM_97440_MUTANT_SKIP_APPLY)
    (void)ApplyMatrix(mx, (Wm97440Svector *)PSX_ADDR(W440_SC_SVEC),
                      (Wm97440Vector *)PSX_ADDR(W440_SC_VOUT));
#endif
#if !defined(WM_97440_MUTANT_SKIP_TRANS)
    (void)TransMatrix(dst, (Wm97440Vector *)PSX_ADDR(W440_SC_VOUT));
#endif
}
