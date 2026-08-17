# W34B24-I14 — wm_800914D0 Implementation Report (Slot-9 cb1)

Continuation after G5 93484. Base: canonical
`5f3766cd29846188dfcf41e0c482944e92766a5f`. Live frontier after 93484
was missing slot-9 cb1 `0x800914D0`. MISSING=0 (93354, 93484, 97770).

## Identity

| Field | Value |
|---|---|
| Function | `wm_800914D0` (scheduler slot-9 cb1) |
| Boundary | `[0x800914D0, 0x80091B54)` = 1668 B / 417 insns |
| SHA-256 | `1d9e6b5882ef8d968a46c7c8f3d2699d719f2aa97dcd65eb1ad9d9bc76cfb14e` |
| File off | `0x219E0` (overlay base `0x8006FAF0`) |
| Source | `pc_port/src/world_map_callback_914d0.{c,h}` |
| Test | `pc_port/tests/w34b24_i14_914d0_prod_test.c` + `run_w34b24_i14_914d0.sh` |

ABI: `$a0` = slot index (observed 9); **all paths return 1**.
Sub JT0 9 entries at `0x80070BE4`, index `(s16)(lhu +4 - 9)`, sltiu 9.
JT1 17 entries at `0x80070C0C`, sltiu `0x11`, dests 0/1/2/0x10 + park.
4 JAL / 3 unique; no JALR; no COP2/GTE/GPU/OT.

- Sub 9 → state 2, +50=`lh 0x8009D52C`, +58=`lh 0x8009BD3A`<<12.
- Sub 0xA → state 0, +50=`lh BD3A`.
- Sub 0xF/0x10/0x11 → state 0x10, +50 = 0x800 / 0xA00 / 0xC00.
- State 0: pad `(lhu 0x8009CD4C >> 2) & 3` starts a ±0x200 heading
  interpolate (state 1), then syncs BD3A toward +50 (wrap at 0xC01,
  acc `(delta<<12)>>3`, mask `0x00FFFFFF`).
- State 1: `+50 = (+50 + +54) & 0xFFF`; arrive → state 0; then sync.
- State 2: chase BD3A toward +50 (wrap 0x801, clamp ±0x180, sra 3).
  Arrive and `+60==0` → state 3 + `wm_80097770(7, 0xB)`.
- State 0x10: same chase with sra 5; arrive → state 3; no 97770.
- Post-state: 0/1/2/0x10 approach `0x8009D55C` (93484 wrap, close
  abs(d>>3)<0x40 snaps X/Z and clears +60; else step and +60=1;
  Y always += dy>>3). State 3 snaps X/Z, Y += dy>>4, full BBB4/BBBC.
- Tail: `wm_80093354(slot+0x28)` then publish four words to `0x8009BE28`.

## Scheduler registration

Weak `extern s32 wm_800914D0(s32)`, `wm_sched_builtin_800914D0` s16 thunk,
resolver arm `0x800914D0u`. Address remains in `s_wm_sched_known_missing`.

## Certification

- O0 / O2 / UBSan-nonrecovering: PASS, normalized stdout byte-identical.
  Host gcc lacks `libubsan`; the runner falls back to clang for UBSan.
- Oracle: seam-forced callees, hand-derived expectations. Does **not**
  bake `NEXT_CALLBACK_TARGET`. Isolation canaries on slots 8/10, D52C,
  CD4C, and D55C.
- Mutants: **18/18 KILLED** (MUTANTS.csv).
- Build: clean + incremental LINK OK ×2; `compiled=47 skipped=0`; exactly
  one strong `T wm_800914D0` @ `00000000004acd40`; local thunk
  `t wm_sched_builtin_800914D0`; no stub shadow; stubs **240/524
  unchanged**.
- Suites: 27/29 `run_w34*.sh` PASS in distrobox (includes I9–I14). The two
  remaining (`run_w34b_r4world_73b04.sh`, `run_w34cb1_90a84.sh`) fail to
  compile under current gcc because `-fpermissive` is a C++-only flag
  promoted by `-Werror` — host toolchain, not a 914D0 regression.
- Fresh clean-export at `/tmp/w34b24-i14-clean-export`: PASS (18/18),
  `world_map.bin` copied as a regular file (180422).
- Isolation canaries (slots 8/10, D52C, CD4C, D55C) are part of the
  focused oracle. `w18b_natural.gdb` reached placeholder; the 180s
  timeout killed gdb before the final W18I verdict line (same host
  harness limit as prior rungs).
- `0x80071A58` was not implemented (not on the live path).

## RUNTIME PAYOFF — THE CALLBACK FRONTIER MOVED

Natural Lahan route, full deep-gate chain through
`XENO_WORLD_FRAME_PROLOGUE=1` (no forced PC/callback/slot/state).
`DISPLAY=:0`. First scheduler pass is Table-A cb0 init (16/16 executed).
Second pass is the live cb1 walk:

```
[worldmap-scheduler] slot=8 state=1 cb=0x800907f4 executed ret=1
[worldmap-scheduler] slot=9 state=1 cb=0x800914d0 executed ret=1
[worldmap-scheduler] MISSING CALLBACK FRONTIER slot=10 state=1 cb=0x80091c18
counters: dispatch=24 executed=23 missing=1 outcome=1 frontier=0x80091c18
```

```
914D0_BODY_EXECUTED=YES
914D0_RETURN=1
SLOT9_STATE_BEFORE=1
SLOT9_STATE_AFTER=1
SCHEDULER_COMPLETED_PASSES_BEFORE=1
SCHEDULER_COMPLETED_PASSES_AFTER=1
NEXT_CALLBACK_TARGET=0x80091C18
NEXT_CALLBACK_SLOT=10
NEXT_CALLBACK_STATE=1
NEXT_CALLBACK_CLASS=MISSING
FRAME_FRONTIER=0x80091C18
CRASH_OR_CUT_PC=0x80071490
```

Scheduler executed callbacks: 22 → **23**. NEW CALLBACK FRONTIER:
**`wm_80091C18` (slot-10 cb1)**. Direct JALs: `0x80091FF8` (missing
helper), `0x80096F18` (missing; PsyQ `RotMatrixYXZ` / `ApplyMatrixLV` /
`ApplyMatrix` only — MISSING=0 once those two overlay helpers land).
Accepted callees on the 91FF8 tail: `93354`, `93660`.
