# W34B24-I42 — wm_80096130 Implementation Report (D788/C624 wait)

Ladder after I41 `0x800967E4`. This is a 71A58 overlay callee.
Overlay `MISSING_CALLEES=0`.

## Identity

| Field | Value |
|---|---|
| Function | `wm_80096130` (wait until published bank slot is 0) |
| Boundary | `[0x80096130, 0x8009623C)` = 268 B / 67 insns |
| SHA-256 | `92c4c23f0e129166ce0c21cca5431851a5b3c6662ce293682e2b947c24562bc0` |
| File off | `0x26640` |
| Source | `pc_port/src/world_map_helper_96130.{c,h}` |
| Test | `pc_port/tests/w34b24_i42_96130_prod_test.c` + `run_w34b24_i42_96130.sh` |

This body restores frame `0x28` and `jr $ra; nop` at `0x80096234`.
Next is `wm_8009623C`. Overlay JAL: `wm_800967E4`. PsyQ: `Vsync`
`0x8004B54C`. SLUS: `func_8002C3D8` ×2. No JALR / COP2 / GPU / OT.

ABI: `void`. Same debug-table predicate as `967E4`. CD path polls
`D788[*0x8009BE44]`; C624 path polls `C624[*0x8009BE44]`. Each
occupied iteration is `Vsync(0)` then `967E4()`. The index is
reloaded from `*0x8009BE44` every check.

## Certification

- Independent retail slice SHA checked by the runner.
- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **4/4 KILLED**.
- Sibling I41 focused oracle: PASS.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_80096130` @ `00000000004b8f26`; no stub shadow; stubs
  **239/524 unchanged**. Remaining assumed: `wm_80097DC0`.
- Fresh clean-export at `/tmp/w34b24-i42-clean-export`: PASS (4/4),
  `world_map.bin` copied as a regular file (180422).
- After this leaf, `71A58_MISSING_CALLEES=6`.

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
`0x80086798` remains unimplemented.
