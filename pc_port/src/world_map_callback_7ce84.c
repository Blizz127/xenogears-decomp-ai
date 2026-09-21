/* Exact native transcription of retail [0x8007CE84, 0x8007D078). */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "psyq/libgte.h"
#include "world_map_callback_7ce84.h"
#include "world_map_common_tail.h"
#include "world_map_helper_848b4.h"
#include "world_map_helper_93354.h"
#include "world_map_helper_93a5c.h"

#define WM_M12D_POOL_PTR    UINT32_C(0x8009BE24)
#define WM_M12D_CONTEXT_PTR UINT32_C(0x8009C620)
#define WM_M12D_SC_VECTOR   UINT32_C(0x1F8000A0)

static s16 m12d_lh(u32 address)
{
    s16 value;
    memcpy(&value, PSX_ADDR(address), sizeof(value));
    return value;
}

static u32 m12d_lw(u32 address)
{
    u32 value;
    memcpy(&value, PSX_ADDR(address), sizeof(value));
    return value;
}

static void m12d_sh(u32 address, u16 value)
{
    memcpy(PSX_ADDR(address), &value, sizeof(value));
}

static void m12d_sw(u32 address, u32 value)
{
    memcpy(PSX_ADDR(address), &value, sizeof(value));
}

static u32 m12d_sra12(u32 bits)
{
    u32 shifted = bits >> 12u;

    if ((bits & UINT32_C(0x80000000)) != 0u)
        shifted |= UINT32_C(0xFFF00000);
    return shifted;
}

static u32 m12d_slot(s32 slot_index)
{
    return m12d_lw(WM_M12D_POOL_PTR) + ((u32)slot_index << 7u);
}

static void m12d_publish_scratch(u32 slot)
{
    m12d_sh(WM_M12D_SC_VECTOR + 0u,
             (u16)m12d_sra12(m12d_lw(slot + 0x28u)));
    m12d_sh(WM_M12D_SC_VECTOR + 2u,
             (u16)m12d_sra12(m12d_lw(slot + 0x2Cu)));
    m12d_sh(WM_M12D_SC_VECTOR + 4u,
             (u16)m12d_sra12(m12d_lw(slot + 0x30u)));
}

s32 wm_8007CE84(s32 slot_index)
{
    u32 context;
    u32 slot;

#if defined(W34N83_MUTANT_WRONG_CONTEXT_LINK)
    wm_800848B4(5, 7);
#else
    wm_800848B4(5, 6);
#endif
    context = m12d_lw(WM_M12D_CONTEXT_PTR);
    m12d_sh(context + 0x1A4u, 0u);
    m12d_sh(context + 0x1BCu, 0u);
    m12d_sh(context + 0x1BEu, 0u);
    m12d_sh(context + 0x1C0u, 0u);
#if !defined(W34N83_MUTANT_SKIP_ROTATION_INIT)
    (void)RotMatrixYXZ((SVECTOR *)PSX_ADDR(context + 0x1BCu),
                       (MATRIX *)PSX_ADDR(context + 0x1C4u));
#endif

    slot = m12d_slot(slot_index);
    m12d_sw(slot + 0x40u, UINT32_C(0xFFFFC000));
    m12d_sw(slot + 0x28u, UINT32_C(0x00B00000));
    m12d_sw(slot + 0x2Cu, 0u);
    m12d_sw(slot + 0x30u, UINT32_C(0x00200000));
    m12d_sw(slot + 0x38u, 0u);
    m12d_sw(slot + 0x3Cu, 0u);
#if defined(W34N83_MUTANT_WRONG_INTERNAL_SEED)
    m12d_sh(slot + 0x20u, 1u);
#else
    m12d_sh(slot + 0x20u, 0u);
#endif
    return 1;
}

s32 wm_8007CF18(s32 slot_index)
{
    u32 slot = m12d_slot(slot_index);
    u32 context = m12d_lw(WM_M12D_CONTEXT_PTR);
    u32 height;

    if (m12d_lh(slot + 4u) == 1) {
        m12d_sh(slot + 4u, 0u);
#if defined(W34N83_MUTANT_SKIP_LATCH_ARM)
        m12d_sh(slot + 0x20u, 0u);
#else
        m12d_sh(slot + 0x20u, 1u);
#endif
    }

    m12d_sw(slot + 0x30u,
             m12d_lw(slot + 0x30u) + m12d_lw(slot + 0x40u));
    wm_80093354(slot + 0x28u);
    height = (u32)wm_80093A5C(m12d_lw(slot + 0x28u),
                              m12d_lw(slot + 0x30u));
#if defined(W34N83_MUTANT_SKIP_HEIGHT_BIAS)
    m12d_sw(slot + 0x2Cu, height);
#else
    m12d_sw(slot + 0x2Cu, height - UINT32_C(0x00004000));
#endif
    m12d_sw(context + 0x1ACu, m12d_sra12(m12d_lw(slot + 0x28u)));
    m12d_sw(context + 0x1B0u, m12d_sra12(m12d_lw(slot + 0x2Cu)));
    m12d_sw(context + 0x1B4u, m12d_sra12(m12d_lw(slot + 0x30u)));
    m12d_publish_scratch(slot);
#if defined(W34N83_MUTANT_WRONG_PRIMARY_MARKER)
    wm_80089160(20u, WM_M12D_SC_VECTOR, 0u);
#else
    wm_80089160(21u, WM_M12D_SC_VECTOR, 0u);
#endif

    if (m12d_lh(slot + 0x20u) == 1) {
        m12d_publish_scratch(slot);
#if defined(W34N83_MUTANT_WRONG_SECONDARY_MARKER)
        wm_80089160(29u, WM_M12D_SC_VECTOR, 0u);
#else
        wm_80089160(30u, WM_M12D_SC_VECTOR, 0u);
#endif
    }
    return 1;
}
