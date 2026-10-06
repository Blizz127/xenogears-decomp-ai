/* Support for instruction-faithful port bodies of unmatched battle functions
 * (M-a; port-only, never seen by the matching build).
 *
 * tools/scripts/battle_lift_asm.py translates a retail function from its
 * splat listing (asm/battle/nonmatchings/...) into C, one statement per MIPS
 * instruction: the o32 registers are u32 locals, branch delay slots are
 * executed where the R3000 executes them, every load and store goes through
 * the battle guest resolver (bg_ptr, the code the interpreter's bus runs),
 * COP2 moves and commands go to the same PsyCross GTE entry points the
 * interpreter's COP2 bus uses (MFC2/MTC2/CFC2/CTC2/doCOP2), and a jump-table
 * JR dispatches on the target word read from the retail table in guest RAM.
 *
 * Calls:
 *  - overlay callees go through PcPort_BattleMipsCallGuest with a0..a3 and
 *    the three o32 stack argument words: an adopted guest-ABI leaf with a
 *    typed thunk runs natively from there, anything else is still
 *    interpreted (listed in each body's header comment);
 *  - main-executable callees are direct C calls to the native function the
 *    interpreter's bridge binds for that address, through the bl_* wrappers
 *    below, which reproduce that bridge's argument/return contract for the
 *    callee (pointer arguments resolved like translate_argument, CompMatrix's
 *    physical-address rule and v0 = a2, the retail trig masks, ...).
 *
 * The stack frame: retail code addresses its frame through $sp and may hand
 * the address of a local to a callee, so the frame must be addressable by a
 * 32-bit word that both bg_ptr and the interpreter accept.  Each TU that uses
 * this header gets one static frame area (the port links -no-pie, so its
 * address fits a u32 and lies in a registered host range); a lifted body
 * takes its frame below the current cursor and gives it back on return, so
 * nested and re-entrant native calls get disjoint frames. */
#ifndef XENO_BATTLE_LIFT_H
#define XENO_BATTLE_LIFT_H

#ifdef XENO_PC_PORT
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "battle_host_guest.h"
#include "psyq/libgte.h"

#define BL_STACK_SIZE 0x8000u

static u8 bl_stack[BL_STACK_SIZE] __attribute__((aligned(16), unused));
static u32 bl_cursor __attribute__((unused));

static __attribute__((unused, noinline, noreturn)) void bl_fail(const char* fn, const char* what,
                                                                 u32 value) {
    fprintf(stderr, "[xeno-port][battle-lift] %s: %s 0x%08x\n", fn, what, value);
    abort();
}

/* Reserve `frame` bytes (the retail frame plus the caller's 0x10-byte home
 * area and three o32 stack-argument words) and return the incoming $sp. */
static inline __attribute__((unused)) u32 bl_enter(const char* fn, u32 frame, u32* saved) {
    uintptr_t base = (uintptr_t)bl_stack;
    u32 sp;

    if ((base >> 32) != 0) {
        bl_fail(fn, "frame area above 4 GiB", 0);
    }
    if (bl_cursor == 0) {
        bl_cursor = (u32)(base + BL_STACK_SIZE);
    }
    *saved = bl_cursor;
    sp = (bl_cursor - 0x20u) & ~7u;
    if (sp < (u32)base + frame + 0x40u) {
        bl_fail(fn, "frame area exhausted", frame);
    }
    bl_cursor = sp - frame;
    return sp;
}

static inline __attribute__((unused)) void bl_leave(u32 saved) {
    bl_cursor = saved;
}

/* Memory, through the one guest resolver. */
static inline __attribute__((unused)) u32 bl_lw(u32 a) { u32 v; memcpy(&v, bg_ptr(a), 4); return v; }
static inline __attribute__((unused)) u32 bl_lh(u32 a) { s16 v; memcpy(&v, bg_ptr(a), 2); return (u32)(s32)v; }
static inline __attribute__((unused)) u32 bl_lhu(u32 a) { u16 v; memcpy(&v, bg_ptr(a), 2); return v; }
static inline __attribute__((unused)) u32 bl_lb(u32 a) { return (u32)(s32)*(s8*)bg_ptr(a); }
static inline __attribute__((unused)) u32 bl_lbu(u32 a) { return *(u8*)bg_ptr(a); }
static inline __attribute__((unused)) void bl_sw(u32 a, u32 v) { memcpy(bg_ptr(a), &v, 4); }
static inline __attribute__((unused)) void bl_sh(u32 a, u32 v) { u16 h = (u16)v; memcpy(bg_ptr(a), &h, 2); }
static inline __attribute__((unused)) void bl_sb(u32 a, u32 v) { *(u8*)bg_ptr(a) = (u8)v; }

/* MULT/MULTU/DIV/DIVU into LO/HI (R3000 results for a zero divisor and for
 * 0x80000000 / -1, as bg_div models them). */
#define BL_MULT(rs, rt) do { int64_t p_ = (int64_t)(s32)(rs) * (int64_t)(s32)(rt); \
    lo = (u32)(uint64_t)p_; hi = (u32)((uint64_t)p_ >> 32); } while (0)
#define BL_MULTU(rs, rt) do { uint64_t p_ = (uint64_t)(u32)(rs) * (uint64_t)(u32)(rt); \
    lo = (u32)p_; hi = (u32)(p_ >> 32); } while (0)
#define BL_DIV(rs, rt) do { s32 n_ = (s32)(rs), d_ = (s32)(rt); \
    lo = (u32)bg_div(n_, d_); hi = (u32)bg_rem(n_, d_); } while (0)
#define BL_DIVU(rs, rt) do { u32 n_ = (rs), d_ = (rt); \
    lo = bg_divu(n_, d_); hi = bg_remu(n_, d_); } while (0)
#define BL_SRA(v, n) ((u32)((s32)(v) >> ((n) & 31)))

/* Overlay callee: through the guest-call service (native typed thunk, else
 * the interpreter).  v0 only: o32 callers of these never read v1. */
static __attribute__((unused, noinline)) u32 bl_call_guest(const char* fn, u32 target, u32 a0, u32 a1,
                                                           u32 a2, u32 a3, u32 sp) {
    u32 args[7];
    u32 v0 = 0;

    args[0] = a0;
    args[1] = a1;
    args[2] = a2;
    args[3] = a3;
    args[4] = bl_lw(sp + 0x10u);
    args[5] = bl_lw(sp + 0x14u);
    args[6] = bl_lw(sp + 0x18u);
    if (PcPort_BattleMipsCallGuest(target, args, 7, &v0) != 0) {
        bl_fail(fn, "guest call failed", target);
    }
    return v0;
}

/* CompMatrix as runtime_bridge_call binds it: word-aligned matrix words, a
 * physical RAM address (< 2 MiB) taken as its KSEG0 alias, v0 = the third
 * argument word. */
static __attribute__((unused)) u32 bl_CompMatrix(const char* fn, u32 a0, u32 a1, u32 a2) {
    u32 w[3];
    void* m[3];
    unsigned k;

    w[0] = a0;
    w[1] = a1;
    w[2] = a2;
    for (k = 0; k < 3; k++) {
        u32 address = w[k];
        if ((address & 3u) != 0) {
            bl_fail(fn, "CompMatrix unaligned matrix", address);
        }
        if (address < 0x200000u) {
            address |= 0x80000000u;
        }
        m[k] = bg_ptr(address);
    }
    CompMatrix((MATRIX*)m[0], (MATRIX*)m[1], (MATRIX*)m[2]);
    return a2;
}
#endif /* XENO_PC_PORT */

#endif
