# W34N33 — retire the world open-loop selector

Verdict: **OPEN_LOOP_SELECTOR_RETIRED**.

## Anchor and change

The rung started at `7159a78f` on
`experiment/worldmap-open-gates-20260823`, equal to origin.  The accepted
WorldMapMain continuation no longer depends on `XENO_WORLD_OPEN_LOOP`.

After the overlay/GTE/common pre-session initialization,
`PcPort_WorldMapInitMain` now always performs the integrated base route:

```text
mode-table slot 0
  -> wm_80071034
       -> slot 1 owner
       -> session scheduler
       -> recurring displayed frames
       -> slot 2 and signed terminal dispatch on a natural exit
```

The former environment-implied setup ladder remains source-visible below the
unconditional return for focused archaeology, but no runtime environment flag
selects it.  The separate `XENO_WORLD_FRAME_LIMIT` control remains the
explicit bounded-test seam; this rung does not claim a natural D554 exit.

The legacy one-frame prologue predicate also no longer reads the retired
selector.  There are zero `XENO_WORLD_OPEN_LOOP` references in production
`world_map_init.c`.

## Verification

The W34C1 cadence certificate passes under O0, O2, and nonrecovering UBSan
with M1-M21 detected.  Its source gate now fails if the retired environment
name reappears in production init.  Exact frame/session ordering, 120
presentations, capture request/fulfillment parity, and bounded-exit terminal
suppression remain proven.

The normal port rebuild reports `LINK OK`.

The decisive detached run omitted `XENO_WORLD_OPEN_LOOP` entirely.  With only
`XENO_WORLD_INIT=1`, the usual field bootstrap, frame limit 120, and the
scripted input schedule, it:

- entered the retail session path and ran slot 0 plus the natural slot-1
  owner;
- produced and fulfilled frame-60 and frame-120 captures in-frame;
- reached `bounded exit frames=120 limit=120` and returned to init;
- reproduced the standing hashes exactly:
  - frame 60:
    `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`;
  - frame 120:
    `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`.

## Remaining selector boundary

`XENO_WORLD_INIT` still chooses the port-owned world entry in
`game_overrides.c`; removing that outer selector is a distinct route-level
change.  Within `PcPort_WorldMapInitMain`, however, the W34N3 open-loop gate is
closed: there is now one active session/frame continuation rather than a
retail path and a gate-selected legacy approximation.
