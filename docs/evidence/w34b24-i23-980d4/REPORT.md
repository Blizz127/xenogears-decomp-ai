# W34B24-I23 — wm_800980D4 Implementation Report (71A58 wrap/cell leaf)

Ladder after I22 `0x80097244`. Next bounded missing 71A58 callee
with `MISSING_CALLEES=0` (no JALs).

## Identity

| Field | Value |
|---|---|
| Function | `wm_800980D4` (X/Z wrap at ±0x800000 + cell publish) |
| Boundary | `[0x800980D4, 0x800981C8)` = 244 B / 61 insns |
| SHA-256 | `b2ad8897db6a2b556897d0e5c726d049e252e78907dedac61642d121d7d90b12` |
| File off | `0x285E4` |
| Source | `pc_port/src/world_map_helper_980d4.{c,h}` |
| Test | `pc_port/tests/w34b24_i23_980d4_prod_test.c` + `run_w34b24_i23_980d4.sh` |

ABI: `$a0` position (s32 x at +0, s32 z at +8). Void. Clears
`0x8009D558`, wraps each axis once at ±0x00800000 (X sets 4/8; Z ORs
1/2), then `sh` `(sra 23)+2` to `0x8009C838` / `0x8009C83C`.

## Certification

- Independent retail slice SHA checked by the runner.
- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **11/11 KILLED**.
- Sibling I13 / I15 / I19 / I20 / I21 / I22 focused oracles: PASS.
- Build: LINK OK ×2; exactly one strong `T wm_800980D4` @
  `00000000004b3334`; no stub shadow; stubs **240/524 unchanged**.
- Fresh clean-export at `/tmp/w34b24-i23-clean-export`: PASS (11/11),
  `world_map.bin` copied as a regular file (180422).
- This helper is not a scheduler callback. The live frontier remains
  `0x80071A58`. After this leaf, `71A58_MISSING_CALLEES=14`.

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
