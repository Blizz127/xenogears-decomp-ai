# W34N4 Phase 1 — retail recurring-loop shape

## Anchor and sources

The phase started on `experiment/worldmap-open-gates-20260823` at
`2ca47f9720ee4d18c59f729124e6bd218345ccae`, equal to origin and to the task's
expected anchor. The W34N3 authority is
`docs/evidence/w34n3-gate-census/README.md`.

Retail authority is the checked-in overlay image:

- `disc/world_map.bin`
- load base: `0x8006FAF0`
- SHA-256:
  `4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70`

The retail instructions cited here were decoded directly with:

```text
mips-linux-gnu-objdump -D -b binary -m mips:3000 -EL \
  --adjust-vma=0x8006faf0 disc/world_map.bin
```

`config/symbol_addrs.slus_006.64.txt` supplies names for system/PSYQ targets.
The prior files
`scratchpad/w34b5g_scheduler_80097800/WORLDMAIN_71034_710A0{,.raw}.txt`
independently agree over their narrower window, but are a cross-check, not the
authority. Port status was established separately from
`pc_port/src/world_map_main_loop_71034.c`,
`pc_port/src/world_map_frame_driver_712d0.c`,
`pc_port/src/world_map_frame_driver.c`, and exact-address searches at HEAD.

## First structural correction

**Established from retail:** `0x80071034` is not a function entry. It is the
outer/session-loop head inside retail `WorldMapMain`, whose port source records
the actual function entry as `0x80070CFC`. There is no prologue at `0x80071034`.
The pre-loop initialization reaches a mode-table slot-0 callback before the
session loop; the retail function epilogue is at `0x800712B8..0x800712CC`.

**Established from port:** `world_map_main_loop_71034.c` exposes the loop head
as a standalone C function and labels `[0x80071034,0x800710E4)` as its retail
boundary. That is a bounded harness seam, not the full retail function
boundary.

## Nested structure

### Pre-session dispatch: `0x80071000..0x80071030`, reusing `0x800710C0`

Retail reads mode index `0x8009C5A8`, computes `mode * 12`, and loads table
slot 0 from `0x8009A058 + mode*12`. A null slot skips to the `D7CC` test at
`0x800710CC`; a non-null slot jumps to the shared `jalr` at `0x800710C4` and
then reaches the same test. This callback is once before the first/re-entered
session loop, not once per displayed frame.

### Session iteration: `0x80071034..0x800710E0`

In exact order:

1. `0x80071034..0x80071060`: read the mode again; call table slot 1 at
   `0x8009A05C + mode*12` through `jalr`.
2. `0x80071064`: call scheduler `0x80097800`.
3. `0x8007106C`: `DrawSync(0)` (`0x800445D0`).
4. `0x80071074`: `Vsync(0)` (`0x8004B54C`).
5. `0x8007107C`: `ControllerResetState` (`0x80035DB0`).
6. `0x80071084..0x80071090`: copy `D_8009D7CC` to `D_8009C894`.
7. `0x80071094`: call the complete frame/session driver `0x800712D0`.
8. `0x8007109C..0x800710C8`: read mode again; call table slot 2 at
   `0x8009A060 + mode*12` through `jalr`.
9. `0x800710CC..0x800710E0`: signed compare `D_8009D7CC < 2`.
   `D7CC >= 2` branches back to `0x80071034`; `D7CC < 2` leaves the session
   loop at `0x800710E4`.

Thus a **session iteration** contains one slot-1 callback, one session-entry
scheduler pass, all frames driven by one invocation of `0x800712D0`, and one
slot-2 callback. It is not a frame iteration.

### Frame-driver session prologue: `0x800712D0..0x80071308`

Once per invocation/session, retail initializes the active draw-environment
pointer (`BE3C`), double-buffer index (`D7F0=1`), and inner-loop latch
(`D554=1`). It then falls into the frame head.

### Recurring frame iteration: `0x8007130C..0x800719CC`

The only recurring-frame back-edge is:

```text
0x800719C0  lw    v0,D_8009D554
0x800719C8  bnez  v0,0x8007130C
```

`D554 != 0` begins another frame at `0x8007130C`. `D554 == 0` falls through
to natural session teardown. A displayed frame is therefore the work between
`0x8007130C` and the draw/branch at `0x800719C8`, not an outer iteration at
`0x80071034`.

Frame call order, with conditional lanes noted, is:

| retail PC | call | condition / port status |
|---|---|---|
| `0x8007133C` | `ControllerPopState` (`0x80035CDC`) | repeated while it returns nonzero; **PORTED** compiled controller body |
| `0x800713FC` | `0x800967E4` CD-work dispatcher | repeated; return 3 calls Vsync and retries. The dispatcher body is **PORTED**, but active `world_map_frame_driver_712d0.c` calls a `void` wrapper and fabricates `queue_result=0`; **PARTIAL/DIVERGENT ON ACTIVE LOOP** |
| `0x8007140C` | `Vsync(0)` | only when dispatcher returns 3; **PORTED** |
| `0x80071424` | `CdSync(1,0x8009C588)` | **PORTED** |
| `0x80071468` | `ClearOTagR(ot,0x400)` | **PORTED ADAPTER**: guest-OT clear replaces unsafe host API use |
| `0x80071478` | `0x800250E0(index)` | generic body exists, but current `world_map_frame_driver_712d0.c` shadows it with an explicit local default stub; **STUB ON ACTIVE LOOP / PORTED ELSEWHERE** |
| `0x80071480` | `0x8001D468` | work-list body exists, but active driver shadows it with a local stub; **STUB ON ACTIVE LOOP / PORTED ELSEWHERE** |
| `0x80071488` | scheduler `0x80097800` | **PORTED**, exact second per-frame pass |
| `0x80071490` | `DrawSync(0)` | **PORTED** |
| `0x80071498` | `Vsync(2)` | **PORTED** |
| `0x800714A0` | `GameCheckAndHandleSoftReset` | **PORTED** compiled system body |
| `0x800714B0` | `PutDispEnv(env+0x5C)` | **PORTED** with guest mapping |
| `0x800714C0` | `PutDrawEnv(env)` | **PORTED** with guest mapping |
| `0x80071578` | `0x80093F18(D55C)` | conditional R4-world terrain selector; **PORTED** as `wm_80093F18` |
| `0x80071684` | `0x80075D4C` | conditional after selector result !=4; active driver uses local default stub and no implementation exists; **STUB/ABSENT** |
| `0x80071704` | `0x8007634C` | conditional input/menu lane; no exact-address body found; **ABSENT** |
| `0x8007175C` | `ControllerGetType(0)` | conditional; **PORTED** |
| `0x8007176C` | `0x80076594` | conditional after controller-type result zero; no body found; **ABSENT** |
| `0x800717FC` | `0x80075E7C(D55C,EF64)` | conditional transition lane; active driver local default stub; **STUB/ABSENT** |
| `0x800718E8` | `0x800758C0` | modes 1..3; active driver local default stub; **STUB/ABSENT** |
| `0x8007190C` | `0x800762FC` | modes 1..3; real bounded body exists only as a private helper in `world_map_init.c`; active driver uses local stub; **STUB ON ACTIVE LOOP / TRANSCRIBED ELSEWHERE** |
| `0x80071914` | `MenuMain` | modes 1..3; **PORTED** compiled system body |
| `0x8007191C` | `0x800762FC` | second modes-1..3 call; same status |
| `0x80071924` | `0x80075B58` | modes 1..3; active driver local default stub and no body found; **STUB/ABSENT** |
| `0x8007197C` | `0x80025044` image-list transfer | generic body exists; alternate bounded driver has a guest-safe port; active driver uses local stub; **STUB ON ACTIVE LOOP / PORTED ELSEWHERE** |
| `0x80071984` | `0x80074F2C` upload pump | transcribed port body exists; active driver shadows it with local stub; **STUB ON ACTIVE LOOP / PORTED ELSEWHERE** |
| `0x8007198C` | `0x80075104` upload pump B | transcribed port body exists; active driver shadows it with local stub; **STUB ON ACTIVE LOOP / PORTED ELSEWHERE** |
| `0x8007199C` | `SetGeomOffset(160,BE0C)` | **PORTED** |
| `0x800719B4` | `DrawOTag(ot+0xFFC)` | **PORTED ADAPTER**: guest OT walk, never host `DrawOTag` on guest RAM |

The active-loop status above is established from port source; it is not a
retail claim. `world_map_frame_driver.c` contains a separate, more complete
bounded transcription of portions of the same range, but the open-loop C calls
`wm_800712D0_run_bounded` from `world_map_frame_driver_712d0.c`. The two port
representations must not be treated as one complete body.

### Natural frame-driver/session exit: `0x800719D0..0x80071A4C`

Reached only when `D554 == 0`:

1. `ResetGraph(1)` at `0x800719D0` — **PORTED**.
2. If `D7F0 == 0`, `MoveImage({0,216,320,216},0,0)` at `0x80071A08` —
   **PORTED**.
3. `0x80096694` at `0x80071A10` — current active driver local default stub,
   no body found; **STUB/ABSENT**.
4. `DrawSync(0)` at `0x80071A18` — **PORTED**.
5. `Vsync(0)` at `0x80071A20` — **PORTED**.
6. `PutDispEnv(0x8009BC9C)` at `0x80071A30` — **PORTED** with mapping.
7. Return to the outer session loop.

## Every outer exit

After the frame driver and slot-2 callback, the signed `D7CC` test has four
structural outcomes:

| condition | retail path | effect and calls |
|---|---|---|
| `D7CC >= 2` | `0x800710DC -> 0x80071034` | another **session** iteration |
| `D7CC == 0` | `0x800710E4 -> 0x80071108..0x800711AC` | `LoadGameStateOverlay(1)`, `ChangeGameState(1)`, conditional `0x80094364`, state publication; first two are **PORTED**, `0x80094364` is **ABSENT** |
| `D7CC == 1` | `0x800710E4 -> 0x800711B0..0x80071260` | `LoadGameStateOverlay(2)`, `ChangeGameState(2)`, sound teardown/setup `0x80039CC4`, `ArchiveDecodeAlignedSize`, `memcpy`, `0x80039850`, `0x80039A80`; these system/audio bodies are **PORTED** |
| signed `D7CC < 2` but not 0/1 | `0x80071100 -> 0x80071264` | default state change/presentation path: `ChangeGameState(0)`, `ClearImage`, `DrawSync`; all **PORTED** |

The terminal calls in instruction order are:

| lane | PC | target | port status |
|---|---|---|---|
| `D7CC==0` | `0x80071108` | `LoadGameStateOverlay(1)` | **PORTED** |
| `D7CC==0` | `0x80071110` | `ChangeGameState(1)` | **PORTED** |
| `D7CC==0`, conditional | `0x80071158` | `0x80094364(D55C,3,EF64)` | **ABSENT** |
| `D7CC==1` | `0x800711B0` | `LoadGameStateOverlay(2)` | **PORTED** |
| `D7CC==1` | `0x800711B8` | `ChangeGameState(2)` | **PORTED** |
| `D7CC==1` | `0x800711F4` | `0x80039CC4` sound teardown | **PORTED** |
| `D7CC==1` | `0x8007120C` | `ArchiveDecodeAlignedSize` | **PORTED** |
| `D7CC==1` | `0x80071224` | `memcpy` | **PORTED** |
| `D7CC==1` | `0x8007123C` | `0x80039850` audio-manager creation | **PORTED** |
| `D7CC==1` | `0x80071254` | `0x80039A80(manager,127,0)` | **PORTED** |
| default | `0x80071264` | `ChangeGameState(0)` | **PORTED** |
| default | `0x80071290` | `ClearImage` | **PORTED** |
| default | `0x80071298` | `DrawSync(0)` | **PORTED** |
| common | `0x800712A8` | `0x800762FC` | **TRANSCRIBED PRIVATELY, NOT INTEGRATED** |
| common | `0x800712B0` | `MainLoop(0)` | **PORTED** |

All three terminal lanes converge at `0x800712A0`: clear a system byte, call
`0x800762FC`, call `MainLoop(0)`, then return at `0x800712B8..0x800712CC`.
`MainLoop` is **PORTED**. The `0x800762FC` sequence is transcribed privately in
`world_map_init.c` but is not integrated as a callable retail-address body,
so this exit call is **ABSENT FROM THE ACTIVE LOOP**.

## Phase 1 conclusion

Retail has two nested cadences:

```text
pre-session slot 0
  -> session loop at 0x80071034
       slot 1 -> scheduler -> sync -> frame driver
         -> frame loop 0x8007130C..0x800719C8 while D554 != 0
       slot 2
       -> repeat session while signed D7CC >= 2
  -> D7CC-selected terminal lane -> common epilogue
```

The current open loop correctly learned that `0x800719C8`, not `0x80071034`,
is the recurring displayed-frame boundary. It is nevertheless incomplete:
several retail frame calls are explicit local stubs, all non-base mode
callbacks are absent, and the standalone-C seam omits the pre-session slot-0
and terminal integration of the containing `WorldMapMain` function.
