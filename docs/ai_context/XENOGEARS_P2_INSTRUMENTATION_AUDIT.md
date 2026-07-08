# Xenogears PC Port — P2 Instrumentation Audit

> **Scope:** inventory and classify runtime probes only.  
> **No code changes.** No rand fix. No gameplay systems touched.  
> **Stash:** leave `wip-handoff-before-audit` alone until a deliberate handoff refresh.  
> **Date:** 2026-07-08  
> **Branch:** `xenogears-cleanup-provenance-audit`  
> **Parent audit:** `docs/ai_context/XENOGEARS_CLEANUP_AND_PROVENANCE_AUDIT.md`  
> **HEAD at write:** `10d9517` (cleanup/provenance audit) on top of `8db4fb2`

---

## Answers (executive)

| # | Question | Answer |
|---|----------|--------|
| 1 | What instrumentation is currently active? | Boot harness logs always on; **several `[field-diag]` sites always on** under `XENO_PC_PORT` despite the name; render/OT `[field-diag]` correctly gated by `XENO_FIELD_DIAG`; stub oracle on first hit; 0xBB logs when upload env set; gdb scripts live only under gitignored `captures/`. |
| 2 | Required for Map1 control / actor-20 smoke? | **Harness env + binary + (for actor-20) gdb script.** No in-tree printf is required to prove control or host-rand. Control = visual/timeout RC=124. Actor-20 rand = gdb write/cond trace on `0x0408`. |
| 3 | Stale / noisy / dangerous? | **Always-on** FieldLoad/`func_8002C644`/`func_80076AC0`/`[FieldMain]` logs are noisy but low-risk. Stub spam is noisy oracle (keep). **Dangerous:** synthetic d-pad injection gdb that mutates `D_800AFE9C`; un-gated diag that could hide real failures in log flood; `XENO_FIELD_NO_MODEL_BUILD=1` (behavior change). |
| 4 | Host-rand before/after proof? | Baseline log already exists; fixed gdb recipe + Map1 ent9 route; success = actor-20 `WRITE_0408` values in `0..4` (not 30k+), control smokes still RC=124. See §5. |

---

## 1. What instrumentation is currently active?

### 1.1 Classification legend

| Class | Meaning |
|-------|---------|
| **ALWAYS** | Active on every PC-port field run (no env gate) |
| **OPT-IN** | Requires env var |
| **ORACLE** | Generated stub first-hit stderr |
| **EXTERNAL** | gdb/scripts under `captures/` (not in binary) |
| **BEHAVIOR** | Changes game path (not mere logging) |

### 1.2 Harness / boot (pc_port) — ALWAYS unless noted

| Tag / site | File | Gate | Role |
|------------|------|------|------|
| `[xeno-port] booting…` | `pc_port/src/port_main.c` | ALWAYS | Boot banner (mentions Silent-Hill-style scaffold) |
| `[xeno-port] PSX RAM…` | `pc_port/src/psx_memory.c` | ALWAYS | RAM buffer size/addr |
| `[xeno-port] CD image` / ArchiveInit / disc warning | `port_main.c` | ALWAYS | Disc wiring |
| `[xeno-port][field] XENO_FIELD_MAP/ENTRANCE` | `port_main.c` | ALWAYS when env set | Confirms harness applied |
| `[xeno-port] entering MainLoop` / Clean shutdown | `port_main.c` | ALWAYS | Lifecycle |
| `[xeno-port][test] forcing KernelMenu select` | `psyq_compat.c` | ALWAYS when `XENO_KERNEL_SEL` set | Auto-enter field |
| `[xeno-port] LZSSDecompress: implausible size` | `game_overrides.c` | On bad overlay size | Stopgap safety (stderr) |
| `[field-0bb] …` | `archive_port.c` | **OPT-IN** `XENO_FIELD_0BB_VRAM_UPLOAD` | VRAM section drain progress |
| `[psyq_compat] ReadTIM: …` | `psyq_compat.c` | Error paths | TIM parse failures |
| `[Psy-X] …` | PsyCross | ALWAYS | Host HAL noise |

### 1.3 Field path — mixed gates

| Tag / site | File | Gate | Cap / notes |
|------------|------|------|-------------|
| `[FieldMain] …` via `FM_LOG` | `src/field/main/main.c` | **ALWAYS** on `XENO_PC_PORT` | ~7 lines per FieldMain entry (entry, mode, controllers, map, UI, loop, teardown) |
| `[field-diag] FieldLoad begin` | `misc3.c:553` | **ALWAYS** PC | Should be under `XENO_FIELD_DIAG` |
| `[field-diag] assets before VM` | `misc3.c:642` | **ALWAYS** PC | same |
| `[field-diag] sprite package loaded` | `misc3.c:723` | **ALWAYS** PC | same |
| `[field-diag] actors allocated` | `misc3.c:754` | **ALWAYS** PC | same |
| `[field-diag] before/after VM` | `misc3.c:912,936` | **ALWAYS** PC | same |
| `[field-diag] func_8002C644: skip/real pin` | `temp2.c:275,281` | **ALWAYS** PC | Skip capped at 4 prints; **real pin uncapped** |
| `[field-diag] func_80076AC0 first` | `virtual_machine.c:591` | **ALWAYS** PC | Once per process (static) |
| `[field-diag] submit[…]` | `misc2.c` FieldAddPrimitives | **OPT-IN** `XENO_FIELD_DIAG` | First 8 |
| `[field-diag] frame=… primSubmits` | `misc2.c` | **OPT-IN** | First 8 frames |
| `[field-diag] func_80075B44 draw/frame` | `misc2.c` | **OPT-IN** | First 4 draw / 8 frame summaries |
| `[field-diag] func_8001E3D8 call/link/done` | `rendering.c` | **OPT-IN** | First 8 calls / links |
| `[xeno-port] missing D_8004FE50 …` | `temp2.c` | On missing prim table | stderr; model-build related |

`XenoFieldDiagEnabled()` exists only in **`misc2.c`** and **`rendering.c`**.  
`misc3.c`, `temp2.c`, and `virtual_machine.c` print `[field-diag]` **without** that gate. That is the main P2 inconsistency.

### 1.4 Stub oracle — ORACLE

| Mechanism | Source | Behavior |
|-----------|--------|----------|
| `[stub] <name>` | `tools/scripts/gen_port_stubs.py` → generated `stubs.c` | First call logs to stderr, returns 0 |

Observed soft stubs on Map1 control smoke (example  
`combined_slotfix_visible_control_ent0_20260708_104518.log`):

- `func_8008D0F4`, `func_8009E91C`, `func_80072254`, `func_80085C90`, `func_8008CD48`

On movement-held routes also: `func_80079288` (encounter bookkeeping — classified elsewhere as non-transition).

### 1.5 Behavior-changing env (not “logs”)

| Env | Default | Effect if set |
|-----|---------|---------------|
| `XENO_FIELD_TEST=1` | off | Field test path + font/party init |
| `XENO_KERNEL_SEL=0` | off | Force FieldMain via fake Circle |
| `XENO_KERNEL_DELAY` | 60 | Frames before force-select |
| `XENO_FIELD_MAP=<n>` | unset | Force map number |
| `XENO_FIELD_ENTRANCE=<n>` | unset | Force spawn table index → script var 2 |
| `XENO_FIELD_0BB_VRAM_UPLOAD=1` | off | Stream field VRAM archive |
| `XENO_FIELD_DIAG=1` | off | Enable **gated** render/OT/draw diag only |
| `XENO_FIELD_NO_MODEL_BUILD=1` | **build ON** | Skips model build + model draw — **not retail** |
| `XENO_DISC` | auto-search | Disc image path |

### 1.6 External probes (gitignored `captures/render_diag/`)

Not compiled in. Required only when re-proving script state:

| Artifact | Purpose |
|----------|---------|
| `map1_actor20_var0408_branch_trace_20260708.gdb` | Host-rand / `0x0408` writer proof |
| `map1_actor20_var0408_branch_trace_20260708_151951.log` | **Baseline BEFORE rand fix** |
| Other `map1_*_*.gdb` | Exit zone, op7, slot init, etc. — historical |

---

## 2. Required for Map1 control vs actor-20 smoke

### 2.1 Map1 visible control (Fei movable)

| Need | Required? | Notes |
|------|-----------|--------|
| Binary `pc_port/build_native/xeno-port` | **Yes** | Host path currently used |
| Disc image | **Yes** | `disc/disc1.bin` or `XENO_DISC` |
| `XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0` | **Yes** | Field entry |
| `XENO_FIELD_MAP=1` | **Yes** | Map1 |
| `XENO_FIELD_ENTRANCE=0` or `8` | **Yes** | Known-good spawns |
| `XENO_FIELD_0BB_VRAM_UPLOAD=1` | Recommended | Background; Fei can show without |
| Live X display (`SDL_VIDEODRIVER=x11`) | **Yes** for real keyboard | Headless needs Xvfb+xdotool |
| Any `[field-diag]` printf | **No** | Pure noise for this smoke |
| `XENO_FIELD_DIAG` | **No** | Must stay off for quiet control smoke |
| gdb | **No** | Visual + RC=124 is enough |
| Stub oracle | **No** for pass/fail | Soft stubs OK if RC=124 and Fei moves |

**Pass criteria (control):**

```text
RUN_RC=124 (timeout, no crash/assert abort)
Field renders; Fei (actor 1) visible
Arrow keys move; C = run
Optional log sanity: XENO_FIELD_MAP=1 and XENO_FIELD_ENTRANCE printed
```

**Exact control smoke command:**

```bash
distrobox enter xenogears-dev -- bash -lc '
  cd /home/blizz/Projects/xenogears-decomp && \
  timeout -s KILL 20 env \
    SDL_VIDEODRIVER=x11 \
    XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 \
    XENO_FIELD_MAP=1 XENO_FIELD_ENTRANCE=0 \
    XENO_FIELD_0BB_VRAM_UPLOAD=1 \
    ./pc_port/build_native/xeno-port \
  ; echo RUN_RC=$?'
```

Repeat with `XENO_FIELD_ENTRANCE=8` for second known-good entrance.  
**Do not** set `XENO_FIELD_DIAG` or `XENO_FIELD_NO_MODEL_BUILD` for this smoke.

### 2.2 Actor-20 / host-rand / `var0x0408` smoke

In-binary logs **do not** print script var writes. Proof is **EXTERNAL gdb**.

| Need | Required? |
|------|-----------|
| Same Map1 harness as control | **Yes** (ent **9** for zone-11 co-proof; ent 0/8 enough for RNG-only) |
| `captures/render_diag/map1_actor20_var0408_branch_trace_20260708.gdb` | **Yes** (or equivalent breakpoints) |
| Break on `FieldScriptMemoryWriteU16` when `index == 0x0408` | **Yes** |
| Optional: conds on actor 20 compares; zone 11 inside; frame inject | For full route proof |
| `XENO_FIELD_DIAG` | **No** — confuses gdb stdout |
| Always-on FieldLoad logs | Irrelevant (gdb may not show them) |

**What the probe proves:**

- Actor **20** writes `0x0408` via opcode `0xA8` (`FieldScriptVMHandlerMulVariableWithRand`).
- Handler does `(rand() * (arg+1)) >> 15` with arg max **4** → retail expects result **0..4**.
- **Before fix (current):** values like `36875`, `47494`, `62140` then fallback write `1` at IP `0x0997`.
- Binary imports host `rand@GLIBC_2.2.5` (`nm -D`); retail C is `src/slus_006.64/psyq/libc.c` (`return (seed>>16)&0x7FFF`).

---

## 3. Stale / noisy / dangerous

### 3.1 Keep as-is (required scaffolding, not “cleanup targets” yet)

| Item | Why keep |
|------|----------|
| Harness env + `[xeno-port][field]` prints | Repro identity |
| Stub oracle | Next missing function discovery |
| `XENO_FIELD_0BB_*` logs when upload on | Confirms VRAM drain |
| Gated `XENO_FIELD_DIAG` in misc2/rendering | Legitimate deep draw debug |
| Sound shim / heap pin **behavior** | Stability; not instrumentation removal |
| External gdb scripts | Only reliable actor-20 proof |

### 3.2 Noisy but low risk (P2 candidates to gate later — **not this PR**)

| Item | Why noisy | Risk if removed/gated wrong |
|------|-----------|------------------------------|
| Always-on `[field-diag]` in **misc3** (6 lines/load) | Tag lies; not opt-in | Low if moved under `XenoFieldDiagEnabled` |
| Always-on `func_8002C644` pin logs | 4 skips every Map1 load | Low; keep skip messages if pin guard stays |
| Always-on `func_80076AC0 first` | One line | Low |
| Always-on `[FieldMain] FM_LOG` | 6–7 lines/entry | Low; optional gate later |
| Soft `[stub]` on Map1 | 5+ lines | **Do not silence** without replacing oracle |
| Psy-X HAL chatter | Video/GL spam | Vendor; ignore |

**Evidence of noise on quiet control smoke**  
(`combined_slotfix_visible_control_ent0_20260708_104518.log`, no `XENO_FIELD_DIAG`):

- 11× `[field-diag]` (all ungated sites)
- 6× `[FieldMain]`
- 9× `[xeno-port]`
- 5× `[stub]`
- 2× `[field-0bb]`

So “default quiet” is **not fully quiet**. Older note that default has zero `[field-diag]` is **stale** relative to current misc3/temp2/VM always-on prints (and Map1 pin path).

### 3.3 Stale / misleading

| Item | Issue |
|------|-------|
| Name `[field-diag]` + claim “opt-in only” | Only half the sites respect `XENO_FIELD_DIAG` |
| `field_diag_default_quiet_20260705_*.log` | Kernel0/Map0 era; not Map1 control baseline |
| Raw-byte “fade at 0x07a8” notes | Corrected in handoff; ignore for smoke |
| Temporary gdb that force position/height | Do not use as “known-good control” |

### 3.4 Dangerous if misused

| Item | Danger |
|------|--------|
| gdb `set D_800AFE9C = 0x1000/0x2000` each frame | Synthetic input; fine for route proof, **not** real-input control proof |
| `XENO_FIELD_NO_MODEL_BUILD=1` | Changes draw/load path; can mask model bugs |
| Enabling `XENO_FIELD_DIAG` during timeout smoke | Log volume + I/O; can slow frame pacing |
| Deleting stub logging globally | Loses oracle; regressions look like “nothing happened” |
| Editing camera/actor/VM while “cleaning logs” | Blast radius; **forbidden** in P2 |

### 3.5 Recommended P2 code actions (future; **not done here**)

Priority order when code cleanup is allowed:

1. **Gate** always-on `[field-diag]` in `misc3.c`, `temp2.c` pin logs (optional keep first N skips always), and `virtual_machine.c` first-attach behind shared `XenoFieldDiagEnabled()` (or one header helper).
2. Optionally gate `FM_LOG` the same way or behind `XENO_FIELD_MAIN_LOG`.
3. Do **not** remove pin-guard **behavior** when silencing prints.
4. Do **not** mix this with the rand PR.

---

## 4. Instrumentation map vs gameplay-risk systems

| System | In-binary probes that touch it? | Smoke dependency |
|--------|----------------------------------|------------------|
| Camera | None required; diag may print OT/frame | Control visual only |
| Actor layout | `func_80076AC0 first` one-shot; not needed | None for pass |
| Input | Harness + real SDL; gdb may inject mask | Control: real keys; actor-20 route: inject OK |
| Walkmesh | None | Control movement visual |
| Script VM | No default printf of opcodes | Actor-20: **gdb only** |
| Renderer | Gated `func_8001E3D8` / submit | Off for smokes |
| Field globals | FieldLoad always-on lines | Noise only |

---

## 5. Host-rand before / after smoke protocol

Use this as the **entire** validation surface for the future PR  
`Match PSX rand range for field script RNG`.

### 5.1 Scope reminder (for that PR only)

- Normalize `rand`/`srand` so field script RNG sees **PSX-style `0..32767`**.
- Call sites that matter: `FieldScriptVMHandlerMulVariableWithRand`, `FieldScriptVMHandlerRandVariable` (`variable_handlers.c`); also other `rand()` in field code inherit the fix.
- Retail reference: `src/slus_006.64/psyq/libc.c` + `include/psyq/rand.h` (`RAND_MAX 32767`).
- Current host: `nm -D …/xeno-port` → `U rand@GLIBC_2.2.5`.
- **Out of scope:** camera, VM dispatch, movement, renderer, actor structs, handoff stash pop.

### 5.2 BEFORE baseline (already captured)

| | |
|--|--|
| Log | `captures/render_diag/map1_actor20_var0408_branch_trace_20260708_151951.log` |
| Script | `captures/render_diag/map1_actor20_var0408_branch_trace_20260708.gdb` |
| HEAD context | Docs `8db4fb2` era; code without PSX rand fix |

**Signature lines (host-rand broken):**

```text
WRITE_0408 #1 frame=1 actor=20 ip=0x88f value=36875 old=0
A20_COND_0408 … var0408=36875 … imm=0..4 … (all miss)
WRITE_0408 #2 frame=1 actor=20 ip=0x997 value=1 old=36875
```

Repeated later frames with values like `47494`, `62140`, `48347`, `28977` — all **outside 0..4**.

### 5.3 AFTER command (run after rand PR; do not run as part of this docs pass)

```bash
# From host project tree with rebuilt binary
distrobox enter xenogears-dev -- bash -lc '
  cd /home/blizz/Projects/xenogears-decomp && \
  nm -D ./pc_port/build_native/xeno-port | grep -E "rand|srand" && \
  timeout -s KILL 90 gdb -batch -x captures/render_diag/map1_actor20_var0408_branch_trace_20260708.gdb --args \
    env SDL_VIDEODRIVER=x11 \
        XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 \
        XENO_FIELD_MAP=1 XENO_FIELD_ENTRANCE=9 \
        XENO_FIELD_0BB_VRAM_UPLOAD=1 \
        ./pc_port/build_native/xeno-port \
  2>&1 | tee captures/render_diag/map1_actor20_var0408_AFTER_randfix_$(date +%Y%m%d_%H%M%S).log'
```

(Adjust gdb `--args` / `env` wrapping to match how your gdb build invokes the binary; keep the **same** breakpoints as the baseline `.gdb`.)

### 5.4 AFTER pass / fail

| Check | Pass | Fail |
|-------|------|------|
| `WRITE_0408` at ip `0x88f` (0xA8 path) | `value` ∈ **0..4** | value ≫ 4 (e.g. 30k+) |
| Fallback `0x0997` assign `1` | Only after legitimate state-0..4 paths complete, or less often | Every cycle after out-of-range random |
| Zone 11 on ent9 route | Still fires (`ZONE_INSIDE` / misc11:992) if route held | New crash/assert |
| Control smoke ent0 & ent8 | `RUN_RC=124`, Fei visible/movable | Regression |
| Diffstat of rand PR | libc/port rand only | Any camera/VM/movement/renderer file |

**Optional stronger check:** after fix, at least one run should show a `0x088f` write of `0`, `2`, `3`, or `4` (not only `1`), proving the branch table is reachable. Non-deterministic; seed control may help later but is not required for the first PR.

### 5.5 Control smokes after rand PR (must stay green)

```bash
# Entrance 0
timeout -s KILL 20 env SDL_VIDEODRIVER=x11 \
  XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 \
  XENO_FIELD_MAP=1 XENO_FIELD_ENTRANCE=0 XENO_FIELD_0BB_VRAM_UPLOAD=1 \
  ./pc_port/build_native/xeno-port; echo RUN_RC=$?

# Entrance 8
timeout -s KILL 20 env SDL_VIDEODRIVER=x11 \
  XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 \
  XENO_FIELD_MAP=1 XENO_FIELD_ENTRANCE=8 XENO_FIELD_0BB_VRAM_UPLOAD=1 \
  ./pc_port/build_native/xeno-port; echo RUN_RC=$?
```

Expect `RUN_RC=124`. Do not enable `XENO_FIELD_DIAG`.

---

## 6. What not to clean in P2 code follow-ups

- Camera / actor layout / script VM / walkmesh / renderer architecture  
- Stub oracle wholesale deletion  
- Pin-guard and sound-shim **logic** (only their print noise, carefully)  
- Popping handoff stash mid-audit history  
- Bundling rand fix with diag gating (separate PRs)

---

## 7. P2 completion checklist

- [x] Inventory always-on vs opt-in vs oracle vs external  
- [x] Identify Map1 control required surface  
- [x] Identify actor-20 / host-rand required surface  
- [x] Mark stale, noisy, dangerous probes  
- [x] Write before/after rand smoke protocol with baseline log path  
- [x] **Code:** gate always-on `[field-diag]` in `misc3.c` / `temp2.c` / `virtual_machine.c`  
  via per-file `XenoFieldDiagEnabled()` (same `XENO_FIELD_DIAG` contract as misc2/rendering).  
  Verified 2026-07-08: Map1 ent0 default → `field-diag=0`, `RUN_RC=124`, stubs still fire;  
  `XENO_FIELD_DIAG=1` restores FieldLoad/pin/`func_80076AC0 first` lines.  
  Did **not** silence `[xeno-port]`, `[FieldMain]`, stubs, or `[field-0bb]`.  
- [ ] (Optional later) gate `FM_LOG`  
- [x] **Code:** PSX-range `rand`/`srand` via `pc_port/src/psx_rand.c` (retail LCG;  
  no glibc import). Verified: actor-20 `0x088f` writes `0` (∈0..4), no host-scale  
  values; Map1 ent0/ent8 `RUN_RC=124`, default `field-diag=0`; zone 11 still fires.

---

## 8. Cross-links

- Cleanup + provenance: `docs/ai_context/XENOGEARS_CLEANUP_AND_PROVENANCE_AUDIT.md`  
- Deep journal: `docs/ai_context/ACTIVE_HANDOFF.md` (do not refresh from stash until deliberate)  
- Noah / reference policy: `docs/ai_context/REFERENCE_SOURCES.md`  
- Retail RNG: `src/slus_006.64/psyq/libc.c`, `include/psyq/rand.h`  
- Script RNG handlers: `src/field/scripts/variable_handlers.c`

---

*End of P2 instrumentation audit. Docs only.*
