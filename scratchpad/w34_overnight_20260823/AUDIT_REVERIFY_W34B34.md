# W34B34 audit re-verification and backedge policy

## Part 0 — fresh retail decode

Authoritative image: `disc/world_map.bin`, load base `0x8006FAF0`, SHA-256
`4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70`.
The read-only Capstone decode was rerun with
`scratchpad/w19a_74e58_audit/disasm_world.py`.

1. **`0x80074F2C`: confirmed.** The fresh range contains exactly 64
   instructions / 256 bytes, ending at `0x8007502C`. `CC9C` is loaded and
   tested with signed `blez`, so count `<= 0` skips the loop. `D780` supplies
   the record-array root. Each 12-byte record uses `+8` as the halfword frame
   index and `+A` as the halfword timer. The timer decrements with u16 wrap;
   only a resulting zero enters the transfer path. The record `+4` descriptor
   supplies its halfword table at `+0x0C`; the sole call is
   `0x80074FEC -> LoadImage(0x80044894)`. The signed index selects the table
   halfword, which is shifted left four for the source pointer.
2. **`0x80075104`: confirmed.** The fresh range contains exactly 73
   instructions / 292 bytes, ending at `0x80075228`. `CD64` is the signed
   count and `D7D0` is the record-array root. The sibling repeats the same
   12-byte record timer/index progression. Its descriptor reads halfwords at
   offsets `+4` and `+6` for dimensions, uses the indexed `+0x0C` table, and
   calls `LoadImage` at `0x800751E4`; entry fields at offsets `0/2/4/6` are
   consumed with the exact widths shown by the listing.
3. **Backedge: confirmed.** `0x800719C0` loads `D554` and
   `0x800719C8` branches to `0x8007130C` when nonzero. `D554` is set to `1`
   at `0x80071308` on frame-driver entry. The retail xref census records
   clears at `0x80071830`, `0x80071954`, and later session-ending paths;
   it is a frame-loop run flag, not a timer that self-clears after a fixed
   number of iterations. `0x8007130C` is already-executed frame-head
   territory, including the scheduler at `0x80071488`, but a repeated frame
   can expose changed callback states and therefore needs a fresh census.
4. **DrawOTag pointer: confirmed.** The tail loads `BE3C` at `0x800719A8`,
   reads `*(BE3C+0x70)` at `0x800719B0`, adds `0xFFC` in the delay slot, and
   calls `DrawOTag` at `0x800719B4`. `BE3C` is the current world frame-buffer
   environment pointer: `0x800712EC` seeds it to `0x8009BC40` and
   `0x80071458` flips it between the two environment records. The `+0x70`
   OT fields are populated by the earlier `0x8007369C` OT allocation/setup;
   the selected OT pointers are valid before this tail. No raw guest OT value
   was passed during this slice because the DrawOTag tail remains held.

No Part 0 discrepancy was found.

## Part 1 — backedge policy

Recommendation: **LOG-AND-HOLD on the first natural backedge encounter.**

- The target re-enters the already executed frame head and therefore reruns
  input drain, CD polling, OT reset, and scheduler pass 2. The captured
  scheduler census is 16 callbacks on pass 1 and 29 on pass 2, all present
  and executed. That proves the current slots are mapped, but does not prove
  that later state transitions cannot make a new callback state eligible.
- `D554` remains `1` through the natural upload-pump state and is cleared by
  event/session-ending branches, not by a bounded helper iteration. The
  backedge is consequently an unbounded game loop. It does not require the
  new upload helper to clear D554, but allowing repeated passes without a
  first-state capture would hide a new-callback census problem.
- The next slice should permit the tail through the first D554 branch,
  capture the complete state, and hold before any second iteration. This
  slice deliberately does not implement or cross that backedge.

## Part 2 — W34B34 implementation

Implemented the exact 64-instruction `0x80074F2C` pump in
`world_map_upload_pump_74f2c.c`. Known PSX/KSEG1 record, descriptor, table,
RECT, and source pointers are mapped with `PSX_ADDR`; unknown values are
logged/counting and that transfer is skipped. The production frame driver
now calls the helper at retail `0x80071984` and stops before `0x80075104`.

The focused certificate uses asymmetric records and checks signed index
wrap, u16 timer decrement/trigger, count gates, mapped LoadImage arguments,
and unknown-pointer behavior. Four mutants were all detected:
wrong record stride, wrong trigger condition, unsigned index, and missing
unknown guard.

## Natural result

The re-anchored GDB diagnostic completed with `rc=0` at frame 916:

```text
upload-pump entry count=2 array=0x800f2c58
upload-pump exit transfers=2 unknowns=0
fp_cut_pc=0x80075104
sched_entry=2 sched_callbacks_executed=29 sched_missing_hits=0
placeholder entered cleanly
```

The natural route advanced from `0x80071984` to `0x80075104`. The D554
backedge was not encountered because the slice stops before the tail branch.
