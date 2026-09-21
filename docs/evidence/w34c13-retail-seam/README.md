# W34C13 — retail world-map seam establishment (read-only)

Branch `experiment/worldmap-open-gates-20260823`, base `612de9c4`. No
production change, no port rebuild, no comparison.

## Harness identity

- Emulator: PCSX-Redux `build293-3e10093a` AppImage,
  sha256 `b27a564e6c32333453433c950ac26e652f40af32d5e236441f36e8123c9c651b`
  (`~/apps/pcsx-redux/`, launched by `tools/vps-desktop/run-pcsx-redux`).
- Config `~/.config/pcsx-redux/pcsx.json`: `Interp: 2`, `Dynarec: false`
  (interpreter), `Debug: true`, `GdbServer: true`, `GdbServerPort: 3334`,
  `Bios: disc/scph5500.bin`.
- BIOS `disc/scph5500.bin` sha256
  `11052b6499e466bbf0a709b1f9cb6834a9418e66680387912451e971cf8a1fef`.
- Disc `disc/disc1.bin` sha256
  `39c547a9afc6da15d847ef81a2c6cea1a6516bdfa562cf13b0999b04e8598bda`
  (raw 2352-byte sectors).

## STEP 1 — the seam (static part)

Retail `0x8009980C` `[0x8009980C,0x80099BFC)` (disc/world_map.bin at
`0x8006FAF0`):

- scratchpad base: `0x80099828 lui $a3,0x1f80` → `0x80099834 move $t0,$a3`
  — the grid cursor starts at **`0x1F800000`**, exactly the port's
  `WM_9980C_SCRATCH`.
- grid-consumption instruction: **`0x800998D8 lwc2 $0, ($t0)`**
  (`0x800998DC lwc2 $1, 4($t0)` for the vertex's second word), i.e. the
  first vertex of triangle A is read into GTE V0 directly from the
  scratchpad grid; `0x800998E0/E4` read `+0x48/+0x4C` (V2), `0x800998F0/F4`
  or `0x80099900/04` read V1. Triangle B reads at `0x80099A44–0x80099A70`.
  `$t0 += 8` per cell (`0x80099BBC/CC`), 8×8 cells with a skipped column.
- Break address for a future hit: `0x800998D8` (first grid read of the
  first cell each call), condition `$t0 == 0x1F800000`.

## STEP 1 — the hit: NOT REACHED, and why

A confirmed hit requires retail execution to reach the world map "under
normal play". On this machine:

- no PCSX-Redux savestates exist (`find` over `~`, `~/.config/pcsx-redux`,
  `~/apps`, repo: none);
- both memory cards (`~/.config/pcsx-redux/memcard1.mcd`, `memcard2.mcd`,
  dated Aug 2) contain **no save blocks** (all 15 directory entries free);
- reaching the first world-map entry from a new game is the full Lahan
  chapter (intro, village events, battles, menus) — roughly an hour of
  interactive play with no existing scripted-input path.

Per the brief, this is reported once and the rung stops here. No
workaround (crafted savestate, memory patch, forced state) was attempted:
any of those would make the "normal play" qualifier false.

## STEP 2 — not performed

No capture was taken; there is no retail grid, camera, slot chain or
position to bank.

## STEP 3 — alignment gap, characterised statically only

What can be said without a retail observation (rule 4: stated as
expectation, not verdict):

- Tile index: on a *first* world-map entry retail runs the same fresh
  placement `0x80073448` the port now runs (W34C9), reading the same table
  row `(29947, 1, 11075)` for world index 1, so the same window `981C8`
  produces the same tile indices for the same patch ordinal. On any later
  entry retail takes the restore path `0x8007565C` (saved block at
  `0x8005A4E4`), which the port does not transcribe.
- Camera angles/height: the port's frame-60 values are `91B54`'s presets
  (`D3F0=0x460000`, `BD38=-0x260`), which retail also stores at slot init
  (`0x80091BAC/BB8`); comparable *if* retail is observed on a first entry
  before any camera state change.
- Port reachability: the port's forced entry (`XENO_WORLD_*` gate stack,
  `XENO_FIELD_ENTRANCE=0`) always lands at the fresh-placement position;
  it cannot be steered to an arbitrary retail position without either the
  restore path (`0x8007565C`) or a different placement-table row.

## Verdict

`NOT_ALIGNABLE_YET` — obstacle: there is no retail world-map state on
this machine (no savestate, empty memory cards, no scripted route to the
first world-map entry), so the retail side cannot be observed at all in
this rung. The static analysis above suggests the two sides *would* align
on a first entry, but that is an expectation, not an observation.

## Limitations

- The seam PC and scratchpad base are from disassembly, not from a live
  breakpoint; the "single confirmed hit" criterion is unmet.
- PCSX-Redux's CLI could not be listed headlessly (`-help` opens the GUI
  under Xvfb); the GDB-server path (port 3334) is configured but was not
  exercised.
