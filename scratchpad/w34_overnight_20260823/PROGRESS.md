# W34 overnight progress — 2026-08-23

## W34B50 — live D554 callback branch census

- Frontier remains `0x800719C8`; no production source changed.
- On all three bounded re-entries, live slot 1 callback `0x8008A72C`
  entered with resync `0` and `wm_80090A84` returned `0`, so its class-1
  D554-clear arm was not selected. Live slot 4 callback `0x8008C844` also
  entered with resync `0`, so its `wm_80090C68`/ret-1 clear path was gated
  off.
- Natural result: rc=0; three tails with D554=1/held=1; scheduler entry 4,
  `53/53` executed, missing 0, invalid 0; mode/renderer/DrawOTag tripwires
  remain zero-hit.
- Evidence: `AUDIT_W34B50_D554_CALLBACK_CENSUS.md`,
  `slice_23_d554_callback_census.log`, and
  `w34b50_d554_callback_census.gdb`.
- Decision: class-(e) remains; no callback forced, no D554 store added, no
  tripwire changed, no push.
- Evidence correction: the first debugger revision printed `0x8001D554`;
  W34B51 corrected it to `0x8009D554` and reproduced D554=1 at every live
  callback entry and tail. The production conclusion is unchanged.

## W34B49 — D4 evidence hygiene

- Frontier remains `0x800719C8`; no production change.
- `git worktree prune -v` removed no metadata; all 49 registered worktree
  directories exist.
- Both banked scheduler proof manifests validate with exit 0: 6/6 and 8/8
  files `OK` when run from their recorded path contexts. An initial
  path-context failure was corrected; no hash mismatch or file mutation.
- Evidence: `AUDIT_W34B49_D4_EVIDENCE_HYGIENE.md`,
  `slice_22_d4_hygiene.log`.
- Decision: no deletion, no re-baseline, no implementation, no push.

## W34B48 — D3 tripwire hygiene

- Frontier remains `0x800719C8`; no production change.
- All 15 forbidden-target registry entries and `should_not_run` bodies are
  present. Mode `0x80072238`, renderer `0x8007299C`, world DrawOTag, loop,
  and excluded convergence guards remain intact and zero-hit naturally.
- Scheduler guest callback resolution still maps known values and logs/stops
  on missing or invalid values; CD40 remains map-known `0x80086700` with a
  counted unknown fallback. No second CD40-style function-pointer slot was
  found.
- Evidence: `AUDIT_W34B48_TRIPWIRE_HYGIENE.md`,
  `slice_21_tripwire_hygiene.log`.
- Decision: no implementation, no guard bypass, no push.

## W34B47 — D2 audit-ahead

- Frontier remains `0x800719C8`; no production change.
- Audited three next regions: frame-exit continuation
  `0x800719D0..0x80071A4C` (31 instructions, unresolved `0x80096694`),
  mode/session initializer `0x80072238..0x80072998` (472 instructions), and
  post-loop teardown `0x8007299C..0x80072BAC` (132 instructions, 26 calls).
- All three remain class-(e) in the current route. D1 found no class-(a/b)
  convergence gap; the first small-looking block is unreachable until D554
  clears and still has an unresolved cleanup leaf.
- Evidence: `AUDIT_W34B47_AUDIT_AHEAD.md`, `slice_20_audit_ahead.log`.
- Decision: no implementation, no tripwire change, no push.

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

## W34B37 — BE3C+0x70 leak and held-backedge census

- Starting proof reproduced at HEAD `0cfeaaeb`: frame 916, D554=1 held at
  `0x800719C8`, and the pre-fix OT leak `0x005f1068`.
- Retail re-audit proved `BE3C+0x70` is the selected 0x1000-byte OT root in
  one of two fixed draw-buffer records. Current-port `wm_8007369C` was the
  sole relevant writer and stored raw truncated HeapAlloc host pointers;
  BE3C itself was correctly guest-valued. Full audit: `AUDIT_W34B37_BE3C_OT.md`.
- Applied a minimal uncommitted writer conversion to `host_ptr_to_psx_u32`,
  plus the newly exposed `0x80071468` ClearOTagR `PSX_ADDR` boundary. Focused
  production-linked conversion test passed O0/O2/UBSan; production build
  passed `LINK OK`.
- Natural progression reached DrawOTag with mapped `p=0x5f2064` and then
  SIGSEGVed in PsyCross `ParsePrimitivesLinkedList` at `LIBGPU.C:456`.
  The W34B37 implementation is not committed because natural rc was not
  clean; do not paper over this crash.
- Full 16-slot census is banked in `slice_09_census.log`: twelve state-1
  slots predict implemented cb1 callbacks; slots 3, 6, 7, and 11 are state 3
  dormant. Retail backedge target re-derived as `0x8007130C`.
- Final report: `W34B37_REPORT.md`. No callback gap and no backedge loop were
  implemented. The next task is OT linked-list representation review.

## W34B37 follow-up — OT ABI/layout boundary (blocked)

- Targeted natural capture is banked in `slice_10_ot_representation.log`;
  `w34b37_ot_target.gdb` arms only after frame 916 reaches the held tail.
- DrawOTag entered naturally with `p=0x5f2064` and root
  `0x5f1068` (`p=root+0xFFC`). PsyCross then consumed malformed tag data and
  SIGSEGVed in `ParsePrimitivesLinkedList`.
- Audit `AUDIT_W34B37_OT_LINKS.md` establishes the cause: retail uses 0x400
  four-byte OT words in 0x1000 bytes, while the production non-extended
  PsyCross `OT_TAG`/`P_TAG` types are 8 bytes and ClearOTagR spans 0x2000
  bytes; guest 24-bit links and low-24 host links are also incompatible.
  This is class-(e), requiring morning review of a world-only adapter versus
  a PsyCross-wide ABI repair.
- No production fix, renderer entry, second-frame iteration, or backedge
  change was attempted after the crash. The W34B37 writer/ClearOTag changes
  remain uncommitted; quarantine files remain untouched.

## Slice 12 — W34B38 world-map guest-OT adapter

- Frontier: DrawOTag ABI crash (ParsePrimitivesLinkedList SIGSEGV) ->
  clean guest-native walk; first D554 BACKEDGE observed and held at
  0x800719C8 (`fp_cut_pc=0x800719c8`).
- Ground truth re-established (`slice_12_ground_truth.log`): with the
  W34B37 writer fix the OT root is genuinely guest (0x800A2228); crash
  reproduced at root+0xFFC before the adapter.
- Design note: `AUDIT_W34B38_ADAPTER_DESIGN.md`. Decisive fact: the
  non-extended P_TAG first word is byte-identical to the retail tag, so
  DrawPrim is the narrow host boundary; only ClearOTagR stride and
  nextPrim's link namespace mismatched.
- Implementation: `world_map_ot_adapter.c/.h`; frame-driver ClearOTagR
  call sites and the tail DrawOTag call routed through the adapter.
  PsyCross untouched. Entry-0 terminator substitution documented
  (retail links the 0x8005698C sentinel; port writes 0x00FFFFFF).
- Certificate: `run_w34b38_ot_adapter_prod_test.sh` O0/O2/UBSan-O2 all
  7/7, byte-identical stdout, mutants M1-M4 (link mask, terminator,
  clear direction, missing bounds check) 4/4 DETECTED
  (`slice_12_tests.log`).
- Natural run (`slice_12_natural.log`): rc=0; pass 1 16/16, pass 2
  29/29 missing=0; upload pumps 2+3 transfers 0 unknowns; adapter walk
  executed for the first time and aborted safely at bucket 0x320 whose
  word is 0x00092EA4 (expected cleared link 0x0A2EA4; delta exactly
  0x10000); packet address 0x80092EA4 is overlay code bytes
  (0x3C01800A). Chain dump: `slice_12_chain.log`. Backedge held; mode
  loop 0x80072238 and renderer 0x8007299C remain zero-hit; placeholder
  entered cleanly.
- Commits: `71f9c56d` (W34B37 writer fix, landed once the crash was
  resolved), `090f075d` (adapter + call sites + certificate).
- Next slice subject (W34B39): who writes bucket 0x320 with the
  0x10000-off link — hardware-watchpoint diagnosis.

## Slice 13 — W34B39 fix particle cleanup slot-table base

- Audit: `AUDIT_W34B39_BUCKET320.md`. Hardware watchpoint at bucket `0x320`
  fired first in `wm_80089580` called by `wm_80089748` from scheduler slot 14
  (`0x80071A58`). The cleanup `sh slot_record+0x0A` changed `0x000A2EA4` to
  `0x00092EA4` because the source loaded `0x8009BDE0`; retail loads the slot
  table pointer from `0x8009BCC0` at `0x800896FC..0x80089708`.
- Implementation: `world_map_helper_89580.c` now uses the retail BCC0 global.
  No OT, renderer, callback routing, or second-frame/backedge code changed.
- Focused certificate: O0/O2/UBSan-O2 all `6/6`, byte-identical; three address
  mutants detected (`slice_13_tests.log`).
- Rebuild: `LINK OK` (`slice_13_build.log`).
- Natural run: rc=0; scheduler pass 1 `16/16`, pass 2 `29/29`, missing=0;
  guest OT walk reaches terminator `0x8009CE6C`, submits 2 packets, takes 1025
  steps, and records zero range/alignment/length/step aborts. The old bucket
  `0x320` abort is gone (`slice_13_natural.log`). First D554 backedge remains
  held at `0x800719C8`; mode loop `0x80072238` and renderer `0x8007299C`
  remain intact and zero-hit.
- Frontier: `0x800719C8` held tail -> completed first guest-native OT walk;
  no milestone 1/2/3 yet.

## W34B40 — held-backedge and audit-ahead review

- Frontier before -> after: control frontier remains `0x800719C8` held;
  W34B39's OT sub-frontier remains clean at terminator `0x8009CE6C`.
- Audit: `AUDIT_W34B40_AHEAD.md`. D1 found no uncovered class-(a/b)
  convergence gap. The frame re-entry `0x800719C8 -> 0x8007130C`, mode
  initializer `0x80072238..0x80072998`, and post-loop region
  `0x8007299C..0x80072BAC` are class-(e) or explicitly deferred.
- Tests: no production code changed; prior W34B39 focused O0/O2/UBSan
  certificate and natural rc=0 evidence remain the current proof.
- Natural state: mode entry `0x80072238` and `0x8007299C` remain zero-hit;
  first backedge is held; no second frame, renderer entry, or framebuffer PNG.
- Commit: no commit; audit-only handoff update. No push.

## W34B41 — fresh held-edge callback census

- Frontier before -> after: control frontier remains `0x800719C8` held;
  no production frontier change.
- Fresh capture: frame 916, scheduler entry 2, pool `0x800D7538`, full final
  16-slot state table in `slice_14_census.log`; natural placeholder reached
  with rc=0.
- Retail proof: `0x800719C8` branches to frame head `0x8007130C`; the next
  scheduler call is the unconditional per-frame site `0x80071488` after the
  frame-head/input/CD synchronization path.
- Prediction: final held state has 12 state-1 cb1 candidates (slots 0, 1, 2,
  4, 5, 8, 9, 10, 12, 13, 14, 15), four state-3 dormant slots (3, 6, 7,
  11). All 12 cb1 addresses are explicitly resolved and linked in
  `world_map_scheduler.c`; no callback gap found.
- Counter note: scheduler `state1=13/state3=3` is pre-callback accounting;
  slot 11 returned 3, yielding the final held-tail table's 12/4 split.
- Audit: `AUDIT_W34B41_CALLBACK_CENSUS.md`. No second frame or callback
  implementation attempted.
- Commit: pending evidence commit; no push.

## W34B42 — reviewed one-frame re-entry

- Frontier before -> after: held control edge `0x800719C8` -> same retail
  edge after one additional frame; frame execution count advanced 1 -> 2.
- Implementation: one bounded `XENO_WORLD_FRAME_REENTRY_ONCE` re-entry from
  the D554 branch target `0x8007130C`, using the production frame prologue
  and the exact nonzero D554 predicate. No mode-loop/renderer path changed.
- Focused test: O0/O2/UBSan-O2 `5/5`; gate, D554-zero, D554-nonzero, and
  high-value cases; three wrong-logic mutants detected.
- Build: LINK OK.
- Natural run: rc=0; tail hit 2; OT draw calls 2; packets 4; 2050 walk
  steps; both walks terminate at `0x8009CE6C`; zero adapter aborts; scheduler
  entry 3, `41/41`, missing 0, invalid 0. Mode `0x80072238` and
  `0x8007299C` remain zero-hit.
- Evidence: `AUDIT_W34B42_SECOND_FRAME.md`, `slice_15_tests.log`,
  `slice_15_build.log`, `slice_15_natural.log`.
- Commit: pending local implementation/evidence commit; no push.

## W34B46 — D554 clear-writer census

- Frontier remains `0x800719C8`; no production change.
- Retail inventory: frame seed `0x80071308`, frame-local clear lanes
  `0x80071830` and `0x80071954`, plus the 15 external clear sites listed in
  `AUDIT_W34B46_D554_CLEAR_WRITERS.md`.
- Current direct writers: live callback bodies `0x8008A72C` and `0x8008C844`,
  frame seed, and an unused legacy driver copy. No other current source
  store exists; `0x8008E76C` only defines the address.
- Classification: `BLOCKED-NEEDS-REVIEW` class (e). The frame-local region is
  183 instructions with unresolved callees, and the other retail writers
  need separate callback/helper audits. The two live callback predicates
  remained inactive over three clean tails.
- Evidence: `AUDIT_W34B46_D554_CLEAR_WRITERS.md`,
  `slice_19_d554_audit.log`; W34B45 natural/census evidence remains rc=0.
- Decision: no implementation, no D554 clear, no tripwire change, no push.
- Commit: `90c9ce77` evidence/audit-only.

## W34B45 — third-tail callback census

- Frontier remains `0x800719C8`; no production change.
- Fresh full table at scheduler entry 4 is identical to W34B41/W34B43:
  12 state-1 cb1 candidates, four state-3 dormant slots, unchanged payloads
  and callback addresses, no callback gap.
- Natural state: D554=1; scheduler `53/53`, missing 0, invalid 0; CD
  dispatcher entry 4 with no I/O failures; rc=0.
- Audit: `AUDIT_W34B45_THIRD_CENSUS.md`; next subject is the D554 clear-writer
  census, not another blind frame extension.
- Commit: `aa4b71d2` evidence-only commit; no push.

## W34B43 — post-second-frame callback census

- Frontier: remains `0x800719C8`; no production code changed.
- Fresh second-tail census: scheduler entry 3, full table in
  `slice_16_census.log`. The final table is byte-for-byte identical to
  W34B41: 12 state-1 cb1 candidates, 4 state-3 dormant slots, unchanged
  callback addresses/payloads, no newly eligible callback.
- Natural state: D554 remains 1; scheduler `41/41`, missing 0, invalid 0;
  CD dispatcher entry 3 with one busy dispatch and no I/O failures; rc=0.
- Audit: `AUDIT_W34B43_POST_SECOND_CENSUS.md`.
- Commit: pending evidence-only commit; no push.

## W34B44 — finite two-reentry diagnostic bound

- Frontier remains `0x800719C8`; two additional reviewed frames now execute
  before the diagnostic bound stops.
- Implementation: `XENO_WORLD_FRAME_REENTRY_TWICE=1` selects limit 2 while
  preserving the nonzero D554 predicate before each call; ONCE behavior is
  unchanged.
- Tests/build: predicate O0/O2/UBSan-O2 `5/5`, 3/3 mutants detected; LINK OK.
- Natural: rc=0; three tails, three OT submissions, six packets, 3075 walk
  steps, zero adapter aborts; scheduler entry 4, `53/53`, missing 0,
  invalid 0. D554 remains 1; mode/renderer tripwires remain zero-hit.
- Evidence: `AUDIT_W34B44_TWO_REENTRY.md`, `slice_17_tests.log`,
  `slice_17_build.log`, `slice_17_natural.log`.
- Commit: pending local implementation/evidence commit; no push.
