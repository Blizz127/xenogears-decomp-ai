# Field menu return: party sprites lose their VRAM pages (2026-09-06)

Evidence index for the user report "when I open the menu and close it, the
character disappears".

## Defect

`func_800799D4` (src/field/main/misc4.c) blits six 0x40 x 0x20 VRAM rects from
`D_800ADCB0` to `D_800ADCC8` before `MenuMain`, and back afterwards, because the
menu reuses the party sprites' texture pages.

Both tables are retail field-overlay `.data`
(`asm/field/data/3DF78.data.s`, RAM 0x800ADCB0 / 0x800ADCC8; `disc/field.bin`
file offsets 0x3E1C0 / 0x3E1D8, field overlay base 0x8006FAF0). They sat in the
0x30-byte gap between the already-migrated `D_800ADC44` and
`g_FieldData_800ADCE0` in `pc_port/src/data_field.c`, so the port's stub
generator emitted them as zeroed BSS. Every save and every restore therefore
degenerated to `MoveImage({0, 0, 64, 32}, 0, 0)` — a no-op self-copy of the
top-left of VRAM. Nothing was saved and nothing was restored.

Retail values (little-endian `{u16 x, u16 y}`):

    D_800ADCB0 (field pages)   (0,224) (64,224) (128,224) (192,224) (256,224) (256,480)
    D_800ADCC8 (backup column) (704,0) (704,32) (704,64) (704,96)  (704,128) (704,160)

The field pages are the two 32-row scratch strips below each 320x224 display
buffer. A sprite whose frames are pre-uploaded (the `frameHeader & 0x8000` path
of `func_8001DAE8`) reads its texels/CLUT from there once and never re-uploads,
so after a menu it draws its quads against 0x0000 texels — fully transparent on
PSX — and the character is simply not there. Sprites on the plain inline path of
`func_8001DAE8` re-upload every frame via `func_800251C8` and self-heal, which
is why the symptom is map- and character-dependent.

## Fix

`pc_port/src/data_field.c`: migrate both tables from retail `.data`.
No game-logic change; `src/field/main/misc4.c` is untouched apart from
env-gated diagnostics.

## Measurements

Two binaries built from the same tree, differing only in whether the two tables
are migrated (`prefix` = stub-zeroed, i.e. the shipped defect; `fixed` =
migrated). Same env, headless, deterministic (two runs of the same binary
produce byte-identical frames).

Repro command (Map 5, Alice's house):

    XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 XENO_FIELD_MAP=5 XENO_FIELD_ENTRANCE=0 \
    XENO_MENU_FORCE=1 XENO_MENU_FORCE_DELAY=250 \
    XENO_FIELD_CAPTURE_DIR=<dir> XENO_FIELD_CAPTURE_EVERY=15 \
    SDL_VIDEODRIVER=offscreen ./pc_port/build_native/xeno-port

Captured `field-frame-*.png` files are BMP despite the extension.

### VRAM probe (`XENO_MENU_PARTY_DIAG=1`)

`vram-probe-map0-{prefix,fixed}.log`, `vram-map1-{prefix,fixed}.log`.
The probe digests the six retail party pages from hard-coded coordinates, so it
measures the same VRAM in both builds regardless of the table contents.

| point        | prefix                                  | fixed                          |
|--------------|-----------------------------------------|--------------------------------|
| pre-backup   | pages populated (75/60/31/0/149/75 nz)  | identical                      |
| pre-restore  | pages 0-4 wiped to 0, page 5 clobbered  | identical                      |
| post-restore | still wiped (fnv d2063dc5 = all zero)   | every page byte-exact restored |

So the menu really does clobber the party pages, and only the fixed build
brings them back.

### Frames

`map5-ab-montage.png` (top row = prefix, bottom = fixed; left = pre-menu frame
240, right = post-menu frame 600):

- Pre-menu: both builds identical — 5 characters plus the yellow wall banner.
- Post-menu, prefix: the banner and the green-clothed character on the right
  are **gone**.
- Post-menu, fixed: both are still there.

1.744% of the frame differs between the two post-menu frames; the difference is
confined to one 92x264 box at x=246 covering exactly the banner and that
character. Non-black coverage: prefix post-menu 49.717%, fixed post-menu
49.720%, both pre-menu 49.642%.

Map 0 (`map0-*.png`) is the same story measured differently: the fixed build's
post-menu frame stays close to its own pre-menu frame (AE 4.28e7) while the
prefix build's diverges (AE 2.13e8). The dark/garbled character colours visible
in `map0-pre-menu.png` are a **separate pre-existing issue** — they are already
there before any menu is opened, in both builds.

### Map 1 player-render census (`map1-player-render-census.log`)

`XENO_PLAYER_RENDER_DIAG=1`. On Map 1 the player sprite emits quads
continuously (frameCount 4..9) across 5160 draws / 5580 frames with exactly one
`NO-QUADS` event, at startup. That is why Map 1 does not show the symptom: the
player there is on the per-frame dynamic upload path. It also refuted the
initial hypothesis — `g_PartyDataBuffers[]` and every actor's `pSpriteData` /
`pActorData` are byte-identical across the free/realloc (`party-diag` lines),
so this was never a host-pointer lifetime bug.

## Regression test

`pc_port/tests/run_field_menu_party_vram_retail_test.sh`
(`pc_port/tests/field_menu_party_vram_retail_test.c`)

- Oracle is `disc/field.bin` itself, pinned by sha256 of the 0x30 bytes at
  0x3E1C0 (`8bd7b18aa22af76fe507ac3d499b40b21ff2609d854cd09a9befa28e325c9eac`).
- 3 regimes: -O0, -O2, -O1+UBSan+ASan.
- 5 controls, each required to BUILD and then FAIL at runtime: `zeroed`
  (the exact pre-fix state), `swapped` (x/y byte-order error), `selfbackup`
  (backup rects on top of the field pages), `stride` (0x10 backup spacing),
  `rect_h` (correct tables, wrong blit height — passes the disc oracle and must
  still be caught by the blit replay).
