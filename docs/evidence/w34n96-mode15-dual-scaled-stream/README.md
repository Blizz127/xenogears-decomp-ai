# W34N96 — mode-15 dual scaled stream

## Scope and retail anchor

- Starting HEAD: `ce502f74cdc96f4e1dc3f84214adb1fac42ff3b7`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail image: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Retail initializer: `[0x8007FC8C,0x8007FD30)`, 164 bytes,
  SHA-256 `f80cf59d17e4d380916903078ae6408fb1895db691f338ae4b2d2686d1e37364`
- Retail update: `[0x8007FD30,0x8007FF70)`, 576 bytes,
  SHA-256 `b837651d395e719555143f5b6cf448eb1e3078a88331bd687548724f9e28a52b`
- Full pair: `[0x8007FC8C,0x8007FF70)`, 740 bytes,
  SHA-256 `2388f5f12744842aee11edd62a84c7025f12785a4d6901163bc7d3284afa0cbe`
- Disassembly source:
  `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`

Only the private mode-15 scheduler pair and its symbolic scheduler resolution
are in scope. The mode-15 lifecycle is not part of this rung.

## Production transcription

`wm_8007FC8C` initializes both context-owned primitive streams through the
already-certified retail helper `wm_8007A06C`, then seeds the exact slot
position `(0x01498000, 0xFFF80000, 0x04AF2000)`, scale phases `0/-2048`,
brightness 128, and the halfword at `slot+0x20`. It returns scheduler state 3.

`wm_8007FD30`:

- consumes and clears latch 1;
- publishes the signed 20.12 slot position into both object records at
  context offsets `0x2FC` and `0x350`;
- copies the 32-byte retail base matrix to `context+0x368`, duplicates it to
  `context+0x314`, and scales the two matrices with the pre-increment phases;
- advances both phases by 384 with the signed 32767 clamp;
- selects each stream through `D_8009D7F0` and applies the current brightness
  through the accepted `wm_800809EC` RGB helper;
- subtracts three from brightness, returning state 1 while live and state 3
  after a negative result is clamped to zero.

The scheduler resolves guest callback addresses `0x8007FC8C/0x8007FD30`
symbolically. No guest address is cast to a native function pointer.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n96_mode15_dual_scaled_stream.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- retail initializer/update/full-slice hashes: PASS
- exact initializer stream sources, counts, position, phases, and halfword
  write: PASS
- duplicated position, matrix destination/order, scale vectors, signed phase
  clamps, active stream side, fade step, and terminal state: PASS
- scheduler init/update resolution with zero missing/invalid hits: PASS
- M1 wrong initial X: detected by `init.position`
- M2 omitted second stream: detected by `init.streams`
- M3 omitted latch clear: detected by `update.latch`
- M4 wrong position shift: detected by `update.position`
- M5 swapped scale vectors: detected by `update.scale_order`
- M6 wrong phase step: detected by `update.phase_steps`
- M7 wrong stream side: detected by `update.stream_side`
- M8 wrong fade step: detected by `update.fade_step`

All M1-M8 were killed by their named assertions. The normal port build passed
`LINK OK` with port-owned addresses verified.

## Natural boundary

A detached natural mode-15 run (entrance 15, accepted scripted input, frame
limit 120) reached mode-table slot 1 and reported:

```text
[worldmap-stub] guest=0x8007d918 lane=slot1 mode=15 slot=1 default_return=0
[worldmap-scheduler] ERROR: pool base null
```

The absent lifecycle setup means the scheduler pool is never allocated and no
presentation opens; the frame-60 capture request consequently remains
unfulfilled. The first exact blocker is therefore retail mode-15 lifecycle
setup `0x8007D918`, not this callback pair. The pair remains certified but
not naturally exercised until that lifecycle owner is restored.
