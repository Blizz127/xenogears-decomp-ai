# W34B24-I28 — wm_80089C78 Implementation Report (71A58 POLY_FT4 submitter)

Ladder after I27 `0x800848F4`. Next bounded missing 71A58 callee
with overlay `MISSING_CALLEES=0` (`93534` ACCEPTED).

## Identity

| Field | Value |
|---|---|
| Function | `wm_80089C78` (256-slot wrap + RotMatrixZ/Scale + RTPT/RTPS + POLY_FT4 OT) |
| Boundary | `[0x80089C78, 0x8008A2C8)` = 1616 B / 404 insns |
| SHA-256 | `693edc23d4e33a80b08c68767a9984fe3ebfc274e980a10693aead42ad5944a2` |
| File off | `0x1A188` |
| Source | `pc_port/src/world_map_helper_89c78.{c,h}` |
| Test | `pc_port/tests/w34b24_i28_89c78_prod_test.c` + `run_w34b24_i28_89c78.sh` |

Previous `wm_80089748` ends `jr $ra; nop`. This body restores frame
`0x50` and `jr $ra; nop`. Next is accepted `wm_8008A2C8`. ABI: void.
Overlay JAL: `wm_80093534`. PsyQ: RotMatrixZ `0x8004B18C`, ScaleMatrix
`0x80049DCC`, ApplyMatrix (retail `0x4A486012` rtv0 cv=none),
SetRotMatrix, SetTransMatrix. COP2: CTC2 camera R, RTPT `0x4A280030`,
RTPS `0x4A180001`, cfc2 FLAG, swc2 SXY/SZ3.

## Certification

- Independent retail slice SHA checked by the runner.
- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **15/15 KILLED**.
- Sibling I13 / I21 / I25 / I26 / I27 focused oracles: PASS.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_80089C78` @ `00000000004b5831`; no stub shadow; stubs
  **239/524 unchanged**. Remaining assumed: `wm_80097DC0`.
- Fresh clean-export at `/tmp/w34b24-i28-clean-export`: PASS (15/15),
  `world_map.bin` copied as a regular file (180422).
- This helper is not a scheduler callback. The live frontier remains
  `0x80071A58`. After this leaf, `71A58_MISSING_CALLEES=9`.

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
`0x80086798` remains unimplemented; overlay store sites to
`0x8009CD40` were measured (see PRE4) but the jalr is still not
treated as a closed implementable set.
