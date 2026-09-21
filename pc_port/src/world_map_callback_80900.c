/* Exact native transcription of retail [0x80080900, 0x800809EC). */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_80900.h"
#include "world_map_common_tail.h"

#define WM_M13M_POOL_PTR  UINT32_C(0x8009BE24)
#define WM_M13M_RESET_X   UINT32_C(0x8009C5AC)
#define WM_M13M_RESET_Y   UINT32_C(0x8009C5B0)
#define WM_M13M_RESET_Z   UINT32_C(0x8009C5B4)
#define WM_M13M_VECTOR    UINT32_C(0x1F8000A0)

static u16 m13m_lhu(u32 address)
{
    u16 value;
    memcpy(&value, PSX_ADDR(address), sizeof(value));
    return value;
}

static u32 m13m_lw(u32 address)
{
    u32 value;
    memcpy(&value, PSX_ADDR(address), sizeof(value));
    return value;
}

static void m13m_sh(u32 address, u16 value)
{
    memcpy(PSX_ADDR(address), &value, sizeof(value));
}

static void m13m_sw(u32 address, u32 value)
{
    memcpy(PSX_ADDR(address), &value, sizeof(value));
}

static u32 m13m_sra12(u32 bits)
{
    u32 shifted = bits >> 12u;

    if ((bits & UINT32_C(0x80000000)) != 0u)
        shifted |= UINT32_C(0xFFF00000);
    return shifted;
}

static u32 m13m_slot(s32 slot_index)
{
#if defined(W34N89_MUTANT_WRONG_SLOT_STRIDE)
    return m13m_lw(WM_M13M_POOL_PTR) + ((u32)slot_index << 6u);
#else
    return m13m_lw(WM_M13M_POOL_PTR) + ((u32)slot_index << 7u);
#endif
}

s32 wm_80080900(s32 slot_index)
{
    u32 slot = m13m_slot(slot_index);

#if defined(W34N89_MUTANT_WRONG_RESET_SOURCE)
    m13m_sw(slot + 0x28u, m13m_lw(WM_M13M_RESET_Z));
#else
    m13m_sw(slot + 0x28u, m13m_lw(WM_M13M_RESET_X));
#endif
    m13m_sw(slot + 0x2Cu, m13m_lw(WM_M13M_RESET_Y));
    m13m_sw(slot + 0x30u, m13m_lw(WM_M13M_RESET_Z));
    return 1;
}

s32 wm_80080944(s32 slot_index)
{
    u32 slot = m13m_slot(slot_index);

#if defined(W34N89_MUTANT_WRONG_LATCH_VALUE)
    if (m13m_lhu(slot + 0x04u) == 2u) {
#else
    if (m13m_lhu(slot + 0x04u) == 1u) {
#endif
#if !defined(W34N89_MUTANT_SKIP_LATCH_CLEAR)
        m13m_sh(slot + 0x04u, 0u);
#endif
#if defined(W34N89_MUTANT_WRONG_VECTOR_SHIFT)
        m13m_sh(WM_M13M_VECTOR + 0u,
                 (u16)(m13m_lw(slot + 0x28u) >> 16u));
#else
        m13m_sh(WM_M13M_VECTOR + 0u,
                 (u16)m13m_sra12(m13m_lw(slot + 0x28u)));
#endif
        m13m_sh(WM_M13M_VECTOR + 2u,
                 (u16)m13m_sra12(m13m_lw(slot + 0x2Cu)));
        m13m_sh(WM_M13M_VECTOR + 4u,
                 (u16)m13m_sra12(m13m_lw(slot + 0x30u)));
        wm_80089160(40u, WM_M13M_VECTOR, 0u);
#if defined(W34N89_MUTANT_WRONG_MARKER_ID)
        wm_80089160(40u, WM_M13M_VECTOR, 0u);
#else
        wm_80089160(41u, WM_M13M_VECTOR, 0u);
#endif
        wm_80089160(42u, WM_M13M_VECTOR, 0u);
    }
    return 1;
}
