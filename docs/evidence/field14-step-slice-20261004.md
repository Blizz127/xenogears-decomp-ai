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
