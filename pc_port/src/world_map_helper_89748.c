/*
 * World-map 512-emitter particle spawner 0x80089748.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x80089748, 0x80089C78).  See world_map_helper_89748.h.
 *
 * Scratch (guest):
 *   0x1F800000  VectorNormal out / velocity workspace
 *   0x1F800010  random VECTOR in / third VectorNormal out
 *   0x1F800020  ApplyMatrix VECTOR out
 *   0x1F8000F0  RotMatrixYXZ MATRIX
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_89580.h"
#include "world_map_helper_89748.h"

#define W748_TAB       0x8009BCC0u
#define W748_REC       0x8009BDF4u
#define W748_SC        0x1F800000u
#define W748_SC_RAND   0x1F800010u
#define W748_SC_APPLY  0x1F800020u
#define W748_SC_MTX    0x1F8000F0u
#define W748_EMIT_N    0x200u
#define W748_EMIT_STR  0x54u
#define W748_SLOT_N    0x100u
#define W748_SLOT_STR  0x4Cu

typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} Wm748Svector;

typedef struct {
    s32 vx;
    s32 vy;
    s32 vz;
    s32 pad;
} Wm748Vector;

typedef struct {
    s16 m[3][3];
    s32 t[3];
} Wm748Matrix;

extern Wm748Matrix *RotMatrixYXZ(Wm748Svector *r, Wm748Matrix *m);
extern Wm748Vector *ApplyMatrix(Wm748Matrix *m, Wm748Svector *v0,
                                Wm748Vector *v1);
extern Wm748Vector *VectorNormal(Wm748Vector *v0, Wm748Vector *v1);
extern long ratan2(long y, long x);
extern int rand(void);

#if defined(WM_89748_TEST_TRACE)
extern void wm_89748_test_store(u32 address, u32 value);
#define W748_TRACE_STORE(a, v) wm_89748_test_store((a), (v))
#else
#define W748_TRACE_STORE(a, v) ((void)0)
#endif

static u32 w748_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static s32 w748_lh(u32 a)
{
    s16 h;

    memcpy(&h, PSX_ADDR(a), 2);
    return (s32)h;
}

static u32 w748_lhu(u32 a)
{
    u16 h;

    memcpy(&h, PSX_ADDR(a), 2);
    return (u32)h;
}

static u32 w748_lbu(u32 a)
{
    u8 b;

    memcpy(&b, PSX_ADDR(a), 1);
    return (u32)b;
}

static void __attribute__((unused)) w748_sb(u32 a, u32 v)
{
    u8 b = (u8)(v & 0xFFu);

    memcpy(PSX_ADDR(a), &b, 1);
    W748_TRACE_STORE(a, (u32)b);
}

static void w748_sh(u32 a, u32 v)
{
    u16 h = (u16)(v & 0xFFFFu);

    memcpy(PSX_ADDR(a), &h, 2);
    W748_TRACE_STORE(a, (u32)h);
}

static void w748_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    W748_TRACE_STORE(a, v);
}

static s32 w748_bits_as_s32(u32 value)
{
    s32 result;

    memcpy(&result, &value, sizeof(result));
    return result;
}

static u32 w748_s32_as_bits(s32 value)
{
    u32 result;

    memcpy(&result, &value, sizeof(result));
    return result;
}

static s32 w748_sra(s32 value, u32 shift)
{
    u32 amount = shift & 31u;
    u32 bits = w748_s32_as_bits(value);
    u32 result;

    if (amount == 0u)
        return value;
    result = bits >> amount;
    if (value < 0)
        result |= UINT32_MAX << (32u - amount);
    return w748_bits_as_s32(result);
}

static s32 w748_mult_lo(s32 a, s32 b)
{
    u64 prod = (u64)(s64)a * (u64)(s64)b;
    u32 lo = (u32)prod;
    s32 result;

    memcpy(&result, &lo, 4);
    return result;
}

static s32 w748_rem(s32 num, s32 den)
{
    if (den == 0 || (num == (s32)0x80000000 && den == -1))
        return 0;
    return num % den;
}

static s32 w748_rand_dir(void)
{
    return (s32)((u32)rand() & 0xFFFu) - 2048;
}

static void w748_store_word(u32 s1, s32 fp, s32 s5)
{
    w748_sw(s1, ((u32)fp << 16) | w748_s32_as_bits(s5));
}

static void w748_spawn(u32 emit, u32 s1, u32 rec, u32 flags, u32 index)
{
#if defined(WM_89748_MUTANT_SKIP_SPAWN)
    (void)emit;
    (void)s1;
    (void)rec;
    (void)flags;
    (void)index;
    return;
#endif
    s32 a1;
    s32 vx;
    s32 vy;
    s32 vz;
    s32 px;
    s32 py;
    s32 pz;
    s32 scale;
    long ang;
    u32 packed;

    w748_sh(rec, index);
    w748_sw(rec + 4u, w748_lw(s1 + 8u));
#if !defined(WM_89748_MUTANT_SKIP_ROT)
    (void)RotMatrixYXZ((Wm748Svector *)PSX_ADDR(emit + 0x1Cu),
                       (Wm748Matrix *)PSX_ADDR(W748_SC_MTX));
#endif
#if !defined(WM_89748_MUTANT_SKIP_APPLY1)
    (void)ApplyMatrix((Wm748Matrix *)PSX_ADDR(W748_SC_MTX),
                      (Wm748Svector *)PSX_ADDR(emit + 0x24u),
                      (Wm748Vector *)PSX_ADDR(W748_SC_APPLY));
#endif

#if defined(WM_89748_MUTANT_SKIP_POS_Y)
    w748_sw(W748_SC + 0x14u, 0u);
#else
    if ((flags & 0x20u) == 0u)
        w748_sw(W748_SC + 0x14u, w748_s32_as_bits(w748_rand_dir()));
    else
        w748_sw(W748_SC + 0x14u, 0u);
#endif
    w748_sw(W748_SC + 0x10u, w748_s32_as_bits(w748_rand_dir()));
    w748_sw(W748_SC + 0x18u, w748_s32_as_bits(w748_rand_dir()));
#if !defined(WM_89748_MUTANT_SKIP_NORM1)
    (void)VectorNormal((Wm748Vector *)PSX_ADDR(W748_SC_RAND),
                       (Wm748Vector *)PSX_ADDR(W748_SC));
#endif

#if defined(WM_89748_MUTANT_WRONG_MOD1)
    if ((flags & 0x04u) == 0u)
#else
    if ((flags & 0x04u) != 0u)
#endif
        a1 = (s32)w748_lhu(s1 + 0x3Cu);
    else
        a1 = w748_rem(rand(), (s32)w748_lhu(s1 + 0x3Cu));

#if defined(WM_89748_MUTANT_WRONG_POS_SHIFT)
    px = (w748_bits_as_s32(w748_lw(W748_SC_APPLY)) << 8) +
         w748_mult_lo(w748_bits_as_s32(w748_lw(W748_SC)), a1) +
         (w748_lh(s1 + 0x10u) << 8);
    py = (w748_bits_as_s32(w748_lw(W748_SC_APPLY + 4u)) << 8) +
         w748_mult_lo(w748_bits_as_s32(w748_lw(W748_SC + 4u)), a1) +
         (w748_lh(s1 + 0x12u) << 8);
    pz = (w748_bits_as_s32(w748_lw(W748_SC_APPLY + 8u)) << 8) +
         w748_mult_lo(w748_bits_as_s32(w748_lw(W748_SC + 8u)), a1) +
         (w748_lh(s1 + 0x14u) << 8);
#else
    px = (w748_bits_as_s32(w748_lw(W748_SC_APPLY)) << 12) +
         w748_mult_lo(w748_bits_as_s32(w748_lw(W748_SC)), a1) +
         (w748_lh(s1 + 0x10u) << 12);
    py = (w748_bits_as_s32(w748_lw(W748_SC_APPLY + 4u)) << 12) +
         w748_mult_lo(w748_bits_as_s32(w748_lw(W748_SC + 4u)), a1) +
         (w748_lh(s1 + 0x12u) << 12);
    pz = (w748_bits_as_s32(w748_lw(W748_SC_APPLY + 8u)) << 12) +
         w748_mult_lo(w748_bits_as_s32(w748_lw(W748_SC + 8u)), a1) +
         (w748_lh(s1 + 0x14u) << 12);
#endif
    w748_sw(rec + 8u, w748_s32_as_bits(px));
    w748_sw(rec + 0xCu, w748_s32_as_bits(py));
    w748_sw(rec + 0x10u, w748_s32_as_bits(pz));

#if !defined(WM_89748_MUTANT_SKIP_APPLY2)
    (void)ApplyMatrix((Wm748Matrix *)PSX_ADDR(W748_SC_MTX),
                      (Wm748Svector *)PSX_ADDR(emit + 0x2Cu),
                      (Wm748Vector *)PSX_ADDR(W748_SC_APPLY));
#endif

#if defined(WM_89748_MUTANT_SKIP_VEL_Y)
    w748_sw(W748_SC + 0x14u, 0u);
#else
    if ((flags & 0x40u) == 0u)
        w748_sw(W748_SC + 0x14u, w748_s32_as_bits(w748_rand_dir()));
    else
        w748_sw(W748_SC + 0x14u, 0u);
#endif
    w748_sw(W748_SC + 0x10u, w748_s32_as_bits(w748_rand_dir()));
    w748_sw(W748_SC + 0x18u, w748_s32_as_bits(w748_rand_dir()));
#if !defined(WM_89748_MUTANT_SKIP_NORM2)
    (void)VectorNormal((Wm748Vector *)PSX_ADDR(W748_SC_RAND),
                       (Wm748Vector *)PSX_ADDR(W748_SC));
#endif

    if ((flags & 0x08u) != 0u)
        a1 = (s32)w748_lhu(s1 + 0x3Eu);
    else
        a1 = w748_rem(rand(), (s32)w748_lhu(s1 + 0x3Eu));

    vx = (w748_bits_as_s32(w748_lw(W748_SC_APPLY)) << 12) +
         w748_mult_lo(w748_bits_as_s32(w748_lw(W748_SC)), a1) +
         (w748_lh(s1 + 0x10u) << 12);
    vy = (w748_bits_as_s32(w748_lw(W748_SC_APPLY + 4u)) << 12) +
         w748_mult_lo(w748_bits_as_s32(w748_lw(W748_SC + 4u)), a1) +
         (w748_lh(s1 + 0x12u) << 12);
    vz = (w748_bits_as_s32(w748_lw(W748_SC_APPLY + 8u)) << 12) +
         w748_mult_lo(w748_bits_as_s32(w748_lw(W748_SC + 8u)), a1) +
         (w748_lh(s1 + 0x14u) << 12);
#if !defined(WM_89748_MUTANT_SKIP_VEL_SUB)
#if defined(WM_89748_MUTANT_WRONG_VEL_SRA)
    vx = w748_sra(vx - px, 8u);
    vy = w748_sra(vy - py, 8u);
    vz = w748_sra(vz - pz, 8u);
#else
    vx = w748_sra(vx - px, 12u);
    vy = w748_sra(vy - py, 12u);
    vz = w748_sra(vz - pz, 12u);
#endif
#endif
    w748_sw(W748_SC, w748_s32_as_bits(vx));
    w748_sw(W748_SC + 4u, w748_s32_as_bits(vy));
    w748_sw(W748_SC + 8u, w748_s32_as_bits(vz));
#if !defined(WM_89748_MUTANT_SKIP_NORM3)
    (void)VectorNormal((Wm748Vector *)PSX_ADDR(W748_SC),
                       (Wm748Vector *)PSX_ADDR(W748_SC_RAND));
#endif

    scale = w748_bits_as_s32(w748_lw(s1 + 0x30u));
#if defined(WM_89748_MUTANT_WRONG_SCALE_SRA)
    w748_sw(rec + 0x18u,
            w748_s32_as_bits(w748_sra(
                w748_mult_lo(w748_bits_as_s32(w748_lw(W748_SC_RAND)), scale),
                8u)));
    w748_sw(rec + 0x1Cu,
            w748_s32_as_bits(w748_sra(
                w748_mult_lo(w748_bits_as_s32(w748_lw(W748_SC_RAND + 4u)),
                             scale),
                8u)));
    w748_sw(rec + 0x20u,
            w748_s32_as_bits(w748_sra(
                w748_mult_lo(w748_bits_as_s32(w748_lw(W748_SC_RAND + 8u)),
                             scale),
                8u)));
#else
    w748_sw(rec + 0x18u,
            w748_s32_as_bits(w748_sra(
                w748_mult_lo(w748_bits_as_s32(w748_lw(W748_SC_RAND)), scale),
                12u)));
    w748_sw(rec + 0x1Cu,
            w748_s32_as_bits(w748_sra(
                w748_mult_lo(w748_bits_as_s32(w748_lw(W748_SC_RAND + 4u)),
                             scale),
                12u)));
    w748_sw(rec + 0x20u,
            w748_s32_as_bits(w748_sra(
                w748_mult_lo(w748_bits_as_s32(w748_lw(W748_SC_RAND + 8u)),
                             scale),
                12u)));
#endif

#if !defined(WM_89748_MUTANT_SKIP_RATAN2)
    ang = ratan2((long)w748_bits_as_s32(w748_lw(W748_SC_RAND + 4u)),
                 (long)w748_bits_as_s32(w748_lw(W748_SC_RAND)));
#else
    ang = 0;
#endif
    w748_sh(rec + 2u, (u32)ang);

    w748_sw(rec + 0x28u, w748_s32_as_bits(w748_lh(s1 + 0x34u)));
    w748_sw(rec + 0x2Cu, w748_s32_as_bits(w748_lh(s1 + 0x36u)));
    w748_sw(rec + 0x30u, w748_s32_as_bits(w748_lh(s1 + 0x38u)));
    w748_sw(rec + 0x40u, w748_lw(s1 + 0x48u));
    w748_sw(rec + 0x44u, w748_lw(s1 + 0x4Cu));
    w748_sw(rec + 0x38u, w748_lw(s1 + 0x40u));
#if defined(WM_89748_MUTANT_WRONG_PACK)
    packed = flags & 3u;
#else
    packed = ((flags & 3u) << 5) | 0x9Du;
#endif
    w748_sh(rec + 0x48u, packed);
    w748_sw(rec + 0x3Cu, w748_lw(s1 + 0x44u));
#if !defined(WM_89748_MUTANT_SKIP_COUNT_INC)
    w748_sh(s1 + 6u, (u32)(w748_lh(s1 + 6u) + 1));
#endif
}

void wm_80089748(void)
{
    u32 tab = w748_lw(W748_TAB);
    u32 recs = w748_lw(W748_REC);
    s32 s5 = 0;
    s32 fp = 0;
    u32 i;
    u32 n;
    u32 stride;

#if defined(WM_89748_MUTANT_WRONG_COUNT)
    n = 1u;
#else
    n = W748_EMIT_N;
#endif
#if defined(WM_89748_MUTANT_WRONG_STRIDE)
    stride = 0x50u;
#else
    stride = W748_EMIT_STR;
#endif

    for (i = 0u; i < n; i++) {
        u32 emit = tab + i * stride;
        u32 s1 = emit + 4u;
        u32 flags_u = w748_lbu(s1 + 0x4Bu);
        u32 s4 = flags_u & 0xFFu;
        s32 interval;
        s32 limit;
        s32 count;
        s32 life_hi;
        u32 slot;
        u32 slot_str;

#if defined(WM_89748_MUTANT_SKIP_FLAG80)
        if (0 && (s4 & 0x80u) == 0u)
            continue;
#else
        if ((s4 & 0x80u) == 0u)
            continue;
#endif

#if defined(WM_89748_MUTANT_SKIP_BIT10)
        if (0 && (s4 & 0x10u) != 0u) {
#else
        if ((s4 & 0x10u) != 0u) {
#endif
            u32 word = w748_lw(s1);

            fp = w748_sra(w748_bits_as_s32(word), 16u);
            s5 = w748_sra(w748_bits_as_s32(word << 16), 16u);
            if (s5 != 0) {
                s5 -= 1;
                goto store_word;
            }
            if (fp == 0) {
#if !defined(WM_89748_MUTANT_SKIP_DEACTIVATE)
                w748_sb(s1 + 0x4Bu, flags_u ^ 0x80u);
#endif
                goto store_word;
            }
            fp -= 1;
        }

        interval = w748_lh(s1 + 0x0Eu);
#if defined(WM_89748_MUTANT_SKIP_INTERVAL)
        if (0 && interval != 0) {
#else
        if (interval != 0) {
#endif
            w748_sh(s1 + 0x0Eu, (u32)(interval - 1));
            goto store_word;
        }

        w748_sh(s1 + 0x0Eu, w748_lhu(s1 + 0x0Cu));
        limit = w748_lh(s1 + 4u);
        count = w748_lh(s1 + 6u);
        life_hi = w748_lh(s1 + 0xAu);
        if (!((u32)life_hi > 0u && limit > 0 && count < limit))
            goto store_word;

#if defined(WM_89748_MUTANT_WRONG_SLOT_STRIDE)
        slot_str = 0x40u;
#else
        slot_str = W748_SLOT_STR;
#endif
        for (slot = 0u; slot < W748_SLOT_N; slot++) {
            u32 rec = recs + slot * slot_str;

#if defined(WM_89748_MUTANT_SKIP_FREE_CHECK)
            if (0 && w748_lh(rec + 6u) != 0)
                continue;
#else
            if (w748_lh(rec + 6u) != 0)
                continue;
#endif
            w748_spawn(emit, s1, rec, s4, i);
            break;
        }

    store_word:
        w748_store_word(s1, fp, s5);
    }

#if !defined(WM_89748_MUTANT_SKIP_89580)
    wm_80089580();
#endif
}
