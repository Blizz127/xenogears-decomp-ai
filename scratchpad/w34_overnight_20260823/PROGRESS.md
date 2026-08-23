# W34 overnight progress — 2026-08-23

## Starting validation

- Worktree tracked dirt before work: quarantined `include/psyq/inline_c.h`
  and `pc_port/src/game_overrides.c` only; no other tracked dirt.
- W34B25 reviewed and committed first as `b0628a4d`:
  `W34B25 host CdSyncCallback 0x80096A6C, PSX_ADDR UI object, native 0x80086700`.
- Production rebuild: `LINK OK`.
- Ordinary banked `run.sh`: `rc=0`, archived one-pass proof 16/16,
  missing=0, frontier `0x8007106c` (matches the banked README).
- Prologue diagnostic baseline: `rc=0`; natural `0x800712D0` entry,
  second scheduler pass entry=2, callbacks executed=29, missing=0,
  `DrawSync`/`Vsync` after scheduler executed, hard cut `0x80071490`,
  placeholder entered cleanly. Mode loop `0x80072238` and renderer
  `0x8007299C` remained zero-hit.
- Baseline diagnostic log: `scratchpad/w34b5hs_natural_scheduler_verify/scheduler_capture_prologue_diag.log`.
- Baseline banked tree: `scratchpad/w34b5hs_natural_scheduler_verify/natural_scheduler_proof_c238e529_w34b25_f3d046a8/`.

## Slices

No overnight implementation slice has been attempted yet. The first audit
is `AUDIT_80071490.md`; it classifies `[0x80071490,0x800714D4)` as the
smallest bounded host-sync/display-environment continuation.

## Slice 01 — post-pass sync/display continuation

- Frontier: `0x80071490 -> 0x800714D4`.
- Implemented the retail calls at `0x80071490`, `0x80071498`, `0x800714A0`,
  `0x800714B0`, and `0x800714C0` in the existing frame-driver family.
  `PutDispEnv` and `PutDrawEnv` receive `PSX_ADDR`-mapped guest pointers.
- Focused production-linked certificate:
  `pc_port/tests/run_w34b26_80071490_prod_test.sh`, O0/O2/UBSan-O2 all
  `12/12`, normalized stdout identical.
- Prior W34B18-C certificate preserved in legacy cut-only mode:
  O0/O2/UBSan `97/97`, 3/3 real helper mutants killed, 14/14 frame mutants
  killed.
- Production rebuild: `LINK OK`.
- Natural prologue diagnostic: `rc=0`; pass 1 `16/16`, pass 2 `29/29`,
  missing `0`, `fp_cut_pc=0x800714d4`, post-pass DrawSync/Vsync/controller
  tripwires executed, mode-loop `0x80072238` and renderer `0x8007299C`
  remained zero, placeholder entered cleanly.
- Evidence: `slice_01_natural.log`, `slice_01_natural_reanchored.log`,
  and `slice_01_prologue_results.txt` in this directory.
- Commit: `286c4c10` (`W34B26 extend frame driver 0x80071490-0x800714C4 sync/display tail`).

## Slice 02 audit

- New frontier: `0x800714D4`.
- Audit: `AUDIT_800714D4.md`.
- Classification: class (a), bounded branch/state region through the common
  `BD34=0` store at `0x80071698`; natural next frontier `0x8007169C`.

## Slice 02 — frame state gates through 0x80071698

- Implemented the fresh-load state gates at `0x800714D4..0x80071698`,
  including the signed `BD24/CE68` comparison and the common `BD34=0`
  store. The all-gates-pass case records the exact next call frontier
  `0x80071578`; the natural fixture takes the `0x8007169C` frontier.
- Focused production-linked certificate:
  `pc_port/tests/run_w34b27_800714d4_prod_test.sh`, O0/O2/UBSan-O2 all
  `8/8`; natural, all-gates, and first-gate-fail cases are covered.
- Production rebuild: `LINK OK`.
- Natural prologue diagnostic: `rc=0`; pass 1 `16/16`, pass 2 `29/29`,
  missing `0`, `fp_cut_pc=0x8007169c`, placeholder entered cleanly.
  Mode-loop `0x80072238` and renderer `0x8007299C` remain zero-hit.
- Evidence: `slice_02_natural.log` and `slice_02_prologue_results.txt`.
- Commit: `99fbd14f` (`W34B27 advance frame state gates
  0x800714D4-0x80071698`).

## Slice 03 audit

- New frontier: `0x8007169C`; audit: `AUDIT_8007169C.md`.
- The captured natural mode-entry state has `C178=1`, so the direct
  `0x800716A8` branch reaches `0x8007185C` without a call. Alternate lanes
  contain absent helpers and remain held at their first unresolved call.
- Classification: class (a) for the natural branch; implement only the
  fresh `C178` load/branch and then audit `0x8007185C` separately.

## Slice 03 — natural C178 branch

- Frontier: `0x8007169C -> 0x8007185C` on the captured natural state.
- Implemented the retail `lw C178` / `bnez` at `0x8007169C..0x800716A8`.
  `C178 != 0` advances to the next bounded region; `C178 == 0` is held at
  the first unresolved helper frontier `0x80071704`.
- Focused production-linked certificate:
  `pc_port/tests/run_w34b28_8007169c_prod_test.sh`, O0/O2/UBSan-O2 all
  `6/6`; offset, width, and sign canaries are included. The W34B27
  certificate remains `8/8` in its continuation-disabled mode.
- Production rebuild: `LINK OK`.
- Natural prologue diagnostic: `rc=0`; pass 1 `16/16`, pass 2 `29/29`,
  missing `0`, `fp_cut_pc=0x8007185c`, placeholder entered cleanly.
  Mode-loop `0x80072238` and renderer `0x8007299C` remain zero-hit;
  the existing renderer sentinel also reports zero.
- Evidence: `AUDIT_8007169C.md`, `slice_03_natural.log`,
  `slice_03_prologue_results.txt`, and `slice_03_tests.log`.
- Commit: `4a19846a` (`W34B28 advance natural C178 branch
  0x8007169C-0x8007185C`).

## Slice 04 audit

- New frontier: `0x8007185C`; audit: `AUDIT_8007185C.md`.
- Classification: class (a) for the captured natural lane; implement the
  BD10/D80C/EE76 flag operations, fresh C178 branch, and D804 clear. Hold
  the alternate call-bearing lane before `0x800758C0`.

## Slice 04 — natural flag/update lane

- Frontier: `0x8007185C -> 0x8007197C` on the captured natural state.
- Implemented the retail D80C clear, BD10 bit-0x100 EE76 halfword toggle,
  fresh C178/D804/D554/BE10 gates, and D804 clear at `0x80071978`.
  The alternate helper-bearing state stops at `0x800718E8`.
- Focused production-linked certificates: B29 O0/O2/UBSan-O2 all
  `10/10`; B28 `6/6`, B27 `8/8`, and B26 `12/12` compatibility suites
  all pass after their continuation guards.
- Production rebuild: `LINK OK` (`slice_04_build.log`).
- Natural prologue diagnostic: `rc=0`; pass 1 `16/16`, pass 2 `29/29`,
  missing `0`, `fp_cut_pc=0x8007197c`, placeholder entered cleanly.
  Mode-loop `0x80072238` and renderer `0x8007299C` remain zero-hit;
  the renderer sentinel remains zero.
- Evidence: `AUDIT_8007185C.md`, `slice_04_natural.log`,
  `slice_04_prologue_results.txt`, `slice_04_tests.log`, and
  `slice_04_build.log`.
- Commit: `0c46ba17` (`W34B29 advance flag/update lane
  0x8007185C-0x8007197C`).

## Slice 05 audit

- New frontier: `0x8007197C`; audit: `AUDIT_8007197C.md`.
- Classification: class (c) host-boundary defect at the already-linked
  `0x80025044` image-list transfer. The next `0x80074F2C` helper is absent
  and larger than a leaf, with a later DrawOTag/backedge sequence.

## Slice 05 — mapped image-list transfer

- Frontier: `0x8007197C -> 0x80071984`.
- Added a production-linked mapped wrapper for the retail image-list call:
  known PSX/KSEG1 record/data pointers use `PSX_ADDR`; unknown context,
  head, or data values log and increment `wm_fp_get_image_unknowns()`.
  The wrapper clears the consumed list and never dereferences an unknown
  guest value as a host pointer.
- Focused production-linked certificate:
  `pc_port/tests/run_w34b30_8007197c_image_transfer_prod_test.sh`,
  O0/O2/UBSan-O2 all `7/7`; empty, known load/clear, chain, unknown
  pointer, and invalid-context cases are covered. Prior B26-B29 suites
  remain green.
- Production rebuild: `LINK OK` (`slice_05_build.log`).
- Natural prologue diagnostic: `rc=0`; pass 1 `16/16`, pass 2 `29/29`,
  missing `0`, `fp_cut_pc=0x80071984`, placeholder entered cleanly.
  No unknown image-list value occurred naturally. Mode-loop
  `0x80072238` and renderer `0x8007299C` remain zero-hit.
- Evidence: `AUDIT_8007197C.md`, `slice_05_natural.log`,
  `slice_05_prologue_results.txt`, `slice_05_tests.log`, and
  `slice_05_build.log`.
- Commit: `14a7119c` (`W34B30 map 0x80025044 image-list boundary
  0x8007197C-0x80071984`).

## Slice 05 detours and blocked frontier

- D1 convergence audit: `DETOUR_D1_CONVERGENCE_AUDIT.md`. No bounded gap
  exists between the built convergence/common-tail pieces and the guarded
  mode-loop entry; no tripwire was weakened or retired.
- D2 audit-ahead packets:
  `AUDIT_AHEAD_80071984.md`, `AUDIT_AHEAD_80074F2C.md`, and
  `AUDIT_AHEAD_80075104.md`.
- Blocked frontier: `0x80071984`, class (e). Crossing it requires the two
  absent overlay helpers, a guest-pointer DrawOTag handoff, and a live
  frame backedge with unresolved callback-pass policy. No overnight code
  crossed this boundary.

## D3/D4 detours

- D3 tripwire audit: `DETOUR_D3_TRIPWIRE_AUDIT.md`. All 15 registry entries
  remain intact; natural mode-loop, renderer, DrawOTag, backedge, and
  excluded-arc guards remain zero-hit. W34B30’s unknown-pointer negative
  path is covered by its focused certificate and did not fire naturally.
- D4 evidence hygiene: `DETOUR_D4_EVIDENCE_HYGIENE.md`. The banked proof
  tree validates fully with `sha256sum -c SHA256SUMS`. `git worktree prune`
  removed no registered worktree metadata; no worktree directories were
  deleted.
- Detour commit: pending; this audit and progress update are the final
  overnight slice.

## W34B34 morning review — 0x80074F2C upload pump

- Fresh retail re-decode and Part 0/1 policy are banked in
  `AUDIT_REVERIFY_W34B34.md`; all four audit items were confirmed. The
  first D554 backedge policy is LOG-AND-HOLD and the backedge itself remains
  unimplemented in this slice.
- Implemented native `0x80074F2C` (`[0x80074F2C,0x8007502C)`, 64
  instructions) with exact signed count, u16 timer/index progression, and
  map-or-log LoadImage pointer handling. Added the production call at
  `0x80071984`, with the new cut before `0x80075104`.
- Focused certificate: `slice_06_tests.log`, O0/O2/UBSan `7/7`, four
  executed mutants all detected. Production build: `slice_06_build.log`,
  `LINK OK`.
- Natural diagnostic: `slice_06_natural.log`, `rc=0`, natural count `2`,
  transfers `2`, unknowns `0`, scheduler entry `2`, callbacks `29`, missing
  `0`, frontier `0x80075104`, placeholder clean. The D554 backedge was not
  encountered because the slice stops before its tail.
- Commit: pending; intended message `W34B34 implement world image-timer
  helper 0x80074F2C boundary 0x80071984-0x80075104`.

## W34B35 morning review — 0x80075104 sibling upload pump

- Confirmed the sibling’s 73-instruction retail body and distinct signed
  dimension/factor scaling formula; audit: `AUDIT_REVERIFY_W34B35.md`.
- Implemented native `0x80075104` (`[0x80075104,0x80075228)`) with exact
  shared timer/index progression, signed indexed table lookup, wrapped
  dimension scaling, and map-or-log LoadImage handling. The production call
  now stops before `SetGeomOffset` at `0x80071994`.
- Focused certificate: `slice_07_tests.log`, O0/O2/UBSan `7/7`, five
  executed mutants all detected. Production build: `slice_07_build.log`,
  `LINK OK`.
- Natural diagnostic: `slice_07_natural.log`, `rc=0`, W34B34 transfers `2`,
  W34B35 transfers `3`, unknowns `0`, scheduler entry `2`, callbacks `29`,
  missing `0`, frontier `0x80071994`, placeholder clean. D554/backedge was
  not encountered.
- Commit: pending; intended message `W34B35 implement world image-timer
  sibling 0x80075104 boundary 0x80075104-0x80071994`.

## W34B36 morning review — frame tail and first backedge hold

- Fresh Part 0 re-verification is banked in `AUDIT_REVERIFY_W34B36.md`.
  The retail tail was confirmed through `0x800719C8`; `D554` is an
  unbounded frame-run flag, so the first natural backedge is LOG-AND-HOLD.
- Implemented the bounded tail `0x80071994..0x800719D0`: exact
  `SetGeomOffset`, PSX/KSEG1 map-or-log `DrawOTag` handoff, and a logged
  first-backedge hold. No raw guest pointer is called and no backedge is
  re-entered.
- Focused certificate: `slice_08_tests.log`, O0/O2/UBSan `4/4`; the
  unknown-pointer mutant was detected. Production rebuild: `slice_08_build.log`,
  `LINK OK`, 47 compiled / 0 skipped.
- Natural diagnostic: `slice_08_natural.log`, `rc=0`. At frame 916 the
  environment was known, OT `0x005f1068` was unknown and counted, so
  `DrawOTag` was safely skipped; `D554=1` then fired naturally at
  `0x800719C8` and was held. Scheduler pass 2 remained 29 executed / 0
  missing; placeholder entered cleanly. Mode-loop and renderer remain
  zero-hit.
- Commit: `b314d2ec` (`W34B36 route SetGeomOffset DrawOTag and hold first
  0x800719C8 backedge`).
