# W34N41 — complete retail `wm_80099BFC` wrapped-entry FT4 submitter

Verdict: **FULL_BODY_TRANSCRIBED_AND_CERTIFIED**.

## Anchor and authority

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `f7250a90efd0dd55d9c39ae0e38eb0c07514bd70`
- Retail authority:
  `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`, function
  `[0x80099BFC,0x80099E88)`
- Caller authority: retail `0x80086358..0x80086398`
- Date: 2026-08-30

The caller proves the retail four-argument contract: `a0` is the source-entry
pointer, `a1` its count, `a2` the active OT base, and `a3` the compacted FT4
packet cursor.

## Retired approximation

The former one-argument C body only wrapped coordinates in place. Retail does
not modify the source records. It wraps camera-relative coordinates in
registers, performs a two-matrix GTE projection, applies FLAG/screen/depth
gates, selects a depth-cue CLUT through IR0, and publishes accepted `0x28`-byte
FT4 packets. The former signature was therefore incompatible with its retail
caller as well as incomplete.

## Retail body now implemented

For each `0x08` source record, until the accepted-output count reaches `0x200`,
the helper now:

1. extracts signed X/Y/Z, subtracts the camera position, and applies retail's
   `[-0x4000,0x4000)` wrapping using `D_8009D160/D2B4`;
2. rotates `(x,y,-z)` through the camera matrix at scratch `+0x28`, adds its
   translation with MIPS-addu semantics, and installs that translation on the
   model matrix at scratch `+0x48`;
3. projects the first three shared vertices, rejects signed GTE FLAG failures,
   requires independent X/Y screen overlap, and rejects `SZ3 >= 0x0E00`;
4. projects the fourth vertex, clamps unsigned IR0 to `0x0FFF`, and selects a
   CLUT from the 16-halfword table at scratch `+0x68`;
5. writes four XY values, links a length-9 packet into `SZ3 >> 4`, advances the
   packet cursor only on acceptance, and publishes the compacted count at
   `0x8009BE04`.

All signed arithmetic that retail performs with MIPS wrapping or arithmetic
shift is spelled without relying on C signed overflow or implementation-defined
right shift.

## Focused certificate

`pc_port/tests/run_w34n41_99bfc.sh` passes O0, O2, and nonrecovering UBSan with
focused warnings clean. Its six-entry route independently exercises one signed
FLAG reject, one screen reject, one accepted depth between `0x0C00` and
`0x0E00`, one `0x0E00` depth reject, two ordinary accepts, compacted packet
addresses, a repeated OT bucket, and an IR0 value above `0x0FFF`. A second
scenario begins at output count 511 and proves the exact 512-packet ceiling.

Nine named mutants are detected:

| mutant | failure mode |
|---|---|
| M1 | use a 256-packet ceiling |
| M2 | omit camera subtraction |
| M3 | fail to negate wrapped Z |
| M4 | omit signed FLAG rejection |
| M5 | omit screen-overlap rejection |
| M6 | use a `0x0C00` depth ceiling |
| M7 | omit IR0 saturation |
| M8 | omit OT publication |
| M9 | advance packets by `0x24` |

## Build and natural regression

The normal product build reports `LINK OK`. The accepted detached route used:

```text
XENO_TEST_INPUT=0:0x2000,600:0x4000,916:0x2000,976:0
XENO_WORLD_TEST_INPUT=0:0x2000,600:0x20,601:0
```

All three standing capture digests remain byte-identical:

```text
frame 60  4310197d3881367bc7859ef174dcf4b9afa8759f0b7dc2ca3eea94b0f84f1521
frame 120 8039c2f96b88e09491eaf1cba049d0efd829f146aa943da744f56c93e85b1c16
frame 600 2b636ae21af4bf0003f60d5e53f8ffd9a042fc5a72457e4c096cff19814e1a38
```

Capture fulfillment remained same-frame, no OT/adapter abort was logged, and
the unseeded route again reached:

```text
[worldmap-open-loop] natural state exit frames=601 D7CC=0
```

## Bound and next dependency

This function is now a complete, callable dependency, but the current
approximate `wm_8008615C` still omits its retail 5x5 dispatch loop. Runtime
neutrality is therefore expected. The next bounded integration target is the
full `[0x8008615C,0x800863E0)` caller, which must use this four-argument
contract rather than restore the former one-argument approximation.
