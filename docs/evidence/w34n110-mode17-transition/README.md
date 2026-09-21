# W34N110 — mode-17 transition/drain callback

## Scope and retail anchor

- Starting HEAD: `1c138fb4b98d6bddf1fe790d0e7c693e0337208d`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail image: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Disassembly: `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`
- initializer `[0x800834D0, 0x800834D8)`, 8-byte SHA-256:
  `5ce5ad86d452c4d2422bd63e15223d5d6b3dfb77f224a88c1f476e9fb34e359d`
- updater `[0x800834D8, 0x8008355C)`, 132-byte SHA-256:
  `63db118754e342edcf4438e409d8e566c4a9bc36d38a117c1012e2b5aeb6d7c5`
- pair `[0x800834D0, 0x8008355C)`, 140-byte SHA-256:
  `49aa1e57f1db503db33c9b2c03221443fa32e1b62a663a5b300a25f6b882dd58`

## Production transcription

`wm_800834D0` restores the retail constant-success initializer.

`wm_800834D8` resolves its 128-byte scheduler record through the pool pointer
at `0x8009BE24`. When the signed halfword latch at record offset `+0x04` is
exactly one, it clears the latch, synchronizes drawing and vertical blank,
calls `wm_80097D64`, applies the position record at `0x8009C5AC` through
`wm_80097BC0`, and drains `wm_800967E4` plus `Vsync(0)` until
`wm_80096668_circular_distance()` reaches zero. Other latch values are an
exact no-op. Both callback addresses are resolved symbolically by the native
scheduler.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n110_mode17_transition.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- initializer return and no-side-effect contract: PASS
- exact latch predicate and clear: PASS
- DrawSync/Vsync/reset/position/drain ordering: PASS
- multi-iteration circular-distance drain: PASS
- idle no-op: PASS
- scheduler initializer/updater resolution: PASS
- initializer/updater/pair retail hashes: PASS
- M1–M6: all detected by named assertions

The normal product build completed with `LINK OK`.

## Natural entrance-17 route

The detached entrance-17 route reached the integrated pair and completed the
exact 120-frame bounded loop:

- `0x800834D0`: one successful initializer execution
- `0x800834D8`: 120 successful recurring updater executions
- unresolved stub hits for either address: zero
- upload-pump unknowns: zero
- frame-60 and frame-120 capture requests fulfilled in their requested frame
- bounded loop returned normally

The remaining private callback census across session entry plus 120 recurring
passes is:

- `0x800827C8`: 121 unresolved initializer hits
- `0x800827EC`: 121 unresolved initializer hits
- `0x80083214`: 605 unresolved initializer hits (five records)

Captures:

- frame 60:
  `/home/blizz/dev/xenogears-decomp/scratchpad/w34n110_mode17_capture/world-frame-000060.bmp`
- frame 60 SHA-256:
  `5851ca7ca04f8f77b7e8eb1b9a22a4510ad97205109f0d34bdf462733b66d2d0`
- frame 120:
  `/home/blizz/dev/xenogears-decomp/scratchpad/w34n110_mode17_capture/world-frame-000120.bmp`
- frame 120 SHA-256:
  `6a0f814c9a83160ea45b66338701e99bac400eb3e7b0b5196b2bd79907561b6a`

Both hashes are bit-identical to W34N109. That is expected observational
neutrality for a naturally recurring callback whose guarded transition arm
was not shown to activate on this input route; it is not evidence that the
guarded path is unimplemented. The focused certificate exercises that path.
The images remain largely blank/inverted while the three callback families
listed above are unresolved, so this rung does not claim visual acceptance.

## Verdict and next target

`MODE17_TRANSITION_PAIR_RESTORED`

The mode-17 private frontier is reduced from four callback families to three.
The next target is selected by comparing the remaining retail pair extents,
starting with `0x800827C8 / 0x80076B34` and
`0x800827EC / 0x800828DC` before the five-instance
`0x80083214 / 0x80083264` family.
