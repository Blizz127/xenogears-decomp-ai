/*
 * World-map scheduler callback 0x80071A50 (slot 14, Table-A cb0).
 *
 * Retail is a two-instruction leaf, verified fresh from disc/world_map.bin
 * (load base 0x8006FAF0):
 *
 *   80071A50: 03e00008  jr    $ra
 *   80071A54: 24020001  addiu $v0, $zero, 1     ; branch delay slot
 *
 * The delay-slot `addiu` retires before the control transfer completes, so
 * $v0 is 1 on every return; there is no other path out of the function. The
 * slice contains no JAL/JALR, no load, no store, no COP2/GTE instruction and
 * no conditional branch, so the callback reads no register argument and has
 * no observable effect on guest memory, slot state, or globals. The
 * scheduler-supplied slot index is consequently unused by retail and is
 * discarded explicitly here.
 *
 * The immediately following function 0x80071A58 is the slot-14 cb1 partner
 * and is deliberately NOT implemented or resolved by this change.
 */
#include "common.h"
#include "world_map_callback_71a50.h"

s32 wm_80071A50(s32 slot_index)
{
    (void)slot_index;
    return 1;
}
