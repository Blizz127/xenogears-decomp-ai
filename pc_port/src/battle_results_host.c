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

/* ---- MIPS arithmetic, exactly as the R3000A (and the interpreter's
 * battle_mips_adapter.c) define it.  No host divide by zero, no signed
 * overflow, no implementation-defined signed shifts. ---------------------- */

/* slt/slti: signed 32-bit compare without converting to a signed type. */
static int br_slt(uint32_t a, uint32_t b)
{
    return (a ^ 0x80000000u) < (b ^ 0x80000000u);
}

/* sra rd, rt, n (n in 1..31). */
static uint32_t br_sra(uint32_t value, unsigned n)
{
    uint32_t shifted = value >> n;
    if (value & 0x80000000u)
        shifted |= ~(0xFFFFFFFFu >> n);
    return shifted;
}

/* Two's-complement value of a guest word, as int64 (no conversion of an
 * out-of-range unsigned value to a signed type). */
static int64_t br_s64(uint32_t value)
{
    return (value & 0x80000000u) ? (int64_t)value - INT64_C(0x100000000)
                                 : (int64_t)value;
}

/* mult rs, rt: signed 32x32 -> 64 into HI:LO. */
static void br_mult(uint32_t rs, uint32_t rt, uint32_t *hi, uint32_t *lo)
{
    uint64_t product = (uint64_t)(br_s64(rs) * br_s64(rt));
    *lo = (uint32_t)product;
    *hi = (uint32_t)(product >> 32);
}

/* div rs, rt (signed).  R3000A results the interpreter reproduces: rt == 0
 * gives LO = (rs >= 0 ? 0xFFFFFFFF : 1), HI = rs; 0x80000000 / -1 gives
 * LO = 0x80000000, HI = 0; otherwise C99 truncating quotient/remainder. */
static void br_div(uint32_t rs, uint32_t rt, uint32_t *hi, uint32_t *lo)
{
    int64_t dividend = br_s64(rs);
    int64_t divisor = br_s64(rt);

    if (divisor == 0) {
        *lo = dividend >= 0 ? 0xFFFFFFFFu : 1u;
        *hi = rs;
    } else if (rs == 0x80000000u && rt == 0xFFFFFFFFu) {
        *lo = 0x80000000u;
        *hi = 0;
    } else {
        /* In int64 neither the quotient nor the remainder can overflow. */
        *lo = (uint32_t)(uint64_t)(dividend / divisor);
        *hi = (uint32_t)(uint64_t)(dividend % divisor);
    }
}

/* func_801DF910 (0x801DF910..0x801DFA34, leaf, 74 instructions, no frame).
 * Called once, from 0x801E04C4 (jal, nop delay slot) in 0x801E03FC's row
 * loop, with a0/a1 = two lbu'd bytes and a2 = 0x801E03FC's saved copy of
 * func_801DFA38's result (0..255 there; the body takes any 32-bit a2).  No
 * branch, J or data word targets it.
 *
 * Percent-bar setup into the mutable module words (guest RAM):
 *   44CC = a0&0xFF   44D0 = a1&0xFF   44D4 = |a1-a0| (a1-a0 first, then
 *   rewritten when negative)   44D8/44DC = q*6400/10000 via the magic
 *   0x68DB8BAD (mult; HI >> 12, minus the sign), q = 100*x / a2 by DIV
 *   44E0 = 2/3   44E4 = 0xE3/0xE5   (u8, by sign of a1-a0)
 *
 *   F910  a3 = a0 & 0xFF ; v0 = a3*100 ; div v0, a2 ; v0 = lo
 *   F930  a0 = 0x68DB8BAD ; v1 = v0*6400 ; mult v1, a0 ; a1 &= 0xFF
 *   F954  sw a3 -> 44CC ; sw a1 -> 44D0 ; a0 = a1-a3 ; v1 = sra v1,31
 *   F970  sw a0 -> 44D4 ; v0 = sra(hi,12) - v1 ; sw v0 -> 44D8
 *   F988  bltz a0 -> F9AC (delay v0 = 2)
 *   F990    sb 2 -> 44E0 ; sb 0xE3 -> 44E4 ; j F9D0
 *   F9AC    sb 3 -> 44E0 ; sb 0xE5 -> 44E4 ; sw a3-a1 -> 44D4
 *   F9D0  v0 = lw 44D4 (re-read) ; v1 = v0*100 ; div v1, a2 ; v1 = lo
 *   F9F8  a0 = 0x68DB8BAD ; v0 = v1*6400 ; mult v0, a0 ; v0 = sra v0,31
 *   FA1C  v1 = sra(hi,12) - v0 ; sw v1 -> 44DC ; jr ra
 * There is no divide-by-zero trap check in the bytes: a2 == 0 yields the
 * R3000A quotient (0xFFFFFFFF for these non-negative dividends), reproduced
 * by br_div; 44D4 is re-read from guest RAM exactly as retail does.
 *
 * v0 on return: sra(second v0*6400 product, 31).
 * Exit registers, always: v1 = the 44DC value, a0 = 0x68DB8BAD,
 * a1 = a1&0xFF, a3 = a0&0xFF, at = 0x801E0000, HI:LO = the second mult's
 * product.  a2 and every other register are unchanged. */
uint32_t battle_results_func_801DF910(uint32_t a0, uint32_t a1, uint32_t a2,
                                      uint32_t a3)
{
    uint32_t hi, lo;
    uint32_t v0, v1, r_a0, r_a1, r_a3;

    (void)a3;
    r_a3 = a0 & 0xFFu;
    v0 = r_a3 * 100u;
    br_div(v0, a2, &hi, &lo);
    v0 = lo;
    r_a0 = 0x68DB8BADu;
    v1 = v0 * 6400u;
    br_mult(v1, r_a0, &hi, &lo);
    r_a1 = a1 & 0xFFu;
    BR_SW(0x801E44CCu, r_a3);
    BR_SW(0x801E44D0u, r_a1);
    r_a0 = r_a1 - r_a3;
    v1 = br_sra(v1, 31);
    BR_SW(0x801E44D4u, r_a0);
    v0 = br_sra(hi, 12) - v1;
    BR_SW(0x801E44D8u, v0);
    if (r_a0 & 0x80000000u) {
        BR_SB(0x801E44E0u, 3u);
        BR_SB(0x801E44E4u, 0xE5u);
        BR_SW(0x801E44D4u, r_a3 - r_a1);
    } else {
        BR_SB(0x801E44E0u, 2u);
        BR_SB(0x801E44E4u, 0xE3u);
    }
    v0 = BR_LW(0x801E44D4u);
    v1 = v0 * 100u;
    br_div(v1, a2, &hi, &lo);
    v1 = lo;
    r_a0 = 0x68DB8BADu;
    v0 = v1 * 6400u;
    br_mult(v0, r_a0, &hi, &lo);
    v0 = br_sra(v0, 31);
    v1 = br_sra(hi, 12) - v0;
    BR_SW(0x801E44DCu, v1);

    BattleResults_SetExitReg(1, 0x801E0000u);
    BattleResults_SetExitReg(3, v1);
    BattleResults_SetExitReg(4, r_a0);
    BattleResults_SetExitReg(5, r_a1);
    BattleResults_SetExitReg(7, r_a3);
    BattleResults_SetExitHiLo(hi, lo);
    return v0;
}

/* func_801DFA38 (0x801DFA38..0x801DFAA4, leaf, 28 instructions, no frame).
 * Called once, from 0x801E042C (jal; delay slot sw s1 to 0x801E03FC's own
 * frame); no branch, J or data word targets it.
 *
 * Returns the maximum of the 14 bytes D_800CDD10[8*i .. 8*i+6] and
 * D_800CDD28[8*i .. 8*i+6], i = a0 & 0xFF (battle BSS, guest RAM), taken
 * pairwise in the retail order with the winning byte re-read:
 *
 *   FA38  a2 = 0 ; a0 = (a0&0xFF) << 3 ; v1 = 0x800CDD10
 *   FA4C  a1 = a0 + v1 + 24 ; a0 += v1 ; v1 = a1 + 7
 *   FA60  v0 = lbu 0(a0) ; if !(v0 <u a2) a2 = lbu 0(a0)
 *   FA78  v0 = lbu 0(a1) ; if !(v0 <u a2) a2 = lbu 0(a1)   (delay: a0++)
 *   FA90  a1++ ; slt a1 < v1 -> FA60 ; jr ra (delay: v0 = a2)
 *
 * v0 on return: the maximum (0..255).
 * Exit registers, always: a0 = 0x800CDD10 + 8*i + 7, a1 = v1 =
 * 0x800CDD28 + 8*i + 7, a2 = the maximum. */
uint32_t battle_results_func_801DFA38(uint32_t a0, uint32_t a1, uint32_t a2,
                                      uint32_t a3)
{
    uint32_t r_a0, r_a1, r_a2, v0, v1;

    (void)a1;
    (void)a2;
    (void)a3;
    r_a2 = 0;
    r_a0 = (a0 & 0xFFu) << 3;
    v1 = 0x800CDD10u;
    r_a1 = r_a0 + (v1 + 24u);
    r_a0 = r_a0 + v1;
    v1 = r_a1 + 7u;
    do {
        v0 = BR_LBU(r_a0);
        if (!(v0 < r_a2))
            r_a2 = BR_LBU(r_a0);
        v0 = BR_LBU(r_a1);
        if (!(v0 < r_a2))
            r_a2 = BR_LBU(r_a1);
        r_a0++;
        r_a1++;
    } while (br_slt(r_a1, v1));

    BattleResults_SetExitReg(3, v1);
    BattleResults_SetExitReg(4, r_a0);
    BattleResults_SetExitReg(5, r_a1);
    BattleResults_SetExitReg(6, r_a2);
    return r_a2;
}

/* func_801E1590 (0x801E1590..0x801E168C, leaf, 64 instructions, no frame).
 * Called once, from 0x801E16C4 (jal; delay slot sw s0 to 0x801E1690's
 * frame), with a0 = sp+24 (ids[8]), a1 = sp+40 (counts[8]), a2 = sp+32
 * (kinds[8]) in 0x801E1690's guest frame; no branch, J or data word targets
 * it.  The body takes the pointers as raw guest words and reaches them only
 * through the accessors, so the caller's guest frame is what it reads and
 * writes.
 *
 * Clears ids[0..7] and counts[0..7] (interleaved, ids first; kinds is NOT
 * cleared), then for each k in 0..7 with D_800CDCFC[k] (id) != 0 searches all
 * 8 slots for kinds[j] == D_800CDCF4[k] && ids[j] == id: a hit increments
 * counts[j]; a miss appends (kind, id) at the cursor (t6/t7) and increments
 * counts at the count cursor t5.  Stale kinds[] bytes are compared (retail
 * reads them; the ids[] test then rejects them since ids[] were zeroed and the
 * id is nonzero) -- reproduced read for read.
 *
 *   1590  v1 = a1 ; a3 = a0 ; t0 = a1+8
 *   159C  sb 0,(a3) ; sb 0,(v1) ; v1++ ; slt v1 < t0 -> 159C (delay a3++)
 *   15B4  t9 = 8 ; t3 = 0x800CDCFC ; t4 = t3-8 ; t8 = t3+8
 *         t5 = a1 ; t7 = a0 ; t6 = a2
 *   15D4  v0 = lbu (t3) ; beq v0,0 -> 1678 (delay t2 = 0)
 *   15E4  t1 = a1 ; t0 = a0 ; a3 = a2
 *   15F0  v1 = lbu (t4) ; v0 = lbu (a3) ; bne -> 162C
 *         v1 = lbu (t3) ; v0 = lbu (t0) ; bne -> 162C
 *         v0 = lbu (t1) + 1 ; j 1644 (delay sb v0,(t1))
 *   162C  t1++ ; t0++ ; t2++ ; slti t2 < 8 -> 15F0 (delay a3++)
 *   1644  bne t2, t9 -> 1678
 *         sb lbu(t4),(t6) ; sb lbu(t3),(t7) ; t6++ ; t7++
 *         sb lbu(t5)+1,(t5) ; t5++
 *   1678  t3++ ; slt t3 < t8 -> 15D4 (delay t4++) ; jr ra
 * The pointer compares are signed (slt), as reproduced by br_slt.
 *
 * v0 on return: the final slt, i.e. always 0.
 * Exit registers, always: v1, a3, t0 (the last values the stream above left
 * in them), t2 (0 if D_800CDCFC[7] == 0, else the last slot index reached),
 * t3 = t8 = 0x800CDD04, t4 = 0x800CDCFC, t5/t6/t7 = the cursors, t9 = 8;
 * t1 only if some id was nonzero (else as on entry). */
uint32_t battle_results_func_801E1590(uint32_t a0, uint32_t a1, uint32_t a2,
                                      uint32_t a3)
{
    uint32_t v0, v1, r_a3, t0, t1 = 0, t2, t3, t4, t5, t6, t7, t8, t9;
    int t1_written = 0;

    (void)a3;
    v1 = a1;
    r_a3 = a0;
    t0 = a1 + 8u;
    do {
        BR_SB(r_a3, 0u);
        BR_SB(v1, 0u);
        v1++;
        v0 = (uint32_t)br_slt(v1, t0);
        r_a3++;
    } while (v0 != 0);
    t9 = 8u;
    t3 = 0x800CDCFCu;
    t4 = t3 - 8u;
    t8 = t3 + 8u;
    t5 = a1;
    t7 = a0;
    t6 = a2;
    do {
        v0 = BR_LBU(t3);
        t2 = 0;
        if (v0 != 0) {
            t1 = a1;
            t0 = a0;
            r_a3 = a2;
            t1_written = 1;
            for (;;) {
                v1 = BR_LBU(t4);
                v0 = BR_LBU(r_a3);
                if (v1 == v0) {
                    v1 = BR_LBU(t3);
                    v0 = BR_LBU(t0);
                    if (v1 == v0) {
                        v0 = BR_LBU(t1) + 1u;
                        BR_SB(t1, v0);
                        break;
                    }
                }
                t1++;
                t0++;
                t2++;
                v0 = (uint32_t)br_slt(t2, 8u);
                r_a3++;
                if (v0 == 0)
                    break;
            }
            if (t2 == t9) {
                v0 = BR_LBU(t4);
                BR_SB(t6, v0);
                v0 = BR_LBU(t3);
                t6++;
                BR_SB(t7, v0);
                v0 = BR_LBU(t5);
                t7++;
                v0 = v0 + 1u;
                BR_SB(t5, v0);
                t5++;
            }
        }
        t3++;
        v0 = (uint32_t)br_slt(t3, t8);
        t4++;
    } while (v0 != 0);

    BattleResults_SetExitReg(3, v1);
    BattleResults_SetExitReg(7, r_a3);
    BattleResults_SetExitReg(8, t0);
    if (t1_written)
        BattleResults_SetExitReg(9, t1);
    BattleResults_SetExitReg(10, t2);
    BattleResults_SetExitReg(11, t3);
    BattleResults_SetExitReg(12, t4);
    BattleResults_SetExitReg(13, t5);
    BattleResults_SetExitReg(14, t6);
    BattleResults_SetExitReg(15, t7);
    BattleResults_SetExitReg(24, t8);
    BattleResults_SetExitReg(25, t9);
    return v0;
}
