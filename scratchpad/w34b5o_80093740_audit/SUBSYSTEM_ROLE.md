# Proven subsystem role and dataflow

The helper constructs the normalized surface normal of the selected triangle
in a terrain height-cell patch.

Evidence:

- `wm_80093660(x,z)` returns the current 4-byte grid-cell address.
- Signed bytes at `cell+0`, `+4`, `+0x24`, and `+0x28` are the four adjacent
  height samples. `cell+1 bit 7` selects a diagonal family.
- Four CFG leaves construct two 3D edge vectors whose horizontal components
  are fixed at `0` or `+/-16` and whose vertical components are differences of
  those height samples.
- Retail calls `OuterProduct0(edge1,edge0,cross)` and then
  `VectorNormal(cross,out)`.  The cross-product Y component is always `-256`,
  so the fixed horizontal geometry is nondegenerate even for a flat height
  patch; a zero cross product is unreachable for valid cell data.

The direct caller at `0x80093A1C` passes the resulting 12-byte normal record to
`wm_800935DC` as its `N` record.  The other caller at `0x800749AC` consumes the
normal through subsequent vector products while constructing an orientation
basis.

This helper does not solve a plane, transform terrain vertices, or return a
height.  It selects a triangle, builds its two edges, crosses them, and
normalizes the result.
