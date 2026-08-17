# W34B24-I39 — wm_800968E0 Implementation Report (CD44 switch)

Ladder after I38. Bounded overlay callee of `0x800967E4`. Overlay
`MISSING_CALLEES=0`. The `jr $v0` at `0x80096908` is a closed
6-entry switch, not an unbounded jalr.

## Identity

| Field | Value |
|---|---|
| Function | `wm_800968E0` (CD44 state dispatch) |
| Boundary | `[0x800968E0, 0x8009699C)` = 188 B / 47 insns |
| SHA-256 | `27221b582c93408a01c5a64c4b05aa1fa6cb1d6a4eef07311f16237a28963c93` |
| File off | `0x26DF0` |
| Source | `pc_port/src/world_map_helper_968e0.{c,h}` |
| Test | `pc_port/tests/w34b24_i39_968e0_prod_test.c` + `run_w34b24_i39_968e0.sh` |

Previous `wm_800967E4` ends `jr $ra; nop` at `0x800968D8`. This body
is `jr $ra; nop` at `0x80096994`. Next is `wm_8009699C`. No JAL /
JALR / COP2 / GPU / OT.

Switch table at `0x80070CA0` (file off `0x11B0`), used only when
`*0x8009CD44 < 6`:

| Index | Target |
|---|---|
| 0 | `0x80096910` return 0 |
| 1 / 2 / 3 | `0x80096950` return 1 |
| 4 | `0x80096918` decrement `*0x8009BD2C`; if zero, increment CD44; return 1 |
| 5 | `0x80096958` CD44=0, clear `D788[ring]`, ring `(+1)&0xf`; return 2 |

`>=6` returns 3. Words 6–7 at the table site point outside this
body and are not reachable (guard is `sltiu 6`).

## Certification

- Independent retail slice SHA and the six table VAs checked by the
  runner.
- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **4/4 KILLED**.
- Sibling I36 / I37 / I38 focused oracles: PASS.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_800968E0` @ `00000000004b8a47` (W29B
  `wm_800968E0_dispatch_partial` remains a separate symbol); no stub
  shadow; stubs **239/524 unchanged**. Remaining assumed:
  `wm_80097DC0`.
- Fresh clean-export at `/tmp/w34b24-i39-clean-export`: PASS (4/4),
  `world_map.bin` copied as a regular file (180422).
- This helper is not a 71A58 callee. After this leaf,
  `71A58_MISSING_CALLEES` remains 7; `967E4` still needs `9699C`.

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
`0x80086798` remains unimplemented. Do not treat this `jr $v0` as
a reason to invent `86798` jalr targets.
