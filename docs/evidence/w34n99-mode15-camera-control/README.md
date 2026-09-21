# W34N99 — mode-15 camera/control callback

## Scope and retail anchor

- Starting HEAD: `018dd56d41240cab8aa4f5513206a975fe2b4746`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail image: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Disassembly: `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`
- initializer `[0x8007E450,0x8007E4E4)`, 148-byte SHA-256:
  `1da9fd8cb77d1763c50e634cbb240d1ecf04ee38e2ff1f790af79f08b2b7f414`
- update `[0x8007E4E4,0x8007EBBC)`, 1,752-byte SHA-256:
  `bb1d11fe32c85e4d81f7b001c642454475e64f171312ec825bbbc07b4fc39965`
- full pair `[0x8007E450,0x8007EBBC)`, 1,900-byte SHA-256:
  `359e73b48329029a597c904e40c2d6a9dfce439ca173c73cbd53ab77adc1aa15`

## Production transcription

`wm_8007E450` initializes the mode-15 camera slot from the reset position,
publishes identical live and shadow positions, restores the retail view
offset, amplitude, latch, speed, and state, and clears the camera gate and
phase word.

`wm_8007E4E4` consumes all retail camera latches (`1-7`, `16`, `17`, and
`24`), invokes the established wrap and camera-matrix producers, implements
the complete state machine (`1-5`, `16`, `17`, `24`, and `25`), preserves
the retail fixed-point update and clamp ordering, and applies the amplitude-
scaled mirrored camera jitter. The scheduler resolves both callback
addresses symbolically.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n99_mode15_camera_control.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- all three retail slice hashes: PASS
- initializer control state and live/shadow position publication: PASS
- all static and dynamic latch effects: PASS
- generic camera-helper arguments and ordering: PASS
- path states, transitions, signed comparisons, clamps, and decay states:
  PASS
- mirrored jitter calculation: PASS
- symbolic scheduler init/update resolution: PASS
- M1 wrong initializer view offset: detected by `init.controls`
- M2 wrong latch-1 camera angle: detected by `latch1.camera_state`
- M3 skipped camera-matrix build: detected by `camera.generic_call`
- M4 wrong state-1 path delta: detected by `state1.path_delta`
- M5 sign-extended state-3 Y immediate: detected by
  `state3.position_delta`
- M6 wrong state-5 amplitude clamp: detected by `state5.clamp`
- M7 wrong state-24 transition amplitude: detected by
  `state24.transition`
- M8 skipped mirrored jitter write: detected by `jitter.mirrored_offset`

All M1-M8 were killed by named assertions. The W34N98 focused regression
remains green. A clean detached worktree containing only the W34N99 delta
completed the normal product build with `LINK OK`; this isolation avoided an
unrelated concurrently added shared-tree source that briefly preceded its own
manifest registration.

## Natural route

The normal detached entrance-15 route naturally executed `0x8007E450` once
and `0x8007E4E4` on all 120 recurring scheduler passes. The next unresolved
private callback set is:

```text
0x8007ECA4  slot 3, 121 hits (initializer plus recurring passes)
0x8007F8AC  slots 4-8, 605 hits
```

The run reached frame 120, fulfilled both capture requests in-frame, kept
both upload pumps at `unknowns=0`, and returned from the bounded loop. The
camera callback legitimately changes the visual state from W34N98: both
captures show the mode-15 sky/cloud field at different positions.

- frame 60 SHA-256:
  `731fbfdb250dc2728d74b9a9d9bf114e2b2a119e43409f15060aacbbbc6751cd`
- frame 120 SHA-256:
  `92f167490aac8d590b63d6e37585b2730a6674c01f03c38f90a8a062439b2299`

## Verdict and next target

`MODE15_CAMERA_CONTROL_RESTORED_NATURALLY_EXECUTED`

Next exact target: decode and restore the mode-15 slot-3 callback beginning
at `0x8007ECA4`, including its direct `0x8007EBBC` dependency, then rerun
this same detached route.
