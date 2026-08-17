# W34B24-I10 — wm_8008C844 Implementation Report (Slot-4 Callback)

Continuation after G5 8B644. Base: canonical
`7293474c2e644691edfe561ef1ce0a42f538d835` (8B644 landed). Live frontier
after 8B644 was missing slot-4 cb1 `0x8008C844`. Its only unresolved
direct callee was the 90A84 sibling `0x80090C68` (MISSING=1). Both are
implemented on this rung.

## Identity

| Field | Value |
|---|---|
| Function | `wm_8008C844` (scheduler slot-4 cb1) |
| Boundary | `[0x8008C844, 0x8008D3F0)` = 2988 B / 747 insns |
| SHA-256 | `495f3cc8ea04d18faf94200ff6135542dab6dae3869d3967197f3ba280934516` |
| File off | `0x1CD54` (overlay base `0x8006FAF0`) |
| Source | `pc_port/src/world_map_callback_8c844.{c,h}` |
| Helper | `wm_80090C68` `[0x80090C68, 0x80090E14)` = 428 B / 107 insns |
| Helper SHA | `d34037e36ac530229ec6712033176c883262d0d69367a361cd28204c6308c6e7` |
| Test | `pc_port/tests/w34b24_i10_8c844_prod_test.c` + `run_w34b24_i10_8c844.sh` |

ABI: `$a0` = slot index (observed 4); `$s5` = 1; **all paths return 1**.
JT1 65 entries at `0x800707CC`, `sltiu 0x41`, 20 dests (19 live + park).
43 JAL / 18 unique; after 90C68, MISSING=0 (rcos/rsin + accepted helpers).

90C68 is the slot-4 class helper: same 12-way heading table as 90A84
(JT `@0x80070B84`) but the flag-0x20 path does **not** inspect
`object+0x0E`. Heading `& 0xF000` writes `+0x38 = rcos` (jal-rsin delay
slot) and `+0x40 = -rsin`.

8C844 is not an 8A72C clone. Material deltas vs the player handler:

- Substates 4/3/7/8 (not 3/2/6).
- Init class tree is hardcoded on `wm_80090C68` (classes 1/3/4), not JT2.
- Movement: `894C8`/`8C1DC` record `0x2C`; `8C040` scales `0x18/0x30`;
  `94238` index 1; 95414 retry tests the **full word**.
- State 2 copies slot-7 X/Z/heading and **skips Y**.
- State 0x0A falls into 0x0B; 0x20 falls into 0x21; 0x30 falls into 0x31.
- Common tail: `wm_80074794(1, slot+0x28)` unless `state==2` or
  (`MODE_FLAG!=2` && `lbu 0x8006F368==7`). Always sra12-publishes to
  `0x8006EF90/92` and `0x8006EE5A` (player warp inputs), not EE54/56/58.

## Scheduler registration

Weak `extern s32 wm_8008C844(s32)`, `wm_sched_builtin_8008C844` s16 thunk,
resolver arm `0x8008C844u`. Address remains in `s_wm_sched_known_missing`;
the resolver finds the strong body first.

## Certification

- O0 / O2 / UBSan-nonrecovering: PASS, normalized stdout byte-identical.
  Host gcc lacks `libubsan`; the runner falls back to clang for UBSan.
- Oracle: seam-forced callees, hand-derived expectations. Does **not**
  bake `NEXT_CALLBACK_TARGET`.
- Mutants: **22/22 KILLED** (MUTANTS.csv).
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_8008C844` @ `00000000004a96a5` and `T wm_80090C68` @
  `00000000004acb7d`; no stub shadow; stubs **240/524 unchanged**.
- Suites: 23/25 `run_w34*.sh` PASS in distrobox (bash-invoked where the
  script bit is off). The two remaining (`run_w34b_r4world_73b04.sh`,
  `run_w34cb1_90a84.sh`) fail to compile under current gcc because
  `-fpermissive` is a C++-only flag promoted by `-Werror` — host
  toolchain, not an 8C844 regression. Independent O0/O2 recheck of both
  (flag dropped; 73B04 uses `pc_port/build/libpsycross.a`): PASS.
- Fresh clean-export at `/tmp/w34b24-i10-clean-export`: PASS (22/22),
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
[worldmap-scheduler] MISSING CALLBACK FRONTIER slot=5 state=1 cb=0x8008d678
counters: dispatch=21 executed=20 missing=1 outcome=1 frontier=0x8008d678
```

```
canonical_head=7293474c2e644691edfe561ef1ce0a42f538d835
previous_frontier=0x8008C844
executed_function=0x8008C844
return=1
next_callback_target=0x8008D678
next_callback_slot=5
next_callback_state=1
next_callback_class=MISSING
new_frontier=0x8008D678
```

The hardened `w18b_natural.gdb` harness reached the scheduler payoff and
post-scheduler DrawSync/Vsync/712D0, then the 300s timeout killed the
placeholder-Vsync tail (exit 137). The frontier measurement is complete
without forced PC/callback/slot/state. Scheduler executed callbacks:
19 → **20**. NEW CALLBACK FRONTIER: **`wm_8008D678` (slot-5 cb1, state 1;
also registered as slot-6 cb1)**.

