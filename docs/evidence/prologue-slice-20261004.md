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

## Follow-up (e): root cause and fix

Receipts: `~/release-staging/xeno-work/e52f8f1e-prologue/` (`e-investigate-c50b9384/`,
`e-trace-4ef1590b/`, `e-verify-02320d42/`).

- **Investigation** (job …-1540902, 3 reference runs, RNG log c50b9384):
  - r1 and r2 armed at map 4 with seed c521f39b after 1,895 `rand()` calls;
    r3 armed with seed 62264e95 after 1,893 calls.
  - After arming, all three runs made identical calls (4,641 each), with 0
    from other threads.
  - r3's frames differ from step 3280 onward, where the fire starts. The fire
    effect consumes `rand()`, and the seed entering map 4 differed.
- **Trace** (job …-2210747, 5 runs, per-call trace 4ef1590b):
  - Every variable call happens in the title map (490), from two per-frame
    callers: `func_8001BD40` (temp3.c, 490–496 calls) and `func_80078B5C`
    (field misc4.c, 1,400–1,402 calls).
  - The test clock armed only with the step probe at map 4 ("boot and title
    keep the host clock"), so the title phase was host-paced.
- **Fix** (4af144ba, opt-in test mode): `XENO_DETERMINISTIC_VBLANK_FROM_FIELD=490`
  arms the clock on the first vblank in the title map. Load/live detection now
  counts every field drain, so it also works before the probe arms.
- **Verification** (job 20261004-172350-…-3653136, source 02320d42, ref ×2 and
  dis ×2, all rc=0):
  - Seed at arming is identical in all 4 runs: 96af344d after 1,645 calls,
    ending at 6b73b862 after 6,286 calls.
  - All 9 checkpoint frames are identical in all 4 runs, **including the
    page-7 fire frames 3350 and 3415**.
  - The SPU log is identical (c2a263e6…, 7,060 rows), and so is the text-box
    sequence (18 events).
  - Summaries ref1/dis1, ref2/dis2 and ref1/ref2 all **PASS**.
  - New Game was chosen in all 4 runs; no title stall.
  - Still varies: the title phase's drain count before the map-4 load
    (1,600–1,615). The RNG consumption and every compared output are
    identical regardless.

Related fix: the port keeps the retail rand seed (guest 0x8005A1FC) in a host
global, so port quicksaves did not carry it. It now goes in a `<save>.rng`
sidecar tied to the checkpoint by its checksum (0b27e2d1); the main
quicksave format is unchanged, so older builds still load it.

## Follow-up (d): map-2 hang, root cause and fix

Receipts: `~/release-staging/xeno-work/e52f8f1e-prologue/` (`d-hang-6b1b5cbc/`,
`d-verify-cee78410/`).

- **Isolation** (job …-573808). The route continued into map 2, with
  xeno-port run as a gdb child.
  - Variant A (lock-step sound + vblank test clock) HUNG; variant B (vblank
    test clock only) HUNG; variant C (lock-step only) ran 480 s past map 2.
  - gdb, main thread: `func_8007554C` (field misc2.c) spinning in
    `while (Vsync(-1) < s1 + D_800B217C + 2)`. `D_800B217C` is the field
    script frame rate (`func_800925A0`: 0/1/2 = 30/20/15 fps).
  - Under the test clock, `Vsync(-1)` returns the serviced vblank count,
    which advanced only by K=2 per blocking Vsync. When the Lahan opening
    switches to 20/15 fps, the target s1+3 or s1+4 is never reached.
  - This is a test-clock defect: the plain clock reaches map 2. It does not
    share a root with (a)/(e).
- **Fix** (cee78410, opt-in test mode): a `Vsync(-1)` whose previous Vsync call
  was also a `Vsync(-1)` with the same count is a spin, and it advances the
  test clock by one vblank. Any `Vsync(1)` or blocking Vsync ends the spin,
  so 30 fps frames are unchanged. At 20/15 fps frames take 3/4 vblanks, as
  on hardware.
- **Verification** (job 20261004-181725-…-1333301):
  - Into map 2 ×2: no stall. Both runs went maps 490 → 4 → 2, logged the
    spin advance, entered the opening battle (interpreted, reference mode)
    and were still running at the 410 s limit (back in field 14).
  - Regression, prologue slice ref + dis: PASS. Frames are identical to the
    accepted e-verify run; SPU c2a263e6… and seed 96af344d are unchanged.
  - Regression, field14 ref + dis: PASS. Both saves are 63929945…, and the
    frames are identical to the (a) run.

## Status

- **Accepted, opt-in test mode (clock armed from the title map):** the full
  prologue narration slice, all 7 pages. Job …-3653136: frames, text-box
  sequence, timing deltas and SPU are identical across 2 reference and 2
  interpreter-disabled runs, with zero fallback.
- Superseded: the earlier pages-1–6 acceptance (job …-144690), and the page-7
  visual nondeterminism under probe-armed clocks, now explained and fixed.
- **Coverage limits:** no input response is exercised (the narration is
  timed), no save is taken (scene control holds), and battle is not reached
  (it is still interpreted).

Receipts are in the runner work directories on blizzpc. The devbox copies
of the small receipt files are in `/tmp/xeno-ii-receipts/`.
