# W34N3 Phase 2 — one-gate-off census

## Method and classification boundary

The probes ran from `fea6113db0455329f3edc0d09d73850c712052d6` on
`experiment/worldmap-open-gates-20260823`, equal to origin. Each isolatable
accepted gate was set to `0` in a fresh process while the accepted route kept
the scripted input `0:0x2000,600:0x4000,916:0x2000,976:0`, detached before
`PcPort_WorldMapInitMain`, and retained the 120-frame bound. The comparison
digests were:

- frame 60: `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`
- frame 120: `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`

The early implication ladder is material to the verdicts. With
`XENO_WORLD_ARCHIVE_SET_INDEX=1`, setting any direct flag from
`XENO_WORLD_INIT` through `XENO_WORLD_FIRST_WDS_CONSUMER` to zero does not
turn its stage off. Those gates are `BLOCKED (shadowed)`, even when an observed
run is bit-identical. Likewise, `XENO_WORLD_FRAME_PROLOGUE` is excluded by the
accepted open loop before its direct flag matters. Calling either case
`VESTIGIAL` would mistake a failed isolation for neutrality.

The one operational `VESTIGIAL` result is `XENO_WORLD_967E4_ROUTE`. Every
other isolatable operational gate either fails, silently changes output, or is
required to replace the incomplete legacy loop.

## Operational results

Digest abbreviations used below:

- **BASE** — the standing frame-60/frame-120 pair above.
- **DARK** — both frames
  `bcb95adec1310dc59578a7041672f9a21e36e74531600274e5ee6da12511479e`.
- **MID** — frame 60
  `9540bc53bafdfafae5319494b3c1099d48a108bcbde94a4fa200448d1ea8f9df`,
  frame 120
  `fcf5b8e0acdee009f2169b23a51e5a9eb2adb87702ead7f5e51f26e79d30c4c4`.

| gate | probe result | verdict | body/dependency required before retirement |
|---|---|---|---|
| `XENO_WORLD_INIT` | Not isolatable: set-index implies the complete early ladder. | **BLOCKED (shadowed)** | `PcPort_WorldMapInitMain` / retail world init `0x80071034`; body is port-owned, but it must be made the normal selector path rather than separately probed under the later umbrella. |
| `XENO_WORLD_MODE_INIT` | Shadowed by set-index. | **BLOCKED (shadowed)** | `wm_80071CDC_mode_init`; transcribed body exists. Retirement waits on dependency-ladder collapse. |
| `XENO_WORLD_SECOND_WAVE` | Shadowed by set-index. | **BLOCKED (shadowed)** | `wm_80071EF0_second_wave`, its readiness poll, and `wm_80073530` fixup; bodies exist. |
| `XENO_WORLD_OBJECT_POOL` | Shadowed by set-index. | **BLOCKED (shadowed)** | `wm_8009766C` / `wm_800976C8`; transcribed pool setup exists. |
| `XENO_WORLD_STATE_TEMPLATE` | Shadowed by set-index. | **BLOCKED (shadowed)** | A180-to-BE4C matrix-template copy in `world_map_init.c`; port body exists. |
| `XENO_WORLD_MODE_ENTER_STATE` | Shadowed by set-index. | **BLOCKED (shadowed)** | `PcPort_WorldMapInitializeModeEnterState`; transcribed state stores exist. |
| `XENO_WORLD_CROSS_PRODUCTS` | Shadowed by set-index. | **BLOCKED (shadowed)** | Four retail `0x80098044` products; body exists. |
| `XENO_WORLD_GPU_ASSET_A` | Shadowed by set-index. | **BLOCKED (shadowed)** | GPU asset-A TIM/CLUT decode/upload body in `world_map_init.c`; body exists. |
| `XENO_WORLD_GPU_ASSET_B` | Shadowed by set-index. | **BLOCKED (shadowed)** | GPU asset-B TIM/CLUT/TPage body in `world_map_init.c`; body exists. |
| `XENO_WORLD_OBJECT_MATRIX` | Shadowed by set-index. | **BLOCKED (shadowed)** | retail `0x80084580` object-matrix construction; body exists. |
| `XENO_WORLD_THIRD_WAVE` | Shadowed by set-index. | **BLOCKED (shadowed)** | retail `0x80072090` archive-request wave; body exists. |
| `XENO_WORLD_BSS_CONSTANTS` | Shadowed by set-index. | **BLOCKED (shadowed)** | retail `0x800736DC` constant paint; body exists. |
| `XENO_WORLD_PRIMITIVE_TEMPLATES` | Shadowed by set-index. | **BLOCKED (shadowed)** | retail `0x80073E30` packet-template initialization; body exists. |
| `XENO_WORLD_RECORD_CLUT_INIT` | Shadowed by set-index. | **BLOCKED (shadowed)** | retail `0x80085F58` relocation/CLUT initialization; body exists. |
| `XENO_WORLD_GFX_WORK_BUFFERS` | Shadowed through FT4 prerequisites. | **BLOCKED (shadowed)** | compiled `GfxAllocateWorkBuffers(5120,0)` route; implementation exists. |
| `XENO_WORLD_FT4_POOLS` | Shadowed by later W18B-W24E flags. | **BLOCKED (shadowed)** | retail `0x80074594`; transcribed body exists. |
| `XENO_WORLD_HEAP_TABLE_RAND` | Shadowed by later W19A-W24E flags. | **BLOCKED (shadowed)** | retail `0x800863E0`; transcribed table/RNG body exists. |
| `XENO_WORLD_UPLOAD_RECORDS` | Shadowed by later W20B-W24E flags. | **BLOCKED (shadowed)** | retail `0x80074E58`; transcribed upload-A body exists. |
| `XENO_WORLD_UPLOAD_RECORDS_B` | Shadowed by later W21B-W24E flags. | **BLOCKED (shadowed)** | retail `0x80075030`; transcribed upload-B body exists. |
| `XENO_WORLD_DRAW_PACKETS` | Direct flag off completed with **BASE**, but set-index still enabled `world_draw_packets_enabled()`. | **BLOCKED (shadowed)** | retail `0x800739B8`; body exists. Direct-gate neutrality was not isolated. |
| `XENO_WORLD_88F64` | Shadowed by W23B-W24E flags. | **BLOCKED (shadowed)** | retail `0x80088F64`; transcribed body exists. |
| `XENO_WORLD_ARCHIVE_READY_POLL` | Shadowed by first-WDS/set-index. | **BLOCKED (shadowed)** | `wm_archive_ready_poll` plus compiled archive synchronization; implementation exists but is inseparable under the accepted umbrella. |
| `XENO_WORLD_FIRST_WDS_CONSUMER` | Shadowed by set-index. | **BLOCKED (shadowed)** | `wm_first_wds_consumer` and `SoundLoadWdsFile`; W34C23 additionally proves the field-WDS SPU allocation lifecycle is not enabled, so this dependency is not independently retireable. |
| `XENO_WORLD_ARCHIVE_SET_INDEX` | Process exited before normal bounded completion; neither capture was produced and frame 60 reported an explicit unfulfilled-capture error. | **FAILS_LOUD** | The nested W23/W24 sequence must become normal flow: `wm_archive_ready_poll`, `wm_first_wds_consumer` / `SoundLoadWdsFile`, and `wm_archive_set_index_transition` / `ArchiveSetIndex(36,0)`. W34C23's field-WDS allocation lifecycle remains a real dependency. |
| `XENO_WORLD_967E4_ROUTE` | Completed 120 frames with **BASE**. The bounded CD dispatcher saw no queued work and changed no accepted-route state. | **VESTIGIAL** | None on this route. The port-owned `wm_800967E4` dispatcher can be kept unconditionally at this established sequence point. |
| `XENO_WORLD_READY_BUFFER_CONSUME` | Process exited; no captures; explicit frame-60 unfulfilled-capture error. | **FAILS_LOUD** | `wm_ready_buffer_consume`, retail ready/free/link sequence `0x80072558..0x800725A8`, must be part of normal flow. |
| `XENO_WORLD_MODE_AUDIO_SETUP` | Process exited; no captures; explicit frame-60 unfulfilled-capture error. | **FAILS_LOUD** | `wm_mode_audio_setup`, retail `0x800725AC..0x800726C0`, including its mode-dependent audio setup and continuation. |
| `XENO_WORLD_CONVERGENCE_P1` | Process exited; no captures; explicit frame-60 unfulfilled-capture error. | **FAILS_LOUD** | `wm_800726C0_convergence_p1` and its return-to-P2 integration. |
| `XENO_WORLD_CONVERGENCE_P2` | Completed with **DARK**, not BASE. First divergence is omission of the `wm_8007272C_convergence_p2` init pass; its prerequisite chain then excludes the common tail. | **FAILS_SILENT** | `wm_8007272C_convergence_p2` must become unconditional after P1's retail-selected return. |
| `XENO_WORLD_FRAMEBUFFER_GTE_INIT` | Frame 60 was DARK; process then exited with an explicit unfulfilled frame-120 capture error. | **FAILS_LOUD** | `wm_80072BB0` framebuffer/GTE initialization and its normal call at retail `0x80072244`. |
| `XENO_WORLD_TERRAIN_POSITION_INIT` | Completed; frame 60 `92a6549f039598297720b02d7f13b163bc649c03a76b0e4c2652e2fa4fe0ca9d`, frame 120 `c22f95f7c6e5ab3f703d93c9a8fdc961c603938cf4483898a6c95647ca7ccf68`. First divergence is omission of terrain state initialization. | **FAILS_SILENT** | `wm_80097BC0` terrain matrix/cell/position initializer at retail `0x8007250C`. |
| `XENO_WORLD_COMMON_TAIL_P0` | Completed with **DARK**. | **FAILS_SILENT** | `wm_8007290C_common_tail_p0`, including its `wm_80089160` selector path, must be normal flow. |
| `XENO_WORLD_COMMON_TAIL_P1` | Completed with **DARK**. | **FAILS_SILENT** | `wm_8007293C_common_tail_p1` / `wm_800978FC`. |
| `XENO_WORLD_COMMON_TAIL_P2` | Completed with **MID**. | **FAILS_SILENT** | `wm_80072944_common_tail_p2` / `wm_8008901C`. |
| `XENO_WORLD_COMMON_TAIL_P3` | Completed with **MID**. | **FAILS_SILENT** | `wm_8007294C_common_tail_p3` / `wm_800865A0`. |
| `XENO_WORLD_COMMON_TAIL_P4` | Completed with **MID**. | **FAILS_SILENT** | `wm_80072954_common_tail_p4` / `wm_80085FE0`. |
| `XENO_WORLD_COMMON_TAIL_P5` | Completed with **MID**. | **FAILS_SILENT** | `wm_8007295C_common_tail_p5`, `wm_80075228`, palette transfer, and normal slot-2 continuation. |
| `XENO_WORLD_SCHEDULER_97800` | Completed with **MID**. First divergence is omission of the post-slot-1 scheduler pass; recurring open-loop scheduling still runs, but initial registered callback state is different. | **FAILS_SILENT** | `wm_80097800` scheduler call at retail frontier `0x80071064` must be normal flow. Its currently reached callbacks are port-owned. |
| `XENO_WORLD_FRAME_PROLOGUE` | Direct flag off completed with **BASE**, but the accepted open loop excludes this body even when its flag is on. | **BLOCKED (mutually excluded)** | Legacy bounded `0x8007106C..0x80071484` body is superseded by the open-loop implementation. It cannot be retired on this test until the real recurring loop replaces both paths. |
| `XENO_WORLD_OPEN_LOOP` | No bounded world frames or captures; the legacy prologue ran once, hard-cut before `0x800719C8`, then the process remained in the placeholder until watchdog timeout. | **BLOCKED (route cannot start)** | Complete the retail recurring `0x80071034` session/frame control flow and replace `ml_dispatch_guest` defaults for guest callbacks `0x80072238`, `0x8007299C`, and any unregistered mode callback. This is the largest obstacle to an unforced native route. |

## Controls and validation-only reads

These ten names were already off or mandated at their accepted values, so a
one-at-a-time `1 -> 0` operational probe does not exist. They remain
`BLOCKED (control/already off)`, not `VESTIGIAL`.

| name | verdict | reason / dependency |
|---|---|---|
| `XENO_WORLD_FRAME_LIMIT` | **BLOCKED (control)** | Must remain 120; removing it would destroy the bounded acceptance oracle. The real recurring-loop exit is the eventual replacement. |
| `XENO_WORLD_FRAME_REENTRY_ONCE` | **BLOCKED (already off)** | Legacy-prologue validation bound; open loop excludes its consumer. |
| `XENO_WORLD_FRAME_REENTRY_TWICE` | **BLOCKED (already off)** | Legacy-prologue validation bound; open loop excludes its consumer. |
| `XENO_WORLD_RECORD_CLUT_DOUBLE_TEST` | **BLOCKED (already off)** | Destructive/idempotence certificate control for retail `0x80085F58`, not route functionality. |
| `XENO_WORLD_FT4_POOLS_DOUBLE_TEST` | **BLOCKED (already off)** | Idempotence control for retail `0x80074594`. |
| `XENO_WORLD_HEAP_TABLE_RAND_DOUBLE_TEST` | **BLOCKED (already off)** | Idempotence control for retail `0x800863E0`. |
| `XENO_WORLD_UPLOAD_RECORDS_DOUBLE_TEST` | **BLOCKED (already off)** | Idempotence control for retail `0x80074E58`. |
| `XENO_WORLD_UPLOAD_RECORDS_B_DOUBLE_TEST` | **BLOCKED (already off)** | Idempotence control for retail `0x80075030`. |
| `XENO_WORLD_DRAW_PACKETS_DOUBLE_TEST` | **BLOCKED (already off)** | Idempotence control for retail `0x800739B8`. |
| `XENO_WORLD_88F64_DOUBLE_TEST` | **BLOCKED (already off)** | Idempotence control for retail `0x80088F64`. |

## Totals and Phase 3 gate

Across all 50 complete names:

- `VESTIGIAL`: **1**
- `FAILS_LOUD`: **5**
- `FAILS_SILENT`: **9**
- `BLOCKED`: **35** (25 operational, 10 controls)

Only `XENO_WORLD_967E4_ROUTE` satisfies the Phase 2 prerequisite for a
retirement attempt. `XENO_WORLD_DRAW_PACKETS` and
`XENO_WORLD_FRAME_PROLOGUE` do not: their BASE results were caused by
shadowing/exclusion, not a tested no-op.
