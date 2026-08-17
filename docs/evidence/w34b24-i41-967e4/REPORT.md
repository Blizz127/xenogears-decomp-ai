# W34B24-I41 — wm_800967E4 Implementation Report (CD/PC work dispatch)

Ladder after I40. Overlay callee of blocked 71A58 callee
`0x80096130`. Overlay `MISSING_CALLEES=0`.

## Identity

| Field | Value |
|---|---|
| Function | `wm_800967E4` (one CD/PC work step) |
| Boundary | `[0x800967E4, 0x800968E0)` = 252 B / 63 insns |
| SHA-256 | `0d16c4f020e76808390b2ad94cdff89aa6c28b60bcd4beb2bd0890af938b1530` |
| File off | `0x26CF4` |
| Source | `pc_port/src/world_map_helper_967e4.{c,h}` |
| Test | `pc_port/tests/w34b24_i41_967e4_prod_test.c` + `run_w34b24_i41_967e4.sh` |

Previous `wm_800966CC` ends `jr $ra; nop` at `0x800967DC`. This body
restores frame `0x28` and `jr $ra; nop` at `0x800968D8`. Next is
`wm_800968E0`. Overlay JALs: `968E0` / `9699C` / `966CC` (I38–I40).
SLUS: `func_8002C3D8` ×2. No JALR / COP2 / GPU / OT.

ABI: `s32(void)`. Return is `$s1` (0 unless `968E0` is nonzero).
C624 path when first debug word `!= 0` AND second `!= 0xFFFFFFFF`
(`sltiu`/`nor`/`sltiu`/`or`/`beqz`). Else `968E0`; if 0, process
`D788[BCB8]` via `9699C` without advancing. C624 path calls
`966CC(C624[BCB8])`, clears that slot, and advances BCB8 mod 16.

W29B `wm_800967E4_dispatch_cd_work` remains a separate symbol and
is not this G5 helper.

## Certification

- Independent retail slice SHA checked by the runner.
- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **3/3 KILLED**.
- Sibling I40 focused oracle: PASS.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_800967E4` @ `00000000004b8da3`; no stub shadow; stubs
  **239/524 unchanged**. Remaining assumed: `wm_80097DC0`.
- Fresh clean-export at `/tmp/w34b24-i41-clean-export`: PASS (3/3),
  `world_map.bin` copied as a regular file (180422).
- This helper is not a 71A58 callee. After this leaf, `96130` is
  bounded (overlay jal `967E4` only; PsyQ `Vsync` `0x8004B54C`).

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
`0x80086798` remains unimplemented.
