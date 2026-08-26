# W34C1 Rung 5 — malformed terrain discriminator

## Scope and substrate

This was a read-only publication-to-decode discriminator. No production fix
or pointer-domain sweep was attempted.

- Branch: `experiment/worldmap-open-gates-20260823`
- HEAD under test: `9aeff4c93c9fe5ad24e06df8a8e913b3050a8f0f`
- Input schedule: `0:0x2000,600:0x4000,916:0x2000,976:0`
- Frame limit: 120
- Bootstrap method: `pc_port/tests/w34c1_scripted_input_detach.gdb`
- Detach seam: before `PcPort_WorldMapInitMain`
- Publication seam: accepted path in `wm_9980c_emit_triangle`, after the
  packet and OT writes
- Decode seam: `wm_ot_draw_otag_guest`, after tag/opcode decode and before
  `DrawPrim`

The temporary probes were compiled with
`XENO_DIAG_DEFINES=-DXENO_DIAG_W34C1_RUNG5`. They used exact guest packet
addresses as the join key. The publication probe retained every frame-60
terrain address, printed the first 20, and the adapter probe counted registry
matches across its complete walk. Adapter traversal order is depth/OT order,
so it was deliberately not compared by ordinal.

Artifacts from the single diagnostic run:

- Log: `scratchpad/w34c1_rung5.hza3qH/run.log`
- Frame-60 BMP:
  `scratchpad/w34c1_rung5.hza3qH/capture/world-frame-000060.bmp`
- Frame-60 SHA-256:
  `38c576579443c43004eb2f4a06f730843f8683735312f74c7f7e4d410c80439d`
- Frame-120 BMP:
  `scratchpad/w34c1_rung5.hza3qH/capture/world-frame-000120.bmp`
- Frame-120 SHA-256:
  `850ce0f1882fabaa04b8e002a4203c258461f8d680851ead73587102877d6185`

The frame-60 BMP is a live, fulfilled frame-60 presentation capture and shows
the already-observed dense malformed terrain.

## Population and health

The two totals describe different populations and are not expected to be
equal:

- Terrain packets published by `wm_9980c_emit_triangle`: **392**
- Published terrain addresses found in the adapter walk: **392**
- All `len > 0` packets decoded by the adapter: **398**
- Non-terrain packets in the same walk: **6**
- Unique decoded guest packet addresses: **398**
- Sample join: **20/20**, with zero missing addresses
- Adapter walk completion: **1**
- Frame-local abort delta, range/alignment/length/steps: **0/0/0/0**
- Cumulative abort counters, range/alignment/length/steps: **0/0/0/0**

The 392 terrain addresses were the contiguous 0x20-byte packet allocation
`0x801D34A4..0x801D6584`. Every one appeared exactly once in the complete
adapter walk. The other six decoded packets are the existing non-terrain
packets from other writers; they are not failed terrain joins.

## Address-joined sample

All 20 terrain records use opcode `0x24` (`POLY_FT3`) and length 7. `XY raw`
lists the three packed 32-bit XY words at packet offsets `+0x08`, `+0x10`, and
`+0x18`. Publication and decode values are identical; `Dec #` records the
different OT traversal order.

| Pub # | Packet | OT bucket | Tag | Dec # | XY raw | Signed XY | Field diff |
|---:|---:|---:|---:|---:|---|---|---|
| 1 | `0x801D34A4` | `0x800A35EC` | `0x070A35E8` | 6 | `003200FB/003700F5/003400EE` | `(251,50)/(245,55)/(238,52)` | none |
| 2 | `0x801D34C4` | `0x800A35E0` | `0x070A35DC` | 12 | `00350102/003A00FD/003700F5` | `(258,53)/(253,58)/(245,55)` | none |
| 3 | `0x801D34E4` | `0x800A35E4` | `0x070A35E0` | 10 | `003700F5/003C00F0/003900E9` | `(245,55)/(240,60)/(233,57)` | none |
| 4 | `0x801D3504` | `0x800A35D4` | `0x070A35D0` | 16 | `003A00FD/003F00F8/003C00F0` | `(253,58)/(248,63)/(240,60)` | none |
| 5 | `0x801D3524` | `0x800A35E4` | `0x071D34E4` | 9 | `003900E9/FFF000F2/003C00F0` | `(233,57)/(242,-16)/(240,60)` | none |
| 6 | `0x801D3544` | `0x800A35D4` | `0x071D3504` | 15 | `003C00F0/FFF200FB/003F00F8` | `(240,60)/(251,-14)/(248,63)` | none |
| 7 | `0x801D3564` | `0x800A3500` | `0x070A34FC` | 44 | `FFF600F0/000000E8/FFEF00E1` | `(240,-10)/(232,0)/(225,-17)` | none |
| 8 | `0x801D3584` | `0x800A34FC` | `0x070A34F8` | 48 | `000000E8/FFFA00E3/FFFC00ED` | `(232,0)/(227,-6)/(237,-4)` | none |
| 9 | `0x801D35A4` | `0x800A356C` | `0x070A3568` | 22 | `FFF40134/002E011D/FFF40125` | `(308,-12)/(285,46)/(293,-12)` | none |
| 10 | `0x801D35C4` | `0x800A356C` | `0x071D35A4` | 21 | `FFF60140/00310127/002E011D` | `(320,-10)/(295,49)/(285,46)` | none |
| 11 | `0x801D35E4` | `0x800A356C` | `0x071D35C4` | 20 | `002E011D/FFF8012B/00310127` | `(285,46)/(299,-8)/(295,49)` | none |
| 12 | `0x801D3604` | `0x800A3558` | `0x070A3554` | 24 | `00310127/FFFA0137/FFFA0148` | `(295,49)/(311,-6)/(328,-6)` | none |
| 13 | `0x801D3624` | `0x800A34BC` | `0x070A34B8` | 85 | `FFFE0150/0000014C/FFFE013F` | `(336,-2)/(332,0)/(319,-2)` | none |
| 14 | `0x801D3644` | `0x800A34BC` | `0x071D3624` | 84 | `FFFE013F/0000013A/FFFE012D` | `(319,-2)/(314,0)/(301,-2)` | none |
| 15 | `0x801D3664` | `0x800A34AC` | `0x070A34A8` | 100 | `0000014C/00030147/0000013A` | `(332,0)/(327,3)/(314,0)` | none |
| 16 | `0x801D3684` | `0x800A34BC` | `0x071D3644` | 83 | `FFFE012D/00000128/FFFE011B` | `(301,-2)/(296,0)/(283,-2)` | none |
| 17 | `0x801D36A4` | `0x800A34AC` | `0x071D3664` | 99 | `0000013A/00030135/00000128` | `(314,0)/(309,3)/(296,0)` | none |
| 18 | `0x801D36C4` | `0x800A34AC` | `0x071D36A4` | 98 | `00030147/00050142/0000013A` | `(327,3)/(322,5)/(314,0)` | none |
| 19 | `0x801D36E4` | `0x800A34BC` | `0x071D3684` | 82 | `FFFE011B/00000116/FFFE010A` | `(283,-2)/(278,0)/(266,-2)` | none |
| 20 | `0x801D3704` | `0x800A34AC` | `0x071D36C4` | 97 | `00000128/00030122/00000116` | `(296,0)/(290,3)/(278,0)` | none |

Exact comparison fields were tag, opcode, length, all three raw XY words, and
their signed decodes. There were no differences in any field.

## Coordinate reading

The sampled numbers are bounded signed screen coordinates, not NaN-like or
wildly out of range:

- Combined sample range: `x=225..336`, `y=-17..63`
- Absolute twice-area range: `34..935`
- Degenerate triangles: 0
- Fully inside the 320x216 viewport: 6
- Partially intersecting the viewport: 14
- Wholly outside the viewport: 0

Several triples are individually small and plausible, including packet
`0x801D34A4` at `(251,50)/(245,55)/(238,52)`. The sample as a sequence also
contains abrupt narrow vertical folds at the top edge: packet `0x801D3524`
connects `(233,57)/(242,-16)/(240,60)`, and `0x801D3544` connects
`(240,60)/(251,-14)/(248,63)`, immediately after neighboring triangles only
five pixels high. Thus the data is neither degenerate nor numerically
explosive, but the malformed top-edge shape is already present in the final
packet coordinates before the adapter consumes them.

## Verdict

`IDENTICAL`

The adapter sees the exact guest packet addresses and exact tag/opcode/length/
XY contents published by the terrain submitter. No sampled-field overwrite or
OT-linkage divergence occurs between these two seams. The first proven
divergence is therefore not in the publication-to-decode interval.
Under the Rung 5 routing rule, the malformed geometry is upstream of the
adapter. The next task is a read-only frame-60 connectivity discriminator in
`wm_8009980C`: record the row/column and 9x9 scratch-grid source index for
each of the three vertices of the first folded published triangles, alongside
their raw 8-byte scratch values and final XY. Compare that triangle-strip walk
with retail. A static precheck already rules out an edge overrun: retail fills
9 rows x 9 columns and emits 8 rows x 8 columns, exactly matching the port's
`row < 9`/`column < 9` producer and `row < 8`/`column < 8` emitter. If an
index diverges, the bounded repair target is the vertex walk/stride. If the
indices match but the scratch values differ from their expected grid entries,
the target is the grid producer or its source data. Only matching indices and
matching scratch values with folded XY route to a W34B65-style per-vertex
matrix/projection lineage record.

A second static precheck rules out a wrong diagonal split. Let `00` be
`(r,c)`, `01` `(r,c+1)`, `10` `(r+1,c)`, and `11` `(r+1,c+1)`. Retail and
the port both emit:

- bit 15 clear: `(00,10,01)` then `(01,11,10)`;
- bit 15 set: `(00,10,11)` then `(01,11,00)`.

The existing publication ordinals cannot establish a first-half/second-half
fold pattern: they increment only for triangles that survive all projection,
screen, depth, and `NormalClip` gates. The runtime discriminator must
therefore log each *attempted* cell and split half as well as the vertex
indices, scratch values, and accepted/rejected outcome.

## Rung 5b — vertex/scratch discriminator

This follow-up was read-only and ran from `8479b1e4` with the same detached
120-frame scripted-input substrate. Temporary instrumentation in
`wm_8009980C` recorded all attempted halves at frame 60; it was removed
before the normal rebuild. The complete log and paired capture are retained
under `scratchpad/w34c1_rung5b_allpatch.XQ8wij/`.

- Patches reached: **100**
- Attempts per patch: **128/128** in every case
- Total attempts: **12,800**
- Accepted: **392**
- Rejected at projection / screen / depth / `NormalClip`:
  **8,186 / 3,686 / 157 / 379**
- Index mismatches: **0**
- Scratch-grid X/Z mismatches: **0**
- Frame-60 capture SHA-256:
  `38c576579443c43004eb2f4a06f730843f8683735312f74c7f7e4d410c80439d`

The first selected patch was wholly off-screen, so the final run widened the
same probe across all patches rather than treating that empty patch as a gate
failure. Productive patches 25, 26, and 27 respectively accepted
**64/128**, **62/128**, and **63/128**. Their first 16 attempts resolved the
retail index layouts exactly. For example, patch 27's `(0,2)` halves resolve
to `2/11/3` and `3/12/11`; the latter projects to
`(196,32)/(180,111)/(172,32)` and is accepted, while its neighboring first
half is rejected by `NormalClip` from its already-projected XY. The abrupt
vertical change is therefore present before that gate consumes it.

Raw eight-byte values were captured for every sampled vertex. The defined
X/Z fields form the expected 9x9 grid in every patch; the high halfword of
the second word is untouched vertex padding and was not treated as terrain
data. The sampled height fields contain bounded, source-dependent elevations
rather than an uninitialized grid or a failed row/column write.

### Rung 5b verdict

`ALL_CLEAN`

The emitter resolves the retail index pattern for all 12,800 attempts, and
the scratch grid has no X/Z discontinuity. The gates receive the same folded
XY that appears in accepted packets; they do not mutate geometry or create a
publication/decode discrepancy. The large whole-frame rejection total spans
off-screen and distant patches, while productive patches retain visible
triangles. No gate implementation divergence is proven by this run.

The next bounded investigation is a W34B65-style frame-60 per-vertex
matrix/projection lineage record for the terrain producer chain, specifically
the first vertex whose projected Y makes a neighboring triangle fold.

### Noted, not investigated: distant-patch projection rejects

The all-patch count includes **8,186** negative-`FLAG` projection rejections,
but productive patches 25, 26, and 27 each have **zero** such rejections.
A targeted read-only follow-up consequently produced no negative-`FLAG`
samples in those visible patches: their folds are valid projected geometry.
The distant-patch rate remains an anomaly for a later patch-selection/range
audit; it is not evidence that the visible fold is caused by GTE projection
failure.

## Rung 5d — paired vertex lineage

This read-only follow-up ran from `eddc6227` at frame 60, patch 27, cell
`(0,2)`, second half. Its vertices are grid indices `3/12/11` and project to
`(196,32)/(180,111)/(172,32)`. The shared GTE state was:

`R=[2896,0,2896;1931,3052,-1932;-2158,2732,2157]`,
`T=[0,0,1001]`, `OFX=160`, `OFY=140`, `H=256`.

| Grid index | Scratch raw | Signed GTE input `(X,height,Z)` | Pre-shift MAC `(1,2,3)` | Shifted `(1,2,3)` | SZ | XY |
|---:|---|---|---|---|---:|---|
| 3 | `0000FD80/00000400` | `(-640,0,1024)` | `(1112064,-3214208,7689984)` | `(271,-785,1877)` | 1877 | `(196,32)` |
| 12 | `0280FD80/00000380` | `(-640,640,896)` | `(741376,-1013632,9162368)` | `(181,-248,2236)` | 2236 | `(180,111)` |
| 11 | `0000FD00/00000380` | `(-768,0,896)` | `(370688,-3214080,7690112)` | `(90,-785,1877)` | 1877 | `(172,32)` |

The GTE input-register words match these three signed vectors; their raw
16-bit values for negative X values were `0xFD80` and `0xFD00`.
The pre-shift reconstruction applies translation as `TR << 12`, matching the
GTE MAC equation; the earlier table incorrectly used the unscaled `TRZ`.

### Rung 5d verdict

`HEIGHT_INPUT`

Index 12 alone enters the GTE with a `+640` scratch height, while its two
neighbors have height zero. That input changes the transformed Z from 876 to
1236, the SZ from 1877 to 2236, and screen Y from 32 to 111. The matrix,
translation, geometry offset, and projection distance are shared across the
three vertices; no matrix multiply or divide divergence is present.

The next bounded task is read-only provenance for the height at grid index
12: identify the packed/source element and the exact retail height formula
used by `wm_80099708`, then compare its address, stride, signed byte, and
conditional sine contribution against the port.

## Rung 5e — terrain height field census

This read-only frame-60 run from `163d6876` dumped the complete raw 9x9 grid
for patch 27 at entry to `wm_8009980C`, after `wm_80099708` populated it and
before its first triangle emission. The raw 81-cell artifact is
`scratchpad/w34c1_rung5e.WyPCtp/grid.txt`; each entry is the requested pair
of un-decoded `u32` words.

- Height-field cells: **77 zero**, **4 nonzero**
- Nonzero low-byte frequencies: `0x80: 2`, `0x38: 1`, `0xC0: 1`
- Nonzero high-byte frequencies: `0x02: 2`, `0x00: 1`, `0xFF: 1`
- Layout: adjacent pair at row 1, columns 3–4 (`0x0280`); isolated entries
  at row 4, column 8 (`0x0038`) and row 5, column 0 (`0xFFC0`).

### Rung 5e verdict

`MIXED`

The field is mostly zero and includes two isolated entries, but it also has a
two-cell `0x0280` cluster. Its nonzero bytes do not have one shared
low-byte/high-byte pattern that proves a packed-field width or signedness
error, nor do they establish a smooth authored height field. The census
therefore does not justify either a producer repair or retiring fault #2.

Rung 5d decoded this halfword as signed `s16` without proving that retail does
so. Positive values in the current decode render downward on screen; both the
field width and signedness remain unproven pending the retail consumption seam.

## Diagnostic cleanup

After the verdict:

- all `XENO_DIAG_W34C1_RUNG5` production instrumentation was removed;
- `git diff --check` passed;
- the three temporarily instrumented production files had zero tracked diff;
- a normal `./pc_port/build_port.sh` completed with `LINK OK`;
- no Rung 4 or Rung 6 work was started.
