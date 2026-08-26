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

## Diagnostic cleanup

After the verdict:

- all `XENO_DIAG_W34C1_RUNG5` production instrumentation was removed;
- `git diff --check` passed;
- the three temporarily instrumented production files had zero tracked diff;
- a normal `./pc_port/build_port.sh` completed with `LINK OK`;
- no Rung 4 or Rung 6 work was started.
