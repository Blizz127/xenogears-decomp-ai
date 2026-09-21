# W34N62 — retail mode-10 small scheduler callbacks

## Scope and anchors

- Starting HEAD: `0d68f30913371e9dd3639e3ca3ec34dcbe41ec22`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail source: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Decode cross-check: `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`

This slice restores only the three fully decoded small callback pairs used by
retail mode 10. It does not integrate the mode-10 setup/teardown lifecycle or
transcribe any of the larger mode-10 callbacks.

| Retail slice | SHA-256 |
|---|---|
| `0x800794D8..0x800795E4` | `256152e5da54eff64bd75bd4530688082473c4195dcfcdbffa3fcabd5b397992` |
| `0x8007A410..0x8007A568` | `0eb78dd4b6f028a4a2475237bd2b91b9acee353b2d465289a0339896058d7c50` |
| `0x8007A568..0x8007A5DC` | `c305e4ab4e51001f447c7636fb56d5d65edbab362b1486b38ec6eeff853b3047` |

## Production behavior restored

The bounded scheduler resolver now maps six guest addresses directly to
their native bodies; no guest address is cast to a host function pointer.

- `0x800794D8` seeds slot X/Z, the context angle `(0, 0x780, 0)`, and its
  `RotMatrixYXZ` result. `0x80079538` wraps X/Z, obtains the terrain height,
  applies retail's `+0x18000`, and publishes the shifted context position.
- `0x8007A410` seeds the 96-tick marker timer. `0x8007A430` consumes a
  one-shot position latch, advances Z, publishes marker 9 while active, and
  restores all four position/heading words plus marker state on expiry.
- `0x8007A568` is retail's side-effect-free return-3 leaf. `0x8007A570`
  clears the slot latch, truncates the context position into the scratch
  `SVECTOR`, and publishes marker 10.

The callback at `0x80079538` deliberately calls the existing port-owned
`wm_80093A5C` dependency with the exact retail arguments. Re-auditing or
expanding that helper is outside this callback slice.

## Focused certificate

`pc_port/tests/run_w34n62_mode10_small_callbacks.sh`:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- all direct branches, return states, scratch arguments, copy widths,
  fixed-point shifts, helper order/arguments, and scheduler resolution: PASS
- M1 wrong `0x780` rotation angle: detected by `orbit_init.rotation`
- M2 missing `+0x18000` height offset: detected by `orbit_follow.height`
- M3 wrong timer seed: detected by `beacon_init.timer`
- M4 wrong active marker ID: detected by `beacon_active.marker`
- M5 missing reset heading copy: detected by `beacon_reset.heading`
- M6 wrong initializer marker ID: detected by `marker_init.id`

## Regression and build gates

- W34N60 mode-9 callback certificate, M1–M5: PASS
- W34N58 shared-mode draw certificate, M1–M5: PASS
- W34N57 mode-8/mode-11 lifecycle certificate, M1–M5: PASS
- W34C1 scheduler cadence certificate, M1–M21: PASS
- canonical `./pc_port/build_port.sh`: LINK OK; port-owned addresses verified
- `git diff --check`: clean

## Runtime bound and next target

No accepted natural route registers these callbacks yet, so this commit is
runtime-neutral until mode-10 lifecycle integration lands. The next target is
the next bounded mode-10 callback pair; natural visual and teardown acceptance
remain owned by the eventual complete mode-10 lifecycle slice.
