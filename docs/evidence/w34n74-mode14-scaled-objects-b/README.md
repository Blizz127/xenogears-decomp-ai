# W34N74 — mode-14 second scaled-object callback pair

## Scope and retail anchor

This code-producing rung transcribes retail `[0x8007B604,0x8007BA08)`, the
second scaled-object scheduler pair registered by mode-14 setup
`0x8007A5DC`.  Its instruction shape parallels W34N73, but its guest callback
addresses and every context range remain distinct and are certified as such.

Authority:

- `disc/world_map.bin`, mapped at `0x8006FAF0`;
- `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`;
- `config/symbol_addrs.slus_006.64.txt`, identifying `0x80049DCC` as
  `ScaleMatrix`;
- exact 1028-byte slice SHA-256:
  `0de3c1b459271bd6a4c01cdc6f9e1ece4f385f7218f4d753f18c9f5d94b668f1`.

`0x8007B604` initializes the two context owners at `context+0x1F8` and
`context+0x24C` through `wm_8007A06C`, reading counts from descriptors at
`context+0x238` and `context+0x28C` and stream pointers at `context+0x240`
and `context+0x294`.  It zeros both slot phases, scales the executable base
matrix by `(0,4096,0)`, publishes that matrix to `context+0x2C0` and
`context+0x314`, enables owner states at `context+0x2A0/+0x2F4`, and returns
scheduler state 3.

`0x8007B798` clears its latch and owner-state halfwords, publishes the reset
X/Z positions and `Y=-64` to `context+0x200..0x208` and
`context+0x254..0x25C`, advances the primary and delayed secondary scale
phases by 384, clamps both at exactly `0x7F00`, scales two independent base
matrix copies, and publishes them to `context+0x218` and `context+0x26C`.

## Production change

- Added `world_map_callback_7b604.c/.h`.
- Registered `0x8007B604/0x8007B798` in the bounded scheduler resolver.
- Added the source to the normal port build manifest.

The twin is not aliased to W34N73's callback addresses.  Guest callback
addresses remain inert data until explicitly resolved to their own linked
native functions.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n74_mode14_scaled_objects_b.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- M1 omitted second primitive initialization: detected by
  `init.primitive_calls`
- M2 primitive count read from the stream: detected by
  `init.primitive_arguments`
- M3 omitted secondary phase seed: detected by `init.slot_seeds`
- M4 secondary phase advanced at 2048: detected by
  `update.stagger_threshold`
- M5 wrong `0x7F00` clamp: detected by `update.phase_clamps`
- M6 second matrix written over the first: detected by
  `update.matrix_destinations`

The certificate also proves this twin's distinct owners, descriptors,
streams, state halfwords, position ranges, initializer matrix destinations,
and update matrix destinations; both base-matrix inputs and scale vectors;
the latch; return states; and scheduler resolution.  W34N73's first-twin
certificate remains green.

Normal port build: `LINK OK`.

## Runtime boundary and next target

Mode 14 remains outside the accepted natural base route.  One mode-14-
specific callback pair remains unresolved: `0x8007AD34/0x8007ADD4`.
Restore and certify that pair next.  Only then is the registered scheduler
surface complete enough to integrate and naturally accept setup/teardown
`0x8007A5DC/0x8007A8AC`.
