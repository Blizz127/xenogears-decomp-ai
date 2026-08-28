# W34N3 — world-map gate census and decomp backlog

## Outcome

W34N3 enumerated all 50 `XENO_WORLD_*` names present at the start, tested every
operational gate that the accepted route could isolate, and retired one:
`XENO_WORLD_967E4_ROUTE`. The retirement kept the existing port-owned CD-work
dispatcher at its established post-archive sequence point and retained exact
frame-60/frame-120 digests.

After that retirement, **39 operational gate names remain in source**, plus ten
bounded/validation controls. The accepted harness still supplies 17
operational names; only 15 materially select that run because
`XENO_WORLD_DRAW_PACKETS` is shadowed by the early implication ladder and
`XENO_WORLD_FRAME_PROLOGUE` is excluded by `XENO_WORLD_OPEN_LOOP`.

The claim is deliberately bounded. A BASE digest for a shadowed or excluded
flag is not evidence that its body is vestigial. Phase 1 records the complete
source inventory in [PHASE1.md](PHASE1.md), Phase 2 records exact probe results
and hashes in [PHASE2.md](PHASE2.md), and Phase 3 records the one retirement in
[PHASE3.md](PHASE3.md).

## Full classification table

`BLOCKED/S` means shadowed by a later implication; `BLOCKED/X` means mutually
excluded; `BLOCKED/C` means a control that was already off or mandated.

| surveyed name | Phase 2 verdict | body or missing integration behind it | current status |
|---|---|---|---|
| `XENO_WORLD_INIT` | `BLOCKED/S` | `PcPort_WorldMapInitMain`, retail entry `0x80071034`; make this the normal selector path | remains |
| `XENO_WORLD_MODE_INIT` | `BLOCKED/S` | `wm_80071CDC_mode_init` | remains |
| `XENO_WORLD_SECOND_WAVE` | `BLOCKED/S` | `wm_80071EF0_second_wave`, poll, `wm_80073530` | remains |
| `XENO_WORLD_OBJECT_POOL` | `BLOCKED/S` | `wm_8009766C` / `wm_800976C8` | remains |
| `XENO_WORLD_STATE_TEMPLATE` | `BLOCKED/S` | A180-to-BE4C matrix-template copy | remains |
| `XENO_WORLD_MODE_ENTER_STATE` | `BLOCKED/S` | `PcPort_WorldMapInitializeModeEnterState` | remains |
| `XENO_WORLD_CROSS_PRODUCTS` | `BLOCKED/S` | four retail `0x80098044` products | remains |
| `XENO_WORLD_GPU_ASSET_A` | `BLOCKED/S` | asset-A TIM/CLUT decode and upload | remains |
| `XENO_WORLD_GPU_ASSET_B` | `BLOCKED/S` | asset-B TIM/CLUT/TPage decode and upload | remains |
| `XENO_WORLD_OBJECT_MATRIX` | `BLOCKED/S` | retail `0x80084580` | remains |
| `XENO_WORLD_THIRD_WAVE` | `BLOCKED/S` | retail `0x80072090` archive requests | remains |
| `XENO_WORLD_BSS_CONSTANTS` | `BLOCKED/S` | retail `0x800736DC` | remains |
| `XENO_WORLD_PRIMITIVE_TEMPLATES` | `BLOCKED/S` | retail `0x80073E30` | remains |
| `XENO_WORLD_RECORD_CLUT_INIT` | `BLOCKED/S` | retail `0x80085F58` | remains |
| `XENO_WORLD_GFX_WORK_BUFFERS` | `BLOCKED/S` | compiled `GfxAllocateWorkBuffers(5120,0)` route | remains |
| `XENO_WORLD_FT4_POOLS` | `BLOCKED/S` | retail `0x80074594` | remains |
| `XENO_WORLD_HEAP_TABLE_RAND` | `BLOCKED/S` | retail `0x800863E0` | remains |
| `XENO_WORLD_UPLOAD_RECORDS` | `BLOCKED/S` | retail `0x80074E58` | remains |
| `XENO_WORLD_UPLOAD_RECORDS_B` | `BLOCKED/S` | retail `0x80075030` | remains |
| `XENO_WORLD_DRAW_PACKETS` | `BLOCKED/S` (BASE) | retail `0x800739B8`; direct off-probe was still implied by set-index | remains |
| `XENO_WORLD_88F64` | `BLOCKED/S` | retail `0x80088F64` | remains |
| `XENO_WORLD_ARCHIVE_READY_POLL` | `BLOCKED/S` | `wm_archive_ready_poll` and archive synchronization | remains |
| `XENO_WORLD_FIRST_WDS_CONSUMER` | `BLOCKED/S` | `wm_first_wds_consumer`, `SoundLoadWdsFile`, and field-to-world WDS ownership cleanup | remains |
| `XENO_WORLD_ARCHIVE_SET_INDEX` | `FAILS_LOUD` | integrate poll, WDS consumer/lifecycle, and `ArchiveSetIndex(36,0)` into normal flow | remains |
| `XENO_WORLD_967E4_ROUTE` | `VESTIGIAL` (BASE) | port-owned `wm_800967E4` dispatcher retained at same point | **retired by `bab4326a`** |
| `XENO_WORLD_READY_BUFFER_CONSUME` | `FAILS_LOUD` | `wm_ready_buffer_consume`, retail `0x80072558..0x800725A8` | remains |
| `XENO_WORLD_MODE_AUDIO_SETUP` | `FAILS_LOUD` | `wm_mode_audio_setup`, retail `0x800725AC..0x800726C0` | remains |
| `XENO_WORLD_CONVERGENCE_P1` | `FAILS_LOUD` | `wm_800726C0_convergence_p1` | remains |
| `XENO_WORLD_CONVERGENCE_P2` | `FAILS_SILENT` | `wm_8007272C_convergence_p2` | remains |
| `XENO_WORLD_FRAMEBUFFER_GTE_INIT` | `FAILS_LOUD` | `wm_80072BB0`, called at retail `0x80072244` | remains |
| `XENO_WORLD_TERRAIN_POSITION_INIT` | `FAILS_SILENT` | `wm_80097BC0`, called at retail `0x8007250C` | remains |
| `XENO_WORLD_COMMON_TAIL_P0` | `FAILS_SILENT` | `wm_8007290C_common_tail_p0` / `wm_80089160` | remains |
| `XENO_WORLD_COMMON_TAIL_P1` | `FAILS_SILENT` | `wm_8007293C_common_tail_p1` / `wm_800978FC` | remains |
| `XENO_WORLD_COMMON_TAIL_P2` | `FAILS_SILENT` | `wm_80072944_common_tail_p2` / `wm_8008901C` | remains |
| `XENO_WORLD_COMMON_TAIL_P3` | `FAILS_SILENT` | `wm_8007294C_common_tail_p3` / `wm_800865A0` | remains |
| `XENO_WORLD_COMMON_TAIL_P4` | `FAILS_SILENT` | `wm_80072954_common_tail_p4` / `wm_80085FE0` | remains |
| `XENO_WORLD_COMMON_TAIL_P5` | `FAILS_SILENT` | `wm_8007295C_common_tail_p5`, `wm_80075228`, palette and slot-2 continuation | remains |
| `XENO_WORLD_SCHEDULER_97800` | `FAILS_SILENT` | normal scheduler pass at retail frontier `0x80071064` | remains |
| `XENO_WORLD_FRAME_PROLOGUE` | `BLOCKED/X` (BASE) | legacy `0x8007106C..0x80071484` diagnostic is excluded by open loop | remains |
| `XENO_WORLD_OPEN_LOOP` | `BLOCKED` (watchdog, no frames) | real recurring session/frame loop and unresolved callbacks `0x80072238`, `0x8007299C`, plus mode callbacks | remains |
| `XENO_WORLD_FRAME_LIMIT` | `BLOCKED/C` | bounded-harness control; real loop exit eventually supersedes it | remains control |
| `XENO_WORLD_FRAME_REENTRY_ONCE` | `BLOCKED/C` | already-off legacy-prologue bound | remains control |
| `XENO_WORLD_FRAME_REENTRY_TWICE` | `BLOCKED/C` | already-off legacy-prologue bound | remains control |
| `XENO_WORLD_RECORD_CLUT_DOUBLE_TEST` | `BLOCKED/C` | idempotence control for `0x80085F58` | remains control |
| `XENO_WORLD_FT4_POOLS_DOUBLE_TEST` | `BLOCKED/C` | idempotence control for `0x80074594` | remains control |
| `XENO_WORLD_HEAP_TABLE_RAND_DOUBLE_TEST` | `BLOCKED/C` | idempotence control for `0x800863E0` | remains control |
| `XENO_WORLD_UPLOAD_RECORDS_DOUBLE_TEST` | `BLOCKED/C` | idempotence control for `0x80074E58` | remains control |
| `XENO_WORLD_UPLOAD_RECORDS_B_DOUBLE_TEST` | `BLOCKED/C` | idempotence control for `0x80075030` | remains control |
| `XENO_WORLD_DRAW_PACKETS_DOUBLE_TEST` | `BLOCKED/C` | idempotence control for `0x800739B8` | remains control |
| `XENO_WORLD_88F64_DOUBLE_TEST` | `BLOCKED/C` | idempotence control for `0x80088F64` | remains control |

## Ranked decomp and integration backlog

The ordering below ranks by downstream gates unblocked. Existing bodies are
named even where the remaining work is control-flow integration rather than
fresh transcription.

### 1. Restore the normal world-init spine and collapse the early implication ladder

Make `PcPort_WorldMapInitMain` the normal world selector path and execute the
already ported retail sequence without environment implication. In dependency
order this covers:

`INIT -> MODE_INIT -> SECOND_WAVE -> OBJECT_POOL -> STATE_TEMPLATE ->`
`MODE_ENTER_STATE -> CROSS_PRODUCTS -> GPU_ASSET_A -> GPU_ASSET_B ->`
`OBJECT_MATRIX -> THIRD_WAVE -> BSS_CONSTANTS -> PRIMITIVE_TEMPLATES ->`
`RECORD_CLUT_INIT -> GFX_WORK_BUFFERS -> FT4_POOLS -> HEAP_TABLE_RAND ->`
`UPLOAD_RECORDS -> UPLOAD_RECORDS_B -> DRAW_PACKETS -> 88F64 ->`
`ARCHIVE_READY_POLL -> FIRST_WDS_CONSUMER -> ARCHIVE_SET_INDEX`.

This one integration spine controls **24 remaining gate names**, the largest
fan-out in the census. Most bounded bodies already exist; the non-mechanical
dependency is W34C23's field-to-world WDS lifecycle. The real loader rejects
the world bank because the field/music allocation still owns SPU range
`0x38000..0x67830`. Publication, transition cleanup, linked-list removal, and
`SoundSpuMemoryFreeBlock` must be witnessed before the WDS stage can become
normal flow.

### 2. Integrate the post-archive continuation in retail order

After archive transition and the now-unconditional W29B dispatcher, retire:

1. `READY_BUFFER_CONSUME` — `wm_ready_buffer_consume`,
   `0x80072558..0x800725A8`.
2. `MODE_AUDIO_SETUP` — `wm_mode_audio_setup`,
   `0x800725AC..0x800726C0`.
3. Two branches can then proceed partly in parallel:
   - display/terrain: `wm_80072BB0 -> wm_80097BC0`;
   - convergence: `wm_800726C0_convergence_p1 ->`
     `wm_8007272C_convergence_p2`.
4. Rejoin both branches at
   `COMMON_TAIL_P0 -> P1 -> P2 -> P3 -> P4 -> P5`.
5. Make the post-slot-1 `wm_80097800` scheduler call at `0x80071064`
   unconditional.

This chain covers 14 remaining gate names. Its isolated failures are the most
dangerous observed behavior: nine of the gates return normally with wrong
frames rather than announcing the missing stage.

### 3. Replace the bounded open-loop harness with retail session/frame control flow

`XENO_WORLD_OPEN_LOOP` is the **single largest architectural obstacle**. With
it off, the current code executes one legacy prologue, hard-cuts before
`0x800719C8`, and then remains in the placeholder without producing a bounded
frame. The replacement must recover the recurring `0x80071034` session/frame
flow, its inner back-edge, natural exit, and real dispatch targets. In
particular, `world_map_main_loop_71034.c:57-73` still substitutes explicit
defaults for guest callbacks `0x80072238`, `0x8007299C`, and unregistered mode
callbacks.

This work retires `OPEN_LOOP` and makes the legacy `FRAME_PROLOGUE` plus its
two reentry controls obsolete. It can be decoded in parallel with the
post-archive bodies, but it cannot become the normal route until the init and
post-archive sequences above provide the state it consumes.

### 4. Retire certificate-only controls after their owning route is unconditional

The seven `*_DOUBLE_TEST` variables and the bounded frame/reentry controls are
not missing game behavior. Keep them while their guarded bodies remain under
active retirement. Delete or move them into focused tests only after the
corresponding production route is unconditional and its certificates no
longer need environment-driven destructive probes.

## Dependency summary

```text
normal selector + early init (24 gates)
             |
             v
archive/WDS ownership -> W29B dispatcher (gate retired)
             |
             v
ready consume -> mode/audio
                  |       \
                  |        convergence P1 -> P2
                  v                         /
             framebuffer -> terrain ------+
                              |
                              v
                    common tail P0..P5
                              |
                              v
                         scheduler
                              |
                              v
                 retail recurring loop
```

The recurring loop can be researched independently, but natural execution
depends on the state produced above it. Conversely, collapsing the early
ladder without restoring the recurring loop still leaves the world map in a
placeholder after one diagnostic frame. Both are required for an unforced
native world map.
