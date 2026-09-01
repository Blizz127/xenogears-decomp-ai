# W34N118 — retail cold world-entry defaults

## Result

`COLD_ENTRY_DEFAULTS_RESTORED`.

The rung started from `66c92f817dcca2317561b5424a643c76cb4d610c` on
`experiment/worldmap-open-gates-20260823`, equal to origin.  A zero entrance
halfword previously stopped in `world_map_main_init_lahan` and entered the
placeholder.  The port now executes retail's bounded cold-entry store block
`[0x80070D58,0x80070F38)` and rejoins the existing shared path at
`0x80070F38`.

Retail authority is `disc/world_map.bin`; the 480-byte range has SHA-256
`3d9ea3bb9d3206dfa400705ae3449f3afb98d9dc5927de7b74ce27d0eb938213`.

## Production behavior

`wm_80070D58_cold_defaults` restores the exact tuple and world-state constants,
the three resource-channel defaults `{0,10,5}`, the position/heading records,
and the two table-selected halfwords at `0x8006EE78/7A`.  It also publishes
retail's `0x07FFFFFF` bound at `0x8009D160`.

On PSX these addresses and `g_pGameState` are one allocation.  The PC port
keeps GameState in a host allocation while compiled world callbacks read the
guest aliases through `PSX_ADDR`.  Each GameState store is therefore mirrored
to both authorities.  The table input is read from the authoritative host
GameState, while the overlay lookup tables remain read-only guest data.

## Certificate

`pc_port/tests/run_w34n118_cold_defaults.sh` passes with strict warnings under:

- O0;
- O2;
- nonrecovering UBSan.

The focused test proves every fixed store, both lookup outputs, the complete
host and guest write sets, host/guest alias equality, read-only lookup tables,
and the null-input guard.  Five targeted mutants are detected by named
assertions:

1. keep the zero entrance;
2. replace channel 2's default `5` with zero;
3. omit the host GameState mirror;
4. use the next lookup-table index;
5. omit the `0x8009D160` bound.

An isolated staged-tree build at anonymous verification commit
`f5fb9c273406ed3d89e20dc80709d909721f9077` ended with `LINK OK` and linked a
strong `wm_80070D58_cold_defaults` symbol.

## Natural zero-entry acceptance

The detached native harness wrote only the zero entrance at the established
pre-world seam.  The former fatal branch instead reported:

```text
[worldmap-init] retail cold defaults applied; entrance=1
```

The route then completed 120 displayed frames, fulfilled both in-frame capture
requests, and reached `bounded exit frames=120 limit=120`.  It produced no
world-map stub hit and no OT-adapter abort.  The captures are coherent world
terrain/minimap views:

| frame | SHA-256 |
|---:|---|
| 60 | `86160ff11d91b3c1a540c3dc8a975e37cc5c7bdfba278150c1213f373da653df` |
| 120 | `e8fd7bf9ebf792f46aa9fc95a21da89c0ab8fdbf5ed8185ec23a8eb9e8139483` |

Artifacts remain in `scratchpad/w34n118_cold_capture/` and
`scratchpad/w34n118_cold_native.log`.

## Next exact frontier

The cold defaults intentionally set `0x8006EE68 = 0x4003`.  That naturally
enters the still-partial `wm_80073300` jump-table arm with index 3.  The port
currently logs an error and substitutes mode 1; retail table
`0x8006FB0C[index]` selects the `0x8007334C` arm and publishes mode 7.  This is
the first remaining divergence on the zero-entry route and is the next bounded
code-producing target.  W34N118 does not claim that fallback is correct.
