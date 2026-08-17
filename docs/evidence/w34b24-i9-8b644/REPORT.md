# W34B24-I9 — wm_8008B644 Implementation Report (Slot-2/3 Companion Callback)

Milestone G5 rung. Base: canonical `2656c3ab33066ccd9f5c09eace2c31596fcd876a`
(PRE4 8B644 closure audit landed; G4 accepted).

## Identity

| Field | Value |
|---|---|
| Function | `wm_8008B644` (scheduler slot-2 cb1 and slot-3 cb1) |
| Boundary | `[0x8008B644, 0x8008BB40)` = 1276 B / 319 insns |
| SHA-256 | `be1d5c3bc2941db081002efb936eb6f219f3d0aefc69d99f1e56f1c52a609895` |
| Source | `pc_port/src/world_map_callback_8b644.{c,h}` |
| Test | `pc_port/tests/w34b24_i9_8b644_prod_test.c` + `run_w34b24_i9_8b644.sh` |

Signature `s32 wm_8008B644(s32 slot_idx)`; returns constant 1 on every path
(`$s4 = 1` from the prologue). Full contract in the header comment + the
PRE4 audit pack. JT1's complete 65-slot map is recorded in the source
comment (11 destinations / 10 live arms + park; 53 parked; OOR -> common
tail). All PRE4 prose claims were re-derived from the instructions during
implementation; no inversions were found.

Companion/follower: no `wm_80095414`, `wm_8008C040`, `wm_80094238`, and no
8A72C-style sra12 publish to `0x8006EE54/56/58`. Common tail calls
`wm_80074794(0, slot+0x28)` iff `lh slot[+0x24] == 0`.

State 8 falls into state 9 (`8BEC8` + `8C1DC`). State 0x28 joins state
0x30's `245D8` / state++ / `8BEC8` tail, not its player-pose `941C4`.

## Scheduler registration

Followed the accepted 8A72C pattern: weak `extern s32 wm_8008B644(s32)`
in `world_map_scheduler.c`, a `wm_sched_builtin_8008B644` s16 thunk, and a
resolver arm for guest address `0x8008B644u`. The address remains in
`s_wm_sched_known_missing` (the fallback classifier); the resolver finds
the strong body first.

## Certification

- O0 / O2 / UBSan-nonrecovering: PASS, normalized stdout byte-identical.
  Host gcc lacks `libubsan`; the runner falls back to clang for the UBSan
  regime (accepted host-toolchain adaptation).
- Oracle coverage (all callees seam-forced, hand-derived expectations):
  substates 1/2/3/5, JT1 OOR guard, parked states, follow-flag copy
  (`0x8006F8E4+slot` and `pool+0x180+(slot<<7)`), ring match/miss and
  `(D154-lag)&0x1F` mask, slot-7 copy, state 8 fallthrough into 9,
  state 9 hold/advance, state 0xA hold/go, warp-in 0x28 (slot*6 xz,
  `<<12`, `-rsin*48`, join 0x30 anim tail, no 8C1DC), state 0x2A clear
  flag -> 0x40, state 0x30 vs pool+0xA8, shared 0x29/0x31 step, state
  0x32 hold/go `(1,6)`, slot-3 same-body smoke (obj id 0x31), tail
  74794 gate, constant return 1, and canary proof that EE54/56/58 are
  not published. `NEXT_CALLBACK_TARGET` is not an oracle input.
- Mutants: **17/17 KILLED** (MUTANTS.csv). Honest note: the retail
  `sltiu 0x41` guard mutant is expressed as a 0x2D guard because
  0x41->0x40 is behaviorally invisible (slot 0x40 parks to the same tail).
- Build: clean + incremental LINK OK ×2; `compiled=47 skipped=0`;
  exactly one strong `T wm_8008B644` @ `00000000004a8872`; no stub
  shadow; stubs 240/524 unchanged.
- Suites: 22/24 scripts PASS in distrobox. The two remaining
  (`run_w34b_r4world_73b04.sh`, `run_w34cb1_90a84.sh`) fail to compile
  under current gcc because `-fpermissive` is a C++-only flag promoted
  to error by `-Werror` — a host-toolchain issue, not an 8B644
  regression. Independent O0/O2 recheck of both oracles (flag dropped)
  PASS. Host-side `libubsan` is missing; focused I9 uses the accepted
  clang UBSan fallback.
- Fresh clean-export of the focused certificate: PASS (17/17),
  `world_map.bin` copied as a regular file.
- Hardened `w18b_natural.gdb`: `verdict=PASS exit=0`, 13x ZERO_VERIFIED
  (DISPLAY=:0; `:10` is absent on this host).
- `0x80071A58` was not implemented (not on the live path).

## RUNTIME PAYOFF — THE CALLBACK FRONTIER MOVED

Natural Lahan route, full deep-gate chain through
`XENO_WORLD_FRAME_PROLOGUE=1` (no forced PC/callback/slot/state).
First scheduler pass is Table-A cb0 init (16/16 executed). Second pass
(frame prologue `jal 0x80097800` at `0x80071488`) is the live cb1 walk:

```
[worldmap-scheduler] slot=1 state=1 cb=0x8008a72c executed ret=1
[worldmap-scheduler] slot=2 state=1 cb=0x8008b644 executed ret=1
[worldmap-scheduler] MISSING CALLBACK FRONTIER slot=4 state=1 cb=0x8008c844
counters: dispatch=20 executed=19 missing=1 outcome=1 frontier=0x8008c844
```

```
canonical_head=2656c3ab33066ccd9f5c09eace2c31596fcd876a
previous_frontier=0x8008B644
executed_function=0x8008B644
return=1
next_callback_target=0x8008C844
next_callback_slot=4
next_callback_state=1
next_callback_class=MISSING
new_frontier=0x8008C844
```

Slot 3 shares this cb1 address but its cb0 returned 3 (dormant); the
scheduler does not dispatch it on this pass. Scheduler executed
callbacks: 18 → **19**. NEW CALLBACK FRONTIER: **`wm_8008C844`
(slot-4 cb1, state 1)**.
