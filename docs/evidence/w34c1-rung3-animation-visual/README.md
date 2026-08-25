# W34C1 Rung 3 — animation visual acceptance (FAIL: scheduler cadence)

Date: 2026-08-25

Production HEAD: `065e94f51eafad4fd854a3a8a0ec876a0bf9fe3d`

## Acceptance route

The in-process schedule was:

`0:0x2000,600:0x4000,916:0x2000,976:0`

This was intended to drive world input through frame 60 and release it for
frames 61–120. The input-domain probe below proves that the post-handoff hook
did not reach the retail world controller consumer. Presentation-boundary
captures were nevertheless produced at frames 60 and 120:

- frame 60 BMP:
  `cbd7be8acc0672aea6c9c3fade53e288f3146cc8c7779a3d7c7e558a6cb2f6d4`
- frame 120 BMP:
  `bcdfc279969de6a13d81d3a9756d5f2fdda02ff1977aefd948d82cceeae58acf`

The external watcher also captured the live X11 window at both presentation
seams. Orca computer-use reported that this Linux provider does not support
screenshots, so the watcher preserved raw XWD images instead:

- frame 60 XWD:
  `c1b448a7644835c19a85c0d39c8aab10b951dfc7c200fa5216f9fa7e7c174ff7`
- frame 120 XWD:
  `fd94c6a49c7dc2728d30c2d8992bf84156b960b0f7fd7b33d9ceac4474e15c52`

Artifacts:

- `scratchpad/w34c1_rung3_capture/`
- `scratchpad/w34c1_rung3_window/`
- `scratchpad/w34c1_full_slot_census.gdb`
- `scratchpad/w34c1_full_slot_census_v2.log`
- `scratchpad/w34c1_rung3_render_identity.gdb/.log`
- `scratchpad/w34c1_assignment_input_domain.gdb/.log`
- `scratchpad/w34c1_rung3_live_pose_state.gdb/.log`
- `docs/evidence/w34c1-rung3-animation-visual/FIELD_MAP.md`

## Corrected entity and pose evidence

The pool at `0x800d7538` is the scheduler-slot pool. Each non-null `+0x4c`
value is a stable native sprite pointer, not a guest address. Direct renderer
identity evidence reaches `func_8001E298` for slots 1 and 2; slots 4 and 5
are suppressed and do not reach the renderer. A complete 64-slot census over
frames 1–30 found no additional non-null sprite pointers, so the earlier
pool changes in slots 0, 10, and 15 were scheduler metadata, not hidden
rendered entities.

The earlier slots-4/5 “gliding records” identification was therefore wrong.
Their native records are byte-identical at frames 1, 30, and 60 and remain
`animation=3`, `wait=1`, `pose=0`, but they are not the visible entities.

Renderer-eligible slots 1/2 retain `animation=0`, `wait=0`, and poses 1/41 at
every frame of the corrected 1–30 census. At the capture seams, their native
`+0x00/+0x04/+0x08` render-transform words remain fixed while `+0xd8/+0xdc`
move smoothly (slot 1 `(312,1516)` to `(378,1337)`; slot 2 `(0,1116)` to
`(0,999)`). This is consistent with camera-relative drawing; `+0xd8/+0xdc`
are not yet semantically identified and are not the render transform.

Those observations are valid, but the earlier conclusion that they were made
during world-player movement is not.

## Retail animation-id writer trace

The complete retail writer inventory for native sprite byte `+0xaf` is:

- `func_80023804` initializes it to zero at `0x800238A4`;
- `func_800245D8` is the general setter and stores the requested signed byte
  at `0x800246D0` (unless the default animation file is null, in which case it
  returns before the store);
- `func_80021D50` restores the byte from sprite snapshot `+0x14` at
  `0x80021D90`, then calls `func_800245D8` with that value.

A complete `world_map.bin` scan found no direct `sb ...,0xaf(...)`; overlay
changes go through `func_800245D8`. Both world sprite constructors initialize
their objects to animation 0. Their active natural state-zero callbacks already
contain the retail animation transitions:

- `wm_8008A72C` at retail `0x8008A8A8..0x8008A91C` selects animation 1 when
  any of slot `+0x38/+0x3c/+0x40` is nonzero (`+0xaf` test at `0x8008A908`,
  setter call at `0x8008A918`), and selects animation 0 when all three are zero
  (test at `0x8008A8D8`, call at `0x8008A8E8`).
- `wm_8008B644` selects animation 1 when the follower position differs from
  its breadcrumb-ring entry (test at `0x8008B7D4`, call at `0x8008B7E4`) and
  animation 0 when it matches (test at `0x8008B7A4`, call at `0x8008B7B4`).

These call sites exist in the port and agree with the retail control flow.
There is no missing animation-assignment call on the observed state-zero
route.

## Input-domain contradiction

The scripted world-loop continuation writes the field held-input word
`D_800AFE9C`. Retail world movement does not consume that word. The frame
driver clears `0x8009CD4C`, drains controller states, and ORs
`g_C1ButtonState` into that world accumulator. `wm_80090A84` then derives the
slot-1 heading and velocity from `0x8009CD4C`; the animation-1 guard consumes
that velocity.

A read-only frame-1 probe under the recorded schedule found:

```text
D_800AFE9C       = 0x2000
g_C1ButtonState  = 0x0000
0x8009CD4C       = 0x0000
slot 1 velocity = (0,0,0), animation=0
slot 2 velocity = (0,0,0), animation=0
```

Before Rung 3a, production `world_map_frame_driver_712d0.c` also contained a
separate retail transcription defect at this seam: it read guest-backed
`0x80069570/574`, `0x8006948C/490`, and `0x800694A4/A8`. Retail reads the
native controller globals at `0x80059570/574`, `0x8005948C/490`, and
`0x800594A4/A8`, as confirmed by the decoded `0x8007134C..0x800713F4` input
drain loop. This is both an erroneous `+0x10000` and the wrong pointer domain.

With a zero world accumulator, slot 1 correctly follows the idle arm and does
not redundantly reselect animation 0. Slot 2 has no moving leader breadcrumb
to follow and likewise remains idle. The animation-assignment hypothesis is
therefore void: the recorded run never drove the retail movement consumer.

## Rung 3a controller-source repair and live result

Commit `065e94f51eafad4fd854a3a8a0ec876a0bf9fe3d` replaces the six
fabricated `+0x10000` guest reads with the six distinct native controller
globals used by retail. The focused production-linked certificate passes at
O0, O2, and nonrecovering UBSan. Its source-order gate proves the retail
held/released/pressed-once order, and M1--M3 are detected by named assertions
for a restored bad guest source, an omitted source, and a missing initial
accumulator clear.

The live gate used a real X11 Right-key hold, not the scripted field-input
word. GDB detached before `PcPort_WorldMapInitMain`; Right remained held
through presentation frame 60 and was released immediately after that frame,
before frame 120. The presentation captures differ:

- held frame 60 BMP:
  `6aaf86edc2ec4ea1bb3ecdd0d2036e036d4394b4245cb9271ac9f43049ffd442`
- released frame 120 BMP:
  `1fb5cb47383475e2f60e97e8e81065bec8ff14547e7b1842a7f84b93fddc3b66`

Both captures are dominated by the separately recorded malformed-terrain
fault and do not expose the rendered character clearly enough to certify a
walk cycle or its return to idle by sight. The visual result is therefore
inconclusive, not a Rung 3 pass.

Rung 2's deterministic schedule and repeatable hashes remain valid, but its
"movement differs from control" observation measured field-phase input, not
world movement. Automated world-movement acceptance still requires the
separate scripted world-controller bridge.

A read-only live-input probe at the existing assignment seam established the
runtime chain independently:

```text
movement-consumer: g_C1ButtonState=0x2000, 0x8009CD4C=0x2000
capture seam:       g_C1ButtonState=0x2000, animation=1
                    0x8009CD4C=0, slot-1 velocity=(0,0,0)
```

The capture-seam zeros are expected post-consumption state, not the next
fault: `wm_8008A72C` clears slot `+0x38/+0x3c/+0x40` in its retail common
movement tail, and `wm_800712D0` clears the input accumulators at its frame
tail. Thus the repaired source reaches the world consumer and the existing
retail assignment path selects walk animation 1. At that seam alone, no break
was yet proven before animation stepping/pose publication.

## Renderer-entry held/released discriminator

The final state-only probe used real X11 Right-key input and did not request a
capture or read the framebuffer. GDB remained attached for the lightweight
state breakpoints. `PcPort_WorldCaptureSetFrame` supplied the one-based
world-frame label. Right was held through frame 25; key-up was issued at the
frame-26 marker and first became observable in the drawn group on frame 27.
Both direct `func_8001E298` entries for each rendered sprite were sampled
through frame 50. Each sample read native sprite `animation +0xaf`, signed
wait timer `+0x9e`, and signed selected pose `+0x34`.

The run produced exactly 200 records: two renderer-eligible sprites, two
scheduler-driven renderer-entry groups, and 50 frames. `first` below is the
outer-main-loop scheduler group before the driver clears the OT; `drawn` is
the post-clear `0x80071488` group whose packets survive to `DrawOTag`. Slot 1
showed:

```text
frame  pass  animation  wait  pose
1      first     0        3     1
1      drawn     1        1     1
2      first     0        1     1
2      drawn     1        1     2
3      first     0        1     2
3      drawn     1        1     3
4-25   first     0        1     3
4-25   drawn     1        1     3
26     first     0        1     3
26     drawn     1        1     3
27     first     0        1     3
27     drawn     0        3     3
28     first     0        1     3
28     drawn     0        0     3
29-50  both      0        0     3
```

Slot 2 remained idle through frame 15; its drawn group first selected
animation 1 on frame 16, then showed the same per-frame `0 -> 1` animation
toggle. Its drawn selected pose was 41/42/43 on frames 16/17/18 and remained
43 through frame 26. After release took effect on frame 27, the drawn group
reached animation 0 / wait 0 on frame 28; both groups were animation 0 / wait
0 from frame 29.

This is not a missing assignment or a missing `AnimScriptTick`. The drawn
second pass does select animation 1, but the earlier pass resets the same
sprite to animation 0 every frame. On each subsequent effective-held frame,
both transitions reset the timer to 1, so the drawn selected pose reaches 3
and then remains pinned through frame 26.

## Retail cadence contradiction

Retail has the same two scheduler call sites at different cadences:

- `0x80071064` runs in the outer `0x80071034` world-session loop. On the
  natural entry, slot 1 runs its state-0 cb0 and transitions to state 1.
- `0x80071488` runs once per displayed world frame inside `wm_800712D0`.
  Recurring frames use the `0x800719C8 -> 0x8007130C` inner back-edge and do
  not revisit `0x80071064`.

The current open-loop port flattened those nested loops. Every bounded host
frame returns from `wm_800712D0`, repeats the outer `wm_80071034` scheduler,
and then runs the legitimate post-input scheduler at `0x80071488`. After the
first frame, the repeated outer pass incorrectly dispatches slot-1 cb1
`wm_8008A72C` with the movement vector already cleared, selecting idle. The
driver then drains held controller input, and the drawn pass sees nonzero
movement and selects walk. This exactly accounts for the observed
`animation 0 -> 1`, `wait=1` restart on every held frame.

This is a session-vs-frame control-flow transcription defect, not permission
to special-case the animation callback or simply suppress a scheduler symbol.
The flattened loop also repeats outer mode dispatch and re-enters one-time
driver setup; a repair must recover the retail nested cadence while applying
the accepted bounded frame limit at the inner frame boundary.

The local no-op `func_8001D468` in `world_map_frame_driver_712d0.c` is a
separate downstream retail divergence: it shadows the already port-owned
work-list drain. It does not select `+0xaf` and therefore does not explain the
cadence/restart evidence above.

## Verdict and next exact task

`RUNG3=FAIL-CADENCE`

Rung 3a repaired the controller-source divergence and proved the live chain
through walk selection, but the renderer-entry discriminator does not pass:
the drawn walk pose does not advance after frame 3 because the flattened loop
restarts idle and walk every frame. The by-eye captures remain independently
obscured by malformed terrain. Per the campaign stop rule, Rungs 4–6 were not
started.

`NEXT_EXACT_TASK=Restore and certify the retail nested session/frame cadence: execute the 0x80071064 outer scheduler once at world-session entry, recur through wm_800712D0's 0x800719C8 -> 0x8007130C frame back-edge with the bounded frame limit at that inner boundary, and prove that only 0x80071488 updates active sprites per recurring displayed frame; then rerun this exact held/released renderer-entry probe.`
