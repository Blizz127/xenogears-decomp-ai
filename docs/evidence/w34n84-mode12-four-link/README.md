# W34N84 — mode-12 four-link callback pair

## Scope and retail anchor

This code-producing rung transcribes retail `[0x8007CC6C,0x8007CE84)`, one
mode-12-specific scheduler callback pair registered by setup `0x8007BF50`.

Authority:

- `disc/world_map.bin`, mapped at `0x8006FAF0`;
- `scratchpad/w34n84_mode12_pair_cc6c.objdump`;
- exact 536-byte slice SHA-256:
  `f1063d6c0894b9116d29bc1408db181ab01709e5bd92abd00cb4316c37b44864`;
- initializer `[0x8007CC6C,0x8007CD20)` SHA-256:
  `3f2b5b3fea92d35624a974e2b1877870a12ab15ae37676894e58773b2f6f3f21`;
- update `[0x8007CD20,0x8007CE84)` SHA-256:
  `b7c47f33d23ed52d722261f7c417755f457e7eca7f84aaeef14ffe1dd4dbaadd`.

`0x8007CC6C` links source context record 4 to destinations 0, 2, 1, and
3 in that retail order, clears object state and YXZ angles at
`context+0x150/+0x168`, constructs the matrix at `+0x170`, seeds the moving
vector, and clears its internal state.

`0x8007CD20` promotes internal state to 1 or 2 through latches 1 and 2,
respectively. Every update advances and wraps X/Z, publishes the shifted
position to `context+0x158` and scratchpad, and emits marker 19. States 1 and
2 additionally emit marker 31. Full fixed-point Z is mirrored to
`0x8009D564` only in states 0 and 1; state 2 deliberately suppresses it.

## Production change

- Added `world_map_callback_7cc6c.c/.h`.
- Registered `0x8007CC6C/0x8007CD20` in the bounded scheduler resolver.
- Added the source to the normal port build manifest.

No mode-12 lifecycle code or adjacent callback pair changed.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n84_mode12_four_link.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- M1 wrong fourth context link: detected by `init.links`
- M2 omitted YXZ matrix initialization: detected by `init.rotation`
- M3 wrong initial Z: detected by `init.slot_seed`
- M4 latch 2 promotes to state 1: detected by `state2.latch_state`
- M5 wrong primary marker ID: detected by `update.primary_marker`
- M6 state-2 marker omitted: detected by `state2.state_marker`
- M7 state 2 incorrectly mirrors Z: detected by `state2.z_suppressed`

The certificate additionally proves all four context-link calls in retail
order; exact context and matrix offsets; all slot seeds; both latch clears;
Z advance and wrap order; context and scratch publication; marker arguments;
state-0 single-marker/Z-mirror behavior; state-1 dual-marker/Z-mirror
behavior; state-2 dual-marker/no-mirror behavior; return states; and scheduler
resolution for both guest addresses.

Normal port build: `LINK OK`.

Adjacent regression:

- W34N83 dual-marker pair: PASS in O0/O2/UBSan, M1–M7 detected.

## Runtime boundary and next target

Mode 12 remains outside the accepted natural routes. Two mode-12-specific
callback pairs remain unresolved: `0x8007C36C/0x8007C3B8` and
`0x8007C724/0x8007C7D8`. The smaller `0x8007C36C` pair is next.
