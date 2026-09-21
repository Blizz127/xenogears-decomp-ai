# W34N54 — retail model-bounds culler 0x8003101C

## Result

`RETAIL_CULLER_RESTORED`

Starting HEAD was `fd0839d8c3566aa892a510a380c90612321e0598` on
`experiment/worldmap-open-gates-20260823`, equal to origin. The accepted
world route still reached the generated `func_8003101C` stub from the model
primitive path. That stub discarded the retail arguments and always returned
zero, so model-level visibility culling never occurred.

## Retail anchor

The authoritative sources are:

- `asm/slus_006.64/nonmatchings/system/temp2/func_8002C700.s`, especially
  `0x8002C70C..0x8002C744`;
- `asm/slus_006.64/nonmatchings/system/temp2/func_8003101C.s`, complete retail
  body `0x8003101C..0x8003159C`;
- `src/slus_006.64/system/temp2.c`, the port caller.

The caller preserves its model-header pointer in `a0`, loads
`D_80050104` into `a1`, and calls `func_8003101C(a0, D_80050104)`. The old C
prototype and call had no arguments, which was not the retail contract.

The model header supplies signed AABB corners at offsets `0x20..0x24` and
`0x28..0x2C`. Mode bit 0 projects three retail triplets; mode bit 1 projects
four more. Each triplet uses exact corner or directional-midpoint samples.
Retail computes each midpoint as `base + trunc((target - base) / 2)`, which
can differ by one from `(base + target) / 2` for odd signed inputs.

The retail visibility seam at `func_80030EE8` executes RTPT and accepts a
sample when any of the three vertices has:

- depth satisfying `(u16)(SZ + 1) >= 2`, rejecting both `0` and `0xFFFF`;
- packed SXY below unsigned `D_800500FC`; and
- unsigned X below `D_800500F8`.

`func_8003101C` returns zero on the first visible triplet and one only when
all selected triplets are outside. The port implementation uses
`RotTransPers3` plus the emulated `SZ1..SZ3` registers rather than calling the
MIPS-inline `func_80030EE8` body on the host.

## Production change

- `src/slus_006.64/system/temp2.c` now declares the two-argument contract and
  passes the model header plus `D_80050104`.
- `pc_port/src/world_map_helper_3101c.c` owns the complete host-safe retail
  culler.
- `pc_port/src/world_map_helper_3101c.h` records that public contract.
- `pc_port/build_port.sh` compiles the new owner. The final binary exports a
  real text definition for `func_8003101C`; stub generation no longer owns it.

No renderer, adapter, camera, terrain, capture, or presentation semantics
changed.

## Focused certificate

Runner: `pc_port/tests/run_w34n54_model_bounds_cull.sh`.

The certificate links the real production helper and proves:

- mode zero touches no bounds memory, performs no projection, and returns
  culled;
- the exact three bit-0 and four bit-1 sample triplets;
- retail directional midpoint rounding for odd signed inputs;
- early return on the first visible triplet;
- seven calls when both mode bits are selected and all samples are outside;
- the `SZ=0xFFFF` rejection rule; and
- the bounds record is read-only.

```text
CERTIFICATE O0/O2/UBSan PASS; strict warnings clean
M1 DETECTED; ASSERTION mode1.call_count
M2 DETECTED; ASSERTION retail.midpoint_direction
M3 DETECTED; ASSERTION mode2.call_count
M4 DETECTED; ASSERTION mode0.culls_without_samples
M5 DETECTED; ASSERTION depth-rejects-ffff
W34N54 0x8003101C FULL CERTIFICATE PASS; M1-M5 DETECTED
```

M1 removes the mode-bit-0 group, M2 substitutes symmetric-average rounding,
M3 removes the fourth mode-bit-1 triplet, M4 inverts the final cull result,
and M5 accepts the retail-invalid `SZ=0xFFFF` value.

The normal PC port reports `LINK OK`.

## Natural-route acceptance

Two independent normal-binary runs, followed by a final run after tightening
the no-bounds-read mode-zero edge case and rebuilding, used the accepted
field-to-world schedule. Each detached before `PcPort_WorldMapInitMain` and
naturally reached the world-session exit at frame 601 with `D7CC=0`. All runs
reported zero upload unknowns. The former `[stub] func_8003101C` line is
absent; the only game function stub reached before the session exit remains
field-side `func_80028B14`.

All ten captures in each run are byte-identical to one another across runs
and to the accepted W34N53 orientation-corrected baseline. This is the
expected neutrality result: the restored helper removes only models whose
retail bounds samples are not visible.

```text
frame 60  15fdbff279b0bb5a83d0e049ba6bce0ca3397a94f6239bdea51424a23b430d04
frame 120 50512b12deef85444cc69c92b924fc88021261f46146887e95a553977ce009c5
frame 180 9631bc3446d7b2e9e153d6ead29b60bc4f4c0cf4073c4703060401375731444e
frame 240 c1b0e95730c85e5f3ab08e358ee72bea2b629f2ceb8ad1efeb79dc97625269df
frame 300 d7ace4cd669a7c7379e575c861ae2944ef2078c5da5c725042f4a476e205a0b2
frame 360 bc9db3123f4d56852e5544a6643d6b6497137b9a29e8995fb572182f51573d8e
frame 420 21607a21862f950e2e26b7ee6e9b1a2fd490f5d00740a10cad083f383ccd7834
frame 480 5a3f53beeb6028f7df8a6c944c38c41f7ced8bd7854e41c7e7108a8f2389bb0f
frame 540 4b98b746e5deeabc86490318c640d84845693073302a3dc38775a3a87fec8f1c
frame 600 894f9da798e4cdb01e093834fda64ea49a8730a068e5a3827aaea507a0d15c93
```

Artifacts:

- `scratchpad/w34n54_run_a.log`
- `scratchpad/w34n54_run_b.log`
- `scratchpad/w34n54_capture_a/`
- `scratchpad/w34n54_capture_b/`
- `scratchpad/w34n54_run_final.log`
- `scratchpad/w34n54_capture_final/`
- `scratchpad/w34n54_frame600.png`

The frame-600 image was visually inspected and still shows upright terrain,
the `Mountain Path` label, and the minimap.
