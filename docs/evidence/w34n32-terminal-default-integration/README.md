# W34N32 — retail signed-default terminal integration

Verdict: **TERMINAL_DEFAULT_INTEGRATED_AND_CERTIFIED**.

## Anchor and scope

The rung started at `2766c9b8` on
`experiment/worldmap-open-gates-20260823`, equal to origin.  W34N31 supplies
the repeated executable retail oracle, while `disc/world_map.bin` is static
instruction authority for `[0x80071264,0x800712A0)` and the common terminal
epilogue.

This rung adds only the signed-default terminal lane.  The already integrated
D7CC-zero and D7CC-one paths remain separate functions and retain their own
certificates.

## Production behavior

`world_map_terminal_default_71264.c` performs the retail sequence:

1. `ChangeGameState(0)`;
2. construct native `RECT {0,0,319,431}`;
3. call `ClearImage(rect,0,0,64)`;
4. call the lane's direct `DrawSync(0)`;
5. clear native `D_800591AE`;
6. call `wm_800762FC()`;
7. call `MainLoop(0)`.

The rectangle is native stack storage because its consumer is the host GPU
API, not a guest-address helper.  This is distinct from the pointer-domain
defects repaired elsewhere in the campaign.

After natural slot-2 teardown, `wm_80071034` now dispatches:

- D7CC zero to the W34N28 lane;
- D7CC one to the W34N30 lane;
- any other signed value below two to this default lane;
- values at least two to another retail session;
- a bounded harness exit to no terminal lane.

## Certificate and regressions

`run_w34n32_terminal_default.sh` links the production helper under O0, O2,
and nonrecovering UBSan with strict warnings.  It proves exact state argument,
rectangle, RGB values, direct sync count, native byte authority, common
epilogue arguments, and call order.  Eight mutants are killed by named
assertions:

- M1 wrong state: `state.change.zero`;
- M2 wrong rectangle origin: `clear.origin.zero`;
- M3 wrong extent: `clear.extent.319.431`;
- M4 wrong blue component: `clear.color.0.0.64`;
- M5 omitted direct sync: `draw_sync.direct.once`;
- M6 guest system-byte twin: `system.byte.native.authority`;
- M7 swapped common epilogue: `order.retail.sequence`;
- M8 wrong MainLoop argument: `main_loop.argument.zero`.

W34N28's production-linked integration test now separately drives signed
D7CC `-1` through the real main-loop dispatch and proves slot 2 precedes the
default terminal seam.  Its D7CC-zero certificate still passes in all three
regimes with M1-M10 detected.  The W34C1 cadence certificate still passes in
all three regimes with M1-M21 detected and proves a bounded exit skips every
natural terminal lane.  The normal port reports `LINK OK`.

## Detached neutrality

The final normal binary ran the accepted detached 120-frame route.  It
fulfilled both capture requests in-frame, reached the bounded exit, and
returned to world-map init.  Because no natural signed-default state occurs,
the captures remain byte-identical to the standing baseline:

- frame 60: `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`;
- frame 120: `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`.

## Bound

W34N31 seeds D7CC `-1`; this rung does not claim natural reachability or name
its producer.  It closes the remaining base WorldMapMain terminal
control-flow gap.  The next route-level acceptance target is a natural
non-bounded session exit that exercises slot 2 and one terminal lane without
a debugger state seed.
