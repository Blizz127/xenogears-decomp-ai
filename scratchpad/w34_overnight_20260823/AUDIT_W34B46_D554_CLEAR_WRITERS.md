# W34B46 — D554 clear-writer census

## Frontier and retail shape

The bounded control frontier is still `0x800719C8`: retail loads
`0x8009D554` at `0x800719C0` and branches to the per-frame head
`0x8007130C` at `0x800719C8` when nonzero. The retail frame function spans
`0x800712D0..0x80071A4C`; its frame-local run-flag seed is the store at
`0x80071308`.

The current port has already linked the bounded calls at `0x80071488`,
`0x8007197C`, `0x80071984`, `0x8007198C`, and the tail's
`SetGeomOffset`/map-or-log `DrawOTag` path. The remaining frame-driver
region is not a single leaf: the retail JAL inventory between the head and
the held edge includes `0x80075D4C`, `0x8007634C`, `0x80035734`,
`0x80076594`, `0x80075E7C`, `0x800758C0`, `0x800762FC` (twice),
`0x8001C634`, `0x80075B58`, then the already-linked `0x80025044`,
`0x80074F2C`, `0x80075104`, `0x8004A12C`, and `0x80044BD0`.
The current production driver intentionally holds the unresolved branches
around `0x8007169C..0x80071978`; the region is approximately 0x2DC bytes
(183 instructions) and contains multiple absent call families.

## Complete retail D554 writer inventory

| retail PC | writer/condition | current-port status |
| --- | --- | --- |
| `0x80071308` | seed `D554 = 1` on frame-driver entry | implemented in `world_map_frame_driver.c` |
| `0x80071830` | frame-local exit after `wm_80075E7C` returns 1; also writes `D7CC = 1` and input mirrors | not implemented; inside the >150i held region |
| `0x80071954` | frame-local transition branch; clears D554/D7CC and writes `D7D8 = 0x8009B6E4` | not implemented; same held region |
| `0x80076BC8` | retail helper/callback clear path | no current source body or resolver mapping identified |
| `0x80077718` | retail helper/callback clear path | no current source body or resolver mapping identified |
| `0x800781B4` | retail helper/callback clear path | no current source body or resolver mapping identified |
| `0x80079350` | retail helper/callback clear path | no current source body or resolver mapping identified |
| `0x8007AD0C` | retail helper/callback clear path | no current source body or resolver mapping identified |
| `0x8007C6FC` | retail helper/callback clear path | no current source body or resolver mapping identified |
| `0x8007E428` | retail helper/callback clear path | no current source body or resolver mapping identified |
| `0x80080550` | retail helper/callback clear path | no current source body or resolver mapping identified |
| `0x800813C0` | retail helper/callback clear path | no current source body or resolver mapping identified |
| `0x8008A860` | `0x8008A72C`, class-1 result from `wm_80090A84`; sets slot main state `0x40`, clears D554/D7CC | implemented at `world_map_callback_8a72c.c:352-356` |
| `0x8008C9B8` | `0x8008C844`, `wm_80090C68` returns 1 (area mismatch); clears D554/D7CC | implemented at `world_map_callback_8c844.c:149-155` |
| `0x8008FAB0` | retail helper/callback clear path | no current source body or resolver mapping identified |
| `0x8008FF20` | retail helper/callback clear path | no current source body or resolver mapping identified |
| `0x800901F8` | retail helper/callback clear path | no current source body or resolver mapping identified |
| `0x8009059C` | retail helper/callback clear path | no current source body or resolver mapping identified |

The source-wide search finds no other current-port `D554` store. The
`world_map_callback_8e76c.c` definition is unused by its body and has no
store. `world_map_frame_driver_712d0.c:298` contains a second legacy
transcription of a mode-transition clear, but it is not the production
frame-driver route and must not be enabled as a shortcut.

## Live callback paths

The W34B45 third-tail capture at scheduler entry 4 retained the same full
table as W34B41/W34B43: slots 1 and 4 are state-1 cb1 candidates
`0x8008A72C` and `0x8008C844`; all twelve state-1 cb1 candidates are
resolved. Three bounded re-entries completed with scheduler `53/53`, no
missing/invalid callbacks, and `D554 = 1` after the third tail. Therefore
neither live callback clear predicate fired on the natural route. This is
observed stability evidence, not proof that either predicate is impossible
under later input/state.

## Classification and decision

This is `BLOCKED-NEEDS-REVIEW` class (e), not a bounded leaf:

1. The frame-local clear sites are embedded in a 183-instruction region
   with unresolved calls and two separate control lanes.
2. Thirteen other retail clear sites have no current implementation or
   accepted resolver mapping; implementing them would require callback or
   helper audits, not a local flag-store patch.
3. The two currently implemented callback writers are already on the live
   scheduler path and remained inactive across three clean frame tails.

Do not clear D554 in the tail, enable the legacy driver, or add an
unbounded re-entry. The next productive work is audit-ahead on the three
post-frontier regions and, separately, a reviewed branch-level census of
the two live callback predicates if the morning review chooses to pursue a
natural session exit.

Evidence: `slice_18_census.log` and the three-frame natural log
`slice_17_natural.log`; both are rc=0, with mode `0x80072238`, renderer
`0x8007299C`, and world DrawOTag tripwires unchanged.
