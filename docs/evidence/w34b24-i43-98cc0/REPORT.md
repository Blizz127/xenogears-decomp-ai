# W34B24-I43 — wm_80098CC0 Implementation Report (CD/PC bank fill)

Ladder after I42 `0x80096130`. This is a 71A58 overlay callee.
Overlay `MISSING_CALLEES=0`.

## Identity

| Field | Value |
|---|---|
| Function | `wm_80098CC0` (keep-list sweep + CD/PC bank fill) |
| Boundary | `[0x80098CC0, 0x8009932C)` = 1644 B / 411 insns |
| SHA-256 | `5a5d9b8f7fcee4651a39633c852c1415a8de66356140702c9eb9d85bd0fd62a4` |
| File off | `0x291D0` |
| Source | `pc_port/src/world_map_helper_98cc0.{c,h}` |
| Test | `pc_port/tests/w34b24_i43_98cc0_prod_test.c` + `run_w34b24_i43_98cc0.sh` |

Previous body ends `jr $ra; nop` at `0x80098CB8`. This body
restores frame `0x38` and `jr $ra; nop` at `0x80099324`. Next is
`wm_8009932C`. Overlay JALs: `9623C` / `962B0` / `96328` / `965A4`.
SLUS: `ArchiveGetFilePath` `0x80028998`, `ArchiveDecodeSector`
`0x800289D0`, `func_8002C3D8` ×2, `HeapAlloc` `0x80031BDC`,
`HeapFree` `0x800320E8`. No JALR / COP2 / GPU / OT.

ABI: `void`. Same debug-table predicate as `96130`. Cleanup frees
`C184[D318[i]]` when that id is absent from `D570[0..80]`. CD path
enqueues 12-byte records via `9623C` and publishes with `96328`.
PC path enqueues 16-byte records via `962B0` and publishes with
`965A4`. Group 1 walks `D570[1..7]` and `D570[73..79]`. Group 2
walks stride-`0x12` from starts 9 and `0x11`. Third group uses
`BBAC[0..3]` as keep indices.

## Certification

- Independent retail slice SHA checked by the runner.
- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **15/15 KILLED**.
- Sibling I31 / I32 / I33 / I36 / I37 / I42 focused oracles: PASS.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_80098CC0` @ `00000000004b9afa`; no stub shadow; stubs
  **239/524 unchanged**. Remaining assumed: `wm_80097DC0`.
- Fresh clean-export at `/tmp/w34b24-i43-clean-export`: PASS (15/15),
  `world_map.bin` copied as a regular file (180422).
- After this leaf, `71A58_MISSING_CALLEES=5`.

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
`0x80086798` remains unimplemented.
