# W34N94 — mode-13 shared draw callbacks

## Scope and retail anchor

- Starting HEAD: `91269d2b00d6c77e1e1cabf8881a8cfa476d1494`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail source: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Disassembly: `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`
- initializer `[0x80076A14,0x80076A1C)`, 8-byte SHA-256:
  `5ce5ad86d452c4d2422bd63e15223d5d6b3dfb77f224a88c1f476e9fb34e359d`
- update `[0x80076A1C,0x80076B34)`, 280-byte SHA-256:
  `19a29eee50ec5ae67dd91bc44d9bcc464ba0189d52480eb0d4ce58f652487bb0`
- full pair `[0x80076A14,0x80076B34)`, 288-byte SHA-256:
  `fe05b5cdd6f6c88c87aca7429f5446e5c5131b675d2637c2130cb875c68e0e07`

## Production behavior restored

`wm_80076A14` is the retail no-write initializer returning state 1.

`wm_80076A1C` now performs the exact shared mode-13 draw sequence:

1. select `wm_80097440` or `wm_80097244` from `D144`;
2. particles and scaled-object rendering from `BBB4`;
3. vertex setup, region models, and position wrap;
4. the conditional `BE28` paging chain;
5. visibility and terrain-context setup from `BE28`;
6. advance `C5BC` by `0x40`;
7. submit terrain, sky, and the final tiled-object draw.

The scheduler recognizes both guest addresses symbolically. They remain in
the known-callback inventory so a focused scheduler link without the
production body reports a bounded missing callback rather than treating the
retail address as invalid.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n94_mode13_shared_draw.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- no-paging and paging call order/arguments: PASS
- render-position versus terrain-position selection: PASS
- input read-only/write-set (`C5BC` only): PASS
- initializer and update scheduler resolution: PASS
- missing/invalid scheduler hits: zero
- M1 swapped camera branch: detected by `no_paging.camera_branch`
- M2 skipped vertex setup: detected by `no_paging.vertex_setup`
- M3 skipped position wrap: detected by `no_paging.position_wrap`
- M4 skipped paging chain: detected by `paging.chain_present`
- M5 wrong terrain position: detected by `paging.visibility_position`
- M6 wrong palette step: detected by `no_paging.palette_step`
- M7 skipped terrain packets: detected by `no_paging.terrain_packets`
- M8 skipped final draw: detected by `no_paging.final_draw`

The W34N93 lifecycle regression remains green with O0/O2/UBSan and M1-M8.
The first full build collided with another agent using the shared build
directory and produced the established false duplicate-definition signature.
After that builder completed, the isolated retry reported `LINK OK` without a
source change.

## Detached natural acceptance

The exact W34N93 discriminator was repeated with entrance/mode 13, GDB
detached before `PcPort_WorldMapInitMain`, and a 120-displayed-frame bound.

- six scheduler records were occupied;
- all six initializers resolved and executed;
- all recurring callbacks resolved;
- scheduler missing/invalid callback hits: zero;
- both upload pumps reported `unknowns=0` throughout;
- frame 60 was requested and fulfilled as frame 60;
- frame 120 was requested and fulfilled as frame 120;
- bounded exit occurred at exactly 120 displayed frames.

Captures:

- `scratchpad/w34n93_mode13_capture/world-frame-000060.bmp`
  SHA-256 `8ea9fc609e1370ab6310fc8f68e19c30bce7417ec41ba99c674df729d837a278`
- `scratchpad/w34n93_mode13_capture/world-frame-000120.bmp`
  SHA-256 `eb7ce6811d882017cf9c0e2d58cca78428fcb0e681fe2ad941e7d9acf83214de`

Visual inspection shows coherent textured ocean, mountains/coastline, sky,
and the airborne object at both frames. The viewpoint and scene content
change between frames 60 and 120, so this is evolving mode-13 output rather
than a static presentation.

## Verdict and residual

`MODE13_NATURAL_120_FRAME_PASS`

W34N93's natural blocker is closed. The remaining lifecycle acceptance is a
separate natural run long enough for the retail mode-13 scripted-control table
to issue command 64, clear `D554/D7CC`, execute `wm_80080218`, and return to
the field-state lane without shortening any timer or forcing any state.
