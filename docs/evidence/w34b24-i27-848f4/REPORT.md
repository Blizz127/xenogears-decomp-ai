# W34B24-I27 — wm_800848F4 Implementation Report (71A58 region submitter)

Ladder after I26 `0x80085CDC`. Next bounded missing 71A58 callee
with overlay `MISSING_CALLEES=0` (`93534` ACCEPTED).

## Identity

| Field | Value |
|---|---|
| Function | `wm_800848F4` (region MATRIX compose + RTPS + model submit) |
| Boundary | `[0x800848F4, 0x80084D00)` = 1036 B / 259 insns |
| SHA-256 | `670f809b7d9c0bcbd32463495e24c057bda8469220f12f3d6de19b893830e93b` |
| File off | `0x14E04` |
| Source | `pc_port/src/world_map_helper_848f4.{c,h}` |
| Test | `pc_port/tests/w34b24_i27_848f4_prod_test.c` + `run_w34b24_i27_848f4.sh` |

Sandwiched by accepted `wm_800848B4` and `wm_80084D00`. ABI: void.
Overlay JAL: `wm_80093534`. PsyQ: ScaleMatrix, CompMatrix, SetRotMatrix,
SetTransMatrix, ApplyMatrixSV, RotTrans. SLUS: `func_8002C700`.
COP2: RTIR `0x4A49E012` ×3, RotTrans `0x4A480012`, RTPS + FLAG + SZ3.

## Certification

- Independent retail slice SHA checked by the runner.
- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **10/10 KILLED**.
- Sibling I13 / I21 / I25 / I26 focused oracles: PASS.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_800848F4` @ `00000000004b4d78`; no stub shadow; stubs
  **239/524 unchanged**.
- Fresh clean-export at `/tmp/w34b24-i27-clean-export`: PASS (10/10),
  `world_map.bin` copied as a regular file (180422).
- This helper is not a scheduler callback. The live frontier remains
  `0x80071A58`. After this leaf, `71A58_MISSING_CALLEES=10`.

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
`0x80086798` remains a genuine jalr blocker on the 71A58 tail.
