# Field-step pad input probe (XENO_FIELD_STEP_PAD_INPUT) — source/test only

Coordinator message 6b86aea263338885 authorized a bounded source/test-only probe.
Started 21:53:02Z on 2026-10-03. This is a unit proof, not runtime evidence: no
game run, no field loop, no timing, no counter-build integration.

## Why

`XENO_FIELD_TEST_INPUT` is clocked by field steps, but it ORs into the
FieldPollControllers accumulators after both control masks, so it cannot be
strict acceptance. `XENO_PAD_TEST_INPUT` writes the physical pad before
ControllerPoll, but it is clocked by vblanks. FieldPollControllers drains and
ORs however many vblank states queued up, so equal vblank holds gave unequal
field updates (field14 frames-cpu1 finding). This probe is clocked by field
steps and injects at the pad.

## Mechanism

- Schedule `XENO_FIELD_STEP_PAD_INPUT="step:value,..."`: first step 0,
  strictly increasing, game CTRL_BTN_* bits. `XENO_FIELD_STEP_PAD_MAP=<0..0xFFF>`
  arms on the first vblank in that field; unset arms on the first vblank.
- Injection: `PcPort_PrepareControllerPoll` runs PsyX_UpdateInput →
  PadTestInputInject → **FieldStepPadApply** → PadOnControl/ForcedMenuNav.
  It clears the active-low bits of g_C1Buffer[2..3] after undoing the retail
  remap (PcPort_PadTestGameToPhysical).
- Clock: `PcPort_FieldTestInputMerge` is the single call after every
  FieldPollControllers drain, and it calls `PcPort_FieldStepPadNoteDrain()`
  first. Step k = drains completed since arming.

## Invariants (each covered by run_field_step_pad_input_test.sh)

1. Every vblank state queued between drain k-1 and drain k carries value(k),
   so drain k's held accumulator D_800AFE9C == value(k) (| real pad) &
   controllerBtnMask & D_800ADB00. This holds for any cadence ≥ 1 vblank per
   step. Tested with 3 cadence patterns × identity and USA-swap layouts.
2. A transition produces its press edge (D_800C2694, retail "Released" =
   newly pressed) exactly once, in the first non-empty drain after it.
3. A drain with no vblank sees no input, is counted (`empty`), is logged,
   and is never filled in. The step still advances.
4. Both masks still remove scheduled bits. Contrast case: the old
   XENO_FIELD_TEST_INPUT path keeps a bit both masks remove.
5. The 16-state FIFO and its overflow flag are unchanged: 20 vblanks in
   one step set g_ControllerIsStateStackFull and nothing compensates.
6. With the variable unset, the pad buffer is untouched and real input
   behaves as before.
7. It refuses (Apply returns -1, and production exits EXIT_FAILURE) when
   combined with XENO_FIELD_TEST_INPUT or XENO_PAD_TEST_INPUT, or given a bad
   schedule or map.
8. Retail-derived limits that are NOT normalized: auto-repeat in
   PressedOnce (D_800C3900) stays vblank-clocked (32-vblank delay, then every
   4th vblank), as in retail. The step clock assumes the field loop calls
   FieldPollControllers once per iteration (misc5.c call sites), which is
   source fact, not timing proof.

Source guards pin func_8003634C order (prepare → ControllerPoll → Push),
the apply seam inside PrepareControllerPoll, the single post-mask merge call
in misc2.c, and NoteDrain at the start of the merge. FieldPollControllers'
drain and the remap helper are lifted verbatim from production. A mutant
that clocks steps per vblank fails the held invariant.

## Results

Docker xenogears-dev:clang (gcc 14 default): guards PASS. O0/O2/UBSan 15
checks each PASS. Mutant killed. Existing run_w34c1_scripted_input.sh (clang-18,
-Wconversion -Wsign-conversion -Werror) PASS. psyq_compat.c compile-only with
the port GFLAGS (gcc-13) OK.

## Pinned artifacts and integration (HELD)

Files touched: pc_port/src/test_input.c and pc_port/src/psyq_compat.c,
plus new test files. include/test_input.h is deliberately unchanged
(local externs), so TUs that include it don't change. Nothing in the
frozen counter recipe changed: base-source.tar (3a73eecc), counter-only.patch,
cache manifest, retained binary d3717bde and fixture 0fa6e676 are untouched.
Using the probe in a counter run means adding this commit's two production
files to the recipe, which rebuilds psyq_compat.o and test_input.o on top of
the three counter objects. That is Root's call; it is not done here.
