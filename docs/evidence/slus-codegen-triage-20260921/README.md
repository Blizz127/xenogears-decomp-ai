# Triage of the slus "short by a large margin" cohort

Date: 2026-09-21 UTC. Companion to `../slus-byte-parity-20260921/README.md`,
which flagged 17 functions whose built body is under 75% of retail — 2,424
bytes — and called them the class that matters for the port: missing
behaviour rather than register allocation.

Method reused verbatim from the field worker's
`docs/evidence/field-codegen-20260921/TRIAGE.md` (Tests A/B/C + the lui
delta), which found 18 of 18 innocent. Starting hypothesis here was therefore
**innocent until proven otherwise**.

## VERDICT

**One real correctness bug, and it is fixed. Everything else is innocent.**

`func_800347C0` was computing a dialogue text-row Y coordinate with the wrong
formula — missing a field, a branch and a multiplier. It is on an executed
path (the Lahan dialogue continue-arrow). Fixed in this commit.

Of the other 16: one is a deliberate, in-file-documented omission; **five have
no deficit at all** and were pure measurement artifacts; ten are the same
codegen-style category the project has already decided not to pay for.

The headline number also shrinks hard under measurement correction. The worst
offender, `func_80034FFC` — reported at 1,204 bytes missing and 29% of retail —
is actually at **97% of retail with a 52-byte residual**.

| | functions | bytes |
|---|--:|--:|
| as reported in the parity report | 17 | 2,424 |
| after correcting two measurement artifacts | 12 with any deficit | 1,156 |
| genuinely MISSING behaviour | **1** | **44** |

## Two measurement artifacts, found and corrected

Both inflate deficits, and between them they account for more than half of the
original 2,424.

**1. Static helpers charged to nobody (1,152 bytes).** `nm` gives a static
helper its own symbol, so a TU that factors retail's inlined code into helpers
shows a fake deficit on the parent. The parity report carried this as a
caveat; for slus it is the single largest effect.

`func_80034FFC` calls two static helpers this project invented,
`func_80034FFC_row0` (572 B) and `func_80034FFC_row1` (580 B), which retail had
inline. Folding them in:

| | retail | built |
|---|--:|--:|
| bytes | 1,696 | 492 + 572 + 580 = **1,644** |
| branches | 58 | **58** |
| loads | 44 | **44** |
| stores | 36 | **37** (one *more*) |
| calls | 0 | 2 (the two helpers) |

Branch and load counts are **identical**, and the build stores once more than
retail. The "−52 branches / −17 stores" that made this look like missing
control flow was entirely the attribution artifact. Residual 52 bytes = two
call/return sequences, two helper prologue-epilogues, and 5 re-materialized
`lui`.

**2. Inter-function `nop` padding charged to the preceding symbol (72 bytes).**
My own retail parser in `../slus-byte-parity-20260921/analyze.py` keys on
`glabel` and accumulates until the *next* glabel, ignoring the `.size`
directive. In the multi-function aggregate `.s` files spimdisasm emits
`.size`, then padding, then the next label — so the padding was charged to the
function above it. Re-measured honouring `.size` (`size_check.py`), **five
cohort members have a true deficit of exactly zero**: `func_8004E9FC`,
`func_8004BED8`, `func_80040C20`, `AverageSZ4`, `func_8004BEC0`. `NormalClip`
drops 20 → 8 and `Lzc` 12 → 8.

`func_80040C20` is the clearest: retail "16 bytes", real body **one
instruction** (`and $v0, $v0, $s5`), a jump-table target inside hand-written
asm referenced by `lui/%lo(func_80040C20)`. Both sides assemble from the same
`.s`. It was never short.

## The third mechanism: retail's unfilled delay-slot nops

The field cohort's dominant cause was base-address re-materialization. Here
the lui delta is small (5 instructions on the worst function). The slus
analogue is **load-delay `nop`s that retail left unfilled** and maspsx/gas
schedules away:

| function | deficit | retail nops | built nops | Δ | bytes | % of deficit |
|---|--:|--:|--:|--:|--:|--:|
| func_80034888 | 396 | 52 | 36 | 16 | 64 | 16% |
| func_80032F54 | 316 | 23 | 4 | 19 | 76 | 24% |
| func_800234AC | 80 | 9 | 2 | 7 | 28 | 35% |
| func_8003014C | 48 | 7 | 2 | 5 | 20 | 42% |
| func_80033B34 | 32 | 5 | 3 | 2 | 8 | 25% |
| NormalClip | 8 | 6 | 1 | 5 | 20 | **100%** |
| Lzc, AverageSZ4, func_8004BED8, func_8004BEC0 | ≤12 | | | | | **100%** |

The fourth mechanism, and the biggest single one after nops, is that retail
**re-loads memory operands it has already loaded**. `func_800234AC` reloads
the same two pointers *five times per loop iteration*, once before each field
store — 10 loads where the build has 2. Same six stores, same eight
iterations, identical behaviour.

## Test A and Test B

| test | functions failing |
|---|---|
| A — retail calls something the C never calls | **1 of 17** |
| B — retail writes a global the C body never names | **0 of 17** |

Test A's single failure is `GfxLoadImageAccelerated`, and it is a deliberate
omission already documented in the source. Retail allocates a 0x2000 heap
buffer, **relocates the stack pointer** into it, calls `LoadImage`, restores
`sp` and frees — a PSX stack-placement performance trick (hence
"Accelerated"). The C calls `LoadImage(D_800592F0, D_800592F4)` with identical
arguments and skips the stack swap. The full version is present in
`system/graphics.c` immediately below, commented out, annotated *"Matches on
ASPSX 2.56, GCC 2.6.0"*. Named absent construct:
`HeapAlloc(0x2000,1)` / `sp` relocation / `HeapFree`. No effect on
`LoadImage`'s arguments or results; not a port correctness issue.

## Per-function table

`store`/`brnch`/`call` are **built minus retail**; a positive `call` means the
C calls more often, i.e. retail inlined. `trueDef` is after both measurement
corrections.

| function | trueDef | store Δ | brnch Δ | call Δ | luiΔ insn | A | B | verdict |
|---|--:|--:|--:|--:|--:|:-:|:-:|---|
| func_80034FFC | 52 | +1 | 0 | +2 | 10 | . | . | static-helper attribution (proven: branches and loads equal) |
| func_80034888 | 396 | −2 | −3 | +1 | 4 | . | . | nops 64B + retail re-loads (96 ld vs 68) + inlining |
| func_80032F54 | 316 | **+5** | 0 | 0 | 0 | . | . | nops 76B + retail re-loads (69 ld vs 39); C stores *more* |
| func_80023804 | 88 | 0 | −1 | 0 | 6 | . | . | re-materialization + nops |
| func_800234AC | 80 | 0 | 0 | 0 | 0 | . | . | retail re-loads 2 ptrs 5x per iteration (10 ld vs 2) |
| func_80030A30 | 68 | 0 | 0 | **+1** | 4 | . | . | retail inlined a callee; re-materialization |
| GfxLoadImageAccelerated | 60 | −2 | 0 | −2 | 0 | **!** | . | deliberate documented omission (stack relocation) |
| func_8003014C | 48 | −3 | −1 | 0 | 0 | . | . | duplication: retail stores the clamp into each arm; no CSE |
| **func_800347C0** | **44** | 0 | −1 | 0 | 0 | . | . | **MISSING — fixed, see below** |
| func_80033B34 | 32 | 0 | 0 | 0 | 2 | . | . | dead 8-byte stack frame + duplicated tail load |
| NormalClip | 8 | 0 | 0 | 0 | 0 | . | . | handwritten GTE stub; nop padding |
| Lzc | 8 | 0 | 0 | 0 | 0 | . | . | handwritten GTE stub; nop padding |
| func_8004E9FC | **0** | — | — | — | — | . | . | artifact — no deficit |
| func_8004BED8 | **0** | — | — | — | — | . | . | artifact — no deficit |
| func_80040C20 | **0** | — | — | — | — | . | . | artifact — no deficit |
| AverageSZ4 | **0** | — | — | — | — | . | . | artifact — no deficit |
| func_8004BEC0 | **0** | — | — | — | — | . | . | artifact — no deficit |

Functions cleared on the volume rule — store Δ ≥ 0, branch Δ ≥ 0, call Δ ≥ 0,
Tests A and B pass — have no missing control flow, no missing side effects and
no missing calls by construction, so their deficit can only be arithmetic and
load volume. `func_80032F54`, `func_800234AC` and `func_80030A30` are cleared
that way. `func_8003014C`, `func_80033B34`, `func_800234AC`, `NormalClip`,
`func_80034888` and `func_800347C0` were additionally read in full against
their retail asm.

## The one real bug: func_800347C0

Retail (`asm/slus_006.64/matchings/system/system/func_800347C0.s`):

```
lh    v1, 0x02(a0)      ; current row
lh    v0, 0x16(a0)      ; scroll base row
subu  v1, v1, v0
bgez  v1, .L800347E4    ; if negative, wrap to the last row
lh    v0, 0x0C(a0)      ;   row count
addiu v1, v0, -1
.L800347E4:
lh    v0, 0x14(a0)      ; row stride in pixels
mult  v1, v0
lh    v0, 0x06(a0)      ; window origin y
mflo  a1
addu  v0, v0, a1
```

What the C had:

```c
s32 func_800347C0(void* arg0) {
    return *(s16*)((u8*)arg0 + 0x6) + (*(s16*)((u8*)arg0 + 0x2) << 2);
}
```

It had been written as the vertical twin of `func_800347AC` directly above it
— same shape, offsets bumped by 2, same `<< 2`. `func_800347AC` is byte-exact
and correct: horizontally the multiplier really is 4 (character cell width).
Vertically it never was. **Three named absent constructs:**

1. the subtraction of the **scroll base** at `+0x16` — ignored entirely;
2. the **negative-wrap clamp branch** `if (row < 0) row = rowCount - 1` using
   `+0x0C` — absent, so scrolled text wrapped nowhere;
3. the **row-stride multiply** by the `s16` at `+0x14`, replaced by a
   hardcoded `<< 2`. `func_80032F54` initialises that field to **0x0E**, so
   the arrow's Y was 3.5× too small.

Corroborated independently: `func_80034888` positions every text row at
`*(u16*)(pWindow + 0x06) + *(s16*)(pWindow + 0x14) * rowIndex` — the identical
formula this function must agree with.

Consequence: the dialogue continue-arrow was drawn near the top of the window
instead of on the current text row, and did not follow scrolled text. Called
from `src/field/dialogue/text_box_render.c:246` (not modified — sibling-owned).

After the fix the built body is **64 bytes, exactly retail's 64**, and matches
instruction for instruction except one delay slot that GCC fills with
`move a1,a0` where retail has `nop`:

```
1638: lh    v1,2(a0)        163c: lh   v0,22(a0)     1640: nop
1644: subu  v1,v1,v0        1648: bgez v1,165c       164c: move a1,a0
1650: lh    v0,12(a0)       1654: nop                1658: addiu v1,v0,-1
165c: lh    v0,20(a1)       1660: nop                1664: mult v1,v0
1668: lh    v0,6(a1)        166c: mflo a2            1670: jr   ra
1674: addu  v0,v0,a2
```

Two test harnesses carried a **copy** of the old body labelled *"Verbatim leaf
logic from src/slus_006.64/system/system.c"* —
`pc_port/tests/portrait_render_regression_test.c` and
`pc_port/tests/npc_event_prod_test.c`. Both were updated to match, so the
harness cannot silently keep testing the old formula.

## Harness output

```
run_portrait_render_regression_test.sh   PORTRAIT_OPT=O0
    Visible portrait, context selection, clamp, and OT order: PASS
    Background/portrait retail OT order: PASS
    shouldRenderPortrait gate: PASS
    Flip flag and small-box sizing: PASS
    Portrait mutant negative controls: PASS
    Retail portrait cache-slot selector: PASS
    Distinct owner/speaker: 192 cases PASS
    Portrait render regression: PASS
run_portrait_render_regression_test.sh   PORTRAIT_OPT=O2   (same 8 lines, PASS)

run_system_menu_frame_retail_test.sh     EXIT 0
    SYSTEM MENU FRAME control rejected: stuck-buffer / wrong-submit-entry
                                        / wrong-page-toggle / skip-debug-recheck
run_system_data_lookup_retail_test.sh    EXIT 0
    SYSTEM DATA LOOKUP RETAIL PASS cases=2194, pointers/read-only tables/glyphs/guards
run_menu_window_open_test.sh
    PASS window opening: six-frame prompt and 262144 seven-slot boundary cases   (O0)
    PASS window opening: six-frame prompt and 262144 seven-slot boundary cases   (O2)
```

**NOT RUN — UBSan regime, on every suite.** This host has no
`libubsan`; `/usr/lib64/libubsan.so.1.0.0` is absent and a bare
`gcc -fsanitize=undefined` link fails, so no UBSan regime executed anywhere in
this dispatch. `run_textbox_timing.sh` and `run_npc_event.sh` are
UBSan-only-at-the-end and therefore did not complete at all; the
`run_menu_window_open_test.sh` UBSan third regime also did not run. This is
the same host limitation the field worker recorded. Do not read any of this as
a UBSan pass.

Two suites needed local fixtures that a worktree does not carry: `disc/` and
`pc_port/extern/PsyCross` were symlinked in from the parent checkout to run
them. Both paths are gitignored and nothing was written to the parent.

## make check

Re-run from clean after the fix: **7 of 10 OK**, unchanged.

```
build/out/slus_006.64: FAILED          build/out/movie.bin: OK
build/out/field.bin: FAILED            build/out/battling.bin: OK
build/out/member_change_menu.bin: OK   build/out/world_map.bin: OK
build/out/shop_menu.bin: OK            build/out/battle_command_file1.bin: OK
build/out/battle.bin: OK
build/out/menu.bin: FAILED
```

`build/out/slus_006.64` 302,716 → **302,760** bytes against retail 303,104: the
fix moved the image 44 bytes **closer** to retail, deficit −388 → −344.
`battling.bin` still `3e6df915…`. Nothing regressed.

## Correction to the parity report

`../slus-byte-parity-20260921/README.md` states the cohort as "17 functions,
2,424 bytes" and `func_80034FFC` as "29% of retail, 1,204 bytes missing". Both
figures are inflated by the two measurement artifacts documented above. The
corrected figures are 12 functions with any deficit totalling 1,156 bytes, and
`func_80034FFC` at 97% of retail with a 52-byte residual. The per-TU totals in
that report are unaffected — they were measured from the linker map, which has
neither problem, and the report already named per-TU as authoritative.

## Scripts

Re-runnable from the project root inside the toolchain container:
`triage.py` (Tests A/B/C + lui delta), `helper_aware.py` (static-helper
attribution), `nop_delta.py` (delay-slot nops), `size_check.py` (the `.size`
padding correction).

## Not established

- Runtime confirmation that the corrected `func_800347C0` places the
  continue-arrow correctly on screen. The fix is verified against retail asm
  instruction-for-instruction and by harness, not by a captured frame.
- Whether `func_80034888`'s residual 192 bytes beyond nops/loads is entirely
  CSE. Every retail construct in it has a C counterpart by reading, and Tests
  A/B/C pass, but the byte accounting is not closed to zero.
- The per-symbol sdata fix for `temp1` remains the known-correct approach to
  the GPREL16 problem, logged in `../slus-byte-parity-20260921/README.md` and
  deliberately not attempted.
