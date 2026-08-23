# W34B37 final report

1. **BE3C+0x70 mechanism:** Retail BE3C selects the two fixed draw-buffer
   records; `+0x70` is the selected 0x1000-byte OT root, consumed by both
   ClearOTagR and DrawOTag. The current-port writer is
   `wm_8007369C` in `world_map_init.c`, which stored raw truncated HeapAlloc
   host pointers at BC38/BCB0. BE3C itself is correctly guest-valued. This
   is class (a), not stale state or a host-allocated architectural record.

2. **Fix status:** The minimal writer conversion passed the asymmetric
   O0/O2/UBSan certificate. A necessary downstream ClearOTagR guest mapping
   was also tested by the natural route. DrawOTag then fired with a valid
   mapped OT pointer but crashed in PsyCross OT traversal. Because the
   natural run was not clean, no implementation commit was made.

3. **Callback census:** Full 16-slot state is banked in
   `slice_09_census.log` and `AUDIT_W34B37_BE3C_OT.md`. State 1 selects cb1;
   state 3 is dormant. Predicted next callbacks are all implemented and
   resolver-mapped. Retail re-entry is `0x800719C8 -> 0x8007130C`, then frame
   head to scheduler pass-start.

4. **Tripwires:** Mode loop `0x80072238`, renderer `0x8007299C`, and all
   existing forbidden-target guards remain intact and zero-hit. The direct
   mapped tail call did enter PsyCross DrawOTag; it did not weaken or remove
   the separate should-not-run guard.

5. **Not checked:** Valid completion of PsyCross OT traversal, nonzero
   framebuffer pixels, renderer entry, second frame iteration, and D554
   backedge execution remain unchecked. The OT linked-list crash must be
   resolved before the loop is considered safe.

6. **Confirmation:** Nothing was pushed. Quarantined tracked dirt was not
   staged or modified. The second-frame backedge loop was not implemented.
   Production changes from the attempted class-(a) fix remain uncommitted
   pending the DrawOTag crash decision.

7. **Exactly one next task:** Diagnose and review the PsyCross OT linked-list
   representation exposed by the mapped W34B37 DrawOTag call, then rerun the
   natural route; do not implement the second frame iteration until that
   crash is resolved.
