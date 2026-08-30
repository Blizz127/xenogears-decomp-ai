# W34N45 — retail visibility-mask producer `0x800983A0/0x800987AC`

## Result

`RESTORED_VERIFIED`

Starting HEAD was `45c442699b5aa18fbd373435f85ecadf53b9bdd6` on
`experiment/worldmap-open-gates-20260823`, equal to origin.  This rung
replaced the fabricated bodies of both naturally coupled functions:

- `wm_800983A0`, retail `[0x800983A0,0x800987AC)`;
- `wm_800987AC`, retail `[0x800987AC,0x80098CC0)`.

The change is naturally reached once per recurring world frame from the base
R4 callback at retail call site `0x80071B14`.

## Retail authority

- raw overlay: `disc/world_map.bin`;
- symbol/call inventory: `scratchpad/world-callgraph-v2/FUNCTIONS.csv` and
  `scratchpad/world-callgraph-v2/ENCODED_JAL.csv`;
- decode command:

```text
mips-linux-gnu-objdump -D -b binary -m mips:3000 -EL \
  --adjust-vma=0x8006faf0 \
  --start-address=0x800983a0 --stop-address=0x80098cc0 \
  disc/world_map.bin
```

The fresh listings used during the transcription are scratch-only
`scratchpad/w34n45_983a0.objdump` and
`scratchpad/w34n45_987ac.objdump`.

## Corrected behavior

The old `983A0` body copied/composed the matrix and then walked 64 unrelated
four-byte entries as though they were objects.  Retail instead:

1. copies the 32-byte terrain matrix at `0x8009D534` to scratch `+0xF0`;
2. composes camera `0x8009C808 * scratch+0xF0 -> scratch+0x110` and publishes
   that matrix to the GTE;
3. builds 25 `0x800 x 0x800` cells in a 5x5 grid around the input X/Z;
4. writes each tri-state coarse result to the 25 halfwords at `0x8009D618`;
5. subdivides only intersecting (`0`) cells into four `0x400` quadrants and
   writes the packed results to 25 eight-byte records at `0x8009D650`;
6. selects one of four 200-byte boundary tables at `0x8009B7A8` from the
   original input's wrapped X/Z quadrant and ORs it into the fine masks.

The old `987AC` body was an unrelated arithmetic approximation and did not
return retail's result.  Retail transforms the four supplied SVECTOR corners
through the live GTE matrix, stores MAC1/MAC2/MAC3 in scratch records
`0x00/0x10/0x20/0x30`, and tests them against four plane pairs sourced from:

```text
XZ0 = 0x8009C828 / 0x8009C830
XZ1 = 0x8009C844 / 0x8009C84C
YZ0 = 0x8009C878 / 0x8009C87C
YZ1 = 0x8009C7F4 / 0x8009C7F8
```

Corners are visited in retail perimeter order `0,1,3,2`.  The return is `-1`
when all four corners are outside any one plane, `0` when any adjacent pair
is outside without a whole-plane rejection, and `1` when no adjacent pair is
outside any plane.

## Focused certificate

Runner: `pc_port/tests/run_w34n45_visibility_mask.sh`

```text
CERTIFICATE O0/O2/UBSan PASS; strict warnings clean
M1  matrix source                     DETECTED
M2  grid cell extent/step             DETECTED
M3  omitted conditional subdivision  DETECTED
M4  wrong quadrant table              DETECTED
M5  replace instead of OR             DETECTED
M6  omitted fourth transform          DETECTED
M7  outside tri-state changed         DETECTED
M8  wrong corner perimeter order      DETECTED
M9  X used for Y/Z planes             DETECTED
M10 inside tri-state changed          DETECTED
W34N45 VISIBILITY MASK CERTIFICATE PASS; M1-M10 DETECTED
```

The certificate also proves the exact matrix call addresses, all 25 coarse
writes, the first cell/subcell scratch topology, the 29-call scripted path
(one intersecting cell plus 24 non-intersecting cells), and packed fine-mask
layout.

## Natural regression

Method: accepted detached route, frame limit 1200, scripted input
`0:0x2000,600:0x4000,916:0x2000,976:0` plus the accepted natural state-exit
schedule.  GDB detached before `PcPort_WorldMapInitMain`.

```text
natural state exit: frames=601, D7CC=0
capture 60:  request_frame=60, fulfillment_frame=60
capture 120: request_frame=120, fulfillment_frame=120
capture 600: request_frame=600, fulfillment_frame=600
adapter abort/boundary output: none
upload unknowns: 0
```

Capture hashes exactly reproduce the W34N43/W34N44 accepted baseline:

```text
frame 60  4310197d3881367bc7859ef174dcf4b9afa8759f0b7dc2ca3eea94b0f84f1521
frame 120 8039c2f96b88e09491eaf1cba049d0efd829f146aa943da744f56c93e85b1c16
frame 600 2b636ae21af4bf0003f60d5e53f8ffd9a042fc5a72457e4c096cff19814e1a38
```

The detached process reached the already-known post-world
`SoundHandleError` stub after the natural state exit and was terminated only
after that marker.  This rung does not claim whole-process `rc=0`.

## Scope

Only the coupled mask producer/classifier and its focused test/evidence are
changed.  No camera, terrain packet, paging, OT, renderer, or harness
semantics were modified.
