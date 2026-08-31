# W34N81 — mode-12 linked-marker B callback pair

## Scope and retail anchor

This code-producing rung transcribes retail `[0x8007D228,0x8007D414)`, one
mode-12-specific scheduler callback pair registered by setup `0x8007BF50`.

Authority:

- `disc/world_map.bin`, mapped at `0x8006FAF0`;
- `scratchpad/w34n81_mode12_linked_marker_b.objdump`;
- exact 492-byte slice SHA-256:
  `25e4ffeb57e7343f65f4dc867c093ffd9a2219fb577f93f8fe65db4a77714ccb`;
- initializer `[0x8007D228,0x8007D2B8)` SHA-256:
  `34b7d9cb0195b0df3645eb741632bbb65899b093fb30f137405b8cc36083fbb7`;
- update `[0x8007D2B8,0x8007D414)` SHA-256:
  `22d6ce14bfefdd7fa57f5a6c54b8f39989e8eb2f3a1ba0e51ed5c7209cd10a89`.

`0x8007D228` links context records 10 and 11, clears the object state and
YXZ angles at the pair's distinct context range, constructs its matrix, and
seeds the retail trajectory.

The normal `0x8007D2B8` branch advances and wraps X/Z, samples terrain with
the retail `-0x4000` bias, publishes the shifted vector to context and
scratchpad, and emits marker 23. Latch 1 instead enables context records at
`+0x348/+0x39C`, destroys marker record 23, publishes the unchanged final
position to scratchpad, emits marker 28, and returns scheduler state 3.

## Production change

- Added `world_map_callback_7d228.c/.h`.
- Registered `0x8007D228/0x8007D2B8` in the bounded scheduler resolver.
- Added the source to the normal port build manifest.

No mode-12 lifecycle code or adjacent twin changed.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n81_mode12_linked_marker_b.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- M1 wrong context-link destination: detected by `init.link`
- M2 omitted YXZ matrix initialization: detected by `init.rotation`
- M3 wrong initial Z: detected by `init.slot_seed`
- M4 omitted terrain-height bias: detected by `move.height_bias`
- M5 wrong moving marker ID: detected by `move.marker`
- M6 omitted completion peer activation: detected by `complete.flags`
- M7 wrong completion marker ID: detected by `complete.marker`

The certificate additionally proves this pair's distinct context offsets;
all slot seeds; Z advance and wrap order; terrain inputs; context position;
normal marker arguments; completion latch clear; marker destruction; bypass
of movement helpers on completion; final-position scratch publication;
completion return state; and scheduler resolution for both guest addresses.

Normal port build: `LINK OK`.

Adjacent regression:

- W34N80 linked-marker pair: PASS in O0/O2/UBSan, M1–M7 detected.

## Runtime boundary and next target

Mode 12 remains outside the accepted natural routes. Five mode-12-specific
callback pairs remain unresolved. The equal-sized retail twin
`0x8007D414/0x8007D4A4` is next; its context offsets, seed position, and
marker IDs must remain distinct.
