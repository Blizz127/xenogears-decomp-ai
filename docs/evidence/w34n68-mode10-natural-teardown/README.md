# W34N68 — mode-10 natural teardown discriminator

## Anchor and scope

- Starting HEAD: `16790af3c2da57e93c591dfc77c4354b0fd40c13`
- Branch: `experiment/worldmap-open-gates-20260823`
- Production changes: none
- Route: mode 10, accepted scripted bootstrap/input, detached before
  `PcPort_WorldMapInitMain`

W34N67's 800-frame run reached only its configured bound. This rung tests
whether that observation identifies a stalled mode-10 state machine or merely
a watchdog shorter than the retail timer sequence.

## State-only discriminator

A paced, attached GDB probe sampled guest scheduler records at entry to
`wm_80079778`. It performed no framebuffer readback and logged internal state
only. Slot 2 and slot 1 followed this sequence:

| Callback hit | Slot 2 state | Slot 1 state | Slot 1 timer | Event |
|---:|---:|---:|---:|---|
| 1 | 0 | 16 | 64 | initial mode-10 state |
| 187 | 1 | 16 | 64 | first Y threshold reached |
| 316 | 2 | 16 | 64 | second Y threshold reached |
| 339 | 3 | 17 | 8 | slot-2 claim consumed by slot 1 |
| 347 | 4 | 18 | 16 | slot 2 reaches its terminal state |
| 363 | 4 | 0 | 1 | slot 1 enters recurring sequence |
| 364 | 4 | 1 | 32 | recurring state 1 |
| 396 | 4 | 2 | 32 | recurring state 2 |
| 428 | 4 | 3 | 48 | recurring state 3 |
| 476 | 4 | 4 | 40 | recurring state 4 |
| 516 | 4 | 5 | 120 | recurring state 5 |
| 636 | 4 | 6 | 90 | recurring state 6 |
| 726 | 4 | 7 | 80 | final countdown begins |
| 800 | 4 | 7 | 6 | `D554=1`, six ticks remain |
| 806 | 4 | 7 | 0 | `D554=0`, `D7CC=0` |

At hit 806, the probe's breakpoint on `wm_80078D24` fired. The slot-2 claim,
slot-1 latch, all timed transitions, natural session exit, and mode-10 teardown
are therefore live. No production timer or predicate is stuck.

## Detached confirmation

The same route was rerun with GDB detached before world initialization and a
900-frame watchdog. The product runtime reported:

```text
[worldmap-open-loop] frame=780/900
[worldmap-open-loop] natural state exit frames=806 D7CC=0
```

The frame-780 capture was fulfilled before exit:

- `world-frame-000780.bmp`
- SHA-256:
  `be727ab195a0996ccefbc0e77cbf2017c3b7c48f8bd347c942d1c96a49453168`

No bounded-exit or capture-pending error occurred. After the natural world
exit and mode-10 teardown, the enclosing application entered `FieldMain`; the
watcher then terminated it after the required evidence was present.

## Verdict

`NATURAL_TEARDOWN_PASS`

The former 800-frame result was six frames short of the retail countdown. It
did not expose a lifecycle defect and does not justify shortening any timer or
altering any callback. W34N66's mode-10 lifecycle integration now has natural
setup, recurring execution, and teardown coverage on this route.

The remaining mode-10 question is visual parity: captures contain the craft,
sky, ocean, and animated geometry, but the large layered dark geometry has not
yet been compared with a retail mode-10 reference.
