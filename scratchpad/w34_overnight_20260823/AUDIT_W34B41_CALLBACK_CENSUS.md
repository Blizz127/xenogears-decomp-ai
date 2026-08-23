# W34B41 — fresh callback-state census at the held backedge

## Result

The W34B39 binary was run with the existing read-only census harness. It
reached frame 916, the held tail, and the placeholder with rc=0. The fresh
capture is banked in `slice_14_census.log`.

The current held state is:

```text
pool       = 0x800D7538
stride     = 0x80
entry      = 2
backedge   = 0x800719C8 -> 0x8007130C
D_8009D554 = 1
held       = 1
```

The slot fields used by the retail scheduler are state `+0x00` (signed
halfword), timer `+0x02`, flag `+0x04`, cb0 `+0x18`, cb1/occupancy `+0x1C`,
and payload `+0x4C`. The full current table is in the evidence log; the
decision summary is:

| slots | final state | next scheduler action | verdict |
| --- | ---: | --- | --- |
| 0, 1, 2, 4, 5, 8, 9, 10, 12, 13, 14, 15 | 1 | cb1 from `+0x1C` | 12 predicted callbacks |
| 3, 6, 7, 11 | 3 | dormant scheduler arm | no callback |

All timer and flag fields were zero. No slot had an invalid state. The
natural scheduler counters reported `state0=16`, `state1=13`, `state3=3`
and `29/29` execution because those counters record the state before each
callback. Slot 11 was state 1 when dispatched and returned 3; the final
held-tail table therefore has 12 state-1 and four state-3 slots. This is not
a census contradiction.

## Backedge control-flow proof

Retail `0x800719C0` loads `D_8009D554`, and `0x800719C8` executes
`bnez v0, 0x8007130C`. The target is the frame-head pad/state clear, not a
direct scheduler call. From there the retail frame driver drains the input
loop and reaches the scheduler at the unconditional call site:

```text
0x800719C8 -> 0x8007130C
                 ... frame-head/input/CD synchronization ...
                 0x80071488 jal 0x80097800
                 0x80071490 DrawSync(0)
```

The call-site census in `w34b18_world_loop_7106c/SCHEDULER_CALLS.md` finds
only two scheduler calls: session call `0x80071064` and per-frame call
`0x80071488`. Therefore a real backedge would re-enter the frame prologue,
then the normal per-frame scheduler pass; it would not jump into the middle
of the pass or bypass frame setup.

## Predicted callbacks and resolver coverage

State 1 selects cb1. The twelve predicted addresses from the fresh table are:

| slot | cb1 | current resolver/body |
| ---: | --- | --- |
| 0 | `0x800925A0` | implemented; explicit resolver mapping |
| 1 | `0x8008A72C` | implemented; explicit resolver mapping |
| 2 | `0x8008B644` | implemented; explicit resolver mapping |
| 4 | `0x8008C844` | implemented; explicit resolver mapping |
| 5 | `0x8008D678` | implemented; explicit resolver mapping |
| 8 | `0x800907F4` | implemented; explicit resolver mapping |
| 9 | `0x800914D0` | implemented; explicit resolver mapping |
| 10 | `0x80091C18` | implemented; explicit resolver mapping |
| 12 | `0x80092C70` | implemented; explicit resolver mapping |
| 13 | `0x80092FD8` | implemented; explicit resolver mapping |
| 14 | `0x80071A58` | implemented; explicit resolver mapping |
| 15 | `0x80087734` | implemented; explicit resolver mapping |

`pc_port/src/world_map_scheduler.c` contains an explicit non-jalr resolver
branch for every address above, and the corresponding native bodies are
linked in the production build. The current natural pass 2 also executed
the same callback family with `missing=0`, `invalid=0`. No callback was
newly exposed by the fresh census.

## OT and tripwire context

Part 1 is now resolved as class (a): W34B39 corrected the single production
writer defect at `0x80089580`'s slot-table base, after W34B37/W34B38 fixed the
guest OT publication and narrow guest-native walk. The natural first frame
now reaches the OT terminator with two packets and zero adapter aborts.

The census does not change the milestone state:

- mode-loop `0x80072238`: intact, zero-hit;
- renderer/teardown entry `0x8007299C`: intact, zero-hit;
- separate world DrawOTag tripwire: intact, zero-hit;
- first backedge: naturally observed and held;
- no second frame was executed and no framebuffer PNG exists.

## Verdict and boundary

The held backedge is callback-covered in principle: every callback predicted
from the final 16-slot state is implemented and explicitly mapped. The
fresh census found no smaller callback slice. Per the standing rule, the
second-frame/backedge loop remains unimplemented; the next authorized task is
to implement the reviewed second iteration in a later session, using this
census and the W34B39 OT proof.
