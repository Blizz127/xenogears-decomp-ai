# W34B24-I40 — wm_8009699C Implementation Report (D788 CD start)

Ladder after I39. Last overlay callee of `0x800967E4`. Overlay
`MISSING_CALLEES=0`.

## Identity

| Field | Value |
|---|---|
| Function | `wm_8009699C` (D788 record → CdSetloc) |
| Boundary | `[0x8009699C, 0x80096A6C)` = 208 B / 52 insns |
| SHA-256 | `2c50a66cb2fe0f79f8dad79a6ea7d759bae43ed65b97635e4e8113e686b88a42` |
| File off | `0x26EAC` |
| Source | `pc_port/src/world_map_helper_9699c.{c,h}` |
| Test | `pc_port/tests/w34b24_i40_9699c_prod_test.c` + `run_w34b24_i40_9699c.sh` |

Previous `wm_800968E0` ends `jr $ra; nop` at `0x80096994`. This body
restores frame `0x18` and `jr $ra; nop` at `0x80096A64`. Next is
CdSync handler `0x80096A6C`. PsyQ: `CdIntToPos` `0x80041430`,
`CdSyncCallback` `0x80040FB4`, `CdControlF` `0x8004111C`. No overlay
JAL / JALR / COP2 / GPU / OT.

ABI: `void(a0)`. 12-byte record: `+0` file_id, `+4` byte_count,
`+8` dest. Sets CD44=1, D3BC=record+12 (second store wins),
clears BE48/CCB0/CCA8/CCA0, D56C=`(count+0x7FF)>>11`, then
`CdIntToPos(file_id, 0x8009CEBC)`, `CdSyncCallback(0x80096A6C)`,
`CdControlF(2, 0x8009CEBC)`.

## Certification

- Independent retail slice SHA checked by the runner.
- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **5/5 KILLED**.
- Sibling I38 / I39 focused oracles: PASS.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_8009699C` @ `00000000004b8bca` (W29B
  `wm_8009699C_d788_processor` remains a separate symbol); no stub
  shadow; stubs **239/524 unchanged**. Remaining assumed:
  `wm_80097DC0`.
- Fresh clean-export at `/tmp/w34b24-i40-clean-export`: PASS (5/5),
  `world_map.bin` copied as a regular file (180422).
- This helper is not a 71A58 callee. After this leaf, `967E4`
  overlay callees are closed.

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
`0x80086798` remains unimplemented.
