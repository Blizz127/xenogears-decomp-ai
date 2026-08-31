# W34N73 — mode-14 first scaled-object callback pair

## Scope and retail anchor

This code-producing rung transcribes retail `[0x8007B200,0x8007B604)`, the
first of two structurally parallel scaled-object scheduler pairs registered
by mode-14 setup `0x8007A5DC`.  It does not integrate the lifecycle or merge
the neighboring `0x8007B604/0x8007B798` pair into this evidence unit.

Authority:

- `disc/world_map.bin`, mapped at `0x8006FAF0`;
- `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`;
- `config/symbol_addrs.slus_006.64.txt`, identifying `0x80049DCC` as
  `ScaleMatrix`;
- exact 1028-byte slice SHA-256:
  `21bd3f79595e2d677c07d54cb77ec7570d1cbd623c3263faec1b730ab9a84de1`.

`0x8007B200` initializes two 84-byte context owners at `context+0x150` and
`context+0x1A4` through the already-owned retail helper `wm_8007A06C`.  Each
primitive count comes from the descriptor pointer at owner-relative `+0x40`,
not from the primitive stream pointer at `+0x48`.  It zeros both slot phases,
scales the executable base matrix by `(0,4096,0)`, publishes that matrix to
`context+0x218` and `context+0x26C`, enables the associated owner states, and
returns scheduler state 3.

`0x8007B394`:

- consumes and clears the slot latch and the two owner-state halfwords;
- publishes the shared reset X/Z positions (arithmetic `>>12`) and `Y=-64`
  to both owners;
- increments the primary scale phase by 384 each update;
- leaves the secondary phase unchanged while primary is at most 2048, then
  increments it by 384 starting at primary 2049;
- clamps both signed phases at exactly `0x7F00` (32512);
- independently scales two fresh copies of the executable base matrix; and
- publishes them to `context+0x170` and `context+0x1C4` before returning 1.

## Production change

- Added `world_map_callback_7b200.c/.h`.
- Registered `0x8007B200/0x8007B394` in the bounded scheduler resolver.
- Added the source to the normal port build manifest.

No raw guest callback address is executed.  All context, descriptor, stream,
matrix, and scratch addresses retain their retail guest-domain semantics.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n73_mode14_scaled_objects_a.sh
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

The certificate also checks descriptor and owner addresses, base-matrix
copying before each scale, exact scale vectors, both initializer matrix
copies, latch clearing, sign-preserving reset positions, both update matrix
publications, return states, and scheduler resolution of both guest callback
addresses.  W34N72's path-camera certificate remains green.

Normal port build: `LINK OK`.

## Runtime boundary and next target

Mode 14 remains outside the accepted natural base route, and this pair adds
no base-mode registration.  Natural runtime evidence is therefore deferred
to complete lifecycle integration.

Two mode-14-specific pairs remain unresolved:

- the structural scaled-object twin `0x8007B604/0x8007B798`;
- `0x8007AD34/0x8007ADD4`.

Restore the twin next using its own retail offsets and certificate; do not
alias the two guest callback addresses merely because their instruction
shapes are parallel.
