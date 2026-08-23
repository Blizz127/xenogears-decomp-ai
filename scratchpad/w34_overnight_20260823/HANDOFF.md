# W34-OVERNIGHT handoff — W34B42 one-frame re-entry complete

## 1. Frontier and final evidence

The run began at the W34B34/W34B35/W34B36 held edge `0x800719C8`. W34B37
corrected the production OT-root representation, W34B38 added the narrow
guest-native OT adapter, and W34B39 corrected the retail slot-table global
used by particle cleanup. The first-frame natural route is now clean through
the OT walk, but the D554 frame backedge remains intentionally held.

Final natural evidence (`slice_13_natural.log`): frame 916; scheduler pass 2
`29 executed / 0 missing`; guest OT root `0x800A3224`; two packets submitted;
1025 walk steps; zero range/alignment/length/step aborts; D554 backedge
`0x800719C8`, `d554=1`, `held=1`, `hit=1`; rc=0. The control frontier is
therefore still `0x800719C8`, with the OT sub-frontier advanced to the guest
terminator at `0x8009CE6C`.

Fresh W34B41 census evidence (`slice_14_census.log`) reproduced frame 916
and rc=0 against the current binary. W34B42 then executed one reviewed
re-entry. Its final natural state (`slice_15_natural.log`) has two frame-tail
passes, four OT packets, 2050 guest-walk steps, scheduler entry 3 with
`41/41` callbacks, and rc=0. D554 remains nonzero after the second tail, so
the bounded control frontier is still `0x800719C8`.

## 2. Milestones

- Milestone 1 (`0x80072238` mode loop): not reached; entry state is zero-hit,
  should-not-run guard intact.
- Milestone 2 (`0x8007299C` renderer/retail teardown entry): not reached;
  entry state is zero-hit, should-not-run guard intact. Retail audit classifies
  this function as post-loop teardown rather than first-frame renderer work.
- Milestone 3 (nonzero framebuffer): not reached; no PNG produced.

## 3. Commit chain

| hash | slice | frontier delta |
| --- | --- | --- |
| `2c9ec8f1` | W34B34 upload pump `0x80074F2C` | `0x80071984 -> 0x80075104` |
| `9b3da910` | W34B35 upload pump `0x80075104` | `0x80075104 -> 0x80071994` |
| `0cfeaaeb` | W34B36 frame tail + D554 hold | `0x80071994 -> 0x800719C8` |
| `ba213582` | W34B37 BE3C audit + callback census evidence | no production delta |
| `71f9c56d` | W34B37 publish guest OT roots | OT root host/guest boundary corrected |
| `090f075d` | W34B38 guest-native world OT adapter | malformed host walk -> bounded guest walk |
| `07c7a9b8` | W34B38 bank adapter evidence | no production delta |
| `80d1f0b8` | W34B39 slot-table base at `0x8009BCC0` | bucket abort -> terminator, 2 packets |
| `215786b4` | W34B40 audit held backedge and post-convergence boundaries | audit only |
| `d17fb03d` | W34B41 fresh callback census | no production delta |
| `a3f20290` | W34B42 reviewed one-frame re-entry | one extra clean frame |
| `5a79d5b0` | W34B43 post-second-frame census | no production delta |
| `77ea585c` | W34B44 finite two-reentry bound | two additional clean frames |
| pending | W34B45 third-tail callback census | no production delta |

All production slices above have clean LINK OK and rc=0 natural evidence.
W34B40, W34B41, and W34B43 contain evidence/audit only. W34B42 adds one
reviewed frame and W34B44 adds a finite second re-entry; the unbounded retail
session loop remains deferred.

## 4. Attempted/reverted slices

W34B37 initially exposed a DrawOTag ABI crash; that attempt was resolved by
the committed W34B37/W34B38 OT representation work. W34B39 was the only
implementation slice in the final clean run and was not reverted. W34B40
attempted no production implementation; it audited the held frame edge and
the next three large regions, then stopped at the explicit second-frame
restriction.

## 5. BLOCKED-NEEDS-REVIEW

`AUDIT_W34B40_AHEAD.md`, `AUDIT_W34B41_CALLBACK_CENSUS.md`,
`AUDIT_W34B42_SECOND_FRAME.md`, `AUDIT_W34B43_POST_SECOND_CENSUS.md`, and
`AUDIT_W34B44_TWO_REENTRY.md` record the current boundaries. Three frame
passes are clean and callback-covered, but D554 remains 1. The mode
initializer `0x80072238`
is approximately 472 instructions with unresolved/gated setup calls, and
`0x8007299C` is an approximately 0x214-byte, 26-call post-loop teardown.
Neither is a bounded first-render slice. The convergence lane has no
uncovered class-(a/b) gap.

## 6. Detours completed

D1 convergence and D3 tripwire audits remain banked. The W34B39 detour fixed
the slot-table base and completed the first guest-native OT walk. W34B40
audited the held frame re-entry, mode initializer, and post-loop teardown;
W34B41 freshly recaptured the full slot table and resolver coverage; W34B42
executed one reviewed additional frame; W34B43 recaptured the identical
second-tail table; W34B44 executed two additional bounded frames. D4 was not
used to alter unrelated worktree contents.

## 7. Tripwire status

| tripwire/guard | status |
| --- | --- |
| `0x80072238` mode-loop entry | intact; zero-hit naturally |
| `0x8007299C` renderer/teardown entry | intact; zero-hit naturally |
| separate world DrawOTag guard | intact; zero-hit; guest adapter is separate |
| frame backedge/second-iteration guard | intact; three hits held after two bounded re-entries |
| loop dispatch/exit guards | intact; zero-hit |
| scheduler missing/invalid callback guards | intact; pass 2 `29/29`, missing `0`, invalid `0` |
| W34B38 OT adapter abort guards | intact; naturally zero aborts in W34B39 |

No should-not-run tripwire was weakened or retired by implementation.

## 8. Recommended next task

The single recommended next task is a retail/current-port D554 clear-writer
census, especially callback paths capable of clearing the frame-run flag,
before any further loop extension. Do not implement the mode initializer or
teardown merely to force either milestone.

## 9. Confirmation

Nothing was pushed. Quarantined tracked dirt in `include/psyq/inline_c.h` and
`pc_port/src/game_overrides.c` was not staged, reverted, or modified. Banked
proof trees were not re-baselined and remain intact. No framebuffer PNG
exists.
