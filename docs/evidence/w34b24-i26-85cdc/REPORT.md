# W34B24-I26 — wm_80085CDC Implementation Report (71A58 billboard submitter)

Ladder after I25 `0x800737EC`. Next bounded missing 71A58 callee
with overlay `MISSING_CALLEES=0` (`93484` ACCEPTED; remaining JALs are
SLUS/PsyQ).

## Identity

| Field | Value |
|---|---|
| Function | `wm_80085CDC` (64-slot wrap + RTPS SZ3 + sprite submit) |
| Boundary | `[0x80085CDC, 0x80085F58)` = 636 B / 159 insns |
| SHA-256 | `fc6f4405d1bd956051519f3db3ed6477e7c0c09ee09739aef2ceb5176ffa6ab6` |
| File off | `0x161EC` |
| Source | `pc_port/src/world_map_helper_85cdc.{c,h}` |
| Test | `pc_port/tests/w34b24_i26_85cdc_prod_test.c` + `run_w34b24_i26_85cdc.sh` |

PRE4's `[0x80085CDC, 0x80085FE0)` (772 B / 193) was wrong: it swallowed
the next function. `wm_80085760` ends `jr $ra; nop` at `0x80085CD4`.
This body restores frame `0x28` and `jr $ra; nop` at `0x80085F50`.
`0x80085F58` is a separate init-only record/CLUT helper (jal from
`0x8007246C`), not a 71A58 callee.

ABI: void. `71A58` jal delay is `nop`; `$a0` unused. Overlay JAL:
`wm_80093484`. SLUS/PsyQ: `SetRotMatrix`/`SetTransMatrix` `0x8009C808`,
`func_80024FF4`, `func_8001E298`, `func_800223B0`, `AnimScriptTick`.
COP2: `lwc2 VXY0/VZ0`, `RTPS` `0x4A180001`, `swc2 SZ3`.

## Certification

- Independent retail slice SHA checked by the runner.
- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **16/16 KILLED**.
- Sibling I13 / I15 / I19 / I20 / I21 / I22 / I23 / I24 / I25 focused
  oracles: PASS.
- Suites: I9–I12, I14, I16–I18 PASS. Older I1–I8 HOST `libubsan.so.1.0.0`
  / `-fpermissive` under host gcc — not an 85CDC regression.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_80085CDC` @ `00000000004b43a8`; no stub shadow; stubs
  **239/524 unchanged**. Remaining assumed: `wm_80097DC0`.
- Fresh clean-export at `/tmp/w34b24-i26-clean-export`: PASS (16/16),
  `world_map.bin` copied as a regular file (180422).
- This helper is not a scheduler callback. The live frontier remains
  `0x80071A58`. After this leaf, `71A58_MISSING_CALLEES=11`.

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
`0x80086798` remains a genuine jalr blocker on the 71A58 tail.
