# W34N28 — retail D7CC-zero terminal integration

Verdict: **TERMINAL_ZERO_INTEGRATED_AND_CERTIFIED**.

## Anchor and scope

The rung started at `bcf72c92` on
`experiment/worldmap-open-gates-20260823`, equal to origin.  It integrates
only WorldMapMain's D7CC-zero lane and common terminal epilogue.  D7CC one and
the signed default lane remain separate work; no behavior is guessed for
them.

Retail instruction authority is `disc/world_map.bin` at base `0x8006FAF0`,
using `[0x800710D0,0x800711B0)` and the shared epilogue
`[0x800712A0,0x800712B8)`.  W34N27 supplies the executable ordering oracle
for the BBC4-nonzero path, and W34N26 supplies the exact production helper
and certificate for `wm_80094364`.

## Production integration

`world_map_terminal_zero_710e4.c` implements this order:

1. `LoadGameStateOverlay(1)`;
2. `ChangeGameState(1)`;
3. if guest BBC4 is zero, read the current D7D8 record;
4. if its signed type at `+0xE` is 3, call
   `wm_80094364(0x8009D55C,3,lhu(0x8006EF64))`;
5. reload D7D8, then publish BD3A and record halfwords `+8/+0xA`;
6. publish the low half of `lw(BD0C)+0x400` to guest EF68;
7. clear the main-executable system byte `D_800591AE`;
8. call `wm_800762FC()` and then `MainLoop(0)`.

The port has separate host globals and guest-RAM twins for main-executable
mutable data.  The terminal output tuple F950/F94E/F954 and `D_800591AE` use
their compiled native authorities because compiled field/system consumers
read those symbols.  The certificate seeds their guest twins differently and
proves they remain untouched.  World-overlay state and EF68 remain guest
memory.  This is the same authority rule established by W34N19 and W34N22,
not a literal rebasing of every retail absolute address.

`world_map_main_loop_71034.c` now remembers a natural signed exit after slot
2, closes the port capture contract, and dispatches this lane only for D7CC
zero.  The bounded 120-frame exit is unchanged and still skips slot 2 and all
natural terminal work.

## Focused production certificate

`run_w34n28_terminal_zero.sh` links the production terminal seam and active
WorldMapMain.  O0, O2, and nonrecovering UBSan pass with strict warnings.  It
covers:

- BBC4 nonzero with deliberately invalid D7D8, proving the dereference and
  region outputs are skipped;
- BBC4 zero/type 3, exact helper arguments, and mandatory D7D8 reload after a
  helper-selected replacement record;
- BBC4 zero/non-type-3, proving publication without a helper call;
- native output/system-byte authorities versus differently seeded guest
  twins;
- EF68 truncation after `BD0C+0x400`;
- full external-call/store event order;
- active WorldMapMain ordering from slot 1 through scheduler, driver, slot 2,
  capture closure, and D7CC-zero terminal dispatch.

Ten mutants are killed by named assertions:

- M1 swapped overlay/state calls: `order.guard.skip`;
- M2 inverted BBC4 guard: `guard.bbc4.skips.outputs`;
- M3 omitted helper: `guard.type3.calls.helper`;
- M4 wrong helper list index: `helper.retail.arguments`;
- M5 stale pre-helper D7D8: `outputs.reload.helper.record`;
- M6 guest output twins: `outputs.native.authority`;
- M7 `BD0C+0x200`: `ef68.bd0c.plus.400`;
- M8 omitted system-byte clear: `system.byte.native.authority`;
- M9 swapped common epilogue: `order.guard.skip`;
- M10 ignored record type: `guard.non3.skips.helper`.

## Regression and runtime gates

The following all pass after the final implementation:

- W34C1 cadence: O0/O2/nonrecovering UBSan, M1-M21 detected;
- W34N7 slot-2 teardown: all focused regimes, M1-M9 detected;
- W34N13 `762FC`: all focused regimes, M1-M6 detected;
- W34N26 `94364`: all focused regimes, M1-M7 detected;
- normal port build: `LINK OK`.

The normal detached 120-frame accepted route completed and fulfilled both
capture requests in-frame.  Because it takes the harness's bounded exit, the
new terminal path is dormant and both artifacts remain byte-identical to the
standing baseline:

- frame 60: `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`;
- frame 120: `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`.

## Bound and next code target

This proves exact static behavior for both BBC4 branches and product dispatch,
but the current natural accepted route never produces D7CC zero.  W34N27 is a
disclosed seeded guard-skip oracle, not natural terminal acceptance.  The
first richer route that naturally exits to field state should confirm the
type-3 arm and resulting field tuple without debugger-attached rendering.

The next bounded WorldMapMain target is the D7CC-one terminal/audio lane
`[0x800711B0,0x80071264)`, followed separately by the signed default lane.
