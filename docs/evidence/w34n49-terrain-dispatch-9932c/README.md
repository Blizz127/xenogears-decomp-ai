# W34N49 — retail terrain dispatcher `0x8009932C`

## Result

`RETAIL_EXACTNESS_RESTORED / NATURAL_NEUTRAL`

Starting HEAD was `1dbba2576c6611497901f63a736005a08ecd268a`, equal to
`origin/experiment/worldmap-open-gates-20260823`.

The existing dispatcher already matched the material retail behavior: table
copies, `CompMatrix(camera, terrain)`, GTE publication, 5x5 cell traversal,
9-wide tile lookup, four quadrant offsets, sparse/all-quadrant selection, and
live compacted packet addressing. A complete instruction-by-instruction review
found one remaining side-effect divergence and no other approximation.

## Retail authority

The authoritative body is `disc/world_map.bin`, decoded as:

```text
mips-linux-gnu-objdump -Dz -b binary -m mips:3000 -EL \
  --adjust-vma=0x8006faf0 \
  --start-address=0x8009932c --stop-address=0x80099708 \
  disc/world_map.bin
```

The bounded body is `[0x8009932C,0x80099708)`, 247 instructions / 988 bytes.
The complete listing used for comparison is
`scratchpad/w34n49_9932c_full.objdump`.

## Proven divergence and repair

Retail reads `D_8009D618[cell]` at `0x8009949C` and branches directly to the
cursor advance at `0x80099694` when the value is `-1`. Consequently, skipped
cells do **not** write the six quadrant-origin halfwords at scratch offsets
`0x330`, `0x334`, `0x338`, `0x33C`, `0x340`, and `0x344`.

The prior C body computed and stored those origins before testing the skip
value. W34N49 moves the six stores inside the active-cell arm while preserving
the unconditional X cursor advance and per-row Z cursor advance. This is an
exact scratch side-effect correction; it does not alter any active terrain
packet.

The review independently reconfirmed:

- 64 shade and seven color halfwords copy to scratch at `+0x288` and `+0x308`;
- all 32 matrix bytes copy from `D_8009D534`, then compose as
  `CompMatrix(D_8009C808, scratch+0x350, scratch+0x370)`;
- start coordinates use the signed shift, low-11-bit mask, negation, and
  `-0x1000` bias found at retail `0x80099430..0x80099464`;
- tile lookup is `(grid_z + C83C) * 9 + grid_x + C838`;
- quadrant sources are `tile_base + {0,0x144,0x288,0x3CC}`;
- if the four `D650` halfwords OR to anything other than `-1`, all quadrants
  run; if the OR is `-1`, only individual entries not equal to `-1` run;
- every call recomputes `packet_base + D7DC * 0x20`, preserving packet
  compaction performed by `wm_80099708`/`wm_8009980C`.

## Certificates

Focused runner: `pc_port/tests/run_w34n49_9932c.sh`

```text
CERTIFICATE O0/O2/UBSan PASS; strict warnings clean
M1 DETECTED; ASSERTION skipped-cells-preserve-origin-scratch
W34N49 0x8009932C EXACTNESS CERTIFICATE PASS; M1 DETECTED
```

The test seeds all 25 cells as skipped, places a sentinel over the complete
origin scratch span, and proves that retail leaves it byte-identical while
still advancing both loop cursors, resetting `D7DC`, and publishing the
composed matrix. M1 restores the pre-W34N49 skipped-cell writes and is killed
by the named scratch-preservation assertion.

The pre-existing full terrain-chain certificate also passes unchanged:

```text
pc_port/tests/run_w34b64_terrain.sh
W34B64 TERRAIN CERTIFICATE PASS; O0/O2/UBSan; strict warnings clean
```

The normal port rebuild completed with `LINK OK`.

## Natural regression

The accepted detached route used the scripted inputs
`0:0x2000,600:0x4000,916:0x2000,976:0` and the world-exit input at frame 601.
It reached the natural state exit with `frames=601 D7CC=0`; upload pumps
reported zero unknowns. The post-repair captures are byte-identical to W34N48:

```text
frame 60  cdc95854e30d798f5634defcdd138ac9f5963afa4de8872981343e77068a46e8
frame 120 d5483c023f2fdecb030c6d335774048d98bc4684e1c524d7a7c3622e175f26bd
frame 600 6e861aecdf5e804bcbf6bb46c002f872308bfe831763ca663e8f2254294c3b1e
```

The unchanged images are the expected result: skipped scratch origins are not
consumed, and all active-cell geometry is unchanged. This rung closes the last
known `wm_8009932C` transcription drift; it does not claim to resolve any
remaining visual roughness.
