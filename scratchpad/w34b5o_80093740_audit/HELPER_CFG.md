# Complete CFG for world 0x80093740

There is one entry, no loop, no early return, and one epilogue.

1. **Entry / lookup (`93740-937AC`)**: save `s0-s3/ra`, preserve output/X/Z,
   call `wm_80093660(x,z)`, compute truncation-toward-zero `x/8` and `z/8`,
   retain their low 16 bits, and read `cell[1]&0x80`.
2. **Flag-1 selection (`937B0-937E0`)**: signed `MULT/MFLO` for
   `local_x*word[B254]` and `(-local_z)*word[B25C]`, wrap with `ADDU`, and
   branch on the signed wrapped sum.  The `bgez` delay slot loads `+16`.
3. **Flag-1 negative (`937E4-93828`)**: construct edges for triangle
   `h00/h01/h11`; jump to convergence.  The jump delay slot computes edge1.Y.
4. **Flag-1 nonnegative (`9382C-93870`)**: construct edges for triangle
   `h00/h11/h10`; jump to convergence.  The jump delay slot computes edge1.Y.
5. **Flag-0 selection (`93874-938A8`)**: signed `MULT/MFLO` for
   `(local_x-0x10000)*word[B244]` and `(-local_z)*word[B24C]`, wrap with
   `ADDU`, and branch on the signed wrapped sum.  The delay slot loads `-16`.
6. **Flag-0 negative (`938AC-938E8`)**: construct edges for triangle
   `h10/h00/h01`; jump to the shared final height subtraction.
7. **Flag-0 nonnegative (`938EC-93920`)**: construct edges for triangle
   `h10/h11/h01`; fall through to the shared final height subtraction.
8. **Convergence (`93924-93954`)**: finish edge1.Y, call
   `OuterProduct0(edge1,edge0,scratch+0x20)`, then call
   `VectorNormal(scratch+0x20,out_normal)`.
9. **Epilogue (`93958-93974`)**: restore saved registers/stack and return the
   `v0` value from `VectorNormal`; the `jr ra` delay slot is `nop`.

R3000A load delays are explicit: `lbu cell[1]` and coefficient loads have a
`nop`; paired signed-byte loads are separated from their consuming `subu` by
the second load plus `nop` or by a jump whose delay slot consumes them.
