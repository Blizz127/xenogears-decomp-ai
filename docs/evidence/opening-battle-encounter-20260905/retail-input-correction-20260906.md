# Retail title input correction, 2026-09-06 UTC

The earlier retail-input-capture report missed ControllerRemapButtonState.
Its physical-Circle-confirm inference is superseded by the live read-only
controller observations and independently decoded retail initializer in
`retail-controller-remap-20260906.md`. The raw input reached the game correctly:
Cross became logical0020(confirm); Circle became logical0040(cancel).
No controller mapping, game RAM/register, story or presentation state was edited.

Root launched the owned isolated profile with a copy of its ordinary pad helper
plus a read-only snapshot endpoint. Prior profile/helper files were preserved.
Service codex-xeno-retail-pad-diagnosis-20260906, invocation
c589686428814562b6ff4fa9f068ac2d, emulator PID3677470. The only loaded state
was the preserved naturally reached title. Cross opened the menu, a later Up
selected New Game, and another Cross started the opening movie.

Evidence directory:
`/tmp/xeno-retail-opening-reference-20260906-8mpinaz6/pad-diagnosis-20260906`.
`06-newgame-selection.png` visually confirms New Game selected.
`08-after-confirm.png` and progress captures show the opening movie.
The later ordinary Start pulse did not visibly skip it. Do not substitute
movie images for evidence that the opening Gear battle was reached.

The service exited cleanly at 2026-09-06 03:42:35 UTC, before any Gear-panel
capture. Root did not request its termination; the cause remains unobserved.
`journal-terminal.txt`, `actions.jsonl`, helper, original profile copies and
all framebuffer captures are pinned by `artifact-pins.json`.
No new retail battle framebuffer or battle savestate was obtained. An async
operator-coordination question remains pending. This checkpoint leaves the
emulator closed to avoid reopening a possibly operator-closed window; this is
a coordination choice, not a new approval requirement. Independent code work continues.

The native ControllerInit override's identity mapping also differs from retail's
face-button swap. This was recorded, not changed: native input/control mapping
needs a separate end-to-end audit before adoption.
