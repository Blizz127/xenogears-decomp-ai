# W34B42 — reviewed one-frame re-entry

## Classification and boundary

W34B41 established that the held edge is callback-covered: the final slot
state predicted twelve cb1 callbacks, all explicitly resolved in the
scheduler, with four dormant state-3 slots. The W34B42 slice therefore adds
one bounded re-entry using already-built frame code; it does not implement a
new callback, mode-loop arm, renderer/teardown function, or raw guest call.

Retail control flow is:

```text
0x800719C0  lw  v0,D_8009D554
0x800719C8  bnez v0,0x8007130C
             ... frame head and input/CD synchronization ...
0x80071488  jal 0x80097800
```

The port now performs the exact branch predicate after the first accepted
frame-prologue return: if the reviewed gate is enabled and D554 remains
nonzero, it invokes the same frame prologue once more. The bounded one-shot
gate is `XENO_WORLD_FRAME_REENTRY_ONCE`; the default path remains unchanged.
The guard is a session harness boundary for this reviewed rung, not a bypass
of the retail D554 decision.

## Implementation

- `world_map_init.c`: adds the W34B42 gate and one additional call only when
  `D_8009D554 != 0` after the first frame; logs the retail source/target PCs.
- `world_map_frame_driver.c/.h`: adds the production-linked predicate
  `wm_800719C8_should_reenter_once`, preserving the unsigned nonzero D554
  test and gate ordering.
- No should-not-run guard was removed or routed around. No raw guest pointer
  is called.

## Verification

Focused certificate (`slice_15_tests.log`): O0/O2/UBSan-O2 all 5/5, identical
stdout, and three wrong-logic mutants detected.

Production build (`slice_15_build.log`): LINK OK.

Natural capture (`slice_15_natural.log`): rc=0; re-entry count 1; frame-tail
hit 2; OT draw calls 2; packets 4; walk steps 2050; both walks terminate at
`0x8009CE6C`; zero range/alignment/length/step aborts; scheduler entry 3;
41/41 callbacks executed; missing 0; invalid 0. Mode-loop `0x80072238`,
post-loop `0x8007299C`, and the separate world DrawOTag tripwire remain
zero-hit.

## Boundary for the next slice

This is deliberately one additional frame, not an unbounded retail session
loop. D554 remains 1 after the second tail, so the next task is to review the
second-frame state transition and then extend the loop only if the new
callback/OT state remains covered. No framebuffer PNG or renderer entry has
been claimed.
