# W34N34 — retire the world-state entry selector

Verdict: **WORLD_STATE_ENTRY_UNCONDITIONAL**.

## Anchor and change

The rung started at `385d031b` on
`experiment/worldmap-open-gates-20260823`, equal to origin.  W34N33 made the
integrated session continuation unconditional once `PcPort_WorldMapInitMain`
was entered.  This rung removes the remaining outer selection in
`game_overrides.c`.

Main game state 3 is now always configured as:

```text
pFnMain    = PcPort_WorldMapInitMain
pMemStart  = PSX_ADDR(0x0009BBB0)
pHeapStart = PSX_ADDR(0x0009D80C)
hasOverlay = 1
```

The port no longer installs `PcPort_WorldMapPlaceholderMain` when
`XENO_WORLD_INIT` is absent.  The raw retail entry `0x80070CFC` is still never
used as a host function pointer; the state dispatches to its port-owned native
owner.

The old `PcPort_WorldMapInitEnabled` helper and per-stage environment readers
remain in `world_map_init.c` only for source-visible archaeology below the
W34N33 unconditional return.  They no longer choose the game-state-3 owner.

## Verification

The W34C1 cadence suite passes O0/O2/nonrecovering UBSan with M1-M21 detected.
It now statically requires the unconditional state-3 binding and rejects any
reintroduction of `PcPort_WorldMapInitEnabled` into `game_overrides.c`.  The
normal port reports `LINK OK`.

The decisive detached run omitted both former route selectors:

```text
XENO_WORLD_INIT        absent
XENO_WORLD_OPEN_LOOP   absent
```

It retained only the field-test bootstrap needed to select world state 3, the
explicit 120-frame bound, scripted input, capture directory, and host audio/
library settings.  The run:

- reached `PcPort_WorldMapInitMain` and detached before world rendering;
- loaded the overlay and entered slot 0 -> slot 1 -> scheduler -> frames;
- fulfilled capture frames 60 and 120 in-frame;
- reached `bounded exit frames=120 limit=120` and returned normally;
- reproduced the standing images exactly:
  - frame 60:
    `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`;
  - frame 120:
    `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`.

## Remaining runtime boundary

`XENO_WORLD_FRAME_LIMIT=120` remains intentionally active in this acceptance
run.  It is a bounded harness exit and suppresses slot 2 plus the natural
terminal lanes.  W34N24/W34N27/W34N29/W34N31 provide seeded retail lifecycle
oracles and W34N7/W34N28/W34N30/W34N32 provide compiled implementations, but
a naturally produced D554-zero route is still required for end-to-end
teardown/transition acceptance.
