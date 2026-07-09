# Matching and Porting Rules

Project rules distilled from handoff guardrails, commit messages, and [`REFERENCE_SOURCES.md`](https://github.com/Blizz127/xenogears-decomp-ai/blob/main/docs/ai_context/REFERENCE_SOURCES.md).

## Core principles

### 1. No fake behavior

- Do not implement stubs that pretend to be correct game logic.
- Do not clamp coordinates, skip primitives, inject dummy packets, or force actors visible.
- Do not implement overlay entry addresses (e.g. `func_801C62A8`) as standalone C functions.
- Menu route stubs are harness problems, not "missing functions to invent."

### 2. Retail evidence first

Priority order for decisions:

1. SLUS-006.64 matching assembly (`asm/field/matchings/...`)
2. Retail runtime behavior (gdb traces on PC port)
3. Matched decomp in this repo
4. External references (Noah) — **research only**, never authoritative alone

### 3. Bounded implementations

- Port the **smallest** function or branch that unblocks the proven live path.
- Per-opcode anim-script fixes, not whole VM rewrites.
- Per-function sound shims, not entire `func_8003B644` chains for field progression.
- One diagnostic target per pass; stop at the next real gate.

### 4. No broad rewrites

- Do not replace assertion failsafes with blind skips (`func_800764B4` was implemented properly, not skipped).
- Do not patch actor scripts or force script variables (`0x0408`, `0x0462`) to "unblock" transitions.
- Do not merge unrelated subsystems (rendering + sound + encounter) in one change.

### 5. Keep hacks labeled

Temporary port mechanisms must be explicit:

| Mechanism | Label |
|-----------|-------|
| `XENO_KERNEL_SEL` / `XENO_FIELD_*` env vars | Test harness |
| `func_800855C8` PC-port no-op | Documented audio shim (`ae8c753`) |
| `XENO_FIELD_0BB_VRAM_UPLOAD` | Opt-in VRAM upload; not retail loader |
| PsyCross patches in `build_port.sh` | Port compatibility; cite reason |
| Generated `stubs.c` entries | Oracle / missing decomp |

### 6. Commit small verified steps

Workflow observed in recent milestones:

1. Read-only gdb/diagnostic proof
2. Docs-only checkpoint commit (when finding is non-obvious)
3. Smallest code fix with asm backing
4. Guard runs: Map0 baseline + Map1 target + default boot
5. Update `ACTIVE_HANDOFF.md`

## Decomp vs PC port

| Aspect | Matching decomp | PC port |
|--------|-----------------|---------|
| Goal | Byte/match verification | Runnable game logic on x86 |
| ASM | Must match or be in `INCLUDE_ASM` | `SKIP_ASM` — needs real C |
| Coverage | Incremental per function | Boot-path oracle drives priority |
| Stubs | Not used in matching build | Auto-generated for undefined symbols |

## Reference sources (Noah)

Approved uses:

- Behavior comparison hints
- Naming hints for `func_8XXXXXXX` / `D_8XXXXXXX`
- VM opcode research
- Data-format clues

Rules:

- Confirm against SLUS asm or runtime before landing code
- No direct code import without deliberate review
- Cite as "Noah reference" alongside asm/runtime evidence

## Protected baselines

Before and after any Map1 experiment, verify **Map0 guard**:

- Actors `1, 2, 16, 23, 25, 26` visible; actor 18 hidden
- `RUN_RC=124` timeout, no new asserts
- Do not re-stub `func_8009AD6C` to force actor 18 visible

## Assert policy

- Deliberate asserts (`"not implemented"`, `"not migrated"`) mark **real missing behavior**.
- Replace with faithful implementation from asm, not no-ops.
- If a rare branch assert is not hit on the current route, it is **not** the current blocker.

## GTE / matrix audit note

Handwritten GTE sequences may hide `cv=0` vs `cv=3` mis-transcriptions. When projection looks wrong, check asm GTE `mvmva` control bits before experimenting with matrix hacks.

## When to add a PC-port shim

Acceptable when:

- Classified as side-system (audio, encounter noise) **and**
- Generated stub already returns without crashing **and**
- Deep dependency chain is unbounded **and**
- Documented boundary is narrow (single wrapper function)

Not acceptable as a substitute for game-logic transition steps.
