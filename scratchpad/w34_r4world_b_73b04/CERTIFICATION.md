# W34-R4WORLD-B certification — 0x80073B04

## Retail authority

- Parent canonical SHA: `7dd3fd6a1d4c68bd9dec01b01190732fd5ba5fbe`
- World image: 180422 bytes, SHA-256 `4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70`, load base `0x8006FAF0`.
- Fresh boundary: `[0x80073B04,0x80073E30)`, 812 bytes, 203 instructions, slice SHA-256 `87f9ff21fffad640d8fb7ec9a96da4f5d29a71275c4b15452d49677b6ad23197`.
- ABI: leaf `void wm_80073B04(void)`; no argument or return value; frame `0x40`, `$ra=0x3C`, `$s0..$s4=0x28..0x38`.
- Fresh decode: five direct JALs (`RotMatrixYXZ`, `CompMatrix`, `SetRotMatrix`, `SetTransMatrix`, `RotTransPers4`), no JALR, one two-iteration loop, one `bltz` bit31 guard, no inline COP2 instructions.

## Geometry and matrix fidelity

The source is static signed `SVECTOR[8]` at `0x8009A300`, stride `0x20`, two quads, four vertices each, with source order `(TL, TR, BL, BR)`. Coordinates are signed 16-bit world values; no vertex count, object, terrain, or model structure is read.

The exact sequence is:

`angles={vx=0,vy=theta,vz=0}` → `RotMatrixYXZ(angles,R@0x1F800038)` → clear `R.t` at absolute `0x1F80004C/50/54` → `CompMatrix(camera@0x8009C808,R,C@0x1F800018)` → `SetRotMatrix(C)` → `SetTransMatrix(C)` → `RotTransPers4` for each quad. Translation comes from the camera; the model translation is zero. The output is screen XY, with depth from the second/last `RotTransPers4`; `p` is ignored. Only the last flag’s bit 31 rejects both quads.

## Packets, bucket, and links

Active packet addresses are `0x8009C744 + quad*0x50 + idx*0x28`, with `idx=*(u32*)0x8009D7F0`. The two `POLY_FT4` packets are 40 bytes; UV halfwords are `V`, `V|0x80`, `V|0x3F00`, `V|0x3F80`, where `V=(theta>>2)&0x7F`. DR_TWIN A/B are `0x8009D3D8/0x8009D3E4`.

The bucket is exactly:

`bucket = arithmetic_right_shift(last_otz, *(s32*)0x80050100)`

then `OT = *(*(u32*)0x8009BE3C + 0x70) + (bucket << 2)`. It is variable, arithmetic, fixed for all four insertions, and unclamped. Every tag operation preserves the high byte with `0xFF000000` and replaces only low 24 bits with `0x00FFFFFF`-masked pointers.

The four pushes are `twinB → prim0 → prim1 → twinA`, yielding head-first draw order `twinA → prim1 → prim0 → twinB → prior`.

## Gates

- Focused oracle: O0, O2, and nonrecovering UBSan O2 all pass 6/6 asymmetric cases with identical normalized logical output and zero UBSan diagnostics.
- Mutants: M1–M23 all compile and fail nonzero with named assertions; see `MUTANTS.csv`. M17 is caught by the explicit link-publication trace despite equal final memory post-image. M18–M23 independently cover wrong camera source, vertex-0/1 swap, projected-destination corruption, bucket clamping, logical-vs-arithmetic shift, and primitive pointer/stride advance.
- Strict focused warnings: `-Wall -Wextra -Wconversion -Wsign-conversion -Werror`, clean for the helper and harness.
- Canonical build: `LINK OK` twice; exactly one real `T wm_80073B04`; no generated stub and no scheduler resolver ownership.
- Regression: exact 15-row matrix, all PASS, zero waivers; see `REGRESSION_MATRIX.md`.

No natural execution claim is made. The helper was tested through its independent memory-state oracle ahead of scheduler reachability; neither `0x80071A58` nor any scheduler callback was forced.
