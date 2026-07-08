# Xenogears PC Port — Cleanup + Provenance Audit

> **Scope of this document:** docs-only map of runtime state, technical debt,
> borrowed/reference-derived areas, and cleanup priorities.
>
> **This pass did not edit game code.** No hacks removed, no systems rewritten,
> no broad renames, no renderer/input/camera/actor/script behavior changes.
>
> **Date:** 2026-07-08  
> **Authoring branch:** `xenogears-cleanup-provenance-audit`  
> **Companion journal:** `docs/ai_context/ACTIVE_HANDOFF.md`  
> **Reference policy:** `docs/ai_context/REFERENCE_SOURCES.md`

---

## 1. Repo state

| Item | Value |
|------|--------|
| **Branch (audit)** | `xenogears-cleanup-provenance-audit` |
| **Branched from** | `ai-private-main` @ `8db4fb28fa847b64c57d90b7c73f883ef979a7fd` |
| **HEAD** | `8db4fb2 Document actor 20 var0408 writer finding` |
| **Working tree** | Clean at audit start (unrelated WIP handoff stashed as `wip-handoff-before-audit`) |
| **Upstream decomp** | `upstream` → `ladysilverberg/xenogears-decomp` |
| **Private remote** | `origin` → `Blizz127/xenogears-decomp-ai` |
| **`main` tip** | `f27c076` (CI checksum only) — **far behind field milestones** |
| **Commits on `ai-private-main` not on `main`** | ~183 |
| **Build (this worktree)** | No `pc_port/build_native/xeno-port` present |
| **Build (host project path)** | `/home/blizz/Projects/xenogears-decomp/pc_port/build_native/xeno-port` exists (mtime 2026-07-08 ~12:08) |
| **Known-good gameplay test** | Map1 entrance 0 or 8: visible Fei + keyboard movement (`XENO_FIELD_TEST=1`, timeout RC=124) |

### Branching note

The requested recipe was:

```bash
git switch main && git pull && git switch -c xenogears-cleanup-provenance-audit
```

That would base cleanup on a tip that **lacks** the field-control / trigger /
walkmesh milestones. This audit branch was created from **`ai-private-main`**,
where those fragile wins live. Rebase/merge strategy onto public `main` is a
later packaging decision, not a cleanup prerequisite.

### Build status (as of HEAD)

- Native field path is expected to **link** with generated stubs (`gen_port_stubs.py`).
- Recent verification logs (see handoff) report **`LINK OK`**, Map1 smokes
  **`RUN_RC=124`** (timeout, no crash), Map0 guard still timeout-stable.
- Stub oracle count was historically ~259–700 depending on phase; post-recovery
  Kernel0 field path can run with **zero `[stub]` lines** on the default route
  after `func_8009AD6C`. Map1 still exercises soft stubs and shimmed side paths.

---

## 2. Current working milestone

### Known-good baseline (do not regress)

| Capability | Status | Evidence |
|------------|--------|----------|
| **Map / entrance where Fei is visible** | Map1, entrances **0** and **8** (also 9 for exit routes) | Milestone commits `55a2dda` + `6bc1752`; handoff “First visible field-control” |
| **Movement** | Works (walkmesh + collision auto-move) | `func_80081F80`, `func_8007B1C4`, `func_8007BAC0`, `func_80082620` |
| **Input** | Real keyboard → pad buffer → field mask | `f73d495` Vsync pad push; arrows = D-pad, C = run/Cross |
| **2D trigger zones** | Fire (Map1 zones enable random encounters) | Zone 5 path @ entrance 8; `FieldScriptHandleTriggerZone2D` |
| **3D exit zones** | Reachable + fire (zones 9/11); **no full map transition yet** | Zone 11 @ entrance 9; op7/op116/op54 run; no fade/map-load |
| **Player actor** | Actor **1** (`g_PlayerActorIndex=1`) | Handoff milestone |

### Exact repro command (visible control)

Run on host via distrobox; needs a live X display for real input:

```bash
distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp && \
  SDL_VIDEODRIVER=x11 XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 \
  XENO_FIELD_MAP=1 XENO_FIELD_ENTRANCE=0 XENO_FIELD_0BB_VRAM_UPLOAD=1 \
  ./pc_port/build_native/xeno-port'
```

- **Fei** = actor 1. Arrows = D-pad; **C** = run/Cross.
- **`XENO_FIELD_ENTRANCE=8`** also works for control + zone-adjacent spawn.
- Expected: field renders (Fei visible), position advances; timeout **RC=124**.
- **Do not use entrances 6/10** for the control milestone — harness-invalid /
  crashy spawn-table cases historically.
- **Default boot without `XENO_FIELD_MAP`** (“Map0”) can render black — known
  separate issue, not a Map1 regression.

### Exit / progression smoke (not yet a full transition)

```bash
# Same binary env, entrance 9; synthetic +X then +Z reaches zone 11 ~frame 134
XENO_FIELD_MAP=1 XENO_FIELD_ENTRANCE=9 XENO_FIELD_0BB_VRAM_UPLOAD=1
# Direction map: D_800AFE9C 0x1000=+X · 0x2000=+Z · 0x4000=-X · 0x8000=-Z
```

**What works end-to-end on that route:**

1. Fei walks into height-valid exit zone 11.
2. Opcode **203** `FieldScriptCheckTriggerZone` inside path fires.
3. Opcode **7** starts actor **18** routine **4** (after slot-init fix `9e1b667`).
4. Opcode **116** sound cue hits PC sound shim `func_800855C8`.
5. Opcode **54** latches script var `0x0462` (local exit latch on actor 48 path).

**What does not work yet:** fade-out, map-load, `func_800A5C40` seamless reload.
Exit path investigation moved into **shared script var `0x0408`** (door-state
driven by actor 20), not op7/slot/op54 failure.

### Demo / capture artifacts (ignored by git)

Under `captures/render_diag/` (gitignored): Fei control repro, Map1 zone/exit
logs, combined slot-init verify, smoke logs. Canonical write-ups are mirrored
into `ACTIVE_HANDOFF.md`.

---

## 3. Technical debt inventory

Debt types used below:

| Type | Meaning |
|------|---------|
| **hack** | Intentional non-retail behavior to keep host alive |
| **stub** | Generated or hand no-op for missing C |
| **assert guard** | `assert(0)` / branch not migrated (often no-op under PC `#define assert`) |
| **layout guess** | Struct/offset assumed from asm; may still be wrong |
| **borrowed scaffold** | Structure/approach from outside project |
| **instrumentation** | Diag printf / env harness not part of retail |
| **temporary workaround** | Documented stopgap with planned removal |

### 3.1 Port harness / platform (required for current progress)

| File / function | Debt type | Why it exists | Required now? | Risk if removed | Recommended action |
|-----------------|-----------|---------------|---------------|-----------------|--------------------|
| `pc_port/src/port_main.c` — `XENO_FIELD_TEST`, `XENO_FIELD_MAP`, `XENO_FIELD_ENTRANCE` | instrumentation / harness | Skip full boot; force field map + spawn | **Yes** for repro | Lose Map1 demos | Keep; document in smoke-test section |
| `pc_port/src/psyq_compat.c` — `XENO_KERNEL_SEL` / `Vsync` pad update | harness + input delivery | Force FieldMain; refresh pad each frame | **Yes** | No field auto-entry / dead input | Keep; do not “clean” into camera code |
| `pc_port/src/psx_memory.c` | borrowed scaffold | 2 MB (+guard) PSX RAM buffer | **Yes** | Hardcoded addr paths break | Keep; reverify bounds later |
| `pc_port/src/game_overrides.c` — `PcPort_InitGameStates`, pad tables, `LZSSDecompress` size guard | hack / temporary workaround | Overlay not disc-loaded; bad size would walk RAM | **Yes** | Crash on overlay load | Keep; later real overlay archive |
| `pc_port/src/work_list_port.c` | port scaffold | Decomp TU skipped; work-list needed for sprites | **Yes** for visibility | Fei draw dies | Keep until real TU links |
| `pc_port/src/archive_port.c` — `XENO_FIELD_0BB_VRAM_UPLOAD` | temporary workaround | Stream field VRAM sections opt-in | Optional for Fei; improves field look | Wrong VRAM layout can hide Fei | Keep default-off; document |
| `tools/scripts/gen_port_stubs.py` + generated `stubs.c` | stub oracle | Phase-1 “next function” discovery | **Yes** for incomplete decomp | Link fails or silent wrong | Keep; shrink by real C only |
| Host `rand()` vs PSX `rand()` (0..32767) | hack / platform bug | glibc `rand` range breaks script RNG opcodes | Breaks door-state / encounters | Scripted random branches wrong | **Candidate tiny P3 fix** (see §7) |

### 3.2 Field / actor / script (gameplay-critical — treat as P0)

| File / function | Debt type | Why it exists | Required now? | Risk if removed | Recommended action |
|-----------------|-----------|---------------|---------------|-----------------|--------------------|
| `func_80080A74` slot init (`misc8.c`) | former layout bug, now fixed | Wrong slot base made idle slots “busy” | Fix is **required** for exit op7 | Breaks exit script start | **P0 do not re-break**; already verified |
| `func_80098CAC` `pSprite` assign (`misc7.c`) | former layout bug, now fixed | Missing assignment SEGV once slots free | **Required** | Crash on script move | **P0** |
| `func_800855C8` sound shim (`misc8.c`) | temporary workaround | Deep sound chain not ported; transition cue only | Soft | `[stub]` noise or block if re-stubbed wrongly | Keep shim until audio port |
| `func_80079288` (generated stub) | stub | Encounter bookkeeping; not transition | Soft | Encounter noise only | Optional dedicated no-op later; **not** battle port |
| `func_8007CD80` Z-clamp fix | verified retail fix | Map1 camera fallback off-screen | **Required** for Fei visible | Map1 black / off-screen | **P0** |
| Walkmesh / movement suite (`func_80081F80`, `func_8007B1C4`, `func_8007BAC0`, `func_80082620`) | verified Xenogears path | Field control chain | **Required** | Lose movement milestone | **P0** |
| `func_8009F5F4` OP_UPDATE_CHARACTER | verified + Noah-named | Player control opcode 0xA7 | **Required** | Lose control | **P0** |
| `ActorData` / `FieldActor` pointer-as-u32 layout (`include/field/actor.h`) | layout (retail-shaped) | PSX pointer width fidelity | **Required** | Mass breakage | **P0** — no struct rewrite |
| Camera eye/at layout fixes (`e656748`, `2430bcf`) | verified layout | Camera globals wrong on host | **Required** for framing | Camera regression | **P0** |
| `func_80075B44` rare-branch asserts | assert guard | Unmigrated special draw paths | Not hit on Kernel0 | Broader maps may assert/no-op | Leave until reached |
| `func_800248D4` / `func_80022660` anim opcode asserts | assert guard | Incomplete anim VM | Hit on some entrances/paths | Crash RC=134 | Only implement when path needs it |
| `temp2.c` `func_8002C644` heap pin guard | temporary workaround | Host heap corruption from baked model “end” ptr | **Yes** for stability | Crash / heap corruption | Keep; document |
| `misc8.c` div-by-zero guards on moveSpeed | temporary workaround | PSX tolerates; host SIGFPE | Safety | Host crash | Keep |
| `misc3.c` model-build / null table guards | temporary workaround | Stubbed tables | Stability | Null deref | Keep; shrink when tables real |
| `XENO_FIELD_NO_MODEL_BUILD` | instrumentation | Diagnostic opt-out | No (default ON) | N/A | Keep env only |
| `[field-diag]` printfs (misc2/3, rendering, VM, temp2) | instrumentation | Load/draw tracing | No for ship | Log noise only if ungated | **P2**: confirm all gated / remove dead |

### 3.3 Instrumentation / env flags (cleanup-friendly)

| Flag / symbol | Purpose | Cleanup priority |
|---------------|---------|------------------|
| `XENO_FIELD_DIAG` | Opt-in `[field-diag]` | P1 document; P2 audit ungated prints |
| `XENO_FIELD_TEST` | Force field test path | Keep (harness) |
| `XENO_KERNEL_SEL` / `XENO_KERNEL_DELAY` | Auto-pick kernel menu | Keep |
| `XENO_FIELD_MAP` / `XENO_FIELD_ENTRANCE` | Map/spawn harness | Keep |
| `XENO_FIELD_0BB_VRAM_UPLOAD` | Opt-in field VRAM | Keep default-off |
| `XENO_FIELD_NO_MODEL_BUILD` | Skip model build | Keep diagnostic |
| `XENO_DISC` | Disc image path | Keep |

### 3.4 Decomp debt (not “port hacks” but still blocks completeness)

- Large `INCLUDE_ASM` surface in field/system (inert under `XENO_PC_PORT` /
  `SKIP_ASM` → becomes undefined → stubs).
- Battle state `func_8001B6C4` stub (`XENO_KERNEL_SEL=1`).
- Menu overlay entry `func_801C62A8` (needs overlay load, not a fake C body).
- `func_800A5C40` still INCLUDE_ASM — likely first hard blocker **if** a real
  map transition is reached.

---

## 4. Provenance inventory

Target long-term:

> Xenogears behavior verified from Xenogears evidence, with outside projects
> used as **references**, not as unexplained copied foundations.

### 4.1 Clearly verified from Xenogears (asm / runtime / matching intent)

These are decomp or port-first ports checked against SLUS_006.64 asm and/or
gdb on Map1:

| Area | Examples | Evidence style |
|------|----------|----------------|
| Walkmesh clamp / movement | `func_8007CD80` Z-clamp, `func_80081F80`, `func_8007B1C4`, `func_8007BAC0`, `func_80082620` | Asm + Map1 visual/position |
| Player control opcode | `func_8009F5F4` (0xA7) | Asm; Noah naming only |
| Script slot init + script move | `func_80080A74`, `func_80098CAC` | Asm + zone-11 op7 proof |
| Trigger zones | `FieldScriptHandleTriggerZone2D`, `FieldScriptCheckTriggerZone` | Runtime IP/zone traces |
| Field VRAM 0xBB drain | `PcPortDrain0xBBToVram` / retail format | Format verified; opt-in |
| Camera vector layout | eye2 store, camera init fixes | Layout + framing |
| ActorData size 0x138 / FieldActor 0x5C | `include/field/actor.h`, `misc8.c` comments | Offset discipline from PSX |

### 4.2 Generic platform / scaffolding (expected for any PC port)

| Component | Role | Provenance notes |
|-----------|------|------------------|
| PsyCross (vendored, gitignored `pc_port/extern/PsyCross`) | GTE/GPU/SPU/CD HAL | Third-party HAL; not game logic |
| SDL2 / OpenAL / OpenGL | Host window/audio/input | Standard |
| `gen_port_stubs.py` stub oracle | Missing-function discovery | Original to this port approach |
| `XENO_FIELD_*` harness env | Deterministic field tests | Port-only |
| Disc image load path | `XENO_DISC` / `disc/` | Port-only |

### 4.3 Adapted / inspired by outside projects

| Source | What was taken | License / policy status |
|--------|----------------|-------------------------|
| **Silent Hill decomp** ([Vatuu/silent-hill-decomp](https://github.com/Vatuu/silent-hill-decomp)) | Project **structure template** (README thanks) | Cited as template inspiration; **not** game logic copy |
| **Silent Hill PC port** ([SlickAmogus/silent-hill-decomp](https://github.com/SlickAmogus/silent-hill-decomp)) | `pc_port` spirit; **PSX RAM emulation pattern** (`psx_memory.c/.h` explicitly “Modeled on” / “Mirrors”) | Scaffolding pattern; must stay host-only; reverify if sharing publicly |
| **Ship of Harkinian / Shipwright** | Named architectural analogy in `pc_port/README.md` | Analogy only |
| **yaz0r/Noah** | **Reference-only**: naming, VM opcodes, field behavior comparison | Policy in `REFERENCE_SOURCES.md`: no direct import without review; must confirm vs asm/runtime. Clone outside repo (`~/Projects/xenogears-reference/Noah`) |
| **splat / spimdisasm / maspsx / objdiff** | Decomp toolchain | Standard decomp ecosystem |

### 4.4 Explicit in-code outside-project citations

| Location | Citation |
|----------|----------|
| `README.md` | Silent Hill decomp as structure template |
| `pc_port/README.md` | Silent Hill PC port + Shipwright spirit |
| `pc_port/src/psx_memory.h` / `.c` | “Modeled on” / “Mirrors the Silent Hill port” |
| `pc_port/src/port_main.c` boot log | “Silent-Hill-style: PSX RAM emu + runtime dispatch table” |
| `src/field/main/misc6.c` `func_8009F5F4` | “Noah OP_UPDATE_CHARACTER is the readable reference” (asm is truth) |
| `docs/ai_context/REFERENCE_SOURCES.md` | Full Noah policy |

### 4.5 Needs rewrite / reverification before public / release-quality

| Item | Why |
|------|-----|
| Full field background / VRAM layout vs actor textures | Opt-in upload still clobber-sensitive |
| Sound subsystem | Shim at `func_800855C8`; deeper SPU path unported |
| Random / encounter path | Host `rand` range; `func_80079288` stub |
| Battle / menu overlays | Stubbed or overlay-entry stubs |
| Any code path only validated on Map1 ent 0/8/9 | Broader maps unproven |
| Silent Hill–pattern RAM / pad wiring | Correct for port goals but not “matching decomp purity”; document boundaries |
| Noah-informed handlers without second source | Re-check every one against asm before claiming matching |

### 4.6 License snapshot

| Component | Observation |
|-----------|-------------|
| **This decomp repo** | No top-level `LICENSE` file found in tree at audit time; upstream is public decomp of retail game (usual decomp legal grey area: no commercial redistribution of assets/binaries). |
| **Retail assets / disc** | User-supplied; not in git (disc ignored). |
| **PsyCross** | Vendored outside tracked tree; use its upstream license when redistributing a binary port. |
| **Noah** | External reference only; do not vendor. Check Noah’s license before any future deliberate code import. |
| **Silent Hill ports/decomps** | Structural inspiration; do not copy their game code. |

**Practical rule for future sharing:** share decomp + port scaffolding with clear
asset requirements; do not ship disc images; cite references; prefer
asm-verified game logic over unexplained foreign foundations.

---

## 5. Gameplay-risk systems

These systems already have fragile wins. Treat as **high blast radius**.

| System | Why risky | Current known-good role | Cleanup stance |
|--------|-----------|-------------------------|----------------|
| **Camera** | Wrong target/layout = black field / off-screen Fei | Framing after walkmesh clamp + camera layout fixes | **P0 — do not touch** |
| **Actor layout** | `ActorData` 0x138, pointer-as-u32, script slots at 0x8C | Slot init + script move depend on exact offsets | **P0 — no rewrite** |
| **Input delivery** | Pad buffer → `ControllerPoll` → `D_800AFE9C` | Field control and zone routing | **P0** |
| **Walkmesh / collision** | Triangle lookup + auto-move | Visible movement milestone | **P0** |
| **Script VM** | Opcodes 0–482 tables, extended dispatch, slot priority | Triggers, control opcode, exit path | **P0**; only add opcodes when blocked |
| **Renderer** | OT link, fade gate `D_800ADC18`, VRAM, prim cull | Fei + field draw | **P0 architecture**; micro-fixes only when proven |
| **Field globals** | `g_GameSceneMapNum`, fade counters, encounter flags, script memory | Map select, transitions, encounters | Document before changing |

### Failure-category checklist (when something breaks)

Use this before rewriting:

1. Wrong original behavior (need asm/runtime)?
2. Bad AI-generated / incomplete port transcription?
3. Borrowed scaffold mismatch (Silent Hill / PsyCross)?
4. Temporary hack still required?
5. Data layout / pointer-width mismatch?
6. Renderer / host platform assumption?

---

## 6. Cleanup priority

### P0 — Do not touch yet

- Camera core (`func_80072A38` and related framing)
- Actor struct / `ActorData` layout redesign
- Script VM architecture rewrite
- Movement / walkmesh / collision rewrite
- Renderer architecture rewrite
- Working slot-init / `pSprite` / OP_UPDATE_CHARACTER / walkmesh fixes
- Map1 ent 0/8 visible-control path

### P1 — Docs / test cleanup only (this phase)

- [x] This audit document
- [ ] Smoke-test doc section (commands, entrances, expected RC) — **included in §2**; optional split file later
- [ ] Hack/stub ledger living list (this §3 is the first cut)
- [ ] Provenance labels in README / REFERENCE_SOURCES cross-links
- [ ] Known-good **git tag** suggestion: `known-good-map1-field-control` @ `b9266c5` or later verified smoke HEAD
- [ ] Keep `ACTIVE_HANDOFF.md` as deep journal; this file as cleanup map

### P2 — Remove obsolete instrumentation (after docs)

- **Done (docs):** full inventory + classification + host-rand smoke protocol in  
  `docs/ai_context/XENOGEARS_P2_INSTRUMENTATION_AUDIT.md` (2026-07-08).
- **Done (code):** always-on `[field-diag]` in `misc3.c` / `temp2.c` / `virtual_machine.c`
  now honors `XENO_FIELD_DIAG` (diagnostic-gating only; no VM/load behavior change).
- gdb probes stay under gitignored `captures/`; stub oracle stays until replaced deliberately.
- Next code PR: PSX rand range (do not mix with further log cleanup).

### P3 — Replace one hack with verified behavior (one at a time)

Candidates (ordered by risk/reward for current Map1 work):

1. **PSX-compatible `rand()`/`srand()` on PC** — fixes script RNG / door-state `0x0408` (actor 20); tiny surface; does not touch camera/actor layout.
2. Narrow encounter shim only if stub noise blocks diagnostics (`func_80079288`) — **do not** implement battle load.
3. Next real map-transition blocker only after RNG/script-state clarified.

### P4 — Later refactor

- Overlay archive load (retire `LZSSDecompress` size stopgap)
- Real audio path (retire `func_800855C8` shim)
- Link real work-list TU instead of `work_list_port.c`
- Battle / menu / worldmap kernels
- Broad symbol renames for readability
- Merge private field stack onto public `main` with CI green

---

## 7. Recommended next code-cleanup target

### One tiny low-risk item only

**Implement PSX-range `rand()` / `srand()` for the PC port** (or force link
`src/slus_006.64/psyq/libc.c` RNG and ensure host `rand` is not used).

| | |
|--|--|
| **Why** | Retail field scripts use `(rand() * (n+1)) >> 15` expecting `0..32767`. Host glibc `rand` yields huge values; Map1 actor 20 forces shared door-state `0x0408` out of range then clamps to `1`, masking other script states. |
| **Why low risk** | Does not touch camera, actor layout, walkmesh, renderer, or VM dispatch. Single libc surface. |
| **Verify** | Rebuild; Map1 ent 0 control smoke RC=124; actor 20 `0xA8` writes produce `0..4`; no Map0 crash. |
| **Do not** | Patch actor scripts or force `0x0408` in memory. |

### Explicit non-targets for “next”

- Camera core  
- Actor struct rewrite  
- Script VM rewrite  
- Movement/collision rewrite  
- Renderer architecture rewrite  
- Full sound engine  
- Full encounter/battle path  

---

## 8. Suggested cleanup sequence (recap)

```text
Now:     cleanup audit + provenance audit + smoke-test docs   ← this document
Next:    known-good tag + (optional) P2 instrumentation audit
Then:    one P3 fix (PSX rand) with smoke verification
Later:   replace one hack at a time with verified Xenogears behavior
Never:   bulk “cleanup” of P0 systems because the tree feels messy
```

### Will cleanup help gameplay?

**Yes, indirectly first.** It makes actor layout, field/camera globals, input,
walkmesh, script VM, triggers, and renderer assumptions safer to extend by
separating:

- wrong original behavior  
- bad/incomplete port code  
- borrowed scaffolding  
- temporary hacks  
- layout mismatches  
- host/renderer assumptions  

That reduces “debugging ghosts” while preserving the Map1 field-control wins.

---

## 9. Commit / process notes for this pass

- **Docs only** created: `docs/ai_context/XENOGEARS_CLEANUP_AND_PROVENANCE_AUDIT.md`
- **No code changes**
- Uncommitted handoff notes about host-`rand` (if any) may exist as a stash
  (`wip-handoff-before-audit`); merge those into `ACTIVE_HANDOFF.md` in a
  separate docs commit if desired — not required for this audit file.

### Suggested follow-up commands (human)

```bash
# Optional known-good tag on verified milestone (after re-smoke if desired):
# git tag -a known-good-map1-field-control b9266c5 -m "First visible field-control"

# After this docs commit is accepted, next code pass (separate PR):
# PSX-compatible rand/srand for XENO_PC_PORT only
```

---

*End of audit. Update this file when debt is retired or provenance reclassified;
keep `ACTIVE_HANDOFF.md` as the chronological experiment journal.*
