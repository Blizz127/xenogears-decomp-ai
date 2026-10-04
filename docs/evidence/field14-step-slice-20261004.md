# Field14 movement slice, interpreter disabled — 2026-10-04 (task e52f8f1e1137)

blizzpc job `20261004-001629-xeno-xeno-e52f-field14-slice-bba0c67f-1404801`, source bba0c67f,
host binary SHA-256 bc8b4b31ce79533d7aeddde8d11886539d8962fdff1bcde058a7fe561c41cc93,
overall rc 0. Receipts with SHA readback (text only):
`~/release-staging/xeno-work/e52f8f1e-field14-slice/receipts/`.

Method:
- Fixture: the legitimate earned field14 save, SHA-256 0fa6e676… (coldboot → New Game →
  prologue → Lahan → real opening battle → field14, standard F11), checked in-job.
- Load: `XENO_QUICKLOAD_ON_TITLE` queues the ordinary F12 quick-load request on the
  title field, because headless SDL has no keyboard.
- Input: `XENO_FIELD_STEP_PAD_INPUT`, the mask-preserving, field-step-clocked pad probe,
  armed in field 14 (Right 300–359, Left 480–539, Down 660–719, with settles).
- No console, dev menu, warp, FIELD_TEST, cheat or vblank schedule. env -i, private
  HOME, SDL offscreen, null audio.
- Two runs: hybrid reference, then `XENO_DISABLE_CPU_INTERPRETER=1`.

Result (field14_step_slice_summary.py, all 8 checks PASS):

| Check | Result |
|---|---|
| fixture loaded, probe armed in field 14 | both runs |
| transitions 300/360/480/540/660/720 | all six, both runs; 0 empty drains |
| quiescent checkpoints | identical: (115,0,−455) → (203,0,−496) → (96,0,−428) → (89,0,−435) |
| disabled run CPU counters | `entries=0 native_bridges=0 interpreted_instructions=0 disabled_refusals=0 unresolved_bridges=0` |
| refusals / stubs | none |

The disabled run never entered the MIPS adapter (`entries=0`): this field movement path
is native C end to end, so its zero fallback is structural, not a refusal masked by luck.

Gaps, disclosed:
- The REFERENCE run's counter line is missing. Both runs ended by the same SIGINT
  timeout and only the disabled one printed it; the cause is not yet known. It does not
  affect the disabled side's acceptance numbers.
- Movement/position semantics only. No audio (null driver), no visual comparison of the
  captured frames, no frame-exact timing, and no save/restore in this run (earlier
  F11/F12 restore evidence is in native-field-slice-20261003.md, on the older binary).

Against the owner's native criterion (interpreter disabled, zero fallback, semantic,
visual, audio, timing and save evidence), this is the first executed slice with the
interpreter disabled, zero counted fallback and matching semantic state on a legitimate
fixture. It is NOT yet a fully accepted slice: visual, audio, timing and save evidence
are outstanding.

## Acceptance evidence, job 3798264 (source cb100a26) — 2026-10-04

`20261004-012113-xeno-xeno-e52f-field14-accept-cb100a26-3798264`, binary d2735c82…, in-job
summary rc 0, "FIELD14 SLICE PASS". Receipts with on-runner artifact hashes:
`~/release-staging/xeno-work/e52f8f1e-field14-acceptance/receipts-cb100a26/`.
(The first acceptance attempt, 2435487 / 395361d9, ran fully but its summary crashed
on a script bug. Recomputed read-only, it gave the same item results.)

| Item | Result |
|---|---|
| visual | PASS: the first presented frame at steps 299/359/479/539/659/719/779/899/1000 is byte-identical between reference and interpreter-disabled; the 9 frames are mutually distinct |
| timing | PASS on the game's frame clock: checkpoint steps identical, presented-frame deltas [60,120,60,120,60,60,120,119] identical. Vblank deltas differ slightly because vblanks are host real-time paced (explained) |
| save | PASS: the ordinary F11 quick save at step 780 is byte-identical between modes and differs from the fixture; after a second walk, F12 reload at 900 restores the saved position in both. This is the port's quick checkpoint, not a memory-card block (field 14 has no save point) |
| counters | PASS: the disabled run logged entries/interpreted/refusals = 0 at all 9 checkpoints in-run; the reference exit receipt also reads 0. The disabled exit line is still lost on SIGINT; in-run receipts make it unnecessary |
| audio | NOT ACCEPTED: SPU register-page hashes and even the distinct-state sequences differ between modes. Source: the sound driver's 240 Hz tick (RCnt2 handler) runs on PsyCross's host interrupt thread, asynchronous to field updates. The port also does not yet render that page to an audio device (sound.c, WDS/B5). Same-mode control job 3927918 decides whether this is host nondeterminism |

The slice passes visual, timing, save and zero-fallback with the interpreter disabled.
It is NOT fully accepted while audio is open.

## Deterministic clocks and the timing-fidelity follow-up

Control job 3927918 showed the field14 SPU stream differs between two runs of the SAME
mode: the sound tick (240 Hz RCnt2 handler) runs on PsyCross's host interrupt thread.
Lock-step job 1941800 (`XENO_SOUND_TICK_LOCKSTEP=1`, 4 ticks per serviced vblank, 9ff66adf)
made the streams match except for host vblank jitter. One run's F11 save differed in one
byte (g_GameState+0x193C, a vblank-driven field-script counter) after extra host vblanks.

`XENO_DETERMINISTIC_VBLANK=<K>` is a **jitter-free test-determinism clock, not a
retail-timing claim**. Once the field-step probe arms, each blocking Vsync advances a
virtual game-vblank count by exactly K (K=2, the port's usual no-jitter field cadence),
Vsync(1) and the vblank service follow it, and presentation stays host-paced. Both flags
are opt-in; neither becomes a shipped default without the owner.

**Tracked follow-up (B), timing fidelity:** measure retail's actual per-frame vblank cost
over the same field14 walk from a reference (emulator or hardware), including any 3-vblank
slowdown frames, and make the virtual clock follow it. Until then, K=2 removes host jitter
but does not model retail frame timing or slowdowns.

## FULL ACCEPTANCE — job 3875192 (source 73cb0497), 2026-10-04

`20261004-060627-xeno-xeno-e52f-field14-split-73cb0497-3875192`, binary 18d7b007…, with the
deterministic test clocks (`XENO_SOUND_TICK_LOCKSTEP=1`, `XENO_DETERMINISTIC_VBLANK=2`:
a jitter-free test-determinism clock, not a retail-timing claim). Determinism was required in
both summaries (`FIELD14_REQUIRE_DETERMINISM=1`). Receipts with on-runner artifact hashes:
`~/release-staging/xeno-work/e52f8f1e-field14-splitclock/receipts/`.

| Item | Result over 4 runs (ref1, dis1, ref2, dis2) |
|---|---|
| self-exit | every run exited by itself at the step-1100 cap, rc=0 |
| zero fallback | the interpreter-disabled runs logged entries/interpreted/refusals = 0 at every checkpoint |
| visual | all 36 checkpoint frames byte-identical across the 4 runs |
| timing | checkpoint steps, presented-frame deltas and game-vblank deltas identical |
| save | the F11 save at step 780 is byte-identical in all 4 runs (63929945…); the F12 reload restores the position |
| audio | all 6 SPU register-page pairs identical on every vblank (2,000 rows) and on distinct states (347) |

Both summaries: "FIELD14 SLICE PASS". The job's own exit code was 2 because of a job-script
bug, not a result: the var-watch extract loop reused the shell variable holding the overall
rc. Every gate line reads rc=0.

The path there:
- Sound: the RCnt2 tick moved from PsyCross's host thread into lock-step (9ff66adf).
- Vblank pacing: a fixed K=2 test clock (f2a179c6).
- Activation transients: 43ffebe1, fb29b1cc.
- 2b7058b7 froze the clock during loads and HUNG the field-14 load (jobs 3219278/3255482,
  stopped, void).
- 73cb0497 split the clock: the service clock stays host-paced while loading and becomes
  deterministic once the field loop is live; the game-visible Vsync(1) clock is frozen
  during loads.

Open notes:
- The var-watch logged ZERO VM writes to script byte 0xC (= g_GameState+0x193C) in these
  runs. The earlier one-byte save difference came in through the host-timed load path, not
  a field-script write. It is now deterministic, but its writer is unidentified.
- Neither flag is a shipped default; that decision is the owner's, with this evidence.
- Timing fidelity follow-up (B), measuring retail's per-frame vblank cost, remains open.
- Audio output: the SPU page is still not rendered to a device (WDS/B5).

**Status: the field14 movement slice is ACCEPTED under the deterministic test clocks.**
The interpreter is disabled with zero fallback, and semantic, visual, timing, save and
SPU-state equality hold across repeated reference and interpreter-disabled runs. This is
the first accepted native runtime slice. Coverage is one field (Lahan, map 14) walk.

Re-run with the fixed job script: `20261004-061423-xeno-xeno-e52f-field14-split2-73cb0497-4085031`,
job exit 0 (overall_rc=0), binary 503ae281…, both summaries "FIELD14 SLICE PASS". The same
36 frame hashes, save 63929945…, 6/6 SPU pairs identical on every vblank, and 0 var-6
writes: 8 identical runs across 2 jobs. Receipts: `…/e52f8f1e-field14-splitclock/receipts-rerun/`.

## Field menu slice ACCEPTED — job 1645287 (source c4cc953a), 2026-10-04

`20261004-070956-xeno-xeno-e52f-field14-menu-c4cc953a-1645287`, binary 4bf7a4d9…, job exit 0.
The fixture, test clocks and deterministic flags are the same as the walk slice.

Schedule:
- Triangle at steps 300 and 420 opens the field system menu. The field requests it
  (D_800ADB64=0x80), and func_800799D4 then runs MenuMain synchronously.
- While MenuMain suspends the field loop, XENO_FIELD_STEP_PAD_SUSPENDED presses Cross
  (u=120, then every 60 vblanks). u=180 closes it ("func_801C55A0 cancel/close
  choice=6", then "MenuMain returned").
- Right walk, F11 save at step 640, self-exit at step 760.

Over 4 runs (ref×2, dis×2):
- each run rc=0, with 2 menu opens and 2 returns;
- checkpoint frames, save (b5446276…) and checkpoint player positions identical:
  (115,0,−455) → (203,0,−496) after the menus;
- all 6 SPU pairs identical on every vblank;
- zero interpreted instructions and refusals.

Both summaries: "FIELD14 SLICE PASS".

Observations:
- The first scripted Cross arrives before the menu's input loop; the remaining scripted
  presses land in the post-menu WAIT_MENU window. Both happen identically in every run.
- POSDIAG stays silent after a field menu because the field transition flag D_800ADB04
  remains clear. Checkpoints therefore read the player position directly (c4cc953a).
  Whether D_800ADB04 staying clear is a port divergence is an open question.

Receipts (tmpfs, per the devbox disk rule): /tmp/xeno-e52f-receipts/field14-menu-c4cc953a/.

## Follow-up (a) resolved: the first-run save byte, 2026-10-04

The variable save byte from the r6.7-t1 gates is g_GameState+0x193D. In 2 of 8
runs the save differed: always the job's first, slower run, whose title phase
ran longer (spu start 24 vs 21–23). Root cause: the same as the prologue's
follow-up (e) (`docs/evidence/prologue-slice-20261004.md`). The test clock
armed only with the step probe, so the title phase before the F12 quick-load
ran host-paced.

Check: job 20261004-174602-…-115167, source 640ac877, walk slice ×4 with
`XENO_DETERMINISTIC_VBLANK_FROM_FIELD=490`.
- Every run passed: rc=0, both summaries PASS, spu-pairs identical.
- Seed at arming was 05463f19 in all 4 runs; every SPU log starts at index 0;
  the load phase begins at the same step and drain (919/951).
- **All 4 F11 saves equal the accepted 63929945…, ref1 included.** Each also
  wrote its rand-seed sidecar (`.rng`, 0b27e2d1).
- Checkpoint frames 299–899 equal the accepted frames. Frame 1000, the only
  checkpoint after the F12 reload at step 900, is identical in all 4 runs but
  differs from the accepted frame. The likely reason: the reload now restores
  the saved rand seed from the sidecar instead of continuing with the host's
  seed (not separately proven).

Receipts: `~/release-staging/xeno-work/e52f8f1e-prologue/a-f14-640ac877/`.
