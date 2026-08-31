# W34N92 — mode-13 camera/control callback

## Scope and retail anchor

- Starting HEAD: `ce9e498c1392a0b3ee22d38d7211c76119a808d4`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail image: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Retail slice: `[0x80080578,0x80080900)`, 904 bytes
- SHA-256: `9c824ffe16c72b81eee6f15dd47134e2099919f34dc324c33cb15cb4ad05c189`
- Disassembly source: `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`

This is the final unowned private callback pair registered by retail mode-13
setup. No lifecycle or previously accepted mode-13 pair changed.

## Production transcription

`wm_80080578` restores the retail viewport height, camera gate, live/shadow
position, initial amplitude, and first-update latch.

`wm_80080600` implements the complete latch and camera state machine:

- latches 1/2/3 select the interpolation, hold, and amplitude-decay states;
- the ungated path invokes the shared camera-record producer with exact guest
  arguments before state advancement;
- state 1 approaches the retail camera height and both angle targets with the
  original signed fixed-point clamps and shifts;
- state 3 decays and clamps the jitter amplitude before returning to state 0;
- one random remainder is biased by half the current range and mirrored into
  camera-input halfwords `BD42` and `BD4A` through guest scratch.

The scheduler resolves both guest addresses symbolically.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n92_mode13_camera_control.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- initialization, positions, gate, latch states: PASS
- first-tick camera interpolation and signed clamps: PASS
- camera call arguments/gate, amplitude terminal state: PASS
- jitter range, bias, guest scratch, and mirrored writes: PASS
- scheduler init/update resolution with zero missing/invalid hits: PASS
- M1 wrong viewport height: `init.camera_constants`
- M2 wrong latch-1 pitch seed: `latch1.state`
- M3 wrong primary clamp: `state1.primary_clamp`
- M4 wrong pitch approach shift: `state1.pitch_approach`
- M5 omitted generic camera call: `camera.generic_call`
- M6 wrong state-3 clamp: `state3.clamp`
- M7 wrong jitter bias: `state1.jitter_mirror`
- M8 omitted jitter mirror: `state1.jitter_mirror`

All M1-M8 were detected by their named assertions.

Adjacent W34N91 dual-stream certificate: PASS at O0/O2/nonrecovering UBSan
with M1-M8 detected. Normal port build: `LINK OK`.

## Acceptance boundary

All four private mode-13 callback pairs are now linked and scheduler-
resolvable. The next bounded target is retail lifecycle
`0x8007FF70/0x80080218`, followed by a detached natural mode-13 run. No
further callback archaeology is required before that integration.
