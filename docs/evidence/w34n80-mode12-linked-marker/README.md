# W34N80 — mode-12 linked-marker callback pair

## Scope and retail anchor

This code-producing rung transcribes retail `[0x8007D078,0x8007D228)`, one
mode-12-specific scheduler callback pair registered by setup `0x8007BF50`.

Authority:

- `disc/world_map.bin`, mapped at `0x8006FAF0`;
- `scratchpad/w34n80_mode12_context_pair.objdump`;
- exact 432-byte slice SHA-256:
  `d2e3b0c9d411d82a765a51b5047c0901a5685c0f7c914b5daffdc5715917c86e`;
- initializer `[0x8007D078,0x8007D110)` SHA-256:
  `74e420b0a1a52b843410fa032b8d7f4cf042384cb411867288df3bad5c828a39`;
- update `[0x8007D110,0x8007D228)` SHA-256:
  `41bc54320958541683b5ecc4c79ba53fa5a622baa6d5a444d066c93a0dc4e5e3`.

`0x8007D078` links context record 9 to records 7 and 8, clears the linked
object state and YXZ angles, constructs its matrix, and seeds a scheduler
slot with the retail X position and negative-Z velocity.

The normal `0x8007D110` branch advances and wraps X/Z, samples terrain with
the retail `-0x4000` height bias, publishes the shifted vector to the context
and scratchpad, and emits marker 22. Latch 1 instead clears the latch, enables
three context objects, destroys marker record 22, skips all movement work,
and returns scheduler state 3.

## Production change

- Added `world_map_callback_7d078.c/.h`.
- Registered `0x8007D078/0x8007D110` in the bounded scheduler resolver.
- Added the source to the normal port build manifest.

No mode-12 lifecycle code or other callback pair changed.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n80_mode12_linked_marker.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- M1 wrong second context link: detected by `init.links`
- M2 omitted YXZ matrix initialization: detected by `init.rotation`
- M3 wrong initial X: detected by `init.slot_seed`
- M4 omitted terrain-height bias: detected by `move.height_bias`
- M5 wrong moving marker ID: detected by `move.marker`
- M6 omitted completion context flag: detected by `complete.flags`
- M7 wrong completion return state: detected by `complete.return`

The certificate additionally proves all context clears; the exact matrix
pointers and slot seeds; Z advance before wrapping; reloaded terrain inputs;
signed-shifted context and scratch publication; marker arguments; the latch
clear; all three completion halfwords; marker destruction; complete bypass of
movement helpers on the latch branch; and scheduler resolution for both guest
addresses.

Normal port build: `LINK OK`.

Adjacent regressions:

- W34N78 mode-12 moving marker: PASS in O0/O2/UBSan, M1–M6 detected.
- W34N79 mode-12 moving target: PASS in O0/O2/UBSan, M1–M7 detected.

## Runtime boundary and next target

Mode 12 remains outside the accepted natural routes, so this rung makes no
visual claim. Six mode-12-specific callback pairs remain unresolved. The next
smallest exact pair is `0x8007D228/0x8007D2B8`.
