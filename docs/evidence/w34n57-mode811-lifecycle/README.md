# W34N57 — retail mode-8/mode-11 lifecycle

## Verdict

`REPAIRED_VERIFIED`

Starting HEAD and origin were both
`ea0896b002b65c9e94404640f24671f284d7eb73` on
`experiment/worldmap-open-gates-20260823`.

Retail modes 8 and 11 now have native owners for all three mode-table slots:

- slot 0 `0x80071EF0`: second archive-wave submission;
- slot 1 `0x80077214`: shared session setup;
- slot 2 `0x80077480`: shared session teardown.

The retail source used for this slice was `disc/world_map.bin`, decoded in
`scratchpad/w34n56_mode8_11.objdump`. The setup range is
`[0x80077214,0x80077480)` and teardown is
`[0x80077480,0x8007756C)`.

## Production change

`world_map_mode811_lifecycle.c` transcribes the exact setup order:

1. framebuffer setup, `MoveImage`, `DrawSync`, and `0x80072DB4`;
2. finish the archive wave submitted by slot 0, then allocate the scheduler
   pool;
3. copy the 32-byte base matrix, write the five mode constants, and initialize
   the position vector;
4. run the mode-specific object/GPU/table/upload stages in retail order;
5. select archive 36, initialize terrain position, and drain while the queue
   distance is strictly greater than zero;
6. register `923A8/925A0`, `7756C/776E0`, `87710/87734`, and
   `71A50/71A58`;
7. execute the six-function common tail.

The teardown calls the nine retail helper/free groups, frees exactly the five
direct allocation slots plus the scheduler pool, then writes the retail
transition tuple (`F94E=17`, `F954=7`, `BBC4=1`, `F950=lhu(BD3A)`). The shared
`0x80084818` and `0x80097D64` leaves were exported from the already-certified
base teardown rather than duplicated.

Slot-0 ownership was split at its real lifecycle boundary: the pre-session
callback submits `0x80071EF0`; slot 1 later polls and performs `0x80073530`
fixups. `world_map_init.c` now resolves only the two known slot-0 families
(`71CDC` for modes 0–7 and `71EF0` for modes 8/11). The main-loop dispatcher
resolves `77214` and `77480` symbolically.

## Cross-mode prerequisite correction

The first entrance-8 run exposed four checks inherited from the historical
base-mode gate ladder. They required the immediately preceding base rung even
though the function body had no such data dependency:

- `0x80084580` required GPU asset B;
- `0x800736DC` required the base third archive wave;
- `0x80085F58` required the base primitive-template builder;
- `0x800863E0` required the base FT4 pools.

Retail mode 8/11 calls all four without those predecessors. The checks were
removed; one-shot protection, real input validation, and every body-level
oracle remain. The final base-route equivalence run below proves this does not
change the established mode-0/1 path.

## Focused certificate

Command: `pc_port/tests/run_w34n57_mode811_lifecycle.sh`

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- M1 swapped object/GPU-A order: detected by `setup.stage_order`
- M2 wrong camera callback pair: detected by `setup.registration_pairs`
- M3 `>= 2` queue drain: detected by `setup.cd_drain_positive`
- M4 missing `C180` free: detected by `teardown.free_sequence`
- M5 wrong teardown selector: detected by `teardown.state_values`

The certificate also checks the 32-byte template copy, all setup constants,
the position vector, archive/terrain arguments, final initializer arguments,
bounded setup/teardown write sets, symbolic dispatch integration, the split
submit/finish seams, and absence of host-stack pointer truncation.

Normal port build: `LINK OK` (`scratchpad/w34n57_build_mode8.log`).

Regression certificates also remain green for W34N56 callbacks, W34N7 base
slot-2 teardown, W34N9 base slot-1 ownership, W34N28 terminal-zero dispatch,
and the W34C1 scheduler cadence. The focused main-loop fixtures gained only
no-op definitions for the newly link-visible mode-8/11 arms.

## Runtime acceptance

### Established base route

The final binary ran the accepted detached route to its natural exit after
displayed frame 601. No generated world-overlay stub occurred before exit and
all upload-pump records reported `unknowns=0`.

The captures remain byte-identical to the W34N54/W34N56 oracle:

- frame 60: `15fdbff279b0bb5a83d0e049ba6bce0ca3397a94f6239bdea51424a23b430d04`
- frame 120: `50512b12deef85444cc69c92b924fc88021261f46146887e95a553977ce009c5`
- frame 600: `894f9da798e4cdb01e093834fda64ea49a8730a068e5a3827aaea507a0d15c93`

Log: `scratchpad/w34n57_base_final_run.log`.

### Mode 8

The harness changed only `g_pGameState+0x2320` to 8 at
`PcPort_WorldMapInitMain`, then detached before any world initialization. The
native route thereafter completed slot 0, slot 1, 121 displayed frames, the
release-driven `D7CC=0` exit, and slot 2. The registered `7756C` initializer ran
once and `776E0` updated naturally each recurring frame. No generated
world-overlay stub occurred during the world interval and upload unknowns were
zero.

- frame 60: `dfa350127063dce5eb7f05552342dccbd055e184aa6e0e3c1c7e76e4c7712202`
- frame 120: `2e4ec92f35d37dda5899c196a1c8867ba05d339c1c4740897cb5982a866b7d44`
- natural exit: displayed frame 121, `D7CC=0`

Both captures show coherent panoramic terrain, ocean, sky, and clouds. Their
different hashes agree with the held camera input. Artifacts:

- `scratchpad/w34n57_mode8_capture/world-frame-000060.bmp`
- `scratchpad/w34n57_mode8_capture/world-frame-000120.bmp`
- `scratchpad/w34n57_mode8_run.log`

### Mode 11

The same selector-only method with entrance 11 completed the identical shared
lifecycle and exited naturally after displayed frame 61. Its frame-60 capture
is byte-identical to mode 8 at the same input/frame boundary:

- frame 60: `dfa350127063dce5eb7f05552342dccbd055e184aa6e0e3c1c7e76e4c7712202`
- natural exit: displayed frame 61, `D7CC=0`

Artifacts: `scratchpad/w34n57_mode11_capture/world-frame-000060.bmp` and
`scratchpad/w34n57_mode11_run.log`.

The `[stub] SoundHandleError` line occurs only after natural world teardown in
the next field state, as bounded by W34N55; it is not a world lifecycle stop.

## Boundary

This proves the shared mode-8/mode-11 lifecycle and the two already-restored
camera callbacks. It does not claim support for modes 9, 10, or 12–18, whose
slot-1/slot-2 pairs remain unresolved. It also does not turn the test-only
selector injection into a product route; ordinary game progression must still
produce these mode indices naturally.

`NEXT_EXACT_TARGET`: restore the next self-contained mode-table lifecycle pair,
ranked by missing callback surface and reuse of already-ported helpers, while
retaining exact base-route neutrality.
