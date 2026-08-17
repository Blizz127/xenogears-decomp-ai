# W34B24-I17 — wm_80091C18 Implementation Report (slot-10 cb1)

Ladder after G5 91FF8 / 96F18. Base: canonical
`a050542672a187a14c404e3cbfb22e0ea0e37ab9`. Live frontier was missing
slot-10 cb1 `0x80091C18` with MISSING=0.

## Identity

| Field | Value |
|---|---|
| Function | `wm_80091C18` (slot-10 cb1 heading/probe) |
| Boundary | `[0x80091C18, 0x80091FF8)` = 992 B / 248 insns |
| SHA-256 | `a6b27abda1c9a686fa224269084b33c35b9e5d99bae651689cb76f24d6f85a37` |
| File off | `0x22128` |
| Slot | Table A/B slot 10 cb1 |
| ABI | `$a0` = slot index (natural 10); **all paths return 1** |
| Source | `pc_port/src/world_map_callback_91c18.{c,h}` |
| Test | `pc_port/tests/w34b24_i17_91c18_prod_test.c` + `run_w34b24_i17_91c18.sh` |

JALs: accepted `91FF8` (state 0, and state 1 when `+22>=5`) and
`96F18` (states 0/1/3). No JALR / COP2 / GPU / OT. `+22` increments
every return. Sub 9 / 0xA install heading tables; 0xE parks; 0x11
arms state 3 and `D3F0=0x00400000`.

## Certification

- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan
  fallback).
- Mutants: **14/14 KILLED**.
- Sibling I14 / I16 focused oracles: PASS.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_80091C18` @ `00000000004ad5d8`; local thunk
  `t wm_sched_builtin_80091C18` @ `00000000004a1282`; no stub shadow;
  stubs **240/524 unchanged**.
- Fresh clean-export at `/tmp/w34b24-i17-clean-export`: PASS (14/14),
  `world_map.bin` copied as a regular file (180422).
- `0x80071A58` was not implemented (not on the live path).

## RUNTIME PAYOFF — THE CALLBACK FRONTIER MOVED

Natural Lahan route, full deep-gate chain through
`XENO_WORLD_FRAME_PROLOGUE=1` (no forced PC/callback/slot/state).
`DISPLAY=:0`. `w18b_natural.gdb` (150s timeout before the 120-Vsync
verdict; scheduler evidence complete).

```
[worldmap-scheduler] slot=9 state=1 cb=0x800914d0 executed ret=1
[worldmap-scheduler] slot=10 state=1 cb=0x80091c18 executed ret=1
[worldmap-scheduler] MISSING CALLBACK FRONTIER slot=11 state=1 cb=0x800922ac
counters: dispatch=25 executed=24 missing=1 completed=1 frontier=0x800922ac
```

```
91C18_BODY_EXECUTED=YES
91C18_RETURN=1
SLOT10_STATE_BEFORE=1
SLOT10_STATE_AFTER=1
SCHEDULER_COMPLETED_PASSES_BEFORE=1
SCHEDULER_COMPLETED_PASSES_AFTER=1
NEXT_CALLBACK_TARGET=0x800922AC
NEXT_CALLBACK_SLOT=11
NEXT_CALLBACK_STATE=1
NEXT_CALLBACK_CLASS=MISSING
FRAME_FRONTIER=0x800922AC
CRASH_OR_CUT_PC=0x80071490
```

Scheduler executed callbacks: 23 → **24**. NEW CALLBACK FRONTIER:
**`wm_800922AC` (slot-11 cb1)**. Overlay leaf
`[0x800922AC, 0x800923A8)` = 252 B / 63 insns, **no JALs**,
MISSING_CALLEES=0.
