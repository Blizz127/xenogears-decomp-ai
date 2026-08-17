# W34B24-I30 — wm_80089580 Implementation Report (89748 particle integrator)

Ladder after I29 `0x800740B8`. Bounded overlay prerequisite of
blocked 71A58 callee `0x80089748`. Overlay `MISSING_CALLEES=0`.

## Identity

| Field | Value |
|---|---|
| Function | `wm_80089580` (256-slot life/integrate/RGB/death) |
| Boundary | `[0x80089580, 0x80089748)` = 456 B / 114 insns |
| SHA-256 | `7bd1f53426cc512dfcdb86212a6eba7c9107100daa0fe6b6f9d41183b800b834` |
| File off | `0x19A90` |
| Source | `pc_port/src/world_map_helper_89580.{c,h}` |
| Test | `pc_port/tests/w34b24_i30_89580_prod_test.c` + `run_w34b24_i30_89580.sh` |

Previous body ends `jr $ra; nop` at `0x80089578`. This body restores
frame `0x10` and `jr $ra; nop` at `0x80089740`. Next is
`wm_80089748`. No JAL / JALR / COP2 / GPU / OT.

ABI: void. `89748` jal delay is `nop`; `$a0` unused.

## Certification

- Independent retail slice SHA checked by the runner.
- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **11/11 KILLED**.
- Sibling I29 `0x800740B8` focused oracle: PASS.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_80089580` @ `00000000004b6ba9`; no stub shadow; stubs
  **239/524 unchanged**. Remaining assumed: `wm_80097DC0`.
- Fresh clean-export at `/tmp/w34b24-i30-clean-export`: PASS (11/11),
  `world_map.bin` copied as a regular file (180422).
- This helper is not a 71A58 callee. After this leaf,
  `71A58_MISSING_CALLEES` remains 8; `89748` is now bounded.

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
`0x80086798` remains unimplemented.
