# W34N30 — retail D7CC-one transition/audio integration

Verdict: **TERMINAL_ONE_INTEGRATED_AND_CERTIFIED**.

## Anchor and scope

The rung started at `a64cb184` on
`experiment/worldmap-open-gates-20260823`, equal to origin.  It transcribes
only retail `[0x800711B0,0x80071264)` and its common terminal epilogue.  W34N29
is the repeatable executable retail oracle; `disc/world_map.bin` at base
`0x8006FAF0` is the instruction authority.

The signed default terminal lane is still out of scope.  No behavior for it is
folded into the D7CC-one implementation.

## Production sequence

`world_map_terminal_one_711b0.c` now performs:

1. `LoadGameStateOverlay(2)` then `ChangeGameState(2)`;
2. clear native system flag `D_800594F8`;
3. expand the three guest F8E5 party bytes to guest halfwords EE70/72/74;
4. call `func_80039CC4()`;
5. read archive id BCC8 and guest source C614;
6. obtain `ArchiveDecodeAlignedSize(BCC8)` and copy that many bytes from the
   guest source to native fixed buffer `D_80062648`;
7. save native `D_80062528` to native `D_8004F2FC`;
8. create a replacement manager with `func_80039850(D_80062648)`, publish it
   to `D_80062528`, and configure `(manager,127,0)`;
9. clear native `D_800591AE`, call `wm_800762FC()`, then `MainLoop(0)`.

WorldMapMain dispatches this lane only after a natural slot-2 return whose
signed D7CC value is one.  D7CC zero continues to the W34N28 lane; values at
least two repeat the session.

## Retail values confirmed by W34N29

The seeded retail oracle observed slot 2 set F954 from `0x0001` to `0x8001`,
then the terminal lane used:

```text
archive id       0x33
source           0x800FAAB8
aligned size     7132
copy destination 0x80062648
old manager      0x80067210
new manager      0x80069320
party tuple      1,1,1
configure        127,0
```

The production C follows the address/authority semantics rather than baking
those state-specific values into code.

## Focused certificate and regression

`run_w34n30_terminal_one.sh` runs the production seam under O0, O2, and
nonrecovering UBSan with strict warnings.  It uses differently seeded native
and guest twins and verifies exact call/store order, party stride, archive id,
copy bytes, old/new manager publications, configuration, and common epilogue.

Ten mutants are killed by named assertions:

- M1 swapped overlay/state calls: `order.retail.sequence`;
- M2 guest `594F8` twin: `byte594f8.native.authority`;
- M3 four-byte party stride: `party.halfword.stride`;
- M4 omitted sound cleanup: `sound.cleanup.once`;
- M5 archive id from C614: `archive.id.from.bcc8`;
- M6 source plus four: `copy.source.exact`;
- M7 omitted old-manager save: `manager.old.saved`;
- M8 omitted new-manager publication: `manager.new.published`;
- M9 level zero: `manager.configure.127.0`;
- M10 swapped common epilogue: `order.retail.sequence`.

W34N28's D7CC-zero certificate remains green.  W34C1 cadence remains green
under O0/O2/nonrecovering UBSan with M1-M21 detected; M8 now explicitly proves
that a bounded exit cannot dispatch a natural terminal lane.  The normal port
reports `LINK OK`.

The final normal detached 120-frame run reached its bounded exit with both
capture requests fulfilled in-frame.  The new natural terminal lane is
dormant there, and captures remain byte-identical to the standing baseline:

- frame 60: `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`;
- frame 120: `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`.

## Bound and next target

W34N29 is a disclosed D7CC seed, not natural transition acceptance.  The
first route that naturally returns D7CC one must confirm the manager handoff
and field re-entry without debugger-attached rendering.

The remaining base WorldMapMain terminal control-flow gap is the signed
default lane `[0x80071264,0x800712A0)`: `ChangeGameState(0)`, clear the retail
rectangle, synchronize, then enter the already integrated common epilogue.
