# W34C5 — tile producers re-derived (981C8 / 97DC0 / 980D4)

Branch `experiment/worldmap-open-gates-20260823`, base `fb744256`.
Production change under rule 3: no read-only probe can make frame 60 draw
from authored tiles; W34C3 named three mis-ported producers on the frame-60
path, and this rung re-derives exactly those against the banked retail
listings (`scratchpad/w34c3_tile_slots.Hfux1q/`, `w34c5_tile_producers.*`).

## Changes

- `world_map_helper_981c8.c` — retail `[0x800981C8,0x800983A0)`: tile
  coordinates `(pos sra 12, +7 if negative, sra 3)`, window origin relative
  to `(map+2)<<8`, both axes wrapped (negative adds, strictly-greater
  subtracts, `0x8009822C–0x80098274`), previous window saved as 162 bytes
  (`0xA0` + one halfword, `0x80098324–2C`), **9×9** fill with per-row `tz`
  wrap (`0x80098344–50`) and per-column `tx` wrap (`0x80098364–70`).
  Old body: no `sra 12`, 8×8 fill, no `tz` wrap.
- `world_map_helper_97dc0.c` — retail `[0x80097DC0,0x800980D4)`: window
  index `row*9+col` (`s3` steps by 9), ready path = 3×3 centre, flush
  `(0,0,0)+96328`, then all 81, flush; not-ready path = `ArchiveGetFilePath`
  + `962B0(path, tile<<11, 0x710, buf)` for all 81, flush
  `(0,0,0,0)+965A4`. Slot table entries and queue buffers are published as
  **guest** addresses (`PsxMemory_GuestAddr`); the old body stored raw host
  pointers that `9932C` then rebased through `PSX_ADDR`.
  `ArchiveDecodeSector` returns a sector number, so `sector + tile` is the
  tile's sector (one 0x710-byte tile per 2048-byte sector), matching the
  not-ready path's `tile << 11`.
- `world_map_helper_980d4.c` (new; the one-line stub removed from
  `9623c.c`) — retail `[0x800980D4,0x800981C8)`: clears `D558`, wraps
  x/z across `±0x800000` setting bits 4/8 and 1/2, publishes
  `map_x/map_z = (pos sra 23) + 2`. Caller `71A58` step 8 now passes
  `0x8009BBB4` (retail `0x80071AC8–D0`).
- `world_map_helper_98cc0.c` — slot-table domain only: `HeapFree` maps the
  guest slot through `PSX_ADDR`, allocations publish guest addresses. Its
  loop geometry is **not** re-derived (see limitations).
- `psx_memory.h` — `PsxMemory_GuestAddr()` (the conversion `world_map_init.c`
  already used locally).

## Certificate (`run_w34c5_tile_producers_prod_test.sh`)

O0 / O2 / UBSan-nonrecovering, `-Wall -Wextra -Wconversion -Wsign-conversion
-Werror`. Oracles are retail loop geometry written independently in the
test: 981C8 window equality on four asymmetric fixtures (route, negative
position, 24×10 world, high wrap), every entry `< width*height`, 162-byte
previous-window copy, canaries; 97DC0 exactly 81 allocations, every window
tile filled with a guest heap address, no write outside the window or past
`C184[256)`, record order (centre 9 → flush → 72 → flush; path variant 81
→ flush); 980D4 seven wrap cases incl. the exact boundary. Mutants, each
detected: M1 8×8 window (`window_value`), M2 no tz wrap (`window_value`),
M3 `(i+j)` index (`alloc_count`), M4 980D4 stub (`d558_bits`), M5 host
pointer in `C184` (`slot_guest_domain`), M6 centre pass skipped
(`record_count`).

## Natural proof (W34C3 probe2 harness, unchanged, read-only)

- init: 81 `wm_8009623C` records from `a97_fill_sector`, `src = 0x44xxx`
  (sector), `buf = 0x8010…/0x8011…` (guest), then the `(0,0,0)` terminator.
- `slots_nonnull`: 0 → **81 window tiles** (the probe's 112/113 count also
  includes the unrelated globals it reads past `C184[256)`).
- frame 60: **100/100 dispatches on allocated guest slots** (`C184[238]=
  0x80104664` …); `D570` rows now wrap (no tile ≥256).
- W34C4 re-measured: 95/100 sources have nonzero `packed & 0x1000` cells,
  i.e. the tile buffers hold delivered data, consumed via `967E4 → 966CC`.
- `D558=0`, `map=(2,2)`, `98CC0=0` throughout: the position never crosses
  `±0x800000` in 120 frames, so paging is not exercised.
- captures: frame-60 `adacdb46…`, frame-120 `f73c628e…` (previous
  `abf1a963…` / `9413ab2b…`). Frame 60 still shows malformed geometry, now
  with recognisable texture content on the triangles — the inputs are
  authored tiles, the transform is not yet retail (see next).

## Limitations

- "Comparable to retail" is not claimed: no retail frame-60 reference exists
  in this repo; the capture is described, not compared.
- `98CC0` (paging refill) loop geometry vs retail `[0x80098CC0,0x80099214)`
  is unverified; only its address domain was touched. It cannot run on this
  route while `D558` stays 0.
- The not-ready branch of `97DC0` passes `ArchiveGetFilePath`'s pointer
  through `PsxMemory_GuestAddr`; if the path table is host-static this is a
  truncated host pointer, as before. The forced route takes the ready branch.
- Buffer contents were inferred from the branch statistics, not byte-dumped.

## Next (one task)

W34B59b — re-derive `wm_80097244`/`wm_80097440` per
`scratchpad/w34b58_evidence/W34B59_PART1_STOP.md` (camera matrix rot **and**
translation to `0x8009C808`, angles from `0x8009BD38/3A/3C` negated, base
matrix `0x8009A180`), certificate M1–M7, then re-capture frame 60.
