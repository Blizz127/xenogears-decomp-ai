# W34C9 — fresh-entry placement (0x80073448) transcribed

Branch `experiment/worldmap-open-gates-20260823`, base `80661112`.
Production change under rule 3: the entry sequence never seeded the runtime
position; no probe can supply it.

## Retail contract

Entry sequence `0x800723D4–0x80072434` (between the port's W8B and W10A):
`if (lhu 0x8006EE6A) 0x80073398; else if (lw 0x8009C894) { 0x8007565C;
0x80075D4C } else 0x80073448(lw 0x8009D3D4)`. On the forced route at entry
`C894 = 0`, `EE6A = 0`, `EE68 & 0x2000 = 0`, `D3D4 = 1`.

`0x80073448` `[0x80073448,0x80073530)`, 58 insns: flag branch (`0x80073460`)
clears bit 0x2000 of `0x8006EE68` (stored in the jal delay slot
`0x80073478`), calls `0x8008DFF4(0x8009C5AC)` and copies `0x8006EE66` to
`0x8009C584`; table branch walks rows `{s16 x, s16 id, s16 z, s16 pad}`
(stride 8, `0x800734F4`) from `lw(0x8009D3F4)` until `id == -1`
(`0x800734DC`) or `id == a0` (`0x800734EC`, sign-extended), then
`C5B0 = 0` (`0x8007349C`), `C5AC = x << 12` (`0x800734A0–A8`), `C5B4 = z << 12`
(`0x800734B4–BC`); no match → all three zero (`0x80073508–1C`).
Provenance of the inputs: `0x8009D3D4` ← `0x80070FD8` (port
`WM_ARG2_STATE_ABS`), `0x8009D3F4` ← `0x80073638` (port `WM_FIX_D3F4`).
Table at `0x800B66EC` on this route: one row `(29947, 1, 11075)` then `-1`.

## Change

- `world_map_helper_73448.c/.h` (new), registered in `build_port.sh`.
- `world_map_init.c`: `wm_entry_placement()` runs after W8B and before W10A
  with the retail selection; the `0x80073398` and `0x8007565C/0x80075D4C`
  branches are logged as not transcribed, never faked.

## Certificate (`run_w34c9_entry_placement_prod_test.sh`)

O0/O2/UBSan PASS. Rows with negative x/z and a negative id, a row after the
terminator that must not be reached, no-match zeros, flag branch (bit
cleared, `wm_8008DFF4(0x8009C5AC)` once, `EE66 → C584`, table untouched),
canaries. Mutants detected: M1 stride 6, M2 no `<<12`, M3 x/z swapped,
M4 unsigned id compare, M5 flag not cleared.

## Natural proof

`[worldmap-entry-placement] 0x80073448 index=1 -> C5AC=0x074fb000
C5B0=0 C5B4=0x02b43000`. Tile window now `26 27 28 29 30 31 16 17 18 / 42 …`
(world rows 1–8, wrapped columns); 81 slots allocated; 100/100 frame-60
dispatches on allocated slots (`0x800…/0x801…/0x802…`); 95/100 sources with
nonzero branch cells. Frame-60 SHA-256 `6daf6cb0…` (W34C7 `6f51c91b…`).
The capture is still a field of tall spikes.

## Limitations / observation for the next rung

The delivered tile words at the drawn tiles (`0x021bbbb0 0x01debb70
0x00884480 …`, `0x113333ae 0x11aee222 …`) have low bytes that alternate
`-80, +112, -128, +104 …` between adjacent vertices; as heights (×8) they
produce the spikes. Whether those bytes are the tile's authored height row
or the wrong sector's bytes (the copy path `9623C record → 967E4 → 966CC`
was never verified against retail) is the next question; this rung does
not decide it.

## Next (one task)

W34C10 (read-only): for one dispatched tile, take its queue record
(`src = sector + tile`) and the delivered 0x710 bytes, and compare them to
the bytes at that sector in `disc/disc1.bin`; establish retail's record
semantics in `0x800966CC` by PC.
