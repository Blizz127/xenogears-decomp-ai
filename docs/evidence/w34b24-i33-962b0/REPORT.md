# W34B24-I33 — wm_800962B0 Implementation Report (98CC0 16-byte enqueue)

Ladder after I32 `0x8009623C`. Bounded overlay prerequisite of
blocked 71A58 callee `0x80098CC0`. Overlay `MISSING_CALLEES=0`.

## Identity

| Field | Value |
|---|---|
| Function | `wm_800962B0` (16-byte queue push; cap 88) |
| Boundary | `[0x800962B0, 0x80096328)` = 120 B / 30 insns |
| SHA-256 | `ecc6b8ff0a942d007e4b5eaa9c89ec3834d62960d9c1736425f4a2006785725c` |
| File off | `0x267C0` |
| Source | `pc_port/src/world_map_helper_962b0.{c,h}` |
| Test | `pc_port/tests/w34b24_i33_962b0_prod_test.c` + `run_w34b24_i33_962b0.sh` |

Previous `wm_8009623C` ends `jr $ra; nop` at `0x800962A8`. This body
is `jr $ra; nop` at `0x80096320`. Next is `wm_80096328`. No JAL /
JALR / COP2 / GPU / OT.

ABI: `s32(a0,a1,a2,a3)`. Returns `-1` if `*0x8009D808 >= 88`, else
stores four words at
`*0x8009D3C0 + *0x8009BE44 * 0x580 + count * 16` and increments
the counter.

## Certification

- Independent retail slice SHA checked by the runner.
- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **7/7 KILLED**.
- Sibling I28 / I30 / I31 / I32 focused oracles: PASS.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_800962B0` @ `00000000004b7f7f`; no stub shadow; stubs
  **239/524 unchanged**. Remaining assumed: `wm_80097DC0`.
- Fresh clean-export at `/tmp/w34b24-i33-clean-export`: PASS (7/7),
  `world_map.bin` copied as a regular file (180422).
- This helper is not a 71A58 callee. After this leaf,
  `71A58_MISSING_CALLEES` remains 7; `98CC0` still needs `96328` /
  `965A4` (those need `963E4` / `964B0`).

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
`0x80086798` remains unimplemented.
