# W34B24-I31 — wm_80089748 Implementation Report (71A58 particle emitter)

Ladder after I30 `0x80089580`. Next bounded missing 71A58 callee
with overlay `MISSING_CALLEES=0` (`89580` ACCEPTED).

## Identity

| Field | Value |
|---|---|
| Function | `wm_80089748` (512-emitter spawn + 256-slot fill + 89580) |
| Boundary | `[0x80089748, 0x80089C78)` = 1328 B / 332 insns |
| SHA-256 | `bfa19d4b844d7814bedb336f1e9837944f935d5dad029a681557b5a65891da73` |
| File off | `0x19C58` |
| Source | `pc_port/src/world_map_helper_89748.{c,h}` |
| Test | `pc_port/tests/w34b24_i31_89748_prod_test.c` + `run_w34b24_i31_89748.sh` |

Previous `wm_80089580` ends `jr $ra; nop` at `0x80089740`. This body
restores frame `0x40` and `jr $ra; nop` at `0x80089C70`. Next is
accepted `wm_80089C78`. ABI: void. Overlay JAL: `wm_80089580`.
PsyQ: RotMatrixYXZ `0x8004A92C`, ApplyMatrix `0x80049CEC`, rand
`0x8003FA38`, VectorNormal `0x80048D7C`, ratan2 `0x8004B32C`.
No JALR / COP2 / GPU / OT.

## Certification

- Independent retail slice SHA checked by the runner.
- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **21/21 KILLED**.
- Sibling I9–I18 / I21–I30 focused oracles: PASS.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_80089748` @ `00000000004b7b4b`; no stub shadow; stubs
  **239/524 unchanged**. Remaining assumed: `wm_80097DC0`.
- Fresh clean-export at `/tmp/w34b24-i31-clean-export`: PASS (21/21),
  `world_map.bin` copied as a regular file (180422).
- This helper is not a scheduler callback. The live frontier remains
  `0x80071A58`. After this leaf, `71A58_MISSING_CALLEES=7`.

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
`0x80086798` remains unimplemented; overlay store sites to
`0x8009CD40` were measured (see PRE4) but the jalr is still not
treated as a closed implementable set.
