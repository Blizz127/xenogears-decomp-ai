# W34N89 — mode-13 marker callback

## Scope and retail anchor

- Starting HEAD: `fc665b354ccdf4216a629adc897a833d9735c38f`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail image: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Retail slice: `[0x80080900,0x800809EC)`, 236 bytes
- SHA-256: `195e8b7fde30969a64b3014c3f7e030f73a331d5af6fdac2c577809bb9b04bf4`
- Disassembly source: `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`

This is the smallest self-contained unowned callback pair registered by the
retail mode-13 setup at `0x8007FF70`. No lifecycle or adjacent mode-13
callback was changed.

## Production transcription

`wm_80080900` uses the scheduler's 0x80-byte slot stride and copies the three
mode reset coordinates at `C5AC/C5B0/C5B4` into slot offsets
`+0x28/+0x2C/+0x30`.

`wm_80080944` reacts only to latch value 1. It clears the latch, converts the
three 20.12 fixed-point coordinates to signed halfwords in guest scratch
`0x1F8000A0`, and publishes the same vector to retail marker records 40, 41,
and 42 in that order. Both callbacks return 1. The bounded scheduler now
resolves both guest addresses symbolically; no guest address is cast to a
native function pointer.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n89_mode13_marker.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- exact reset-source and slot write set: PASS
- signed 20.12 conversion, latch guard/clear, marker order and arguments: PASS
- scheduler init/update resolution with zero missing/invalid hits: PASS
- M1 wrong slot stride: `init.position_copy`
- M2 wrong reset source: `init.position_copy`
- M3 wrong latch value: `update.latch_clear`
- M4 omitted latch clear: `update.latch_clear`
- M5 wrong vector shift: `update.vector_shift`
- M6 wrong marker id: `update.marker_ids`

All M1-M6 were detected by their named assertions.

Adjacent W34N86 mode-12 camera/control regression: PASS at
O0/O2/nonrecovering UBSan with M1-M8 detected.

Normal port build: `LINK OK`.

## Acceptance boundary

Mode 13 is not yet integrated into the mode table because its setup registers
three additional unowned mode-specific pairs. This pair is therefore
certificate-complete but intentionally not claimed as naturally exercised.
The next code-producing target is the next smallest registered mode-13 pair,
`0x8008032C/0x80080370` or `0x80080578/0x80080600` after comparing their
bounded retail bodies.
