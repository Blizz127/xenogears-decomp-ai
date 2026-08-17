/*
 * World-map D788 CD start 0x8009699C.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x8009699C, 0x80096A6C).  See world_map_helper_9699c.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_9699c.h"

#define W99C_CEBC  0x8009CEBCu
#define W99C_CD44  0x8009CD44u
#define W99C_D3BC  0x8009D3BCu
#define W99C_BE48  0x8009BE48u
#define W99C_CCB0  0x8009CCB0u
#define W99C_CCA8  0x8009CCA8u
#define W99C_CCA0  0x8009CCA0u
#define W99C_D7F4  0x8009D7F4u
#define W99C_D614  0x8009D614u
#define W99C_D56C  0x8009D56Cu
#define W99C_CEB8  0x8009CEB8u
#define W99C_C590  0x8009C590u
#define W99C_CB    0x80096A6Cu

int CdIntToPos(int i, void *p);
void *CdSyncCallback(void *func);
int CdControlF(unsigned char com, unsigned char *param);

#if defined(WM_9699C_TEST_TRACE)
extern void wm_9699c_test_store(u32 address, u32 value);
#define W99C_TRACE_STORE(a, v) wm_9699c_test_store((a), (v))
#else
#define W99C_TRACE_STORE(a, v) ((void)0)
#endif

static u32 w99c_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static void w99c_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    W99C_TRACE_STORE(a, v);
}

void wm_8009699C(u32 record)
{
    u32 file_id;
    u32 byte_count;
    u32 dest;
    u32 blocks;

    file_id = w99c_lw(record);
#if defined(WM_9699C_MUTANT_CD44_TWO)
    w99c_sw(W99C_CD44, 2u);
#else
    w99c_sw(W99C_CD44, 1u);
#endif
    w99c_sw(W99C_D3BC, record);
    w99c_sw(W99C_D3BC, record + 12u);
    byte_count = w99c_lw(record + 4u);
    dest = w99c_lw(record + 8u);
    w99c_sw(W99C_BE48, 0u);
    w99c_sw(W99C_CCB0, 0u);
    w99c_sw(W99C_CCA8, 0u);
    w99c_sw(W99C_CCA0, 0u);
#if defined(WM_9699C_MUTANT_WRONG_BLOCKS)
    blocks = (byte_count + 0x7ffu) >> 10;
#else
    blocks = (byte_count + 0x7ffu) >> 11;
#endif
    w99c_sw(W99C_D7F4, file_id);
    w99c_sw(W99C_D614, file_id);
    w99c_sw(W99C_D56C, blocks);
    w99c_sw(W99C_CEB8, byte_count);
    w99c_sw(W99C_C590, dest);

#if !defined(WM_9699C_MUTANT_SKIP_POS)
    (void)CdIntToPos((int)file_id, PSX_ADDR(W99C_CEBC));
#endif
#if defined(WM_9699C_MUTANT_WRONG_CB)
    (void)CdSyncCallback((void *)(uintptr_t)0x80096A70u);
#else
    (void)CdSyncCallback((void *)(uintptr_t)W99C_CB);
#endif
#if !defined(WM_9699C_MUTANT_SKIP_SETLOC)
    (void)CdControlF(2u, (unsigned char *)PSX_ADDR(W99C_CEBC));
#endif
}
