# W34C6 — terrain base height reads the wrong byte

Branch `experiment/worldmap-open-gates-20260823`, base `ab0f1db6`.
Production change under rule 3 (the producer computes the wrong number; no
probe can correct it). Scope: one expression in `wm_80099708` and the
oracle that certified it.

## Retail contract (disc/world_map.bin, [0x80099708,0x8009980C))

The per-cell base height is the **low byte** of the packed tile word,
sign-extended and multiplied by 8:

- branch taken: `0x80099790 sll v1,v1,0x18`; `0x80099794 sra v1,v1,0x15`
  → `(s8)(packed & 0xFF) << 3`, added to `(sine_z * sine_x_twice) >> 20`
  (`0x8009978C mult`, `0x8009979C sra 0x14`), then `sll 0x10`.
- branch not taken: `0x800997AC sll v0,v1,0x18`; `0x800997B0 sra v0,v0,5`
  → `((s8)(packed & 0xFF) * 8) << 16`.

The high byte of the same word is colour, decoded by `wm_8009980C` at
`0x80099858-0x80099870` (`srl 0xC & 0xF0`, `srl 8 & 0xF000`, `sra 0xD & 3`).

## Port before this rung

`world_map_helper_99708.c` used `wm_99708_sign8(packed >> 24) * 8` — the
high (colour) byte — in both branches. `pc_port/tests/w34b64_terrain_prod_test.c`
(`de24a5c3`) certified exactly that: its oracle `expected_y` was
`(s8)(packed >> 24) * 8`, and the suite had no mutants, so a port-shaped
oracle passed against a port-shaped body (the 89160 pattern).

## Change

- `wm_99708_base_height(packed)` = `sign8(packed & 0xFF) * 8` (retail PCs in
  the comment); old shape kept as mutant `WM_99708_MUTANT_HIGH_BYTE`.
- `expected_y` in the certificate now takes the low byte; the fixture already
  had distinct low/high bytes (`row*3+1` vs `row*37+0x83`).
- `run_w34b64_terrain.sh` gains the mutant stage; it must fail
  `grid Y follows packed height contract`. Result: PASS O0/O2/UBSan,
  `grid mutant HIGH_BYTE: DETECTED`.

## Natural proof (W34C3 probe2 harness, read-only)

Same substrate; 81 slots allocated, 100/100 frame-60 dispatches on guest
heap slots (unchanged from W34C5). Frame-60 SHA-256
`ec2ea281009bfff1…` (was `adacdb46…`). The capture
still shows malformed terrain: the random shards are gone and the field is
now dominated by tall vertical spikes — heights are no longer colour bytes,
but the vertex→screen transform is still not retail. No fault, no adapter
abort, bounded exit at 120.

## Limitations

- Retail-comparable is not claimed; only the change in character is
  described.
- The sine term (term A) and the vertex x/z assembly were re-read against
  retail (`0x80099744-0x800997E8`: x += 0x80 per column, z -= 0x80 per
  row, angle_z += 0x200 per column, angle_x += 0x200 per row) and match; the
  rest of the transform (`0x8009D534` matrix source, the composite at
  scratch+0x370, `wm_8009980C`'s emission) is not verified by this rung.

## Next (one task)

W34C7 (read-only): at frame 60, for one dispatched tile, dump the origin,
the `0x8009D534` matrix source, `0x8009C808`, the composite at scratch+0x370,
the position `0x8009BBB4`, and the first two rows of scratch vertices; then
establish the retail producer of `0x8009D534` (rule 1) and compare
`wm_8009980C`'s vertex indexing/projection against `[0x8009980C,0x80099BFC)`.
