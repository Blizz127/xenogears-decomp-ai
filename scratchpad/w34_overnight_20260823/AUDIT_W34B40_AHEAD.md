# W34B40 audit-ahead — held frame backedge and post-convergence regions

## Scope and current cut

The W34B39 production fix is committed as `80d1f0b8`.  The natural route is
clean through the first frame tail: scheduler pass 2 executes `29/29` slots,
the guest-native OT walk reaches its terminator at `0x8009CE6C`, submits two
packets, and records no adapter abort.  The run still holds at the first
backedge:

```
0x800719C8 -> 0x8007130C   D_8009D554 = 1, held=1, hit=1
```

The held-backedge implementation is intentional.  This audit does not add a
second frame, remove a guard, or route a raw guest callback.

## D1 convergence result

The existing convergence lane covers the bounded work between the mode-entry
region and the common tail:

| retail range | current coverage | status |
| --- | --- | --- |
| `0x80072238..0x80072378` | mode-entry state stores and state-transfer bridge | built behind explicit gates |
| `0x80072378..0x800726C0` | setup/audio/pre-convergence cut | built/guarded |
| `0x800726C0..0x80072728` | convergence p1 | built |
| `0x8007272C..0x80072780` | convergence p2 | built |
| `0x80072784..0x8007290C` | excluded arms | deliberately guarded |
| `0x8007290C..0x80072998` | common tail p0..p5 | built; sentinel is `0x8007299C` |

There is no uncovered class-(a) or class-(b) gap on this lane.  The next
unimplemented portions are large functions or deliberately excluded arms,
so D1 does not authorize another production slice while the frame driver is
held.

## Audit 1 — frame-loop re-entry (`0x8007130C..0x800719C8`)

This is the actual per-frame loop, not the session loop.  The retail frame
driver begins at `0x800712D0`, reaches the frame head at `0x8007130C`, runs
the scheduler/update/render preparation, and branches back from `0x800719C8`
when `D_8009D554 != 0`.  The current port has the first pass and first tail
wired, but the second natural iteration would create another scheduler pass
and expose callback state that has not been reviewed for repeated execution.

Relevant calls and boundaries are already catalogued in
`scratchpad/w34b18_world_loop_7106c/RETAIL_CFG.md` and the W34B34–W34B39
slice logs.  The region includes DrawSync/VSync-style synchronization,
callback dispatch, upload-pump state, OT setup/walk, and the D554 backedge;
it is not a leaf.  The surrounding frame-driver range is approximately
`0x780` bytes (`0x800712D0..0x80071A48`, about 480 instructions), and a
second scheduler pass has unresolved repeated-state questions.

Classification: **class-(e), BLOCKED-NEEDS-REVIEW**.  The explicit session
rule also forbids implementing the second-frame/backedge iteration here.
The correct next action is a fresh callback-state census at the held edge,
followed by a reviewed bounded re-entry slice in a later session.

## Audit 2 — mode/session initializer (`0x80072238..0x80072998`)

Retail identifies `0x80072238` as the one-shot session initializer.  It is
not the per-frame loop and it is not required before the already-running
first frame can finish its held-tail analysis.  The raw listing and call
graph in `W_72238_RAW.txt`, `POST_CONVERGENCE_CALL_GRAPH.csv`, and
`DETOUR_D1_CONVERGENCE_AUDIT.md` establish an approximately `0x764`-byte,
472-instruction function.

The region performs the following classes of work:

- clears the display and waits through `ClearImage`/`DrawSync`;
- loads or archives CD data and polls asynchronous work;
- copies the A180 configuration block into the BE4C session block;
- seeds CCA4/D3CC/D804/CEC0/C7E8/BD34/D144 and related session globals;
- installs the `CD40 = 0x80086700` callback target and performs later setup;
- enters the existing convergence pieces and eventually the common tail.

The complete direct-call inventory, with current port/guard status, is
banked in `POST_CONVERGENCE_CALL_GRAPH.csv` and the raw disassembly.  The
important unresolved or gated calls include the prerequisite at `0x80072BB0`,
the setup helper at `0x80072DB4`, and later guarded setup/audio/resource
helpers.  Existing supported shims include DrawSync, ArchiveCdDataSync,
polling, VSync, and GfxAllocateWorkBuffers; these do not make the whole
function bounded.  The region contains multiple sync waits, stateful setup,
and excluded arms, with no small isolated leaf that would advance the held
frontier.

Classification: **class-(e), BLOCKED-NEEDS-REVIEW**.  Keep the
`0x80072238` should-not-run tripwire intact.  Do not implement this region as
a speculative route to the mode milestone.

## Audit 3 — post-loop teardown (`0x8007299C..0x80072BAC`)

The address called the “renderer” by the milestone naming is, in the retail
control flow, the session teardown callback entered after `0x800712D0`
returns and the D7CC session count is below two.  The exact function range is
`0x8007299C..0x80072BAC` (exclusive end near `0x80072BB0`), approximately
`0x214` bytes.  It is not a first-frame prerequisite.

The 26-call inventory is banked in `WM_8007299C_AUDIT.md` and
`POST_CONVERGENCE_CALL_GRAPH.csv`.  Its body includes:

- a 64-slot loop that tests/frees slot payloads at `+0x4C`;
- resource and graphics cleanup through the existing heap/GPU helpers;
- calls around `0x80092DD0`, `0x800931B0`, `0x80084818`, `0x80086124`,
  `0x80086568`, `0x800866C8`, `0x8007474C`, `0x80074F04`, `0x800750DC`,
  `0x80088FF4`, `0x80089128`, and `0x80097D64`;
- heap frees for BC38/BCB0/BC3C/BCB4 and related session resources.

Classification: **class-(e), BLOCKED-NEEDS-REVIEW**.  It has a loop,
resource ownership, and many direct callees; implementing it would not move
the first-render frontier and would violate the bounded-slice rule.

## Audit-ahead conclusion

The critical path is now upstream frame continuation, not the OT adapter and
not a missing convergence helper.  W34B39 removed the first-frame OT abort;
the remaining frontier is the deliberately held D554 edge.  The mode-loop
and teardown regions are documented for morning review, but no production
change is justified in this session.

## Evidence references

- `slice_13_natural.log`: clean W34B39 natural run and tripwire counts.
- `AUDIT_W34B39_BUCKET320.md`: watchpoint proof and retail global address.
- `DETOUR_D1_CONVERGENCE_AUDIT.md`: convergence coverage and gap audit.
- `DETOUR_D3_TRIPWIRE_AUDIT.md`: guard inventory and zero-hit verification.
- `w34b18_world_loop_7106c/RETAIL_CFG.md`: session loop versus frame loop.
- `w34b4a_post_convergence_audit/WM_8007299C_AUDIT.md`: exact teardown audit.
