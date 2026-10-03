# Xenogears handover: Codex to Claude Opus 5.5

Owner-authorized routing change, 2026-10-03. Recorded 2026-10-03 03:35:26Z. Codex stops at this clean local
checkpoint; Claude becomes the single writer after the coordinator's
**HANDED OVER** report. No push, publication, new long job, job cancellation,
queue movement or runner-guard change was performed for this handover.

## Checkout and exact checkpoints

Use **`/home/blizz/dev/xenogears-decomp-codex-native`**, branch
**`codex/native-port-20261002`**. Its implementation checkpoint is
**`3a73eecc668052dc69ca2e46d8337190fc1df700`**. This HANDOVER.md is committed in
one subsequent documentation-only commit; its exact SHA is the checkout's
`git rev-parse HEAD` and is reported on coordinator task **f5b072578f5f**.
The implementation checkout was clean before refreshing this file and must be
clean at the handover report. No additional changes are needed to collect the
already-running job.

Recent local commits, oldest first:

- `4234d0f6c14e13a7fc3ed3364a37b11e89b7edda`: isolated Codex ownership/route gaps.
- `f05ec5a973dc675285dc84f5c2d3e215fe66b675`: separate goal metrics, dashboard
  path/inventory fixes and isolated RESULTS route driver.
- `02d5ab752d6b5f08ecc5d40252892e2ccbbef634`: fail-closed CPU-interpreter-disabled
  production diagnostic and O0/O2/UBSan mechanism tests.
- `54e3996c3d323dfc1647eef04e4d748ff610ecf8`: completed runtime gate receipts and
  matched-source/production object ownership inventory.
- `3a73eecc668052dc69ca2e46d8337190fc1df700`: opt-in RESULTS entry/render/return
  canonical-memory snapshots, RESULTS-only reference control, battle-frame
  input/hash logger and focused tests; production gate **in flight**.

The user terminal's original worktree
`/home/blizz/dev/xenogears-decomp-ai-archerfish-20260927` remains on
`codex/r6-dev-menu-wip` at `285f8a2e406833d8b14284f57c6d08d33f99815e`.
Its untracked AGENTS.md, CLAUDE.md, debug_compiled_object.o and debug_source.c
are preserved. Do not confuse it with the current implementation checkout.
The battle-results worktree is at `f334836f` on
`codex/battle-results-native-resume-20261002`; **leave its uncommitted
infra-scratch/leaves3 untouched**. Claude's earlier Dangerous rm permission
prompt remains the owner's decision. No holds or prompts were answered.

## Owner goal and separate measurements

Owner goal: aim for **85% native and matched, carried into the port**. It is
aspirational, with no hard deadline or stop; keep advancing bounded milestones.
Never count word exports, carved data, assembly-backed functions or snapshots
as recovered/native C. Never infer native runtime coverage from match counts.

| Measurement | Latest verified evidence |
|---|---|
| (a) Authoritative matched functions | **2704 / 4464 = 60.57%**, with all ten pinned ROM hashes PASS at `4234d0f6` |
| Supporting matched code bytes | **388028 / 1383684 = 28.04%** |
| (b) Production execution with CPU interpreter disabled | **3000-frame field diagnostic smoke PASS**; accepted useful playable slices **0**, coverage percentage **unknown** |
| (c) Matched sources actually linked and tested in the port | **102 battle bodies** in the verified owner/object/link/test intersection; this is a bounded subset, whole-port total **unknown** |

The matching denominator covers ten pinned targets and includes SDK functions
inside the executable category. It is not the entire game/disc archive scope;
RESULTS/intro are outside it. The earlier README's 2651 count was stale: no new
53-function gain was claimed. Reaching 85% of this denominator needs 3795
matches, **1091 additional verified matches**. No sustained conversion rate
has been measured; weeks to months is only a planning scale, not a promised
completion date. See `docs/evidence/native-goal-85-20261003.md`.

The 102-body inventory excludes five port-only definitions and checks exact
matched owner unit, strong production object/final symbol and link arguments
(including collision renames). Host adaptations exist. Every allowlisted leaf
has a completed differential case in each compiler-mode run; **888 inconclusive
and 20 hand-only cases remain**, so this is bounded test acceptance, not
whole-program correctness or production execution coverage.

## Current milestone and immediate next step

**Collect and assess the running RESULTS route gate below. Do not resubmit it.**
It builds and tests the new recorder/reference control, then attempts two
RESULTS-only-interpreted reference runs. Native adoption is attempted only
when both references returned and their snapshots are stable. The route uses
the previously observed **map-1 empty-party diagnostic fixture**: a return
is not a valid victory/useful playable slice. Never promote it as one.

Snapshots cover bus-visible 0x235C game-state/transition bytes, the 0x6500
RESULTS module/workspace, battlers/actions/enemy AI/group state at entry,
DrawOTag boundaries and the observed return at saved RA/SP in the same CPU.
They see native helper writes as well as interpreted writes. They do **not**
prove full RAM, timing, audio, visual or save parity. Missing returns, read/I/O
failures, truncation, unstable references and >512 RESULTS render boundaries
fail the evidence closed. **Snapshot binaries may contain retail code: private
runner/staging only, never source or published packages.**

`XENO_BATTLE_RESULTS_FORCE_INTERPRET=1` disables only RESULTS adoption, not
other native bridges. `XENO_BATTLE_RESULTS_SNAPSHOT_DIR` enables snapshots;
`XENO_BATTLE_STATE_HASH_LOG` enables diagnostic battle-frame logs/input, while
its decoded W/store lines omit native writes and must not be used as parity.
The process driver isolates HOME/config/cards and a copied checkpoint. Its
`XENO_RESULTS_DIRECT_FIELD=1` mode uses map 1 instead of a checkpoint.

If the gate succeeds behaviorally, assess actual identity/native diagnostics
and fallback statistics, then establish a valid progression fixture and
input/visual/audio/timing/save evidence. If reference runs differ or fault,
diagnose that instead of claiming native parity. Next caller candidate
`801E3A18` still requires a **real guest stack frame and helper call-order
recorder/contract proof**. Do not adopt the parked caller drafts before those
proofs. Independent pinned ROM/objdiff verification is required for matching
credit; RESULTS semantic reconstructions alone do not add to (a).

## Running job: let finish, then collect

Runner: **blizzpc-bazzite, `blizz@100.87.173.93`**, existing
`~/devbox-runner/bin/submit` infrastructure.

**`20261002-222154-xeno-results-route-3a73eecc-1551701`**

- Source: `3a73eecc668052dc69ca2e46d8337190fc1df700`.
- Verified archive SHA256:
  `f9007302068f4cfe6739dfcb6e949523b180408c17ae0fb968ae1adcf3abc9e0`.
- Latest read-only observation: **RUNNING**, fixed snapshot **2838 files**,
  matching prebuild complete, `rom-check rc=0`; host build in progress.
  No exit receipt yet. This is **not** recorded as a gate PASS.
- Commands: `~/release-staging/xeno-work/native-goal-85/results-route-gate-commands.sh`.
- Submit receipt: same directory, `results-route-submit.log` and
  `results-route-queue-before-submit.txt`.
- Remote artifacts: `~/devbox-runner/work/<job>/out/`.
- Remote status: `~/devbox-runner/logs/<job>.exit`; full job log `<job>.log`.

Read-only collection:

```sh
JOB=20261002-222154-xeno-results-route-3a73eecc-1551701
ssh blizz@100.87.173.93 "cat ~/devbox-runner/logs/$JOB.exit; cat ~/devbox-runner/work/$JOB/out/gates.txt"
ssh blizz@100.87.173.93 "tail -30 ~/devbox-runner/logs/$JOB.log"
```

Absence of the exit receipt means running/queued, not failure or PASS. Fetch
small logs/receipts first, hash them on blizzpc with `sha256sum`, copy via scp,
and verify local SHA256 before using them. Leave private raw state snapshots
and large captures on the runner unless needed. The wrapper may print
permission-denied cleanup messages for container-owned generated `src/` files;
assess `out/gates.txt` and explicit exit receipt, not the cleanup tail alone.

## Completed verification and receipts

`20261002-215713-xeno-85-runtime-02d5ab75-1416327` is finished, **overall exit1**:
ROM/build/store-v0, disabled mechanism O0/O2/UBSan, RESULTS/overlay/leafbridge/
guestcall/file1 suites, normal OFF identity and disabled-interpreter field
smoke PASS. Source guard: **2831 real indexed files PASS**. Binary guard:
**19 reference runs / 25068 bytes FAIL**. Normalized reference file/address/
length/symbol set is unchanged from the prior baseline; it remains a release
blocker. The field smoke has 432 paired keys, zero mismatches, 16 ambiguous
keys and both fail-closed startup markers, not full playable-slice acceptance.

Twenty small receipts (about1.2MB) were SHA256 verified under
`~/release-staging/xeno-work/native-goal-85/runtime-02d5ab75/`, including
`sha256-readback.json` and `binary-baseline-comparison.json`.

Census job `20261002-214543-xeno-85-census-4234d0f6-1305133`: ROM and objdiff
report completed. Overall exit1 was a dashboard relative-path formatting
exception **after** report generation, now corrected without rerunning the
census. Verified receipts: `native-goal-85/census/`, including progress.json,
objdiff.json and corrected DASHBOARD-path-fixed.md. Metrics are in
`native-goal-85/metrics-baseline.json`; reproducible production inventory is
`tested-port-inventory.json` generated by
`tools/scripts/matched_port_inventory.py`.

Prior batch-3 job `20261002-140340-xeno-battle-results-batch3-6e189169-4116556`:
all six behavioral suites PASS, 15 RESULTS leaves x1530 cases/516 controls per
O0/O2/UBSan mode; meter/mutation controls PASS. Overall exit1 on the same19
binary findings. Record: `docs/evidence/battle-results-batch3-20261002.md`;
verified receipts: `~/release-staging/xeno-work/codex-handover-20261002/evidence/`.

At current `3a73eecc`, short local synthetic tests pass: trace off/native-write/
return/read-failure controls in all3 modes (9PASS), comparator4PASS, isolated
process driver3PASS, strict compilation of the imported state-hash recorder,
`git diff --check`. New RESULTS-only differential controls and the production
recorder still await the in-flight job; do not call them runner-gated yet.

**Every old empty-index runner retail_data_guard PASS remains UNVERIFIED.**
Only fixed nonempty-index checks are evidence. Never suppress real findings.

## Build/test/run commands and runner limits

All long builds, full suites, reports/census and runtime routes stay on blizzpc.
**One heavy job per lane**, check queue before submit, PE/PlanetSide priority;
existing disk/memory/temperature/battery/gaming/canary limits apply. At the last
observation runner free space was about32GB against its owner-set30GB floor.
Do not change the guard, move/resubmit jobs or interfere with other lanes.
No visible windows: headless/offscreen or authorized Xvfb only. No owner-device
launches outside existing authorization. Small focused tests/edits may be local;
no large /tmp files (tmpfs), use staging/scratch disk.

The existing wrapper was used through its task-specific preserved copy:
`~/release-staging/xeno-work/codex-handover-20261002/xeno_runner_handover.sh`.
It uses submit, archive transfer SHA readback, fixed safe.directory/nonempty
snapshot, existing clang toolchain image, readonly owner data mount and private
HOME/TMPDIR/XDG paths. **Do not run this again while the current job is active**.
For a future necessary gate after collecting it:

```sh
TMPDIR="$HOME/release-staging/xeno-work/native-goal-85/scratch" XRR_IMAGE_TAG=clang   bash "$HOME/release-staging/xeno-work/codex-handover-20261002/xeno_runner_handover.sh"   -t 7200 -j 2 /home/blizz/dev/xenogears-decomp-codex-native NEW-TAG   "$(cat "$HOME/release-staging/xeno-work/native-goal-85/results-route-gate-commands.sh")"
```

The wrapper prebuilds `make build`; gate commands then run `make rom-check`,
`bash pc_port/build_port.sh`, six battle suites/trace controls/OFF identity,
headless routes, and `make port-guards`. Adapt source-commit/tag for a **new**
candidate; don't relabel the running snapshot. Standalone local focused tests:

```sh
RESULTS_TRACE_TEST_OUT="$HOME/release-staging/xeno-work/native-goal-85/results-trace-focused"   bash pc_port/tests/run_battle_results_trace_test.sh
RESULTS_TRACE_TEST_OUT="$HOME/release-staging/xeno-work/native-goal-85/results-trace-focused"   python3 pc_port/tests/battle_results_trace_compare_test.py
python3 -W error::ResourceWarning pc_port/tests/battle_results_route_driver_test.py
```

## Native/hybrid, blockers and private publication

The port remains **hybrid**. Fifteen identity-gated RESULTS leaves and169
battle allowlist bodies have bounded differential evidence; roughly48 other
RESULTS routines, intro and other battle paths still execute CPU instructions.
`XENO_DISABLE_CPU_INTERPRETER=1` blocks instruction execution before fetch and
permits native bridges, but does not convert any code. No useful playable
battle/native percentage claim is accepted. The fallback meter's call-share
heuristic is diagnostic, not a native-runtime coverage metric.

Open blockers: valid gameplay progression/results fixtures; caller stack and
call-order contracts; random-state/callback contracts; host-vs-guest menu/shop
structure layout; missing functions/stubs/inconclusive cases; **19 binary
retail-data findings**. ROUND-2 port-equivalent ledger590 blocked+80 port-unsafe
functions remains separate pending work; no inflated metric was derived from it.

**No new private package has been promoted/published by Codex.** The current
candidate fails the binary data gate. Owner-authorized frequent OWNER-ONLY
builds can follow relevant identity/release/data gates and artifact hash/size
readback plus launcher update detection. Preserve rollback, saves/settings,
Original/addons-off behavior, disclosures and existing credential holds.
Source commits are not published builds. No public push/release, account/access
policy change, competing publisher, forced launcher restart or owner launch.

## HD-2D task83b32c4c7ff6: preserve and continue separately

Branch `codex/hd2d-octopath`, checkout `/home/blizz/dev/xeno-hd2d-octopath`,
HEAD **`10d46904f721c340deacde61542c49d300d4947a`**. Existing untracked
AGENTS.md/CLAUDE.md, drafts/grid audit and authoring scripts remain untouched;
do not blindly add private/retail-derived art to source to make it clean.
Approved direction: detailed-v2 retail-faithful exact2x per part, all Fei
facings/frames required, art opt-in Enhanced and OFF by default. RejectedV15
art/6c06976b remains rejected/gated; the old Fei placeholder remains.

Private `~/release-staging/xeno-work/hd2d/approved-v2-full-fei/` has the full
source corpus (201frames,402mirrors,2956parts), authoring scripts/ledger,
`batch000-015` and coverage evidence. First32-state batch has **25 mask PASS /
7 FAIL**; do not advance/promote it without resolving those (six small1–2pixel
masks and a large frame3 diagnostic background-triangle contamination).
Coverage transport fix10d46904 has front2602/2602 and side2538/2538 exact masks
with focused O0/O2/UBSan proof; this does not prove full Fei art acceptance.

Owner scene previews are complete OFFLINE MOCKUPS under
`~/release-staging/xeno-work/hd2d/owner-preview-hd2d-effects/`: village-190-warm
and village-47-cool three-panel PNGs, effects fullres/clean PNGs, retail/sprite
only inputs and `effects-settings-and-provenance.json`. These are not in-engine
HOOK4 acceptance. HOOK4 effect/toggle/intensity persistence and production
Original/OFF/data gates remain required before package handoff.

Canonical shared menu checkpoint is **cd5ac29d registry32/scroller**; all19
HD-2D rows fit. Proposal9f842e35 contributes tests/docs, not a second capacity
implementation. Preserve all GPU changes and reconcile duplicate horizontal
pad bits6/7. Established serialization: gate588404 -> cd5ac29d ->6c06976b gated
art state ->9f842e35 tests ->Original/OFF/build/REAL data guard ->private package.
Owner art rejection/opt-in decisions override any older promotion wording.

Keep animation-metadata-bd267d18f1b7, approved-v2-full-fei,
owner-preview-hd2d-effects, retail-line-c1ed7f61914a and resume-83b32c4c7ff6.
The five previously proposed archival dirs were not offloaded by this Codex
continuation. Archived older HD evidence is on **blizzpc**
`~/devbox-archive/2026-09-30/xeno-hd2d/`, not Alienware; read ARCHIVED-20260930.md.
Do not delete anything still referenced by pending gates/releases/rollback.

## Reporting and ownership boundary

Native ledger task: **f5b072578f5f**. HD-2D ledger task: **83b32c4c7ff6**.
Coordinator report command:

```sh
python3 /home/blizz/.local/share/devbox-coordinator/coordinator.py report   --task f5b072578f5f --status working --summary 'Concrete result and verification evidence'
```

After the HANDED OVER report, **Codex makes no further project writes**.
Claude owns collection, interpretation, further edits/jobs and any gated
private-package decisions. This handover does not mark the85% goal achieved.
