# PROJECT GOAL AND PLAN — read this before planning any work

_Set 2026-09-17. This file defines **what this project is for** and therefore
which work is worth doing. `ACTIVE_HANDOFF.md` records where the work currently
stands; this file records what the work is **aiming at**. If the two ever
disagree about priorities, this file wins._

---

## The goal

**A native PC port of Xenogears in the mould of Ship of Harkinian (Zelda: OoT)
and the Silent Hill decomp/port projects — running natively, moddable, and
ultimately re-rendered with new graphics (HD-2D is the stated ambition).**

The goal is **not** a byte-matching decompilation for its own sake. Byte parity
is a *verification tool* used where it pays for itself (see "When to byte-match"
below). It is not the deliverable and it is not the critical path.

## Why that distinction decides the work order

An `INCLUDE_ASM`'d function makes the rebuilt ROM byte-perfect and is worth
**exactly zero** to the port: you cannot execute MIPS assembly on x86. This
repo demonstrates both halves of that at once:

- `movie.bin` and `battle.bin` rebuild **byte-identical to retail**, yet movie
  is **0%** decompiled (0/34 functions in C) and battle is **0%** in the port.
- The port's working code includes many bodies that are *not* byte-exact.

Byte-perfect ROM != portable code. **C coverage of executed code paths is the
critical path; byte parity is a quality gate layered on top.**

## Architecture: where you stand against Ship of Harkinian

| Ship of Harkinian | This project | State (2026-09-21) |
|---|---|---|
| `libultraship` — SDL/GL/AL platform layer | **PsyCross** (`pc_port/extern/PsyCross` + ~30 patches in `pc_port/patches/`) | ~65% |
| **Fast3D** — translates N64 display lists to OpenGL | PSX GPU primitive path (`pc_port/src/psyq_compat.c`, PsyCross prim patches) | partial |
| OTR assets — enables HD texture packs | `pc_port/src/archive_port.c`, disc archive layer | ~55% |
| zeldaret/oot decomp — **100% C** | `src/**` — 20.44% byte-exact C; **720 of 906 `INCLUDE_ASM` sites have no C body anywhere** | ← the gap |
| *(nothing — never needed one)* | **`pc_port/src/battle_mips_runtime.c` — a 1,159-line MIPS interpreter executing `disc/battle.bin`** | ← the blocker |

**That last row is the whole strategic picture.** SoH could exist because the
OoT decomp was complete; they never needed an N64 CPU interpreter. This port
needs a MIPS interpreter *because* the decomp is incomplete. It is a legitimate
bridge — it makes the game run today — but:

> **Code executing through the MIPS interpreter is a black box. You cannot
> re-render, re-texture or restyle geometry produced by code you do not have in
> C. Every system that must look different in HD-2D has to be real C first.**

Battle is 0% C, so today it runs *only* as interpreted MIPS. `world_map` was
never even disassembled. Those are the two systems furthest from the goal.

---

## Plan

### Phase A — eliminate the MIPS interpreter (the real unlock)

Give every executed function a C body. **Correct, not necessarily byte-exact**,
verified with the existing differential harness (`pc_port/tests/`, which
already runs tens of thousands of checks at O0/O2/UBSan). Ordered by what the
port needs to restyle:

> **Read "O0/O2/UBSan" in older evidence documents as O0/O2.** Until
> 2026-09-21 the UBSan regime had never executed on this machine: it fails on
> the bare Fedora host (no `libubsan` RPM) and only works inside the podman
> toolchain container, and the link error arrived *after* O0 and O2 had printed
> PASS, so it was repeatedly summarised as green. `sanitizer_preflight.sh` now
> makes a regime that cannot run say so. 34 suites ran a real UBSan regime for
> the first time on 2026-09-21 and found zero undefined behaviour — but a
> separate audit flags **349 of 566 suites** as structurally able to print PASS
> while abandoning later regimes or their negative controls. This bar is the
> thing byte parity is traded away for, so treat it as partly unproven until
> that audit is worked through. See `docs/ai_context/HARNESS_REGIMES.md`.

1. **`world_map` (180,422 B).** Never split — as of 2026-09-17 there is no
   `config/world_map.yaml`, no `asm/world_map/`, no `src/world_map/`, while
   `pc_port/src/` carries 408 hand-written `world_map` references. The port has
   been hand-reverse-engineering a module that was never decompiled. This is
   also the "World Rendering 5% / E190 / `0x8009C620` render-context" blocker.
2. **battle (857 functions, 0% C)** plus the pad/vblank deadlock. Everything
   visual in combat sits behind the interpreter.
3. **menu (7.36%)** and **field (34.71%)**. UI is the cheapest, most visible
   HD win.

Gate for this phase: adopted-leaf count and the MIPS interpreter's call count.
Measured 2026-09-21: **108 of 857 battle functions on real host C leaves**
(was 96), **76 function stubs, 489 data placeholders**.

**Do not use the function-stub count as the scoreboard.** It is confounded:
adopting a leaf pulls its TU into the link, which can make previously-unseen
functions newly referenced, so the count can rise while real progress is made.
It did exactly that on 2026-09-21 (76 → 77 while four leaves were adopted).
Adopted-leaf count and interpreter call count are the honest measures.

### Phase B — renderer abstraction (where HD-2D actually lives)

- **Turn on extended/PGXP-style primitives.** `pc_port/build_port.sh` currently
  sets `USE_EXTENDED_PRIM_POINTERS=0` — "PsyCross's simple (non-PGXP)
  primitive". PGXP-style recovered geometry precision (instead of PSX integer
  snapping) is this project's equivalent of moving to Fast3D. Cheap, high
  visual impact, and a prerequisite for anything HD.
- **Plan for the depth problem early.** HD-2D post-processing (depth of field,
  tilt-shift, bloom) needs a depth buffer. **The PSX has none** — it sorts with
  the Ordering Table, which gives draw *order*, not depth. Depth must be
  synthesised from the OT index or recovered from GTE transform output. This is
  the largest technical unknown in the HD-2D goal; budget for it rather than
  discovering it late. Prior OT work exists (see the Map 2 OT notes).

### Phase C — asset replacement, then art

Higher-resolution sprites/backgrounds through the archive layer, i.e. the OTR
equivalent. Only meaningful once A and B land.

### Phase 0 — hygiene that unblocks everything (do opportunistically)

- ~~**Commit the tree.**~~ — **done.** `src/battle/` is tracked (174 files as
  of 2026-09-21). The old warning that all 173 were untracked no longer holds.
- ~~**Complete `config/checksum.sha`**~~ — **already complete.** Verified
  2026-09-21: it carries all ten modules and every recorded hash equals the
  sha256 of the corresponding retail file in `disc/`. `menu.bin` *is* gated.
  The old claim here that it "gates 5 of 10" was wrong.
- ~~**Fix the slus link**~~ — **fixed 2026-09-21.** The undefined symbols were
  not separate globals but six interior fields of one `InterruptEnvironment_t`
  at `0x800578A4`; a second, pre-existing GPREL16 truncation sat underneath.
  `make check` now runs end to end for the first time in this project.

---

## When to byte-match

**Do** byte-match the low-level engine — GTE, GPU, memory, archive, controller.
A subtly-wrong body there produces precisely the bugs that have cost this
project the most time: objects scaled 7.0x from reading a table at the wrong
address, an all-zero colour matrix rendering Blackmoon Forest black, and
`func_8007234C` called with zero arguments that "only worked by accident of
register liveness". Byte parity catches that entire class at compile time.

**Do not** hold up gameplay or UI logic for byte parity. A differential test is
cheaper and nearly as strong. Measured cost on 2026-09-17: five byte-exact
functions in one session, where understanding the semantics took minutes and
matching the register allocation took hours — roughly 90% of the effort went
into byte parity. At that rate, matching everything is a multi-year path;
C coverage is far faster because "correct" is much cheaper than "identical".

**Always** keep the per-overlay byte gate. It is free and catches drift:

```
ninja build/out/<overlay>.bin && cmp disc/<overlay>.bin build/out/<overlay>.bin
```

**Seven** of ten modules are byte-identical (`make check`, 2026-09-21):
`battle`, `member_change_menu`, `shop_menu`, `movie`, `battling`, `world_map`,
`battle_command_file1`. The three that fail are `slus_006.64` (layout;
declined, see below), `field.bin` (codegen, no correctness gap behind it) and
`menu.bin` (INCLUDE_ASM hoisting — accepted, see
`MATCHING_LAYOUT_CONTRACT.md`).

An earlier version of this file said four of ten. That was wrong, and the
reason it was wrong matters more than the number: `gears` used to discard
splat's exit code, so a dead split left ninja relinking a stale tree and the
resulting byte counts were written down as measurements. Fixed 2026-09-21;
`gears` now fails loudly. Three 2026-09-21 worker-dispatches were spent
discovering that three "repair" tasks had nothing to repair.

## Measuring progress

Use the objdiff report; **do not** hand-maintain numbers, and do not trust
"100% done" claims.

```
make report                                   # -> build/progress.json
python3 tools/scripts/progress_dashboard.py   # -> docs/evidence/decomp-campaign/DASHBOARD.md
```

Headline on 2026-09-21: **20.44% of code matched**, 42.59% of functions.
That is *lower* than the 23.27% / 46.69% recorded on 2026-09-17 because the
denominator grew — `world_map` (180,422 B) is now inside the build and counted
— not because anything regressed. There is no longer a coverage hole.

**Known metric trap:** `tools/scripts/decomp_status.py` reports `unported = 0`
and "100% done" because it classifies any `INCLUDE_ASM` inside
`#ifndef XENO_PC_PORT` as COEXISTENCE without checking that a C body exists on
the other branch. 720 of 906 such sites have no C body anywhere (measured 2026-09-21). Treat that
script as the port-stub view only.
