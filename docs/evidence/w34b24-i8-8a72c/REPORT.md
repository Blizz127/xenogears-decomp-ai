# W34B24-I8 — wm_8008A72C Implementation Report (Slot-1 World Player Callback)

Milestone G4 rung. Base: canonical `7613dff` (keystone 95414 landed;
dependency closure MISSING=0 per W34B24-PRE3, committed alongside).

## Identity

| Field | Value |
|---|---|
| Function | `wm_8008A72C` (scheduler slot-1 cb1) |
| Boundary | `[0x8008A72C, 0x8008B2BC)` = 2960 B / 740 insns |
| SHA-256 | `1d58efac94432cb6892462260a1d7679dfcfddc9d0568b08244b1fd7cf1b4b3f` |
| Source | `pc_port/src/world_map_callback_8a72c.{c,h}` |
| Test | `pc_port/tests/w34b24_i8_8a72c_prod_test.c` + `run_w34b24_i8_8a72c.sh` |

Signature `s32 wm_8008A72C(s32 slot_idx)`; returns constant 1 on every path.
Full contract in the header comment + the PRE3 audit pack (committed with
this rung). JT1's complete 65-slot map is recorded in the source comment
(19 live targets verified word-by-word from the image; 46 parked; OOR ->
common tail). All PRE3 prose claims were re-derived from the instructions
during implementation; no inversions were found in this function (the I7
lesson applied — instructions win).

## Scheduler registration

Followed the accepted pattern exactly: weak `extern s32 wm_8008A72C(s32)`
in `world_map_scheduler.c`, a `wm_sched_builtin_8008A72C` s16 thunk, and a
resolver arm for guest address `0x8008A72Cu`. The address remains in
`s_wm_sched_known_missing` (the fallback classifier); the resolver finds
the strong body first, exactly as for the 17 previously accepted callbacks.

## Certification

- O0 / O2 / UBSan-nonrecovering: PASS, normalized stdout byte-identical.
- Oracle coverage (all callees seam-forced, hand-derived expectations):
  substates 3/2/6 (lap gate closed and open), JT1 OOR guard, parked
  states, resync path (incl. +0x24 suppression of 74794), JT2 classes
  1/3(area==7 and !=7)/4, the movement matrix — r=1 straight commit with
  ring append + 7528C; breadcrumb gate closed; area==7 +3 bias; r=0->1
  retry (4-word velocity redirect via the +0x44 residue); r=0->0 double
  failure (only +0x40/+0x38 zeroed; else-branch 8C040 args); r=0x10000
  s16-width retry discrimination; velocity-zero idle-anim arm — ring wrap
  0x1F->0, teleport refill (32 entries incl. the faithful stale scratch
  word and index reset), ring reset from slot pos, mode-wait arms
  (1/2-busy/3/0), slot-2 handshake all three arms (absent, quiet, busy —
  pool-relative +0x286 vs own +0x86 discriminated), slot-4 claim hold/go,
  approach setup/step, slot-3 release both arms -> 0x40, follow slot-7
  copy, tail mirrors + 74794 gate + constant return 1.
- Mutants: **17/17 KILLED** (MUTANTS.csv). Honest notes: the retail
  `sltiu 0x41` guard mutant is expressed as a 0x2D guard because
  0x41->0x40 is behaviorally invisible (slot 0x40 parks to the same
  tail); the breadcrumb `sll 16/sra 15` sign extension cannot be mutated
  observably (area <= 258 never sets bit 15) and is transcribed
  faithfully instead.
- Build: clean + incremental LINK OK; exactly one strong `T wm_8008A72C`;
  no stub shadow; stubs 240/524 unchanged.
- Suites: 23/23 PASS (22 accepted + this rung). Hardened `w18b_natural`
  harness: verdict=PASS exit=0, 13x ZERO_VERIFIED.

## RUNTIME PAYOFF — THE CALLBACK FRONTIER MOVED

Frontier measurement (natural Lahan route, full deep-gate chain,
DISPLAY=:10), first run with this body linked:

```
[worldmap-scheduler] slot=1 state=1 cb=0x8008a72c executed ret=1
[worldmap-scheduler] MISSING CALLBACK FRONTIER slot=2 state=1 cb=0x8008b644
counters: dispatch=19 executed=18 missing=1 outcome=1 frontier=0x8008b644
```

- `wm_8008A72C` executes naturally (a0=1) and returns 1; no crash, no
  assert, placeholder stable, clean run.
- Scheduler executed callbacks: 17 -> **18**.
- NEW CALLBACK FRONTIER: **`wm_8008B644` (slot-2 cb1, state 1)**.

## Host adaptations

None beyond the accepted conventions (PSX_ADDR model incl. the raw-low-32
native object pointer at slot+0x4C, following the accepted
`world_map_callback_8a2c8.c` precedent). No dispatch cap was needed — the
function is a single pass, not a loop.
