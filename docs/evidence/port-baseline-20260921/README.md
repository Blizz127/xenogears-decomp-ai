# Port baseline — 2026-09-21

Measured from worktree `wt-port-baseline`, branch `Blizz127/wt-port-baseline`,
starting at `ed221dc0` (2026-09-20 22:09:10 -0500). Every number below is
produced by a command shown next to it, run inside the mandated podman
toolchain container. Where a number could not be produced, it is marked
`NOT_RUN` with the reason. Nothing here is copied from a doc and presented as
current — see "Stale doc claims found" for the numbers that turned out to be
wrong.

This run received three corrections from the coordinator mid-task; they are
folded into the sections below and credited where they apply. In particular:
**the config/symbol_addrs.slus_006.64.txt splat-parser blocker described below
was found by the battle_command_file1 worker of this wave (terminal
`term_c96ecb3c`), not by this worker** — the fix (commit `66d87a6c` on the
main checkout) was cherry-picked into this worktree as commit `326530d8` on
the coordinator's explicit instruction, so the rest of this baseline could
actually be measured rather than reported as a wall of `NOT_RUN`.

---

## 0. The blocker that made every other number wrong until today

At `ed221dc0` (this worktree's starting commit), **no overlay could be split
or built at all**:

```
$ podman run ... bash -lc 'source /.venv/bin/activate && make build'
...
error reading config/symbol_addrs.slus_006.64.txt, line 412:
	firstfile = 0x80040584; // type:func (retail .text function; the port generator must not emit a data stub)
Line must be of the form
<function_name> = <address>; // attr0:val0 attr1:val1 [...]
with <address> in hex preceded by 0x, or dec
...
AssertionError: Line must contain a single semi-colon
grep: linker/undefined_funcs_auto.field.txt: No such file or directory
sed: can't read linker/undefined_funcs_auto.field.txt: No such file or directory
make: *** [Makefile:71: build] Error 2
```

Root cause: commit `cb2a215a` (2026-09-20) added an inline comment containing
a semicolon (`// type:func (... a data stub;)`) to two lines
(`firstfile`/`nextfile`, lines 412–413) of
`config/symbol_addrs.slus_006.64.txt`. splat 0.33.2's symbol-file parser
asserts exactly one `;` per physical line, comment included, and dies. This
one file is shared by every overlay's `gears matching` pass, so the failure
was not scoped to `slus_006.64` — **it killed `asm/`, `linker/`, and every
`build/out/*.bin` for the whole repo.**

Worse: `make`'s recipe continued past the failed `gears matching` invocation
(note the `grep:`/`sed:` lines *after* the Python traceback, still trying to
patch a `linker/` file that was never generated) before finally failing on an
unrelated `sed`. **The coordinator identified the underlying mechanism:
`gears` swallows splat's nonzero exit, so a stale `build/`/`linker/` tree gets
relinked and produces plausible-looking but garbage byte counts.** That
matches this run's own logs exactly, and it is very likely the reason
`ACTIVE_HANDOFF.md` has been carrying stale/contradictory byte-diff figures
for `battling`, `world_map`, and `battle_command_file1` (see §"Stale doc
claims" below) — **this is a measurement-integrity defect in the build
tooling, not a decompilation gap**, and it is the reason two of the three
sibling repair tasks in this wave were chasing numbers that had already been
fixed.

Fix: `66d87a6c` (main checkout), replacing the in-comment `;` with `--`.
Cherry-picked into this worktree as `326530d8`. **This worktree's HEAD is now
`ed221dc0` + `326530d8`; everything below is measured after that fix.**

---

## 1. Per-overlay byte gate (all 10 `gears.toml` modules)

Commands, run per overlay after the fix above:

```
ninja build/out/<overlay>.bin
stat -c%s build/out/<overlay>.bin ; stat -c%s disc/<overlay>.bin
cmp -l disc/<overlay>.bin build/out/<overlay>.bin | wc -l
```

| Overlay | build/out size | disc size | differing bytes | Verdict |
|---|---:|---:|---:|---|
| `member_change_menu` | 26,624 | 26,624 | 0 | **MATCH** |
| `shop_menu` | 55,296 | 55,296 | 0 | **MATCH** |
| `battle` | 343,936 | 343,936 | 0 | **MATCH** |
| `movie` | 29,779 | 29,779 | 0 | **MATCH** |
| `battling` | 142,953 | 142,953 | 0 | **MATCH** — a sibling worker (`term_c96ecb3c`'s cohort) is actively changing `config/battling.yaml`; measured here as a BASELINE, expected to move |
| `world_map` | 180,422 | 180,422 | 0 | **MATCH** — a sibling worker (`term_872d8fdf`) is actively changing `config/symbol_addrs.world_map.txt`; measured here as a BASELINE, expected to move |
| `battle_command_file1` | 19,516 | 19,516 | 0 | **MATCH** — a sibling worker (`term_c96ecb3c`) is actively changing `config/battle_command_file1.yaml`; measured here as a BASELINE, expected to move |
| `field` | 249,442 | 260,862 | 219,198 (size differs; `cmp` diverges at byte 157) | **FAIL** |
| `menu` | 153,472 | 153,864 | 133,762 (size differs; `cmp` diverges at byte 73) | **FAIL** |
| `slus_006.64` | *(no file produced)* | 303,104 | — | **NOT_RUN** — see below |

Cross-checked against the gate file directly:

```
$ sha256sum -c config/checksum.sha
build/out/member_change_menu.bin: OK
build/out/shop_menu.bin: OK
build/out/battle.bin: OK
build/out/menu.bin: FAILED
build/out/movie.bin: OK
build/out/battling.bin: OK
build/out/world_map.bin: OK
build/out/battle_command_file1.bin: OK
sha256sum: build/out/slus_006.64: No such file or directory
build/out/slus_006.64: FAILED open or read
sha256sum: WARNING: 1 listed file could not be read
sha256sum: WARNING: 2 computed checksums did NOT match
```

**Result: 7 of 10 match retail byte-for-byte** (`member_change_menu`,
`shop_menu`, `battle`, `movie`, `battling`, `world_map`,
`battle_command_file1`); 2 fail (`field`, `menu`); 1 could not be built
(`slus_006.64`).

### Stale doc claims found here

- `docs/ai_context/PORT_GOAL_AND_PLAN.md:131` claims **"Four of ten modules
  are byte-identical today: `battle`, `member_change_menu`, `shop_menu`,
  `movie`."** That is stale — measured today, **seven** match, not four.
  `battling`, `world_map`, and `battle_command_file1` all match now too (see
  below); this doc predates all three fixes.
- `ACTIVE_HANDOFF.md` carries an "88% differ" figure for `battling` and a
  "2,743-byte" figure for `world_map`. Both are refuted by this run's own
  `cmp -l` (0 differing bytes for both) and by the coordinator's independent
  verification (sha1 `5fee1deab3558c23bbe9c3a2eda3a2021a570911` for
  `world_map.bin`). The mechanism is almost certainly the "`gears` swallows
  splat's exit code" defect in §0: those stale numbers were measured against
  a silently-stale relink, not a real build.
- `battle_command_file1`'s "it regressed" claim was independently bisected
  and refuted this run: the `config` diff between `0b4876e2` and HEAD is
  empty. No layout fix was needed for it.

### `slus_006.64` — NOT_RUN, and it is the pre-existing "known-red" issue, now wider

```
$ ninja build/out/slus_006.64
...
FAILED: [code=1] build/out/slus_006.64.elf
mips-linux-gnu-ld: build/src/slus_006.64/psyq/libetc/intr.c.i:(.text+0x154): undefined reference to `D_800578A6'
mips-linux-gnu-ld: ...undefined reference to `D_800578E0'
mips-linux-gnu-ld: ...undefined reference to `D_800578D4'
mips-linux-gnu-ld: ...undefined reference to `D_800578A8'
```

This is the documented "slus link is red, pre-existing, not yours to fix"
issue (`ACTIVE_HANDOFF.md:229`) — `D_800578D6`/`D_800578D8` are two
auto-generated struct-offset labels inside `g_InterruptEnvironment`
(`config/symbol_addrs.slus_006.64.txt:837`, a sizeless 0x1068-byte struct)
that the `Makefile` patches into `linker/undefined_syms_auto.slus_006.64.txt`
by hand. **The struct now throws off four more such offsets**
(`D_800578A6`, `D_800578A8`, `D_800578D4`, `D_800578E0`) that the Makefile's
hard-coded patch list does not cover — the known issue has grown, not
shrunk. Not fixed here per task scope (measurement, not repair); recorded as
a finding.

Because `slus_006.64.elf` never links, `make check` (`clean && build &&
sha256sum --check`) cannot pass and was not attempted as a whole — consistent
with the task brief's statement that it is red at HEAD and not this worker's
to fix.

---

## 2. `config/checksum.sha` — already complete, verified, not edited

**Correction to the task brief, confirmed independently by both this worker
and the coordinator: the file already gates all 10 modules, not 5.** The
brief's claim (mirroring `docs/ai_context/PORT_GOAL_AND_PLAN.md:102`, "gates 5
of 10 modules; menu.bin is built but never checked") is **stale** — commit
`6cd6cb75` ("chore: gate all 10 overlay hashes in checksum.sha", 2026-09-17)
is already an ancestor of this worktree's HEAD and added the missing five
pins, including `menu.bin`.

Verified every pin against the retail file it claims to gate:

```
$ sha256sum disc/<name>.bin   # for all 10, compared against config/checksum.sha
```

| build/out target | pin in checksum.sha | sha256(disc/*.bin) | Match |
|---|---|---|---|
| `slus_006.64` | `dc0b2dd7…` | `dc0b2dd7…` | ✓ |
| `field.bin` | `38a1ce82…` | `38a1ce82…` | ✓ |
| `member_change_menu.bin` | `3b9e2b89…` | `3b9e2b89…` | ✓ |
| `shop_menu.bin` | `7890e14b…` | `7890e14b…` | ✓ |
| `battle.bin` | `1830b4ef…` | `1830b4ef…` | ✓ |
| `menu.bin` | `82f84a24…` | `82f84a24…` | ✓ |
| `movie.bin` | `50e1a9d9…` | `50e1a9d9…` | ✓ |
| `battling.bin` | `3e6df915…` | `3e6df915…` | ✓ |
| `world_map.bin` | `4c15fd32…` | `4c15fd32…` | ✓ |
| `battle_command_file1.bin` | `64668d85…` | `64668d85…` | ✓ |

**All 10/10 pins are correct retail hashes. No edit was made to
`config/checksum.sha` in this run** — per the coordinator's explicit
correction, adding entries would have been wrong; the file was already
correct, and the task brief and the plan doc's §"Phase 0" item are both
stale on this point.

---

## 3. Objdiff progress report

```
$ make report                                  # -> build/progress.json
$ python3 tools/scripts/progress_dashboard.py  # -> docs/evidence/decomp-campaign/DASHBOARD.md
wrote docs/evidence/decomp-campaign/DASHBOARD.md  (20.44% code, 42.59% functions)
```

**Headline: 20.44% of code matched (282,764 / 1,383,684 bytes); 42.59% of
functions (1,901 / 4,464).**

This does **not** hold at the previously-recorded 23.27% / 46.69%
(2026-09-17) — but it is **not a regression**. The numerator is flat
(282,764 vs. 282,788 bytes, a −24 byte rounding difference); the denominator
grew by exactly 168,336 bytes, which is `world_map/120C`'s unmatched byte
count per this run's own burn-down table. `world_map` was "outside the build
and therefore unmeasured" as of the 2026-09-17 note in
`PORT_GOAL_AND_PLAN.md:145`; it is now inside the objdiff universe (thanks to
the sibling `world_map` worker's fixes) and contributes 0% matched against a
large denominator, which mechanically pulls the percentage down even though
no code got worse. **Report this as: percentage dropped because the
measurement got more honest, not because anything regressed.**

Per-overlay (from the same report), useful for corroborating §1's byte gate:

| Overlay | Code matched | Functions |
|---|---:|---:|
| Battle Overlay | 8.33% (27,672 / 332,316) | 292 / 857 |
| Field Overlay | 34.71% (87,824 / 253,052) | 559 / 884 |
| Main Executable | 43.09% (93,960 / 218,072) | 808 / 1,223 |
| Menu Overlay | 7.36% (10,912 / 148,360) | 71 / 307 |
| Battling Overlay | 0.00% (0 / 139,692) | 0 / 483 |
| Shop Menu Overlay | 70.67% (37,460 / 53,008) | 104 / 118 |
| Movie Overlay | 0.00% (0 / 26,972) | 0 / 34 |
| Member Change Menu Overlay | 100.00% (24,936 / 24,936) | 67 / 67 |
| Battle Command File1 | 0.00% (0 / 18,940) | 0 / 105 |

Note the direct confirmation of the plan doc's central point (§"Why that
distinction decides the work order"): `battling`, `movie`, and
`battle_command_file1` are **0% decompiled to C by this measure** yet all
three rebuild **byte-identical to retail** per §1 — 100% `INCLUDE_ASM`,
0% real code. Byte-perfect ROM really is orthogonal to C coverage, measured
directly in this run.

---

## 4. Phase A gate numbers

Per `docs/ai_context/PORT_GOAL_AND_PLAN.md:76`, the gate is "function-stub
count and MIPS interpreter call count both trending to zero." Do **not** use
`tools/scripts/decomp_status.py` for coverage — confirmed still broken as
described (see below) — and its "100% done" framing is not a real number in
this repo.

### 4a. Function-stub count — measured from a real build, not from a doc

`pc_port/src/stubs.c` **does not exist**. Stubs are generated at build time
by `tools/scripts/gen_port_stubs.py` into **`pc_port/build_native/stubs.c`**,
invoked from stage `[4/5]` of `pc_port/build_port.sh`. That script never ran
successfully in this worktree until the §0 fix was cherry-picked (it needs
`build/out/*.elf`/`.map` from a working matching build to classify
symbols). After the fix:

```
$ bash pc_port/build_port.sh
...
==> [4/5] Trial link to discover undefined references
    undefined symbols to stub: 566
  classified 9 leftover splat-style names by prefix (func_/D_/g_/jtbl_/jpt_)
Wrote pc_port/build_native/stubs.c: 76 function stubs, 489 data symbols (11 sized from symbol_addrs)
==> [5/5] Final link
    LINK OK -> pc_port/build_native/xeno-port (port-owned addresses verified)
```

Confirmed by direct count on the generated file:

```
$ grep -c '^long .*(void) {' pc_port/build_native/stubs.c   # 76
$ grep -c '^unsigned char' pc_port/build_native/stubs.c     # 489
```

**Current: 76 function stubs, 489 data placeholders.** The handoff's
last-recorded "78 function stubs, 577 data placeholders" is the closest
comparable prior figure; today's build is 2 stubs and 88 data placeholders
lower — moving in the right direction (toward zero), consistent with the gate
framing, though not confirmed to be a monotonic trend since the prior number
was recorded under conditions that may also have been affected by the §0
build-integrity defect.

### 4b. Distinct functions still executed as interpreted MIPS

**No build-verified dynamic call count exists** — that requires running the
port binary with tracing, which is out of scope for a measurement pass
(and the project's own memory notes flag the existing MIPS trace instrumentation
as unreliable/capped without extra env flags). Two different **static**
proxies were produced instead, and they disagree with each other in an
instructive way — both are reported rather than picking the flattering one:

1. **From the objdiff universe (§3):** the `battle` overlay has 857 total
   retail functions, 292 already byte-exact C in the *matching* build. That
   leaves at most 565 functions whose retail code is not yet plain C.
2. **From the port build's own accounting (`bash pc_port/build_port.sh`
   stage `[2c/5]`):**
   ```
   ==> [2c/5] Compiling adopted battle overlay host bodies
   # 96 adopted leaves across 61 translation units
   ...
   61 battle host units define the full allowlist
   ...
   # 96 adopted leaves reach no generated stub (76 stubs checked)
   ```
   **96 battle functions have a real, linked, non-stubbed host C body** that
   the port actually uses in place of the retail machine code — a
   substantially better number than the plan doc's blanket "battle is 0% C"
   framing (`PORT_GOAL_AND_PLAN.md:28`, `:52`), though still only **96 of
   857 (≈11.2%)**.
3. A separate static source scan (this worker's own script, method below)
   found only ~21–23 `battle` functions with a body-with-braces reachable
   from the port branch of `src/battle/*.c` or `pc_port/src/*.c` — lower
   than (2) because it missed host bodies defined in the separate
   `pc_port/src/battle_overlay_host_leaves.inc`-driven leaf files that stage
   `[2c/5]` compiles from; **(2) is the authoritative number**, produced by
   the actual build, not a regex.

**Bottom line: at minimum 761 of 857 battle functions (857 − 96) have no
verified host C replacement and, when the retail code path reaches them,
still run through `pc_port/src/battle_mips_runtime.c`'s interpreter** (now
1,327 lines, up from the plan doc's recorded 1,159 — `wc -l
pc_port/src/battle_mips_runtime.c`). This is a lower bound, not an exact
count, because it is a static reachability proxy, not an instrumented trace.

### 4c. `INCLUDE_ASM` sites with no C body anywhere

Static verification script (not committed; ad hoc analysis, method
described so it is reproducible): walks every `INCLUDE_ASM(...)` call site in
`src/**.c`, tracks `#ifdef`/`#ifndef XENO_PC_PORT` branch nesting per line,
and — unlike `decomp_status.py` — actually searches for a real function
**definition** (name followed by `(...)  {` with no `;` before the brace) in
the port-active branch of the same file, in the matching `pc_port/src/*_port.c`
replacement file, or anywhere under `pc_port/src/*.c`, rather than trusting
the `#ifndef` branch shape alone.

```
TOTAL INCLUDE_ASM sites: 906
  unconditional (no port branch at all):                169
  coexistence-claimed (inline #ifdef/#ifndef XENO_PC_PORT): 737
    - verified real C body found:  186
    - claimed but NO real body found: 551
==> no C body anywhere (unconditional + unverified coexistence): 720 / 906  (79.5%)
```

**906 total matches the doc's claimed denominator exactly.** The measured
no-body count is **720/906 (79.5%)**, close to but not identical to the
doc's claimed **717/906 (79.1%)** — a 3-function difference, well within the
noise of two different static heuristics counting the same thing; **the
doc's claim is corroborated, not refuted**, by an independently-produced
number.

The trap the task warned about is real and directly demonstrated. Example:
`src/battle/main.c:88-93`:
```c
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main", func_80070F40);
INCLUDE_ASM("asm/battle/nonmatchings/main", func_800716D8);
INCLUDE_ASM("asm/battle/nonmatchings/main", func_8007171C);
INCLUDE_ASM("asm/battle/nonmatchings/main", func_800718BC);
#endif
```
There is **no `#else`** — in port mode (`XENO_PC_PORT` defined) these four
functions are entirely absent from the compiled TU, functionally identical
to an unconditional `INCLUDE_ASM` (auto-stubbed at link, if referenced).
`tools/scripts/decomp_status.py`'s `scan_preproc_coexistence()` (its own
docstring, lines 27-30) marks any `INCLUDE_ASM` inside a bare `#ifndef
XENO_PC_PORT` block as `COEXISTENCE` without checking for a paired `#else`
body — so it silently classifies all four of these as "the port has a real
body" when the port has *nothing*. Confirmed live: `decomp_status.py` could
not even be run this session (`FileNotFoundError:
.../asm` — it depends on the split `asm/` tree, which only exists
transiently mid-build and was not preserved), so its "100% done" framing
could not be re-checked directly, but the source-level trap it documents is
demonstrated above independent of that.

---

## 5. Ranked next-wave list

Ranked by ROI toward "get executed code out of the MIPS interpreter and into
C," per the plan doc's Phase A framing — HD-2D rendering needs battle/UI code
in real C before anything visual in it can be restyled.

1. **Land the §0 fix repo-wide and add a splat-exit-code check to `gears
   matching`/`gears report`.** Size: trivial (the 2-line content fix is
   already committed as `66d87a6c`; the exit-code-swallowing wrapper fix is
   a few lines in the `gears` Rust tool). Acceptance test: `gears matching`
   returns nonzero and `make build` aborts *immediately*, with no
   stale-relink possible, if any `config/symbol_addrs.*.txt` fails to parse.
   Ranked #1 because this is not a decomp task at all — it is why this
   whole baseline had to be re-measured from scratch, and why at least three
   figures in `ACTIVE_HANDOFF.md` (battling, world_map, battle_command_file1
   byte diffs) were fiction. Every other number in this repo is suspect
   until this class of failure can't happen silently again.
2. **Port more of `battle`'s 761 still-interpreted functions to real host C
   leaves** (the same mechanism as the 96 already-adopted ones in
   `pc_port/src/battle_overlay_host_leaves.inc` / stage `[2c/5]`). Size:
   large (761 functions, several hundred to low-thousands of retail bytes
   each). Acceptance test: differential harness in `pc_port/tests/` at
   O0/O2/UBSan, per the plan doc's stated bar ("correct, not necessarily
   byte-exact"), plus the `[2c/5]` "N adopted leaves reach no generated
   stub" self-check staying green. Ranked #2, unchanged from the plan doc:
   battle is where "everything visual in combat sits behind the
   interpreter," and at 96/857 (≈11%) it is still overwhelmingly a black
   box — this is the actual Phase A gate.
3. **Close `menu`'s remaining `INCLUDE_ASM` gap.** Measured this run: 139
   sites, 109 already verified with real bodies, only **30 with no body
   anywhere** — menu's port-coexistence gap (as distinct from its 7.36%
   matching-byte-parity figure, a different axis) is nearly closed. Size:
   small (30 functions, one file: `src/menu/main/misc.c` and neighbors).
   Acceptance test: same differential-harness bar as #2. Ranked #3 because
   it is the cheapest way to take an entire overlay's port-side gap to zero,
   and menu UI is a highly visible HD-2D surface per the plan doc's Phase A
   ordering (`:73`, "menu... cheapest, most visible HD win").
4. **Close `field`'s remaining `INCLUDE_ASM` gap** — only 3 sites measured
   this run, essentially free. Size: trivial. Acceptance test: same harness.
   Ranked #4 purely on size; not visually significant on its own but a
   one-session item that removes `field` entirely from the "has an
   INCLUDE_ASM gap" list. Note: `field`'s byte gate (§1) currently **FAILS**
   or is only 34.71% matched by objdiff (§3) — this item is specifically
   about the small port-coexistence gap, not the larger byte-parity failure,
   which is a separate, bigger investigation this baseline did not have
   scope to root-cause.
5. **`world_map`'s remaining decompilation** (0/386 functions matched in
   `world_map/120C`, the single largest unmatched object in the whole repo
   at 168,336 unmatched bytes per §3's burn-down table). Size: very large.
   Acceptance test: objdiff `Functions` column for `world_map/120C` moving
   off 0/386, plus the byte gate (already MATCH, §1) staying green
   throughout. **Ranked last only because a sibling worker already owns
   `config/symbol_addrs.world_map.txt` and is actively working this** — by
   raw size and "never even disassembled until recently" history
   (`PORT_GOAL_AND_PLAN.md:66`) it would otherwise rank #1 or #2; listed
   here so the next wave doesn't duplicate in-flight work.

Explicitly **not** ranked: `battling` and `battle_command_file1` are, per
prior handoff notes, deliberately excluded from the port itself ("retail
state-4 MIPS overlay scaffold"), so their 0%-C objdiff numbers are not a gap
to close — they byte-match (§1) and that is the only bar that applies to
them.

---

## Method notes / reproducibility

- All commands above ran inside the mandated podman container
  (`localhost/xenogears-dev-toolchain:krom-20260913`, `. /.venv/bin/activate`),
  with the worktree mounted read-write at the container path
  `/home/blizz/Projects/xenogears-decomp`, `disc/` mounted read-only from the
  parent checkout, and (deviating from the task brief's exact recipe, per
  coordinator correction) **`assets/` copied in** rather than mounted
  read-only, because `gears matching`/splat writes generated databin
  segments into `assets/` during the build. `pc_port/extern/PsyCross` was
  additionally mounted read-only from the parent checkout (vendored,
  gitignored, environment-only — not present in a fresh worktree at all) so
  that `pc_port/build_port.sh` could run.
- `make build`'s final `ninja -j$(NUMPROC)` invocation always fails at the
  `slus_006.64.elf` link step (§1's "NOT_RUN" row) and, being unguarded by
  `-k`, stops scheduling new independent targets after that point. The 9
  non-`slus_006.64` `.bin` artifacts in §1 were obtained by re-invoking
  `ninja` directly with explicit `build/out/<overlay>.bin` targets after the
  `make build` run, which is safe and produces identical output to what a
  fully-green `make build` would have written for those targets (ninja is
  content-addressed on the same build graph) — it does not touch anything
  `slus_006.64`-dependent.
- §4c's script is ad hoc (written for this measurement, not committed to the
  repo) — its exact heuristic is described inline so results are
  reproducible, but it should not be mistaken for a permanent tool. It is a
  strictly better check than `decomp_status.py` for the one question it
  answers (does a coexistence-claimed `INCLUDE_ASM` site actually have a
  body?), but it is not a replacement for `decomp_status.py`'s broader
  bookkeeping.

---

## Addendum, same day — closing ranked item #1: `gears` swallowing splat's exit code

Two corrections to the numbers above, from sibling work that landed after
the original measurement pass (both independently re-verified in this
follow-up, not taken on faith):

- **`slus_006.64` is no longer `NOT_RUN`.** Commit `c1ea7b87` (cherry-picked
  into this worktree) declared the six interior fields of the single
  `InterruptEnvironment_t` at `g_InterruptEnvironment = 0x800578A4`
  coherently (the four this report flagged, `D_800578A6`/`D_800578A8`/
  `D_800578D4`/`D_800578E0`, plus the two already-patched
  `D_800578D6`/`D_800578D8`) and removed two mixed TUs (`temp1.`,
  `animation_scripts.`) from the `SdataSmall` preset, which had been forcing
  a latent GPREL16 overflow. `slus_006.64.elf` now links. It still does not
  match retail byte-for-byte (that is a separate, open question, not fixed
  here) — `sha256sum -c config/checksum.sha` now reports it `FAILED` rather
  than "no such file or directory," which is real progress: **`make check`
  reaches a result for the first time in this project's history.**
- **`field`'s byte-parity failure has been root-caused as CODEGEN, not
  layout**, per the sibling working it (324 of 878 functions diverge). Out
  of scope here; noted only so this report doesn't read as stale on the
  point.

Re-measured after both fixes, same commands as §1/§2:

```
$ sha256sum -c config/checksum.sha
build/out/slus_006.64: FAILED
build/out/field.bin: FAILED
build/out/member_change_menu.bin: OK
build/out/shop_menu.bin: OK
build/out/battle.bin: OK
build/out/menu.bin: FAILED
build/out/movie.bin: OK
build/out/battling.bin: OK
build/out/world_map.bin: OK
build/out/battle_command_file1.bin: OK
sha256sum: WARNING: 3 computed checksums did NOT match
```

**7 of 10 OK, 3 FAILED (`slus_006.64`, `field`, `menu`) — this matches the
independently-reported `make check` result exactly**, which is a good
cross-check for both this report's byte-gate methodology and the sibling's
link fix.

### The fix: `tools/gears/src/main.rs`, `split_binary()`

Found and fixed exactly where ranked item #1 said it would be. `gears`'s
*entire* subprocess surface is one function, `split_binary()`
(`tools/gears/src/main.rs:93`), called once for the main binary and once per
overlay from `main()`'s split loop (lines ~247–274). A repo-wide search
(`grep -n "Command::new\|.spawn()\|.output()\|.status()" tools/gears/src/*.rs`)
confirms this is the **only** subprocess `gears` invokes anywhere — there is
no second instance of this bug class to find in the same tool. (Ninja and the
MIPS toolchain are invoked directly by the `Makefile`, not by `gears`, and
were out of this task's scope — "on the same path" means inside `gears`
itself.)

Before:

```rust
fn split_binary(yaml_path: &String) {
    let splat_child = Command::new("python3")
        .arg("-m").arg("splat").arg("split")
        .arg("--disassemble-all").arg("--make-full-disasm-for-code")
        .arg(format!("{}", yaml_path))
        .spawn()
        .expect("Failed to start splat process.");
    let _ = splat_child.wait_with_output().expect("Failed to wait on splat");
}
```

`wait_with_output()` returns `Result<Output, io::Error>`. `.expect(...)`
only panics if *waiting itself* fails (an OS-level error); it says nothing
about the child's exit status, which lives in `Output.status`. `let _ = ...`
then discards that `Output` — status, stdout, and stderr all together — the
instant it's produced. There is nothing after this line that ever looks at
whether splat succeeded. Checked: **no comment, commit message, or doc
anywhere in the repo explains this as deliberate**, and no caller treats a
nonzero splat exit as tolerable-by-design. This reads as a "spawn it, wait
for it to finish, move on" oversight from whoever first wrote the loop, not
a workaround for a known-flaky splat exit code — so the correct fix is to
stop discarding the status, not to special-case any exit code.

After (`tools/gears/src/main.rs:93-118`):

```rust
fn split_binary(yaml_path: &String) {
    let command_description = format!(
        "python3 -m splat split --disassemble-all --make-full-disasm-for-code {}",
        yaml_path
    );
    let splat_child = Command::new("python3")
        .arg("-m").arg("splat").arg("split")
        .arg("--disassemble-all").arg("--make-full-disasm-for-code")
        .arg(format!("{}", yaml_path))
        .spawn()
        .expect("Failed to start splat process.");
    let output = splat_child.wait_with_output().expect("Failed to wait on splat");

    if !output.status.success() {
        eprintln!();
        eprintln!("gears: FATAL: splat exited with {} while splitting '{}'.", output.status, yaml_path);
        eprintln!("gears: command was: {}", command_description);
        eprintln!("gears: splat's own error output is printed above (its stderr is inherited, not captured).");
        eprintln!("gears: stopping now -- a failed split must not fall through to ninja generation or a stale relink.");
        process::exit(1);
    }
}
```

Deliberately **kept** stdio inherited rather than switching to
`Stdio::piped()`: splat streams a live progress bar during a multi-minute
matching pass, and capturing would buffer that away until the end, which
would be a real regression for anyone watching a long build. Because stdio
is inherited, splat's actual error text (the Python traceback, the assertion
message) already prints live, in place, before `gears` ever sees the exit
status — so `gears` doesn't need to re-capture and re-print that text
itself; it only needs to (a) notice the failure, (b) say unambiguously that
it was *splat*, with the exact command and exit status, and (c) stop cold,
so nothing can print or build *after* the failure to bury it. That is what
made the original failure "buried" in practice: not that the text was
invisible, but that `make`'s recipe kept running commands after it (the
`grep`/`sed` lines patching `linker/` files that splat never generated),
so the real cause scrolled off screen under irrelevant follow-on noise.
Stopping immediately is what actually fixes that.

`tools/gears` is built as a prebuilt binary on Linux/x86_64
(`Makefile`'s `NEED_GEARS := false` branch — `make build` never invokes
`cargo` at all on this platform); the container has no `cargo`/`rustc`
installed. Rebuilt on the host (`cargo build --release`, matching how
`tools/gears/prebuilt/gears` was produced originally) and copied over
`tools/gears/prebuilt/gears`; confirmed the resulting binary runs correctly
inside the pinned toolchain container (no glibc-version mismatch) before
relying on it for anything below.

### Acceptance 1 — healthy tree, `make build` still succeeds, same 7 overlays match

```
$ podman run ... bash -lc 'source /.venv/bin/activate && make build; echo EXITCODE=$?'
...
[1241/1241] Creating ELF build/out/battle.elf (Objcopy)
EXITCODE=0
```

Followed by the `sha256sum -c config/checksum.sha` output quoted above:
7 OK / 3 FAILED, identical to the pre-fix §1 table (`slus_006.64`, `field`,
`menu` are exactly the three that were already failing for reasons unrelated
to this fix — the link fix in `c1ea7b87` and the pre-existing
CODEGEN/layout issues, respectively). **Nothing regressed.**

### Acceptance 2 — reintroduce the exact HEAD defect; prove it now fails loudly

Reintroduced the precise byte-for-byte defect: changed line 412 of
`config/symbol_addrs.slus_006.64.txt` back to

```
firstfile = 0x80040584; // type:func (retail .text function; the port generator must not emit a data stub)
```

(semicolon inside the parenthetical, exactly as it read at `ed221dc0`), then
ran the identical `make build` command. Full, unedited tail:

```
splat 0.33.2 (powered by spimdisasm 1.33.0)
  0%|          | 0/1068 [00:00<?, ?it/s]Loading symbols (symbol_addrs.slus_006.64):   0%|          | 0/1068 [00:00<?, ?it/s]
error reading config/symbol_addrs.slus_006.64.txt, line 412:
	firstfile = 0x80040584; // type:func (retail .text function; the port generator must not emit a data stub)
Line must be of the form
<function_name> = <address>; // attr0:val0 attr1:val1 [...]
with <address> in hex preceded by 0x, or dec

Loading symbols (symbol_addrs.slus_006.64):  38%|███▊      | 411/1068 [00:00<00:00, 184488.33it/s]
Traceback (most recent call last):
  File "<frozen runpy>", line 198, in _run_module_as_main
  File "<frozen runpy>", line 88, in _run_code
  File "/.venv/lib/python3.12/site-packages/splat/__main__.py", line 31, in <module>
    splat_main()
  File "/.venv/lib/python3.12/site-packages/splat/__main__.py", line 27, in splat_main
    args.func(args)
  File "/.venv/lib/python3.12/site-packages/splat/scripts/split.py", line 555, in process_arguments
    main(
  File "/.venv/lib/python3.12/site-packages/splat/scripts/split.py", line 483, in main
    initialize_all_symbols(all_segments)
  File "/.venv/lib/python3.12/site-packages/splat/scripts/split.py", line 200, in initialize_all_symbols
    symbols.initialize(all_segments)
  File "/.venv/lib/python3.12/site-packages/splat/util/symbols.py", line 323, in initialize
    handle_sym_addrs(path, sym_addrs_lines, all_segments)
  File "/.venv/lib/python3.12/site-packages/splat/util/symbols.py", line 107, in handle_sym_addrs
    assert line.count(";") == 1, "Line must contain a single semi-colon"
           ^^^^^^^^^^^^^^^^^^^^
AssertionError: Line must contain a single semi-colon

gears: FATAL: splat exited with exit status: 1 while splitting 'config/slus_006.64.yaml'.
gears: command was: python3 -m splat split --disassemble-all --make-full-disasm-for-code config/slus_006.64.yaml
gears: splat's own error output is printed above (its stderr is inherited, not captured).
gears: stopping now -- a failed split must not fall through to ninja generation or a stale relink.
make: *** [Makefile:71: build] Error 1
EXITCODE=2
```

**No ninja invocation follows the `gears: FATAL` block** — compare this to
the original `ed221dc0` failure mode, where `gears matching` silently
returned success and `make`'s recipe went on to run `grep`/`sed` against
`linker/` files that were never generated, failing later and further away
from the real cause. Now the build stops at the source of the problem, names
the tool, quotes the command, and gives the exit status, before anything
else can run.

Reverted the line immediately after capturing the above (`git diff HEAD --
config/symbol_addrs.slus_006.64.txt` confirmed clean), then re-ran `make
build` once more to confirm the healthy path (Acceptance 1) still holds
after the revert — `EXITCODE=0`, same 7/10 byte-gate result.

### What was and wasn't touched

- Fixed: `tools/gears/src/main.rs` (`split_binary`) and the regenerated
  `tools/gears/prebuilt/gears` binary it produces. This is the entire fix.
- Cherry-picked (not authored here, but required to reach a "healthy tree"
  baseline to test against): `c1ea7b87` (`slus_006.64` link fix, touches
  `Makefile` and `gears.toml`, not `src/**`/`pc_port/**`/`config/*.yaml`).
- Not touched: `src/**`, `pc_port/**`, any `config/*.yaml`, `src/field/**`,
  `config/menu.yaml`, and the sibling-owned evidence docs named in this
  task's constraints.
- Checked and found clean: no second subprocess call site in `gears` to fix.
  If a future contributor adds one (e.g. a new external tool invocation),
  the same discard-the-`Output` mistake is easy to repeat; there is no
  lint or test in this repo that would catch it today.
