# W34N66 — retail mode-10 lifecycle

## Scope and anchor

- Starting HEAD: `5d6226f7ffa21a769c845761d0c77d0059dd8524`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail source: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Retail lifecycle slice: `[0x80078A60, 0x80078E2C)`
- Slice SHA-256: `e96afde60f4e234393aa888b3e487eb7d3cfdf6bf28e89457d766a8ca5ffc62d`

This rung integrates mode 10's setup and teardown callbacks. It does not
change the four previously accepted mode-10 scheduler callback pairs.

## Production behavior restored

`wm_80078A60` now performs the retail setup sequence:

- shared framebuffer setup, the `MoveImage((0,0,320,216),704,256)` transition,
  `DrawSync(0)`, and `wm_80072DB4(64,0,4,2)`;
- second archive-wave completion and `wm_80076954` relocation, object-pool
  creation, template copy, mode globals, cross products, and WDS load;
- mode-10 position publication, object matrices, both GPU assets, BSS
  constants, heap tables, upload records, draw packets, and shared tables;
- SEDS linkage, archive selection 36, terrain positioning, and convergence;
- the eight retail callback pairs in exact order; and
- the `0x800978FC`, `0x8008901C`, `0x800865A0`, and `0x80075228` tail.

`wm_80078D24` now performs the retail teardown sequence: audio shutdown and
SEDS release, shared world teardown helpers, five owned-allocation frees,
pool teardown, and terminal state publication (`F94E=272`, `F954=0`,
`BBC4=1`, `F950=BD3A`). The main-loop dispatcher resolves both lifecycle
addresses symbolically.

## Focused certificate

`pc_port/tests/run_w34n66_mode10_lifecycle.sh`:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- setup order and arguments, state publications, exact registration pairs,
  sound linkage, convergence, teardown order, frees, and terminal state: PASS
- M1 wrong transition argument: detected by `setup.transition_args`
- M2 wrong mode constant: detected by `setup.state_values`
- M3 swapped object/GPU-A stages: detected by `setup.stage_order`
- M4 wrong registration pair: detected by `setup.registration_pairs`
- M5 omitted sound linkage: detected by `setup.sound_link`
- M6 omitted palette tail: detected by `setup.stage_order`
- M7 omitted SEDS free: detected by `teardown.seds_free`
- M8 wrong teardown state: detected by `teardown.state_values`

## Regression and build gates

- W34N65 sequence/context callback certificate, M1–M11: PASS
- W34N61 mode-9 lifecycle certificate, M1–M9: PASS
- W34N57 mode-8/11 lifecycle certificate, M1–M5: PASS
- W34C1 scheduler cadence certificate, M1–M21: PASS
- canonical `./pc_port/build_port.sh`: LINK OK; port-owned addresses verified
- `git diff --check`: clean

The cadence certificate's standalone fixture gained inert definitions for the
two new lifecycle symbols. Its synthetic route does not dispatch mode 10; the
fixture change only keeps the production main-loop object link-complete.

## Natural-route result and acceptance boundary

The mode-10 natural route reaches setup, registers exactly eight occupied
scheduler records, and executes all eight initializer callbacks. On recurring
frame 1, slots 0, 1, and 2 execute successfully. Slot 6 then reaches
`wm_80079538 -> wm_80093A5C -> wm_800935DC`, where the plane solver aborts
because its input normal has `ny == 0`.

The failure is downstream of the lifecycle and originates in the pre-existing
`wm_80093A5C` body. That body contains explicit placeholder state (including a
zero-address `s6` source), duplicated packed-coordinate extraction, and a
fabricated zero coordinate. No lifecycle line was changed to mask the abort.

- mode-10 setup: naturally reached
- eight registration pairs: naturally reached and executed
- displayed frame 1: not completed
- frame 60 / frame 120: not reached
- captures: none
- teardown: not reached
- visual acceptance: BLOCKED

## Next exact fault

Transcribe retail `wm_80093A5C` exactly, certify its terrain/plane inputs, then
rerun mode-10 natural visual and teardown acceptance. Do not weaken
`wm_800935DC`'s zero-normal guard.
