# Current Status

> Last aligned to handoff/commits through `8db4fb2` (July 8, 2026).
> For live detail see [`ACTIVE_HANDOFF.md`](https://github.com/Blizz127/xenogears-decomp-ai/blob/main/docs/ai_context/ACTIVE_HANDOFF.md).

This page answers six questions the project tracks constantly.

---

## What currently works

### PC port infrastructure

| Area | Status | Notes |
|------|--------|-------|
| Native build/link | **Verified working** | `./pc_port/build_port.sh` → `pc_port/build_native/xeno-port` |
| PsyCross integration | **Verified working** | SDL2/OpenAL/OpenGL; documented patches in `build_port.sh` |
| Boot-path stub oracle | **Verified working** | Undefined symbols auto-stubbed; live path logs `[stub] <name>` |
| Kernel0 field route | **Verified working** | `XENO_KERNEL_SEL=0` reaches `FieldMain`, times out cleanly (`RUN_RC=124`) |

### Map 0 (default field harness)

| Area | Status | Notes |
|------|--------|-------|
| Field overlay load | **Verified working** | Field main loop runs |
| Actor sprite draw path | **Verified working** | `func_8001E3D8` links `POLY_FT4` into OT; user-confirmed sprite/fade/zoom |
| Work-list path | **Committed fix** | `pc_port/src/work_list_port.c` restored real work-list functions |
| `D_800ADC18` fade gate | **Decoded / trusted** | 4→0 over frames 0–4; `FieldAddPrimitives` gated behind it |
| Map0 actor draw list | **Verified working** | 6 visible sprite actors: 1, 2, 16, 23, 25, 26 (actor 18 correctly hidden) |

### Map 1 (field test harness)

| Area | Status | Notes |
|------|--------|-------|
| Map1 load/render | **Verified working** | With `XENO_FIELD_MAP=1` + entrance selector; Fei visible on terrain |
| Field background VRAM | **Temporary hack (opt-in)** | `XENO_FIELD_0BB_VRAM_UPLOAD=1` uploads streamed `0xBB` archive |
| Visible player control | **Verified working** | Real keyboard input moves Fei on rendered field (`b9266c5` milestone) |
| Walkmesh + movement chain | **Committed fix** | Triangle lookup, walk step, collision auto-move (`55a2dda`, `6bc1752`, `79c7ee7`, `e7b0101`) |
| Encounter trigger zones (2D) | **Proven (read-only)** | Zone 5 fires `func_80093B10` enable-encounters path (`94d8cfc`) |
| Exit trigger zone 11 reachability | **Proven (read-only)** | Height-valid zone 11 fires opcode 203 inside path |
| Exit op7 → op116 → op54 | **Committed fix** | Slot init + sprite pointer + sound shim (`9e1b667`, `ae8c753`) |

---

## What was proven

- **End-to-end visible field control** — input → opcode 0xA7 → animation → walkmesh → collision → on-screen movement (`b9266c5`).
- **Map1 has two trigger systems** — encounter zones 0–7 (2D) vs exit zones 8–11 (3D height-gated) (`0e2c181`, `94d8cfc`).
- **Opcode 7 stall root cause** — `func_80080A74` slot-init offset bug made idle actor script slots appear busy (`213b826`); fixed in `9e1b667`.
- **`func_80098CAC` pSprite bug** — masked slot-init issue; fixed in `9e1b667`.
- **`func_800855C8` is audio-side** — not a hard transition blocker; shim committed in `ae8c753`.
- **`func_80079288` is encounter bookkeeping** — exposed after op54 during held input; not the missing fade/map-load step (`ae8c753` docs).
- **Var `0x0462` is a local exit latch** on actor 48 routine 1, not a separate consumer (`4856738`, `e13c0e3`).
- **Actor 18 routine 4 is a short door animation** — not the map transition itself (`bb31b27`).
- **Shared door state var `0x0408`** — owned by actor 20 routine 1; actor 18 routine 1 branches on it (`0545ef3`, `8db4fb2`).

---

## What is still broken

| Blocker | Severity | Notes |
|---------|----------|-------|
| **Map1 exit transition incomplete** | **Active frontier** | Zone 11 fires, op7/op116/op54 run, but no fade/map-load yet |
| **Host `rand()` range mismatch** | **Active frontier** | Actor 20's `var0x0408` chooser expects PSX `0..32767`; host glibc `rand()` writes out-of-range values → always falls back to `1` (`8db4fb2`) |
| **Default boot Map0 black screen** | Known separate | Without `XENO_FIELD_MAP`, default route renders black — pre-existing, not a regression |
| **Battle route (`XENO_KERNEL_SEL=1`)** | Stubbed | `func_8001B6C4` is INCLUDE_ASM; immediate stub abort |
| **Direct menu route (`XENO_KERNEL_SEL=4`)** | Harness gap | `MenuMain` without overlay load hits `func_801C62A8` stub |
| **`func_800A5C40` at map reload** | Future blocker | INCLUDE_ASM; likely first hard blocker if full transition reached |
| **`func_80075B44` rare branches** | Assert-only | Four real asm branches not hit on Kernel0 route; not current gate |
| **Screen coordinate OT anomaly** | Unresolved | OT1 vs OT2 high-address bits differ; lower 32 bits consistent |
| **Entrances 6/10** | Harness-invalid | Out-of-range spawn-table walkmeshId; crash — do not use for milestones |
| **~250 generated function stubs** | Ongoing | Boot-path oracle still reports missing functions on broader routes |

---

## Which fixes are committed

Recent committed milestones (see [Phase Log](Phase-Log) for full list):

| Commit | What |
|--------|------|
| `e7b0101` | OP_UPDATE_CHARACTER player-control opcode (0xA7) |
| `79c7ee7` | Walk-step vector `func_80081F80` |
| `55a2dda` | Walkmesh triangle lookup and movement resolution |
| `6bc1752` | Collision-aware auto-move `func_80082620` + `func_800825AC` |
| `b9266c5` | First visible field-control milestone documented |
| `9e1b667` | Actor script slot init + `func_80098CAC` sprite pointer |
| `ae8c753` | PC-port sound shim for field transition cue (`func_800855C8`) |

Docs-only investigation checkpoints (no code): `94d8cfc`, `0e2c181`, `213b826`, `7441ca9`, `ef1015f`, `36f195d`, `4856738`, `e13c0e3`, `bb31b27`, `0545ef3`, `8db4fb2`.

---

## Which hacks are temporary

| Hack | Env / location | Purpose | Retail-final? |
|------|----------------|---------|---------------|
| `XENO_KERNEL_SEL` | `psyq_compat.c` | Force kernel menu state (field/battle/menu) | No — test harness |
| `XENO_FIELD_TEST` | `port_main.c` | Skip boot, enter field test path | No |
| `XENO_FIELD_MAP` | `port_main.c` | Set `D_8006F94E` map selector | No — coverage harness |
| `XENO_FIELD_ENTRANCE` | `port_main.c` | Set `D_8006F954` spawn entrance index | No — spawn harness |
| `XENO_FIELD_0BB_VRAM_UPLOAD` | `archive_port.c` | Synchronous `0xBB` VRAM drain | No — opt-in; needs retail-shaped loader |
| `func_800855C8` PC-port no-op | `misc8.c` (`ae8c753`) | Skip deep sound chain on transition cue | Shim — documented audio boundary |
| PsyCross GPU patches | `build_port.sh` | Font rendering, DR_MODE length, dfe draw | Port compatibility fixes |
| Generated function stubs | `pc_port/build_native/stubs.c` | Oracle for missing decomp | Temporary by design |

---

## Which systems are decoded enough to trust

| System | Trust level | Evidence |
|--------|-------------|----------|
| Field script VM dispatch | **High** | Two handler tables; byte `254` = extended opcode via `g_FieldScriptVMHandlers2` |
| Opcode 0xA7 `OP_UPDATE_CHARACTER` | **High** | Retail asm + committed implementation; `scriptFlags & 0x4000` gate |
| Opcode 7 actor-script start | **High** | `func_8009EB78` free-slot scan matches retail |
| Opcode 203 3D trigger zones | **High** | Height gate + inside path at `misc11.c:992` traced live |
| Actor script slot layout | **High** | Slot base `p+0x8C+i*8`; flag word semantics proven |
| Walkmesh movement chain | **High** | End-to-end position advance with real input |
| `D_800ADC18` fade-in gate | **High** | Init/decrement/gating traced in source |
| Map1 encounter zones 0–7 | **High** | All route to `func_80093B10` |
| Map1 exit zones 8–11 | **High** | Separate 3D height-gated system; zone 11 reachable |
| Actor 20 `var0x0408` state machine | **Medium-high** | Static decode + runtime writes traced; RNG cause identified |
| Camera Z-clamp (`func_8007CD80`) | **High** | Inverted clamp bug found and fixed (July 5 handoff) |
| Noah reference | **Low (research only)** | Non-matching; confirm against SLUS asm/runtime always |
