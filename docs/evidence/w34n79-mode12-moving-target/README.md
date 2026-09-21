# W34N79 — mode-12 moving-target callback pair

## Scope and retail anchor

This code-producing rung transcribes retail `[0x8007D774,0x8007D918)`, one
of the remaining mode-12-specific scheduler callback pairs registered by
setup `0x8007BF50`.

Authority:

- `disc/world_map.bin`, mapped at `0x8006FAF0`;
- `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`;
- exact 420-byte slice SHA-256:
  `27f92cad15414e886a8ce98d6483c6f28a80bf027bf658340244a5ff61b7af86`;
- initializer `[0x8007D774,0x8007D7FC)` SHA-256:
  `96134df85ac70eeaf96316cc762b13566c9a7de1338ea40d9762763268d7b5be`;
- update `[0x8007D7FC,0x8007D918)` SHA-256:
  `e41b4150cd7f736ac33e902e59f4a9036708211a93c431ab90e8b2a2eca7d2d8`.

`0x8007D774` enables the context object at `context+0x540`, clears its
YXZ angles, constructs its matrix, and seeds the scheduler slot with the
retail X/Y/Z position and negative-Z velocity. It returns scheduler state 3.

`0x8007D7FC` consumes latch 1 by disabling the context object and resetting
X/Z relative to the live world position, or consumes latch 2 by setting the
slot's internal state to 2. It then advances Z, wraps X/Z, publishes the
shifted vector to `context+0x548`, and, only while internal state 2 is active,
mirrors the full fixed-point position to `0x8009D55C`.

## Production change

- Added `world_map_callback_7d774.c/.h`.
- Registered `0x8007D774/0x8007D7FC` in the bounded scheduler resolver.
- Added the source to the normal port build manifest.

No mode-12 lifecycle code or other callback pair changed.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n79_mode12_moving_target.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- M1 wrong context state seed: detected by `init.context_state`
- M2 omitted YXZ matrix initialization: detected by `init.rotation`
- M3 wrong initial Y: detected by `init.slot_seed`
- M4 wrong latch-1 Z offset: detected by `latch1.position_reset`
- M5 wrong latch-2 internal state: detected by `latch2.state`
- M6 wrap helper given the wrong vector base: detected by
  `update.wrap_target`
- M7 omitted state-2 fixed-point position mirror: detected by
  `latch2.position_copy`

The certificate additionally proves the exact angle and matrix pointers; all
six slot seeds; initializer and update return states; both latch clears; the
context-state clear; Z advance before wrapping; reload and signed shift of all
three coordinates after wrapping; suppression of the fixed-point mirror in
state 0; its exact three-word publication in state 2; and scheduler resolution
for both guest callback addresses.

Normal port build: `LINK OK`.

Adjacent regression:

- W34N78 mode-12 moving marker: PASS in O0/O2/UBSan, M1–M6 detected.

## Runtime boundary and next target

Mode 12 remains outside the accepted natural routes, so this rung makes no
visual claim. Seven mode-12-specific callback pairs remain unresolved. The
next rung should take the smallest remaining exact retail pair before any
lifecycle integration is attempted.
