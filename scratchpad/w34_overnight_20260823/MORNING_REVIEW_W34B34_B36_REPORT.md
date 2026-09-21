# W34B34–W34B36 continuation report

## 1. Part 0 re-verification

All four requested items were confirmed: ROM SHA256, both upload-pump
instruction ranges and formulas, the `0x80071984` tail/backedge, and the
BE3C environment/OT relationship. Details are in
`AUDIT_REVERIFY_W34B34.md`, `AUDIT_REVERIFY_W34B35.md`, and
`AUDIT_REVERIFY_W34B36.md`.

## 2. Backedge policy

`D554` is a nonzero frame-run flag, not a self-clearing bounded timer. The
first natural `0x800719C8 -> 0x8007130C` edge was therefore captured as
LOG-AND-HOLD. Natural evidence is `D554=0x00000001`, `held=1`, `hit=1`;
the run remained `rc=0` without introducing a second pass.

## 3. Landed slices

| commit | slice | frontier delta | tests / natural |
| --- | --- | --- | --- |
| `2c9ec8f1` | W34B34 `0x80074F2C` upload pump | `0x80071984 -> 0x80075104` | O0/O2/UBSan 7/7, 4 mutants; `rc=0` |
| `9b3da910` | W34B35 `0x80075104` upload pump | `0x80075104 -> 0x80071994` | O0/O2/UBSan 7/7, 5 mutants; `rc=0` |
| `b314d2ec` | W34B36 tail `0x80071994..0x800719D0` | `0x80071994 -> 0x800719C8` | O0/O2/UBSan 4/4, guard mutant; `rc=0` |

## 4. Final natural frontier

`0x800719C8`, held at the first natural D554 backedge. The run captured
frame 916, frame-prologue entry 1, CD work calls 1, VSync retries 0, pad
iterations 1, scheduler calls 1, scheduler pass 2 entry, 29 callbacks, 0
missing callbacks, last callback `0x80087734`, last slot 15, and scheduler
frontier `0x8007106C`.

## 5. Milestones

- Milestone 1 (`0x80072238`): not reached; guard intact and zero-hit.
- Milestone 2 (`0x8007299C`): not reached; guard intact and zero-hit.
- Milestone 3: not reached; no framebuffer PNG.

The first backedge was naturally encountered, but `DrawOTag` itself was not
called because the live OT field was `0x005f1068`, outside the accepted
PSX/KSEG1 guest ranges. It was counted and safely skipped.

## 6. Tripwire status changes

No tripwire was weakened, removed, bypassed, or naturally retired. Existing
mode-loop, renderer, world DrawOTag, backedge, scheduler-dispatch, and
excluded-arc guards remain intact and zero-hit. The new frame-tail backedge
is a diagnostic hold, not a retired guard.

## 7. Not checked

The held second frame iteration, its callback-state changes, the full
DrawOTag execution with a valid guest OT, mode-loop entry, renderer entry,
and framebuffer pixels were not checked because doing so requires a reviewed
second-pass/backedge implementation.

## 8. Confirmation

Nothing was pushed. Quarantined tracked dirt in `include/psyq/inline_c.h`
and `pc_port/src/game_overrides.c` was not staged or modified. Banked proof
trees were not re-baselined or altered.

## 9. Exactly one next task

Implement the reviewed second-frame iteration beginning with a callback-state
census at the held `0x800719C8` backedge, preserving all current tripwires.
