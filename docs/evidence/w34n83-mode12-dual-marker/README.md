# W34N83 — mode-12 dual-marker callback pair

## Scope and retail anchor

This code-producing rung transcribes retail `[0x8007CE84,0x8007D078)`, one
mode-12-specific scheduler callback pair registered by setup `0x8007BF50`.

Authority:

- `disc/world_map.bin`, mapped at `0x8006FAF0`;
- `scratchpad/w34n83_mode12_pair_ce84.objdump`;
- exact 500-byte slice SHA-256:
  `0971130ac4b0dc4da5608a07420621640c34d942a2bbed1caf786e69467ac244`;
- initializer `[0x8007CE84,0x8007CF18)` SHA-256:
  `ae6d1a7a27c50a40534319c5c1b2c87e75f9760a2d6d2e71782573ab2999fe2f`;
- update `[0x8007CF18,0x8007D078)` SHA-256:
  `8c88b9e90b5cecc02a3b6e2115ea2cd5ec7e1d3de2b596cfa203be9d4d51cf4f`.

`0x8007CE84` links context records 5 and 6, clears object state and YXZ
angles at `context+0x1A4/+0x1BC`, constructs the matrix at `+0x1C4`, seeds
the moving vector, and clears the slot's internal dual-marker flag.

`0x8007CF18` consumes latch 1 by setting that internal flag persistently.
Every update advances and wraps X/Z, samples terrain with the retail
`-0x4000` bias, publishes to `context+0x1AC` and scratchpad, and emits marker
21. Once armed, the same final position is additionally emitted as marker 30
on that frame and every later frame.

## Production change

- Added `world_map_callback_7ce84.c/.h`.
- Registered `0x8007CE84/0x8007CF18` in the bounded scheduler resolver.
- Added the source to the normal port build manifest.

No mode-12 lifecycle code or adjacent callback pair changed.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n83_mode12_dual_marker.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- M1 wrong context-link destination: detected by `init.link`
- M2 omitted YXZ matrix initialization: detected by `init.rotation`
- M3 wrong internal-state seed: detected by `init.slot_seed`
- M4 latch fails to arm the persistent flag: detected by `latch.arm`
- M5 omitted terrain-height bias: detected by `move.height_bias`
- M6 wrong primary marker ID: detected by `move.primary_marker`
- M7 wrong secondary marker ID: detected by `latch.dual_markers`

The certificate additionally proves this pair's exact context offsets and
trajectory; Z advance and wrap order; terrain inputs; context publication;
unarmed single-marker behavior; latch clear; armed two-marker ordering and
arguments; persistence across a later no-latch update; return states; and
scheduler resolution for both guest addresses.

Normal port build: `LINK OK`.

Adjacent regression:

- W34N82 linked-marker C: PASS in O0/O2/UBSan, M1–M7 detected.

## Runtime boundary and next target

Mode 12 remains outside the accepted natural routes. Three mode-12-specific
callback pairs remain unresolved. The next smallest exact pair is
`0x8007CC6C/0x8007CD20`.
