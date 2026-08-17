/*
 * World-map four-quad submitter 0x800737EC.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x800737EC, 0x800739B8).  See world_map_helper_737ec.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_737ec.h"

#define W7EC_ANGLE        0x8009BD3Au
#define W7EC_INDEX        0x8009D7F0u
#define W7EC_VERTEX       0x8009A280u
#define W7EC_PACKET       0x8009D194u
#define W7EC_CAMERA       0x8009C808u
#define W7EC_DB           0x8009BE3Cu
#define W7EC_SHIFT        0x80050100u
#define W7EC_SC_ANG       0x1F800000u
#define W7EC_SC_COMP      0x1F800008u
#define W7EC_SC_ROT       0x1F800028u
#define W7EC_SC_P         0x1F800048u
#define W7EC_SC_FLAG      0x1F80004Cu
#define W7EC_PTR_MASK     0x00FFFFFFu
#define W7EC_TAG_MASK     0xFF000000u

typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} Wm737ecSvector;

typedef struct {
    s16 m[3][3];
    s32 t[3];
} Wm737ecMatrix;

extern Wm737ecMatrix *RotMatrixYXZ(Wm737ecSvector *r, Wm737ecMatrix *m);
extern Wm737ecMatrix *CompMatrix(Wm737ecMatrix *m0, Wm737ecMatrix *m1,
                                 Wm737ecMatrix *m2);
extern void SetRotMatrix(Wm737ecMatrix *m);
extern void SetTransMatrix(Wm737ecMatrix *m);
extern s32 RotTransPers4(Wm737ecSvector *v0, Wm737ecSvector *v1,
                         Wm737ecSvector *v2, Wm737ecSvector *v3, s32 *sxy0,
                         s32 *sxy1, s32 *sxy2, s32 *sxy3, s32 *p, s32 *flag);

static u32 w7ec_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static u16 w7ec_lhu(u32 a)
{
    u16 h;

    memcpy(&h, PSX_ADDR(a), 2);
    return h;
}

static void w7ec_sh(u32 a, u32 v)
{
    u16 h = (u16)(v & 0xFFFFu);

    memcpy(PSX_ADDR(a), &h, 2);
}

static void w7ec_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
}

static s32 w7ec_bits_as_s32(u32 value)
{
    s32 result;

    memcpy(&result, &value, sizeof(result));
    return result;
}

static u32 w7ec_s32_as_bits(s32 value)
{
    u32 result;

    memcpy(&result, &value, sizeof(result));
    return result;
}

static s32 w7ec_srav(s32 value, u32 shift)
{
    u32 amount = shift & 31u;
    u32 bits = w7ec_s32_as_bits(value);
    u32 result;

    if (amount == 0u)
        return value;
    result = bits >> amount;
    if (value < 0)
        result |= UINT32_MAX << (32u - amount);
    return w7ec_bits_as_s32(result);
}

void wm_800737EC(void)
{
    u16 theta = w7ec_lhu(W7EC_ANGLE);
    u32 index = w7ec_lw(W7EC_INDEX);
    u32 quad;
    u32 nquads;

#if defined(WM_737EC_MUTANT_WRONG_ANGLE)
    w7ec_sh(W7EC_SC_ANG, theta);
    w7ec_sh(W7EC_SC_ANG + 2u, 0u);
#else
    w7ec_sh(W7EC_SC_ANG, 0u);
#endif
    w7ec_sh(W7EC_SC_ANG + 4u, 0u);
#if !defined(WM_737EC_MUTANT_WRONG_ANGLE)
    w7ec_sh(W7EC_SC_ANG + 2u, theta);
#endif

#if !defined(WM_737EC_MUTANT_SKIP_ROT)
    (void)RotMatrixYXZ((Wm737ecSvector *)PSX_ADDR(W7EC_SC_ANG),
                       (Wm737ecMatrix *)PSX_ADDR(W7EC_SC_ROT));
#endif

#if !defined(WM_737EC_MUTANT_SKIP_T_CLEAR)
    w7ec_sw(W7EC_SC_ROT + 0x14u, 0u);
    w7ec_sw(W7EC_SC_ROT + 0x18u, 0u);
    w7ec_sw(W7EC_SC_ROT + 0x1Cu, 0u);
#endif

#if defined(WM_737EC_MUTANT_SWAP_COMP)
    (void)CompMatrix((Wm737ecMatrix *)PSX_ADDR(W7EC_SC_ROT),
                     (Wm737ecMatrix *)PSX_ADDR(W7EC_CAMERA),
                     (Wm737ecMatrix *)PSX_ADDR(W7EC_SC_COMP));
#else
    (void)CompMatrix((Wm737ecMatrix *)PSX_ADDR(W7EC_CAMERA),
                     (Wm737ecMatrix *)PSX_ADDR(W7EC_SC_ROT),
                     (Wm737ecMatrix *)PSX_ADDR(W7EC_SC_COMP));
#endif

#if !defined(WM_737EC_MUTANT_SKIP_SETROT)
    SetRotMatrix((Wm737ecMatrix *)PSX_ADDR(W7EC_SC_COMP));
#endif
#if !defined(WM_737EC_MUTANT_SKIP_SETTRANS)
    SetTransMatrix((Wm737ecMatrix *)PSX_ADDR(W7EC_SC_COMP));
#endif

#if defined(WM_737EC_MUTANT_WRONG_LOOP)
    nquads = 3u;
#else
    nquads = 4u;
#endif

    for (quad = 0u; quad < nquads; quad++) {
        u32 vertex = W7EC_VERTEX + quad * 0x20u;
#if defined(WM_737EC_MUTANT_WRONG_INDEX_MUL)
        u32 packet = W7EC_PACKET + index * 32u + quad * 0x48u;
#elif defined(WM_737EC_MUTANT_WRONG_STRIDE)
        u32 packet = W7EC_PACKET + index * 36u + quad * 0x40u;
#else
        u32 packet = W7EC_PACKET + index * 36u + quad * 0x48u;
#endif
        s32 otz;
        s32 flag;

        otz = RotTransPers4((Wm737ecSvector *)PSX_ADDR(vertex),
                            (Wm737ecSvector *)PSX_ADDR(vertex + 8u),
                            (Wm737ecSvector *)PSX_ADDR(vertex + 0x10u),
                            (Wm737ecSvector *)PSX_ADDR(vertex + 0x18u),
                            (s32 *)PSX_ADDR(packet + 8u),
                            (s32 *)PSX_ADDR(packet + 0x10u),
                            (s32 *)PSX_ADDR(packet + 0x18u),
                            (s32 *)PSX_ADDR(packet + 0x20u),
                            (s32 *)PSX_ADDR(W7EC_SC_P),
                            (s32 *)PSX_ADDR(W7EC_SC_FLAG));
        flag = w7ec_bits_as_s32(w7ec_lw(W7EC_SC_FLAG));
#if defined(WM_737EC_MUTANT_SKIP_FLAG)
        flag = 0;
#endif
#if defined(WM_737EC_MUTANT_SKIP_OT)
        (void)w7ec_srav(otz, 0u);
#endif
        if (flag >= 0) {
#if !defined(WM_737EC_MUTANT_SKIP_OT)
            u32 db = w7ec_lw(W7EC_DB);
            u32 ot_base = w7ec_lw(db + 0x70u);
            u32 shift = w7ec_lw(W7EC_SHIFT);
            s32 bucket;
            u32 ot_addr;
            u32 old_ot;
            u32 tag;

#if defined(WM_737EC_MUTANT_WRONG_SHIFT)
            (void)shift;
            bucket = w7ec_srav(otz, 2u);
#else
            bucket = w7ec_srav(otz, shift);
#endif
            ot_addr = ot_base + ((u32)bucket << 2);
            old_ot = w7ec_lw(ot_addr);
            tag = w7ec_lw(packet);
            w7ec_sw(packet, (tag & W7EC_TAG_MASK) | (old_ot & W7EC_PTR_MASK));
            w7ec_sw(ot_addr, (old_ot & W7EC_TAG_MASK) | (packet & W7EC_PTR_MASK));
#endif
        }
    }
}
