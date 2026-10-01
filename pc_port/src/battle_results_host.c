/* battle_results_host.c -- native bodies for adopted routines of the retail
 * battle RESULTS module (0x801DE000..0x801E4500).
 *
 * A body here runs only when battle_mips_runtime.c's identity gate has
 * proved that the retail results module is the code resident at its address
 * (battle_results_gate.inc), and only for routines listed in
 * battle_results_host_leaves.inc.  Every other results routine is interpreted.
 *
 * Each body is a 1:1 rendering of the retail instruction stream -- same
 * loads and stores, same order, same widths -- through the guest accessors in
 * battle_results_host.h (never PSX_ADDR; see the address rule there).  The
 * listing references are to the module linked at 0x801DE000.
 *
 * Proof: pc_port/tests/run_battle_results_host_differential_test.sh.
 */

#include "battle_results_host.h"

/* func_801E24B0 (0x801E24B0..0x801E2528, leaf, 31 instructions).
 * Called once, from the results entry routine at 0x801E25F0 (jal, nop delay
 * slot); no branch, J or data word targets it.
 *
 * For each of the 48 result-id bytes D_800D2FE4[j] that is nonzero, every one
 * of the 150 entries g_GameState+0x2026 (0x8006F65A) equal to it gets
 * D_800D2CB0[j] stored to the parallel byte at g_GameState+0x1F90
 * (0x8006F5C4).  A later j overwrites an earlier one.  0x8006F5C4/0x8006F65A
 * are inside the native g_GameState copy; D_800D2FE4/D_800D2CB0 are battle
 * BSS in guest RAM -- which is why the accessors, not PSX_ADDR, are required.
 *
 *   24B0  a2 = 0 ; a1 = 0x800D2FE4
 *   24BC  v0 = lbu 0(a1) ; beq v0, 0 -> 2514
 *   24CC  a0 = 0
 *   24D0  v1 = lbu 0(a1)               (re-read every inner iteration)
 *   24DC  v0 = lbu 0x8006F65A(a0) ; bne v1, v0 -> 2504
 *   24F4  v0 = lbu 0x800D2CB0(a2) ; sb v0, 0x8006F5C4(a0)
 *   2504  a0++ ; slti a0 < 150 -> 24D0
 *   2514  a2++ ; slti a2 < 48 -> 24BC (delay: a1++)
 *   2524  jr ra
 *
 * The id is re-read inside the inner loop exactly as retail does; the store
 * range (0x8006F5C4..0x8006F659) never overlaps the id table, so this changes
 * no value, but it keeps the access sequence identical.  Reads are pinned to
 * temporaries so C's unspecified operand order cannot reorder them.
 *
 * v0 on return: the final `slti v0, a2, 48` with a2 == 48, i.e. always 0.
 * Other exit registers, reproduced exactly (the caller's victory path goes on
 * to jal 0x800BFBA0 / 0x801E2280 without setting arguments, so they are not
 * provably dead):
 *   a1 = 0x800D2FE4 + 48, a2 = 48                       always;
 *   a0 = 150, v1 = the last nonzero id, at = 0x80070000 + 149
 *                                  only if some id was nonzero (else as on entry). */
uint32_t battle_results_func_801E24B0(uint32_t a0, uint32_t a1, uint32_t a2,
                                      uint32_t a3)
{
    int32_t j;
    int32_t i;
    int inner_ran = 0;
    uint8_t last_id = 0;

    (void)a0;
    (void)a1;
    (void)a2;
    (void)a3;
    for (j = 0; j < 48; j++) {
        if (BR_LBU(0x800D2FE4u + (uint32_t)j) == 0)
            continue;
        inner_ran = 1;
        for (i = 0; i < 150; i++) {
            uint8_t id = BR_LBU(0x800D2FE4u + (uint32_t)j);
            last_id = id;
            uint8_t entry = BR_LBU(0x8006F65Au + (uint32_t)i);
            if (id == entry) {
                uint8_t value = BR_LBU(0x800D2CB0u + (uint32_t)j);
                BR_SB(0x8006F5C4u + (uint32_t)i, value);
            }
        }
    }
    BattleResults_SetExitReg(5, 0x800D2FE4u + 48u);
    BattleResults_SetExitReg(6, 48u);
    if (inner_ran) {
        BattleResults_SetExitReg(1, 0x80070000u + 149u);
        BattleResults_SetExitReg(3, last_id);
        BattleResults_SetExitReg(4, 150u);
    }
    return 0;
}
