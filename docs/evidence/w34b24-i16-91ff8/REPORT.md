# W34B24-I16 — wm_80091FF8 Implementation Report (91C18 probe helper)

Ladder after G5 96F18. Base: canonical
`91af2097c55016d6211b7fd9c14b1d95fa406904`. Live frontier is missing
slot-10 cb1 `0x80091C18`. This helper is one of its two overlay JALs;
the other (`0x80096F18`) is already accepted.

## Identity

| Field | Value |
|---|---|
| Function | `wm_80091FF8` (6×6 height probe / heading walk) |
| Boundary | `[0x80091FF8, 0x80092234)` = 572 B / 143 insns |
| SHA-256 | `9e3c8c4ceabb5b9f2d9bc213857d21570bd968ea7cc2b6fea95883b36d2c93f9` |
| File off | `0x22508` |
| Source | `pc_port/src/world_map_helper_91ff8.{c,h}` |
| Test | `pc_port/tests/w34b24_i16_91ff8_prod_test.c` + `run_w34b24_i16_91ff8.sh` |

ABI: `$a0` threshold index, `$a1` heading halfword table, `$a2` height
halfword table. `$v0` = walk index, or `$a0` when
`abs(B234[a0] + (D560>>12) - ((min<<3)-0x50)) < 0x41`.

JALs: `96F18` (accepted), `93354` (accepted), `93660` (accepted).
MISSING_CALLEES=0. No JALR / COP2 / GPU / OT.

Min height `s4` starts at 0 each outer (only samples `< 0` update).
BCDC uses toward-zero `/2` (`srl 31; addu; sra 1`) then `sll 12`.
Probe origin masked with `0xFFF80000`. Grid step `0x80000`, 6×6.

## Certification

- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan
  fallback). Signed `sll` is done on the register bit pattern.
- Mutants: **14/14 KILLED**.
- Sibling I14 / I15 focused oracles: PASS.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_80091FF8` @ `00000000004b1792`; no stub shadow; stubs
  **240/524 unchanged**.
- Fresh clean-export at `/tmp/w34b24-i16-clean-export`: PASS (14/14),
  `world_map.bin` copied as a regular file (180422).
- This helper is not a scheduler callback. The live frontier remains
  `0x80091C18`, now MISSING=0 (`91FF8`, `96F18`).

`0x80071A58` was not implemented (not on the live path).
