# W34N117 — retail mode-18 object/presence controller

## Anchors

- Starting HEAD: `f6f5e3ae23490428ea1d243d93a3ab3029b4e4e6`.
- Canonical image: `disc/world_map.bin`, load base `0x8006FAF0`.
- `wm_80084068 [0x80084068,0x8008440C)`, SHA-256
  `85bdeac8df0c583a1cd43a66f14f180a34e49220d105461798bc56fe3d0c027b`.

## Production result

`wm_80084068` is now an exact bounded native transcription of the mode-18
slot-3 object/presence controller and resolves symbolically through the world
scheduler. It implements:

- all four retail latch commands, including the exact per-command presence
  clear lists and object-enable changes;
- the shared signed-Y approach through the already-certified
  `wm_800771D8`;
- the four state-specific presence publication lists through
  `wm_80089160`, including state 4's special zero-Y second marker; and
- the final fixed-point position publication to both paired object
  subrecords.

No new helper or subsystem was introduced.

## Certificate

`pc_port/tests/run_w34n117_mode18_object.sh`:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- M1 retained object enables: DETECTED by `latch1.object_disable`
- M2 wrong latch-2 clear list: DETECTED by `latch2.clear_list`
- M3 wrong latch-3 clear list: DETECTED by `latch3.clear_list`
- M4 wrong latch-4 clear list: DETECTED by `latch4.clear_list`
- M5 wrong signed Y step: DETECTED by `state1.interpolation`
- M6 wrong state-1 presence list: DETECTED by
  `state1.presence_list`
- M7 wrong state-2 presence list: DETECTED by
  `state2.presence_list`
- M8 wrong state-3 presence list: DETECTED by
  `state3.presence_list`
- M9 retained state-4 Y for the second marker: DETECTED by
  `state4.second_y_zero`
- M10 missing paired object publication: DETECTED by
  `object.position_publish`

The exact staged tree was materialized as isolated tree commit
`f315b0ca9c440ef6e27ed407cc36cae28e9ae533`. Its normal PC port build linked
successfully and contained a strong `wm_80084068` symbol. As in W34N116, the
isolated tree avoids unrelated concurrent boot/menu edits in the shared
worktree; those edits are not part of this result.

## Natural entrance-18 acceptance

The exact isolated executable ran the detached entrance-18 route with the
standard scripted input for 120 bounded frames.

- `wm_80083A00` executed successfully 120 times.
- `wm_80084068` executed successfully 120 times.
- The complete run reported zero world-map stub hits.
- Both upload pumps retained `unknowns=0`.
- Frame-60 and frame-120 capture requests were fulfilled on their requested
  frames.
- The bounded loop returned after exactly 120 frames.

Capture digests:

- frame 60:
  `e07098c96bd2b4eaf217e27dff866ab94a2e2de24369f072424a588921b218da`
- frame 120:
  `aa534e56ad302a7b939499bef85d9489321bcd6289aee642148ca271ede19902`

Both 640x480 captures are coherent. Frame 60 shows the horizon flyover with
layered mountains, clouds, and textured foreground terrain. Frame 120 shows a
distinct elevated terrain/cloud view. No white foreground or vertical-band
corruption remains.

## Verdict and boundary

`MODE18_PRIVATE_CALLBACK_FRONTIER=CLOSED`.

Mode 18 now has its retail lifecycle, all three private initializers, and both
private update callbacks. The natural 120-frame route is stub-free and
visually coherent. This rung does not claim pixel equivalence with retail,
because no retail mode-18 capture oracle was run.

The next task is a repository-wide world-mode closure census: distinguish any
remaining callbacks/helpers that are genuinely live in entrances 0-18 from
unreachable generated stubs, then run the maintained natural mode matrix. Do
not invent another mode-18 implementation target without new runtime evidence.
