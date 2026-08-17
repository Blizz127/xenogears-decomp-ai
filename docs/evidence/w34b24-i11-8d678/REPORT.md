# W34B24-I11 — wm_8008D678 Implementation Report (Slot-5/6 Callback)

Continuation after G5 8C844. Base: canonical
`f1434044ba3f9361a4cc50f570e76154ebefba6d` (8C844 landed). Live frontier
after 8C844 was missing slot-5 cb1 `0x8008D678` (also slot-6 cb1).
MISSING=0 (apparent `0x8004B32C` is PsyQ `ratan2`).

## Identity

| Field | Value |
|---|---|
| Function | `wm_8008D678` (scheduler slot-5 cb1 and slot-6 cb1) |
| Boundary | `[0x8008D678, 0x8008DD6C)` = 1780 B / 445 insns |
| SHA-256 | `62f045bd1be041b4dafa1ea440d4af9d45739decc8c8c5d2cebe748000a56636` |
| File off | `0x1DB88` (overlay base `0x8006FAF0`) |
| Source | `pc_port/src/world_map_callback_8d678.{c,h}` |
| Test | `pc_port/tests/w34b24_i11_8d678_prod_test.c` + `run_w34b24_i11_8d678.sh` |

ABI: `$a0` = slot index (observed 5; also slot 6); **all paths return 1**.
Sub JT 8 entries at `0x800708D4`, index `(s16)(lhu +4 - 1)`, sltiu 8.
JT1 65 entries at `0x800708F4`, sltiu `0x41`, 18 dests (17 live + park).
30 JAL / 12 unique; MISSING=0.

Not an 8C844 clone. Material deltas:

- Follow flag is `lbu[0x8006F8E1+slot]` (not F8E4+slot / F8E5).
- Follow==1 is the ring-follower path; follow!=1 is dormant anim-3.
- `894C8`/`8C1DC` record is `slot+0x28` (slot 5 → `0x2D`).
- State 8 uses neighbor `pool+(lh+6)<<7` from substate 1 only, then
  falls into 9. State 0x10 falls into 0x11. State 0x20 / 0x30 do **not**
  fall into the next state.
- State 0x10 aims at slot-4 pose via `ratan2(dz,dx)`.
- Common tail: `wm_80074794(1, slot+0x28)` unless `state==2` or
  (`MODE_FLAG!=2` && `lbu[0x8006F364+slot]==7`). Always sra12-publishes
  X/Z to `0x8006EF90+(slot-4)*6` and heading to `0x8006EE52+slot*2`
  (slot 5 → EF96/EF98 / EE5C).

## Scheduler registration

Weak `extern s32 wm_8008D678(s32)`, `wm_sched_builtin_8008D678` s16 thunk,
resolver arm `0x8008D678u`. Address remains in `s_wm_sched_known_missing`;
the resolver finds the strong body first.

## Certification

- O0 / O2 / UBSan-nonrecovering: PASS, normalized stdout byte-identical.
  Host gcc lacks `libubsan`; the runner falls back to clang for UBSan.
- Oracle: seam-forced callees, hand-derived expectations. Does **not**
  bake `NEXT_CALLBACK_TARGET`.
- Mutants: **17/17 KILLED** (MUTANTS.csv).
- Build: clean + incremental LINK OK ×2; `compiled=47 skipped=0` (src/*.c
  game TUs; port-only 8D678 is outside that counter); exactly one strong
  `T wm_8008D678` @ `00000000004aac4a`; no stub shadow; stubs **240/524
  unchanged**.
- Suites: 24/26 `run_w34*.sh` PASS in distrobox. The two remaining
  (`run_w34b_r4world_73b04.sh`, `run_w34cb1_90a84.sh`) fail to compile
  under current gcc because `-fpermissive` is a C++-only flag promoted
  by `-Werror` — host toolchain, not an 8D678 regression. Independent
  O0/O2 recheck of both (flag dropped; 73B04 uses
  `pc_port/build/libpsycross.a`): PASS.
- Fresh clean-export at `/tmp/w34b24-i11-clean-export`: PASS (17/17),
  `world_map.bin` copied as a regular file (180422).
- `0x80071A58` was not implemented (not on the live path).

## RUNTIME PAYOFF — THE CALLBACK FRONTIER MOVED

Natural Lahan route, full deep-gate chain through
`XENO_WORLD_FRAME_PROLOGUE=1` (no forced PC/callback/slot/state).
`DISPLAY=:0` (`:10` absent). First scheduler pass is Table-A cb0 init
(16/16 executed). Second pass is the live cb1 walk:

```
[worldmap-scheduler] slot=1 state=1 cb=0x8008a72c executed ret=1
[worldmap-scheduler] slot=2 state=1 cb=0x8008b644 executed ret=1
[worldmap-scheduler] slot=4 state=1 cb=0x8008c844 executed ret=1
[worldmap-scheduler] slot=5 state=1 cb=0x8008d678 executed ret=1
[worldmap-scheduler] MISSING CALLBACK FRONTIER slot=8 state=1 cb=0x800907f4
counters: dispatch=22 executed=21 missing=1 outcome=1 frontier=0x800907f4
```

```
canonical_head=f1434044ba3f9361a4cc50f570e76154ebefba6d
previous_frontier=0x8008D678
executed_function=0x8008D678
return=1
next_callback_target=0x800907F4
next_callback_slot=8
next_callback_state=1
next_callback_class=MISSING
new_frontier=0x800907F4
```

Slots 3/6/7 stay dormant (cb0 returned 3). Scheduler executed callbacks:
20 → **21**. NEW CALLBACK FRONTIER: **`wm_800907F4` (slot-8 cb1, state 1)**.
The harness reached DrawSync/Vsync/712D0 after the second scheduler pass;
the placeholder-Vsync tail was stopped after the frontier measurement.

