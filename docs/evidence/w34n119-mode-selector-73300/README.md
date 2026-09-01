# W34N119 — retail world submode selector `0x80073300`

## Result

`RETAIL_SUBMODE_SELECTOR_RESTORED`.

Starting HEAD was `c85169333dfb2e6248200b4bd366b16170a26d02` on
`experiment/worldmap-open-gates-20260823`, equal to origin.  W34N118 made the
zero-entry route reach the exact next divergence: cold control word `0x4003`
entered `wm_80073300`, where the port logged an error and substituted mode 1.

Retail authority is `disc/world_map.bin`.  The exact 152-byte function range
`[0x80073300,0x80073398)` has SHA-256
`6276fa42750763b6851a06a656a8a298a1a982989261d2f235754894bea81a36`.

## Branch transcription

The port now implements the complete retail selector:

| condition | result at `0x8009BE10` |
|---|---:|
| bit `0x4000` clear, all `F8E5..F8E7` zero | 1 |
| bit `0x4000` clear, any flag nonzero | 2 |
| index 0 | no write |
| index 1 | 4 |
| index 2 | 5 |
| index 3 | 7 |
| index 4 | 7 |
| index >= 5 | no write |

The index is masked with `0x1FFF`, matching the retail delay-slot operation.
The former inline approximation was replaced by the independently compiled
`world_map_mode_selector_73300.c` body.

## Certificate and build

`pc_port/tests/run_w34n119_mode_selector.sh` passes under O0, O2, and
nonrecovering UBSan with strict warnings.  It covers the full branch table,
no-write arms, high-bit masking, and read-only inputs.  M1-M5 are detected by
named assertions: cold index 3 mapped to 1, inverted flag result, index-0
write, out-of-range write, and unmasked index.

The W34N118 certificate was rerun unchanged and remains green.  An isolated
staged-tree build at anonymous verification commit
`9f8bfffdd722d01ecabb1995ddee57ad075ccc2c` ended with `LINK OK`.

## Natural cold-entry acceptance

The same detached zero-entry route now contains no `wm_80073300` error.  It
completed 120 displayed frames, fulfilled both captures in-frame, and reached
the bounded exit with zero world-map stub hits and zero OT-adapter aborts.
Correct mode selection legitimately changed both W34N118 images:

| frame | SHA-256 |
|---:|---|
| 60 | `317a339ec91020f13d9e44c53e600e4b2266760b9750684f91bd15ee8cfd5332` |
| 120 | `c94d9e9e17904cdd53c8a51b34359af82f0b8cc2b48927e454624edcf7dfc714` |

Both are coherent terrain/minimap views.  Artifacts remain under
`scratchpad/w34n119_cold_capture/`.

## Nineteen-record callback census

A clean pre-change binary (the selector change is dormant on every observed
non-cold arm) ran entrance records 1 through 18 for 120 displayed frames each.
Every run reached its bounded exit with:

- zero world-map callback stub hits;
- zero OT-adapter aborts;
- zero setup/runtime errors.

None logged the former bit-`0x4000` approximation error.  After the exact
selector build, a separate `entrance=0x8000` run normalized to mode-table
record 0 and likewise reached 120 frames with zero stubs, aborts, or errors.
The literal zero entrance is the cold initializer and correctly normalizes to
record 1 instead of selecting record 0.

This closes the live mode-table/callback frontier across all 19 retail records
under the maintained bounded route.  It does not claim retail pixel parity for
all special modes, nor natural reachability of every record from gameplay.

## Next target

Select the next world-map target from route-wide non-callback incompleteness:
generated function stubs, explicit partial retail ranges, or remaining visual
divergence.  Do not continue decoding mode-local callbacks without a new live
witness; the callback census is clean.
