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
