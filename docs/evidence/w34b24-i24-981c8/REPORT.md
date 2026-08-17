# W34B24-I24 — wm_800981C8 Implementation Report (71A58 8x8 index leaf)

Ladder after I23 `0x800980D4`. Next bounded missing 71A58 callee
with `MISSING_CALLEES=0` (no JALs). Replaces the prior assumed-function
stub `wm_800981C8`.

## Identity

| Field | Value |
|---|---|
| Function | `wm_800981C8` (copy + 8×8 wrapped cell-index fill) |
| Boundary | `[0x800981C8, 0x800983A0)` = 472 B / 118 insns |
| SHA-256 | `90e483f10929f9022a21582a9d8046cb978c9cf09927d848580f8a427905a6fe` |
| File off | `0x286D8` |
| Source | `pc_port/src/world_map_helper_981c8.{c,h}` |
| Test | `pc_port/tests/w34b24_i24_981c8_prod_test.c` + `run_w34b24_i24_981c8.sh` |

ABI: `$a0` position (s32 x/z). Void. Cell coords from `(x>>12)` toward-zero
`/8` minus `(lh cell+2)<<8`, wrap once by `dim<<8`, then `>>8`. Copies
0xA2 bytes `0x8009D570` → `0x8009D318`, then writes an 8×8 halfword
grid at `0x8009D570` (`row*dim_x + col`, wrapping each axis).

## Certification

- Independent retail slice SHA checked by the runner.
- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **11/11 KILLED**.
- Sibling I13 / I20 / I21 / I22 / I23 focused oracles: PASS.
- Build: LINK OK ×2; exactly one strong `T wm_800981C8` @
  `00000000004b383c`; no stub shadow; stubs **239/524** (was 240/524;
  assumed `wm_800981C8` removed). Remaining assumed: `wm_80097DC0`.
- Fresh clean-export at `/tmp/w34b24-i24-clean-export`: PASS (11/11),
  `world_map.bin` copied as a regular file (180422).
- This helper is not a scheduler callback. The live frontier remains
  `0x80071A58`. After this leaf, `71A58_MISSING_CALLEES=13`.

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
