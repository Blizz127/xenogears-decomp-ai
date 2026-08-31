# W34N78 — mode-12 moving-marker callback pair

## Scope and retail anchor

This code-producing rung transcribes retail `[0x8007D600,0x8007D774)`, one
of nine mode-12-specific scheduler callback pairs registered by setup
`0x8007BF50`.

Authority:

- `disc/world_map.bin`, mapped at `0x8006FAF0`;
- `scratchpad/w34n78_mode12_small_pair.objdump`;
- exact 372-byte slice SHA-256:
  `39cd7a36405687d90d83eb8d1b276781e295d5c997f213fc30d241c7823ed2a1`;
- initializer `[0x8007D600,0x8007D690)` SHA-256:
  `fe0aba201fef8e69f7753642255b2099cd6fb1661ec174e553034d97ab765224`;
- update `[0x8007D690,0x8007D774)` SHA-256:
  `c88088a5ed1810e7ef8e71733f5ea8d4191f0cd658fe4849cb307ecc54824466`.

`0x8007D600` links world-context records 13 and 14, clears the linked
context's state and YXZ rotation vector, constructs its rotation matrix, and
seeds one scheduler slot with the retail world position and negative-Z
velocity.

`0x8007D690` advances Z, applies the retail signed X/Z wrap helper to the
slot vector, reloads the wrapped coordinates, samples terrain height and
subtracts `0x4000`, publishes the shifted X/Y/Z position to the linked
context and scratchpad, and emits marker 25.

## Production change

- Added `world_map_callback_7d600.c/.h`.
- Registered `0x8007D600/0x8007D690` in the bounded scheduler resolver.
- Added the source to the normal port build manifest.

No mode-12 lifecycle code or other callback pair changed. Guest callback
addresses remain inert data until explicitly resolved to these linked native
functions.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n78_mode12_moving_marker.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- M1 wrong context-link destination: detected by `init.link`
- M2 omitted YXZ rotation initialization: detected by `init.rotation`
- M3 wrong Z velocity seed: detected by `init.slot_seed`
- M4 wrap helper given the height field instead of the X/Y/Z vector:
  detected by `update.wrap_target`
- M5 omitted terrain-height bias: detected by `update.height_bias`
- M6 wrong marker ID: detected by `update.marker`

The certificate additionally proves all four context halfword clears; the
exact angle/matrix pointers; all six slot seeds; Z advance before wrapping;
reload of both X and Z after the wrap helper mutates guest memory; terrain
sample arguments; arithmetic-shifted context publication; scratchpad vector
publication; marker flags; return states; and scheduler resolution for both
guest callback addresses.

Normal port build: `LINK OK`.

Adjacent regressions:

- W34N75 mode-14 camera control: PASS in O0/O2/UBSan, M1–M6 detected.
- W34N76 mode-14 lifecycle: PASS in O0/O2/UBSan, M1–M8 detected.

## Runtime boundary and next target

Mode 12 remains outside the accepted natural routes, so this rung makes no
visual claim. Eight mode-12-specific callback pairs remain unresolved. The
next bounded target is the adjacent small pair `0x8007D774/0x8007D7FC`;
mode-12 lifecycle integration remains deferred until its registered callback
surface is complete.
