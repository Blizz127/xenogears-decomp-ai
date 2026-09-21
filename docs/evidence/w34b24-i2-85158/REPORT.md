# W34B24-I2 — wm_80085158 Implementation Report

## Lane

| Field | Value |
|---|---|
| START_CANONICAL | `7575e26e49a1c2980e7d17bb935a458700ef6cbf` (fetched; local == origin) |
| branch | `candidate/w34b24-i2-85158`, isolated worktree |
| base | `7575e26` |

## Identity (independently recovered)

| Field | Value |
|---|---|
| Function | `wm_80085158` |
| VA range | `[0x80085158, 0x80085418)` |
| Size | 704 bytes / 176 instructions (all decode; clean jr-ra / prologue edges) |
| SHA-256 (slice) | `1be50c96bf66f3eb63e8c54f49ccedf8cdfaec408d5e7e029503519303d0b4aa` |
| File offset | `0x15668` in `disc/world_map.bin` (VA − 0x8006FAF0) |
| Source | `pc_port/src/world_map_helper_85158.{c,h}` |
| Test | `pc_port/tests/w34b24_i2_85158_prod_test.c` + `run_w34b24_i2_85158.sh` |

## Retail contract

```
u32 wm_80085158(u32 pos_vec, u32 coef_out, u32 normal_out, u32 attr, u32 node_id)
  $a0 pos_vec    : s32 20.12 vector (X +0, Z +8); caller passes 0x1F800060
  $a1 coef_out   : caller 0x1F800070; receives
                     +0 = (pos.X sra 12) - rec[8]        (SUBU wrap)
                     +8 = rec[0x10] - (pos.Z sra 12)     (ASYMMETRIC)
                     +4 = plane height (written by wm_800935DC)
  $a2 normal_out : caller 0x1F800080; receives the unit plane normal
  $a3 attr       : andi 0xFFFF; region record = *(0x8009C620) + 84*attr
  0x10($sp)      : node id, lhu (ZERO-extended halfword)
  $v0            : wm_800935DC result (the solved height); both retail
                   callers (0x80095414 sites 0x956A0 / 0x957EC) overwrite
                   $v0 immediately
Frame 0x38; saves $s0-$s5, $ra.  No direct COP2 in the body; GTE via
canonical libgte only.
```

Sequence (transcribed exactly):
1. coef stores (+0 then +8, wrap-faithful, asymmetric direction).
2. 8-word MATRIX copy `rec[0x20..0x3F] -> 0x1F8000F0`, then trans
   overwrites: `t[2]=0`, `t[0]=0`, scale vector `(0x800,0x800,0x800)`
   stored at `0x1F800010` in order +8/+4/+0, `t[1]=rec[0xC]`.
3. `ScaleMatrix(0xF0, 0x10)` -> `SetRotMatrix(0xF0)` ->
   `SetTransMatrix(0xF0)` (stateful GTE hand-off order preserved).
4. Node triangle: `nodes=rec[0x44]`, `node=nodes+8+14*node_id`,
   `verts=*(nodes+4)`; three `RotTrans(verts + 8*lh(node+2i))` with
   SIGN-extended s16 vertex indices, outputs to scratch 0x00/0x10/0x20
   (vtx1 output overwrites the consumed scale vector).
5. In-place edge vectors with the exact retail load/store interleave:
   `0x10 -= 0x00` and `0x20 -= 0x00` per component.
6. `OuterProduct0(0x20, 0x10, 0x20)` — output ALIASES input 0
   (retail GTE reads before writing; PsyCross must preserve this).
7. `sra 2` on the cross components.
8. `VectorNormal(0x20, normal_out)`.
9. `return wm_800935DC(coef_out, 0x1F800000 /* transformed V0 */,
   normal_out)` — writes coef_out[+4], the height the caller compares
   against `pos.y>>12` (±11) and stores as `out[4] = height<<12` in
   slot 0.

## Certification

- Retail gates: full-file SHA + 704-byte slice SHA re-verified per run.
- Focused oracle at `-O0`, `-O2`, `-O2 -fsanitize=undefined
  -fno-sanitize-recover=all`; normalized stdout byte-identical; empty
  stderr.
- Independent oracle: recording test-local libgte (accepted 85760
  pattern) with forced RotTrans/cross/normal values; asserts GTE call
  order (Scale->SetRot->SetTrans->OP0->VN), exact PSX_ADDR pointer
  identities, scale-vector and trans-slot snapshots at call time,
  s16-sign vertex addressing (negative index case), lhu node-id
  (0x8001 case), attr masking (0x30003 case), edge values as u32 wrap,
  sra-2 discrimination, and the final height against an INDEPENDENT
  s64-truncation reimplementation of the accepted 935DC contract
  (real `wm_800935DC` linked into the test).
- Mutants: 12/12 KILLED (see MUTANTS.csv) — stride, mask, asymmetry,
  shift, both signedness classes, trans-slot, scale value, call order,
  OP0 argument swap, missing sra, 935DC argument swap.
- Suites: 17/17 PASS on the candidate tree (16 accepted + this rung;
  `run_w34b18c` required a real file copy of world_map.bin because its
  fixture check uses `stat -c%s`, which does not follow symlinks —
  worktree environmental, not a regression).
- Build: clean + incremental `./pc_port/build_port.sh` both `LINK OK`;
  exactly one strong `T wm_80085158`; no stub shadow; stubs 241/524
  unchanged; game TUs compiled=47 skipped=0.

## Hazards recorded for acceptance

1. `OuterProduct0` output aliasing (out == first input) relies on
   PsyCross reading both inputs before writing — worth re-checking in
   acceptance.
2. Vertex index is `lh` (s16) but node id is `lhu` (u16) — opposite
   signedness on adjacent fields.
3. The scale vector at 0x1F800010 is consumed by ScaleMatrix and then
   overwritten by RotTrans output — ordering is load-bearing.
4. coef X/Z directions are asymmetric (pos−rec vs rec−pos).
5. Return value is 935DC's height; callers ignore it, but it is kept
   for ABI faithfulness.
