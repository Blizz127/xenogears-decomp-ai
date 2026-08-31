# W34N97 — retail mode-15 lifecycle

## Scope and retail anchor

- Starting HEAD: `d04fc571bb1a4042f22718e88acb56693e825605`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail source: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Disassembly: `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`
- setup `[0x8007D918,0x8007DCE0)`, 968-byte SHA-256:
  `ab9b837b1fbe2cd151872dbef46adde5d4bc326f32eda9ff8f25f9858b1b2e2e`
- teardown `[0x8007DCE0,0x8007DE14)`, 308-byte SHA-256:
  `99b7a4b704e53f845351db4e8631ae9722bd3677b165027521eb334340eff143`
- full lifecycle `[0x8007D918,0x8007DE14)`, 1,276-byte SHA-256:
  `10a992b3a1d3a72956daa00fe9ed27f42807376b48bdadc0ecc1f046bc0c3b8f`

## Production behavior restored

`wm_8007D918` now owns the retail mode-15 session setup. It performs the
framebuffer copy and transition, finishes the mode's second archive wave,
constructs the object pool and resource chain, copies the retail base matrix,
publishes the mode constants and callback pointer, and selects the initial
20.12 position from the eight-byte-stride table at `0x8009A5B4`.

After terrain convergence it completes WDS/SEDS/song ownership and registers
the eleven retail scheduler records, in order:

1. `0x800923A8 / 0x800925A0`
2. `0x8007DE14 / 0x8007DE98`
3. `0x8007E450 / 0x8007E4E4`
4. `0x8007ECA4 / 0x8007EE34`
5. five instances of `0x8007F8AC / 0x8007F968`
6. `0x8007FC8C / 0x8007FD30`
7. `0x80078948 / 0x80078950`

The setup finishes with the retail shared tail. `wm_8007DCE0` performs the
corresponding audio/SEDS shutdown, shared resource teardown, five allocation
frees, pool release, and terminal-state publication (`F94E=417`, `F950=BD3A`,
`F954=A5CC[D3D4]`, `BBC4=1`). Both lifecycle addresses are resolved
symbolically by the main-loop dispatcher.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n97_mode15_lifecycle.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- setup/teardown/full retail hashes: PASS
- exact setup ordering, arguments, matrix/state/position writes: PASS
- all eleven scheduler records in retail order: PASS
- WDS/SEDS/song ownership and five-free teardown sequence: PASS
- M1 wrong transition argument: detected by `setup.transition_args`
- M2 wrong position stride: detected by `setup.position_vector`
- M3 swapped object/GPU-A stages: detected by `setup.stage_order`
- M4 wrong repeated registration count: detected by
  `setup.registration_count`
- M5 skipped WDS load: detected by `setup.wds_load`
- M6 skipped song start: detected by `setup.song_start`
- M7 skipped SEDS free: detected by `teardown.seds_free`
- M8 wrong terminal state: detected by `teardown.state_values`

All M1-M8 were killed by named assertions. W34N96 and the neighboring W34N93
lifecycle certificates remain green. The normal product build completed with
`LINK OK` and port-owned addresses verified.

## Natural route

The accepted detached run selected entrance 15, completed setup, allocated a
non-null scheduler pool at `0x800C6B0C`, ran 120 displayed frames, fulfilled
both capture requests in their requested frames, and returned normally from
the bounded loop. Captures:

- frame 60: `scratchpad/w34n97_mode15_capture/world-frame-000060.bmp`,
  SHA-256 `da5072d11def85a34c931bc34cd2a72039848f80bf772570bf101bd776097d89`
- frame 120: `scratchpad/w34n97_mode15_capture/world-frame-000120.bmp`,
  SHA-256 `96effaee2d5ea2ae45ae8439e972c1fe7424ae902a7a6609649c2f7c9d32e135`

The first scheduler pass reported 11 occupied records and naturally executed
the shared `923A8`, restored W34N96 `7FC8C`, and shared `78948` initializers.
The remaining private mode-15 records were reached but unresolved:

```text
0x8007DE14  slot 1, one hit per pass
0x8007E450  slot 2, one hit per pass
0x8007ECA4  slot 3, one hit per pass
0x8007F8AC  slots 4-8, five hits per pass
```

The upload pumps remained at `unknowns=0`. The bounded 120-frame acceptance
does not naturally select slot 2, so teardown is structurally certified here
but still needs a later natural-exit witness.

## Verdict and next target

`LIFECYCLE_RESTORED_NATURAL_120_FRAME_PASS`

Next exact target: decode, transcribe, and certify the first missing private
mode-15 scheduler pair `0x8007DE14 / 0x8007DE98`, then repeat this identical
natural discriminator without changing lifecycle semantics.
