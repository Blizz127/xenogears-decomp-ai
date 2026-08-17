# W34B24-I22 — wm_80097244 Implementation Report (71A58 warm-arm camera)

Ladder after I21 `0x80097440`. Base: worktree `w34-g5-80097244` on
`822b40253631f04c477416da8eb22cef1ccbecfc` plus I21. Live frontier is
missing slot-14 cb1 `0x80071A58`. This is the warm-arm sibling of 97440
(`*0x8009D144 != 0`).

## Identity

| Field | Value |
|---|---|
| Function | `wm_80097244` (camera-from-look; 71A58 warm arm) |
| Boundary | `[0x80097244, 0x80097440)` = 508 B / 127 insns |
| SHA-256 | `2d0e27a454a833ee8c31bc741150c0ec4a6491fd932046bbed8646628fb341f5` |
| File off | `0x27754` |
| Source | `pc_port/src/world_map_helper_97244.{c,h}` |
| Test | `pc_port/tests/w34b24_i22_97244_prod_test.c` + `run_w34b24_i22_97244.sh` |

ABI: `$a0` look record (SVECTOR +0, SVECTOR +8, VECTOR +0x10). Void.
Delta look−eye → `VectorNormal` N1 → `OuterProduct12(N1, up)` → N2 →
`OuterProduct12(N1, N2)` → N3. Pack N2/N3/N1 as s16 rows of MATRIX
`0x8009C808`. Negated-eye `ApplyMatrix` + `TransMatrix`. PsyQ only.
MISSING_CALLEES=0. Next overlay function is accepted-target 97440.

## Certification

- Independent retail slice SHA checked by the runner.
- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **12/12 KILLED**.
- Sibling I13 / I15 / I19 / I20 / I21 focused oracles: PASS.
- Suites: I9–I12, I14, I16–I18 PASS.
- Build: LINK OK ×2; exactly one strong `T wm_80097244` @
  `00000000004b2ef0`; no stub shadow; stubs **240/524 unchanged**.
- Fresh clean-export at `/tmp/w34b24-i22-clean-export`: PASS (12/12),
  `world_map.bin` copied as a regular file (180422).
- This helper is not a scheduler callback. The live frontier remains
  `0x80071A58`. After this leaf, `71A58_MISSING_CALLEES=15`.

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
