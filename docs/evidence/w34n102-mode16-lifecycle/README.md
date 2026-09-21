# W34N102 — mode-16 lifecycle

## Scope and retail anchor

- Starting HEAD: `c73f9a5216e6da768023da4eae248f65d20b7ee4`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail image: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Disassembly: `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`
- setup `[0x80080D00,0x8008106C)`, 876-byte SHA-256:
  `56b693c92cec3c3ad36ab58f0c94e98d718f91d7b699396b0289e0c8cbdef6da`
- teardown `[0x8008106C,0x80081174)`, 264-byte SHA-256:
  `3d75170cc44b97a7b11cfaa4b14310794e4a4e7370d19f57410ed2f060506337`
- complete lifecycle `[0x80080D00,0x80081174)`, 1140-byte SHA-256:
  `daa11d626943c7d8ed410c704cbd69ef15a820604c2a11fbe68d2847fefa7677`

## Production transcription

`wm_80080D00` restores the retail mode-16 session setup: framebuffer
transition, second archive-wave completion, object-pool creation, base matrix
and fixed state, WDS cleanup, initial world position, object/GPU/third-wave
asset stages, heap/upload/draw stages, WDS and song ownership, terrain
convergence, eight scheduler registrations, and the shared world-map tail.

The registered callback pairs, in retail order, are:

1. `0x800923A8 / 0x800925A0`
2. `0x80081174 / 0x800811C0`
3. `0x800813E8 / 0x80081470`
4. `0x800817A0 / 0x80081868`
5. `0x800819C8 / 0x80081B24`
6. `0x80081C3C / 0x80081D80`
7. `0x80081FB4 / 0x80081FD8`
8. `0x80078948 / 0x80078950`

`wm_8008106C` restores the corresponding audio, scheduler, graphics, heap,
and pool teardown and the terminal state writes (`F94E=506`, `F954=0`,
`BBC4=1`, and `F950=BD3A`). The mode table dispatches both callbacks
symbolically. The second-wave finish seam is shared with the accepted archive
pipeline rather than duplicated.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n102_mode16_lifecycle.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- all three retail slice hashes: PASS
- setup call order, arguments, state writes, fixed position, and tables: PASS
- all eight callback registration pairs and order: PASS
- WDS/song ownership and convergence sequence: PASS
- teardown order, frees, and terminal state: PASS
- symbolic mode-table dispatch: PASS
- no host-pointer truncation in the lifecycle implementation: PASS
- M1 wrong transition argument: detected by `setup.transition_args`
- M2 wrong mode constant: detected by `setup.state_values`
- M3 swapped object/GPU-A stages: detected by `setup.stage_order`
- M4 wrong callback pair: detected by `setup.registration_pairs`
- M5 skipped WDS load: detected by `setup.wds_load`
- M6 skipped song start: detected by `setup.song_start`
- M7 skipped SEDS free: detected by `teardown.seds_free`
- M8 wrong terminal state: detected by `teardown.state_values`

All M1-M8 were killed by named assertions. The normal product build completed
with `LINK OK`.

## Natural entrance-16 route

The detached entrance-16 route completed 120 recurring frames, fulfilled both
capture requests in their requested frames, returned from the bounded loop,
and reported `unknowns=0` from both upload pumps.

- frame 60 SHA-256:
  `6e0771f9c0ea4951783d453b448a35882dc2e31ae0e4c6ea4decc4d89851d9f4`
- frame 120 SHA-256:
  `af9dfb802c212976135ee3b089d998828cb230e320383c8a87a6b78972c86860`

The two images differ, proving that the restored lifecycle produces evolving
mode-16 output. Frame 60 contains a dark navy scene with thin colored geometry
near the top and a pale object at upper right; frame 120 retains the dark
background and top geometry but no longer contains that object.

The shared pair `0x80078948/0x80078950` resolved naturally. Six private mode-16
initializer addresses remained unresolved and each produced 121 stub hits:

- `0x80081174`
- `0x800813E8`
- `0x800817A0`
- `0x800819C8`
- `0x80081C3C`
- `0x80081FB4`

Because their initializers are absent, their corresponding update callbacks do
not become active. This is the bounded reason the captures are not treated as
complete mode-16 presentation.

## Verdict and next target

`MODE16_LIFECYCLE_RESTORED_NATURAL_120_FRAME_PASS`

The next exact target is the smallest first private pair,
`0x80081174/0x800811C0`. Its initializer spans only 76 retail bytes; resolving
it removes the first per-frame stub and activates the associated scripted
control callback without broadening into the other five pairs.
