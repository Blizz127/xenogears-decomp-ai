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
5. Adversarial asm-vs-C refutation by independent agents **before** commit on substantial asm ports — most July 9 passes were adversarially verified against retail asm by independent refutation agents before commit (0xFC: 3 agents; 0xE0 chain: 5; 0xF5/0xF6: 2; 0xBC/0x24: 1; POLYCHECK: 1 — none refuted). Documented catches: the `0x8FE7` → `0xC3E7` diagnostic mask typo and a degenerate-loop convention fix; the scratchpad-pointer deref was caught by a live SIGSEGV during the POLYCHECK build, not by review. The 0xA3, 0x94, and 0xBC/0x25 passes were verified by live probe + smokes only.
6. Update `ACTIVE_HANDOFF.md`

## Decomp vs PC port

| Aspect | Matching decomp | PC port |
|--------|-----------------|---------|
| Goal | Byte/match verification | Runnable game logic on x86 |
| ASM | Must match or be in `INCLUDE_ASM` | `SKIP_ASM` — needs real C |
| Coverage | Incremental per function | Boot-path oracle drives priority |
| Stubs | Not used in matching build | Auto-generated for undefined symbols |

## PC-port memory and stub rules (July 9, 2026)

Distilled from the Map1→Map15 reload campaign (see [Phase Log](Phase-Log); depth in the July 9 `ACTIVE_HANDOFF.md` entries).

### 7. PSX-embedded struct fields stay u32

- Any struct the game embeds in PSX heap objects at fixed offsets must keep **u32 pointer fields** — never native pointers. The port links `-no-pie`, so all code/data sits below 4 GiB and u32 round-trips safely.
- Cautionary example (`ce61f3d`): `work_list_port.c`'s `WorkListEntry` used native 64-bit pointers (fields at 0/8/16/24/40) while the game embeds the PSX 0x1C-byte layout (fields at 0/4/8/0xC/0x18). The bug stayed latent because the port's work lists had always been **empty** — the first real entries (opcode `0xE0` child sprites) made `TimerWorkListUpdate` call a garbage callback pointer. The already-landed `func_8001CE74` (opcode `0x96`) had the same latent bug and was fixed with it.

### 8. Scratchpad pointers are unmapped

- `func_8007CD3C` returns PSX scratchpad pointers (`0x1F800000`-based), which are unmapped on the port — dereferencing one is a live SIGSEGV (found during the POLYCHECK migration, `3858bd8`).
- Convention: keep the arena push/pop calls balanced (`func_8007CD3C` / `func_8007CD60`) so arena state stays retail-exact, but back the actual workspace with a C local.

### 9. Frozen `stubs.c`: new symbols need explicit definitions

- When the matching ELFs are absent, generated `stubs.c` is frozen and will not grow entries for newly referenced symbols. Each one needs an explicit definition.
- Functions: implement them, or add hand-written log-once stubs (e.g. field-overlay `func_800BA8F4` / `func_800BC158`, which have no source anywhere in the repo).
- Data: real values, not placeholders — e.g. `D_8006FC48[] = "POLYCHECK %d\n"` in `pc_port/src/data_field.c`.

### 10. `$sp`-switch trampolines: elide the switch, keep the alloc/free pairs

- Retail stack-switch trampolines (e.g. the opcode-`0xFC` VRAM-upload chain's double `$sp` switch, `ce61f3d`) cannot be expressed in C.
- Elide the switch itself, but keep every `HeapAlloc`/`HeapFree` pair so heap state stays retail-exact.

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
- Unmigrated branches and sub-commands assert **individually and loudly** — never silent no-ops. Each surfaces with its own id, so the live path names the next bounded pass by itself (e.g. opcode `0xBC`'s unported sub-commands assert per sub-command; `0xF7` and the misc8 select-target/on-top machinery assert by design).

## GTE / matrix audit note

Handwritten GTE sequences may hide `cv=0` vs `cv=3` mis-transcriptions. When projection looks wrong, check asm GTE `mvmva` control bits before experimenting with matrix hacks.

## When to add a PC-port shim

Acceptable when:

- Classified as side-system (audio, encounter noise) **and**
- Generated stub already returns without crashing **and**
- Deep dependency chain is unbounded **and**
- Documented boundary is narrow (single wrapper function)

Not acceptable as a substitute for game-logic transition steps.
