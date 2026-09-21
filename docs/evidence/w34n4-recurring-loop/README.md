# W34N4 — retail recurring world session/frame loop

## Outcome

W34N4 decoded the retail structure around `0x80071034` far enough to bound a
later transcription rung. The central correction is that `0x80071034` is a
**session-loop label inside `WorldMapMain`**, not a frame-loop function entry.
One session invokes a mode-table slot-1 callback, the scheduler, the complete
`0x800712D0` frame driver, and a slot-2 callback. Displayed frames recur only
on the inner `0x800719C8 -> 0x8007130C` back-edge.

This rung changed no production source and made no runtime-design choice. The
supporting phases are:

- [PHASE1.md](PHASE1.md) — exact nested control-flow shape, calls, and exits;
- [PHASE2.md](PHASE2.md) — mode table and callback targets;
- [PHASE3.md](PHASE3.md) — W34N3 state dependencies and ordering verdict.

## Anchor and retail authority

The rung began on `experiment/worldmap-open-gates-20260823` at
`2ca47f9720ee4d18c59f729124e6bd218345ccae`, equal to origin. Its backlog
authority is `docs/evidence/w34n3-gate-census/README.md`.

Retail authority is:

- `disc/world_map.bin`, loaded at `0x8006FAF0`, SHA-256
  `4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70`;
- direct decode using `mips-linux-gnu-objdump -D -b binary -m mips:3000 -EL
  --adjust-vma=0x8006faf0 disc/world_map.bin`;
- `config/symbol_addrs.slus_006.64.txt` for named system/PSYQ targets.

The pre-existing
`scratchpad/w34b5g_scheduler_80097800/WORLDMAIN_71034_710A0{,.raw}.txt`
agrees over its limited range and was used only as a cross-check. Port status
was derived separately from the current `pc_port/src` and compiled `src`
sources. No retail control-flow conclusion was reconstructed from the port.

## Retail structure in one view

```text
WorldMapMain pre-session work
  0x80071000: mode-table slot 0 (null permitted)
  0x80071034: session head
      mode-table slot 1
      0x80097800 scheduler
      DrawSync(0) -> Vsync(0) -> ControllerResetState
      C894 = D7CC
      0x800712D0 frame/session driver
          0x800712D0..0x80071308 session prologue
          0x8007130C..0x800719C8 displayed frame
          if D554 != 0: back to 0x8007130C
          if D554 == 0: 0x800719D0..0x80071A4C teardown/return
      mode-table slot 2
      if signed D7CC >= 2: back to 0x80071034
  if D7CC == 0: field-state terminal lane
  if D7CC == 1: transition/audio terminal lane
  otherwise signed D7CC < 2: default terminal lane
  common epilogue -> MainLoop(0) -> return
```

Base modes 0 through 7 resolve the three table slots to:

| slot | retail target | role |
|---|---|---|
| 0 | `0x80071CDC` | pre-session mode/archive request setup |
| 1 | `0x80072238` | full base-world session setup |
| 2 | `0x8007299C` | full base-world session teardown |

Modes 8 through 18 use `0x80071EF0` as slot 0 and mode-specific slot-1/slot-2
targets listed in Phase 2. Their bodies and reachability were not decoded in
this rung.

## Scoped later implementation work

### Control flow to transcribe

1. **Containing `WorldMapMain` control around `0x80071000..0x800712CC`.**
   Integrate the slot-0 dispatch, session loop at `0x80071034`, signed `D7CC`
   session decision, all three terminal lanes, and common epilogue. Do not
   preserve `0x80071034` as though it were a complete retail function boundary.
2. **Frame/session driver `0x800712D0..0x80071A4C`.** Merge the two existing
   partial port representations into one active retail-ordered path, including
   the single inner frame back-edge, natural exit, and correct return-bearing
   `0x800967E4` retry loop.
3. **Base slot 1 `0x80072238..0x80072998`.** Most bounded setup bodies already
   exist; the work is primarily to integrate them into one callable retail
   sequence, filling only proven missing seams. It must replace the external
   implication ladder, not run in addition to it.
4. **Base slot 2 `0x8007299C..0x80072BAC`.** This teardown body is absent and
   needs transcription, including its 64-object free walk and resource/audio
   lifecycle.

The hexadecimal end addresses above describe the decoded inclusive
instruction spans; Phase 2 records the corresponding half-open callback
boundaries `[0x80072238,0x8007299C)` and `[0x8007299C,0x80072BB0)`.

### Missing frame helpers that require transcription

The active driver calls unresolved bodies at:

- `0x80075D4C`
- `0x8007634C`
- `0x80076594`
- `0x80075E7C`
- `0x800758C0`
- `0x80075B58`
- `0x80096694`

Terminal lane `D7CC == 0` also conditionally calls absent `0x80094364`.
Whether every helper must precede a base-mode first implementation can be
decided only from the chosen bounded state coverage; none may be silently
replaced by a neutral default in a claim of complete retail transcription.

### Existing bodies that need integration, not retranscription

- system work-list functions `0x800250E0`, `0x8001D468`, and `0x80025044`;
- upload pumps `0x80074F2C` and `0x80075104`;
- the bounded `0x800762FC` sequence currently private in `world_map_init.c`;
- scheduler `0x80097800` at both its session-entry and per-frame positions;
- controller/system/PSYQ calls already compiled into the port;
- guest-OT clear/draw adapters already used for guest RAM;
- the transcribed `0x80071CDC`/`0x80071EF0` pre-session bodies and W34N3
  setup-stage bodies.

The later implementation must remove the active driver's local stubs that
shadow these real bodies; the existence of a body elsewhere is not evidence
that the active loop currently calls it.

### Callback resolution order

For the accepted base-world mode, `0x80072238` must resolve before the session
loop can become the normal first-session path, and `0x8007299C` must resolve
before natural session exit can be accepted. Mode-8..18 callback pairs are a
separate generality requirement; they do not block a deliberately bounded
base-mode implementation, but they do block calling it a complete all-mode
`WorldMapMain` transcription.

## Ordering against W34N3

The later work has two separable gates:

1. **Parallel-safe work:** transcribe the nested control flow and certify it
   against synthetic guest state and the retail instruction sequence.
2. **Natural integration:** wait for W34N3 backlog item 1 and item 2 to be
   integrated in retail order. Item 1 remains blocked at the field-to-world
   WDS/SPU ownership lifecycle. Item 2 then supplies framebuffer, terrain,
   convergence, common-tail, and scheduler state. Retail `0x80072238` owns
   those stages and `0x8007299C` tears their results down.

Therefore the transcription is **parallel to** the remaining spine work, but
making it the normal product route is **strictly downstream of both backlog
items**. A later rung must not bypass this by retaining the implication ladder
as hidden pre-initialization.

## Acceptance oracle after transcription

The current frame-60/frame-120 digests are not an oracle for this change. They
were produced by a bounded seam that skips retail slot 1 on its first session,
bypasses slot 2 at bounded exit, and substitutes local defaults for retail
calls. Correct control flow can legitimately change both images and timing.

Replace those digests with four layers:

1. **Focused structural certificate against retail decode.** Assert exact
   mode-table indexing and slot order; one slot-1 and slot-2 call per session;
   scheduler positions; one session prologue; the `D554` frame back-edge; the
   signed `D7CC >= 2` session back-edge; and all `D7CC` terminal lanes. Mutants
   must include swapped slots, missing/double scheduler, frame/session
   back-edge confusion, unsigned `D7CC`, skipped teardown, and a bounded-exit
   off-by-one.
2. **Retail dynamic event trace.** For the same mode/state, compare call
   addresses and state tuples at slot dispatch, scheduler passes, frame head,
   `D554` branch, slot-2 return, and `D7CC` decision. A PCSX-Redux/interpreter
   trace with an equivalent savestate is needed to create this oracle; it does
   not currently exist in the repository.
3. **Port natural/bounded health.** Exactly 120 displayed frames for the
   bounded harness, one effective presentation per displayed frame,
   capture-request frame equal to fulfillment frame, no pending capture at
   exit, zero adapter aborts, and a separate natural-exit run that executes
   frame teardown and slot 2.
4. **New deterministic visual baseline.** After the structural and dynamic
   oracles pass, run the final route twice and require matching new frame-60
   and frame-120 digests between those runs. Bank those as the successor
   baseline; do not compare them to the current open-loop hashes.

Synthetic tests should cover `D7CC == 0`, `D7CC == 1`, other signed values
below 2, and the `D7CC >= 2` session repeat. Production state must not be
forced merely to make the natural acceptance pass.

## Undetermined items

- **Formal mode-table range.** The retail image has 19 contiguous
  pointer-shaped records, but no array-length symbol or writer proof for
  `C5A8 <= 18` was found. The writer/range proof for `0x8009C5A8` is needed.
- **Modes 8..18.** Their exact slot-body semantics, state dependencies, and
  product reachability require separate retail decodes and route evidence.
- **Dynamic retail cadence/state oracle.** Static binary decode proves the
  branches and calls, not the naturally observed sequence of `D554`, `D7CC`,
  mode changes, or conditional lanes. An equivalent retail emulator savestate
  and trace harness are needed.
- **Exact per-global last writers.** Phase 3 classifies gated ownership by
  setup stage, but last-writer timing for `BD34`, `C178`, `D804`, `BD24`,
  `CE68`, `D80C`, `D55C`, and related transition state remains dynamic-trace
  work.
- **Absent helper semantics.** The frame and terminal helpers listed above
  require their own bounded retail decode before transcription.

These unknowns do not prevent a bounded base-mode control-flow implementation
rung. They do prevent claiming complete all-mode parity or using the current
forced-route captures as retail proof.
