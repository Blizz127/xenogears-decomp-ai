# W34B24-I3 — wm_80085418 Implementation Report

## Lane

| Field | Value |
|---|---|
| START_CANONICAL | `7575e26e49a1c2980e7d17bb935a458700ef6cbf` (fetched; local == origin) |
| branch | `candidate/w34b24-i3-85418` (isolated worktree, based on canonical) |
| audit basis | `docs/evidence/w34b24-pre2-85418/` (W34B24-PRE2), key claims re-verified against the binary in this lane |

## Identity (re-verified)

| Field | Value |
|---|---|
| boundary | `[0x80085418, 0x80085760)` |
| size | 840 bytes / 210 instructions (all decode; straight-line, no branches) |
| slice sha256 | `f389a45dcad7e9df75b9a8cb811c5aa96c596e251baa98de1c5e801bb7a0d98b` |
| file offset | `0x15928` in `disc/world_map.bin` |

Source: `pc_port/src/world_map_helper_85418.{c,h}`
Test: `pc_port/tests/w34b24_i3_85418_prod_test.c` + `run_w34b24_i3_85418.sh`

## Contract (from the PRE2 audit, independently re-derived here)

```
s32 wm_80085418(u32 pos_vec, s32 y_offset, u32 attr, u32 node_id)
```
- `pos_vec`: guest (X,Y,Z) in 20.12; each component `lw` + `sra 12`.
- `y_offset`: PLAIN INTEGER (retail callers pass immediate 0x70); used once
  (`subu`) for the second probe row's Y.
- `attr`, `node_id`: masked `andi 0xFFFF`.
- Returns `((dotA ^ dotB) >> 31)` in {0, -1} — -1 when the two probe points
  straddle the node triangle's plane; dot == 0 counts as non-negative.
  NOT booleanized.

Pipeline (single basic block): rec = `*(0x8009C620) + 84*attr`; copy
rec+0x20..0x3F to scratch MATRIX 0x1F8000F0 (retail triple order); t = (0,
rec[0xC], 0) (store order t[2], t[0], scale vz/vy/vx = 0x800, then t[1]);
`ScaleMatrix` -> `SetRotMatrix` -> `SetTransMatrix`; three `RotTrans` over the
node's vertex triangle (s16 indices at node +0/+2/+4, node =
rec[0x44]+8+14*id, vertices 8-byte SVECTORs at `*(rec[0x44]+4) + idx*8`) into
scratch 0x00/0x10/0x20; in-place edge build (exact retail interleave);
`OuterProduct0(0x20, 0x10, 0x20)`; per-component `sra 2`;
`VectorNormal(0x20, 0x30)`; packed s16 probe rows at 0x110 (dx stored to
+0 AND +6; dy1 +2; dy2 = dy1 - y_offset +8; dz +4 AND +0xA; Z sign-inverted:
`dz = rec[0x10] - pos.Z - v0.z` vs `dx = pos.X - rec[8] - v0.x`); row2
(0x11C..) is uninitialized scratch whose GTE dot is dead — never written or
read by the port (asserted); `ApplyMatrixLV(0x110, 0x30, 0x40)`; xor-sign
return.

All subtraction u32 wrap; `sh` stores truncate the full s32 to s16 (dy2 is
derived from the un-truncated dy1); `sra` helpers used for 12/2/31 shifts.

## Certification

- `run_w34b24_i3_85418.sh`: retail full-file + slice SHA gates; focused
  oracle at `-O0`, `-O2`, `-O2 -fsanitize=undefined
  -fno-sanitize-recover=all`; normalized stdout byte-identical; empty stderr.
- Recording test-local libgte (accepted 85760 pattern) models the
  SetRotMatrix/SetTransMatrix -> RotTrans hand-off at the contract level:
  asserts exact call order (Scale -> SetRot -> SetTrans -> RotTrans x3 ->
  OP0 -> VN -> AMLV), pointer identity for every argument, matrix content at
  ScaleMatrix/SetRot/SetTrans time (copied rec matrix + t=(0,rec[0xC],0) +
  scale 0x800^3), RotTrans vertex addresses (including a NEGATIVE s16 index
  -2 -> base-16), edge contents at OP0, `sra 2` at VN input, hand-derived
  probe rows (incl. s16 wrap and dy2 sign-crossing) at AMLV, and the
  full return-sign matrix (+,+ / -,- / +,- / -,+ / 0,± / INT_MIN / 0,0).
- attr-mask case (`attr | 0x10000` behaves as attr).
- The dead row2 is asserted never-stored by production.
- Mutants: **13/13 KILLED**, distinct semantic assertions (MUTANTS.csv).
- Suites: 17/17 accepted suites pass in this worktree (the lone initial
  failure was worktree fixture plumbing — `stat -c%s` on a symlinked
  `world_map.bin`; fixed by copying the real file; not a code issue).
- Build: clean `LINK OK` + incremental `LINK OK`; exactly one strong
  `T wm_80085418`; no stub shadow; stubs 241/524 unchanged (the symbol was
  previously absent — its only callers 95414/95CD4 have no bodies yet).

## Runtime payoff

None yet by design: both retail callers (`wm_80095414` @0x80095628,
`wm_80095CD4` @0x80095EB0) are unimplemented. This closes another
READY_BOUNDED rung of the 95414 ladder (remaining: 85158 — in flight in a
separate lane — then 84DB8/84D00, then 95414 itself).
