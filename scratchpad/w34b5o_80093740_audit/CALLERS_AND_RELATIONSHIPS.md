# Calls, prototype, fixed point, and terrain relationships

## Direct calls made by 0x80093740

| Call PC | Target | Count | Classification |
|---|---:|---:|---|
| `0x80093768` | `0x80093660` | 1 | accepted PC-port implementation, real `T wm_80093660` |
| `0x80093944` | `0x8004A4D8` `OuterProduct0` | 1 | main-executable PsyQ/GTE compatibility body, real `T` |
| `0x80093950` | `0x80048D7C` `VectorNormal` | 1 | main-executable PsyQ/GTE compatibility body, real `T` |

There are zero `jalr` sites and zero indirect calls.  No dependency is a
generated stub.  Implementability is **B: implementable with already-accepted
dependencies**.

## Exact prototype

`s32 wm_80093740(u32 out_normal_addr, s32 x, s32 z)`

- `a0`: 32-bit guest address of a 12-byte output VECTOR.  Both main RAM and
  physical scratchpad are observed at callers.
- `a1`: signed 32-bit X coordinate.
- `a2`: signed 32-bit Z coordinate.
- `a3` and stack arguments: unused.
- `v0`: value returned by `VectorNormal` (squared length of its unnormalized
  input in the accepted native dependency).
- `v1`: no output contract. Both callers ignore `v0/v1`.

## Complete caller scan

Mechanical scan of every aligned JAL word in `world_map.bin` found two calls:

1. `0x800749AC`: `a0=0x1F800030`, `a1=scratch[0x70]`,
   `a2=scratch[0x78]`.  Those coordinates originate as signed halfwords at
   offsets `0` and `4` of a world record, shifted left 12.  The normal at
   scratch `+0x30` is consumed at `0x800749B8` by the next vector-product step.
   The return value is ignored.
2. `0x80093A1C` inside unresolved `0x80093978`: `a0=sp+0x30`, `a1=x`,
   `a2=z`.  The output record is passed as `a2` to `wm_800935DC` at
   `0x80093A2C`; `wm_800935DC` consumes its `N[0..2]` and writes only the Y
   word of the query point.  The return from `wm_80093740` is ignored.

## Relationship to the accepted cell helper

The helper calls `wm_80093660` with exactly its incoming X/Z and consumes the
returned guest pointer.  It reads the current record's flag/height and the
three neighboring height samples at `+4`, `+0x24`, and `+0x28`.  It neither
changes the record nor changes the terrain table/stride used transitively by
`wm_80093660`.

## Fixed-point authority

Callers prove that at least one natural source creates X/Z as signed halfword
coordinates shifted left 12; the higher helper also treats its X/Z in the same
signed scale.  This helper computes truncation-toward-zero X/8 and Z/8 and
retains low 16 bits solely for triangle selection.  With Q12 caller inputs,
that selection coordinate is Q9 and wraps at the cell span.

The constructed edge vectors are not assumed Q12: their horizontal components
are literal `0` or `+/-16`, and their vertical components are raw signed-byte
height differences. `OuterProduct0` uses `sf=0` (no post-multiply shift).
`VectorNormal` produces a 4096-scale normalized VECTOR, which is the only
proven Q12-scale output.  No fixed-point scaling is added inside this helper.

There is no DIV/DIVU or BREAK sequence in 0x80093740.  The only shifts are the
two signed SRA-by-3 operations after the negative `+7` adjustment; together
they are exactly signed truncation-toward-zero division by eight.  There is no
left shift in this function.
