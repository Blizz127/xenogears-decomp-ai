/*
 * World-map scheduler callback 0x80087710 (slot 15, Table-B cb0).
 *
 * Fresh retail listing from world_map.bin (load base 0x8006FAF0):
 *
 *   80087710: 000421c0  sll   a0,a0,7
 *   80087714: 3c02800a  lui   v0,0x800a
 *   80087718: 8c42be24  lw    v0,-0x41dc(v0)
 *   8008771c: 24030008  addiu v1,zero,8
 *   80087720: 00441021  addu  v0,v0,a0
 *   80087724: ac400050  sw    zero,0x50(v0)
 *   80087728: ac430054  sw    v1,0x54(v0)
 *   8008772c: 03e00008  jr    ra
 *   80087730: 24020001  addiu v0,zero,1  ; return delay slot
 *
 * The body is a leaf: no JAL/JALR, conditional branch, COP2/GTE, rendering,
 * or helper call.  Its independent cb1 partner begins at 0x80087734.
 * A byte-identical body at 0x800877E0 belongs to another Table-B stream and
 * is deliberately not aliased or resolved here.
 */
#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_87710.h"

#define WM_87710_POOL_PTR    0x8009BE24u
#define WM_87710_SLOT_STRIDE 0x80u
#define WM_87710_SLOT_FIELD0 0x50u
#define WM_87710_SLOT_FIELD1 0x54u

s32 wm_80087710(s32 slot_index)
{
    u32 pool;
    u32 slot;

    /* Volatile word accesses preserve the retail access widths and the two
     * stores' observable order without introducing a production helper. */
    pool = *(volatile u32*)PSX_ADDR(WM_87710_POOL_PTR);
#if defined(WM_87710_TEST_TRACE)
    wm_87710_test_trace(0x80087718u, WM_87710_TRACE_LW,
                        WM_87710_POOL_PTR, 4u, pool);
#endif

    /* R3000 `sll a0,7` and `addu` are modulo-32-bit operations. */
    slot = pool + ((u32)slot_index << 7);

    *(volatile u32*)PSX_ADDR(slot + WM_87710_SLOT_FIELD0) = 0u;
#if defined(WM_87710_TEST_TRACE)
    wm_87710_test_trace(0x80087724u, WM_87710_TRACE_SW,
                        slot + WM_87710_SLOT_FIELD0, 4u, 0u);
#endif

    *(volatile u32*)PSX_ADDR(slot + WM_87710_SLOT_FIELD1) = 8u;
#if defined(WM_87710_TEST_TRACE)
    wm_87710_test_trace(0x80087728u, WM_87710_TRACE_SW,
                        slot + WM_87710_SLOT_FIELD1, 4u, 8u);
#endif

    return 1;
}
