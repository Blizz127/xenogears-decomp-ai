# W34N76 — retail mode-14 lifecycle

## Scope and retail anchor

- Starting HEAD: `07494b965e2b5b42418d7e786f7cefd4f3077184`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail source: `disc/world_map.bin`, loaded at `0x8006FAF0`
- setup `[0x8007A5DC,0x8007A8AC)`, 720-byte SHA-256:
  `df001fb45188e7b7e04323dddd843f195d574120b8d02d8c94331a306aac845e`
- teardown `[0x8007A8AC,0x8007A9B4)`, 264-byte SHA-256:
  `2a339d84f90efb1f9b258c90b67a88d01ed4b321709a28c06c884765c1299a8e`
- full lifecycle `[0x8007A5DC,0x8007A9B4)`, 984-byte SHA-256:
  `dcfb4937c169a6c1a38a9f794a11041b4249b22cb2074d81ce5569c3799eb723`

## Production behavior restored

`wm_8007A5DC` now performs the retail setup sequence: framebuffer transition,
second archive-wave poll and `0x80076954` relocation, object-pool creation,
matrix template and mode-state publication, cross products, WDS ownership,
mode-14 reset position `(0x05BED000,0xFFF60000,0x062A8000)`, object/GPU and
draw resources, archive selection and terrain convergence, eight scheduler
pair registrations in exact order, and the shared post-registration tail.

Those registrations cover the shared base callback, all six mode-14-specific
pairs restored by W34N70–75, and the shared draw pair.  The main-loop guest
dispatcher now resolves both lifecycle addresses symbolically.

`wm_8007A8AC` performs retail audio/SEDS shutdown, shared world teardown,
five owned-allocation frees, pool teardown, and terminal publication
`F94E=282`, `F954=0`, `BBC4=1`, `F950=BD3A`.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n76_mode14_lifecycle.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- setup order, arguments, template/state writes, reset position, sound
  ownership, two-iteration convergence, and all eight registration pairs:
  PASS
- teardown order, WDS release, five frees, and terminal state: PASS
- M1 wrong transition argument: `setup.transition_args`
- M2 wrong mode constant: `setup.state_values`
- M3 swapped object/GPU-A stages: `setup.stage_order`
- M4 wrong final registration pair: `setup.registration_pairs`
- M5 omitted sound linkage: `setup.sound_link`
- M6 omitted palette tail: `setup.stage_order`
- M7 omitted SEDS free: `teardown.seds_free`
- M8 wrong teardown state: `teardown.state_values`

All M1–M8 were detected.  The certificate also proves the mode-14 second-wave
wrapper executes the established poll plus `wm_80076954` fixup and rejects
host-pointer truncation in the lifecycle source.

Normal port build: `LINK OK`.

Adjacent regressions:

- W34N75 final camera-control callback: PASS O0/O2/UBSan, M1–M6 detected.
- W34N66 mode-10 lifecycle: PASS O0/O2/UBSan, M1–M8 detected.

## Natural 120-frame gate

The accepted bootstrap selected entrance/mode 14, detached GDB before
`PcPort_WorldMapInitMain`, and ran the product loop for 120 displayed frames.

- setup reached and registered 8 occupied records;
- all 8 initializer callbacks resolved and executed, with no missing or
  invalid scheduler callback;
- recurring frame 120 retained the three expected live callbacks after the
  one-shot records completed;
- frame 60 and frame 120 requests were fulfilled by the same numbered
  presentation;
- bounded loop returned normally; upload pumps reported zero unknowns.

Captures:

- `scratchpad/w34n76_mode14_capture/world-frame-000060.bmp`
  SHA-256 `6776da2371d1842354928edeedb12ca46e29729ef67833b0533b7f2341d50c72`
- `scratchpad/w34n76_mode14_capture/world-frame-000120.bmp`
  SHA-256 `f6c4b790cbcac42ded76ca9285d2942672fef791d38988187116edf5e8c724ac`

Visual inspection shows the world horizon and textured terrain at frame 60,
then the mode-14 vertical light/energy effect over that scene at frame 120.
This establishes coherent evolving output, not retail pixel parity.

The 120-frame bound precedes the 615 ticks encoded in the retail mode-14
state/timer tables, so it does not exercise natural terminal state 11 or
`wm_8007A8AC`.  Natural teardown is the next exact acceptance target; no timer
is shortened for that test.
