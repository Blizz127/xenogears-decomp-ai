# Fei HD-2D field experiment

Approved pixel-art direction: Octopath Traveler 0 inspired proportions and color
clusters, retaining Fei's ponytail, cream shirt, teal sash/trousers and shoes.
Created with the built-in image generation tool on 2026-09-19.

`fei.png` is the unmodified generated 1254 x 1254 RGBA atlas. `fei.rgba` is the
same pixels in tightly packed 8-bit RGBA order, converted without resampling:

    magick fei.png -depth 8 rgba:fei.rgba

The runtime reads alpha bounds (alpha >= 128) inside each of eight equal grid
cells: that box is the cell's *art box*. Layout:

| Row | Column 0 | Column 1 | Column 2 | Column 3 |
| --- | --- | --- | --- | --- |
| Top | Front idle | Front step | Back idle | Back step |
| Bottom | Left idle | Left step | Right idle | Right step |

Generation prompt: Production game sprite atlas for Xenogears Fei Fong Wong in
an Octopath Traveler 0 inspired HD-2D pixel character aesthetic. Genuine
transparent alpha background, no checkerboard, backdrop, grid or labels. Eight
full-body sprites in a uniform four-column, two-row grid. Same scale and
baseline, compact 3.5-head proportions. Long brown ponytail, cream martial arts
shirt with navy trim, teal sash and baggy trousers, brown shoes. Crisp visible
pixel clusters, warm highlights and cool shadows, no smooth painting or vector
curves. Front idle/walk, back idle/walk, left idle/walk, right idle/walk. No ground
shadow or glow, no weapons.

## Controls and scope

The toolbar's **FEI HD2D ON/OFF** button changes presentation immediately.
Enabled by default as requested; `XENO_FEI_HD2D=0` starts with retail sprites.
The toolbar choice lasts for the current process. Asset override:
`XENO_FEI_HD2D_ASSET=/absolute/path/to/fei.rgba` (same dimensions/layout).
Launch from the repository root, as with the port's normal disc paths.

This is an eight-cell field idle/walk/run prototype, not a complete HD-2D
remaster. Combat, other party members, special animation IDs and nonstandard
sprite transforms retain their retail artwork. There are four directions and
two poses per direction; diagonals use the nearest cardinal view. The authored
shading is baked into the art. Existing sprite tint, projected height, floor
shadow and ordering-table depth are retained; there is no new dynamic lighting,
depth-of-field, or environment art pass.

Only presentation packets change. The retail sprite animation interpreter,
collision, movement, field scripts and combat state continue unchanged. Missing
or invalid assets, insufficient packet space, off-screen or degenerate parts, or
a frame without an opaque texel retain the original primitives.

## Footprint rule and mapping

A replacement draws with exactly the retail sprite's footprint. Every retail
part (the POLY_FT4 func_8001E3D8 builds, one per sprite tile, possibly at
different OT depths) is replaced in place: the same four screen vertices
(position, size, feet anchor), the same OT slot and link order, wrapped in its
own bind / part / reset packets so the texture never leaks to scenery that shares
the slot. More detail comes only from texel density inside those quads.

Mapping, per drawn frame:

1. *Retail silhouette box*: for each part, the opaque texels of its UV rect are
   read from the CPU VRAM mirror through its tpage and CLUT (colour 0 is
   transparent) and mapped through the part's quad to screen space; the union
   over the parts is the retail frame's silhouette box, so part padding
   (transparent tile margins) does not count.
2. The cell's art box is mapped affinely onto that silhouette box. The cell
   therefore takes the retail frame's aspect, with its padding aligned so that
   head row, feet row and side edges coincide with the retail silhouette.
3. Each part vertex gets the atlas texel under its own screen position. A part's
   UV rect in the retail frame's space thus lands on the proportional sub-rect of
   the art box, and neighbouring parts share their seam texels exactly.
4. Each cell is uploaded as its own texture: the art box plus transparent padding
   of half its size on every side, so part margins outside the silhouette sample
   transparency rather than a neighbour cell. Alpha is made binary at 128, like
   retail's 1-bit transparency, so the drawn silhouette is the aligned art box.

Horizontal extents inside a part follow the generated art's own outline (it is
not traced from the retail sprite). Rows, the feet anchor and occlusion by scenery
follow retail exactly. `pc_port/tests/run_fei_hd2d_footprint_test.sh` measures
all of this in-game, with Fei behind the easel in his house.
