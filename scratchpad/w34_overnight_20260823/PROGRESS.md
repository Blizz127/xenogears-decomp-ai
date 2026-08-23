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
- Commit: pending; intended message `W34B29 advance flag/update lane
  0x8007185C-0x8007197C`.
