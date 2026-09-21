# W34N93 — retail mode-13 lifecycle

## Scope and retail anchor

- Starting HEAD: `081ed014a09a51358b23bf66b62c12650fff344d`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail source: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Disassembly: `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`
- setup `[0x8007FF70,0x80080218)`, 680-byte SHA-256:
  `4b9662d71a2529dba0e1885b0ae64747a5cabb3f6738c9f88fb14547244163bd`
- teardown `[0x80080218,0x8008032C)`, 276-byte SHA-256:
  `c252cfee38cec23471b3ba0d9fd4bceb23ef902aef811778a94a0e4c6ee89ff2`
- full lifecycle `[0x8007FF70,0x8008032C)`, 956-byte SHA-256:
  `d25fdc3dc80c30b402fc37337d1ff0d447b09e7278398867aafaf28dac44170a`

## Production behavior restored

`wm_8007FF70` now performs the exact mode-13 setup sequence: framebuffer
transition, archive-wave poll and `0x80076954` relocation, object pool and
matrix/state publication, WDS ownership, reset position
`(0x04100000,0xFFF80000,0x013C0000)`, resource stages, archive selection,
terrain convergence, six scheduler registrations, and the shared tail.

The six registrations are, in retail order:

1. `0x800923A8 / 0x800925A0`
2. `0x8008032C / 0x80080370`
3. `0x80080578 / 0x80080600`
4. `0x80080900 / 0x80080944`
5. `0x80080A28 / 0x80080AC4`
6. `0x80076A14 / 0x80076A1C`

`wm_80080218` now performs retail audio/SEDS shutdown, including the
mode-13-specific `wm_80086124` call, shared teardown, five allocation frees,
pool teardown, and terminal publication `F94E=132`, `F954=2`, `BBC4=1`,
`F950=BD3A`. The main-loop dispatcher resolves both lifecycle addresses.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n93_mode13_lifecycle.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- exact setup/teardown ordering and arguments: PASS
- exact template/state/position writes: PASS
- all six registration pairs: PASS
- WDS linkage/release and five-free sequence: PASS
- M1 wrong transition argument: detected by `setup.transition_args`
- M2 wrong mode constant: detected by `setup.state_values`
- M3 swapped object/GPU-A stages: detected by `setup.stage_order`
- M4 wrong final registration pair: detected by `setup.registration_pairs`
- M5 omitted sound linkage: detected by `setup.sound_link`
- M6 omitted palette tail: detected by `setup.stage_order`
- M7 omitted SEDS free: detected by `teardown.seds_free`
- M8 wrong teardown state: detected by `teardown.state_values`

The adjacent W34N92 camera/control certificate also passes O0/O2/UBSan with
M1-M8 detected. The isolated normal port build reports `LINK OK`.

## Natural-route discriminator

The detached product run selected entrance/mode 13 and entered the restored
setup normally. Its first scheduler pass found all six occupied records. Five
initializers resolved and executed; the sixth stopped at the separately
unowned shared draw initializer:

```text
[worldmap-scheduler] slot=4 state=0 cb=0x80080a28 executed ret=3
[worldmap-stub] guest=0x80076a14 kind=invalid_callback count=1 slot=5 state=0 default_return=0
[worldmap-scheduler] pass complete slots=64 occupied=6 dispatched=6 executed=5
```

The upload pumps continued with `unknowns=0`, but without the shared draw
record no effective presentation fulfilled the frame-60 capture request:

```text
[worldmap-capture] ERROR unfulfilled request_frame=60 completed_frame=60
[worldmap-open-loop] capture fulfillment failed frame=60
```

This is an honest integration boundary, not a reason to alter mode-13
lifecycle semantics. The setup and teardown are structurally certified; the
natural visual gate remains open until the retail shared pair
`0x80076A14/0x80076A1C` is owned by the scheduler.

## Verdict and next target

`LIFECYCLE_RESTORED_NATURAL_BLOCKED_BY_SHARED_DRAW_CALLBACK`

Next exact target: transcribe and certify the bounded shared draw callback
pair `[0x80076A14,0x80076B34)`, register it symbolically, then repeat this
same detached mode-13 gate without changing the lifecycle.
