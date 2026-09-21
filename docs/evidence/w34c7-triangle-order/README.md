# W34C7 — terrain triangle A had V1/V2 swapped

Branch `experiment/worldmap-open-gates-20260823`, base `9720386f`.
Production change under rule 3 (winding is a producer property).

## Retail contract (disc/world_map.bin, [0x8009980C,0x80099BFC))

Per 8×8 cell (`0x80099840-44` counters, `a0 += 4`, `t0 += 8`, one extra
step per row), two triangles through `RTPT`/`NCLIP` (`0x80099910/14`,
`0x800999AC`):

- **A** (`0x800998D8-0x8009990C`): `V0 = vertex` (`lwc2 $0/$1`),
  `V2 = vertex+0x48` (`lwc2 $4/$5`), `V1 = vertex+0x50` when packed bit 15
  is set (`0x800998F0`, `bgez t9` on `packed<<16`) else `vertex+8`
  (`0x80099900`). Colours: `+0xC = colors[0]`, `+0x14 = colors[3]` /
  `colors[1]`, `+0x1C = colors[2]` (`0x80099A08-0x80099A38`).
- **B** (`0x80099A44-0x80099A74`): `V0 = vertex+8`, `V1 = vertex+0x50`,
  `V2 = vertex` (bit 15 set) / `vertex+0x48` (clear); colours
  `[1]`, `[3]`, `[0]`/`[2]`.
- Gates, in order: `FLAG` bit 31 (`mfc2 $31`, `bltz`), any x < 320 and any
  y < 216 (unsigned on the 16-bit halves), `max(SZ1,SZ2,SZ3) < 0xF00`,
  `NCLIP > 0`; OT bucket `max >> 4`; IR0 clamp `0xFFF`; tag `0x07000000`.

## Port before this rung

Triangle B, the gates, the colour selection and the packet layout matched.
Triangle A emitted `(vertex, vertex+0x48, vertex+0x50)` / `(vertex,
vertex+0x48, vertex+8)` — V1 and V2 exchanged in both orientations, so
`NormalClip` saw the opposite winding for every first triangle. The
`w34b64` submit oracle asserted that swapped order verbatim
("clear/set orientation triangle one order").

## Change

- `world_map_helper_9980c.c`: triangle A now `(vertex, bit15 ? +0x50 : +8,
  +0x48)`; old shape kept as `WM_9980C_MUTANT_SWAPPED_FIRST`.
- `w34b64_terrain_prod_test.c`: the two triangle-A checks assert retail's
  order ("triangle A vertex order (bit 15 clear/set)"); runner gains the
  mutant stage. Result: PASS O0/O2/UBSan, `HIGH_BYTE` and `SWAPPED_FIRST`
  detected.

## Natural proof

Same substrate and harness as W34C3–C6. Frame-60 SHA-256 `6f51c91b…`
(W34C6 `ec2ea281…`). The capture changes which faces survive but remains a
field of tall spikes.

## What the spikes are (seed for the next rung)

At frame 60 (W34C7 seed dump in `docs/evidence/w34c6-height-byte/`): the
composite equals `0x8009C808`; the camera record `0x8009BD40` is
`pos=(527,-669,-528)`, `target=(0,0,0)`, built by `wm_80096F18`, whose
retail body `[0x80096F18,0x80097070)` the port matches
(`pos = target + R·(0,0,-(height>>12))`, `target = (0, posY>>12, 0)`, `up =
R·(0,-4096,0)` at +0x10). The eye is therefore ~1000 units from the world
origin and 669 units up, while authored tile heights span ±1024 at a
128-unit vertex pitch. The inputs to that record — camera height
`0x8009D3F0`, position `0x8009BE28`, angles `0x8009BD38/3A/3C` — have no
retail-writer provenance line in the field map yet.

## Limitations

- Not compared to retail: no retail frame-60 reference exists here.
- `RotTransPers4`'s `long*` p/flag outputs at scratch `0x58/0x5C` overlap on
  the 64-bit host (8-byte stores); the 73B04 horizon gate reads stale
  pointer bits there in this build (`flag_bits=0x801E34AC`). Not on the
  terrain path; recorded, not fixed.

## Next (one task)

W34C8 (rule 1, read-only): establish the retail producers of `0x8009D3F0`
(camera height), `0x8009BE28` (camera position) and `0x8009BD38..3D`
(angles) by PC, compare the port's writers, and dump their frame-60 values;
verdict on whether the eye placement is retail's or a seeding defect.
