# W34-OVERNIGHT handoff — current W34B37 state

## 1. Frontier and final evidence

The run began at the W34B34/W34B35/W34B36 result `0x800719C8` and tested the
first natural frame tail through DrawOTag at `0x800719B4`. The working-tree
W34B37 writer conversion plus ClearOTagR guest mapping advanced the call into
PsyCross, but the natural run stopped with SIGSEGV in
`ParsePrimitivesLinkedList` at `PsyX_GPU.cpp:906`.

Evidence: `slice_10_ot_representation.log`; audit:
`AUDIT_W34B37_OT_LINKS.md`. Before the crash: frame 916; scheduler pass 2
`29 executed / 0 missing`; upload pumps completed `2` and `3` transfers with
zero unknowns; DrawOTag entered naturally once; D554 was not re-entered.

## 2. Milestones

- Milestone 1 (`0x80072238` mode loop): not reached; guard intact and zero-hit.
- Milestone 2 (`0x8007299C` renderer): not reached; guard intact and zero-hit.
- Milestone 3 (nonzero framebuffer): not reached; no PNG produced.

## 3. Commit chain

| hash | slice | frontier delta |
| --- | --- | --- |
| `2c9ec8f1` | W34B34 upload pump `0x80074F2C` | `0x80071984 -> 0x80075104` |
| `9b3da910` | W34B35 upload pump `0x80075104` | `0x80075104 -> 0x80071994` |
| `0cfeaaeb` | W34B36 frame tail + D554 hold | `0x80071994 -> 0x800719C8` |
| `ba213582` | W34B37 BE3C audit + callback census evidence | no production delta |

The W34B37 production changes are intentionally uncommitted because the
natural DrawOTag run is not clean.

## 4. Attempted/reverted slices

W34B37 attempted the class-(a) BC38/BCB0 host-pointer conversion and the
necessary ClearOTagR `PSX_ADDR` handoff. The focused O0/O2/UBSan conversion
certificate and LINK OK build passed. No implementation slice was committed;
no slice was reverted. The newly exposed DrawOTag crash is the subject of the
blocked OT ABI audit, not a downstream paper-over.

## 5. BLOCKED-NEEDS-REVIEW

`AUDIT_W34B37_OT_LINKS.md` records the blocker. Retail uses 0x400 four-byte
OT entries in a 0x1000-byte allocation and guest 24-bit links. The production
binary's non-extended PsyCross ABI uses 8-byte padded `OT_TAG`/`P_TAG` types,
so ClearOTagR strides 8 bytes and `root+0xFFC` is not a valid host tag
boundary; guest packet links also differ from PsyCross's low-24 host links.
Morning review must choose between a world-only guest-OT adapter and a
PsyCross-wide ABI repair.

## 6. Detours completed

D1 convergence audit, D2 audit-ahead packets, D3 tripwire hygiene, and D4
evidence/worktree hygiene remain banked from the prior handoff. W34B37 added
the full 16-slot held-tail census: all predicted cb1 callbacks are mapped;
slots 3, 6, 7, and 11 are dormant; retail backedge target is `0x8007130C`.

## 7. Tripwire status

All existing should-not-run guards remain intact. The mode-loop and renderer
guards are zero-hit; the separate world DrawOTag guard was not weakened. No
tripwire was retired by implementation, and no second-frame/backedge code was
added.

## 8. Recommended next task

Have morning review approve the OT representation direction, then add a
focused OT-layout/link certificate before rerunning the natural route. Do not
implement the second frame iteration until DrawOTag completes safely.

## 9. Confirmation

Nothing was pushed. Quarantined tracked dirt in `include/psyq/inline_c.h` and
`pc_port/src/game_overrides.c` was not staged, reverted, or modified. Banked
proof trees were not re-baselined. No framebuffer PNG exists.
