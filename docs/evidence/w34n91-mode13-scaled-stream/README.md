# W34N91 — mode-13 dual scaled stream

## Scope and retail anchor

- Starting HEAD: `a4d50956a477440aac4beeb9f11d0779d028a5d9`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail image: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Retail unit: `[0x800809EC,0x80080D00)`, 788 bytes
- SHA-256: `73c678706ea4d808fdccb738e0450671a63a8ee4491b3e0a04d0442a2e0a3991`
- Disassembly source: `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`

The unit contains the direct RGB-stream helper `wm_800809EC` and the
mode-13 callback pair `wm_80080A28/wm_80080AC4`. No lifecycle or other
mode-13 callback changed.

## Production transcription

The initializer seeds the retail fixed position, two scale phases, and fade
value, then initializes both 0x54-byte context-owned primitive streams through
the already-certified `wm_8007A06C` helper. It returns scheduler state 3.

The update callback:

- consumes and clears latch 1;
- publishes the signed 20.12 position into both context transforms;
- copies the 32-byte retail base matrix into both matrix records;
- scales them with the two pre-increment phase vectors;
- advances both phases by 384 and clamps them at 32767;
- selects the active primitive side through `D7F0` and writes the current RGB
  fade to every 40-byte record in both streams;
- decrements brightness by four, returning state 1 while live and state 3
  after clamping a negative result to zero.

The scheduler resolves both callback guest addresses symbolically.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n91_mode13_scaled_stream.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- RGB helper count/stride/write bounds: PASS
- initializer streams, position, phase, return state: PASS
- transform duplication, matrix/scale order, phase clamps: PASS
- active-side tint, fade step, terminal clamp/state: PASS
- scheduler init/update resolution with zero missing/invalid hits: PASS
- M1 wrong initial Y: `init.position`
- M2 omitted second stream: `init.streams`
- M3 omitted latch clear: `update.latch`
- M4 wrong position shift: `update.position`
- M5 swapped scale vectors: `update.scale_order`
- M6 wrong phase step: `update.phase_steps`
- M7 wrong stream side: `update.stream_side`
- M8 wrong fade step: `update.fade_step`

All M1-M8 were detected by their named assertions.

Adjacent W34N90 mode-13 scripted-control certificate: PASS at
O0/O2/nonrecovering UBSan with M1-M8 detected.

Normal port build: the first attempt collided with a concurrent shared-tree
builder and produced unrelated duplicate-definition noise; the isolated rerun
after that process exited passed `LINK OK` without a source change.

## Acceptance boundary

Three of four private callback pairs registered by mode-13 setup are now
owned. The sole remaining callback unit is `0x80080578/0x80080600`; after it
is restored, mode-13 lifecycle integration can proceed without a known
scheduler stub boundary.
