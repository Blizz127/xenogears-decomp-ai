/* battle mainasm_15238: the retail run at file offset 0x15238, in retail order,
 * split off the front of main35_p4 because its C is gcc 2.6.3 code
 * (BattleGcc263; main35_p4's func_80085084 onward is 2.7.2).  Functions still
 * listed as INCLUDE_ASM are not byte-matching C yet.  The whole file is
 * retail-build only; any port body for these functions lives in the port's
 * own TU. */
#include "common.h"

#ifndef XENO_PC_PORT
extern u8 D_800C3E2C;
extern u16 D_800C3D64;
extern u8 D_800C3EB4[][28];
extern u16 func_80089C08(u8);
extern u16 func_80089C9C(u16, u8);

/* func_80084D28.s, byte-exact: the target mask of the selection's group.
 * Of the 11 combatants, every one that the D_800C3D64 mask accepts
 * (func_80089C9C) and that shares the group (row byte 0 of D_800C3EB4) of
 * the selected combatant D_800C3E2C adds its func_80089C08 bit.  The
 * natural loop matches only under gcc 2.6.x; under 2.7.2 the counter
 * increment stays at the loop end (retail has it in the first branch's
 * delay slot). */
u16 func_80084D28(void) {
    u16 mask = 0;
    s32 i;

    for (i = 0; i < 11; i++) {
        if (func_80089C9C(D_800C3D64, i) != 0) {
            if (D_800C3EB4[D_800C3E2C][0] == D_800C3EB4[i][0]) {
                mask |= func_80089C08(i);
            }
        }
    }
    return mask;
}

INCLUDE_ASM("asm/battle/nonmatchings/mainasm_15238", func_80084DE4);
#endif
