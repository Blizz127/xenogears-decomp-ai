# W34N86 — final mode-12 camera/control callback pair

## Scope and retail anchor

This code-producing rung transcribes retail `[0x8007C724, 0x8007CC6C)`,
the last unresolved mode-12-specific scheduler pair registered by setup
`0x8007BF50`.

Authority:

- `disc/world_map.bin`, mapped at `0x8006FAF0`;
- `scratchpad/w34n86_mode12_pair_c724.objdump`;
- exact 1352-byte slice SHA-256:
  `b2feeed8f84d242cbaaf14381c79c68eb698f4dac6f0979cfc0643170f94a878`;
- initializer `[0x8007C724,0x8007C7D8)` SHA-256:
  `ee813230a19b0d090c0de922c57e11bf0180173b7a0dc51d32c44683ef45438d`;
- update `[0x8007C7D8,0x8007CC6C)` SHA-256:
  `9c912a14c6aab5a2350d7ea4d5370492b858781b8221d01e409e9638a42bf677`.

`0x8007C724` initializes the mode-12 camera height, camera gate, reset/live
and shadow positions, Euler angles, motion phase/speed, and jitter amplitude.

`0x8007C7D8` implements the complete six-value command latch and seven-state
camera controller. It preserves these non-obvious retail details:

- latch value 5 is a no-op and remains uncleared;
- state 1 changes to state 2 only when phase is strictly greater than its
  threshold;
- state 2 clamps a negative decelerated speed to zero before phase advance;
- state 4 publishes markers 26/27 and destroys both at the 4096 amplitude
  floor;
- state 5 selects state 1 (the target of the retail jump/delay-slot pair),
  not state 2;
- state 6 uses path table `0x8009A568`; the other active states use
  `0x8009A4F8`;
- a `-1` path sentinel skips interpolation and camera-input replacement but
  still calls `wm_80097244` and `wm_80097070`;
- the optional generic camera build is gated only by `D_8009D144 == 0`;
- signed `rand() % (amplitude >> 12) - (amplitude >> 13)` jitter is added to
  both camera halfwords.

## Production change

- Added `world_map_callback_7c724.c/.h`.
- Registered `0x8007C724/0x8007C7D8` in the bounded scheduler resolver.
- Added the source to the normal port build manifest.

No mode-12 lifecycle function or previously accepted callback changed.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n86_mode12_camera_control.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- M1 wrong initial view height: detected by `init.camera_constants`
- M2 wrong latch-1 threshold: detected by `latch.one`
- M3 wrong negative-speed clamp: detected by `state2.negative_clamp`
- M4 wrong second state-4 marker: detected by `state4.markers`
- M5 wrong state-6 path table: detected by `state6.final_table`
- M6 omitted generic-camera call: detected by `camera.generic_gate`
- M7 truncated interpolation remainder: detected by `camera.blend_args`
- M8 omitted second jitter axis: detected by `jitter.mirrored_axes`

The certificate additionally proves every initializer write; reset/live and
shadow position publication; latch values 1, 2, 4, 5, and 6; the strict
state-1 boundary; state-2 underflow; the state-5 delay-slot result; marker
coordinates and lifetime; state-6 shadow restore, phase, and deceleration;
normal/final path-record addresses; all five interpolation arguments; camera
input derivation; path-sentinel preservation; helper arguments; signed jitter;
return values; and bounded scheduler resolution for both guest addresses.

Normal port build: `LINK OK`.

Adjacent regression:

- W34N85 scripted-control pair: PASS in O0/O2/UBSan, M1-M8 detected.

## Mode-12 boundary and next target

All mode-12-specific scheduler callback pairs registered by retail setup
`0x8007BF50` are now port-owned and focused-certified. Mode 12 is still not an
accepted natural route because its lifecycle remains unintegrated.

Next: transcribe/integrate retail setup `0x8007BF50` and teardown
`0x8007C260`, register this now-complete callback surface through the normal
mode table, then run a bounded natural mode-12 lifecycle acceptance. This
commit does not claim that runtime gate.
