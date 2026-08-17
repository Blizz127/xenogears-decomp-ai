# W34B24-I32 — wm_8009623C Implementation Report (98CC0 12-byte enqueue)

Ladder after I31 `0x80089748`. Bounded overlay prerequisite of
blocked 71A58 callee `0x80098CC0`. Overlay `MISSING_CALLEES=0`.

## Identity

| Field | Value |
|---|---|
| Function | `wm_8009623C` (12-byte queue push; cap 88) |
| Boundary | `[0x8009623C, 0x800962B0)` = 116 B / 29 insns |
| SHA-256 | `96b15f7b4ebaf3a214643c751b8cdd824dd3a7e9bd3e07e5db91ba4efe13e7f0` |
| File off | `0x2674C` |
| Source | `pc_port/src/world_map_helper_9623c.{c,h}` |
| Test | `pc_port/tests/w34b24_i32_9623c_prod_test.c` + `run_w34b24_i32_9623c.sh` |

Previous `wm_80096130` ends `jr $ra; nop` at `0x80096234`. This body
is `jr $ra; nop` at `0x800962A8`. Next is `wm_800962B0`. No JAL /
JALR / COP2 / GPU / OT.

ABI: `s32(a0,a1,a2)`. Returns `-1` if `*0x8009D808 >= 88`, else
stores three words at
`*0x8009BE08 + *0x8009BE44 * 0x420 + count * 12` and increments
the counter.

## Certification

- Independent retail slice SHA checked by the runner.
- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **7/7 KILLED**.
- Sibling I28 / I30 / I31 focused oracles: PASS.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_8009623C` @ `00000000004b7e1b`; no stub shadow; stubs
  **239/524 unchanged**. Remaining assumed: `wm_80097DC0`.
- Fresh clean-export at `/tmp/w34b24-i32-clean-export`: PASS (7/7),
  `world_map.bin` copied as a regular file (180422).
- This helper is not a 71A58 callee. After this leaf,
  `71A58_MISSING_CALLEES` remains 7; `98CC0` still needs `962B0` /
  `96328` / `965A4`.

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
`0x80086798` remains unimplemented.
