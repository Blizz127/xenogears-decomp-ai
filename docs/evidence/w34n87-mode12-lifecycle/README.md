# W34N87 — retail mode-12 lifecycle

## Scope and retail anchor

- Starting HEAD: `f805ac914331b555602eedce3f8e8520faec80d7`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail source: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Disassembly: `scratchpad/w34n87_mode12_lifecycle.objdump`
- setup `[0x8007BF50,0x8007C260)`, 784-byte SHA-256:
  `3b8a07c9f0591e66f49a240a0b481970732e504b640a899ab0dcf37ac453d627`
- teardown `[0x8007C260,0x8007C36C)`, 268-byte SHA-256:
  `c87cc08ed787bcf56a042bc397978352c58c33bd7260c493ac547333c7110113`
- full lifecycle `[0x8007BF50,0x8007C36C)`, 1052-byte SHA-256:
  `d5754947b95867cd6ecd9aa1d01f49d075fd37e01c7a4ca0e41d661a56bfefca`

## Production behavior restored

`wm_8007BF50` performs the retail mode-12 setup sequence: framebuffer
transition, second archive-wave poll plus `0x80076954` relocation, object-pool
creation, matrix template and mode-state publication, cross products, WDS
ownership, reset position `(0x00D00000,0xFFF60000,0x00400000)`, object/GPU
and draw resources, archive selection and terrain convergence, all eleven
scheduler registrations in exact order, and the shared post-registration
tail.

Those registrations cover the shared base callback, the nine mode-12 pairs
restored and certified by W34N78-W34N86, and the shared draw pair. The
main-loop guest dispatcher now resolves both mode-12 lifecycle addresses
symbolically.

`wm_8007C260` performs retail audio/SEDS shutdown, shared world teardown,
five owned-allocation frees, pool teardown, and terminal publication
`F94E=273`, `F954=2`, `BBC4=1`, `F950=BD3A`.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n87_mode12_lifecycle.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- setup order, arguments, template/state writes, reset position, WDS/sound
  ownership, two-iteration convergence, and all eleven registration pairs:
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

All M1-M8 were detected. The certificate additionally proves that the
mode-12 second-wave wrapper executes the established poll plus
`wm_80076954` fixup and rejects host-pointer truncation in the lifecycle
source.

Normal port build: `LINK OK` (the first attempt collided with another active
builder in shared `build_native`; the isolated retry passed without a source
change).

Adjacent regression:

- W34N86 final camera/control callback: PASS O0/O2/UBSan, M1-M8 detected.

## Natural 120-frame gate

The accepted bootstrap selected entrance/mode 12, detached GDB before
`PcPort_WorldMapInitMain`, and ran the product loop for 120 displayed frames.

- setup registered eleven occupied records in exact retail order;
- the first scheduler pass inspected eleven occupied records and executed all
  eleven initializer callbacks (`dispatched=11`, `executed=11`);
- every recurring callback resolved; no missing/invalid scheduler boundary
  occurred;
- frame 60 and frame 120 requests were fulfilled by the same numbered
  presentation;
- the bounded loop returned at exactly 120 frames;
- both upload pumps reported `unknowns=0` throughout.

Captures:

- `scratchpad/w34n87_mode12_capture/world-frame-000060.bmp`
  SHA-256 `13a52c764dfc39ca118e6f072545f4a3d6675a65a6e48fe1c3b92ca4f87fd8d6`
- `scratchpad/w34n87_mode12_capture/world-frame-000120.bmp`
  SHA-256 `78634ad19e9883566355a7499def871bd92bc86f8582a7ad1deb71f08c14199f`

Visual inspection shows textured ocean terrain, multiple ships, wakes/light
effects, and a changed viewpoint/content between frames 60 and 120. This is
coherent evolving mode-12 output, not merely a counter gate.

The retail command/timer tables require about 1463 displayed updates before
command 64 clears `D554/D7CC`; the 120-frame run therefore does not exercise
natural `wm_8007C260` teardown. A follow-up natural-exit run must retain the
retail timers rather than shorten them.
