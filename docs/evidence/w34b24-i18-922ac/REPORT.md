# W34B24-I18 — wm_800922AC Implementation Report (slot-11 cb1)

Ladder after G5 91C18. Base: canonical
`37dea39329e182553c29dc3f60bc8007de4df924`. Live frontier was missing
slot-11 cb1 `0x800922AC` with MISSING=0 (leaf).

## Identity

| Field | Value |
|---|---|
| Function | `wm_800922AC` (slot-11 cb1 scale ramp) |
| Boundary | `[0x800922AC, 0x800923A8)` = 252 B / 63 insns |
| SHA-256 | `325945d54dee4548f08763d086d81e00fc58becb19887b4c00bd6232aea3c4a3` |
| File off | `0x227BC` |
| Slot | Table A/B slot 11 cb1 |
| ABI | `$a0` = slot index (natural 11); `$v0` = 3 when +20==0, else 1 |
| Source | `pc_port/src/world_map_callback_922ac.{c,h}` |
| Test | `pc_port/tests/w34b24_i18_922ac_prod_test.c` + `run_w34b24_i18_922ac.sh` |

Leaf: no JAL / JALR / COP2 / GPU / OT. Publishes `+50>>12` to
`0x8009BE0C`. State 1 steps −0x1000 until `< 0x78`; state 2 steps
+0x1000 until `>= 0x8C`.

## Certification

- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan
  fallback).
- Mutants: **9/9 KILLED**.
- Sibling I17 focused oracle: PASS.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_800922AC` @ `00000000004add18`; local thunk
  `t wm_sched_builtin_800922AC` @ `00000000004a12b8`; no stub shadow;
  stubs **240/524 unchanged**.
- Fresh clean-export at `/tmp/w34b24-i18-clean-export`: PASS (9/9),
  `world_map.bin` copied as a regular file (180422).
- `0x80071A58` was not implemented (not on the live path).

## RUNTIME PAYOFF — THE CALLBACK FRONTIER MOVED

Natural Lahan route, full deep-gate chain through
`XENO_WORLD_FRAME_PROLOGUE=1` (no forced PC/callback/slot/state).
`DISPLAY=:0`. `w18b_natural.gdb` (150s timeout; scheduler evidence
complete). First cb1 visit returns 3 (slot +20 is still 0).

```
[worldmap-scheduler] slot=10 state=1 cb=0x80091c18 executed ret=1
[worldmap-scheduler] slot=11 state=1 cb=0x800922ac executed ret=3
[worldmap-scheduler] MISSING CALLBACK FRONTIER slot=12 state=1 cb=0x80092c70
counters: dispatch=26 executed=25 missing=1 completed=1 frontier=0x80092c70
```

```
922AC_BODY_EXECUTED=YES
922AC_RETURN=3
SLOT11_STATE_BEFORE=1
SLOT11_STATE_AFTER=3
SCHEDULER_COMPLETED_PASSES_BEFORE=1
SCHEDULER_COMPLETED_PASSES_AFTER=1
NEXT_CALLBACK_TARGET=0x80092C70
NEXT_CALLBACK_SLOT=12
NEXT_CALLBACK_STATE=1
NEXT_CALLBACK_CLASS=MISSING
FRAME_FRONTIER=0x80092C70
CRASH_OR_CUT_PC=0x80071490
```

Scheduler executed callbacks: 24 → **25**. NEW CALLBACK FRONTIER:
**`wm_80092C70` (slot-12 cb1)**. Overlay
`[0x80092C70, 0x80092DD0)` = 352 B / 88 insns. JALs are main-exe
`0x80034614` / `GetStringEntry@0x80033728` / `0x80034714` /
`0x80034888` (same family as accepted 92BE4 / 92DF8). Independent
callee-closure audit is the next G5 rung — not implemented here.
