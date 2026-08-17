# W34B24-I35 — wm_800964B0 Implementation Report (16-byte key insertion)

Ladder after I34 `0x800963E4`. Bounded overlay prerequisite of
`0x800965A4`, itself a remaining overlay callee of blocked 71A58
callee `0x80098CC0`. Overlay `MISSING_CALLEES=0`.

## Identity

| Field | Value |
|---|---|
| Function | `wm_800964B0` (16-byte record insertion pass) |
| Boundary | `[0x800964B0, 0x800965A4)` = 244 B / 61 insns |
| SHA-256 | `125fa0364f7974f4a59d5701be4e21fbd671565404ade24bbc4a27857eb28ca6` |
| File off | `0x269C0` |
| Source | `pc_port/src/world_map_helper_964b0.{c,h}` |
| Test | `pc_port/tests/w34b24_i35_964b0_prod_test.c` + `run_w34b24_i35_964b0.sh` |

Previous `wm_800963E4` ends `jr $ra; nop` at `0x800964A8`. This body
restores frame `0x10` and `jr $ra; nop` at `0x8009659C`. Next is
`wm_800965A4`. No JAL / JALR / COP2 / GPU / OT.

ABI: `void(a0)`. `$a0` = list of 16-byte records. Key is word `+4`
(`lw -12(a1)` vs `lw 4(a1)` with `a1 = cur+16`). Terminator is word
`+0` of the next record. Compare is unsigned `sltu`. Adjacent
records swap all 16 bytes; after a swap, if `start < current` both
pointers step `-16`, else on no-swap they step `+16`. Loop while
`lw(next) != 0`.

## Certification

- Independent retail slice SHA checked by the runner.
- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **4/4 KILLED** (`SKIP_EMPTY`, `SIGNED_CMP`, `WRONG_KEY`,
  `WRONG_BACK`).
- Sibling I32 / I33 / I34 focused oracles: PASS.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_800964B0` @ `00000000004b83fd`; no stub shadow; stubs
  **239/524 unchanged**. Remaining assumed: `wm_80097DC0`.
- Fresh clean-export at `/tmp/w34b24-i35-clean-export`: PASS (4/4),
  `world_map.bin` copied as a regular file (180422).
- This helper is not a 71A58 callee. After this leaf,
  `71A58_MISSING_CALLEES` remains 7; `965A4` becomes implementable.

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
`0x80086798` remains unimplemented.
