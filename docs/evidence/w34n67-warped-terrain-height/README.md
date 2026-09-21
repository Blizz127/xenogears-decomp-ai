# W34N67 — retail warped-terrain height sampler

## Scope and anchor

- Starting HEAD: `4aa182b9860c6e69c8a6e1d60cd15200dc8b63fc`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail source: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Retail function: `[0x80093A5C, 0x80093E8C)`
- Slice SHA-256: `95e643f45cc9e23616b613d8ae1d66f423d5c36fa504463f362aff2530c9e8df`

This bounded repair replaces a placeholder transcription that aborted the
first recurring mode-10 frame. It does not weaken the accepted plane solver
or change any mode-10 callback/lifecycle body.

## Root cause and production repair

Retail takes signed fixed-point world X and Z coordinates. It calls
`wm_80093660(x,z)` and uses that returned cell pointer. The former port body
instead loaded a fabricated cell pointer from guest address zero and treated
the second world coordinate as a data pointer.

The replacement restores the complete retail dataflow:

- independent X/Z subcell indices and the eight ordered retail trig calls;
- four corner bytes at cell offsets `0x00`, `0x04`, `0x24`, and `0x28`;
- warped corner heights at scratch offsets `0xA2/0xAA/0xB2/0xBA`;
- both cell-flag domains and both signed coefficient-selection branches;
- exact edge vectors and `edge1 x edge0` winding;
- cross-product normalization, local query X/negative-Z, and flag-selected
  base corner; and
- `wm_800935DC(query,base,normal)` followed by retail's `<< 12` result.

All signed multiplications, selection sums, shifts, truncations, and stores
retain explicit 32-bit R3000A boundaries. Retail call `0x8003F8B0` uses the
port's established Xenogears trig-entry shim rather than PsyCross's
conventional symbol naming.

## Focused certificate

`pc_port/tests/run_w34n67_93a5c.sh`:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- cell-lookup arguments, eight trig arguments/order, four corner values, all
  four triangle-selection cases, cross winding, normalization input, plane
  arguments/query/base/normal, output shift, and authorized write set: PASS
- M1 reused X as cell-lookup Z: detected by `cell_lookup.coordinates`
- M2 omitted the X-neighbor phase: detected by `height.rcos_order`
- M3 read the wrong Z-neighbor byte: detected by `height.corner_values`
- M4 inverted the cell flag: detected by `selection.edges`
- M5 reversed cross-product winding: detected by `cross.operands`
- M6 omitted query-Z negation: detected by `plane.query`
- M7 always selected the h00 base: detected by `plane.base`
- M8 shifted the return by 3 instead of 12: detected by `return.shift`

## Regression and build gates

- W34N62 mode-10 small callbacks, M1–M6: PASS
- W34N65 mode-10 sequence/context callbacks, M1–M11: PASS
- W34N66 mode-10 lifecycle, M1–M8: PASS
- W34C1 scheduler cadence, M1–M21: PASS
- canonical `./pc_port/build_port.sh`: LINK OK; port-owned addresses verified
- `git diff --check`: clean

## Natural runtime result

The detached mode-10 route no longer aborts in
`wm_80079538 -> wm_80093A5C -> wm_800935DC`. Slot 6 executes on every
recurring frame through the 240-frame bound. Captures were fulfilled at
frames 60, 120, 180, and 240, and the bounded world loop returned normally.

| Frame | BMP SHA-256 |
|---:|---|
| 60 | `d6ad8e6825d46f8a2ab0a3898d9b0b6c98d05a62fb6e7fb5f165b7f1c7ca9c12` |
| 120 | `301d988a893db4fe7684a0bbbee7ad06de69edaedc65820f7148a0dc39b968c4` |
| 180 | `b01e0a02b73b55ae17eee9a66d74ea661a9edaa36e88ef8216dedbfe2a328190` |
| 240 | `c92604f6fda34ffb60e1edfc87391dc7f5784dd97575c317227e2c87a8488d8b` |

The images are distinct and show the mode-10 flying craft, ocean, sky, and
animated effect/terrain geometry. The large layered dark geometry is not
visually accepted as plausible without a retail comparison; this rung proves
runtime restoration, not final mode-10 visual parity.

The enclosing application entered its next field state after the bounded
world return and was terminated after all four artifacts were complete. That
post-world termination is not a mode-10 crash.

## Extended natural-exit gate

A second detached run extended the same route to 800 recurring frames. Slots
1 and 2 continued dispatching `0x80078EA4` and `0x80079778` through the final
frame, captures were fulfilled through frame 780, and the world loop reached
only the configured bounded exit:

```text
[worldmap-open-loop] bounded exit frames=800 limit=800
```

The mode-10 state machine did **not** naturally clear `D554`, dispatch the
slot-2 teardown callback `0x80078D24`, or leave the session before the bound.
This is not attributed to the repaired sampler: its recurring slot continues
to execute without the former `wm_800935DC` abort. The next discriminator is
the internal state of slot 2 (`+0x20/+0x22`, position, and velocity) together
with slot 1's `+0x04` latch. Retail slot 2 is the producer that eventually
claims slot 1 through `wm_80097770(1,1)`; the run log proves that the callbacks
remain scheduled, but it does not yet prove which internal transition stalls.

## Remaining acceptance boundary

Natural mode-10 teardown remains unexercised after 800 frames. Visual parity
also remains open pending a retail mode-10 reference or narrower packet
attribution for the layered geometry. W34N67 therefore closes the concrete
`wm_80093A5C` transcription blocker while preserving both lifecycle and visual
questions as separate follow-up work.
