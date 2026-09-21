# W34N58 — shared mode draw callbacks

## Scope and anchors

- Starting HEAD: `6ec4271bb163cf4cf4cba321c89b5782bdfb0e9c`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail source: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Retail boundary: `[0x80078948, 0x80078A60)`
- Implemented callbacks: `wm_80078948` and `wm_80078950`

The registration census found this pair in the slot table for modes 9, 10,
12, 14, 15, 16, and 18.  It is therefore the smallest shared callback seam
remaining below the W34N57 mode-8/mode-11 lifecycle.

## Retail behavior restored

`0x80078948` is the two-instruction initializer which returns one without
reading or writing its slot.

`0x80078950` performs, in retail order:

1. camera matrix selection (`0x80097440` for `D_8009D144 == 0`, otherwise
   `0x80097244`), with `0x8009BD40` as the input record;
2. particle, scaled-object, region-model, and position-wrap passes;
3. the tile-window/drain/refill chain only when `D_8009D558 != 0`;
4. visibility and terrain submission using `0x8009BE28`, while the earlier
   scaled-object/wrap passes use `0x8009BBB4`;
5. context arguments loaded through `*(0x8009BE3C) + {0x70,0x74}`;
6. the `D_8009C5BC += 0x40` palette/index advance; and
7. terrain, sky, then tiled-object publication.

All direct callees were already port-owned.  The scheduler now resolves both
guest addresses through explicit weak-symbol dispatch; no raw guest address is
called as a host function pointer.

## Focused certificate

`pc_port/tests/run_w34n58_shared_mode_draw.sh`:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- scheduler cb0/cb1 guest-address resolution: PASS
- direct call order, branch behavior, address arguments, write set: PASS

Named mutants:

- M1 swapped camera branch: detected by `no_paging.camera_branch`
- M2 missing position-wrap pass: detected by `no_paging.position_wrap`
- M3 missing conditional paging chain: detected by `paging.chain_present`
- M4 wrong terrain-position record: detected by `paging.visibility_position`
- M5 wrong palette step: detected by `no_paging.palette_step`

## Regression and build gates

- W34B68 second-scheduler certificate: PASS
- W34C1 cadence certificate (M1–M21): PASS
- W34N56 mode-8/mode-11 callback certificate: PASS
- W34N57 mode-8/mode-11 lifecycle certificate: PASS
- canonical `./pc_port/build_port.sh`: LINK OK, port-owned addresses verified

## Runtime bound

The currently accepted base and mode-8/mode-11 routes do not register this
pair, so W34N58 is intentionally runtime-neutral there.  Natural visual
acceptance belongs to the first lifecycle that registers it (mode 9 is the
smallest remaining candidate).  W34N58 removes the shared draw-pair stub; it
does not claim any of those seven modes complete by itself.
