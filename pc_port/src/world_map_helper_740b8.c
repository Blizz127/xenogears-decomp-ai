/*
 * World-map POLY_G3 / bit-sprite submitter 0x800740B8.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x800740B8, 0x80074594).  See world_map_helper_740b8.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_740b8.h"

#define W740_INDEX        0x8009D7F0u
#define W740_ANGLE        0x8009BD3Au
#define W740_POS          0x8009D55Cu
#define W740_POS_Z        0x8009D564u
#define W740_BCDC         0x8009BCDCu
#define W740_BE0C         0x8009BE0Cu
#define W740_DB           0x8009BE3Cu
#define W740_VERTS        0x8009A340u
#define W740_G3           0x8009C664u
#define W740_C5A0         0x8009C5A0u
#define W740_C5C0         0x8009C5C0u
#define W740_F3           0x8009C898u
#define W740_XY_TAB       0x8009B6F4u
#define W740_BITS         0x8007F160u
#define W740_CASE18       0x8007EE60u
#define W740_CASE19       0x8007EE82u
#define W740_CASE1A       0x8007EE78u
#define W740_SC           0x1F800000u
#define W740_SC_ANG       0x1F8000B8u
#define W740_SC_MTX       0x1F8000F0u
#define W740_SC_X         0x1F800104u
#define W740_SC_Y         0x1F800108u
#define W740_SC_Z         0x1F80010Cu
#define W740_OT_HI        0xFF000000u
#define W740_OT_LO        0x00FFFFFFu
#define W740_MAGIC1       0xD00D00D1u
#define W740_MAGIC2       0x300C0301u
#define W740_UDIV_X       0xA01A01A1u
#define W740_UDIV_Y       0x80601807u

typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} Wm740Svector;

typedef struct {
    s16 m[3][3];
    s32 t[3];
} Wm740Matrix;

extern Wm740Matrix *RotMatrix(Wm740Svector *r, Wm740Matrix *m);
extern void SetRotMatrix(Wm740Matrix *m);
extern void SetTransMatrix(Wm740Matrix *m);

#if defined(WM_740B8_TEST_TRACE)
extern void wm_740b8_test_rtpt(u32 v0, u32 v1, u32 v2, u32 sxy0, u32 sxy1,
                               u32 sxy2);
extern void wm_740b8_test_store(u32 address, u32 value);
#define W740_TRACE_STORE(a, v) wm_740b8_test_store((a), (v))
#else
#define W740_TRACE_STORE(a, v) ((void)0)
#endif

static u32 w740_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static u32 w740_lhu(u32 a)
{
    u16 h;

    memcpy(&h, PSX_ADDR(a), 2);
    return (u32)h;
}

static void w740_sh(u32 a, u32 v)
{
    u16 h = (u16)(v & 0xFFFFu);

    memcpy(PSX_ADDR(a), &h, 2);
}

static void w740_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    W740_TRACE_STORE(a, v);
}

static s32 w740_bits_as_s32(u32 value)
{
    s32 result;

    memcpy(&result, &value, sizeof(result));
    return result;
}

static u32 w740_s32_as_bits(s32 value)
{
    u32 result;

    memcpy(&result, &value, sizeof(result));
    return result;
}

static s32 w740_sra(s32 value, u32 shift)
{
    u32 amount = shift & 31u;
    u32 bits = w740_s32_as_bits(value);
    u32 result;

    if (amount == 0u)
        return value;
    result = bits >> amount;
    if (value < 0)
        result |= UINT32_MAX << (32u - amount);
    return w740_bits_as_s32(result);
}

static s32 w740_mult_hi(s32 a, s32 b)
{
    s64 prod = (s64)a * (s64)b;

    return (s32)(prod >> 32);
}

static u32 w740_multu_hi(u32 a, u32 b)
{
    return (u32)(((u64)a * (u64)b) >> 32);
}

static u32 w740_udiv_screen(u32 value, u32 magic, u32 addend)
{
    u32 hi = w740_multu_hi(value, magic);
    u32 tmp = (value - hi) >> 1;

    return ((hi + tmp) >> 8) + addend;
}

static void w740_ot_insert(u32 ot_slot, u32 packet)
{
#if defined(WM_740B8_MUTANT_SKIP_OT)
    (void)ot_slot;
    (void)packet;
#else
    u32 head = w740_lw(ot_slot);
    u32 tag = w740_lw(packet);

#if defined(WM_740B8_MUTANT_WRONG_OT_MASK)
    w740_sw(packet, (tag & 0xFFFF0000u) | (head & 0x0000FFFFu));
    w740_sw(ot_slot, (head & 0xFFFF0000u) | (packet & 0x0000FFFFu));
#else
    w740_sw(packet, (tag & W740_OT_HI) | (head & W740_OT_LO));
    w740_sw(ot_slot, (head & W740_OT_HI) | (packet & W740_OT_LO));
#endif
#endif
}

static void w740_write_magic(void)
{
    s32 pos_x = w740_bits_as_s32(w740_lw(W740_POS));
    s32 pos_z = w740_bits_as_s32(w740_lw(W740_POS_Z));
    s32 n;
    s32 hi;
    u32 x;
    u32 y;

#if defined(WM_740B8_MUTANT_WRONG_SRA)
    n = w740_sra(pos_x, 8u);
#else
    n = w740_sra(pos_x, 12u);
#endif
    hi = w740_mult_hi(n, w740_bits_as_s32(W740_MAGIC1));
    x = w740_s32_as_bits(w740_sra(
        w740_bits_as_s32((w740_s32_as_bits(hi) + w740_s32_as_bits(n))), 8u));
    x = w740_s32_as_bits(w740_bits_as_s32(x) - w740_sra(pos_x, 31u));
    x = w740_s32_as_bits(w740_bits_as_s32(x) + 0x30);

#if defined(WM_740B8_MUTANT_WRONG_SRA)
    n = w740_sra(pos_z, 8u);
#else
    n = w740_sra(pos_z, 12u);
#endif
    hi = w740_mult_hi(n, w740_bits_as_s32(W740_MAGIC2));
    y = w740_s32_as_bits(w740_sra(hi, 6u));
    y = w740_s32_as_bits(w740_bits_as_s32(y) - w740_sra(pos_z, 31u));
    y = w740_s32_as_bits(w740_bits_as_s32(y) + 0x78);
    y = w740_s32_as_bits(w740_bits_as_s32(y) -
                         w740_bits_as_s32(w740_lw(W740_BE0C)));

#if !defined(WM_740B8_MUTANT_SKIP_MAGIC)
    w740_sw(W740_SC_X, x);
    w740_sw(W740_SC_Z, w740_lw(W740_BCDC));
    w740_sw(W740_SC_Y, y);
#endif
}

#if !defined(WM_740B8_TEST_TRACE)
#include <psx/inline_c.h>

static void w740_rtpt_sxy(u32 v0, u32 v1, u32 v2, u32 sxy0, u32 sxy1, u32 sxy2)
{
    gte_ldv0(PSX_ADDR(v0));
    gte_ldv1(PSX_ADDR(v1));
    gte_ldv2(PSX_ADDR(v2));
    gte_rtpt();
    gte_stsxy3(PSX_ADDR(sxy0), PSX_ADDR(sxy1), PSX_ADDR(sxy2));
}
#endif

static void w740_project_tri(u32 verts, u32 packet)
{
#if defined(WM_740B8_MUTANT_SKIP_SETMAT)
    (void)0;
#else
    SetRotMatrix((Wm740Matrix *)PSX_ADDR(W740_SC_MTX));
    SetTransMatrix((Wm740Matrix *)PSX_ADDR(W740_SC_MTX));
#endif
#if defined(WM_740B8_MUTANT_SKIP_RTPT)
    (void)verts;
    (void)packet;
#elif defined(WM_740B8_TEST_TRACE)
#if defined(WM_740B8_MUTANT_SKIP_SXY)
    (void)packet;
    wm_740b8_test_rtpt(verts, verts + 8u, verts + 0x10u, W740_SC, W740_SC + 4u,
                       W740_SC + 8u);
#else
    wm_740b8_test_rtpt(verts, verts + 8u, verts + 0x10u, packet + 8u,
                       packet + 0x10u, packet + 0x18u);
#endif
#elif defined(WM_740B8_MUTANT_SKIP_SXY)
    w740_rtpt_sxy(verts, verts + 8u, verts + 0x10u, W740_SC, W740_SC + 4u,
                  W740_SC + 8u);
#else
    w740_rtpt_sxy(verts, verts + 8u, verts + 0x10u, packet + 8u, packet + 0x10u,
                  packet + 0x18u);
#endif
}

void wm_800740B8(void)
{
    u32 index = w740_lw(W740_INDEX);
    u32 db = w740_lw(W740_DB);
    u32 ot;
    u32 packet;
    u32 verts;
    u32 bits;
    u32 xy;
    u32 i;
    u32 ntris;
    u32 nbits;

#if defined(WM_740B8_MUTANT_WRONG_INDEX_MUL)
    packet = W740_G3 + (index << 6);
#else
    packet = W740_G3 + (((index << 3) - index) << 4);
#endif
#if defined(WM_740B8_MUTANT_WRONG_VERT_BASE)
    verts = W740_VERTS + 8u;
#else
    verts = W740_VERTS;
#endif
#if defined(WM_740B8_MUTANT_WRONG_TRI_COUNT)
    ntris = 3u;
#else
    ntris = 4u;
#endif

    for (i = 0u; i < ntris; i++) {
#if defined(WM_740B8_MUTANT_WRONG_ANGLE)
        w740_sh(W740_SC_ANG, w740_lhu(W740_ANGLE));
        w740_sh(W740_SC_ANG + 2u, 0u);
        w740_sh(W740_SC_ANG + 4u, 0u);
#else
        w740_sh(W740_SC_ANG, 0u);
        w740_sh(W740_SC_ANG + 2u, 0u);
        w740_sh(W740_SC_ANG + 4u, w740_lhu(W740_ANGLE));
#endif
#if !defined(WM_740B8_MUTANT_SKIP_ROT)
        (void)RotMatrix((Wm740Svector *)PSX_ADDR(W740_SC_ANG),
                        (Wm740Matrix *)PSX_ADDR(W740_SC_MTX));
#endif
        w740_write_magic();
        w740_project_tri(verts, packet);
        ot = w740_lw(db + 0x70u);
        w740_ot_insert(ot, packet);
#if defined(WM_740B8_MUTANT_WRONG_VERT_STRIDE)
        verts += 0x10u;
#else
        verts += 0x18u;
#endif
#if defined(WM_740B8_MUTANT_WRONG_PKT_STRIDE)
        packet += 0x20u;
#else
        packet += 0x1Cu;
#endif
    }

#if !defined(WM_740B8_MUTANT_SKIP_C5A0)
    ot = w740_lw(db + 0x70u);
    w740_ot_insert(ot, W740_C5A0);
#endif

    bits = w740_lw(W740_BITS);
    xy = W740_XY_TAB;
#if defined(WM_740B8_MUTANT_WRONG_F3_MUL)
    packet = W740_F3 + (index << 8);
#else
    packet = W740_F3 + (index << 9);
#endif
#if defined(WM_740B8_MUTANT_WRONG_BIT_LOOP)
    nbits = 0x10u;
#else
    nbits = 0x20u;
#endif

    for (i = 0u; i < nbits; i++) {
#if defined(WM_740B8_MUTANT_SKIP_BITMASK)
        if (((bits & 1u) != 0u) && 0) {
#elif defined(WM_740B8_MUTANT_FORCE_ALL_BITS)
        if (((bits & 1u) != 0u) || 1) {
#else
        if ((bits & 1u) != 0u) {
#endif
            u32 x;
            u32 y;
            u32 src;

#if defined(WM_740B8_MUTANT_WRONG_CASE)
            src = 0u;
#else
            src = i;
#endif
            if (src == 0x18u) {
                x = w740_udiv_screen(w740_lhu(W740_CASE18), W740_UDIV_X, 0xCFu);
                y = w740_udiv_screen(w740_lhu(W740_CASE18 + 4u), W740_UDIV_Y,
                                     0x77u);
            } else if (src == 0x19u) {
                x = w740_udiv_screen(w740_lhu(W740_CASE19), W740_UDIV_X, 0xCFu);
                y = w740_udiv_screen(w740_lhu(W740_CASE19 + 4u), W740_UDIV_Y,
                                     0x77u);
            } else if (src == 0x1Au) {
                x = w740_udiv_screen(w740_lhu(W740_CASE1A), W740_UDIV_X, 0xCFu);
                y = w740_udiv_screen(w740_lhu(W740_CASE1A + 2u), W740_UDIV_Y,
                                     0x77u);
            } else {
#if defined(WM_740B8_MUTANT_WRONG_DEFAULT_ADD)
                x = w740_lhu(xy) + 0xD1u;
                y = w740_lhu(xy + 2u) + 0x79u;
#else
                x = w740_lhu(xy) + 0xD0u;
                y = w740_lhu(xy + 2u) + 0x78u;
#endif
            }
            w740_sh(packet + 8u, x);
            w740_sh(packet + 0xAu, y);
            ot = w740_lw(db + 0x70u);
            w740_ot_insert(ot, packet);
        }
        bits >>= 1;
        xy += 4u;
        packet += 0x10u;
    }

    ot = w740_lw(db + 0x70u);
#if defined(WM_740B8_MUTANT_WRONG_FT4_STRIDE)
    w740_ot_insert(ot, W740_C5C0 + index * 0x20u);
#else
    w740_ot_insert(ot, W740_C5C0 + (((index << 2) + index) << 3));
#endif
}
