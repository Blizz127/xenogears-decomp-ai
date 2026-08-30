# W34N42 — complete retail `wm_8008615C` 5x5 FT4 dispatch

Verdict: **FULL_BODY_TRANSCRIBED_AND_CERTIFIED**.

## Anchor

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `0aa57707623e84c1cdc09958a847cfca7fe5a354`
- Retail authority:
  `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`, function
  `[0x8008615C,0x800863E0)` (161 instructions)
- Date: 2026-08-30

W34N41 first completed the four-argument `wm_80099BFC` dependency. This rung
restores its exact caller rather than leaving that body unreachable behind the
former approximate setup helper.

## Completed retail sequence

The helper now:

1. writes the four shared SVECTORs at scratch offsets `0/8/0x10/0x18`;
2. copies the camera matrix to scratch `+0x28` and the constant model matrix to
   `+0x48`, then applies `RotMatrixZ(-D_8009BD3C)` to the model rotation;
3. copies all 16 CLUT halfwords from `0x8009D478` to scratch `+0x68`;
4. clears the compacted output count at `0x8009BE04` and walks the exact 5x5
   validity grid at `0x8009D618`;
5. resolves each enabled cell through `(map_z+row)*9 + map_x+column`, the tile
   map at `0x8009D570`, and the `0x08` record table rooted at the pointer in
   `0x8009C7EC`;
6. skips zero-count records and calls `wm_80099BFC(entries,count,ot,packet)`
   using the active draw record's OT, the buffer-selected root at
   `D_8009D7E8[D_8009D7F0]`, and the latest compacted count times `0x28`.

This replaces the former nine-vertex approximation, host-stack temporary
matrix, positive heading, and entirely omitted 5x5 dispatch.

## Certificate

`pc_port/tests/run_w34n42_8615c.sh` passes O0, O2, and nonrecovering UBSan with
focused warnings clean. Its grid contains two productive enabled cells, one
enabled zero-count cell, and 22 disabled cells. The submitter stub advances the
shared output count so the second call proves that packet compaction is observed
across calls rather than recomputed locally.

Seven named mutants are detected:

| mutant | failure mode |
|---|---|
| M1 | positive rather than negative heading |
| M2 | copy only eight CLUTs |
| M3 | walk 4x4 rather than 5x5 |
| M4 | use a nine-wide validity stride |
| M5 | omit the `-1` validity gate |
| M6 | use an eight-wide tile map |
| M7 | ignore the active packet buffer |

## Build and natural regression

The normal product build reports `LINK OK`. The accepted detached route again
used the unseeded frame-601 exit schedule. Capture fulfillment remained
same-frame and all standing hashes remained byte-identical:

```text
frame 60  4310197d3881367bc7859ef174dcf4b9afa8759f0b7dc2ca3eea94b0f84f1521
frame 120 8039c2f96b88e09491eaf1cba049d0efd829f146aa943da744f56c93e85b1c16
frame 600 2b636ae21af4bf0003f60d5e53f8ffd9a042fc5a72457e4c096cff19814e1a38
```

The route naturally reached `frames=601 D7CC=0` with no primitive-link or OT
adapter abort. Neutral captures mean that the accepted view did not publish a
new surviving wrapped FT4; they do not weaken the synthetic certificate's
proof of productive cells and compacted call arguments.

## Bound

Both `[0x8008615C,0x800863E0)` and its dependency
`[0x80099BFC,0x80099E88)` are now complete. The next live `r4world` frontier is
the still-approximate terrain/render helpers adjacent to this dispatch, not
another extension of either completed boundary.
