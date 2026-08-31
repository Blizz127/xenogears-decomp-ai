# W34N69 — mode-10 particle texture fields

Verdict: **RETAIL_TEXTURE_FIELDS_RESTORED**.

## Anchor and authority

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `b643ac52d69f46aa54051ae5860486dbaa069f1a`
- Retail listing:
  `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`
- Retail function: `wm_8008901C`, `[0x8008901C,0x80089128)`

The normal mode-10 frame rendered the explosion particles as opaque dark
quadrilateral slabs.  Suppressing `wm_80089C78` removed the slabs, while the
object matrices, projected geometry, UVs, packet length/opcode, texture page,
and texture window remained structurally valid.

## Discriminator chain

The mode-10 TIM upload is ordinal 107:

```text
mode=0x00000009
CLUT=(256,511,256,1), source hash=5fb6fafd
pixels=(832,256,64,256), source hash=2b647fbb
```

At the draw, CPU VRAM and PsyCross's active GPU texture matched exactly:

```text
CLUT rectangle hash = 784f59eb
page rectangle hash = 27ae67e2
```

The texture's intended transparent background is palette index 63, whose
CLUT value is `0x0000`; 1,873 of the model-1 texture's 4,096 pixels use it.
A temporary shader diagnostic that discarded index 63 removed every slab and
revealed a coherent particle cloud.  An explicit lookup of CLUT `(319,511)`
produced the same frame byte-for-byte.  These diagnostics exonerated the TIM,
VRAM upload, object geometry, UV interpolation, and transparency rule.

The decisive packet-to-renderer witness was instead:

```text
pre-fix:  page=00bd clut=00bd uv=4020/5f20/403f/5f3f
```

Every generated vertex received the texture-page word as its CLUT.

## Retail divergence and repair

Retail stores the two helper results in this order:

```text
0x80089090 jal GetTPage(1,1,832,256)
0x800890A4 sh v0,15(s0)  -> packet+0x16 tpage = 0x00bd
0x800890A0 jal GetClut(256,511)
0x800890B0 sh v0,7(s0)   -> packet+0x0e clut  = 0x7fd0
```

Here `s0 = packet + 7`.  The port's two C assignments were reversed even
though the surrounding comment described the retail order correctly.  The
old focused test repeated that wrong oracle.  The repair swaps the two stores
to their retail packet fields and corrects the assertions.

The post-fix packet-to-renderer witness is:

```text
post-fix: page=00bd clut=7fd0 uv=4020/5f20/403f/5f3f
```

No PsyCross shader or renderer change is part of the repair.

## Certificate

`pc_port/tests/run_w34n69_8901c_texture_fields.sh` runs the production-linked
`wm_8008901C` certificate under:

```text
O0     PASS
O2     PASS
UBSan PASS (nonrecovering)
strict warnings clean
M1 swapped texture fields DETECTED
```

M1 is killed by the named assertions `record0 packet clut is GetClut` and
`record0 packet tpage is GetTPage`, rather than by process status alone.

The normal port build completed with `LINK OK` after all temporary diagnostics
and diagnostic CMake flags were removed.

## Native acceptance

The repaired frame 60 is visually coherent: the opaque slabs are gone and the
craft is covered by the intended bright explosion/particle cloud.

```text
broken frame 60 = d6ad8e6825d46f8a2ab0a3898d9b0b6c98d05a62fb6e7fb5f165b7f1c7ca9c12
fixed  frame 60 = 1a00e4fd919127d0572bbe47cb883ba59228cbc325afe29d6c3d004f3a8d3df2
fixed frame 120 = c5a0da0fc6894fba33970fbb6b9c9b911e73781b93c39269bddab0de55041cdb
fixed frame 780 = be727ab195a0996ccefbc0e77cbf2017c3b7c48f8bd347c942d1c96a49453168
```

Artifacts:

- `scratchpad/w34n69_texture_fields_fixed_capture/world-frame-000060.png`
- `scratchpad/w34n69_mode10_teardown_fixed_capture/world-frame-000060.bmp`
- `scratchpad/w34n69_mode10_teardown_fixed_capture/world-frame-000120.bmp`
- `scratchpad/w34n69_mode10_teardown_fixed_capture/world-frame-000780.bmp`

The final normal binary produced distinct captures every 60 frames through
frame 780, reached the natural mode-10 exit at frame 806 with `D7CC=0`, and
returned to `FieldMain`.  No primitive-link or OT-adapter abort was logged.

## Hygiene

- `git diff --check`: clean
- all `XENO_DIAG_W34N69_*` production/vendor diagnostics removed
- CMake diagnostic flags restored to empty
- tracked changes for this rung are limited to the two retail field stores,
  their focused certificate, the runner, and this evidence record
