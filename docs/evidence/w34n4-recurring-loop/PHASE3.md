# W34N4 Phase 3 — recurring-loop state dependencies

## Scope and authority

The retail read sites and ordering in this phase come from the direct
`disc/world_map.bin` decode documented in Phase 1. The producer names and gate
ownership come from the committed W34N3 backlog. Port files are used only to
identify how the accepted forced route currently supplies the same state; they
are not used to reconstruct retail control flow.

The table below is deliberately limited to state that the recurring session or
frame loop reads and that the accepted forced route establishes through a
W34N3 gate. Controller state, system globals, and static overlay data are noted
where they are loop inputs, but are not falsely assigned to a W34N3 gate.

## Session-level dependencies

| retail consumer | state read | W34N3 producer | backlog ownership | current forced-route status |
|---|---|---|---|---|
| `0x80071000`, `0x80071034`, `0x8007109C` | mode index `0x8009C5A8` and fixed mode table `0x8009A058` | normal selector / `INIT`, plus loader static-data residency | **item 1** for the mode; loader substrate for the table | the implication ladder establishes the selector state before entering the standalone loop |
| pre-session slot 0 (`0x80071CDC` for modes 0..7) | world tuple/entrance and archive-request state | `INIT -> MODE_INIT` | **item 1** | executed earlier by gated initialization; omitted from the standalone `0x80071034` seam |
| slot 1 `0x80072238` | archive waves, loaded buffers, mode/entrance state, static tables | `SECOND_WAVE` through `ARCHIVE_SET_INDEX` | **item 1**, with WDS/SPU lifecycle as the non-mechanical blocker | current route executes the callback's slices under gates and then skips the first slot-1 dispatch |
| `0x80071084..0x80071094` | `D7CC`, copied to `C894`, then consumed by `0x800712D0` and callbacks | initialized/updated by setup and recurring callbacks | spans **item 2** and recurring execution | forced route supplies a usable latch; the bounded exit does not exercise its natural session transition |
| session-entry scheduler `0x80097800` at `0x80071064` | scheduler pool `0x8009BE24`, registered callback slots, object pointers | `OBJECT_POOL`, convergence registration, common tail | **item 1** creates the pool; **item 2** populates/registers it | current route gates both prerequisites and calls the scheduler before its standalone first session |
| slot 2 `0x8007299C` | audio/WDS ownership, scheduler objects, graphics/work allocations, `D7CC` | first WDS consumer plus post-archive audio/common-tail setup | **items 1 and 2** | bounded exit bypasses slot 2 entirely; natural teardown is untested |

The mode table itself is executable-resident data populated by the static-data
loader work completed before this rung. It is not one of W34N3's remaining
gates. The mode index and the data that make its selected callbacks safe to
execute are gated state.

## Frame-driver dependencies

### Display, OT, and projection state

Retail `0x800712D0` initializes the active environment selector `BE3C`, but it
expects the two environment records at `0x8009BBC8` and `0x8009BC40`, their OT
roots at `+0x70`, framebuffer/GTE state, and geometry offset state to exist.
Those are produced by `wm_80072BB0` and the setup around it:

- `FRAMEBUFFER_GTE_INIT` (`0x80072BB0`) — **item 2**;
- `OBJECT_POOL`, `GFX_WORK_BUFFERS`, `FT4_POOLS`, upload-record and draw-packet
  stages — **item 1** supplies the allocations and packet substrate;
- `TERRAIN_POSITION_INIT` plus common-tail/scheduler callbacks — **item 2**
  supplies the position/camera-dependent values, including the value ultimately
  consumed by `SetGeomOffset(160, BE0C)`.

The current route provides these through gates before `wm_800712D0` runs. A
control-flow transcription can address the same guest globals, but cannot
meaningfully render without these producers.

### Archive/CD and ready-buffer state

The recurring frame's `0x800967E4` retry loop and `CdSync` consume archive/CD
queue state rooted in the archive waves and post-archive continuation:

- archive requests, loaded buffers, and readiness polling — **item 1**;
- the now-unconditional `0x800967E4` dispatcher at the post-archive sequence
  point — already retired from gating by W34N3;
- ready-buffer ownership/consumption — `READY_BUFFER_CONSUME`, **item 2**.

The active port driver currently loses the retail return value from
`0x800967E4` and therefore cannot reproduce the retail `return == 3` Vsync
retry. That is a port transcription divergence, not a missing W34N3 producer.

### Scheduler, world objects, terrain, and camera

The per-frame scheduler call at `0x80071488`, the world callbacks it invokes,
and the terrain/camera render chain consume:

- pool base `BE24` and object allocation — `OBJECT_POOL`, **item 1**;
- mode-enter templates, cross products, object matrix, GPU assets, primitive
  templates, CLUTs, FT4 pools, lookup tables, upload records, and draw packets
  — **item 1**;
- ready-buffer state, mode audio, convergence P1/P2, framebuffer/GTE state,
  terrain position, and common-tail callback registration — **item 2**;
- live pose/position, area-selection, camera matrix, geometry offset, and
  transition latches — recurring scheduler callbacks, seeded by the item-1 and
  item-2 state above.

The frame driver's tests of `BD34`, `C178`, `D804`, `BD24`, `CE68`, `D80C`,
`D55C`, and related presence/transition fields are therefore downstream state,
not independent zero-initialized defaults. The committed port sources show
which callbacks read/write them, but a complete retail producer proof for each
individual word is outside this phase. Their ownership category is established
at the stage level; exact per-word last-writer timing is **undetermined** and
would require a retail dynamic trace.

### Inputs not owned by W34N3

The controller drain reads the six named controller-state globals and writes
the world input words `CD4C`, `CD50`, `BD10`, `BD14`, `BD18`, and `BD1C` each
frame. This is normal system/controller state, not item 1 or item 2. The frame
driver also reads system soft-reset, menu, and game-state globals. These are
external inputs to the world loop and should remain injected or controlled by
the focused harness; they are not reasons to keep a world initialization gate.

## Natural exit dependencies

The frame-driver exit `0x800719D0..0x80071A4C` uses initialized display state
and the session latches. The following slot-2 teardown `0x8007299C` frees or
stops state from both backlog groups:

- item 1: object pool, graphics/work allocations, WDS ownership, archive and
  upload allocations;
- item 2: audio manager state, registered scheduler objects, and the final
  common-tail state that makes those objects live.

Consequently, testing only the recurring back-edge while breaking at a bounded
frame count cannot certify natural teardown. A teardown test must allow
`D554 == 0`, execute slot 2, and observe the resulting `D7CC` session or
terminal decision.

## Downstream/parallel verdict

**Transcription is parallel; retail-faithful natural integration is strictly
downstream.**

- The instruction order, nested session/frame back-edges, dispatch table
  mechanics, and terminal lanes can be transcribed now and certified with
  synthetic guest state. That work does not require the WDS/SPU blocker to be
  solved first.
- Making that transcription the normal product route is downstream of **both**
  W34N3 backlog items. Item 1 must supply a naturally owned archive/WDS and
  allocation spine. Item 2 must execute the post-archive continuation that
  creates the framebuffer, terrain, convergence, common-tail, and scheduler
  state. Retail slot 1 `0x80072238` is itself the function that owns those
  stages, and retail slot 2 `0x8007299C` consumes their resulting lifecycle.

Thus the later implementation rung may proceed as a bounded control-flow
transcription with a focused oracle, but it must not claim natural-route
acceptance until the item-1 SPU-blocked spine and item-2 continuation have been
integrated in retail order.
