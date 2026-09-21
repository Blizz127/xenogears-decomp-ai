# Fei HD-2D field experiment

Approved pixel-art direction: Octopath Traveler 0 inspired proportions and color
clusters, retaining Fei's ponytail, cream shirt, teal sash/trousers and shoes.
Created with the built-in image generation tool on 2026-09-19.

`fei.png` is the unmodified generated 1254 x 1254 RGBA atlas. `fei.rgba` is the
same pixels in tightly packed 8-bit RGBA order, converted without resampling:

    magick fei.png -depth 8 rgba:fei.rgba

The runtime reads alpha bounds inside each of eight equal grid cells. Layout:

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
or invalid assets, insufficient packet space, or mixed-depth sprite parts retain
the original primitives. The renderer inserts bind / RGBA quad / reset packets,
so the texture override does not leak to subsequent scenery.
