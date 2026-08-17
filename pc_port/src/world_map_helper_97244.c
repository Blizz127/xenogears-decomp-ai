/*
 * World-map camera-from-look helper 0x80097244.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x80097244, 0x80097440).  See world_map_helper_97244.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_97244.h"

#define W244_DST_MATRIX  0x8009C808u
#define W244_SC_V0       0x1F800000u
#define W244_SC_N2       0x1F800010u
#define W244_SC_N3       0x1F800020u
#define W244_SC_N1       0x1F800030u
#define W244_SC_SVEC     0x1F800040u
#define W244_SC_MAT      0x1F800048u

typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} Wm97244Svector;

typedef struct {
    s32 vx;
    s32 vy;
    s32 vz;
    s32 pad;
} Wm97244Vector;

typedef struct {
    s16 m[3][3];
    s32 t[3];
} Wm97244Matrix;

extern Wm97244Vector *VectorNormal(Wm97244Vector *v0, Wm97244Vector *v1);
extern void OuterProduct12(Wm97244Vector *v0, Wm97244Vector *v1,
                           Wm97244Vector *v2);
extern Wm97244Vector *ApplyMatrix(Wm97244Matrix *m, Wm97244Svector *v0,
                                  Wm97244Vector *v1);
extern Wm97244Matrix *TransMatrix(Wm97244Matrix *m, Wm97244Vector *v);

#if defined(WM_97244_TEST_TRACE)
extern void wm_97244_test_store(u32 address, u32 value);
#define W244_TRACE_STORE(a, v) wm_97244_test_store((a), (v))
#else
#define W244_TRACE_STORE(a, v) ((void)0)
#endif

static u32 w244_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static u32 w244_lhu(u32 a)
{
    u16 h;

    memcpy(&h, PSX_ADDR(a), 2);
    return (u32)h;
}

static s32 w244_lh(u32 a)
{
    s16 h;

    memcpy(&h, PSX_ADDR(a), 2);
    return (s32)h;
}

static void w244_sh(u32 a, u32 v)
{
    u16 h = (u16)(v & 0xFFFFu);

    memcpy(PSX_ADDR(a), &h, 2);
    W244_TRACE_STORE(a, (u32)h);
}

static void w244_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    W244_TRACE_STORE(a, v);
}

static Wm97244Vector *w244_vec(u32 a)
{
    return (Wm97244Vector *)PSX_ADDR(a);
}

void wm_80097244(u32 look_addr)
{
#if defined(WM_97244_MUTANT_SKIP_DELTA)
    (void)w244_lh(look_addr);
    w244_sw(W244_SC_V0, 0u);
    w244_sw(W244_SC_V0 + 4u, 0u);
    w244_sw(W244_SC_V0 + 8u, 0u);
#else
    w244_sw(W244_SC_V0, (u32)(w244_lh(look_addr + 8u) - w244_lh(look_addr)));
    w244_sw(W244_SC_V0 + 4u,
            (u32)(w244_lh(look_addr + 0xAu) - w244_lh(look_addr + 2u)));
    w244_sw(W244_SC_V0 + 8u,
            (u32)(w244_lh(look_addr + 0xCu) - w244_lh(look_addr + 4u)));
#endif

#if !defined(WM_97244_MUTANT_SKIP_VN1)
    (void)VectorNormal(w244_vec(W244_SC_V0), w244_vec(W244_SC_N1));
#endif
#if !defined(WM_97244_MUTANT_SKIP_OP1)
#if defined(WM_97244_MUTANT_WRONG_UP)
    OuterProduct12(w244_vec(W244_SC_N1), w244_vec(look_addr),
                   w244_vec(W244_SC_V0));
#else
    OuterProduct12(w244_vec(W244_SC_N1), w244_vec(look_addr + 0x10u),
                   w244_vec(W244_SC_V0));
#endif
#endif
#if !defined(WM_97244_MUTANT_SKIP_VN2)
    (void)VectorNormal(w244_vec(W244_SC_V0), w244_vec(W244_SC_N2));
#endif
#if !defined(WM_97244_MUTANT_SKIP_OP2)
    OuterProduct12(w244_vec(W244_SC_N1), w244_vec(W244_SC_N2),
                   w244_vec(W244_SC_V0));
#endif
#if !defined(WM_97244_MUTANT_SKIP_VN3)
    (void)VectorNormal(w244_vec(W244_SC_V0), w244_vec(W244_SC_N3));
#endif

#if !defined(WM_97244_MUTANT_SKIP_PACK)
    {
        u32 i;
        const u32 src[9] = {
            W244_SC_N2, W244_SC_N2 + 4u, W244_SC_N2 + 8u,
            W244_SC_N3, W244_SC_N3 + 4u, W244_SC_N3 + 8u,
            W244_SC_N1, W244_SC_N1 + 4u, W244_SC_N1 + 8u,
        };

        for (i = 0; i < 9u; i++)
            w244_sh(W244_DST_MATRIX + i * 2u, w244_lw(src[i]));
    }
#endif

#if defined(WM_97244_MUTANT_SKIP_NEGU_EYE)
    w244_sh(W244_SC_SVEC, w244_lhu(look_addr));
    w244_sh(W244_SC_SVEC + 2u, w244_lhu(look_addr + 2u));
    w244_sh(W244_SC_SVEC + 4u, w244_lhu(look_addr + 4u));
#else
    w244_sh(W244_SC_SVEC, 0u - w244_lhu(look_addr));
    w244_sh(W244_SC_SVEC + 2u, 0u - w244_lhu(look_addr + 2u));
    w244_sh(W244_SC_SVEC + 4u, 0u - w244_lhu(look_addr + 4u));
#endif

#if !defined(WM_97244_MUTANT_SKIP_MAT_COPY)
    {
        u32 i;

        for (i = 0; i < 8u; i++)
            w244_sw(W244_SC_MAT + i * 4u, w244_lw(W244_DST_MATRIX + i * 4u));
    }
#endif

#if !defined(WM_97244_MUTANT_SKIP_APPLY)
    (void)ApplyMatrix((Wm97244Matrix *)PSX_ADDR(W244_SC_MAT),
                      (Wm97244Svector *)PSX_ADDR(W244_SC_SVEC),
                      w244_vec(W244_SC_V0));
#endif
#if !defined(WM_97244_MUTANT_SKIP_TRANS)
    (void)TransMatrix((Wm97244Matrix *)PSX_ADDR(W244_DST_MATRIX),
                      w244_vec(W244_SC_V0));
#endif
}
