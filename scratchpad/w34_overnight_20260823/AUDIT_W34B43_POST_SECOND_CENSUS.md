# W34B43 — post-second-frame callback census

## Evidence

W34B42's one reviewed re-entry completed cleanly. W34B43 reran the full
read-only slot census with `XENO_WORLD_FRAME_REENTRY_ONCE=1` and captured the
second held tail at scheduler entry 3. The complete table is in
`slice_16_census.log`.

The second-tail table is byte-for-byte identical to W34B41's first-tail
table:

- pool `0x800D7538`, stride `0x80`;
- slots 0, 1, 2, 4, 5, 8, 9, 10, 12, 13, 14, 15 final state 1;
- slots 3, 6, 7, 11 final state 3/dormant;
- all timers and flags zero;
- all cb0/cb1 addresses and payload values unchanged.

The final state still predicts the same twelve cb1 callbacks and four
dormant slots. Every predicted cb1 remains explicitly resolved in
`world_map_scheduler.c`; no new callback frontier exists.

## Control and runtime state

The retail control proof is unchanged: `0x800719C8` branches to
`0x8007130C`, which proceeds through frame-head synchronization to the
per-frame scheduler call at `0x80071488`. The second-tail capture observed
D554 still equal to 1, so the retail loop would continue.

The natural run completed with rc=0 and scheduler entry 3, `41/41` executed,
missing 0, invalid 0. The CD dispatcher had entry 3 with one busy dispatch
but no process/read/close failures. This is progress and not a crash.

## Verdict

The one-reentry state is stable and callback-covered. The remaining critical
path is repeated frame execution, not an uncovered callback or OT boundary.
The next bounded slice may extend the reviewed re-entry count, but must retain
a finite diagnostic bound until D554's clearing behavior is observed. The
mode-loop and `0x8007299C` should-not-run tripwires remain intact and zero-hit.
