# Prologue narration slice (New Game route), 2026-10-04

Task e52f8f1e1137, step (ii). This route uses no fixture and loads no save:
retail boot → title (map 490) → New Game → prologue narration (map 4). Input
comes from the raw boot pad schedule (`XENO_PAD_TEST_INPUT`, clocked by Vsync-shim
calls, with the title-smoke timings). When map 4 loads, that schedule hands over
to the drain-clocked field-step probe (47d50a9b: accepted only when
`XENO_PAD_TEST_STOP_FIELD == XENO_FIELD_STEP_PAD_MAP`). Each run exits by itself
at field step 3420. That is after the last narration page, before the map-2
load and the opening battle. Battle code still runs on the interpreter; this
slice does not exercise it.

Every run used the deterministic test clocks (`XENO_SOUND_TICK_LOCKSTEP=1`,
`XENO_DETERMINISTIC_VBLANK=2`). This is a jitter-free test-determinism clock,
not a retail-timing claim, and it is opt-in only.

Tools: `pc_port/tests/run_prologue_step_slice.sh` and
`pc_port/tests/prologue_step_slice_summary.py`.

## Discovery (job 20261004-154217-…-2948712, reference ×2)

- New Game was chosen in both runs with the historical 2-vsync Up pulse. The
  acceptance runs add 4 more: 6/6 under the test clock. The earlier
  unreliability was seen on the host-paced clock.
- The narration has 7 text-box pages, shown at steps 200, 757, 1301, 1749,
  2277, 2910 and 3297 and hidden at 755, 1299, 1747, 2275, 2787, 3083 and 3409.
  Every page logs `wait=0`: the narration advances on its own timer and never
  waits for a confirm press. This slice therefore shows scripted narration
  and its display, not an input response to dialogue.
- **Map-2 hang (open, follow-up d).** In both reference runs the process hung
  shortly after the map-2 load (about step 3438): frames froze for more than
  5 minutes. The plain-clock title smoke reaches map 2, so this points to the
  deterministic clock at the second load. It is not yet diagnosed. No slice
  crosses into map 2 under the test clock until it is.

## Acceptance (job 20261004-160819-…-144690, source 44cd6f9f)

Four runs: ref1, dis1, ref2 and dis2, all rc=0, each exiting by itself at step 3420.

| Check | ref1 vs dis1 | ref2 vs dis2 | ref1 vs ref2 |
|---|---|---|---|
| hand-over and arming at map 4; New Game; map 4 loaded, map 2 not | PASS | PASS | PASS |
| no stubs, no battle; disabled run: 0 interpreted instructions, 0 refusals, 0 unresolved bridges at all 9 checkpoints | PASS | PASS | PASS |
| text-box sequence (18 events, 7 pages) | identical | identical | identical |
| timing: checkpoint steps and presented/vblank deltas | identical | identical | identical |
| SPU register page on every logged vblank (6,988 rows) | identical (e96d35bd… in all 4) | identical | identical |
| checkpoint frames 100–3000 (7 frames) | identical | identical | identical |
| checkpoint frames 3350 and 3415 (final page) | identical | **differ** | **differ** |

Summary verdicts: ref1/dis1 **PASS**; ref2/dis2 **FAIL** (visual only);
ref1/ref2 **FAIL** (visual only).

**Final-page visual nondeterminism (open, follow-up e).** In 2 of 4 runs the
last two checkpoint frames differ: ref2 and dis2 also differ from each other.
The difference is confined to the burning-village fire animation of the last
page, about 23,000 bytes in one region roughly 270×190 px. Text, frame layout
and every other pixel match. This is run-to-run nondeterminism, not a
difference between native and interpreted execution: ref1 equals dis1, and
ref2 differs from dis2.

The cause is not yet known. The pre-arm frame and vblank offsets (presented
3746 / 3796 / 3769 / 3802) do not predict which runs match, so a simple
"more boot frames, different random-number state" explanation does not hold
as stated. Candidates: the flame effect's random-number or timer source, and
the host-paced boot and load phases before the probe arms.

## Status

- **Accepted, opt-in test mode:** narration pages 1–6 of the prologue. That
  covers visual evidence for steps 0–3000 in all 4 runs, plus text-box
  sequence, timing deltas and SPU through step 3420 in all 4 runs, with zero
  fallback.
- **Not accepted:** the visual identity of page 7 (fire animation), follow-up (e).
- **Coverage limits:** no input response is exercised (the narration is
  timed), no save is taken (scene control holds), and battle is not reached
  (it is still interpreted).

Receipts are in the runner work directories on blizzpc. The devbox copies
of the small receipt files are in `/tmp/xeno-ii-receipts/`.
