# field.bin exactness — 2026-09-10

Session goal (user-selected): drive `field.bin` to byte-exact retail.
Acceptance: from-clean `make rom-check` → `PASS build/out/field.bin 38a1ce82…`;
member_change_menu / shop_menu / battle stay PASS; slus unchanged red.

## Result

**Not reached.** Verified progress, no regressions:

| artifact | before | after |
|---|---|---|
| field.bin built hash | `ef934006` | `44288dd2` |
| field.bin size | 243254 | 246014 (**+2760 B**) |
| slus_006.64 | `f7c1f169` | `f7c1f169` (unchanged) |
| member_change_menu | PASS `3b9e2b89` | PASS `3b9e2b89` |
| shop_menu | PASS `7890e14b` | PASS `7890e14b` |
| battle | PASS `1830b4ef` | PASS `1830b4ef` |

Field per-function census (tools below):

| | baseline | after |
|---|---|---|
| retail functions | 878 | 878 |
| exact size | 657 | **658** |
| short | 157 (Σ 20124) | 156 (Σ 17192) |
| long | 64 (Σ 1304) | 64 (Σ 1304) |
| net function-byte delta | 18820 | **15888** |

`field.bin` remains short of retail `38a1ce82` (260862 B); the overlay is still
**14848 B short** and therefore **not byte-exact**.

## Fix 1 — `func_800A3474` / `func_800A3F4C` (src/field/scripts/virtual_machine.c)

Baseline: `func_800A3474` 416 B vs retail 2072 (−1656); `func_800A3F4C` 524 B
vs retail 2044 (−1520).

Root cause: the two field-state copy helpers `FieldStateRestoreCopy` /
`FieldStateSaveCopy` were byte-loop helpers taking a runtime `size`.

```c
for (i = 0; i < size; i++) { dstBytes[i] = D_800AFC50[i]; }
D_800AFC50 += size;
```

Retail **inlines a compiler-expanded block move at every call site**: an
alignment test (`or src,dst; andi ..,3; beqz`) selecting an unaligned
`lwl/lwr … swl/swr` 16-byte loop or an aligned `lw/sw` 16-byte loop, then a
tail, i.e. GCC 2.x `emit_block_move`. Measured compiler behaviour (pinned
`tools/gcc-2.7.2-psx`):

- `__builtin_memcpy(d, s, 0x38)` **with a literal constant** → block move, no call.
- `memcpy(d, s, 0x38)` **with a literal constant** → identical block move.
- the same builtin/memcpy with a *runtime* size → `jal memcpy`.
- wrapping either in a `static inline` helper or a macro → `jal memcpy` (the
  call is lowered before inlining/constant-folding).

Fix: the helpers were removed and each call site now writes the retail idiom
directly, e.g.

```c
memcpy(D_800B007C, D_800AFC50, 0x38);
D_800AFC50 += 0x38;
```

All 20 call sites (10 restore + 10 save) are literal constants (0x38, 0x74,
0x400, 0x2E4, 0x1C8, 0x8, 0x138, 0xC, 0x10, 0x800), so every one expands.
A local `extern void* memcpy(void*, const void*, size_t);` was added (the
matching build is `-nostdinc` and this TU does not include a memcpy decl).

Second shape fix: retail stores `D_800AFC50` **twice** at entry (base, then
base+4). The source that reproduces this is three statements with the
`g_FieldNumActors` access between the two stores:

```c
D_800AFC50 = D_8005A4E4;
g_FieldNumActors = D_8005A4E4[0];      /* save variant: D_8005A4E4[0] = g_FieldNumActors; */
D_800AFC50 = D_8005A4E4 + 4;
```

`D_800AFC50 = D_8005A4E4 + 4;` alone emits one store; `= base` then `+= 4`
gets dead-store-eliminated to one store. Behaviour is unchanged in all forms
(the intermediate value is never read).

Measured after: `func_800A3474` 1864 B (−208 vs retail), `func_800A3F4C`
1996 B (−48). Their block moves are now structurally retail-shaped; the
residual is register allocation in the actor loop and the tail, not the copy
idiom.

## Fix 2 — `func_8007C670` (src/field/main/misc4.c), now instruction-exact

Retail (36 B) loads `*arg0`, stores it back **in both branches** of
`if (arg2 < 0)`, and adds `arg2` only on the non-negative path before
`*arg1 = value`. The port omitted the `*arg0 = value;` stores (24 B).
Restored:

```c
if (arg2 >= 0) {
    *arg0 = value;
    value += arg2;
} else {
    *arg0 = value;
}
*arg1 = value;
```

Result: 36 B = retail size, instruction-for-instruction identical
(`scratchpad/.../cmp.py` reports only the absolute `j` encoding differs, which
is the overlay's text-displacement effect; the jump is function-relative and
lands correctly).

No semantic change: `*arg0 = *arg0` is a no-op store.

## Method / tooling (scratchpad/field-exact-20260910/)

- `census.py` — per-function retail size (splat `.s`) vs built size (`nm -S`
  on `build/out/field.elf`); reproduces the recorded census.
- `seqalign.py` — difflib alignment of normalised instruction streams; separates
  real insertions/deletions from register-allocation substitutions.
- `summarize_align.py` — block-size summary of seqalign output.
- `/tmp/cmp.py` — raw 32-bit instruction-word comparison (retail `.s` words are
  stored byte-swapped; this script byte-swaps them). Definitive for
  size-matched functions; absolute branch/data encodings still differ while the
  overlay text is displaced.

## Honest remaining state / next steps

Byte-exact `field.bin` needs **all** 216 size-mismatched functions fixed plus a
final whole-overlay address match. The deficit is spread across files
(dialogue/text_box_render.c ≈3.5 KB, main/misc2.c ≈3.2 KB,
scripts/virtual_machine.c ≈2.7 KB, main/misc8.c ≈1.7 KB, main/misc4.c ≈1.6 KB,
main/misc.c ≈1.1 KB). `func_8007E1C0` alone is missing ~464 retail instructions
of genuine logic (a decomp gap, not codegen). The many −4/−8 functions are
individual logic/shape differences (several are real branch-polarity
differences, e.g. `func_80078BC8`, `func_8008D604`), not a shared lever.

This was a partial session: two verified fixes. No commit/stage/push (project
convention). The port still links (`pc_port/build_port.sh`: LINK OK, 69 stubs /
547 data symbols, unchanged).

## Continuation (same day): inlined-helper lever + two real bugs

**Result:** field.bin `44288dd2` (246014) -> `141a1be2` (**246378**, +364 B);
slus `f7c1f169` and the three PASS pins unchanged; port LINK OK. Absolute
per-function mismatch (short+long) 17840 -> 17592 B.

**Lever — `static inline` helper defined before its caller is inlined; a
`static` helper whose body appears *after* the caller cannot be.** The built
code was calling small local helpers that retail had inlined. Marking them
`static inline` only works if the definition precedes every call; where it did
not, the definition was *moved above the first caller* (misc4.c
`FieldPackedXZ` / `FieldCopyCameraEdge`). Verified per-function size changes
(all toward retail, no regressions):

| helper | caller(s) | retail | before | after |
|---|---|---|---|---|
| FieldPackedXZ / FieldCopyCameraEdge | func_8007BEF4 | 1916 | 1404 | 1468 |
| " | func_8007C694 | 1704 | 1212 | 1276 |
| " | func_8007CD80 | 1620 | 1072 | 1156 |
| FieldTextBoxLinkPrim | func_8007E1C0 | 3148 | 1144 | 1316 |
| " | func_8008004C | 1448 | 712 | 800 |
| FieldActorSyncSpritePosition | func_80076AC0 | 1776 | 1040 | 1144 |
| func_80084A40_RestoreActorState | func_80084A40 | 2704 | 2396 | 2632 |
| FieldPositionalSfxScreenPan | func_800860F0 / 862CC | 272 / 284 | 212 / 252 | 228 / 272 |
| FieldSetArchiveQueueEntry | func_80077884 | 560 | 524 | 540 |

`IsLiveHeapBlock` was tried and **reverted** — inlining it made `FieldFree`
worse (see next item); it is not a retail call at all.

**Fix — port-only code leaking into the matching build (`FieldFree`).** The
code's own comment says the `IsLiveHeapBlock` free-list walk is "a cheap,
always-safe backstop ... since retail's own asm has no equivalent check at
all", yet the two calls were compiled into the matching build. They are now
guarded `#ifdef XENO_PC_PORT`; the helper is dropped as unused in the matching
build. `FieldFree` 704 -> 664 (retail 656, was +48 now +8). Port keeps the
backstop.

**Fix — wrong archive function (`func_8009C154`, text_box.c).** The C called
`ArchiveDataSync()` while its own comment said "Retail: ArchiveCdDataSync(1)".
Retail `jal`s `ArchiveCdDataSync` with `$a0=1` and branches on the return
(`0x8009C1B4`), then never calls `ArchiveDataSync` in this function;
`ArchiveCdDataSync` (0x80028A60) and `ArchiveDataSync` (0x800286CC) are
different addresses. Fixed to `ArchiveCdDataSync(1)` plus a prototype. Size
unchanged (944 vs retail 996) but this removes a real behavioural defect: the
port was polling a sync without starting the read.

**Tooling added:** `jaldiff.py` (retail-vs-built `jal` target sets per
function — how the remaining wrong/inlined-call cases were found),
`compare_census.py` (per-function size diff between two census CSVs).

**Remaining systematic leads (not taken, recorded):**
- libgte calls that retail inlines as PSY-Q macros (`gte_ApplyMatrixSV`,
  `gte_SetRotMatrix`, `gte_NormalClip`, `gte_Square0`, …) in func_80075B44,
  func_800748E8, func_800764B4, func_8007B1C4, func_8007CD80, func_8008399C.
  The macros exist for both builds (`include/psyq/gtemac.h`,
  `pc_port/extern/PsyCross/include/psx/gtemac.h`), but retail *also* calls
  several of these as functions elsewhere, so the choice is per-function.
- `func_80072D74` is missing the retail rand()-driven camera-shake generation
  (`0x8007313C..0x800731AC`: three `rand()` products against `g_Scene+0xA2/A6/AA`
  into `D_800AF8E0/E4/E8`, plus the `g_Scene+0x9C` reset block) — genuine
  missing logic, ~73 instructions.
- `func_8007254C` / `func_80080A74` overshoot because the C materialises each
  camera global by its own symbol while retail keeps `$s0 = &g_CamInterpolation`
  and reaches adjacent globals by negative offset; needs the documented
  matching/port split, not a plain rewrite.

**Committed:** no.

## Third batch: camera-shake logic + four VM-argument bugs

**Result:** field.bin `141a1be2` (246378) -> `6cd7862d` (**246746**); slus
unchanged, three PASS pins unchanged, port LINK OK. Census exact 658 -> 659,
short Σ16328 -> 15972, long Σ1264 -> 1276.

- **`func_80072D74` (misc2.c) 872 -> 1188 (retail 1212).** The whole retail
  block 0x80073068..0x800731FC was missing: zero `D_800AF8E0/E4/E8`; if
  `g_Scene+0x98 != 0` then either integrate the shake velocities
  `g_Scene+0xA0/A4/A8 += +0xAC/B0/B4` (when `+0x9A != 0`) or cancel and clear
  (when `+0x9C != 0`); then `D_800AF8E0/E4/E8 = rand() * (s16)(g_Scene+0xA2/A6/AA)`
  and the negative clamp. The clamp existed but ran unconditionally and outside
  the guard. Real behaviour fix: camera shake never ran.
- **`func_800889BC` (misc.c) 420 -> 436 (retail 428).** Retail reads arguments
  in order (1),(1),(3),(5),(5),(7) and computes `speed = ((arg1b>>4)<<8) +
  GetArgument(3)`, `speed2 = ((arg5b>>4)<<8) + GetArgument(7)`. The C used
  `| (arg & 0xF)` and the wrong argument in both. Real bug (two actor-movement
  speed values wrong).
- **`func_8009640C` (misc10.c) 148 -> 164 = retail size.** Real bug: the C
  decremented `func_800950A0(item)[idx]` then wrote 0xFF into the *same* array;
  retail writes 0xFF into `func_8009501C(item)[idx]`, a call the C never made.
- **`func_80096214` (misc10.c) 152 -> 176 (retail 172); `func_800882B8`
  (misc.c) 156 -> 164 (retail 168).** Retail duplicates the
  `FieldScriptVMGetInstructionArgument(3)` read into both arms; the C hoisted it
  above the branch.

**Caveat for future jaldiff runs:** some "RETAIL-ONLY jal" flags are GCC
cross-jumping identical calls retail duplicated (`func_800A28D4` mode1/2
`func_8002303C(pSprite, mode+1, 0)`; `func_800A73E8`'s two HeapUnpinBlock pairs;
`func_800821F4`'s two `func_801E8330` calls). Those are codegen-shape, not
missing logic.

**Still open:** `func_800A08B8` third `func_80076AC0` call site; `func_8008B978`
fourth `FieldScriptGetBytecodeOffset`; `func_8009C5A8` second `func_8007F814`;
libgte macro conversion (`gte_ApplyMatrixSV`/`SetRotMatrix`/`NormalClip`/
`Square0`).

## GLOBAL BLOCKER: duplicated jump tables shift every address

`disc/field.bin` is the retail reference (size 260862, sha256 `38a1ce82…` =
the pin). Its first 0x1B9 bytes match `build/out/field.bin`; after that the
words diverge only in *value* (layout), not content — until the end of rodata.

- Retail rodata ends at file offset **0x2FC**; retail text starts there
  (`addiu sp,sp,-0x28`).
- Built rodata ends at **0x41C**; text starts there. The extra **0x120 bytes**
  are 4–6 C-generated jump tables (`func_8007BEF4`, `func_8007C694` ×2,
  `func_8007D3D4`, and one at 0x8008B9xx) emitted into `.rodata` by GCC.
- The retail tables already exist in `asm/field/data/0.rodata.s`
  (`jtbl_8006FB8C/FBAC/FBCC/FBEC`, `jtbl_8006FC88`, `jtbl_8006FD30/FDAC`) and
  are already linked in at their retail offsets. So the C tables are
  **duplicates**, and they shift text/data/bss by 0x120.
- Consequence: every `%hi/%lo` reference to any symbol resolves 0x120 (or more)
  away from retail, so **no function can be byte-exact until this is fixed**,
  regardless of per-function matching. This is the "jtbl literal-u32 era"
  recorded in ACTIVE_HANDOFF (July 11 entry).
- Extra detail: retail `jtbl_8006FB8C` has 8 entries (case 7 → `addiu
  s0,zero,-1` at 0x8007C244, bounds `sltiu v0,s2,8`); the C `switch(sideMask)`
  in `func_8007BEF4` currently expresses case 7 as `default`, so GCC emits a
  7-entry table with a bounds-to-default check. Same observable behaviour
  (sideMask is 3 bits) but a different table shape.

Fix approaches (none applied yet): (a) remove the jtbls from
`0.rodata.s` and make the C emit them at the retail offsets (requires the
linker-script rodata order to reproduce retail's interleave, plus exact case
sets); or (b) keep the asm tables and rewrite the C dispatch as a computed
`goto *((void**)jtbl_8006FB8C)[sideMask]` so GCC emits no table — but the
generated `sltiu/beqz/sll/addu/lw/jr` must still match retail.

### PROOF that this is the only remaining class of blocker

`func_8008D808` (misc.c) was rewritten to match retail's redundant reloads
(114 lines: `SCRIPT_READ_U8_REL(n) = v` per byte + per-call
`g_FieldScriptVMCurActor->scriptInstructionPointer`). Result: built is now
**127 instructions / 508 bytes = retail exactly**, and a raw word-by-word
compare of `disc/field.bin[0x1DD18..]` vs `build/out/field.bin[0x1B25C..]`
shows the *only* differing words are address immediates:

```
word  1: retail 8c420078 built 8c42cb98   lw v0,%lo(g_FieldScriptVMCurActor)
word  9: retail 8c42dc00 built 8c42a71c   lw v0,%lo(g_FieldScriptVMCurScriptData)
word 27: retail 0c0234b8 built 0c022a23   jal func_8008D2E0
```

Same opcode, same registers, same offsets — only the `%lo`/jump target differs,
because our layout is shifted 0x120 by the duplicated jump tables. So the
per-function matching work IS producing retail code; fixing the jump-table
duplication should convert a whole class of functions from "size-exact" to
"byte-exact". This makes the jump-table de-duplication the single highest-value
task.

**Committed:** no.
