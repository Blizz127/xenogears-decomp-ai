# W34B24-I12 — wm_800907F4 Implementation Report (Slot-8 cb1)

Continuation after G5 8D678. Base: canonical
`297930b6583b9d858dd2d0a4ab4e4fa7bcf1f2c8` (8D678 landed). Live frontier
after 8D678 was missing slot-8 cb1 `0x800907F4`. MISSING=0 (only JAL is
PsyQ `RotMatrixZYX` @ `0x8004ABBC`).

## Identity

| Field | Value |
|---|---|
| Function | `wm_800907F4` (scheduler slot-8 cb1) |
| Boundary | `[0x800907F4, 0x80090A18)` = 548 B / 137 insns |
| SHA-256 | `bd2946bda218185bd796696ef10c845d09f2ebfb9a99814d090e303ae46cc91b` |
| File off | `0x20D04` (overlay base `0x8006FAF0`) |
| Source | `pc_port/src/world_map_callback_907f4.{c,h}` |
| Test | `pc_port/tests/w34b24_i12_907f4_prod_test.c` + `run_w34b24_i12_907f4.sh` |

ABI: `$a0` = slot index (observed 8); **all paths return 1**.
Sibling of accepted `wm_800906E0` (slot-8 cb0). Next symbol is accepted
`wm_80090A18`. No jump table; states 0/1/2 plus parked 3+.

- Sub 9 → state 1; sub 0xA → state 2.
- State 0 zeros `+0x58/+0x5C`.
- State 1 ramps both up by 4 (`+5C` lags until `+58>=0x11`), clamp 0x80,
  both-max → state 3.
- State 2 ramps down (`+5C` lags until `+58<0x70`), floor 0, both-zero →
  state 0.
- Common tail: `+50 += +58`, `+54 -= +5C`, then two `RotMatrixZYX` into
  context records 2 and 3.

## Scheduler registration

Weak `extern s32 wm_800907F4(s32)`, `wm_sched_builtin_800907F4` s16 thunk,
resolver arm `0x800907F4u`. Address remains in `s_wm_sched_known_missing`.

## Certification

- O0 / O2 / UBSan-nonrecovering: PASS, normalized stdout byte-identical.
- Mutants: **15/15 KILLED** (MUTANTS.csv).
- Build: clean + incremental LINK OK ×2; `compiled=47 skipped=0`; exactly
  one strong `T wm_800907F4` @ `00000000004abeda`; no stub shadow; stubs
  **240/524 unchanged**.
- Suites: I9/I10/I11/I12 PASS. Distrobox `run_w34*.sh` 24/26 on the 8D678
  rung; the two `-fpermissive` scripts are the same host-toolchain issue
  (independent O0/O2 recheck PASS).
- Fresh clean-export at `/tmp/w34b24-i12-clean-export`: PASS (15/15),
  `world_map.bin` copied as a regular file (180422).
- `0x80071A58` was not implemented (not on the live path).

## RUNTIME PAYOFF — THE CALLBACK FRONTIER MOVED

Natural Lahan route, full deep-gate chain through
`XENO_WORLD_FRAME_PROLOGUE=1` (no forced PC/callback/slot/state).
`DISPLAY=:0`. Second scheduler pass:

```
[worldmap-scheduler] slot=5 state=1 cb=0x8008d678 executed ret=1
[worldmap-scheduler] slot=8 state=1 cb=0x800907f4 executed ret=1
[worldmap-scheduler] MISSING CALLBACK FRONTIER slot=9 state=1 cb=0x800914d0
counters: dispatch=23 executed=22 missing=1 outcome=1 frontier=0x800914d0
```

```
canonical_head=297930b6583b9d858dd2d0a4ab4e4fa7bcf1f2c8
previous_frontier=0x800907F4
executed_function=0x800907F4
return=1
next_callback_target=0x800914D0
next_callback_slot=9
next_callback_state=1
next_callback_class=MISSING
new_frontier=0x800914D0
```

Scheduler executed callbacks: 21 → **22**. NEW CALLBACK FRONTIER:
**`wm_800914D0` (slot-9 cb1)**. Direct JALs: `wm_80093354` (accepted),
`wm_80097770` (accepted), and missing helper `0x80093484`.

