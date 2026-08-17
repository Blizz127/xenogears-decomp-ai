# W34B24-PRE2 — Read-Only Audit of wm_80085418 (Node-Triangle Plane-Straddle Filter)

Mode: READ ONLY. No implementation, no source/build/test changes, no commits.

## Step 1 — Boundary (independently recovered)

| Field | Value |
|---|---|
| target | `0x80085418` |
| start / end | `[0x80085418, 0x80085760)` |
| byte_count | 840 (0x348) |
| instruction_count | 210 (all decode; no embedded data) |
| retail_sha256 | `f389a45dcad7e9df75b9a8cb811c5aa96c596e251baa98de1c5e801bb7a0d98b` |
| file offset | `0x15928` in `disc/world_map.bin` (VA − 0x8006FAF0) |

Edges verified: `jr $ra / nop` tail at `0x80085410/14` (previous function),
epilogue `addiu $sp,$sp,0x38 / jr $ra / nop` at `0x80085754..5C`, fresh
prologue `addiu $sp,$sp,-0x30` at `0x80085760` (= accepted `wm_80085760`).

## Step 2 — ABI

```
signature = s32 wm_80085418(u32 pos_vec, s32 y_offset, u32 attr, u32 node_id)
  $a0 = pos_vec : guest ptr; s32 X/+0, Y/+4, Z/+8 in 20.12 (each read is
        lw then sra 12).  ($s4)
  $a1 = y_offset: PLAIN INTEGER, not a pointer.  Used once, arithmetically
        (`subu` at 0x800856E0) to derive the second probe point's Y.  Both
        retail callers pass the immediate 0x70 (`addiu $a1,$zero,0x70`).
        ($s6)
  $a2 = attr    : u16 (`andi 0xFFFF`), attr-record index; record =
        *(0x8009C620) + 84*attr.
  $a3 = node_id : u16 (`andi 0xFFFF` after move to $s0), node-record index;
        node = rec[0x44] + 8 + 14*node_id.
  $v0 = s32 in {0, -1}:  ((dotA ^ dotB) >> 31) arithmetic — -1 when the two
        probe points lie on opposite sides of the node triangle's plane
        (sign bits differ), 0 when on the same side.  The 95414 caller
        discards a candidate on result == 0.  NOT a boolean {0,1}.
Stack frame = 0x38; saved $s0-$s6 + $ra (0x18..0x34).  sp+0x10 = RotTrans
        flag out (long).
Scratchpad = heavy: 0x00-0x2F (three transformed vertices then in-place
        edges), 0x30 (unit normal), 0x40 (ApplyMatrixLV out), 0x10-0x18
        (scale vector), 0xF0-0x10F (MATRIX copy), 0x110-0x11B (packed
        SVECTOR pair).
COP2 = none directly; GTE entirely via canonical libgte, INCLUDING the
        stateful SetRotMatrix/SetTransMatrix -> RotTrans register flow.
```

## Step 3 — Memory contract / algorithm

1. **Attr record fetch:** `rec = *(0x8009C620) + 84*attr` (same 84-byte
   record table as 95414 case-3; base pointer is a runtime heap value, not
   dumpable from the image).
2. **Matrix setup:** copy 32 bytes `rec+0x20..0x3F` to scratch MATRIX at
   `0x1F8000F0` (8 word loads/stores in retail order); then
   `t[2]=0 (@0x10C)`, `t[0]=0 (@0x104)`, scale vector `(0x800,0x800,0x800)`
   stored order `0x18, 0x14, 0x10`, `t[1]=rec[0xC] (@0x108)`.
   `ScaleMatrix(0xF0, 0x10)` (halves the rotation), `SetRotMatrix(0xF0)`,
   `SetTransMatrix(0xF0)` (loads GTE state).
3. **Node triangle:** node = `rec[0x44]+8+14*id`; vertex indices are the
   THREE s16s at node +0/+2/+4 (the +6/+8/+0xA link fields and +0xC type
   field of the same 14-byte record are read by 95414, not here).  Vertex
   base = `*(rec[0x44]+4)`; each vertex is an 8-byte SVECTOR at
   `base + idx*8`.  Three `RotTrans(vtx, out, sp+0x10)` calls write
   transformed VECTORs to scratch `0x00`, `0x10`, `0x20` (in that order).
4. **Face normal:** in-place edge build `[0x10] -= [0x00]` (x,y,z),
   `[0x20] -= [0x00]` (x,y,z) with the exact interleaved retail load/store
   order; `OuterProduct0(0x20, 0x10, 0x20)`; then each normal component
   `sra 2`; `VectorNormal(0x20, 0x30)` -> unit normal at 0x30.
5. **Probe points (packed s16):**
   `dx = (pos.X>>12) - rec[8] - v0.x` -> sh 0x110 AND 0x116;
   `dy1 = (pos.Y>>12) - v0.y` -> sh 0x112; `dy2 = dy1 - y_offset` -> sh 0x118;
   `dz = rec[0x10] - (pos.Z>>12) - v0.z` -> sh 0x114 AND 0x11A.
   NOTE the Z sign inversion vs X (rec-origin minus position for Z,
   position minus rec-origin for X).
6. **Plane dots:** `ApplyMatrixLV(0x110, 0x30, 0x40)` treats the packed
   SVECTOR pair as MATRIX rows: out[0x40] = dot(row0, n)>>12,
   out[0x44] = dot(row1, n)>>12, out[0x48] = dot of UNINITIALIZED row2
   (scratch 0x11C..) — computed by the GTE but never read.  Do not zero
   row2 in a native implementation; it is simply dead.
7. **Return:** `v0 = ((dotA ^ dotB) >> 31)` (arithmetic sra) -> 0 or -1.

Widths: all vertex/probe stores are `sh` (s16 wrap-truncation of the
computed s32 — faithful truncation required); record loads are `lw`/`lh`;
attr/node ids masked `andi 0xFFFF`; pos components `sra 12`.

## Step 4 — CFG (see CFG.csv)

Straight-line: no branches, no loops, no early returns — a single basic
block from prologue to epilogue (unique among the world helpers audited so
far).  All behavioral variation flows through data.

## Step 5 — Dependencies (see CALL_GRAPH.csv)

9 JAL sites, 7 unique callees — ALL CANONICAL (PsyCross libgte):
ScaleMatrix, SetRotMatrix, SetTransMatrix, RotTrans ×3, OuterProduct0,
VectorNormal, ApplyMatrixLV.  No missing dependencies; no overlay callees.

Callers (full-overlay JAL scan): `0x80095628` (wm_80095414 case-1 filter
loop — MISSING body) and `0x80095EB0` (inside `wm_80095CD4`, the next
function after 95414 — also missing, un-audited).  Both pass
`a1 = 0x70` immediate, `a0 = 0x1F800060`, `a2 = attr (lhu sp[0x18])`,
`a3 = candidate id (lhu)`.

## Step 6 — Classification

**READY_BOUNDED** — 210 straight-line insns, zero missing callees, but
heavy GTE plumbing and runtime-built data structures.

Certification strategy recommendation:
- Recording libgte seams in the certificate (the accepted 85760/95324
  pattern): assert call ORDER (ScaleMatrix -> SetRot -> SetTrans ->
  RotTrans x3 -> OuterProduct0 -> VectorNormal -> ApplyMatrixLV), exact
  pointer identity for every argument, MATRIX contents at ScaleMatrix/
  SetRotMatrix time (copied rec matrix + trans (0, rec[0xC], 0) + scale
  0x800), and RotTrans input addresses (vtxbase + idx*8).
- Controlled RotTrans/VectorNormal/ApplyMatrixLV outputs to drive both
  return signs and the dead-row2 case.
- Synthetic attr record + node + vertex fixtures in emulated RAM at a
  test-chosen `*(0x8009C620)` heap pointer (retail table is runtime-built;
  no image dump possible).
- Oracle classes: s16 wrap in the `sh` probe stores (components exceeding
  16 bits), Z-sign inversion, dy2 = dy1 - y_offset, sra-12 on 20.12
  positions, sra-2 normal downscale, xor-sign return at INT_MIN/0 dots
  (0 ^ negative -> -1; both negative -> 0; dot == 0 -> sign bit 0).

## Hazards (top 5)

1. `$a1` is an integer Y offset (0x70 at both call sites), trivially
   misread as a scratchpad pointer (0x1F800070).
2. Stateful GTE: RotTrans consumes the matrix loaded by SetRotMatrix/
   SetTransMatrix — seams must model the state hand-off; production relies
   on PsyCross.
3. ApplyMatrixLV reads uninitialized scratch (row2 @0x11C) whose result is
   dead — must not be "repaired" (no invented zeroing) and must stay
   unread.
4. Z-axis sign inversion in the probe build (`rec[0x10] - pos.Z - v0.z`)
   vs X (`pos.X - rec[8] - v0.x`).
5. Return is `(dotA ^ dotB) >> 31` in {0, -1}; the caller tests `== 0` —
   booleanizing to {0, 1} is a real divergence class (dot == 0 counts as
   non-negative).
