# W34C3 — tile slot population (read-only) + W34C4 branch rate

Branch `experiment/worldmap-open-gates-20260823`, substrate `e8256ece`
(W34C2 landed). Read-only: gdb probes only, no production change, no diag
build. Artifacts: `scratchpad/w34c3_tile_slots.Hfux1q/` (probe1/probe2
logs, both harnesses, retail listings, capture hashes).

## Question

Why does the patch-27 tile source resolve to a NULL heap slot, and which
producer should have filled it before `wm_8009932C` reads it?

## Field map (retail provenance)

| field | addr | established by |
|---|---|---|
| tile slot table `C184` | `[0x8009C184,0x8009C584)`, 256 × u32 (one per world tile) | `97BC0` zero-fill loop `0x80097C1C–0x80097C34` (`v0=0x8009C580`, 256 iterations, step −4); indexed `C184 + tile*4` at `0x800994EC–F8` (9932C), `0x80097E50–54` (97DC0) |
| tile index window `D570` | `0x8009D570`, 81 × s16, 9×9 | fill loop `0x80098330–0x8009839C` (981C8): value `tz*width + tx`, `tx`/`tz` wrapped to 0 at `width`/`height` (`0x80098344–50`, `0x80098364–70`) |
| world width / height | `0x8009D160` / `0x8009D2B4` (s32) | init stores `0x80071CBC` / `0x80071CB4`; consumed as `t2`/`t4` in 981C8 (`0x800981D4`, `0x800981DC`) |
| paging gate `D558` | `0x8009D558` (u16 bits 1/2/4/8) | written only by `0x800980D4` (`0x800980E8`, `0x80098124`, `0x80098154`, `0x8009818C`) |
| `map_x` / `map_z` | `0x8009C838` / `0x8009C83C` (s16) | `0x800981A4` / `0x800981BC` (in `0x800980D4`: `(pos>>23)+2`), init `0x80097C78/88` |
| window lookup | `tile_slot = (grid_z+map_z)*9 + (grid_x+map_x)` → `D570[tile_slot]` → `C184[tile]` | `0x800994AC–0x800994F8` |
| quadrant dispatch | `tile_base + 0 / 0x144 / 0x288 / 0x3CC` → `wm_80099708` | `0x80099580–0x8009968C` |
| retail producer (init) | `0x80097DC0`: for all 81 window entries, `if C184[tile]==0` → `HeapAlloc(0x710)`, queue copy | `0x80097E34–0x80097EA0` (3×3 centre, `s3` from `0x1B` step 9), `0x80097EBC–0x80097F38` (9×9, `s3` step 9) |
| retail producer (paging) | `0x80098CC0`, gated by `D558 != 0` in 71A58 step 9 | `0x80071A58` listing; 98CC0 `0x80098D40` (`0x51` window loop) |

All values below are stated as producer output or source data explicitly.

## Observed (probe2, frame 1 / 60 / 120)

- `width=16 height=16` → 256 world tiles (source data from init).
- `D570` (producer output of port 981C8): `204 205 206 207 192 193 194 195
  220 …` — rows `tz = 12..19`, i.e. tile numbers **192..319**. Values ≥256
  cannot exist in retail (rows wrap at 16).
- `C184` non-null entries: exactly 11 written by port 97DC0 at init
  (`[192..195]`, `[204..207]`, `[220..222]`, host heap pointers
  `0x0064a78c…`), plus entries ≥256 that are not table entries at all:
  `[256]=0x00000E00` is `0x8009C584` (`WM_STATE_VALUE`), `[262]=0x801AB42C`
  is `0x8009C59C`, etc. `D808=6` after init (six queued copies), `D558=0`
  and `map_x=map_z=2` at every frame.
- Frame 60, all 100 `wm_8009932C` dispatches (25 cells × 4 quadrants,
  whole unit logged): `tile_index ∈ {284..286, 300..304, 316..319}` — rows
  17–19. **80 dispatches read `C184[300..318] == 0`** (out-of-table zero
  memory) and **20 read out-of-table garbage** (`0x001E807F`, `0x00780137`,
  `0x7F908000`, `0x06000000` at indices 284–286, 319). Not one dispatch
  reaches an allocated slot; the 11 allocated tiles (rows 12–13) lie outside
  the drawn window (rows 14–18 of `map_z+grid_z`).
- Counts over 120 frames: `97DC0=1` (init), `981C8=1` (init), `98CC0=0`,
  `96130=0` (step 9 never taken: `D558` stays 0).
- Frame-60/120 capture hashes identical to W34C2's (`abf1a963…`,
  `9413ab2b…`): the probe is non-invasive.

## Verdicts (what was observed)

1. `NO_DRAWN_TILE_HAS_A_SLOT` — every frame-60 dispatch indexes `C184`
   with a tile number ≥284; none is allocated, 80/100 are out-of-table
   reads of zero, 20/100 are out-of-table reads of unrelated globals.
   Rule 6 is satisfied for the whole drawn window, not just patch 27.
2. `D570_OUT_OF_DOMAIN` — the port's `wm_800981C8` emits tile numbers up to
   319 for a 256-tile world. Retail wraps `tz` per row (`0x80098344–50`);
   the port body wraps only `tx` (`world_map_helper_981c8.c`, fill loop).
3. `97DC0_WINDOW_MISDECODED` — the port's `wm_80097DC0` indexes
   `D570[(i + j)]` (i,j ∈ 3..5; then i<3, j<6); retail uses `s3 + s1` with
   `s3` = `9·row` (`0x80097E3C`, `0x80097EA0`, `0x80097ED4`, `0x80097F38`),
   covering the 3×3 centre then all 81 window tiles. Port allocates 11 of
   81 at init.
4. `980D4_IS_A_STUB` — port `wm_800980D4()` writes `D808=0` and returns;
   retail `0x800980D4` is the position wrap/paging function that clears
   `D558`, sets bits when `|x|`/`|z|` cross `0x800000`, adjusts the
   position, and publishes `map_x/map_z`. With it stubbed, step-9 paging
   (`98CC0`) can never run and `map_x/map_z` never move.
5. `C184_IS_256_ENTRIES` — the port comments' "0x51 entries" is wrong;
   the table is 256 × u32 ending at `0x8009C584`. Port code that writes
   `C184[tile]` for tile ≥256 (97DC0 with the current D570) corrupts the
   globals from `WM_STATE_VALUE` upward. Probe2 shows the current 11
   writes stayed <256; this is a latent hazard, not an observed one.

## W34C4 — the 5/81 branch rate

Per-source `packed & 0x1000` rate at frame 60 (probe1): NULL-base
quadrants `+0/+0x144/+0x288`: 41.8 / 38.3 / 39.5 per 81 (reading the
scratchpad alias `g_PsxRam+0..0x510`); `+0x3CC`: 5.0 per 81; garbage bases:
0–79 per 81. The rate tracks whatever memory the bad pointer lands on. The
5/81 figure is a property of the scratchpad alias, not of authored terrain;
no branch input at frame 60 comes from a populated tile.

## Which producer should have filled the slot

At init, retail `0x80097DC0` fills **all 81 window tiles** (each
`HeapAlloc(0x710)` + queued copy from the decoded archive sector at
`decoded + tile`); on paging, `0x80098CC0` refills after `0x800980D4` raises
`D558`. Both port bodies diverge from retail in loop geometry (97DC0) or are
stubbed (980D4); and even a correct 97DC0 would be fed out-of-domain tile
numbers by the port's 981C8. Three producers, one index domain.

## Limitations

- The archive-copy path (`wm_8009623C` queue → `wm_80096328`) was not
  verified to deliver bytes into the 11 allocated buffers; only the pointer
  publication was observed.
- `98CC0`'s phase-3/4 loop geometry against retail (`0x80098DBC–0x80099214`)
  was not decoded beyond the 0x51 phase-1 loop; it is listed for the fix
  rung, not verified here.
- Retail `97DC0`'s not-ready branch (`ArchiveGetFilePath` + `0x800962B0`)
  has no port counterpart; whether the forced route takes it was not
  measured (`ready` was true: 97DC0 did allocate).

## Next (one task)

W34C5 — re-derive `wm_800981C8` (tz wrap), `wm_80097DC0` (9-stride window
loops, both branches) and `wm_800980D4` (paging) against the retail
listings banked here, with a certificate whose oracle is the retail loop
geometry (tile numbers < width·height; 81 allocations at init; D558/map
publication), then re-run this probe: expected `slots_nonnull ≥ 81`, all
frame-60 dispatches on allocated slots, `D570 max < 256`.
