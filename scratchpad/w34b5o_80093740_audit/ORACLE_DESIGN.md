# Independent oracle design for world 0x80093740

The oracle is designed before the production translation and does not call
`wm_80093740`, `wm_80093660`, `OuterProduct0`, or `VectorNormal`.

It treats the selected terrain record as four signed height samples laid out
on a 2x2 patch: `h00=cell[0]`, `h10=cell[4]`, `h01=cell[0x24]`, and
`h11=cell[0x28]`.  The byte at `cell[1]` selects one of two diagonals.  The
two coefficient pairs select one triangle on that diagonal using independent
per-product low-32 reduction and a signed interpretation of the wrapped sum.

For each of the four resulting leaves, the oracle names three geometric
vertices, subtracts the common base vertex to form two edges, and computes
`edge1 x edge0` with explicit 32-bit reduction.  This is structurally
different from the production register/scratchpad translation.

`VectorNormal` is an already accepted external dependency.  To avoid sharing
that production helper, every golden contains literal expected normalized
components and the literal squared-length return obtained from the accepted
dependency.  The oracle independently checks the selection sum, triangle,
edges, and cross product before those literals are used as final authority.

The focused suite covers both diagonal flags, both signed-sum outcomes,
both zero boundaries (which take the nonnegative branch), negative coordinate
rounding, signed height extrema, `MULT/MFLO` high-word loss, an exact
`0x80000000` sign transition, and modulo-sum wrap.  A defined swapped-winding
mutant computes `edge0 x edge1`; the flat golden changes from `(0,-4096,0)` to
`(0,4096,0)` and is rejected by output comparison.
