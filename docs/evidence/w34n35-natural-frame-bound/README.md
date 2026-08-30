# W34N35 — remove the implicit 600-frame world cutoff

Verdict: **ABSENT_FRAME_LIMIT_IS_UNBOUNDED**.

## Anchor and fault

The rung started at `0396cada` on
`experiment/worldmap-open-gates-20260823`, equal to origin.  W34N33 and
W34N34 retired both route selectors, making the integrated owner the normal
game-state-3 path.  Source review then exposed a remaining harness behavior in
that product path: an absent `XENO_WORLD_FRAME_LIMIT` silently selected 600,
so otherwise normal world play would return from the frame driver after 600
displayed frames even while retail D554 remained nonzero.

## Production behavior

The main-loop frame-limit parser now returns:

- zero when `XENO_WORLD_FRAME_LIMIT` is absent (retail-style unbounded
  recurrence);
- a positive explicit bound when the harness variable contains one;
- zero for a nonpositive parsed value.

`wm_800712D0_run_bounded` accepts zero as the unbounded sentinel and tests the
bound only when it is positive.  D554 becoming zero still exits immediately
through the retail natural teardown path.  Negative run-structure bounds
remain errors.  The standalone legacy `wm_800712D0()` wrapper retains its own
explicit 600-frame request; the normal WorldMapMain owner does not call it.

## Certificates and regression

W34N28's production-linked main-loop integration now covers both modes:

1. explicit `XENO_WORLD_FRAME_LIMIT=120` is forwarded as 120;
2. with the variable unset, zero is forwarded and a synthetic natural D554
   exit still reaches slot 2 plus the signed terminal dispatcher.

Its O0/O2/nonrecovering-UBSan certificate remains green with M1-M10 detected.
The W34C1 cadence certificate remains green in all three regimes with M1-M21
detected.  Its M4/M5 mutation rewrite was re-anchored to the new guarded
bounded-return expression; both off-by-one mutants are again proven to change
the production code and are killed by
`frame_limit.displayed_frames.exactly_120`.

The normal port reports `LINK OK`.

After the final rebuild, the selector-free detached route with the explicit
120-frame test bound again completed normally and reproduced:

- frame 60:
  `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`;
- frame 120:
  `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`.

## Remaining acceptance boundary

Normal world execution now has no environment-selected owner and no implicit
time cutoff.  The unresolved lifecycle acceptance is behavioral: produce
D554 zero naturally through movement/transition state, then observe real
slot-2 teardown and the matching signed D7CC terminal lane.  The seeded retail
oracles and compiled terminal implementations exist; the natural producer is
the remaining route target.
