# W34B24-I38 — wm_800966CC Implementation Report (C624 PC-file pass)

Ladder after I37. Bounded overlay callee of `0x800967E4`, itself the
remaining overlay callee of blocked 71A58 callee `0x80096130`.
Overlay `MISSING_CALLEES=0`.

## Identity

| Field | Value |
|---|---|
| Function | `wm_800966CC` (16-byte PC-file record pass) |
| Boundary | `[0x800966CC, 0x800967E4)` = 280 B / 70 insns |
| SHA-256 | `dc05678521353ada407a6ff648c69f330c8e419b6d50912c7e90f9426744c082` |
| File off | `0x26BDC` |
| Source | `pc_port/src/world_map_helper_966cc.{c,h}` |
| Test | `pc_port/tests/w34b24_i38_966cc_prod_test.c` + `run_w34b24_i38_966cc.sh` |

This body restores frame `0x28` and `jr $ra; nop` at `0x800967DC`.
Next is `wm_800967E4`. PsyQ: `PCopen` `0x8004C318`, `PClseek`
`0x8004C348`, `PCread` `0x8004C398`, `PCclose` `0x8004C338`. No
overlay JAL / JALR / COP2 / GPU / OT.

ABI: `void(a0)`. `$a0` = 16-byte records. Terminator is word `+0`.
Clears `0x8009BE48` / `0x8009CCB0` / `0x8009CCA8` / `0x8009CCA0`.
Open/read/close retry 8 times. `PCread` dest is word `+12`, count
is word `+8`. W29B `wm_800966CC_c624_processor` swapped those two
arguments; this helper follows retail.

## Certification

- Independent retail slice SHA checked by the runner.
- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **6/6 KILLED**.
- Sibling I36 / I37 focused oracles: PASS.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_800966CC` @ `00000000004b8824` (W29B
  `wm_800966CC_c624_processor` remains a separate symbol); no stub
  shadow; stubs **239/524 unchanged**. Remaining assumed:
  `wm_80097DC0`.
- Fresh clean-export at `/tmp/w34b24-i38-clean-export`: PASS (6/6),
  `world_map.bin` copied as a regular file (180422).
- This helper is not a 71A58 callee. After this leaf,
  `71A58_MISSING_CALLEES` remains 7; `967E4` still needs `968E0` /
  `9699C`.

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
`0x80086798` remains unimplemented.
