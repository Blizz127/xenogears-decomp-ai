# W34B24-I36 — wm_80096328 Implementation Report (12-byte bank publish)

Ladder after I34 `0x800963E4`. Remaining overlay callee of blocked
71A58 callee `0x80098CC0`. Overlay `MISSING_CALLEES=0` (only
`wm_800963E4`, ACCEPTED I34).

## Identity

| Field | Value |
|---|---|
| Function | `wm_80096328` (12-byte bank sort + ring publish) |
| Boundary | `[0x80096328, 0x800963E4)` = 188 B / 47 insns |
| SHA-256 | `1c3a21b605818951c1122c1bd7fe3c2b54d755207de2b303ead5e5e6a786b5d0` |
| File off | `0x26838` |
| Source | `pc_port/src/world_map_helper_96328.{c,h}` |
| Test | `pc_port/tests/w34b24_i36_96328_prod_test.c` + `run_w34b24_i36_96328.sh` |

Previous `wm_800962B0` ends `jr $ra; nop` at `0x80096320`. This body
restores frame `0x28` and `jr $ra; nop` at `0x800963DC`. Next is
`wm_800963E4`. Overlay JAL: `wm_800963E4`. No JALR / COP2 / GPU / OT.

ABI: `s32(void)`. Bank `*0x8009BE44` of the `0x420`-stride pool at
`*0x8009BE08`. Fail (`-1`) if the first word is 0 or
`*(0x8009D788 + idx*4) != 0`. Else sort via `963E4`, clear
`*0x8009D808`, publish the list pointer into that table slot, and
advance the ring `(idx+1)&0xf`. Both arms write 0 to `0x8009D808`.

## Certification

- Independent retail slice SHA checked by the runner.
- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **8/8 KILLED**.
- Sibling I34 / I35 focused oracles: PASS.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_80096328` @ `00000000004b8536`; no stub shadow; stubs
  **239/524 unchanged**. Remaining assumed: `wm_80097DC0`.
- Fresh clean-export at `/tmp/w34b24-i36-clean-export`: PASS (8/8),
  `world_map.bin` copied as a regular file (180422).
- This helper is not a 71A58 callee. After this leaf,
  `71A58_MISSING_CALLEES` remains 7; `98CC0` still needs `965A4`.

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
`0x80086798` remains unimplemented.
