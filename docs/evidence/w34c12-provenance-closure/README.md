# W34C12 — remaining provenance lines and the retail-reference blocker

Branch `experiment/worldmap-open-gates-20260823`, base `41e65bc9`.
Read-only.

## Provenance closed (rule 1)

| input of `wm_8009980C` | source side | retail producer (PC) | port |
|---|---|---|---|
| tile bytes (`tile_data`) | source data | CD loader `0x8009699C`/`0x80096C0C` from sector records built by `0x80097DC0` (`sector + tile`) | W34C11; tile 60 == disc sector `0x44110` |
| scratch vertex grid | producer output | `0x80099708` (`0x80099744–0x800997E8`) | W34C6 (low-byte height) |
| sine table `0x800523F0` | source data | main-exe `.sdata`; `0x80099724` | W34C2 |
| camera `0x8009C808` | producer output | `0x80097440` (`0x800975B4`) / `0x80097244` (`0x8009734C–8C`) | `087241d7` (w34b60/w34b65) |
| camera record `0x8009BD40` | producer output | `0x80096F18` (`[0x80096F18,0x80097070)`) | verified W34C7 |
| height `0x8009D3F0`, angles `0x8009BD38` | source for `96F18`/`97440` | `0x80091B54` presets `lui 0x46` → `D3F0` (`0x80091BAC`), `−0x260` → `BD38` (`0x80091BB8`); alt preset `lui 0x28` (`0x80091BE8`) | port `91b54`/`91c18`; observed `0x460000` / `−608` from frame 1 |
| position `0x8009C5AC/B4` | source for `97BC0`/`981C8` | `0x80073448` table row (`0x80073494–BC`) | W34C9 |
| matrix source `0x8009D534` | source data | `0x80097BE4–0x80097D10` (identity) | W34C7 seed |
| shade table `0x8009CCB4` (64 h) | source data | `0x800979C8`: `GetClut(0, 432+i)` → `sh` at `0x80097AE8` | W10B (`world_map_init.c:3716`) |
| colour table `0x8009CD54/CD5C` | source data | `0x800979C8`: `GetTPage` → `sh` at `0x80097B30` / `0x80097B70` | W10B (`world_map_init.c:3732`) |
| OT/packet buffers | producer output | `0x8007369C` allocations; `0x800739B8` templates | W34B37/W14B |

Every input now has a retail producer with a cited PC. Not re-verified in
full: `0x80091C18`'s interpolation constants (the port's step cap
`0x400000` vs the retail constant materialised at `0x80091E7C–80`); at
frame 60 the values are still the frame-1 presets, so this cannot affect the
frame-60 capture, but it is a fidelity line for a later rung.

## Criterion (1): "comparable to retail" — blocked externally

There is no retail frame-60 reference in the repo. The machine has no PS1
emulator installed, and `~/dev/xenogears-assets/bios/` is empty, so none
could run the retail disc even if installed (network is available; a BIOS
is not). The frame-60 capture after W34C11 (`26e59e42…`) is a continuous
textured ground plane with a horizon band; frame 120 (`ae06268a…`) is a
full-view terrain mosaic. Both are dark and low-contrast. Without a
reference, no comparability verdict is recorded (rule 4/7).

## Next (one task)

Produce the retail reference off-box: run the retail disc in an emulator
with a BIOS on the same route (new game → Lahan entrance, the
`0:0x2000,600:0x4000,916:0x2000,976:0` input schedule) and capture frame 60;
then compare horizon line, tile grid alignment and shade against
`26e59e42…`. The remaining on-box fidelity line is `0x80091C18`'s
interpolation constants.
