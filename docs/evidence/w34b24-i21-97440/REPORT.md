# W34B24-I21 — wm_80097440 Implementation Report (71A58 cold-arm camera)

Ladder after G5 92FD8. Base: canonical
`822b40253631f04c477416da8eb22cef1ccbecfc`. Live frontier is missing
slot-14 cb1 `0x80071A58` with MISSING=17. First implementable
prerequisite is this PsyQ-only leaf (PRE4).

## Identity

| Field | Value |
|---|---|
| Function | `wm_80097440` (camera-from-angles; 71A58 cold arm) |
| Boundary | `[0x80097440, 0x8009766C)` = 556 B / 139 insns |
| SHA-256 | `6bce51ba8bb3a013e923fcc2b66b1bc59646522ac76aad21923171cb89ddfd4b` |
| File off | `0x27950` |
| Source | `pc_port/src/world_map_helper_97440.{c,h}` |
| Test | `pc_port/tests/w34b24_i21_97440_prod_test.c` + `run_w34b24_i21_97440.sh` |

ABI: `$a0` eye SVECTOR. Void. Copies MATRIX `0x8009A180` to three
scratch slots, `RotMatrixX/Y/Z(-lh 0x8009BD38/3A/3C)`, `MulMatrix0` into
`0x8009C808`, negated-eye `ApplyMatrix`, `TransMatrix`. PsyQ only.
MISSING_CALLEES=0. Next overlay function starts at `0x8009766C`.

## Certification

- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **13/13 KILLED**.
- Sibling I13 / I15 / I19 / I20 focused oracles: PASS.
- Suites: I9–I12, I14, I16–I18 PASS. Older I1–I8 HOST `libubsan.so.1.0.0`
  / `-fpermissive` under host gcc — not a 97440 regression.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_80097440` @ `00000000004b2b51`; no stub shadow; stubs
  **240/524 unchanged**.
- Fresh clean-export at `/tmp/w34b24-i21-clean-export`: PASS (13/13),
  `world_map.bin` copied as a regular file (180422).
- This helper is not a scheduler callback. The live frontier remains
  `0x80071A58`. After this leaf, `71A58_MISSING_CALLEES=16`.

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
