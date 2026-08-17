# W34B24-I25 — wm_800737EC Implementation Report (71A58 four-quad submitter)

Ladder after I24 `0x800981C8`. Next bounded missing 71A58 callee
with overlay `MISSING_CALLEES=0` (PsyQ only).

## Identity

| Field | Value |
|---|---|
| Function | `wm_800737EC` (four-quad RotTransPers4 + OT insert) |
| Boundary | `[0x800737EC, 0x800739B8)` = 460 B / 115 insns |
| SHA-256 | `e967f7509aa004e89b3876a53b9673a0bac961055e2f298012d8abf01be9d621` |
| File off | `0x3CFC` |
| Source | `pc_port/src/world_map_helper_737ec.{c,h}` |
| Test | `pc_port/tests/w34b24_i25_737ec_prod_test.c` + `run_w34b24_i25_737ec.sh` |

ABI: void. `RotMatrixYXZ((0, lh 0x8009BD3A, 0))`, clear R.t,
`CompMatrix(0x8009C808, R, C)`, `SetRot`/`SetTrans`, then four
`RotTransPers4` quads from `0x8009A280` into
`0x8009D194 + index*36 + quad*0x48`. Insert when flag ≥ 0 using
`srav(otz, *0x80050100)` into `*(*0x8009BE3C+0x70)`.

## Certification

- Independent retail slice SHA checked by the runner.
- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **12/12 KILLED**.
- Sibling I13 / I21 / I22 / I23 / I24 focused oracles: PASS.
- Build: LINK OK ×2; exactly one strong `T wm_800737EC` @
  `00000000004b3c51`; no stub shadow; stubs **239/524 unchanged**.
- Fresh clean-export at `/tmp/w34b24-i25-clean-export`: PASS (12/12),
  `world_map.bin` copied as a regular file (180422).
- This helper is not a scheduler callback. The live frontier remains
  `0x80071A58`. After this leaf, `71A58_MISSING_CALLEES=12`.

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
`0x80086798` remains a genuine jalr blocker on the 71A58 tail.
