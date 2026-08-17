# W34B24-I44 — wm_800747DC Implementation Report (keep-list GTE submitter)

Ladder after I43 `0x80098CC0`. This is a 71A58 overlay callee.
Overlay `MISSING_CALLEES=0` (`93740` / `93978` ACCEPTED). End was
independently locked from retail this turn.

## Identity

| Field | Value |
|---|---|
| Function | `wm_800747DC` (keep-list GTE submitter) |
| Boundary | `[0x800747DC, 0x80074E58)` = 1660 B / 415 insns |
| SHA-256 | `ca255b493d7948e43c2bea3ff15f14bd714f3729adbbaf821884b5195bd959d4` |
| File off | `0x4CEC` |
| Source | `pc_port/src/world_map_helper_747dc.{c,h}` |
| Test | `pc_port/tests/w34b24_i44_747dc_prod_test.c` + `run_w34b24_i44_747dc.sh` |

Previous accepted `wm_80074794` ends before this prologue
(`addiu $sp, -0x68`). This body restores frame `0x68` and
`jr $ra; nop` at `0x80074E50`. Next is a new function at
`0x80074E58`. Overlay JALs: `93978`, `93740`. PsyQ:
`OuterProduct12` `0x8004A480`, `VectorNormal` `0x80048D7C`,
`RotMatrixY` `0x8004AFEC`, `MulMatrix` `0x80049ACC`,
`ScaleMatrix` `0x80049DCC`. COP2: CTC2 R+T, RTIR×3, RT, RTPT,
FLAG, RTPS. No JALR.

ABI: `void`. Early-out if `*0x8009BE38 == 0`. Loop count is that
word; each 8-byte record at `*0x8009D30C` is submitted into the
OT at `*(*0x8009BE3C+0x70)` with masks `0xFF000000` /
`0x00FFFFFF` and slot `(depth >> 4) << 2` (arithmetic). Clears
`*0x8009BE38` on the way out. Mode 1 scales `0x1800/1800/1800`.
Mode 2 copies `0x8009A180`, `RotMatrixY(*0x8006EE66)`,
`MulMatrix`, then scales `0x1800/1000/4800`. Other modes skip
ScaleMatrix. FLAG < 0 skips SXY/SZ store, RTPS, and OT.

## Certification

- Independent retail slice SHA checked by the runner.
- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **18/18 KILLED**.
- Sibling I26 / I27 / I29 / I31 / I32 / I33 / I36 / I37 / I42 / I43
  focused oracles: PASS.
- Suites: I9–I12, I14, I16–I18 PASS.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_800747DC` @ `00000000004ba53b`; no stub shadow; stubs
  **239/524 unchanged**. Remaining assumed: `wm_80097DC0`.
- Fresh clean-export at `/tmp/w34b24-i44-clean-export`: PASS (18/18),
  `world_map.bin` copied as a regular file (180422).
- After this leaf, `71A58_MISSING_CALLEES=4`.

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
`0x80086798` remains unimplemented.
