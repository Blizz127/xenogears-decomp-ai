# W34N82 — mode-12 linked-marker C callback pair

## Scope and retail anchor

This code-producing rung transcribes retail `[0x8007D414,0x8007D600)`, one
mode-12-specific scheduler callback pair registered by setup `0x8007BF50`.
It is structurally parallel to W34N81 but owns distinct links, context state,
trajectory, and marker IDs.

Authority:

- `disc/world_map.bin`, mapped at `0x8006FAF0`;
- `scratchpad/w34n82_mode12_linked_marker_c.objdump`;
- exact 492-byte slice SHA-256:
  `b55394785a4e23da0c1a29fa5ef8514ac0626883e6682a595a3c6a5996788ce5`;
- initializer `[0x8007D414,0x8007D4A4)` SHA-256:
  `47a370db29e5bdf04518f21593ef13f2e4abc65fbb61181916e9353d50191a90`;
- update `[0x8007D4A4,0x8007D600)` SHA-256:
  `bf81b14a7f68c6076fdbbdd1136dcb09598620e9b6a7bddf18281de1c0d81338`.

`0x8007D414` links context records 12 and 15, clears object state and YXZ
angles at `context+0x3F0/+0x408`, constructs the matrix at `+0x410`, and
seeds the retail `(X,Z)=(0x00E80000,0x00280000)` trajectory.

The normal `0x8007D4A4` branch advances and wraps X/Z, samples terrain with
the retail `-0x4000` bias, publishes to `context+0x3F8` and scratchpad, and
emits marker 24. Latch 1 instead enables context records at `+0x3F0/+0x4EC`,
destroys marker record 24, emits marker 29 from the unchanged final position,
and returns scheduler state 3.

## Production change

- Added `world_map_callback_7d414.c/.h`.
- Registered `0x8007D414/0x8007D4A4` in the bounded scheduler resolver.
- Added the source to the normal port build manifest.

No mode-12 lifecycle code or W34N81 twin changed.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n82_mode12_linked_marker_c.sh
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

The certificate additionally proves this twin's distinct context offsets and
seed position; Z advance and wrap order; terrain inputs; context position;
normal marker arguments; completion latch clear; marker destruction; bypass
of movement helpers on completion; final-position scratch publication;
completion return state; and scheduler resolution for both guest addresses.

Normal port build: `LINK OK`.

Adjacent regression:

- W34N81 linked-marker B: PASS in O0/O2/UBSan, M1–M7 detected.

## Runtime boundary and next target

Mode 12 remains outside the accepted natural routes. Four mode-12-specific
callback pairs remain unresolved. The next smallest exact pair is
`0x8007CE84/0x8007CF18`.
