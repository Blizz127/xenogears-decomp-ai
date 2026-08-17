# W34B24-I37 — wm_800965A4 Implementation Report (16-byte bank publish)

Ladder after I35 `0x800964B0` and I36 `0x80096328`. Last remaining
overlay callee of blocked 71A58 callee `0x80098CC0`. Overlay
`MISSING_CALLEES=0` (only `wm_800964B0`, ACCEPTED I35).

## Identity

| Field | Value |
|---|---|
| Function | `wm_800965A4` (16-byte bank sort + ring publish) |
| Boundary | `[0x800965A4, 0x80096668)` = 196 B / 49 insns |
| SHA-256 | `6393023f3f0436ba14cc3178433ed15c867bbcebf33540483856ee95ae964ab7` |
| File off | `0x26AB4` |
| Source | `pc_port/src/world_map_helper_965a4.{c,h}` |
| Test | `pc_port/tests/w34b24_i37_965a4_prod_test.c` + `run_w34b24_i37_965a4.sh` |

Previous `wm_800964B0` ends `jr $ra; nop` at `0x8009659C`. This body
restores frame `0x28` and `jr $ra; nop` at `0x80096660`. Next is
`wm_80096668`. Overlay JAL: `wm_800964B0`. No JALR / COP2 / GPU / OT.

ABI: `s32(void)`. Bank `*0x8009BE44` of the `0x580`-stride pool at
`*0x8009D3C0`. Fail (`-1`) if the first word is 0 or
`*(0x8009C624 + idx*4) != 0`. Else sort via `964B0`, clear
`*0x8009D808`, publish the list pointer into that table slot, and
advance the ring `(idx+1)&0xf`. Both arms write 0 to `0x8009D808`.

## Certification

- Independent retail slice SHA checked by the runner.
- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **8/8 KILLED**.
- Sibling I34 / I35 / I36 focused oracles: PASS.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_800965A4` @ `00000000004b86ad`; no stub shadow; stubs
  **239/524 unchanged**. Remaining assumed: `wm_80097DC0`.
- Fresh clean-export at `/tmp/w34b24-i37-clean-export`: PASS (8/8),
  `world_map.bin` copied as a regular file (180422).
- This helper is not a 71A58 callee. After this leaf,
  `71A58_MISSING_CALLEES` remains 7; `98CC0` overlay callees are
  all ACCEPTED. `98CC0` itself still has SLUS JALs
  `ArchiveGetFilePath` `0x80028998`, `ArchiveDecodeSector`
  `0x800289D0`, `func_8002C3D8`, `HeapAlloc` `0x80031BDC`, and
  `HeapFree` `0x800320E8`. Overlay set is closed.

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
`0x80086798` remains unimplemented.
