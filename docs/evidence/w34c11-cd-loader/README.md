# W34C11 — CD record loader, record sort, and queue wiring

Branch `experiment/worldmap-open-gates-20260823`, base `6918329e`.
Production change under rule 3 (no probe can put disc bytes into the tile
buffers). Artifacts: `scratchpad/w34c11_cd_loader.*/` (captures, probe log,
`tile60.bin`, retail listing, harnesses, certificate log).

## Retail contract (disc/world_map.bin)

- `0x800967E4`: ready `(r1 == 0) | (~r2 == 0)`. Ready: pre-check
  `0x800968E0` (busy → return), else `D788[lw 0x8009BCB8]` → CD loader
  `0x8009699C` (`0x80096834–58`). Not ready: `C624[lw 0x8009BCB8]` → PC-file
  loader `0x800966CC`, clear entry, `BCB8 = (BCB8+1) & 0xF`
  (`0x8009686C–0x800968C0`).
- `0x8009699C` + sync callback `0x80096A6C` + ready callback `0x80096C0C`:
  record list of 12-byte `{sector, size, buf}` (next `+0xC`, end at
  `sector == 0`, `0x80096D50–0x80096D9C`); per record `sectors =
  (size + 0x7FF) >> 11` (`0x80096A04–08`), `CdlSetloc` then `CdlReadS`;
  per sector: `size < 0x800` → `size/4` words into `buf`, remainder into the
  buffer pointed to by `0x8009D7D4` (`0x80096C78–0x80096CC8`), else `0x200`
  words, `buf += 0x800`, `size -= 0x800` (`0x80096CD4–0x80096CF8`);
  completion via the pre-check: `D788[BCB8] = 0`, `BCB8++ & 0xF`, state
  `0x8009CD44 = 0` (`0x80096958–0x80096988`).
- `0x8009623C`: queue a record at `BE08 + BE44*1056 + D808*12`, `D808++`
  (cap 0x58). `0x80096328`: publish block `BE44` to `D788[BE44]` only if its
  first record is non-empty and the slot is free, after sorting it
  (`0x80096358–0x800963B4`); else `D808 = 0`, return −1.
- `0x800963E4`: **bubble sort of the block by sector** (swap while
  `next.sector < cur.sector`, `0x8009640C–0x80096490`).
- `0x80095F78` allocates the record block (`0x4200`) and the `0x800` spill
  buffer and stores their addresses at `BE08`/`D3C0` and `D7D4`.

## Port before this rung

`wm_800967E4` had the consumers swapped (ready → `966CC`, not ready →
`9699C`) and indexed `D788` with `BE44`; `wm_8009699C` was a stub; the
record base `BE08`/`D3C0` and spill pointer `D7D4` were stored as raw host
pointers; `wm_800963E4` overwrote the block with `0x09000000` and zeros
("record initializer") — no retail counterpart; `wm_80096328` published
empty blocks. Net effect (W34C10): no tile buffer was ever written.

## Change

- `world_map_helper_966cc.c`: `wm_8009699C(list)` transcribed synchronously
  on PsyCross (`CdIntToPos` + `CdControl(CdlSetloc)` seek the image;
  `CdRead`/`CdReadSync` deliver whole sectors; the partial-sector and spill
  rules applied per retail; completion side-effects per the pre-check).
  Retail's asynchronous state machine is not reproduced — same bytes, same
  globals, no callback timing.
- `world_map_helper_96130.c`: `wm_800967E4` rewired to retail (index `BCB8`,
  busy check on `0x8009CD44`).
- `world_map_helper_9623c.c`: `wm_800963E4` = the retail sort.
- `world_map_helper_96328.c`: publish rule per retail.
- `world_map_init.c`: `BE08`/`D3C0`/`D7D4` published as guest addresses.

## Certificate (`run_w34c11_cd_loader_prod_test.sh`)

O0/O2/UBSan PASS against a simulated disc (per-sector byte generator):
partial record (0x710) exact bytes and no overrun, spill remainder at
`*D7D4`, two-sector and sector+partial records, completion (`D788` cleared,
`BCB8` advanced, state idle, other entries intact), ready/not-ready wiring,
queue count, sort order with payload, terminator kept, block canaries,
empty block not published. Mutants detected: M1 swapped consumers, M2
`BE44` index, M3 partial copies 0x800, M4 no round-up, M5 no completion,
M6 old `963E4` fill, M7 no sort, M8 descending sort, M9 publishes empty.

## Natural proof

- Tile 60's delivered buffer equals `disc/disc1.bin` sector `0x44110`
  (raw 2352-byte sectors, data at +24) for all 0x710 bytes (`FULL MATCH`).
- Frame 60: 81 slots, 100/100 dispatches on allocated slots; the `packed &
  0x1000` branch is now taken in 0 cells (64/100 sources have non-zero
  words) — authored data, not the alias pattern.
- Captures: frame 60 `26e59e42…`, frame 120 `ae06268a…`. Frame 60 shows a
  continuous, textured green ground plane under the horizon band; frame 120
  a full-view terrain mosaic. No spikes, no shards.

## Limitations

- "Comparable to retail" is described, not measured: there is no retail
  frame-60 reference in the repo. The view is dark and low-contrast; whether
  lighting/shade tables are retail's is unexamined.
- The synchronous loader ignores retail's per-frame streaming cadence
  (`0x13`-sector proximity re-Setloc, pause/resume states); anything timed
  on the CD machine's states (`0x8009CD44` values 2–12) sees only 1 → 0.
- Only tile 60 was byte-verified against the disc.
- The not-ready (PC-file) path was exercised only through stubs.

## Next (one task)

W34C12 (read-only): produce a retail frame-60 reference for this route
(emulator capture on the same entrance/input schedule) and compare against
`26e59e42…` structurally (horizon line, tile grid alignment, shade), then
list any input of `wm_8009980C` still lacking a provenance line.
