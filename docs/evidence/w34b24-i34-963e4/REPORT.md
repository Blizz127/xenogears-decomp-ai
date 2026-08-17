# W34B24-I34 — wm_800963E4 Implementation Report (12-byte key insertion)

Ladder after I33 `0x800962B0`. Bounded overlay prerequisite of
`0x80096328`, itself a remaining overlay callee of blocked 71A58
callee `0x80098CC0`. Overlay `MISSING_CALLEES=0`.

## Identity

| Field | Value |
|---|---|
| Function | `wm_800963E4` (12-byte record insertion pass) |
| Boundary | `[0x800963E4, 0x800964B0)` = 204 B / 51 insns |
| SHA-256 | `a51c358b5980c6b51328dbeead5d3ff963b162c5e708dedd342090becddd9edc` |
| File off | `0x268F4` |
| Source | `pc_port/src/world_map_helper_963e4.{c,h}` |
| Test | `pc_port/tests/w34b24_i34_963e4_prod_test.c` + `run_w34b24_i34_963e4.sh` |

Previous `wm_80096328` ends `jr $ra; nop` at `0x800963DC`. This body
restores frame `0x10` and `jr $ra; nop` at `0x800964A8`. Next is
`wm_800964B0`. No JAL / JALR / COP2 / GPU / OT.

ABI: `void(a0)`. `$a0` = list of 12-byte records. Key is word `+0`.
Terminator is a zero key on the next record (`lw list+12 == 0`
early-out). Compare is unsigned `sltu`. Adjacent records swap all
12 bytes; after a swap, if `start < current` both pointers step
`-12`, else on no-swap they step `+12`. Loop while `lw(next) != 0`.

## Certification

- Independent retail slice SHA checked by the runner.
- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **4/4 KILLED** (`SKIP_EMPTY`, `SIGNED_CMP`, `WRONG_KEY`,
  `WRONG_BACK`).
- Sibling I31 / I32 / I33 focused oracles: PASS.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_800963E4` @ `00000000004b81c1`; no stub shadow; stubs
  **239/524 unchanged**. Remaining assumed: `wm_80097DC0`.
- Fresh clean-export at `/tmp/w34b24-i34-clean-export`: PASS (4/4),
  `world_map.bin` copied as a regular file (180422).
- This helper is not a 71A58 callee. After this leaf,
  `71A58_MISSING_CALLEES` remains 7; `96328` becomes implementable.

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
`0x80086798` remains unimplemented.
