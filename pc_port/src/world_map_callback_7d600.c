/* Exact native transcription of retail [0x8007D600, 0x8007D774). */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "psyq/libgte.h"
#include "world_map_callback_7d600.h"
#include "world_map_common_tail.h"
#include "world_map_helper_848b4.h"
#include "world_map_helper_93354.h"
#include "world_map_helper_93a5c.h"

#define WM_M12M_POOL_PTR    UINT32_C(0x8009BE24)
#define WM_M12M_CONTEXT_PTR UINT32_C(0x8009C620)
#define WM_M12M_SC_VECTOR   UINT32_C(0x1F8000A0)

static u32 m12m_lw(u32 address)
{
    u32 value;
    memcpy(&value, PSX_ADDR(address), sizeof(value));
    return value;
}

static void m12m_sh(u32 address, u16 value)
{
    memcpy(PSX_ADDR(address), &value, sizeof(value));
}

static void m12m_sw(u32 address, u32 value)
{
    memcpy(PSX_ADDR(address), &value, sizeof(value));
}

static u32 m12m_sra12(u32 bits)
{
    u32 shifted = bits >> 12u;

    if ((bits & UINT32_C(0x80000000)) != 0u)
        shifted |= UINT32_C(0xFFF00000);
    return shifted;
}

static u32 m12m_slot(s32 slot_index)
{
    return m12m_lw(WM_M12M_POOL_PTR) + ((u32)slot_index << 7u);
}

s32 wm_8007D690(s32 slot_index)
{
    u32 slot = m12m_slot(slot_index);
    u32 context = m12m_lw(WM_M12M_CONTEXT_PTR);
    u32 height;

    m12m_sw(slot + 0x30u,
             m12m_lw(slot + 0x30u) + m12m_lw(slot + 0x40u));
#if defined(W34N78_MUTANT_WRAP_WRONG_VECTOR)
    wm_80093354(slot + 0x2Cu);
#else
    wm_80093354(slot + 0x28u);
#endif
    height = (u32)wm_80093A5C(m12m_lw(slot + 0x28u),
                              m12m_lw(slot + 0x30u));
#if defined(W34N78_MUTANT_SKIP_HEIGHT_BIAS)
    m12m_sw(slot + 0x2Cu, height);
#else
    m12m_sw(slot + 0x2Cu, height - UINT32_C(0x00004000));
#endif

    m12m_sw(context + 0x44Cu, m12m_sra12(m12m_lw(slot + 0x28u)));
    m12m_sw(context + 0x450u, m12m_sra12(m12m_lw(slot + 0x2Cu)));
    m12m_sw(context + 0x454u, m12m_sra12(m12m_lw(slot + 0x30u)));
    m12m_sh(WM_M12M_SC_VECTOR + 0u,
             (u16)m12m_sra12(m12m_lw(slot + 0x28u)));
    m12m_sh(WM_M12M_SC_VECTOR + 2u,
             (u16)m12m_sra12(m12m_lw(slot + 0x2Cu)));
    m12m_sh(WM_M12M_SC_VECTOR + 4u,
             (u16)m12m_sra12(m12m_lw(slot + 0x30u)));
#if defined(W34N78_MUTANT_WRONG_MARKER_ID)
    wm_80089160(24u, WM_M12M_SC_VECTOR, 0u);
#else
    wm_80089160(25u, WM_M12M_SC_VECTOR, 0u);
#endif
    return 1;
}
