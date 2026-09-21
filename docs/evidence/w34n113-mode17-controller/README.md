# W34N113 — mode-17 slot-2 camera/position controller

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `8e531232329b62725ff56b0d19293cd2114955ba`
- Date: 2026-09-01
- Verdict: **PASS**

## Retail anchors and scope

The source was transcribed directly from `disc/world_map.bin`, loaded at
`0x8006FAF0`.  The certificate hashes every implemented retail slice before
execution:

| range | role | SHA-256 |
| --- | --- | --- |
| `[0x80076DA4,0x80076F54)` | angular target approach | `9ca630c02dbc139b19922610562d62cd67869a1c38f2e6f1fcb61ca09934f008` |
| `[0x80076F54,0x80076FA8)` | camera-height approach | `d6eb171c02b2c15cbe00e6f89e2ab5031493f1035e97c8c62a48b8ea69509f8d` |
| `[0x80076FA8,0x800771D8)` | world-position approach/publication | `2ef8c2f88eb7c643e72b0592c731e9358b30bd8e18f7174d1c1bce8d54e82ae7` |
| `[0x800771D8,0x80077214)` | signed clamp-or-step leaf | `6fa6ffda1e1b785f1fa51ecb300e546a55368e90c480e8abcea01a0620219f35` |
| `[0x800827EC,0x800828DC)` | scheduler initializer | `099342952f8dd1fa9410b40a16350c7e5d3df5829ac986b60c500a383edef568` |
| `[0x800828DC,0x80082F64)` | scheduler updater | `355ca7f913e39472888eab6375a32ae92a1947fb36f5c6a417cdc8a15cf4a7e3` |
| `[0x800827EC,0x80082F64)` | complete private pair | `8886a3c010c021bcb0fad54a3809a7647c304bb94567f407d961bbc90fe70765` |

`wm_800827EC` restores the reset-position fanout, initial fixed-point camera
targets, view height, controller state, and jitter amplitude.  `wm_800828DC`
restores all twelve command-latch cases, the twelve-entry active-state switch,
the retail `D144` camera-build gate, the three common target-approach helpers,
and centered random jitter publication.  The scheduler resolves both callback
addresses symbolically; it never casts a guest address to a native function
pointer.

## Focused certificate

`pc_port/tests/run_w34n113_mode17_controller.sh` passes at O0, O2, and O2 with
nonrecovering UBSan under strict warnings.  It proves:

- complete initializer constants, position fanout, and bounded write region;
- strict signed clamp-or-step behavior of `wm_800771D8`;
- exact command-latch-to-state mapping for commands 1 through 12 and the
  representative camera/position presets;
- camera call arguments and `D144` gate;
- angular, height, and position work-vector construction, thresholds,
  accumulators, and publication;
- state-2 direction, state-4 strict ceiling/clamp, and state-11 wrap call;
- the common motion-helper pipeline and centered jitter arithmetic;
- scheduler initializer and updater resolution.

M1–M13 are all detected by named assertions: initial angle, view height,
strict interpolation, latch 8, latch 12, camera omission, early scale clamp,
rotation threshold, position threshold, state-2 direction, state-11 wrap,
jitter bias, and helper-pipeline omission.

The W34N109 lifecycle, W34N110 transition, W34N111 command-stream, and W34N112
particle certificates all remain green.  The normal PC port build links
successfully.

## First natural integration failure and bounded repair

The first detached run reached the new updater successfully, then faulted in
`ModelPrimTriDepthCueVariant4` during the same frame's shared draw callback.
The backtrace was:

```text
ModelPrimTriDepthCueVariant4
func_8002C700
wm_848f4_dispatch
wm_800848F4
wm_80076A1C
wm_80097800
```

The selected C620 record buffer was guest address `0x80103368`, but
`wm_848f4_dispatch` passed it raw to the native renderer.  This was a stale
W34N44 fixture assumption exposed by W34N112's proven guest-domain producer
and by W34N113 making the region model naturally visible.  The bounded repair
rebases only that consumer through `PSX_ADDR`.  The W34N44 certificate fixture
now uses guest buffers and passes O0/O2/nonrecovering UBSan; new M15 restores
the raw guest pointer and is detected by `renderer-domain-and-buffer`.

## Natural entrance-17 acceptance

The final run used scripted input
`0:0x2000,600:0x4000,916:0x2000,976:0`, detached before
`PcPort_WorldMapInitMain`, selected entrance 17, and ran the corrected
120-frame cadence:

- `0x800827EC`: 1 initializer execution;
- `0x800828DC`: 120 updater executions;
- `0x80083214`: 5 initializer executions;
- `0x80083264`: 600 updater executions;
- `0x800834D8`: 120 executions;
- `0x80076A1C`: 120 executions;
- unresolved world-map callback stubs: 0;
- upload-pump unknown records: 0;
- OT-adapter abort/error diagnostics: 0;
- frame-60 request fulfilled at frame 60;
- frame-120 request fulfilled at frame 120;
- bounded loop returned normally.

Captures:

- frame 60: `scratchpad/w34n113_mode17_capture/world-frame-000060.bmp`
  — `6169f86e4b815ba14f1129413aced9373052c272b21f34d6e6b28e254094656e`
- frame 120: `scratchpad/w34n113_mode17_capture/world-frame-000120.bmp`
  — `f581ebe09416527736306ba6c8b734651d19258f817b508c7ba8d602805cd45a`

Both captures were inspected.  They show a coherent world scene with terrain,
clouds, coastline, and region geometry.  The frames differ naturally as the
scene evolves.  This is the first mode-17 acceptance in which the complete
private scheduler family is resolved and the world scene is visibly rendered;
the prior abstract-band hashes are retired for this route.

## Result

`MODE17_SLOT2_CONTROLLER_RESTORED`

All scheduler callbacks registered by the accepted entrance-17 lifecycle now
resolve to port-owned implementations on the natural route.  No private
mode-17 callback remains in the runtime stub census.
