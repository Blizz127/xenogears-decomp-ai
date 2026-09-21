# W34C10 — the tile bytes are never loaded (read-only)

Branch `experiment/worldmap-open-gates-20260823`, base `92bf4991`. gdb
dump + disc-image comparison + retail disassembly only. Artifacts in
`scratchpad/w34c9_entry_placement.*/` (`tile60.bin`, `w34c10.gdb`).

## Observed

- Frame 60, first dispatched tile (index 60, slot `0x80104664`): the
  delivered 0x710-byte buffer has 1394 non-zero bytes, head
  `b0bb1b02 70bbde01 80448800 68464406 …`.
- Its queue record: `src = sector 0x440D0 + 60 = 0x4410C` (records are
  `sector + tile`, base from `ArchiveDecodeSector(lw 0x8009BCD8)`; the first
  record `src=0x44121` is the 3×3 centre tile 77).
- `disc/disc1.bin` (raw 2352-byte sectors, form-1 data at +24; sector 16 is
  the PVD): the buffer's first 64 bytes occur in none of sectors
  `0x4410C±6`, and no sector equals the buffer. The bytes reaching
  `wm_80099708` are **not the tile's sector** — they are whatever the heap
  block held when `HeapAlloc` returned it (source data, never written by any
  producer on this route).

## Retail record consumers (rule 1)

- `0x800967E4`: `ready = (r1 == 0) | (~r2 == 0)` from two `func_8002C3D8`
  calls. **Ready** (`0x8009681C–0x80096860`): `0x800968E0` pre-check, then
  `a0 = 0x8009D788[lw 0x8009BCB8]`, non-zero → `0x8009699C(a0)` — the **CD**
  loader. **Not ready** (`0x80096868–0x800968C0`):
  `a0 = 0x8009C624[lw 0x8009BCB8]` → `0x800966CC(a0)` — the **PC-file**
  loader — then clears the entry and advances `BCB8` mod 16.
- `0x8009699C` `[0x8009699C,0x80096A6C)` 52 insns: takes the 12-byte record
  list `{sector, size, buf}` (`0x800969B4`, `0x800969D8–E0`), stores list
  state at `0x8009D3BC/…`, sectors = `(size + 0x7FF) >> 11` (`0x80096A04–08`),
  `CdIntToPos(sector, 0x8009CEBC)`, `CdSyncCallback(0x80096A6C)`,
  `CdControlF(CdlSetloc=2, pos)`. Its sync callback `[0x80096A6C,0x80096C0C)`
  104 insns drives a 12-state machine (jump table at `0x80070CB8`) with
  `CdControlF`/`CdReadyCallback`/`CdSyncCallback`, i.e. the actual reads
  into `buf` and the walk to the next record (`+0xC`).
- `0x800966CC` `[0x800966CC,0x800967E4)`: 16-byte records
  `{path, offset, size, buf}` via `PCopen/PClseek/PCread/PCclose` — the
  `962B0` records of the not-ready path.
- `0x80096130` drains the same way: ready → `D788` loop (`Vsync` +
  `967E4`), not ready → `C624` loop.

## Port

`world_map_helper_96130.c`: `wm_800967E4` has the two consumers **swapped**
(ready → `D788` → `wm_800966CC`; not ready → `C624` → `wm_8009699C`), and
`wm_8009699C` in `world_map_helper_966cc.c` is a stub ("Full implementation
requires CD subsystem integration"). On this route `ready` is true, so the
12-byte sector records reach the PC-file loader, which treats `sector` as a
path pointer; `PCopen` fails (8 retries), the record is skipped, and the tile
buffer is never written. `wm_800968E0` is also declared with a different
signature (`(addr, count)`) than retail's 47-insn `[0x800968E0,0x8009699C)`
pre-check; not examined further.

## Verdict

`TILE_BYTES_UNLOADED`: every terrain vertex at frame 60 is computed from
uninitialised heap; the spikes have no authored source. The producers
that should fill the buffers are retail's `0x8009699C` + `0x80096A6C`
(+ `0x800968E0`) behind a correctly wired `0x800967E4`.

## Limitations

- Only tile 60 was byte-compared; the other 80 buffers were inferred from
  the same (unrun) load path.
- The disc image's sector base for the world archive was taken from the
  record arithmetic, not independently from the ISO directory.
- Whether the port's `ready` value (`func_8002C3D8`) matches retail's
  on real hardware was not examined; retail on a console takes the CD path.

## Next (one task)

W34C11 — re-derive `wm_800967E4` (branch wiring per `0x80096814–0x800968C0`)
and transcribe the CD loader `0x8009699C` with its callback machine
`0x80096A6C` and pre-check `0x800968E0` onto PsyCross's `CdControlF /
CdReadyCallback / CdSyncCallback`, with a certificate whose oracle is the
record contract (sector, size, buf, next) and a proof that tile 60's buffer
equals `disc1.bin` sector `0x4410C` bytes; then re-capture frame 60.
