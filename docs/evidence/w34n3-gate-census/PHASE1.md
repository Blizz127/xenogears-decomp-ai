# W34N3 Phase 1 — `XENO_WORLD_*` source inventory

## Anchor and scope

The session started on `experiment/worldmap-open-gates-20260823` at
`77b30d552831d59dea516e1b4c03f65cf0ac5b11`, equal to origin. The task's
expected `75cf17db` was the pre-W34C23 head; this is not an anchor mismatch
because the intervening committed/pushed W34C23 evidence is present.

The inventory searched string-valued reads in `pc_port/src`, compiled `src`,
and `include`. There are **50 complete environment names**: 40 operational
world-route flags, three bounded-harness controls, and seven destructive or
idempotence validation controls. Adjacent fragments used only to build log
messages (`XENO_WORLD_BSS_`, `...FRAMEBUFFER_`, `...MODE_AUDIO_`,
`...READY_BUFFER_`, and `...TERRAIN_`) are not separate variables.

The operational flags are not independent. `world_map_init.c:792-1148` builds
an implication ladder: a later early-init flag turns on its prerequisites,
while the post-W24 gates use explicit `&&` prerequisites. Consequently,
setting an early direct flag to zero does not necessarily turn its stage off.
Phase 2 must classify such a probe as `BLOCKED (shadowed)`, not `VESTIGIAL`.

Implementation labels used below:

- **PORT** — port-owned C route/body.
- **TRANSCRIBED** — bounded retail body transcribed into port C and still
  guarded.
- **COMPILED** — existing compiled game function routed by port glue.
- **HARNESS/STUB** — bounded harness containing explicit default-return guest
  stubs; it is not a complete retail loop.
- **CONTROL** — test/bound control, not a decomp gate.

## Operational gates

| flag | source gate and on-path | off versus on | class |
|---|---|---|---|
| `XENO_WORLD_INIT` | `world_map_init.c:1144-1152`; selects `PcPort_WorldMapInitMain` at `game_overrides.c:2345-2369` | off selects the hollow state-3 placeholder unless a later ladder flag implies it; on loads overlay 0x0F and executes native init | PORT |
| `XENO_WORLD_MODE_INIT` | `world_map_init.c:1138-1142,6977-6983`; `wm_80071CDC_mode_init` at 2435 | off skips slot-0 mode initialization unless implied; on executes retail 0x80071CDC | TRANSCRIBED |
| `XENO_WORLD_SECOND_WAVE` | `world_map_init.c:1132-1136,6983-6991`; bodies at 2553/2645/2682 | off skips archive wave/fixup unless implied; on runs 0x80071EF0, poll, 0x80073530 | TRANSCRIBED |
| `XENO_WORLD_OBJECT_POOL` | `world_map_init.c:1126-1130,6991-6998`; body at 2866 | off skips the 64-slot pool unless implied; on runs 0x8009766C/0x800976C8 | TRANSCRIBED |
| `XENO_WORLD_STATE_TEMPLATE` | `world_map_init.c:1119-1124,6998-7006`; body at 2962 | off skips A180→BE4C unless implied; on copies the retail MATRIX template | PORT |
| `XENO_WORLD_MODE_ENTER_STATE` | `world_map_init.c:1112-1117,7006-7014`; body is `PcPort_WorldMapInitializeModeEnterState` | off skips ten state stores unless implied; on performs the bounded retail state write | TRANSCRIBED |
| `XENO_WORLD_CROSS_PRODUCTS` | `world_map_init.c:1105-1110,7014-7023`; body at 3174 | off skips basis setup unless implied; on executes four 0x80098044 products | TRANSCRIBED |
| `XENO_WORLD_GPU_ASSET_A` | `world_map_init.c:1098-1103,7027-7036`; body at 3445 | off skips asset A unless implied; on decodes/uploads its TIM/CLUT | TRANSCRIBED |
| `XENO_WORLD_GPU_ASSET_B` | `world_map_init.c:1091-1096,7036-7047`; body at 3604 | off skips asset B unless implied; on decodes/uploads TIM/CLUT/TPage | TRANSCRIBED |
| `XENO_WORLD_OBJECT_MATRIX` | `world_map_init.c:1084-1089,7047-7058`; body at 3797 | off skips table construction unless implied; on runs 0x80084580 | TRANSCRIBED |
| `XENO_WORLD_THIRD_WAVE` | `world_map_init.c:1077-1082,7058-7071`; body at 4060 | off skips five archive requests unless implied; on runs 0x80072090 | TRANSCRIBED |
| `XENO_WORLD_BSS_CONSTANTS` | `world_map_init.c:1070-1075,7071-7088`; body at 4292 | off skips constant paint unless implied; on runs 0x800736DC | TRANSCRIBED |
| `XENO_WORLD_PRIMITIVE_TEMPLATES` | `world_map_init.c:1063-1068,7088-7106`; body at 4451 | off skips packet templates unless implied; on runs 0x80073E30 | TRANSCRIBED |
| `XENO_WORLD_RECORD_CLUT_INIT` | `world_map_init.c:1056-1061,7106-7125`; body at 4895 | off skips relocation/CLUT unless implied; on runs 0x80085F58 | TRANSCRIBED |
| `XENO_WORLD_GFX_WORK_BUFFERS` | `world_map_init.c:1048-1054,7125-7145`; route at 5254 | off skips allocation unless implied; on calls compiled `GfxAllocateWorkBuffers(5120,0)` | COMPILED via PORT route |
| `XENO_WORLD_FT4_POOLS` | `world_map_init.c:792-804,7145-7162`; body at 5414 | off skips FT4 pools unless a later flag implies it; on runs 0x80074594 | TRANSCRIBED |
| `XENO_WORLD_HEAP_TABLE_RAND` | `world_map_init.c:806-817,7162-7182`; body at 5757 | off skips tables/RNG unless implied; on runs 0x800863E0 | TRANSCRIBED |
| `XENO_WORLD_UPLOAD_RECORDS` | `world_map_init.c:819-829`; body at 6317 | off skips 0x80074E58 unless implied; on builds upload records A | TRANSCRIBED |
| `XENO_WORLD_UPLOAD_RECORDS_B` | `world_map_init.c:831-840`; body at 6327 | off skips 0x80075030 unless implied; on builds upload records B | TRANSCRIBED |
| `XENO_WORLD_DRAW_PACKETS` | `world_map_init.c:842-850`; body at 6402 | off skips 0x800739B8 unless implied; on builds draw packets | TRANSCRIBED |
| `XENO_WORLD_88F64` | `world_map_init.c:852-859`; body at 6535 | off skips final table init unless implied; on runs 0x80088F64 | TRANSCRIBED |
| `XENO_WORLD_ARCHIVE_READY_POLL` | `world_map_init.c:861-867`; body at 6669 | off skips readiness poll unless implied by WDS/set-index; on calls real archive sync and branches on C894 | PORT/COMPILED |
| `XENO_WORLD_FIRST_WDS_CONSUMER` | `world_map_init.c:869-874`; body at 6727 | off skips WDS load unless set-index implies it; on calls real `SoundLoadWdsFile` | PORT/COMPILED; W34C23 proves allocation failure |
| `XENO_WORLD_ARCHIVE_SET_INDEX` | `world_map_init.c:876-880`; body at 6802 | off stops W24E; on calls compiled `ArchiveSetIndex(36,0)` | COMPILED via PORT route |
| `XENO_WORLD_967E4_ROUTE` | `world_map_init.c:882-887,7321-7349`; helper in `world_map_helper_96130.c:31` | off skips the CD dispatcher; on runs one bounded 0x800967E4 | PORT |
| `XENO_WORLD_READY_BUFFER_CONSUME` | `world_map_init.c:889-895,7350-7358`; body at 1993 | off retains third-wave buffers; on performs retail ready/free/link branch | TRANSCRIBED |
| `XENO_WORLD_MODE_AUDIO_SETUP` | `world_map_init.c:897-902,7358-7366`; body at 2096 | off skips mode-dependent audio setup and all `&&` dependents; on runs the bounded audio branch | TRANSCRIBED |
| `XENO_WORLD_CONVERGENCE_P1` | `world_map_init.c:904-915,7383-7391`; body in `world_map_convergence.c` | off skips first table pass and P2/common tail; on runs 0x800726C0 | TRANSCRIBED |
| `XENO_WORLD_CONVERGENCE_P2` | `world_map_init.c:917-924,7391-7398`; body in `world_map_convergence.c` | off skips second table pass/common tail; on runs 0x8007272C when P1 selects it | TRANSCRIBED |
| `XENO_WORLD_FRAMEBUFFER_GTE_INIT` | `world_map_init.c:926-935,7366-7373`; body `world_map_framebuffer_init.c:109` | off leaves display/GTE state uninitialized and blocks terrain init; on runs 0x80072BB0 | TRANSCRIBED |
| `XENO_WORLD_TERRAIN_POSITION_INIT` | `world_map_init.c:937-946,7374-7382`; body `world_map_terrain_init.c:102` | off skips D534/cell/position init and blocks common tail; on runs 0x80097BC0 | TRANSCRIBED |
| `XENO_WORLD_COMMON_TAIL_P0` | `world_map_init.c:948-957,7399-7405`; body in `world_map_common_tail.c:263` | off stops the common-tail chain; on executes selector reset path 0x8007290C | TRANSCRIBED |
| `XENO_WORLD_COMMON_TAIL_P1` | `world_map_init.c:959-967,7406-7412`; body `world_map_common_tail.c:698` | off stops after P0; on calls 0x800978FC | TRANSCRIBED |
| `XENO_WORLD_COMMON_TAIL_P2` | `world_map_init.c:969-977,7413-7419`; body `world_map_common_tail.c:873` | off stops after P1; on calls 0x8008901C | TRANSCRIBED |
| `XENO_WORLD_COMMON_TAIL_P3` | `world_map_init.c:979-987,7420-7426`; body `world_map_common_tail.c:1044` | off stops after P2; on calls 0x800865A0 | TRANSCRIBED |
| `XENO_WORLD_COMMON_TAIL_P4` | `world_map_init.c:989-997,7427-7433`; body `world_map_common_tail.c:1232` | off stops after P3; on calls 0x80085FE0 | TRANSCRIBED |
| `XENO_WORLD_COMMON_TAIL_P5` | `world_map_init.c:999-1009,7434-7440`; body `world_map_common_tail.c:1424` | off stops before wm_80075228/palette and scheduler; on completes slot-1 tail | TRANSCRIBED |
| `XENO_WORLD_SCHEDULER_97800` | `world_map_init.c:1011-1021,7471-7477`; `world_map_scheduler.c:545` | off skips post-slot-1 scheduler; on dispatches registered port-owned callbacks | PORT scheduler with explicit callback registry |
| `XENO_WORLD_FRAME_PROLOGUE` | `world_map_init.c:1023-1033,7478-7506` | off skips the legacy bounded single-frame continuation; on runs it only when open loop is off | TRANSCRIBED legacy diagnostic |
| `XENO_WORLD_OPEN_LOOP` | `world_map_init.c:8037-8044`; `world_map_main_loop_71034.c:46-73,116+` | off returns to placeholder/legacy prologue; on runs bounded recurring frames, but unresolved guest mode callbacks become explicit default-return stubs | HARNESS/STUB |

No operational on-path is a mere forced constant. The important incomplete
behavior is concentrated in `XENO_WORLD_OPEN_LOOP`: `ml_dispatch_guest`
explicitly substitutes default returns for unresolved guest callbacks at
`0x80072238`, `0x8007299C`, and any unregistered address.

## Bounded and validation controls

These are reads and therefore part of the complete inventory, but they are not
candidate decomp gates and cannot be meaningfully tested by changing an
accepted `1` to `0` because they are already off or mandated by the task.

| flag | source/use | classification for Phase 2 |
|---|---|---|
| `XENO_WORLD_FRAME_LIMIT` | `world_map_main_loop_71034.c:39-44`; bounds the harness | CONTROL; retained at 120 by task |
| `XENO_WORLD_FRAME_REENTRY_ONCE` | `world_map_init.c:1035-1045`; legacy prologue bound | CONTROL; already off under open loop |
| `XENO_WORLD_FRAME_REENTRY_TWICE` | same | CONTROL; already off under open loop |
| `XENO_WORLD_RECORD_CLUT_DOUBLE_TEST` | `world_map_init.c:5216` | validation-only destructive/idempotence probe; already off |
| `XENO_WORLD_FT4_POOLS_DOUBLE_TEST` | `world_map_init.c:5705` | validation-only; already off |
| `XENO_WORLD_HEAP_TABLE_RAND_DOUBLE_TEST` | `world_map_init.c:6050` | validation-only; already off |
| `XENO_WORLD_UPLOAD_RECORDS_DOUBLE_TEST` | `world_map_init.c:6140` | validation-only; already off |
| `XENO_WORLD_UPLOAD_RECORDS_B_DOUBLE_TEST` | `world_map_init.c:6153` | validation-only; already off |
| `XENO_WORLD_DRAW_PACKETS_DOUBLE_TEST` | `world_map_init.c:6495` | validation-only; already off |
| `XENO_WORLD_88F64_DOUBLE_TEST` | `world_map_init.c:6634` | validation-only; already off |

Phase 2 will record these ten as `BLOCKED (control/already off)`, not pretend
that an unchanged digest proves their code is vestigial.
