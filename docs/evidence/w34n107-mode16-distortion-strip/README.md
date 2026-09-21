# W34N107 — mode-16 distortion strip

## Scope and retail anchor

- Starting HEAD: `00cffe21c0668512fd9bdc05fe2c8c0759f12de5`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail image: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Disassembly: `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`
- initializer `[0x80081C3C,0x80081D80)`, 324-byte SHA-256:
  `be3a3d2145779e0491bae8222e8fa7e106cd06f072926f7f2f32a48e9d1d9f85`
- update `[0x80081D80,0x80081FB4)`, 564-byte SHA-256:
  `5afe7efefdd15e45ed722657d3d3de04d962448b11ae2234ea83153c539470d3`
- complete pair `[0x80081C3C,0x80081FB4)`, 888-byte SHA-256:
  `157d1127b1e5d3dafa1f47894557070b89000a38ddb8171eaede5d57b6126566`

## Production transcription

`wm_80081C3C` restores the fifth private mode-16 initializer:

1. allocates two 7,680-byte guest-resident primitive buffers and one
   384-byte width table;
2. publishes all three allocations as guest addresses at `D158`, `D15C`,
   and `D148`, avoiding raw native-pointer truncation;
3. initializes all 192 40-byte records in the first buffer as retail
   length-9, opcode-44-plus-bit-0 textured quads, RGB 128, and
   `GetTPage(2,0,640,256)`;
4. mirrors the complete initialized buffer; and
5. seeds every width-table halfword to 1.

`wm_80081D80` restores the recurring strip/copy update:

- selects the current `D7F0` primitive buffer;
- generates all 192 adjacent scanline quads using
  `rand() % width - width/2`, exact XY/UV layout, and retail byte/halfword
  truncation;
- links every quad into the active guest OT while preserving packet length;
- builds the parity-selected `DR_MOVE` for source rectangle
  `(64, side*216, 192, 216)` to destination `(640,256)`; and
- links that move packet after the strip chain.

The scheduler resolves `0x80081C3C` and `0x80081D80` symbolically.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n107_mode16_distortion_strip.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- initializer/update/full retail slice hashes: PASS
- exact three allocation sizes/flags and guest publication: PASS
- all 192 texture-page calls and primitive layouts: PASS
- complete 7,680-byte buffer mirror: PASS
- all 192 width seeds: PASS
- selected-buffer strip writes and deterministic centering samples: PASS
- full primitive-to-OT-to-move chain: PASS
- exact `SetDrawMove` record, rectangle, and destination: PASS
- symbolic scheduler initializer/update resolution: PASS
- M1 raw host-pointer publication: detected by `init.alloc_publish`
- M2 wrong first allocation size: detected by `init.alloc_sizes`
- M3 wrong primitive opcode: detected by `init.primitive_layout`
- M4 skipped buffer mirror: detected by `init.buffer_mirror`
- M5 wrong width seed: detected by `init.width_seed`
- M6 wrong selected buffer: detected by `update.selected_buffer`
- M7 skipped width centering: detected by `update.strip_geometry`
- M8 wrong move destination: detected by `update.move_packet`

All M1-M8 were killed by named assertions. The normal product build completed
with `LINK OK`.

## Natural entrance-16 route

The clean detached entrance-16 route executed `0x80081C3C` once and
`0x80081D80` 120 times. Neither produced a stub hit. Both capture requests
were fulfilled in-frame, both upload pumps retained `unknowns=0`, and the
bounded loop returned normally after exactly 120 frames.

- frame 60:
  `/home/blizz/dev/xenogears-decomp/scratchpad/w34n107_mode16_capture/world-frame-000060.bmp`
- frame 60 SHA-256:
  `2a5aaa0abc643ca2c3bbd4a564d967ee06a7105aebeed2343b2886513e535f63`
- frame 120:
  `/home/blizz/dev/xenogears-decomp/scratchpad/w34n107_mode16_capture/world-frame-000120.bmp`
- frame 120 SHA-256:
  `1664fa97b0704c07cfec4ff5670b92aa7bbf4e01db71d5fe19b3e842932f4a8a`

Both hashes differ from the committed W34N106 baselines
(`eb6195d9...` / `de1764e1...`) and from each other. Visual inspection shows
the coherent tower/terrain scene with the restored tall rectangular
distortion/copy region around the tower. This is a material presentation
advance attributable to the strip and move-packet path.

Only one private mode-16 initializer remains unresolved, with 121 stub hits:

- `0x80081FB4`

## Verdict and next target

`MODE16_DISTORTION_STRIP_RESTORED_NATURALLY_EXECUTED_VISUAL_ADVANCE`

The next target is the final private pair `0x80081FB4/0x80081FD8`.
