### Latest native fidelity slice — 2026-09-19

Four battle effect progression callbacks (`800A3490`, `800A3514`, `800A3578`,
`800A35C8`) now have native bodies and verified scalar bridge dispatch.
Current build: **78 function stubs, 577 data placeholders, 96 adopted battle
leaves**. Retail-instruction differential checks pass O0/O2/UBSan; no natural
battle-effect observation is claimed. See
[the evidence](../evidence/battle-effect-callbacks-20260919/README.md).
Earlier numeric coverage claims below are historical, not current certification.

> **PROJECT GOAL:** a native PC port in the mould of Ship of Harkinian /
> the Silent Hill decomp ports, ultimately re-rendered (HD-2D).
> **Read `docs/ai_context/PORT_GOAL_AND_PLAN.md` before planning any work.**
> C coverage of executed code paths is the critical path; byte parity is a
> quality gate, not the deliverable. An `INCLUDE_ASM` body is worth nothing to
> the port. Anything still running through `battle_mips_runtime.c` can never be
> restyled.

















### world_map IS NOW SPLIT AND BUILDS — 98.5% byte-correct, one diagnosed defect left
`world_map.bin` (180,422 B, the largest module, previously **never
disassembled**: no yaml, no `asm/`, no `src/`) now splits, assembles, links and
builds at the exact retail size. **2,743 of 180,422 bytes differ (98.5%
correct).** New: `config/world_map.yaml`, `config/symbol_addrs.world_map.txt`,
and `world_map` in `gears.toml`.

Geometry (cross-checked against the port's own notes in
`pc_port/src/world_map_init.c`, which agree exactly — `func_80071B9C` in the
fresh split is the port's `wm_80071B9C`):

- loads at **vram 0x8006faf0**, sharing the overlay slot with `battling.bin`
- `WorldMapMain` 0x80070CFC = file offset 0x120C (the entry point, so code
  starts there)
- code ends after `func_80099BFC`; trailing data at D_80099E8C = offset 0x2A39C
- file end 0x8006faf0 + 0x2C0C6 = 0x8009BBB6, so bss starts 0x8009BBB8
- the render context `0x8009C620` (the "E190" blocker) is **in bss**, as is
  `0x8009BE4C`

**Three split rules learned, all confirmed by byte count:**

1. **The leading block must be `rodata`, not `data`.** `section_order` is
   `[".rodata", ".text", ".data", ".bss"]`, so a leading `data` subsegment
   links *after* the code and the overlay mismatches from byte 1 (retail offset
   0 is `0x00000005`; a data-first split emits `addiu $sp,$sp,-0x28` there).
   This single change took world_map from 154,742 differing bytes to 2,743.
   `movie.bin`, which matches retail exactly, uses `rodata` here.
   **`config/battling.yaml` still has the broken `data` shape — that is the
   root cause of its 88% byte difference, and fixing it is the cheapest large
   win available.**
2. **A non-multiple-of-4 overlay length needs a `databin` tail segment**, or
   the build comes out short (world_map was 2 bytes, battling 1, bcf1 4).
3. Trailing data must be its own `data` subsegment, not left inside the `asm`
   one — inside `asm` splat emits it with `dlabel`, which does not export, and
   the link fails on undefined `D_8009B564` / `D_8009BB48` / `D_8009A5B4`.

**The remaining 2,743 bytes — diagnosed, not guessed.** Every diff is a `%lo()`
field 8 bytes high on a **bss** symbol, e.g. at offset 0x148C retail is
`sw $v0, %lo(D_8009C894)($at)` (`0xac22c894`) and the build emits `0xac22c89c`
— `D_8009C894` is being linked at 0x8009C89C. So bss symbols are **allocated
by the linker**, not placed at their retail addresses. Nudging the bss start is
NOT the fix: 0x8009bbb0 is worse (2,907). The fix is to pin the bss symbol
addresses — they are all encoded in their own names — the way this repo already
pins `g_Heap` / `ApplyMatrixSV` / `D_800578D6`, or via
`config/symbol_addrs.world_map.txt`. That is the next step.

### Three unbuilt overlays wired into the build — movie.bin now MATCHES retail
`gears.toml` gained `battling`, `movie`, `battle_command_file1` (+192 KB of
retail code that no metric could see before). Two small blockers, both of a
class already solved elsewhere in the repo:

- `config/battling.yaml` carried `generate_asm_macros_files: False`, a
  newer-splat option the pinned 0.33.2 *rejects* — and gears ignores splat's
  nonzero exit, so the whole overlay split silently vanished.
  `config/battle.yaml` already documents this exact trap; copied its comment.
- `battling` then failed to link on two undefined data symbols, `D_80090F38`
  and `D_800925A4` — same class as slus's `D_800578D6`/`D_800578D8`, fixed the
  same way in the `Makefile` patch block.

Results: **`movie.bin` 50e1a9d9e08b == retail, byte-for-byte** (it is ~100%
INCLUDE_ASM, so this is ROM parity, not decompilation — 0/34 functions are C).
`battling` now links at 142,952 B vs retail 142,953. `battle_command_file1`
builds at 19,512 B vs 19,516.

**LEAD, highest ROI in the project:** `battling` is 483 functions and ~99%
INCLUDE_ASM, so its bytes *should* already be retail — yet 125,553 of 142,952
differ (88%). That is a **layout** bug (everything shifted), not a
decompilation gap; the same file-scope-asm-hoisting rule that forced the
mainc18 TU split is the first thing to check. Fixing it plausibly takes
battling from 0.02% to near-matching with no decompilation at all.
`battle_command_file1` is the same class (56% differ, 4 B short) and the
handoff history claims it matched exactly at one point — bisect that.

Byte gate now: **4 of 10 modules == retail** (battle, member_change_menu,
shop_menu, movie). Honest objdiff headline moved 27.46% -> **23.27%** because
the denominator grew by the newly-measured 192 KB; coverage hole fell 24.6% ->
**11.9%** (only `world_map` left).

### Progress measurement is now objdiff-based, and it found four coverage holes
`DASHBOARD.md` is no longer hand-maintained: `tools/scripts/progress_dashboard.py`
renders it from `build/progress.json` (`make report`). Two bugs made that
pipeline unusable before:

1. **The metric was inflated.** `gears report` passes `-DSKIP_ASM` so unmatched
   functions are absent from the compared object, but `include/include_asm.h`
   only guarded on `M2CTX`/`PERMUTER` — so `INCLUDE_ASM` still pasted the retail
   `.s` back in and every unmatched function read as byte-identical to target.
   Fixed. VERIFIED not to touch the matching build: `field.bin e91869555a1b88d6`
   / `battle.bin 1830b4ef1fe37129` identical with and without the edit.
2. **battle and menu were not in `tools/objdiff/config.yaml` at all** — ~480 KB
   of code, entirely unreported. Added (`battle/` with a trailing slash, or the
   prefix also swallows `battling/`).

Honest headline: **27.46% of code matched** (282,764 / 1,029,744 bytes),
55.01% of functions. Battle 8.33%, menu 7.36%, field 34.71%, slus 43.09%,
shop 70.67%, member_change_menu 100%.

**COVERAGE HOLES — the above is not a fraction of the whole game:**

- `gears.toml` builds only field, member_change_menu, shop_menu, menu, battle
  (+slus). **`world_map` (180,422 B), `battling` (142,953 B), `movie`
  (29,779 B) and `battle_command_file1` (19,516 B) are not built at all** —
  372,670 of 1,516,356 retail bytes (24.6%) are in neither the numerator nor
  the denominator. `world_map` has no `config/*.yaml` either, so it is not even
  split; it is the overlay behind the "World Rendering" card.
- **`config/checksum.sha` gates only 5 artifacts** — slus, field,
  member_change_menu, shop_menu, battle. `menu.bin` is built but **ungated**,
  and the four unbuilt overlays are absent. A module missing from that file is
  never checked, so "the gate is green" can never mean "the game matches".
- `build/out/field.bin` is 249,442 B against retail 260,862 B — field is
  **11,420 bytes short**, i.e. structurally incomplete, not merely mismatched.
- 68 of 325 objdiff units are raw asm segments with no C file at all (33,832 B,
  171 functions, 3.3% of the denominator, correctly counted as 0 matched).

`decomp_status.py` still reports `unported = 0` / "100% done" because its
classifier calls any `INCLUDE_ASM` inside `#ifndef XENO_PC_PORT` COEXISTENCE
without checking that a C body exists on the other branch. Measured: **717 of
906 `INCLUDE_ASM` sites have no C body anywhere** (battle 493/523, battling
167/169). Prefer the objdiff dashboard; treat that script as the port-stub view.

### Battle: fifty-third batch — main62/main63 timing family, all 5 PARKED (no source change banked)
The `main62`/`main63` family writes an opcode byte to `+0x5FA0` of the
`D_800C34B0` block and a count to the `D_800C3E50`'th word of the `+0x5F6C`
table. **Semantics are now pinned** (this is the durable result of the pass):

| body | opcode | count |
|---|---|---|
| `func_8009E278` | 2 | `D_800D2DC8[0x4F] * *(u32*)(D_800D2DC8+0x64) / 10` — **unsigned** (`multu`, `srl 3`) |
| `func_8009E2EC` | 2 | `D_800C3DFC[0x11] * *(u32*)(D_800D2DC8+0x64) / 20` — **unsigned** (`multu`, `srl 4`) |
| `func_8009E364` | 2 | `D_800C3E00[0x5B] * D_800C3DFC[0x11]` — plain product, no divide |
| `func_8009E410` | 0xA | `*(u16*)(D_800D2DC8+0x3A) * D_800C3DFC[0x11] / 20` — **signed** (`mult` 0x66666667, `sra 3`, `subu`) |
| `func_8009E48C` | 0xB | same as `func_8009E410` |

Divisor check, since it is easy to misread: `0xCCCCCCCD`+`srl 3` is /10 and
`srl 4` is /20; `0x66666667`+`sra 2` would be /10, and retail's `sra 3` is /20.
The pre-existing `func_8009E48C` coexistence body's `/ 20` is correct.

**Banked:** `func_8009E410` gained the coexistence body it never had (port-side
`#ifdef XENO_PC_PORT`, matching build still takes the retail `#else` asm) —
`battle.bin` re-verified `1830b4ef…`, 0 of 85984 words differing.

**Parked, all five:** each reproduces every retail instruction and differs only
in the destination register of the `mflo`/`mfhi` — retail `$v1`, every candidate
`$a1`. This is the *same* residual as the fifty-second batch's `func_8007AC80`,
so treat it as one open toolchain question, not five bugs. Register allocation
runs before `sched2` in this cc1, so the pseudo's live range at allocation time
spans the intervening byte store, which forces it off `$v0`/`$v1`. Shortening
the range does not help: computing the count inline at the store **grows** the
overlay by 16 bytes, and assigning the local after the byte store grows it by 8
(both re-materialise the symbol addresses). Do not re-walk those two; go at the
`mflo` destination directly, ideally with the permuter.

### Battle: fifty-second batch — the mainc18 operator family, 5 bodies (INCLUDE_ASM( 528 → 523)
Counts here are measured as `INCLUDE_ASM(` occurrences in `src/battle/*.c`; they
are not the body/stub metric the earlier batches quote, so don't chain them.

The whole of `src/battle/mainc18.c` (6 leaves, 17–23 instructions each) is one
family: each applies one arithmetic op to the 16-bit cell
`D_800D3420[index * 0x40][p[1]]`, where the board record `p = *ppBoard` carries
the cell index at `p[1]` and a little-endian operand at `p[2..3]`.

| body | size | op |
|---|---|---|
| `func_8007ABD8` | 0x58 | `cell += op`, saturate 0xFFFF |
| `func_8007AC30` | 0x50 | `cell -= op`, clamp 0 |
| `func_8007AC80` | 0x5C | `cell *= op`, saturate — **parked, 1 word** |
| `func_8007ACDC` | 0x48 | `cell /= op` (quotient) |
| `func_8007AD24` | 0x48 | `cell %= op` (remainder) |
| `func_8007AD6C` | 0x44 | `cell &= op` |

**Operand-order rule (new, and the whole batch hinged on it):** these five want
`(p[3] << 8) + p[2]`, which also sinks the `lbu p[2]` below the cell-address
computation. Flipping it took the batch from 60 differing words to 4. The
*compound-assignment* members of the same family — the already-landed
`func_8007ADB0`/`func_8007ADF4` and the new `func_8007AD6C` — are byte-exact
with the **opposite** order, `p[2] + (p[3] << 8)`. The family is not internally
consistent; try both orders before assuming a shape is wrong.

**Parked:** `func_8007AC80` reaches 22/23 words under every one of ~30 shapes.
The multiply's two halves are coupled — `cell * op` gives retail's
`mult $a0,$v0` but `mflo $a0` (retail: `mflo $v1`), while `op * cell` gives
retail's `mflo $v1` but reverses the `mult` operands. Permuter next.

Because file-scope `INCLUDE_ASM` is emitted ahead of every compiled body,
parking AC80 inside one TU hoists it in front of ABD8/AC30 and shifts the
overlay (measured: 51 differing words). Split with the existing `_q1`/`_q2`
pattern instead — `mainc18_q1` (0xB0E8), `asm` (0xB190), `mainc18_q2` (0xB1EC).

Gates: `battle.bin` `1830b4ef…` == retail, **0 of 85984 words differing**;
`member_change_menu.bin` and `shop_menu.bin` also == retail; field/menu
known-red, pins untouched. Port side NOT built and NOT run — the port has no
battle overlay at all (`func_80281204` still stubbed), so this is a matching
claim only. Evidence: `docs/evidence/battle-mainc18-operators-20260917/`.

**Environment note:** `make check` cannot complete — the slus link is red, and
that is pre-existing (a pristine `f67fe692` worktree fails it too). The battle
overlay links independently, which is what makes the byte gate usable. Two of
the working tree's slus undefined refs (`D_800578D6`/`D_800578D8`, from the
decompiled `psyq/libetc/intr.c`) are now patched into the generated linker
symbols by the `Makefile`, beside the existing `g_Heap`/`ApplyMatrixSV` hacks.

### Battle: fifty-first batch — func_800B9258 + func_800BE0DC (237 bodies, 617 stubs, 133 TUs)
Two small leaves, both 100% under CDK, picked from a fresh all-unmatched size
census (619 bodies; smallest are ~0x2C–0x44):

| body | size | notes |
|---|---|---|
| `func_800B9258` | 0x2C | `p = (u8*)(u32)D_800C3610[0]; if (p) *(u32*)(p + 0x34) += 1;` |
| `func_800BE0DC` | 0x2C | `p = (u8*)(u32)D_800D2D68[0]; if (p) func_800BDCF8(p);` |

`func_800BE0DC`'s callee is a landed body, and its slice is resident, so the
oracle executes the retail `func_800BDCF8` slice (no self-bridge).

Gates: `battle.bin` `1830b4ef…` == retail; rom-check member/shop/**battle PASS**
(slus/field known-red, pins untouched); port LINK OK 72 stubs `xeno-port`
`f808d261…`; Map0 x2 `RUN_RC=124` `primSubmits=2` `plain=7`; differential covers
fifty bodies — **checks=34210 PASS at O0/O2/UBSan**, forty-one mutants rejected.
Parked this pass: `func_800BB540` (9/63 — the `D_800C3666` address must be
materialised as `lui $a2` between `bit = 1` and `n = 3`; three shapes failed),
`func_800B15D8` (8/45) and `func_8007B9C8` (6/15, load/store order).

### Battle: fiftieth batch — func_800BFBA0 + func_800B8774 (235 bodies, 619 stubs, 132 TUs)
Two more store-fold bodies, both 100% under CDK:

| body | size | notes |
|---|---|---|
| `func_800BFBA0` | 0xE0 | guarded WDS fetch: `func_800B8354` → `ArchiveSetIndex(0x2C,0)` → `HeapAlloc(ArchiveDecodeAlignedSize(5),0)` → `ArchiveReadFileToBuffer(5, buf, 0, 0x80)` → `func_800B8354` → `SoundFindWdsEntry(*(u16*)(buf+0x20))`; when not found `func_800C0F70()`, `D_800C3A6C[0] = SoundLoadWdsFile(buf,0)`, `while ((func_8003BDFC(0) << 16) != 0) func_800BE790();`, `D_800C3620[0] = 1`, `D_800C3622[0] = 0`; shared `HeapFree(buf)` |
| `func_800B8774` | 0xCC | teardown: `if (*(u32*)(D_800C3EB0 + 0x8000 + 0xC84) == 0) func_800BE790();` → `DrawSync(0)` → `i = 0; n = 0xB; for (; i != n; i++) func_800BADD4(i);` → `GfxFreeWorkBuffers`, `WorkListsFreeAllEntries`, `func_8003852C(D_8005919C[0])`, `HeapFree(D_8005919C[0])`, `D_800591AD[0] = 0`, `DrawSync(0)`, `func_800A9F94`, `func_800A4820`, `HeapFree(D_800D2D54[0])` |

**Loop-shape rule (new):** `func_800B8774` only reproduces retail's
`s0 = 0; s1 = 0xB` (bound in a callee-saved register, `bne` instead of `slti`)
when the counter and bound are assigned as *separate statements in that order*
before a `for (; i != n; i++)` — writing `for (i = 0; i != n; i++)` saves one
register and swaps the two initialisers, and `i < 0xB` emits `slti`.

**Harness lesson:** the BFBA0 oracle first walked off the end of its slice
(`pc=0x800c07ec`, "instruction budget exhausted") because I had guessed the slice
length as 0xA0 — it is **0xE0** (next symbol `func_800BFC80`). Always take the
length from the map (start of the next symbol), not from the visible listing tail.

Gates: `battle.bin` `1830b4ef…` == retail; rom-check member/shop/**battle PASS**
(slus/field known-red, pins untouched); port LINK OK 72 stubs `xeno-port`
`f808d261…`; Map0 x2 `RUN_RC=124` `primSubmits=2` `plain=7`; differential covers
forty-eight bodies — **checks=32818 PASS at O0/O2/UBSan**, thirty-nine mutants
rejected.

### Battle: forty-ninth batch — func_800BEB04 (233 bodies, 621 stubs, 131 TUs)
`func_800BEB04` (0xC0, `src/battle/mainc119.c`) lands at **46/48 under CDK**; the
two residual "mismatches" are the splat listing notation
`lui $a1, (0x801FC000 >> 16)` / `ori $a1, $a1, (0x801FC000 & 0xFFFF)` versus
objdump's computed `lui $a1,0x801F` / `ori $a1,$a1,0xC000` — the encodings are
identical and the linked overlay is byte-clean, so the body is retail-exact.

Body: guard on `D_800591B2[0] != D_800591B3[0]`; when they differ it stores the
new value, runs `func_800B8354`, `ArchiveGetArchiveOffsetIndices(&a, &b)`,
`ArchiveSetIndex(0xC, 2)`, `ArchiveReadFileToBuffer(new + 2, 0x801FC000, 0, 0x80)`,
`func_800B8354`, `ArchiveSetIndex(a, b)`, `DrawSync(0)`, `Vsync(0)`,
`EnterCriticalSection()`, `FlushCache()`, `ExitCriticalSection()`; then
`D_800591B0[0] = 1` on both paths.

**Test lesson (third variant of the same class):** a stubbed callee that writes
through *pointer arguments* (`ArchiveGetArchiveOffsetIndices(&a, &b)`) must write
through the **bus** on the oracle side (`wr(NULL, c->gpr[4], 4, …)`) — calling the
host stub with guest stack addresses would corrupt host memory. Also: `DrawSync`
needed its own test definition because the landed body now keeps it alive (other
landed bodies with `DrawSync` had been dropped by `--gc-sections`).

Gates: `battle.bin` `1830b4ef…` == retail; rom-check member/shop/**battle PASS**
(slus/field known-red, pins untouched); port LINK OK 72 stubs `xeno-port`
`f808d261…`; Map0 x2 `RUN_RC=124` `primSubmits=2` `plain=7`; differential covers
forty-six bodies — **checks=28246 PASS at O0/O2/UBSan**, thirty-seven mutants
rejected. Parked this pass: `func_800B15D8` (8/45 — the source pointer must end up
in `$t0` with the destination symbol in `$a3`/`$a2`; three attempts all landed in
the wrong registers).

### Battle: forty-eighth batch — func_800BF9EC + func_800BF600 (232 bodies, 622 stubs, 131 TUs)
Two more store-fold bodies, both 100% under CDK:

| body | size | notes |
|---|---|---|
| `func_800BF9EC` | 0xB0 | guarded streamed-file fetch: `func_800B8354`, `ArchiveSetIndex(0x2C, 0)`, `HeapAlloc(ArchiveDecodeAlignedSize(1), 0)`, `ArchiveReadFileToBuffer(1, buf, 0, 0x80)`, `func_800B8354`, `func_8002DDE4(buf, 0,0,0,0,0,0)`, `func_800BE790`, `HeapFree(buf)`, clear `D_800C3621[0]` |
| `func_800BF600` | 0xCC | `a1` task gate at `+0x48` (early `func_800B7C34(a0); return`), `D_800C3CE8[0] = 0`, `+0xAF` non-zero → `func_80021BF8(task, func_800BF5E8)` + `func_800245D8(task, (s8)*(task+0xAF))`, `func_800B7C34(a0)`, then `while (D_800C3CE8[0] == 0) func_800BE790();` + `func_80021BF8(task, 0)` |

Decl-type collision again: the body's `func_800245D8(u8*, u32)` had to be typed
`(u32, u32)` to match the shared `CDK_DECLS` entry (same registers, same code).

Gates: `battle.bin` `1830b4ef…` == retail; rom-check member/shop/**battle PASS**
(slus/field known-red, pins untouched); port LINK OK 72 stubs `xeno-port`
`f808d261…`; Map0 x2 `RUN_RC=124` `primSubmits=2` `plain=7`; differential covers
forty-five bodies — **checks=25878 PASS at O0/O2/UBSan**, thirty-five mutants
rejected. Parked this pass with data: `func_800B1F0C` (0/24 — the OT argument must
be saved to `$t0` before the two work-buffer loads), `func_800BF2B8` (32/39 —
only the last two field load/store orders differ), `func_800BFDA8` (5/40 → **26/40**
after the array rule; the `D_8006BE10+0x10` base still folds to `$at`).

### Battle: forty-seventh batch — func_800B8D04 + func_800B8840 (230 bodies, 624 stubs, 131 TUs)
Two more store-fold bodies, both 100% under CDK:

| body | size | notes |
|---|---|---|
| `func_800B8D04` | 0x78 | `func_800B8354(); func_800BF9EC(); while (D_80059464[0] != func_800BF720()) func_800BE790(); func_800BF3A4();` then `HeapFree(D_800C3618[0])` + clear when non-zero |
| `func_800B8840` | 0x84 | `D_800591AD[0] = 1; D_80059464[0] = 0; D_800591AC[0] = 0; D_800591A8[0] = 0x2000;` then `func_800BED30`, `func_800BE108`, `WorkListsReset`, `func_800BB7F8`, `GfxAllocateWorkBuffers(0x5000, 0)`, `func_800BCD8C`, `func_800B7C28`, `func_800B89F4`, `D_80050104[0] = 0;` — `D_800591A8` must be **`u32[]`** (retail `sw`, not `sh`) |

**Harness hazard hit and fixed (same class as the earlier `func_8008FA60` bit):** I
added a bridge for `func_800BCD8C`'s *own entry address* so `func_800B8840`'s
oracle could call the landed body — but the adapter consults the bridge at every
PC, so the BCD8C oracle check then called the **host C** instead of the retail
slice and the `store_bcd8c` mutant silently stopped being rejected (checks still
passed). The six self-bridges (`func_800BED30`, `func_800BE108`, `func_800BB7F8`,
`func_800BCD8C`, `func_800B7C28`, `func_800BF3A4`) are removed; their slices are
already resident in guest RAM, so the oracle executes real retail code.
Rule: only bridge a callee whose slice is *not* used as an oracle entry.

Gates: `battle.bin` `1830b4ef…` == retail; rom-check member/shop/**battle PASS**
(slus/field known-red, pins untouched); port LINK OK 72 stubs `xeno-port`
`f808d261…`; differential covers forty-three bodies — **checks=22448 PASS at
O0/O2/UBSan**, thirty-three mutants rejected. Map0 offscreen ×2: run 2 `RUN_RC=124`
`primSubmits=2`, run 1 aborted once with a **port-side** `VectorNormal: retail
signed ADD overflow` assert (rc=134); three further runs (3–5) are `RUN_RC=124`
with no assert, and the `xeno-port` binary is byte-identical to the baseline that
has passed Map0 for the last several batches, so this is an intermittent
pre-existing field-path diagnostic, not a regression from these battle-only
changes — worth a separate look, not a blocker for this batch.

### Battle: forty-sixth batch — nine more store-fold bodies (228 bodies, 626 stubs, 129 TUs)
Another straight pass down the store-fold census; all nine are 100% under CDK
(PSY-Q ≈ 0 on the same drafts):

| body | size | notes |
|---|---|---|
| `func_800BF3A4` | 0x44 | guarded `func_800C1140(D_800C3618[0])` then clear the flag |
| `func_800B3C2C` | 0x48 | three removes/frees, `D_800C3560[0] = 0;` in the `func_800A6F98` delay slot |
| `func_800BB7F8` | 0x4C | four global stores, `func_800BC2F0(0)` |
| `func_800BB690` | 0x50 | `func_800A9540(*(u32*)(p+0x1C))`, `D_800C3CB8[0]++`, rebind `func_800BB620` |
| `func_800BC404` | 0x50 | guarded `func_800BC2F0(1)`/`func_800BC460(p)`, then `D_80059454[0] = D_800C3CDC[0]` |
| `func_800BF354` | 0x50 | returns the `func_800C0FAC` result; `u32 r;` stays uninitialised on the already-set path (retail returns `$v1`) |
| `func_800BF998` | 0x54 | `s16 a = (s16)(D_800D2D4C[0]+1);` gives retail's `sll`/`sra 16` pair; `func_800A9FF0(0xB)` at `a == 2` |
| `func_800BCB54` | 0x60 | node flag `*(u8*)(*(u32*)(s0+0x1C)+0x2B) \|= 1`, removes, free, clear |
| `func_800BEBC4` | 0x54 | **pointer must be computed after** `func_800BEC18()` (before it, cc1 keeps it in `$s0` and the frame/order diverge) |

Gates: `battle.bin` `1830b4ef…` == retail; rom-check member/shop/**battle PASS**
(slus/field known-red, pins untouched); port LINK OK 72 stubs `xeno-port`
`f808d261…`; Map0 x2 `RUN_RC=124` `primSubmits=2` `plain=7`; differential covers
forty-one bodies — **checks=19800 PASS at O0/O2/UBSan**, thirty-one mutants
rejected. Test-side rule reconfirmed: any pointer taken from a *fixture slot* must
be a host pointer for the C run and the matching guest address for the oracle
(`func_800BCB54`'s two-level fixture, `func_800B16F0`'s base).

### Battle: forty-fifth batch — ten more bodies from the store-fold list (219 bodies, 635 stubs, 123 TUs)
Continued straight down the store-fold census with the array rule; all ten are
100% under CDK where PSY-Q was ~0:

| body | size | shape | shape rule used |
|---|---|---|---|
| `func_800B8048` | 0x0C | 3/3 | `u32 D_800C3E1C[]` store |
| `func_800B7134` | 0x2C | 11/11 | `D_800C3CB4[0] = g_GfxCurOT[0]` then call |
| `func_800B16F0` | 0x30 | 12/12 | `u8 n = p[1];` **before** the counter update, else the load is hoisted |
| `func_800B8068` | 0x30 | 12/12 | two calls then `D_800591B1[0] = 1` |
| `func_800B3588` | 0x38 | 14/14 | `TimerWorkListRemoveTask(p); HeapFree(p); D_800C3548[0] = 0;` |
| `func_800B383C` | 0x3C | 15/15 | `WorkListRemoveTask(p+0x1C)` + `D_800C3558[0] = 0` |
| `func_800BDCF8` | 0x3C | 15/15 | `D_800D2D68[0] = 0;` first, then the two removes |
| `func_800C0F70` | 0x3C | 15/15 | kept `$s0` address + guarded `SoundFreeWdsEntry` |
| `func_800B397C` | 0x44 | 17/17 | 5th parameter is `u8` (`lbu`), not `u32` |
| `func_800BEDE8` | 0x44 | 17/17 | `HeapFree(D_800C3610[0])` + three clears |

**Decl-type conflicts are the new failure mode.** `CDK_DECLS` is emitted into
every CDK TU, so a callback declared `(void)` there collides with the same symbol
landing as `(u8*)` (`func_800B3588`): the fix is to type the shared decl to match
the real body and stop re-declaring it per body. Test-side corollary: once a
former stub lands it must be deleted from `pc_port/tests` (the linker keeps the
TUs' definitions), and every *called* stub needs a bridge case — the new
`func_800B7160`/`func_800B7C34`/`func_800B7E94`/`func_800B39C0`/`WorkListRemoveTask`/
`TimerWorkListRemoveTask`/`HeapFree`/`SoundFreeWdsEntry` bridges were missing at
first and the oracle silently ran into zero-filled memory (surfacing as a bogus
`waddCalls` mismatch).

Gates: `battle.bin` `1830b4ef…` == retail; rom-check member/shop/**battle PASS**
(slus/field known-red, pins untouched); port LINK OK 72 stubs `xeno-port`
`f808d261…`; Map0 x2 `RUN_RC=124` `primSubmits=2` `plain=7`; differential covers
thirty-two bodies — **checks=11600 PASS at O0/O2/UBSan**, twenty-six mutants
rejected.

### Battle: forty-fourth batch — the `$at`-store family is SOLVED (209 bodies, 645 stubs, 118 TUs)
**Correction to the "root-caused, keep `INCLUDE_ASM`" verdict below:** the store
family was never toolchain-blocked — it needed the *array declaration* rule
(above) **plus** the CDK cc1. Declaring the target as `extern <T> SYM[];` and
writing `SYM[0] = …` makes cc1 keep the address in a general register and emit
`lui rt,%hi(SYM)` + `<st> …,%lo(SYM)(rt)`, which is exactly retail's shape.
Every member tried lands 100% under CDK where PSY-Q 2.7.2 scores ~0:

| body | size | PSY-Q | CDK |
|---|---|---|---|
| `func_800B7C28` | 0x0C | 0/4 | **3/3** |
| `func_800BCD8C` | 0x0C | 0/4 | **3/3** |
| `func_800BF730` | 0x0C | 0/4 | **3/3** |
| `func_800BC454` | 0x0C | 0/4 | **3/3** |
| `func_800BC3F8` | 0x0C | 0/4 | **3/3** |
| `func_800B8054` | 0x14 | 0/6 | **5/5** |
| `func_800BE108` | 0x14 | 0/6 | **5/5** |
| `func_800BED30` | 0x1C | 0/8 | **7/7** |
| `func_800B9B30` | 0x24 | 1/10 | **9/9** |

(B9B30 is the kept-*pointer* variant: `extern u8* D_800C3610[];` then
`D_800C3610[0][0x48] = 1; D_800C3610[0][0x49] = D_800C3610[0][0x1C];` — two
separate `D_800C3610[0]` reads reproduce retail's double `lw`.)

The per-body `extern` declarations live *inside* the generated body text
(immediately after the `/* func_X.s */` marker) rather than the shared
`CDK_DECLS` block, because the same symbol is typed differently in different
bodies (`u32 D_800C3610[]` in BED30 vs `u8* D_800C3610[]` in B9B30) and
`CDK_DECLS` is emitted into every CDK TU.

**Consequence for the census:** the 86 unmatched battle bodies with a non-`$at`
store fold, and the 24 with a delay-slot symbol store, are *candidates*, not
blockers; re-draft them with the array rule (and CDK) before assuming anything
about the toolchain.

Gates: `battle.bin` `1830b4ef…` == retail; rom-check member/shop/**battle PASS**
(slus/field known-red, pins untouched); port LINK OK 72 stubs `xeno-port`
`f808d261…`; Map0 x2 `RUN_RC=124` `primSubmits=2` `plain=7`; differential covers
twenty-two bodies — **checks=5560 PASS at O0/O2/UBSan**, eighteen mutants
rejected (`store_7c28`, `store_bcd8c`, `arg_bf730`, `zero_b8054`, `copy_b9b30`
added; each setter check seeds its global, runs the retail slice, resets, runs
the shipped C and compares the touched bytes).

### Battle: forty-third batch — func_800B35C0 + the array-declaration rule (200 bodies, 654 stubs, 109 TUs)
`func_800B35C0` (0x98, `src/battle/mainc76.c`) sat in the parked
"symbol address kept in a callee-saved register" family (`lui $s1,%hi(D_800C3548)`
then `lw`/`sw …%lo($s1)` across the branch). It is now **38/38 under CDK** with
one textual change: declare the slot as an **array** and index it —

```c
extern u32 D_800C3548[];          /* not `extern u32 D_800C3548;` */
...
if (D_800C3548[0] == 0) { ... D_800C3548[0] = (u32)s0; }
else { s0 = (u8*)(u32)D_800C3548[0]; ... }
```

**Shape rule (new, general — the second big recipe of this session):** a scalar
global that retail accesses through a kept base register has to be declared as an
**array** and indexed (`G[0]`), otherwise cc1 materialises the address per access
(`$at` macro form). This also explains `func_800BF5E8` (`D_800C3CE8[0]++`, landed
in batch 42) and should be tried first for the rest of the parked list
(`func_800BB760`, `func_800BDE58`, `func_800BB7F8`, `func_800B3C2C`,
`func_800C0F70`, …). Applied to `func_800BB760` it produces the kept-`$s1`/`$s2`
prologue and lifts the body from 7/38 to 8/39, i.e. the structure is right but
the save/schedule order still differs; `func_800BDE58` needs its guard load
hoisted above the frame first (retail), so it is not a pure array fix.

Body: `func_800B35C0` returns the shared `D_800C3548` slot, allocating
`TimerWorkListAllocateTask(0, 0x24)` + rebinding `func_800B3358` /
`func_800B3588` and zeroing `+0x24/+0x26/+0x28` when empty, otherwise copying
`+0x1C/+0x1E/+0x20` into those halfwords.

Gates: `battle.bin` `1830b4ef…` == retail; rom-check member/shop/**battle PASS**
(slus/field known-red, pins untouched); port LINK OK 72 stubs `xeno-port`
`f808d261…`; Map0 x2 `RUN_RC=124` `primSubmits=2` `plain=7`; differential now
covers thirteen bodies — **checks=3368 PASS at O0/O2/UBSan**, thirteen mutants
rejected (`size_35c0` = the alloc size `0x24`→`0x28`; the 35C0 check exercises
both the allocate path, comparing the whole task buffer and the recorded
callbacks, and the reuse path, comparing the copied halfwords).

### Battle: forty-second batch — four bodies from the draft sweep (199 bodies, 655 stubs, 108 TUs)
Instead of hand-picking the next target, re-scored **every** existing draft under
the CDK harness (`t*.c` in `/var/tmp/xeno-harness`, 200 drafts with a resolvable
function name) against its PSY-Q score and landed the ones that hit 100%:

| body | size | PSY-Q | CDK | TU |
|---|---|---|---|---|
| `func_800BF5E8` | 0x18 | 1/7 (14%) | **6/6** | `src/battle/mainc104.c` |
| `func_800B6438` | 0x2C | 8/11 (73%) | **11/11** | `src/battle/mainc82.c` |
| `func_8007ADF4` | 0x44 | 10/17 (59%) | **17/17** | `src/battle/mainc18.c` |
| `func_800B16A4` | 0x4C | 18/19 (95%) | **19/19** | `src/battle/mainc74.c` |

Bodies: `D_800C3CE8[0]++` (note: `extern u32 D_800C3CE8[]` — the **array**
declaration is what makes the CDK cc1 keep `lui $v1,%hi` across the load/store
pair, resolving the old "hand-written asm" verdict); `WorkListSetTaskCallback(
(u8*)(u32)*(u32*)(p+0x6C) + 0x1C, D_80025A88)`; `row[p[1]] ^= p[2] + (p[3]<<8)`
with `row = D_800D3420 + (index<<6)`; and the node walk `q = (u8*)(off + (u32)p)`
summing `(q[0]+1)*4` while stepping by `(q[1]+1)*4` `n` times.

**Technique worth repeating:** `/var/tmp/xeno-harness/draft-scores.txt` is the
full `psx=<n>/<d> cdk=<n>/<d>` table for all drafts; regenerate it with the CDK
harness (run each harness with `</dev/null` — otherwise the child processes eat
the `while read` loop's stdin and the sweep stops after one line).

Gates: `battle.bin` `1830b4ef…` == retail; rom-check member/shop/**battle PASS**
(slus/field known-red, pins untouched); port LINK OK 72 stubs `xeno-port`
`f808d261…`; Map0 x2 `RUN_RC=124` `primSubmits=2` `plain=7`; the differential now
covers twelve bodies — **checks=2944 PASS at O0/O2/UBSan**, twelve mutants
rejected (`inc_bf5e8`, `off_b6438`, `xor_7adf4`, `mul_b16a4` added; 7ADF4
compares the whole `D_800D3420` row, B16A4 the returned sum, B6438 the recorded
`WorkListSetTaskCallback` task/callback).

### Battle: forty-first batch — func_800B73A0 (195 bodies, 659 stubs, 104 TUs)
`func_800B73A0` (0x4C, `src/battle/mainc87.c`) was parked at 13/19
("only the argument-setup order differs"). With the CDK compiler it is 17/19,
and the two remaining "mismatches" are the splat listing notation
`lui $a0, (0x10F7C >> 16)` / `ori $a0, $a0, (0x10F7C & 0xFFFF)` versus objdump's
computed `lui $a0,0x1` / `ori $a0,$a0,0xF7C` — the encodings are identical
(checked word-by-word), so the body is retail-exact and the build's byte compare
is the arbiter.

Body: `func_800B7424(WorkListsAddTasks(0x10F7C, 0, func_800B6F0C,
func_800B7134, func_800B7364))` — the three callback addresses go through
locals, which is what produces retail's `lui`/`addiu` placement around the call.

**Test-harness rules re-confirmed (worth reusing):** a landed callee must not be
defined again in a `pc_port/tests` stub (link error). Where the test must replace
a landed body (`func_800B7424` here) the runner weakens it with
`objcopy --weaken-symbol=…` on the TU object; and any *kept* landed body drags in
its own callees (`func_800B7364` needs `WorkListRemoveTask`,
`TimerWorkListRemoveTask`, `func_80025180`), so those get no-op stubs.

Gates: `battle.bin` `1830b4ef…` == retail; rom-check member/shop/**battle PASS**
(slus/field known-red, pins untouched); port LINK OK 72 stubs `xeno-port`
`f808d261…`; Map0 x2 `RUN_RC=124` `primSubmits=2` `plain=7`; differential now
covers eight bodies — **checks=1952 PASS at O0/O2/UBSan**, eight mutants
rejected (`size_73a0` = `0x10F7C`→`0x10F80`).

### Battle: fortieth batch — func_800B64D4 (194 bodies, 660 stubs, 103 TUs)
`func_800B64D4` (0x44, `src/battle/mainc80.c`) was parked for a long time
(1/17) with "cc1 folds `D_800C3EB0 + 0x8C8C` into an `$at` address where retail
adds the constant at runtime". It is now C and **17/17 under the CDK cc1**:

```c
void func_800B64D4(u8* a0, u8* a1) {
    u8* base = (u8*)(u32)D_800C3EB0;      /* <- the fix: opaque base value   */
    u8* p = base + (a1[1] << 2) + 0x8C8C; /*    keeps the add at runtime     */
    func_800245D8(*(u32*)p, a1[0]);
}
```

**Shape rule (new, general):** routing a symbol through an intermediate pointer
variable (`u8* base = (u8*)(u32)SYM;` then `base + CONST`) stops cc1 folding the
constant into the relocation. The first `addiu` of the address and the constant
then stay separate and the constant is materialised with `li` → `ori`/`addiu`,
matching retail's runtime `addu`. Writing `SYM + CONST` inline folds it
(measured both ways on this body and on `func_800BB9D4`).

**Near-miss banked:** `func_800BB9D4` (0xE4) reaches **49/57** with the same
trick; the residue is scheduler/allocator only — retail stores `+0x38` before
`+0x34` and reuses `$a1` for `D_800C354C[2]`, ours keeps the C statement order
and uses `$v1`/`$a1` the other way round. Rejected: swapping the two statements
(43/57).

Gates: `battle.bin` **`1830b4ef…` == `disc/battle.bin`** (cmp clean);
rom-check member/shop/**battle PASS** (slus/field known-red, pins untouched);
port LINK OK, 72 stubs, `xeno-port` `f808d261…`; Map0 x2 `RUN_RC=124`
`primSubmits=2` `plain=7`. The differential
`run_battle_cdk_task_spawners_retail_test.sh` now also covers 64D4 (bridge for
`func_800245D8`, table fixture at `D_800C3EB0+0x8C8C`): **checks=1512 PASS at
O0/O2/UBSan**, seven mutants rejected (`idx_64d4` = `0x8C8C`→`0x8C90`).

### Battle: batches 38–39 — six CDK bodies landed (193 bodies, 661 stubs, 102 TUs)
The CDK compiler found last pass is now a first-class build preset, and six
verified bodies are C:

| body | retail | TU | shape (CDK) | shape (PSY-Q 2.7.2) |
|---|---|---|---|---|
| `func_800B5924` | 0x98 | `src/battle/mainc75.c` | 38/38 | 34/38 |
| `func_800B56E4` | 0x48 | `src/battle/mainc75.c` | 18/18 | 15/18 |
| `func_800B5C18` | 0xA8 | `src/battle/mainc76.c` | 42/42 | 39/42 |
| `func_800B5DC4` | 0x30 | `src/battle/mainc77.c` | 12/12 | 1/13 |
| `func_800BDC78` | 0x80 | `src/battle/mainc78.c` | 32/32* | — |
| `func_800BF7C8` | 0x94 | `src/battle/mainc95.c` | 37/37 | 33/37 |

\* the two "mismatches" are the splat `lui $v1, (0xC0000 >> 16)` listing
notation vs objdump's `lui $v1,0xC` — same encoding, linked byte compare is the
arbiter.

5924/5C18/F7C8/572C-style task spawners call
`TimerWorkListAllocateTask(*(u32*)(a0+0x6C), size)` →
`TimerWorkListSetTaskCallback`, then the
`+0x1C/+0x20/+0x24/+0x28/+0x2C/+0x30` field copies; 5924/5C18 return the task,
5C18 also tail-calls `func_800B5B3C`, 5924/BF7C8 set
`*(u32*)(a0+0xAC) |= 0x20`. 56E4 forwards `*(u32*)(*(u32*)(a0+4)+0x38)` to
`func_8001E148` then `func_800C08CC(5, q+0x78, func_800B51B0)` (the callback
address must be passed inline — materialising it in a local before the first
call puts the `lui` in the wrong block). 5DC4 sets `*(u16*)(a0+0x34) = 1` then
rebinds `func_800B5CC0`. DC78 runs `func_800BDF1C`, adds
`0xC0000`/`0xFFF40000` to `+0x48` depending on `+0x7C`, counts `+0x88` down and
on negative resets it to `0x10` and rebinds `func_800BDC14`.

**Tooling (kept):**
- `gears.toml` gained `[[build_preset]] BattleCdk` — `gcc = "gcc-2.7.2-cdk-psx"`,
  `maspsx_flags = "--dont-expand-li --use-comm-section --run-assembler"`,
  `paths = ["battle/mainc"]`. The CDK cc1 materialises a call argument's symbol
  address as `lui %hi` before the call with `addiu %lo` in the jal delay slot
  (a0 copy first); PSY-Q 2.7.2 emits one `la` that gas expands in place.
- `tools/scripts/gen_battle_tus.py` is the vendored copy of the battle TU
  generator (was only in `/var/tmp/xeno-harness/gen2.py`) and now has a third
  flavour class: `CDK_SET`/`CDK_DECLS` + `mainc<N>` run names; a run may not mix
  `mainc`/`mainl`/`main` flavours because the `li` expansion is TU-wide.
- `config/battle.yaml` (102 `[0x…, c, main…]` entries) and
  `pc_port/build_port.sh` `REFERENCE_ONLY_GAME_TUS` (102 `src/battle/main*.c`)
  were regenerated from the generator's `yaml_entries.txt` / `tu_names.txt`;
  `linker/battle.ld` was moved aside and regenerated by `make build`.
- Shape rule: pointer fields and pointer reads use the repo's
  `(u32)`/`(u8*)(u32)` convention — `*(u32*)(s0 + 0x1C) = (u32)s1;` and
  `u8* q = (u8*)(u32)*(u32*)(p + 4);`. Matching bytes are identical (pointers
  are 32-bit there) and the host build stays UBSan-clean — an 8-byte `u8*`
  store tripped UBSan on the 4-byte-aligned field and would corrupt the port
  struct layout.

**New differential:** `pc_port/tests/run_battle_cdk_task_spawners_retail_test.sh`
(+ `battle_cdk_task_spawners_retail_test.c`). It loads the three retail slices
from `disc/battle.bin` (sha `1830b4ef…`; slice shas `9b682d4c…`/`44119b00…`/
`5d1d554a…`) into the MIPS adapter, bridges `TimerWorkListAllocateTask`
(0x8001CD08), `TimerWorkListSetTaskCallback` (0x8001CD6C), `func_800B57E4`,
`func_800B5B3C` to host stubs, and compares eight seeds of the return value,
every recorded callee argument, the callback address (retail
0x800B5854/0x800B5B3C/0x800BF73C vs the host symbols), the task buffer words
and the actor bytes against the shipped C: **checks=744 PASS at O0/O2/UBSan**,
3 mutants rejected (`size_5924`, `off_5c18`, `mask_f7c8`).

Gates: `battle.bin` **`1830b4ef…` == `disc/battle.bin`** (cmp clean);
rom-check member/shop/**battle PASS**, slus `61b675d2…` / field `761df27a…`
known-red (pins untouched); port LINK OK, 72 stubs, `xeno-port` `f808d261…`
unchanged; Map0 offscreen x2 `RUN_RC=124` `primSubmits=2` `plain=7`.

**Next CDK candidates** (bodies whose jal delay slot holds `addiu r,r,%lo(…)`,
i.e. the same compiler signature — 19 found, 3 landed here, 16 left):
`func_800B35C0`, `func_800B39C0`, `func_800B3F04`, `func_800B81BC`,
`func_800B89FC`, `func_800B9508`, `func_800B9F78`, `func_800BB760`,
`func_800BB9D4`, `func_800BC158`, `func_800BCC60`, `func_800BCD98`,
`func_800BDE58`, `func_800BE790`, `func_800BF600`, plus `func_800B572C`
(drafted: body transcribes but lands 24/46 — callee-saved roles come out
`a0→$s2 a1→$s0 res→$s1` instead of retail `a0→$s1 a1→$s2 res→$s0`; parked).

**Differential:** `pc_port/tests/run_battle_cdk_task_spawners_retail_test.sh`
now covers all six bodies — slices `9b682d4c…` (5924), `44119b00…` (5C18),
`5d1d554a…` (F7C8) plus 5DC4/56E4/DC78 read from the same `disc/battle.bin`,
with bridges for `TimerWorkListAllocateTask`/`SetTaskCallback`,
`func_800B57E4`, `func_800B5B3C`, `func_800BDF1C`, `func_8001E148`,
`func_800C08CC` and per-side callback-address checks: **checks=1248 PASS at
O0/O2/UBSan**, six mutants rejected (`size_5924`, `off_5c18`, `mask_f7c8`,
`flag_5dc4`, `arg_56e4`, `cd_dc78`; the DC78 mutant is the `0x10` countdown
reset). DC78 runs both countdown paths across the eight seeds
(`callback branch seeds=4`).

**Rule probe for the rest of the CDK list (2026-09-11):** the remaining
candidates split into two parked families, both re-confirmed with the CDK cc1:
1. *Symbol address kept in a register across calls* (`lui $sX,%hi(SYM)` then
   `lw/sb …,%lo(SYM)($sX)` — retail `func_800BB760`, `func_800BDE58`,
   `func_800B35C0`, and most of the list). Plain load/store C, `volatile`
   globals, a local `p = &G` pointer, and `*p` forms all rematerialise the
   address through `$at`; the CDK cc1 only keeps it for RTL shapes reachable
   through constructs we have not found (or via ASPSX-only behaviour). Probe
   source: `/var/tmp/xeno-harness/cdkrule{,2}.c` (v1…v5, w1…w4 all fold).
2. *Callee-saved role rotation* (`func_800B572C`: body transcribes, 24/46 —
   `a0→$s2 a1→$s0 res→$s1` instead of retail `a0→$s1 a1→$s2 res→$s0`).
The two clean spawners left are `func_800B39C0` (0x1AC) and `func_800BB9D4`
(0xE4, no kept `$s` address) — neither drafted yet.

**Later additions to the same list:** `func_800BB9D4` is now 49/57 (scheduler /
allocator residue only, see batch 40) and `func_800B0AB4` improved 4/24 → **7/24**
by moving the `p[2]` load inside the innermost block (the old draft hoisted it);
the remaining residue is which caller-saved register holds the `D_800658C8`
pointer (`$a1` in retail vs `$v1` in ours) and the matching cascade — same
allocation family, parked.

### Battle: `$v0`-store family root-caused (toolchain-level, no body landed)
Ran the `$at`/`$v0` symbol-store family to ground this pass. The blocker is
**not** a missing C shape: the retail battle overlay contains 88 unmatched
functions whose symbol stores were assembled from an *explicit*
`lui rt,%hi(SYM)` + `<st> …,%lo(SYM)(rt)` address (rt a general register),
and our cc1 never emits that form.

**Census (from the retail listings, `asm/<overlay>/**`; pattern A = store
through `lui rt,%hi(SYM)` + `%lo(SYM)(rt)` with rt != `$at`; pattern B = a
symbol store sitting in a jump delay slot):**

| overlay | funcs | A | B | A∪B | A∩B | landed with A or B |
|---|---|---|---|---|---|---|
| battle | 857 | 86 | 24 | **88** | 22 | **0** |
| field | 884 | 0 | 0 | 0 | 0 | 0 (field is fully C) |
| slus_006.64 | 1226 | 3 | 0 | 3 | 0 | 0 |

Examples: `func_800B7C28` (`lui $v0,%hi(D_800D2FDC); jr $ra; sb $zero,%lo($v0)`),
`func_800BF730` (`sw $a0,%lo(D_800C3628)($v0)` in the `jr` slot),
`func_800BED30`, `func_800BB760`, `func_800B8054`, `func_800BE108`.

**Mechanism evidence:**
1. `tools/maspsx/aspsx/ASM/V0_AT.S` + `test_v0_at.py` (real ASPSX vectors):
   `lw $2,SYM` / `sw $2,SYM2` → ASPSX emits `lui v0; lw v0,0(v0); nop;
   lui at; sw v0,0(at)`. So ASPSX expands *macro* stores with `$at` — exactly
   like the gas passthrough in our build. A retail `$v0`/`$v1`/`$a0`-based
   symbol store therefore cannot come from the macro form; its `.s` carried an
   explicit `%lo(SYM)(reg)` address.
2. All three shipped cc1s (`gcc-2.7.2-psx`, `gcc-2.6.0-psx`,
   `gcc-2.7.2-cdk-psx`) print the *macro* form (`sh $0,SYM`, `sw $4,SYM`) for
   every C shape tried this pass: scalar/array/struct/union members, pointer
   locals, `volatile`, `register` pointers, integer/pointer casts,
   `*(u8*)((u32)&D)`, loops, `-G0`/`-G8`, `-fpic`/`-fPIC`, `-mno-abicalls`,
   and reading `*t` twice. None produces `%lo(SYM)(reg)`.
3. GCC 2.7.2.3 `config/mips/mips.c` only prints the register+symbol form for
   `(plus (reg) (symbol_ref))` addresses (the "pretend address mode" note in
   `GO_IF_LEGITIMATE_ADDRESS`), and the `la` printer emits `la` (→ `lui` +
   `addiu`), never a bare `lui` + `%lo(reg)` store. `legit`/`movsi` paths were
   re-checked against the fetched 2.7.2.3 source.
4. Consequence for our build: maspsx passes an explicit `%lo(SYM)(reg)` store
   through untouched (so the pattern *is* reachable if a compiler emits it),
   but no cc1 we have does. `as`'s macro expansion always picks `$at`.

**Conclusion:** these 88 battle bodies (≈13% of the remaining 667) are not
C-expressible with the three in-repo cc1s (2.7.2-psx, 2.6.0-psx, 2.7.2-cdk)
for every C shape tested; either the original overlay's setters are asm-authored,
or an assembler (ASPSX) delay-slot expansion is responsible. They stay
`INCLUDE_ASM` (already byte-exact) and should not be re-litigated without a new
cc1 or an assembler-side delay-slot rule.

### Battle: the CDK compiler is the missing piece for the callback family
**`tools/gcc-2.7.2-cdk-psx` materialises a function/symbol address as
`lui %hi` + `addiu %lo` split around the call**, where `gcc-2.7.2-psx` emits a
single `la` that gas expands in place. That is exactly the residue of the parked
TaskCallback family. Measured this pass with the CDK cc1
(`/var/tmp/xeno-harness/tryc_cdk.sh` = same pipeline, `gcc-2.7.2-cdk-psx` +
`--dont-expand-li`):

| body | PSY-Q 2.7.2 | CDK | note |
|---|---|---|---|
| `func_800B5C18` (0xA8) | 39/42 | **42/42** | `u8* f(u8* a0, u8* a1)`: `TimerWorkListAllocateTask(*(s32*)(s1+0x6C), 0x18)`, callback `func_800B5B3C`, then `+0x1C/+0x20/+0x2C/+0x30/+0x24/+0x28` copies, tail-call, return |
| `func_800BF7C8` (mainl95) | 33/37 | **37/37** | draft `/var/tmp/xeno-harness/t64_BF7C8.c` |
| `func_800B5924` (main75) | 34/38 | **38/38** | draft `/var/tmp/xeno-harness/t65_B5924.c` |
| `func_800B56E4` | 14/18 | 0/20 | CDK is *worse*; draft is not the right C yet |

The overlay is a **mix** of compilers, so this must be a per-TU preset:
`src/battle/main.c` compiles byte-identically under 2.7.2-psx and CDK except
`func_80071A8C` (CDK rewrites its clear loop), and `src/battle/main75.c` is
byte-identical under both. `XenoGraphics` already uses `gcc-2.7.2-cdk-psx` for
slus `animation_scripts`, so the precedent exists.

**Landing recipe (not yet executed — the bodies are verified, the TU plumbing is
the remaining work):**
1. `gen2.py`: add a third flavour class (`CDK_FUNCS = {func_800B5C18,
   func_800BF7C8, func_800B5924}`), make the run-merge flavour test compare the
   class (not a bool), name those runs `mainc<N>`, and add a decl block
   (`extern u8* TimerWorkListAllocateTask(s32, s32); extern void
   TimerWorkListSetTaskCallback(u8*, void*); extern void func_800B5B3C(u8*);`).
2. `gears.toml`: `[[build_preset]] name = "BattleCdk"` with
   `gcc = "gcc-2.7.2-cdk-psx"`, `maspsx_flags = "--dont-expand-li
   --use-comm-section --run-assembler"`, `paths = ["battle/mainc"]`.
3. Draft the bodies as `RUN_T41*.c`, run `gen2.py`, then replace the whole
   `config/battle.yaml` `[0x…, c, main…]` block and the
   `REFERENCE_ONLY_GAME_TUS` list in `pc_port/build_port.sh` from
   `yaml_entries.txt` / `tu_names.txt`, move `linker/battle.ld` aside, `make
   build`, and require `battle.bin` `1830b4ef…` again.
4. **Do not hand-insert a C body mid-TU.** File-scope `__asm__` (INCLUDE_ASM)
   blocks are emitted before compiled bodies, so a mid-range C body moves to the
   end of the TU and shifts everything after it (measured: 1654 differing bytes
   starting at 0x800B447C, because `battle/main75.` also switched the whole TU
   to the CDK+literal preset). That edit was reverted and `battle.bin` is
   `1830b4ef…` again.

**Also parked with data (medium sweep, this pass):**
- `func_800BCBB4` (0xAC, main91): full body transcribes and reaches **26/43**
  under the `addiu` (`--dont-expand-li`) flavour; the residue is pure callee-saved
  assignment — retail `p→$s0, a→$s1, q→$s2` vs ours `p→$s1, q→$s0, a→$s2`, plus
  retail's `$a0` copy of `q` for the two byte stores and the `addiu $v1,$v0,0x1000`
  temp. `u32` parameter + `(u8*)` cast is what reproduces retail's
  `addu $v0,$a0,0; …; addu $s0,$v0,0` chain; five load/decl orderings all gave
  the same (wrong) register roles.
- `func_800BE6E8` (0xA8, main93, digit-string builder over `D_800C37A4[]`): body
  transcribes (7/42 nominal, alignment-sensitive) but cc1 merges the loop's
  `q = value / *t` and `value = value % *t` into one `divu` + `mflo`/`mfhi`,
  while retail has two separate divisions in two blocks; also needs the `addiu`
  flavour for `0x2D`/`1` and `count = 0` before the stack-arg load.

Gates: no source changes this pass (investigation only) — `battle.bin`
`1830b4ef…` == `disc/battle.bin` unchanged; worktree untouched. Census + evidence
in `/var/tmp/xeno-harness/store-family-census-20260911.txt`.

### Battle: thirty-seventh batch — func_800BE11C (187 bodies, 667 stubs, 96 TUs)
Landed the object tick, transcribed from its retail listing and byte-compared:
it advances the `0x3C` halfword by `0x64`, adds `0x330` to each of the
`0x40/0x44/0x48` words, wraps the three `0x68/0x69/0x6A` bytes through
`func_80021AD8(v, -4)`, and when the `0x60` countdown reaches zero tail-calls
the object's `+0xC` handler with the object as argument. 42/42 shape match; the
linked build confirms it byte-exact.

**Shape rules:** the three `func_80021AD8` results are stored through
`*(u8*)(s + 0x68/69/6A) = (u8)…` (a struct-field assignment makes cc1 reload
differently), and the countdown must be written explicitly
(`*(s32*)(s + 0x60) = *(s32*)(s + 0x60) - 1;` then `if (… == 0)`) so the store
lands in the `bnez` delay slot.

**Parked this pass:** `func_800BF2B8` (29/39 — the two `D_800C3618/361C` stores
fell to the `$at` family) and `func_800BFDA8` (5/40 — needs the
`D_800591AC`-save/restore shape plus the `func_80023B84` tail), plus
`func_800BD974` at 23/42 (the `ReadGeomOffset`/`ReadGeomScreen` output stores
are typed through `long` in retail).

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (187 bodies, 667
`INCLUDE_ASM`, 96 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: thirty-sixth batch — func_800B9B54 (186 bodies, 668 stubs, 95 TUs)
Landed the actor angle re-bind, transcribed from its retail listing and
byte-compared: when `a != b` it runs the `func_800BEF24` + `func_800223B0`
and `func_800BEF24` + `func_80021FE0` pairs on `a`, then — unless `b`'s `0xAF`
byte is `0x15` — the mirrored pairs on `b`, each result truncated through
`(s16)`. 43/43 shape match under `--dont-expand-li`; landed in `mainl87`; the
linked build confirms it byte-exact.

**Shape rule:** the `(s16)` truncation of the helper's return is what produces
retail's `sll v0, 16; sra a1, v0, 16` pair — writing the value through an
`s16` temporary or a cast to a wider type loses it.

**Parked this pass:** `func_800BB760` (7/38) and `func_800BED4C` (5/39) —
both need the `lui`-base pointers held in *callee-saved* registers across
their calls (`s1`/`s2` in retail) so the symbol stores never go through `$at`;
the plain/local-pointer forms all rematerialise. This is the same
register-caching family as `func_800BB7F8` and `func_800B3C2C`.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (186 bodies, 668
`INCLUDE_ASM`, 95 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: thirty-fifth batch — func_800BA8F4 (185 bodies, 669 stubs, 94 TUs)
Landed the SVECTOR resolve/rotate body, transcribed from its retail listing and
byte-compared: builds an SVECTOR from the object's `+2/+6/+0xA` halfwords,
resolves it through `func_800A5914` (falling back to `func_800A579C` on a
negative result), passes it to `func_800A5870`, then writes the handle back at
`+0x78` and the possibly-updated `vy` at `+0x84`. 36/36 shape match; landed in
`mainl87`; the linked build confirms it byte-exact.

**Shape rules that closed the last 3 diffs (33/36 → 36/36):**
1. Each halfword must go through a *reused `s32` temporary* per field
   (`t = *(s16*)(s + 2); *(s16*)((u8*)&v + 0) = (s16)t;`) — assigning the
   `s16` straight into the struct field makes cc1 narrow the load to `lhu`,
   while the temporary forces retail's sign-extended `lh`.
2. The two tail writes are ordered `+0x84` first, then the `+0x78` handle.
3. The `li 4` constant needs the addiu flavour (mainl TU).

**Parked this pass:** `func_800BF7C8` (34/37) and `func_800B5924` (35/38) — the
same `TaskCallback` family as `func_800B56E4`: retail copies the task pointer
into `a0` *before* materialising the callback address, cc1 does the `la`
first. `func_800BDB74` needs the `s1 = s0` self-copy before its indirect call
to reproduce the 0x20 frame.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (185 bodies, 669
`INCLUDE_ASM`, 94 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: thirty-fourth batch — func_80071A8C (184 bodies, 670 stubs, 93 TUs)
Landed the row clear, transcribed from its retail listing and byte-compared:
it zeroes `D_800D3725[i]` for `i` from `0x2A0` down to 0 in `0x60` steps,
clears `D_800D2D28[0xB5]` / `[0xB4]` and runs `func_800716D8()`. 21/21 shape
match; it merged into the same `main.c` run as `func_80071A08`/`71A38` (the
decl block gained `D_800D3725`), and the linked build confirms it byte-exact.

**Parked this pass:** `func_800BB7F8` (6/19 — the `$at` symbol-store family
again: retail materialises the base in `v1` before the constant), and
`func_800B3C2C` (10/19 — the `D_800C3560 = 0` store needs to sit in the
`func_800A6F98` delay slot with a base register). `func_800B56E4` improved to
16/18 under `--dont-expand-li`; the residue is a two-instruction delay-slot
swap (`addiu a1, s0, 0x78` vs the callback's `addiu a2, a2`), and hoisting the
pointer into a local made it worse (15/18).

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (184 bodies, 670
`INCLUDE_ASM`, 93 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: thirty-third batch — func_80071A38 (183 bodies, 671 stubs, 93 TUs)
Landed the three-byte mirror, transcribed from its retail listing and
byte-compared: `D_800D2D28[i + 0x7C] = D_800C3EAC[i + 0x2EB]` for `i < 3`,
then `func_800716D8()`. 21/21 shape match; it merged into the `func_80071A08`
run in `main.c` (the decl block gained `D_800D2D28` and `func_800716D8`), and
the linked build confirms it byte-exact.

**Shape rule:** retail keeps the *base* first in both `addu`s, which needs the
pointer form `(D_800D2D28 + i)[0x7C] = (D_800C3EAC + i)[0x2EB]` — plain
`D_800D2D28[i + 0x7C]` makes cc1 emit `addu v1, v0, a0` (index first).

**Parked this pass (with findings):** `func_800B6808` (0x128 — the whole
Square0/SquareRoot0/ratan2/RotMatrix/ApplyMatrix body transcribes cleanly and
scores 61/74; the residue is entirely delay-slot/statement scheduling);
`func_800B64D4` (1/17 — cc1 folds `D_800C3EB0 + 0x8C8C` into an `$at` address
where retail adds the constant at runtime); `func_800B0AB4` (4/24 — cc1
hoists and retypes the second `lh`); `func_800B73A0` (13/19 — only the
argument-setup *order* differs, all instructions are present);
`func_800B62C8` joins `func_800B626C`/`func_800B61F8` as a **stack-switch
thunk** (`sw $sp, 0(t0); addiu t0, -4; addu $sp, t0`) — not C-expressible.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (183 bodies, 671
`INCLUDE_ASM`, 93 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: thirty-second batch — func_800B6CEC (182 bodies, 672 stubs, 93 TUs)
Landed the vector→angle body, transcribed from its retail listing and
byte-compared: the three `>> 8` components of `(s + 0xC/10/14)` into a VECTOR
(`vz == 0` bumped to 4), `Square0`, `len = SquareRoot0(r.vx + r.vz)`, then
`-ratan2(vz, vx)` into the object's `+2` halfword, `ratan2(vy, len)` into `+4`,
`+0` cleared and `0x10000000` OR'd into `(s + 0x3C)`. 52/53 on the harness —
the only diff is the `lui v1, (0x10000000 >> 16)` listing artefact — and the
linked build confirms it byte-exact.

**Two shape rules:** (1) `len` must be an `s32`/`long`, not `s16` — the
halfword type makes cc1 re-sign-extend it (`sll s0, 16`) before the second
`ratan2` where retail passes `s0` straight through; (2) each of the three
object stores re-dereferences `*(u32*)(s + 0x20)`, matching retail's reload
per store (a cached `s16*` cost an extra saved register and 0x40+ frame).
The `vz == 0 → 4` constant keeps it in the addiu-flavoured `mainl80` run.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (182 bodies, 672
`INCLUDE_ASM`, 93 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: thirty-first batch — func_800B6DC0 (181 bodies, 673 stubs, 93 TUs)
Landed the sub-object zeroer, transcribed from its retail listing and
byte-compared: with `p = *(u32*)(s + 0x20)` and `q = *(u32*)(p + 0x34)`, it
zeroes the eight 8-byte entries `q[i * 8]` (two bytes, then three halfwords)
and the two flag bytes at `p + 0x3C/0x3D`. 49/49 shape match under
`--dont-expand-li`; the linked build confirms it byte-exact in `mainl80`.

**Two shape rules:** (1) each of the five stores re-dereferences the whole
chain — writing the expression inline per store reproduces retail's repeated
`lw 0x20(a0)` / `lw 0x34(v0)` loads, and the scaled index must come first in
integer space (`*(u8*)((s32)(i << 3) + (s32)q + off)`) or cc1 swaps the
`addu` operands; (2) the `i != 8` bound materialises as `addiu` in retail, so
the body only matches in an addiu-flavoured TU (added to `LIT_FUNCS`, landing
in `mainl80`).

**Parked this pass:** `func_800B6E84` (33/34 — the single `addu a2, a1, zero`
zero-copy, the same family as `func_800B16A4`), `func_800B6518` (the four
stack halfword slots with signed→unsigned type-pun reloads — a `volatile`/
union-shaped source), `func_800B61F8` and `func_800B626C` (both stack-switch
thunks that overwrite `$sp`; not C-expressible), plus `func_800B6464`,
`func_800B69E4`.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (181 bodies, 673
`INCLUDE_ASM`, 93 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: thirtieth batch — the ratan2 angle pair (180 bodies, 674 stubs, 92 TUs)
Landed `func_800B6C44` and `func_800B6C98` (0x54 each), transcribed from their
retail listings: yaw from `(s + 0x10)` / `(s + 0x14)` against `(s + 0xC)`,
both shifted right 8 and passed to `ratan2`, stored into the object's `+4` /
`+0` halfword, then `0x10000000` OR'd into `(s + 0x3C)`. Both hit 20/21 on the
harness — the single "diff" is the listing artefact `lui v1, (0x10000000 >>
16)` vs the decoded `lui v1, 0x1000` — and the linked build confirms both
byte-exact, so the artefact is cosmetic in this case.

**Shape rule:** the object pointer (`*(u32*)(s + 0x20)`) must be loaded *after*
the `ratan2` call. Computing it up front makes cc1 keep it in a callee-saved
register across the call (0x20 frame with `s1`); declaring the pointer at the
top but assigning it after the call reproduces retail's 0x18 frame with only
`s0`.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (180 bodies, 674
`INCLUDE_ASM`, 92 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: twenty-ninth batch — func_800B639C (178 bodies, 676 stubs, 91 TUs)
Landed the stream-advance wrapper, transcribed from its retail listing and
byte-compared: it advances by `(s8)p[1] << 8 | p[0]`, then calls
`func_800B3CD4(p[5], p[3], p[4], (s8)p[0], (s8)p[1], (s8)p[2])` — three
unsigned bytes in the argument registers, three signed bytes on the stack.
21/21 shape match; the linked build confirms it byte-exact.

**Shape rule:** the function takes an unused first parameter (retail receives
the stream pointer in `a1`, so the source signature is
`(u32 unused, u8* p)`); declaring only `(u8* p)` moves the pointer into `a0`
and shifts every load.

**Parked:** `func_800B69E4` (0x6C — the `+=` sibling of the decoder pair: the
instruction sequence matches but retail keeps the pair in `v1` and the old
halfword in `v0`, the mirror of the `func_800B6990` allocation), plus
`func_8008A144`/`func_8008A3EC`.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (178 bodies, 676
`INCLUDE_ASM`, 91 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: twenty-eighth batch — func_800B6990 closes the decoder pair (177 bodies, 677 stubs, 90 TUs)
Landed `func_800B6990` (0x54) — the `sh` sibling of `func_800B6930`: same
advance pair, three signed big-endian 16-bit fields, but stored as halfwords.
21/21 shape match; it merged into the `func_800B6930` run, and the linked build
confirms it byte-exact.

**Shape rule (resolves the previous turn's register swap):** the pair must be
built from two *declared-at-the-top* locals — `s32 hi; u32 lo;` — with
`hi = *(s8*)(p + 1); lo = p[0];` then `(hi << 8) | lo`, one pair per store.
Writing the expression inline makes cc1 narrow early (`lbu` + `sll 24` +
`sra 16`); assigning hi first *inside* the statement (a C89 declaration after
statements) does not compile with the 2.7.2 front end.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (177 bodies, 677
`INCLUDE_ASM`, 90 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: twenty-seventh batch — func_800B6930 (176 bodies, 678 stubs, 90 TUs)
Landed the caller-stream decoder, transcribed from its retail listing and
byte-compared: reads `(s8)p[1] << 8 | p[0]` as the advance, then decodes three
signed big-endian 16-bit fields into the destination words' high halves at
`+0`, `+4`, `+8`. 24/24 shape match; the linked build confirms it byte-exact.

**Sibling parked with data — `func_800B6990` (0x54):** the same walker but its
three stores are `sh` halfwords. Writing the fields through a `u16*` makes cc1
narrow early (`lbu` + `sll 24` + `sra 16`) instead of retail's `lb` + `lbu` +
`sll 8` + `or`; computing each pair into a `s32` local first recovers the right
instruction *sequence* (12/21) but leaves retail's `v0`/`v1` roles swapped, so
it stays parked.

**Also parked this pass:** `func_8008A144` (0x130, the four-`LoadImage`
texture scroll): the raw transcription reaches 59/76 — the important finding
is that `D_800D2D28 + 0x30` is a **u32** field (`lw`/`sw`/`sltiu` in retail,
not `lbu`), and that each `LoadImage` re-dereferences both `D_800C3EA4` and
`D_800D2D28` (retail saves no registers — a cached `u8*` local forces a
0x20 frame with `s0`/`s1` where retail has 0x18 with only `ra`). The residual
is register allocation/ordering.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (176 bodies, 678
`INCLUDE_ASM`, 90 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: twenty-sixth batch — func_8008A274 (175 bodies, 679 stubs, 89 TUs)
Landed the `D_800C3E4C` state-1 handler, transcribed from its retail listing and
byte-compared: the optional `func_8007171C`, `func_80089CCC(0)` when the
argument is zero, the `D_800D2D28` `0xA9`/`0xAB` bumps, the `D_800C3EA4`
`+0x6410` scroll with its `0x7C` / `4` bounds and `+0x6416` direction flag,
the `D_800D39D4` 1/3-up 2/4-down switch on `D_800D3288`, then
`func_8008A144`. 94/94 shape match; the linked build confirms it byte-exact.

**Shape rule:** the `+0x6410` scroll field must be read as `s32` (a `u32`
field makes cc1 emit `sltiu` for the `>= 0x81` test where retail has signed
`slti`, and the second branch's `bgez` disappears).

**Parked with a full decode — `func_8008A3EC` (0x290, the state-2 handler):**
two nested loops. (1) `done = 1; muted = 0;` then
`while (done & 0xFF)`: if `ControllerGetType(0) != 0` → when `muted != 0`
`SoundEnableAllSpuChannels()` + `D_80059488 = vol`, else `done = 0`; if
`ControllerGetType(0) == 0` and `muted == 0` → the two
`GraphicsDrawPauseLetters(0x88, 0x64/0x144)`, `SoundMuteAllSpuChannels()`,
`muted++`, `vol = D_80059488`. (2) `func_8008A144()`, then when
`D_800D2D28[0xCA] != 0` the `i < 0x10`, stride `0x38` loop over `D_800D3278`
that decrements `+0x26` (u16) when `+0x28 != 0` and clamps at zero via the
sign-extended low half. (3) the `do { … } while (D_800C3444 != 0)` pop loop
with the `D_800D3014 = 0xFF` refresh, `ControllerResetState()` on a
re-appearing controller, the `D_8005917C` `-1` / press-4+8 → `D_800C48EA = 1`
release path, the released-0x20 → `D_800D3014 = 4` path, and the released
0x800 + `D_800CCC58` mute path. A first transcription matched 37/166 — the
remaining work is branch inversion (`bnez` vs `beqz` on the first test) and
`vol`'s declaration/initialisation order.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (175 bodies, 679
`INCLUDE_ASM`, 89 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: twenty-fifth batch — three 0x8008 bodies (174 bodies, 680 stubs, 88 TUs)
Landed, each transcribed from its retail listing and byte-compared:
`func_800800E8` (clears the `D_800D2D28` `0xAD/0xC7/0xA8` bytes, downgrades
`D_800D32A1[(index & 0xFF) * 8]` from 2 to 1, frees `D_800D2DB4`),
`func_80080BD0` (sets the `D_800C3EAC + 0x2DB` flag, then while the `0x2D3`
state is below 3 and `D_800C204C` is clear runs the
`func_8007FCE8`/`FDEC`/`800E8`/`FB70` chain) and `func_8008A9C0` (the
`D_800C3E4C` switch onto `func_8008A684`/`A274`/`A3EC`). 30/30, 39/39 and
32/32 shape matches; the linked build confirms all three byte-exact.

**Run merge bookkeeping (again):** `func_80080BD0` is adjacent above the landed
`func_80080B64`, and `func_8008A9C0` sits *below* the landed `func_8008AA40` /
`func_8008AA74`, so both runs changed heads — the decl blocks had to move to
`func_80080B64` (adding `D_800D3278`, `D_800C204C`, the four call externs) and
to the new `func_8008A9C0` key (adding AA40/AA74's `D_8005919C`,
`func_80039DB8`, `D_800D366C`).

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (174 bodies, 680
`INCLUDE_ASM`, 88 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: twenty-fourth batch — three 0x8008 bodies (171 bodies, 683 stubs, 88 TUs)
Landed, each transcribed from its retail listing and byte-compared:
`func_80089AF8` (the eight-call overlay chain), `func_8008FA60` (clears
`D_800D2D28[index + 0xB0/0xB8]`, runs `func_800716D8` and frees the
`D_800D2E38` / `D_800D2D90` slot entries) and `func_80080B64` (stamps
`0xFE` / the argument into the `D_800C402F` / `D_800C400B` rows at
`(*D_800C3EAC)[0x2DA] * 72`, then clears `D_800D366C`). 22/22, 30/30 and
27/27 shape matches; the linked build confirms all three byte-exact.

**Harness collision (fixed):** `func_8008FA60` was one of the tiny-leaves
test's *log stubs*, so landing the real body broke the differential link
(`multiple definition`). The stub was deleted (the bridge now runs the shipped
body) and the test gained the two fixtures the real code needs
(`u32 D_800D2E38[0x100]; u32 D_800D2D90[0x100];`). This is the same pattern as
the earlier `func_80089C9C`/`func_8008AA40` cases: **a landed body must not
also exist as a test stub or bridge stub.**

**Shape rule (`func_8008FA60`):** the `D_800D2D28` byte clears only reproduce
retail's `addu v0, v0, s0` (base first) when written in integer space —
`*(u8*)((int)(unsigned int)p + i + 0xB0) = 0;`.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (171 bodies, 683
`INCLUDE_ASM`, 88 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: twenty-third batch — two more 0x8009 leaves (168 bodies, 686 stubs, 85 TUs)
Landed, each transcribed from its retail listing and byte-compared:
`func_8009A074` (mirrors the low 3 bits then the `0x7F8` bits of the
`D_800C34B0 + 0x5FAE` halfword into `+ 0x5FAC` when the target is non-zero —
the global must be re-dereferenced between the two blocks) and
`func_8009B098` (initialises slots 1 and 2 of the `D_800C34B0` block:
`0x64/0x64` halfwords, `0x14`/`0x0F` bytes, `0x1FBF` halfword, `0x149` byte).
26/26 and 27/27 shape matches; the linked build confirms both byte-exact.

**Parked from this sweep:** `func_8009E364` (23/25 — the product's `mflo`
lands in `a1` instead of retail's `v1`; fixing the pointer order with the
scaled-index-first form closed the other diff), `func_8009B104` (7/56),
`func_8009C134` (13/25).

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (168 bodies, 686
`INCLUDE_ASM`, 85 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: twenty-second batch — two 0x8009 bodies (166 bodies, 688 stubs, 83 TUs)
Landed, each transcribed from its retail listing and byte-compared:
`func_8009D354` (the `func_8009DBFC(0) << 24` gate that stamps state 6 into
`D_800C34B0[D_800C3E50 + 0x5FA0]`) and `func_8009CB68` (sets/clears bit `0x20`
of the `D_800CCD8C[-0xA4 + slot]` halfword at `+0x84` from bit `0x200` of the
`D_800CCD8C` slot's `+0x80`). 19/19 and 23/23 shape matches; the linked build
confirms both byte-exact.

**Shape rule (`func_8009CB68`):** the `-0xA4` base must be built as a *pointer*
then adjusted (`u8* q = base; q -= 0xA4; r = (u16*)((u32)(unsigned int)q +
off);`). Folding it into the offset register (`(u32)(base - 0xA4) + off`) puts
the `addiu -0xA4` on `v0` instead of retail's `a0`.

**Parked from the same range:** `func_8009B104` (the ×50 clamp store: retail
keeps the 99999 constant in `a2` and materialises a second copy in `v0` for the
second store, but cc1 schedules the `D_800C34B0` load and the `lhu` differently
— best 7/56), `func_8009C134` (bit-set loop: retail's single `ori a3, zero, 3`
vs cc1's two constant materialisations — best 13/25).

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (166 bodies, 688
`INCLUDE_ASM`, 83 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: twenty-first batch — func_8009AEFC (164 bodies, 690 stubs, 81 TUs)
Landed `func_8009AEFC` (0xDC): clears the `D_800C34B0 + 0x5FC2` latch, resets
the `(index * 368)` slot's `0x7C/0x84/0x88/0x8C` halfwords keeping only the
`0x2000` bit of `0x80`, calls `func_8009B104(index & 0xFF, slot)` for state 7,
and for state 3 walks slots 3..0xA clearing bit `0x20` in the `D_800CCD68`
rows. 55/55 shape match; the linked build confirms it byte-exact.

**Shape rules from this body:** the latch store has to come *before* the
`index * 368` computation (retail loads the global pointer, stores, then
reloads for the row base); the state byte must be read into a `u32` (a `u8`
temporary makes cc1 re-mask it and shifts the tail by an instruction); and the
counted loop needs an explicit masked condition (`(u32)(i & 0xFF) < 0xB`) to
reproduce retail's `andi; sltiu` pair.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (164 bodies, 690
`INCLUDE_ASM`, 81 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: twentieth batch — func_8007D148 and the buffered-gather family map (163 bodies, 691 stubs, 80 TUs)
Landed `func_8007D148` (the `D_800D3420` halfword at `p[2]` split into the
caller buffer at `(off * 8 + p[1])` and `+1`, with the board pointer reloaded
for the second offset). 24/24 shape match; the linked build confirms it
byte-exact. Its second store only reproduces retail when written as
`dst += o + (*ppBoard)[1]; dst[1] = …;` — folding the whole address into one
`dst[…]` expression puts the pointer in the wrong register.

**Family map for the remaining `func_8007A628` bodies** (so the next session
does not re-derive it): the ones that match are the plain *scan* forms
(`func_8007C4A0/C580/C678/C75C/D6A8/D7B4` — landed). The ones that do **not**
are the *buffered gather* forms (`func_8007CB20/C33C/C840/CC50/CD10/CEA4/CFB8`,
`func_8007D8C0/DA1C/DB78/DCF8/DE78/DFD4`): they march a stack `u8 buf[8]` and
keep a separate cursor + counter, and cc1 both re-orders the prologue
(`buf`/`i`/`off` register assignment) and rotates the loop differently
(retail keeps `i++` in the `beqz` delay slot and the byte offset in the
loop-back delay slot; cc1 emits nops or unrolls by 2). Eight declaration
permutations and a do-while rewrite were tried on `func_8007CB20` — best 19/76
— so treat this shape as a distinct parked family.

**Also confirmed this batch:** the `$v0`-store family (`func_800B3588/B383C`,
12/14 and 13/15) is not source-expressible — absolute store, integer cast,
array form, `volatile`, and an out-of-line temporary all produce cc1's
`lui $at; sw $zero` instead of retail's `lui $v0; sw $zero, sym($v0)`.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (163 bodies, 691
`INCLUDE_ASM`, 80 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: nineteenth batch — three more slot choosers (162 bodies, 692 stubs, 80 TUs)
Landed, each transcribed from its retail listing and byte-compared:
`func_8007C580` (slots 3..0xA over the signed `D_800D2E0C` halves, skipping the
`index + 3` slot), `func_8007D6A8` (the `D_800D32A1`-gated scan over slots 0..2
against the 32-bit `D_800CCDEC` row) and `func_8007D7B4` (the same gate over
slots 3..0xA against the 16-bit `D_800CCD34` row). All three stamp
`func_80089C08(best)` into the `D_800D3420` row. 62/62, 67/67 and 67/67 shape
matches; the linked build confirms them byte-exact.

**Prologue-order rule (again):** the `+3`-offset variants (`func_8007C580`)
only reproduce retail's prologue when the offset is computed *before* the
marching table pointer (`s32 skip = (index & 0xFF) + 3;` then
`u8* s1 = D_800D2E0C;`), otherwise `lui s1`/`addiu s1` come first and the
whole epilogue shifts.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (162 bodies, 692
`INCLUDE_ASM`, 80 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: eighteenth batch — the D_800CCD34 slot choosers (159 bodies, 695 stubs, 80 TUs)
Three *medium* bodies landed, each transcribed from its retail listing and
byte-compared: `func_8007C678` and `func_8007C75C` (scan slots 0..2 / 3..0xA,
keep the smallest `D_800CCD34` halfword among the `func_8007A628`-accepted
ones, then stamp `func_80089C08(best)` into the `D_800D3420` row) and
`func_8007C4A0` (the same scan over slots 0..2 against the signed
`D_800D2E06` halfword, keeping its low byte). 57/57, 57/57 and 56/56 shape
matches; the linked build confirms all three byte-exact.

**Shape rules for this family:** the winning-slot scan needs the row base
materialized into a local (`u16* row = (u16*)(D_800D3420 + rowOff);`) — writing
the whole address inline makes cc1 fold it into a symbol-relative `$at` store
that retail does not have; and the marching-table variant only reproduces
retail's prologue when the loop counter is initialized *before* the table
pointer (`s32 i = 0; u8* s1 = D_800D2E06;`) so `sw s0` precedes `lui s1`.
`func_8007CC50` (the slot-match counter) stays parked: retail keeps the
`i++`/`slti` pair in the branch-delay slots and cc1 emits nops there.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (159 bodies, 695
`INCLUDE_ASM`, 80 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: seventeenth small-leaf batch — 2 more leaves (156 bodies, 698 stubs, 78 TUs)
Landed, each transcribed from its retail listing and byte-compared per function:
`func_8007BB2C` (the halfword board-row fields written into `D_800CCE3E` at
the `(index + 3) * 368` slot — the value must be built *before* the slot
arithmetic or cc1 emits the `+3` chain first) and `func_800B6BFC` (the
`0x140 x 0xE0` RECT handoff to `MoveImage(0x2C0, 0x100)` then
`func_800B73A0`; addiu-flavoured, so it lands in `mainl65`).

**Landing mechanics that bit this time:** `func_8007BB2C` is adjacent *below*
the landed `func_8007BB70`, so it became the run head and its `T6_DECLS` entry
(moved from the `func_8007BB70` key) had to carry all seven `D_800CCE3x`
arrays; and `func_800B6BFC` needs `RECT`/`MoveImage` declared in its own decls
block because `common.h` does not pull in `libgpu.h`.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (156 bodies, 698
`INCLUDE_ASM`, 78 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: sixteenth small-leaf batch — 3 leaves, incl. the first 0x8009 body (154 bodies, 700 stubs, 77 TUs)
Landed, each transcribed from its retail listing and byte-compared per function:
`func_8007B198` (the `D_800D3420` halfword row product clamped at 0xFFFF into
the `p[3]` slot), `func_80097D08` (the descending `0xB..0` sweep stamping
`0xFF` into `D_800C34B0[+0x5FA0]` and clearing the matching `0x5F6C` word) and
`func_8009C0E0` (the `index * 368` `D_800CCE30` / `D_800CCE4A` stamps plus
`g_GameState + 0x16DA |= 0x4000`).

**Two more shape rules:** the descending sweep needs the *scaled index first*
in integer space (`*(u32*)(((u32)i << 2) + (s32)(unsigned int)ptr + off)`) or
cc1 commutes the `addu`; and `func_8009C0E0`'s `lhu`/`sh` pair only comes out
as `lui/addiu` when the flag is written through a `u16*` local
(`u16* p = (u16*)(g_GameState + 0x16DA); *p |= 0x4000;`). The listing shows a
symbol addend there; the linked build confirms the encoding is identical, so a
one-line harness mismatch on `addiu ..., %lo(sym+add)` is not a real diff —
the `battle.bin` byte compare is the arbiter.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (154 bodies, 700
`INCLUDE_ASM`, 77 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: fifteenth small-leaf batch — 4 row clamp/copy bodies (151 bodies, 703 stubs, 76 TUs)
Landed, each transcribed from its retail listing and byte-compared per function:
`func_8007B0C8` (the `D_800D3420` halfword row sum clamped at 0xFFFF into the
`p[3]` slot), `func_8007B134` (the same row difference clamped at zero),
`func_8007BB70` / `func_8007BBD8` (the three board-row bytes copied into
`D_800CCE3x` at the `(index + 3) * 368` slot, with the board pointer reloaded
per field).

**Two shapes that pin the 368-slot helper:** the slot must be `u32 slot =
index + 3;` — a `u8 slot` masks *after* the `+3` and the two instructions come
back swapped; and the row-clamp bodies need `s32` arithmetic so cc1 picks
`slti` (signed) rather than `sltiu`, matching retail's clamp branches.

**Port-list bug fixed:** the `REFERENCE_ONLY_GAME_TUS` rewrite regex required a
suffix after `main`, so `"src/battle/main.c"` was never inside the replaced
range and got re-inserted on every landing (three copies by now). The pattern
is now `^    "src/battle/main[^"]*\.c"$` and the list is verified equal to the
generator's `tu_names.txt` (76 entries) after every regeneration.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (151 bodies, 703
`INCLUDE_ASM`, 76 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: fourteenth small-leaf batch — 7 row-family leaves (147 bodies, 707 stubs, 76 TUs)
Landed, each transcribed from its retail listing and byte-compared per function:
`func_8007A828` (byte stamp into the caller buffer, the `D_800D3430` row probe
and the `+1` wrap), `func_8007AFFC` / `func_8007B040` / `func_8007B084` (the
`D_800D3430` byte row AND / OR / XOR into the `p[3]` slot), `func_8007B208` /
`func_8007B264` (the `D_800D3420` halfword row division and remainder into the
`p[3]` slot) and `func_8007BC40` (two byte stamps into the caller buffer with
the board pointer reloaded between them).

**What made this family match:** the three-way row ops only need plain
`row[p[3]] = row[p[1]] op row[p[2]];` — the whole AND/OR/XOR triplet and both
divide forms are natural codegen. The *near-misses* in the same area are all
the other shape: bodies that put the *destination* index in `p[1]`
(`func_8007AD6C/ADB0/ADF4/AC30/AC80/7A9D0/7AA60`) need the scaled index first
and land the row base in `a2`/`v0`; cc1 keeps choosing `v1` and reorders the
`addu`, so they stay parked.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (147 bodies, 707
`INCLUDE_ASM`, 76 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: thirteenth small-leaf batch — 3 more C bodies (140 bodies, 714 stubs, 73 TUs)
Landed, each transcribed from its retail listing and byte-compared per function:
`func_80085C48` (the `D_800C48E8`/`D_800D2C94`/`D_800D2C96` halfword stamps
then `func_80098C6C(a2 & 0xFFFF)`), `func_8007B3E4` (the masked
`func_8007A280((v + 3) & 0xFF, p[1], p[2] | (p[3] << 8), 0)` call) and
`func_800879A8` (the 0x20-iteration, 0x48-stride stamp loop that stores the
`func_80089C08` result at `D_800C3FFE + off`).

**Merged-run decl hazard, again:** `func_80085C48` is adjacent to the landed
`func_80085C88`, so the new body became the run head and its `T6_DECLS` entry
replaced the `T4_SET`/`T4_DECLS` block the run used to get — the first build
failed with `D_800D2D28` undeclared in `main29.c`. A merged run's decls are the
**union** of every body in it; `func_8007B3E4` needed the same treatment on
`T5C_DECLS` (`extern void func_8007A280(u8, u8, u32, u32)`).

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (140 bodies, 714
`INCLUDE_ASM`, 73 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

### Battle: twelfth small-leaf batch — 7 loop/table bodies (137 bodies, 717 stubs, 72 TUs)
Landed, each transcribed from its retail listing and byte-compared per function:
`func_80071A08` (three-byte clear over `D_800C3EAC + 0x2EB`), `func_80085350`
(`0xFF` across `D_800D2D5C[0..0xA]` plus the marching `D_800D2D70` halfword
clears), `func_80085E78` (seven `0xFF` bytes at `0x2CC` then `0x2D6 = 0`),
`func_800885D0` (the `D_800C3EB4 -> D_800D301C` two-level lookup),
`func_800B8354` (`while (ArchiveDataSync()) func_800BE790();`), `func_8007AAF4`
(row byte `% p[2]` stored back through the same pointer) and `func_800B73EC`
(`HeapAlloc(0x10F7C, 1)` parking block — addiu-flavoured, so it lands in
`mainl61`).

**Two source shapes worth reusing:** the row/index loops only keep the base
first (`addu v0, v0, v1`) when written as
`*(u8*)(i + off + (s32)(unsigned int)ptr) = …` — indexing the array directly
makes GCC commute the operands; and a lookup through two tables needs the
pointer-cast form
`*(u8*)((u32)D_800D301C + (t << 2))` or GCC folds `(v + 0x18) * 4` into
`v * 4 + 0x60` and loses the retail `addiu`/`sll` pair. The marching-pointer
loop also needs the `0xFF` held in a named `u8` so the `ori` is hoisted before
the pointer setup.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (137 bodies, 717
`INCLUDE_ASM`, 72 TUs); tiny-leaves differential `checks=7072`, 73 mutants;
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

**TU renumbering:** three bodies opened their own runs, so the TU set grew
from 67 to 72 (`mainl58`, `mainl61`, `mainl67`, `mainl71` are the new
addiu-flavoured names) and every later `mainN` shifted. `linker/battle.ld` must
be moved aside so gears regenerates it, and `pc_port/build_port.sh`'s
`REFERENCE_ONLY_GAME_TUS` list has to be rewritten from the generator's
`tu_names.txt` in the same pass.

### Battle: eleventh small-leaf batch — 8 more C bodies, and an oracle hole closed (130 bodies, 724 stubs, 67 TUs)
Landed, each transcribed from its retail listing and byte-compared per function:
`func_8007AB30` / `func_8007AB68` / `func_8007ABA0` (the row AND / OR / XOR
triplet over `s_boardPtr`), `func_8007E674` (the `D_800D2C8B[idx] = 4` row
stamp), `func_8008AA40` (`(sh 0x14(pMem) << 16) | (a & 0xFF)` into
`func_80039DB8`), `func_8009E508` (`0xF80B` mask row), `func_800B8D7C`
(`func_8002A498(0)`), `func_800BCAD0` (`func_800BC2F0(1)`).

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (130 bodies, 724
`INCLUDE_ASM`, 67 TUs); tiny-leaves differential now **checks=7072 with 73
mutants rejected** (kinds 65..72 added); board-family + leaf-setter
differentials PASS; rom-check member/shop/battle PASS with slus `61b675d2…` /
field `761df27a…` unchanged; port LINK OK, 72 stubs, `xeno-port` `f808d261…`
unchanged; Map0 x2 `RUN_RC=124` plain=7.

**Oracle hole (closed):** the MIPS adapter runs `bridge()` **before every
instruction fetch**, entry included. `func_8008AA40` was still bridged from
its `INCLUDE_ASM` days, so its case silently ran the host C on *both* sides
and its `aa40_shift` mutant was invisible. Removing that bridge entry (the
retail slice is resident via `kCases`) restores the mutant check. Any landed
body that is also a case entry must not be bridged; the remaining bridged
callees with shipped C are `func_80089C08` / `func_80089C9C` (deliberate —
the test owns their fixtures, and neither is a case entry).

**Fixture lesson:** mappings are first-match, so `D_800D2C8B` must be listed
before the wide `D_800D2C60` buffer (retail places them 0x2B apart); with the
order reversed the oracle wrote into the wrong host array and the `2C60`
comparison could never fail.

### Battle: tenth small-leaf batch — the 0x8008B/C screen-state family (122 bodies, 732 stubs, 63 TUs)
Landed, each transcribed from its retail listing and byte-compared per function:
`func_8008B168`, `func_8008BC98`, `func_8008CD28`, `func_8008C3F0` (the
D_800D2D28 group stamps — one pointer local per group, as in the byte-setter
family — followed by the `func_80089C08` → `func_800BC404` → `func_800BCD98`
tail over the `D_800C3EAC` row selected by the entry index) and
`func_8008C360` (the teardown variant with the `func_8008FA60(0)`,
`func_8008FA60(1)`, `func_800716D8`, `func_8007765C`, `func_80077980` chain;
its final `0xB1`/`0xB0` pair must share one pointer local or GCC reloads).
The four stamp variants differ only in which group is written and the `0xB7`
code (1 / 2 / 3 / 4).

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (122 bodies, 732
`INCLUDE_ASM`, 63 TUs); tiny-leaves differential now **checks=5792 with 68
mutants rejected** (kinds 60..64 added); board-family + leaf-setter
differentials PASS; rom-check member/shop/battle PASS with slus `61b675d2…` /
field `761df27a…` unchanged; port LINK OK, 72 stubs, `xeno-port` `f808d261…`
unchanged; Map0 x2 `RUN_RC=124` plain=7.

**Test-harness notes:** `func_8007765C` and `func_80077980` are now *resident
slices* (they are case entries themselves and are called by `func_8008C360` —
bridge them and the oracle runs the host C, hiding their mutants).
`func_800BC404` is still an `INCLUDE_ASM` body, so the differential models its
retail behaviour in a stub; `func_80089C08` is bridged to the shipped body
(whose `D_800C3448` fixture the test already owns). A mutant can also hide
behind its own side effects: `func_8008C360`'s `[0xC6] = 0` store is
immediately overwritten by the `func_80077980` call it makes, so the mutant for
that body flips the branch condition instead.

**Parked:** `func_80079ED8` (`jtbl_8006FB7C` switch dispatcher — needs the
jump-table carve treatment), `func_8008CDE4`, `func_8008CFB8` (the larger
0x8008Cxxx bodies), plus the earlier `A22E8`/`A23E8`/`AA514`/`A2F94`/`AF400`
and row-formula/clamp families.

### Battle: ninth small-leaf batch — 4 more C bodies (117 bodies, 737 stubs, 62 TUs)
Landed: `func_8007FCE8` / `func_8007FDEC` (free the parked object and clear the
`D_800D2D28` liveness byte — the global is re-dereferenced after the call, so
write it as two `D_800D2D28[...]` accesses, not a cached pointer),
`func_80078CEC` (stamps the `D_800D2E60` table byte into the `D_800C402F` row at
the `*(D_800C3EAC + 0x2DA) * 72` offset) and `func_8007893C` (gates on the
`D_800D2E5F` row and runs the `0x1E`-argument triple).

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (117 bodies, 737
`INCLUDE_ASM`, 62 TUs); tiny-leaves differential now **checks=5432 with 63
mutants rejected** (kinds 56..59 added); board-family + leaf-setter
differentials PASS; rom-check member/shop/battle PASS with slus `61b675d2…` /
field `761df27a…` unchanged; port LINK OK, 72 stubs, `xeno-port` `f808d261…`
unchanged; Map0 x2 `RUN_RC=124` plain=7.

**Fixture lessons:** the D_800D2E5D/E5F/E60/E61/E62 shared buffer keeps paying
off — aliases at +1/+3/+4/+5/+6 cover every table in that retail cluster. Two
traps for table-indexed bodies: (a) the tested slice must be loaded with its
*full* size (0x40 truncated a 0x50 function and ran the emulator off the end);
(b) an unmasked index walks out of the fixture, and the oracle reads the row
area there while the host C reads arbitrary memory — so pass a masked index
(`arg & 7`) to both sides. A merged run's decls must be the union of every body
in it (adding `func_80078CEC` to the `func_80078C9C` run needed
`D_800D2E60`/`D_800D39E0`/`D_800D2E62` as well).

**Parked:** `func_80079ED8` is a `jtbl_8006FB7C` switch dispatcher (needs the
jump-table carve treatment), `func_8008B168` / `func_8008CD28` are the
`D_800D2D28` group + `func_80089C08`/`func_800BC404`/`func_800BCD98` tails (next
batch), `func_800A22E8`, `func_800A23E8`, `func_800AA514`, `func_800A2F94`,
`func_800AF400` and the row-formula/clamp families stay on the ledger.

### Battle: eighth small-leaf batch — 5 more C bodies + the `li`-encoding unlock (113 bodies, 741 stubs, 60 TUs)
Landed: `func_800A3484` / `func_800AEEEC` (store -1 through an **`s16`** —
writing the unsigned `0xFFFF` constant makes GCC emit `ori`, retail has
`addiu`), `func_800B3B6C` / `func_800BCAA4` / `func_800BFD88` (the retail
assembler expanded their `li` constants as `addiu`; they now build in
`mainl*` TUs that select the new **BattleLiteral** preset with maspsx
`--dont-expand-li`, which leaves `li` to gas and produces the addiu encoding).

**The harness was hiding this:** objdump aliases both `addiu rt,$zero,K` and
`ori rt,$zero,K` to `li`, and the normalisers mapped `li` to `ori` (and
special-cased `addiu`/`ori` zero-base forms differently), so true encoding
mismatches were scored as matches or as spurious diffs. `cmp2.py` now decodes
the real opcode from the instruction word and compares the retail spelling.
With that fix: `func_800716D8` turned out to need no flag at all, and the
`s16` spelling fixed both -1 stores.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (113 bodies, 741
`INCLUDE_ASM`, 60 TUs incl. mainl49/mainl55/mainl59); tiny-leaves differential
now **checks=5144 with 59 mutants rejected** (kinds 51..55 added); board-family
+ leaf-setter differentials PASS; rom-check member/shop/battle PASS with slus
`61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72 stubs, `xeno-port`
`f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

**Generator additions:** runs never mix addiu- and ori-flavoured bodies (the
flag is TU-wide), LIT runs are named `mainl<N>`, and the stale-file cleanup glob
is `src/battle/main*.c` so old `mainl*` TUs cannot linger and collide in the
host tests. The yaml C-entry replacement must span the whole contiguous block —
a partial replace left duplicate entries and a stale linker script (symptom:
`ld: cannot find build/src/battle/main60.c.o`).

**Parked:** `func_800716D8` — the retail branch skips only the *first* call
(`func_800BE790` always runs; the function always returns 0), which the
corrected body now models, but its epilogue schedules `addu v0,zero,zero`
before the `lw ra` while ours emits it after and fills the `jr` delay slot
(12/17). `func_800B56E4` (16/18), `B5FBC`/`B61B0` (15/18), `B73EC` (12/14) and
`BC404` (17/20) all move closer under `--dont-expand-li` but keep residuals.

### Battle: seventh small-leaf batch — 6 more C bodies (108 bodies, 746 stubs, 55 TUs)
Landed, each transcribed from its retail listing and byte-compared per function:
`func_80079E18` / `func_80079E4C` (set/clear the `D_800D2D28[0xB4]` flag and
the 96-byte `D_800D3725` row byte), `func_80079E7C` (scans the
`func_80089C9C` table for the first live slot — the loop counter must be `s32`
so GCC emits `slti`, not `sltiu`), `func_80078C9C` (stamps 0xFC into the
`D_800C402F` row, then forwards), `func_8009A7B8` (reads the
`g_GameState + 0x271` row byte at a 0xA4 stride) and `func_8009E3C8` (writes 4
to the `D_800C3D60` object and 3 into the `D_800CCE4A` row).

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (108 bodies, 746
`INCLUDE_ASM`, 55 TUs main..main55); tiny-leaves differential now **checks=4780
with 54 mutants rejected** (kinds 45..50 added: the flag pair, the scan, the
row stamp, the g_GameState reader, the object/row writer); board-family +
leaf-setter differentials PASS; rom-check member/shop/battle PASS with slus
`61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72 stubs,
`xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

**Harness lessons:** landed bodies now collide with the older per-test stubs —
the family differential used to *spy* on `func_80079E7C`'s argument, and with
the real body linked the spy had to go; the check now probes its behaviour
instead (`D_800C3448[i] = 1 << i` makes the return value the lowest set bit of
the row word, which still pins the argument). New fixture buffers must also be
in the comparison set or their mutants are invisible (`s_402fBuf` /
`s_ce4aBuf` are snapshotted per case now), and fixture values decide whether a
mutant is observable: the loop-bound mutant only shows with an all-zero table
(early exit hides it), and the 0xA4-stride mutant only shows with a
non-uniform `g_GameState` fill.

**Parked near-misses found in this pass:** `func_800716D8` (16/17 —
`addiu $v0,$zero,-1` vs `ori`), `func_800A2F94` (2/17), `func_80079E7C`'s
siblings unchanged; the D_800D2D28 row helpers all landed this time.

### Battle: sixth small-leaf batch — 4 more C bodies (102 bodies, 752 stubs, 51 TUs)
Landed, each transcribed from its retail listing and byte-compared per function:
`func_8007AF5C` / `func_8007AFAC` (divide / modulo the D_800D3430 row bytes),
`func_80079114` (builds the `D_800D2E60 | (D_800D2E61 << 8)` word for the
index and forwards the `D_800D2E5D` byte through `func_8007A280`) and
`func_800764EC` (the eight-call teardown chain).

**Recipe note:** in `func_80079114` the OR operands must be written
`byte60 | (byte61 << 8)` — the reversed form makes GCC emit `or a2,a2,v0`
where retail has `or a2,v0,a2` (the 21/22 → 22/22 difference).

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (102 bodies, 752
`INCLUDE_ASM`, 51 TUs main..main51); tiny-leaves differential now **checks=3860
with 48 mutants rejected** (kinds 41..44 added); board-family + leaf-setter
differentials PASS; rom-check member/shop/battle PASS with slus `61b675d2…` /
field `761df27a…` unchanged; port LINK OK, 72 stubs, `xeno-port` `f808d261…`
unchanged; Map0 x2 `RUN_RC=124` plain=7. The teardown chain needed six more
bridged stubs (kinds 27..34) and `D_800D2E61` is aliased into the shared
`s_e5dBuf` at +5.

**Parked near-misses found in this pass:** `func_8007AC30` / `ABD8` / `AE98`
(the D_800D3420/D_800D3430 clamp family — 10..12/22, same base-ordering
residual as the formula family), `func_800AA600` (11/22), `func_800AA514` (5/20,
register-order in the sign-extension/multiply), `func_800BC404` (16/20,
`addiu a0,zero,1` vs `ori`), `func_800BF354` (0/20), `func_800BF998` (4/23),
`func_800BB690` (8/21), `func_800A3578` (6/20).

### Battle: fifth small-leaf batch — 6 more C bodies (98 bodies, 756 stubs, 49 TUs)
Landed, each transcribed from its retail listing and byte-compared per function:
`func_8008AC00` (heap-user switch, then alloc `(size + 3) * 26`),
`func_80076B68` / `func_80076BAC` / `func_80076BF0` / `func_80076C34` (call the
semi-transparency helper, stamp the three colour bytes and OR the flag into
`+0x16`; the four differ only in the byte/flag constants) and `func_80079054`
(forward the `D_800D2E5D` / `D_800D2E60` table bytes for the index through
`func_80079ED8`).

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (98 bodies, 756
`INCLUDE_ASM`, 49 TUs main..main49); tiny-leaves differential now **checks=3604
with 44 mutants rejected** (kinds 35..40 added: alloc wrapper, the four
semi-transparency stamps, the table forwarder); board-family + leaf-setter
differentials PASS; rom-check member/shop/battle PASS with slus `61b675d2…` /
field `761df27a…` unchanged; port LINK OK, 72 stubs, `xeno-port` `f808d261…`
unchanged; Map0 x2 `RUN_RC=124` plain=7.

**Harness lessons:** another resident-slice entry is needed when a landed body
calls another landed body from the retail side (`func_80076AC8` is now resident
next to `func_8008ABB8`); the shared resident block is a small table so more can
be added. Fixtures for globals that retail keeps a few bytes apart must mirror
that layout *and its parity*: one `s_e5dBuf` backs `D_800D2E5D` (+1),
`D_800D2E60` (+4) and `D_800D2E62` (+6) so the `u16` reads stay naturally
aligned (UBSan caught the misaligned variant immediately). The shape harness now
skips zero-valued immediates on both sides (relocatable-object placeholders) but
still compares every nonzero numeric immediate.

**Parked near-misses found in this pass:** `func_8007B2C0` / `B310` / `B360` /
`func_8007E6A0` / `E6F0` (the u16/u32 row formula family — 17/20, only the table
base register differs: retail allocates `a2`, ours `v1`; four shapes did not
flip it), `func_800B16A4` (17/19 — `total = i` copy folded away and the `p + off`
addend order), `func_800B56E4` / `B5FBC` / `B61B0` (`addiu $r,$zero,K` vs
`li`→`ori`), `func_8007AD6C` (base-order residual), `func_800A23E8` (0/19).

### Battle: fourth small-leaf batch — 5 more C bodies (92 bodies, 762 stubs, 48 TUs)
Landed, each transcribed from its retail listing and byte-compared per function:
`func_8008ABB8` (switch heap user to 2, then allocate), `func_800A22A8` /
`func_800A2D1C` (free the parked block and clear the state halfwords),
`func_800B7364` (work-list pair + `func_80025180(p)`) and `func_800B9020`
(forward the signed +0xB0 byte to `func_800245D8`, then `func_80021BF8(p, 0)`).

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (92 bodies, 762
`INCLUDE_ASM`, 48 TUs main..main48); tiny-leaves differential now **checks=3216
with 36 mutants rejected** (kinds 30..34 added: allocator wrapper, the two
state resets, the work-list pair, the signed-byte forwarder); board-family +
leaf-setter differentials PASS; rom-check member/shop/battle PASS with slus
`61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72 stubs,
`xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

**Harness hardening (important):** the shape harness ignored *immediates*, so a
wrong frame size or offset could still score full marks — `func_800BB314`
scored 15/15 while the built frame was 0x18 instead of retail's 0x20, which
broke the byte-identity of the whole overlay. The normalizers now compare
numeric immediates too (skipping `%hi/%lo` symbol forms and the zero
placeholders of relocatable address materialisation). Also: a bridge registered
for a case's *own entry address* makes the oracle call the host C instead of
the retail slice, so mutants become invisible — `func_8008ABB8`'s slice is now
kept resident in guest RAM instead of bridged. And the host `/tmp` tmpfs filled
up from another session's work (`/tmp` 81%), which silently produced empty
objects; the harness and generator now live in `/var/tmp/xeno-harness` and run
with `TMPDIR=/var/tmp/xeno-host-cc`.

**Parked near-misses found in this pass:** `func_800BB314` (only the frame size
differs: retail 0x20 with s0/ra at 0x18/0x1C, ours 0x18 — every call-shape and
prototype variant tried keeps 0x18); `func_80085310` (GCC hoists the
D_800C3EBE base into a register where retail re-materialises it through `$at`
per access); `func_800A22E8` (2/18 — loop form/register residual);
`func_800BE6A0` (nibble formatter, 3/18).

### Battle: third small-leaf batch — 7 more C bodies (87 bodies, 767 stubs, 45 TUs)
Landed, each transcribed from its retail listing and byte-compared per function:
`func_800764B4` (clears D_8005959C then the three teardown calls),
`func_80076AC8` (semi-transparency on, shade texture off), `func_8007765C`
(frees the block at `D_800C3EA4 + 0xA230`), `func_8007AAB8` (divides the
D_800D3430 row byte by the board field), `func_8008AC50` (pumps
`ArchiveDataSync` until idle), `func_8009AB00` (clears bit 0 of the
`D_800C34B0` row's +0x15A byte) and `func_800B7330` (DrawSync + HeapFree).

**Recipe notes:** the row divide matches with the integer row form
(`u8* pRow = D_800D3430 + ((u32)index << 6); pRow[p[1]] /= p[2];`); the
`D_800C34B0` row stride is `index * 0x170` (GCC's `((3x)<<3 - x)<<4`
decomposition); the sync pump is a plain `while (ArchiveDataSync() != 0)
func_800716D8();`.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (87 bodies, 767
`INCLUDE_ASM`, 45 TUs main..main45); tiny-leaves differential now **checks=2892
with 31 mutants rejected** (kinds 23..29 added: teardown chain, SetSemiTrans/
SetShadeTex wrapper, parked-block free, row divide, sync pump, row bit clear,
DrawSync+HeapFree); board-family + leaf-setter differentials PASS; rom-check
member/shop/battle PASS with slus `61b675d2…` / field `761df27a…` unchanged;
port LINK OK, 72 stubs, `xeno-port` `f808d261…` unchanged; Map0 x2
`RUN_RC=124` plain=7.

**Harness lessons:** the host `/tmp` quota can break the host-side test build
(`error writing to /tmp/cc*.s: Disk quota exceeded`) — run the differentials
with `TMPDIR=/var/tmp/xeno-host-cc`. A fixture value can hide a mutant when the
operation is a no-op for it (`x &= 0xFE` on an even byte 0x44) — the
`func_8009AB00` case needs an odd fixture byte (0x5B). A stub whose return
changes across calls (`ArchiveDataSync` returning non-zero twice) must be reset
before *both* the oracle and the C run or the two sides see different call
counts.

**Blocked near-misses found in this pass:** `func_800AF400` (GCC peels the
bit-0 test; retail enters the variable-shift loop directly — 0/16 across three
loop shapes); `func_800B3588` / `func_800B383C` / `func_800B73EC` (`sw
$zero,sym` macro picks `$at`, and the `addiu $r,$zero,1` case); `func_800C0F70`
(retail keeps the global's address in `s0` across the call, ours rematerialises
it into `$at`); `func_8007E740` (row-family base-order residual, 6/16);
`func_800AA79C` (13/16).

### Battle: second small-leaf batch — 7 more C bodies (80 bodies, 774 stubs, 39 TUs)
Landed, each transcribed from its retail listing and byte-compared per function:
`func_800AA760` (D_800D3368 pointer-table byte setter), `func_8007B3B0`
(four-argument forward of `(x+3)&0xFF` plus the board fields), `func_8007BA88`
/ `func_8007BAB8` (descending byte/halfword clears), `func_800B6A50`
(three-word copy out of the +0x70 sub-object), `func_8007D1A8` (stores the
`g_GameState + 0x1924` word into the D_800D3410 row) and `func_8008FDE4`
(fixed-argument `func_8008FC1C` wrapper).

**Recipe added:** in the descending clears the counter must be initialized as
its *own statement before* the row address (`s32 i = 0xF;` then
`u8* p = ...; for (; i >= 0; i--) *p-- = 0;`). Writing it in the `for` header
makes GCC emit the constant after the address chain (4/12); the split form is
12/12 for both clears.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (80 bodies, 774
`INCLUDE_ASM`, 39 TUs main..main39); tiny-leaves differential now **checks=2292
with 23 mutants rejected** (kinds 17..22 added: pointer-table setter, forward,
two clears, sub-object copy, g_GameState row store, fixed-arg wrapper);
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` `f808d261…` unchanged; Map0 x2 `RUN_RC=124` plain=7.

**Test-harness refactor:** the retail row region is now ONE host buffer
(`s_rowArea`) with `.set` aliases for `D_800D3344`, `D_800D3368`, `D_800D3410`,
`D_800D342E`, `D_800D3430`, `D_800D343F`, `D_800D366C`, mapped as a single
guest range 0x800D3300..0x800D3B00 — retail keeps those addresses overlapping
(D_800D3410's rows run into D_800D3430's, the descending clears walk through
the same area), and a per-case snapshot now compares the whole buffer.
Aliases work for reads *and* stores as long as the test uses the declared
symbol. Fixture gotcha: a pointer field read through `u8**` on the host is 8
bytes — write a full host pointer, a `u32` store leaves the fill pattern in
the high half and the C dereferences garbage.

**Blocked near-misses found in this pass:** `func_800B5DC4`, `func_800B8068`
and `func_800BCAD0` need `addiu $r,$zero,K` where our `li` expands to `ori`
(the BFD88/A3484/AEEEC class); `func_800B6438` (8/11, address-register/
ordering residual); `func_800B9258` (9/11, only `lui v0; lw v1,v0` vs our
`lui v1; lw v1,v1`).

### Battle: batch of 7 more C bodies (73 bodies, 781 stubs, 34 TUs)
Landed, each transcribed from its retail listing and byte-compared against
`disc/battle.bin`: `func_8007A8B4` (per-iteration `*ppBoard` reload copy loop),
`func_80085C88` (mask once into a local, two setup calls, clear
`D_800D2D28[0xAD]`), `func_8008BC40` / `func_8008CCCC` / `func_8008B108`
(`D_800D2D28` byte-group clears), `func_80077610` (0x670 alloc, park at
`D_800C3EA4 + 0xA230`, bzero, follow-up) and `func_8008AA74` (conditional
forward while `D_800D366C` is set).

**Recipe added:** for the `D_800D2D28` multi-store helpers the C needs **one
`u8*` local per store group** (`p = D_800D2D28; p[0x9E] = 0; ...`). Retail
re-dereferences the global pointer once per group, not once per store; three
separate `D_800D2D28[i] = v;` statements make GCC reload after *every* store
(measured 8/35) and the comma-operator form does not help. With per-group
locals all three helpers are byte-exact.

Gates: `battle.bin` `1830b4ef…` == `disc/battle.bin` (73 bodies, 781
`INCLUDE_ASM`, 34 TUs main..main34); `run_battle_tiny_leaves_retail_test.sh`
extended with kinds 12..16 (D_800D2D28 group helpers, D_800D366C forwarder,
the allocator case, the copy loop, the two-call wrapper) — **PASS O0/O2/UBSan
checks=1872, 16 mutants rejected** (new: groupB2_one, alloc_off4, copy_xor);
board-family + leaf-setter differentials PASS; rom-check member/shop/battle
PASS with slus `61b675d2…` / field `761df27a…` unchanged; port LINK OK, 72
stubs, `xeno-port` unchanged; Map0 x2 `RUN_RC=124` plain=7.

**Harness lessons (differential):** fixture mappings are first-match, so a wide
fixture (the 0x400-byte `D_800D3430`) shadows later real globals inside its
range — put narrow globals (`D_800D366C`) first. `D_800C3EA4` must map to the
pointer *variable*, not the buffer; the buffer store address is a host pointer
and is resolved by the host-range check. Case slice sizes must cover the whole
function (0x40 truncates the >0x40-byte helpers and runs the emulator off the
end).

**Blocked near-misses found in this pass (same classes as the ledger):**
`func_800B3B6C` (9/10) and `func_800BCAA4` (10/11) differ only by
`addiu $r,$zero,K` vs our `ori` (`li` expansion — the BFD88/A3484/AEEEC class);
`func_800BE0DC` (9/11) is `lui v0; lw a0,v0` vs our `lui a0; lw a0,a0`;
`func_800BDCF8` (11/15) is the `sw $zero,sym` macro picking `$at` where retail
used `$v0`; `func_8007FEC4` (15/20) schedules the last `sh` into the epilogue
load slot.

### Battle: func_8007A900 landed (66 bodies, 788 stubs, 28 TUs)
`func_8007A900` (0x8007A900..0x8007A92C, 0x2C) is shared matching C transcribed
from `asm/battle/matchings/main5/func_8007A900.s`:

    u8* p = *ppBoard;
    u8* pRow = D_800D3430 + ((u32)index << 6);

    pRow[p[1]] = p[2];

Retail forms the row pointer first (index<<6 added to the base), then indexes it
with p[1] and stores p[2]. Built TU is instruction-identical (shape harness
11/11) and the 0x2C overlay slice sha `11029896…` equals retail.

TU renumbering: the new run [0xAE10,0xAE3C) splits the old main5, so
`config/battle.yaml` now lists `main..main28` (main6..main28 shifted by one) and
`REFERENCE_ONLY_GAME_TUS` gained `src/battle/main28.c`.

* `battle.bin` sha `1830b4ef…` == `disc/battle.bin` (byte-identical; all 66
  landed bodies verified against the pinned overlay).
* `run_battle_tiny_leaves_retail_test.sh` extended with kind 11 (D_800D3430
  fixture, distinct fill): PASS O0/O2/UBSan checks=1356, 13 mutants rejected
  including the new `row_shift5`. Board-family and leaf-setter differentials
  still PASS.
* rom-check: member/shop/battle PASS; slus 303068 `61b675d2…` and field
  `761df27a…` unchanged from the last recorded state (checksum.sha untouched).
* Port LINK OK, 72 stubs, `xeno-port` `f808d261…` unchanged (battle TUs are
  reference-only). Map0 x2: `RUN_RC=124`, FieldMain, plain=7, primSubmits
  nonzero, no new asserts.

**Parked near-miss (do not re-derive):** `func_8007A92C` and `func_8007A968`
need retail's *late* base materialization — after the `lbu p[1]` load, the base
reuses p's register (`lui v1; addiu v1; addu a1,a1,v1` in A92C). 36 statement-
order permutations plus off-first, pointer-row, single-statement-store and
reused-p shapes all make GCC 2.7.2 hoist `lui/addiu` into the load-delay slot
and allocate a2 (ceiling 5/15); the `lw`/`sll` swap travels with it. Keep both as
INCLUDE_ASM. The 0x8007E9xx reader siblings hoist the base early and are landed
(17/17 recipe above).

**Parked (no repo change):** `func_8002AC24` (0x8002AC24..0x8002B084, 280
insns, temp2.c) is one of the five remaining slus bodies with no C anywhere.
A candidate transcribed from the listing (scratch:
`/tmp/grok-goal-fed9e9423187/implementer/parked-func_8002AC24.{c,log,diff.txt}`)
reproduces the whole control flow — status!=1 rearm shared with the landed
`func_8002B084`, the `D_8004FE34` finish path, the >=0x800 / partial
`CdGetSector` paths, the `D_8004FE3C`-gated position check with `D_8004FDE8++`
re-arm, the queue advance (`func_80028A18` + `ArchiveDecodeSizeAligned`,
`(g_ArchiveCurFileSector - sector) << 11`, `D_8004FE10--`), the seek re-arm
(state 6, `CdControlF(9, NULL)`) and the finish path — but is 115/281
instruction-identical and stays INCLUDE_ASM. Residuals: retail hoists
`ori a1,zero,0x200` into the `bgez` delay slot (ours emits `nop` + later
`ori`); retail's queue read is `lhu a0` + a redundant `andi s0,a0,0xFFFF`
(every source shape tried makes GCC 2.7 fold the mask); and the queue-entry
field load/store order differs (`lhu, lw, store FE10, andi, store FE08`).

### Next frontier (staged, not yet landed): the 0x8007E8AC accessor family
The longest remaining stretch of small battle bodies is
**0x8007E8AC..0x8007EF6C (28 functions, 432 insns)**, inside `main.c`'s asm run.
Adding it means re-partitioning once (TU boundaries are run ends): the generator
recipe is "for each C run in address order emit a TU `[previous run end, this run
end)` whose source holds that range's stubs followed by that run's C", then
update `config/battle.yaml`, `REFERENCE_ONLY_GAME_TUS` and the differential's TU
list. The first ten listings are read; they are a two-table accessor family:

| function | shape |
|---|---|
| `func_8007E8AC` | `p = *(u8**)a0; D_800D3364[(p[2]<<3)+(p[1]<<6) + 0x140] = p[3]` (byte store) |
| `func_8007E8E0` | `a0 = *(u8**)a0; v = *(u16*)(D_800D3420 + (idx<<6) + (a0[1]<<1)); D_800D2DC0 = func_80079E7C(v) + 1` |
| `func_8007E934` | `u8 buf[0x10]; func_80078508(buf);` (frame 0x28, no result use) |
| `func_8007E954` | `return (D_800D3430[idx<<6][p[1]] ^ p[2]) == 0` |
| `func_8007E98C` | `return (u16)(D_800D3420[idx<<6][p[1]]) ^ (p[2] \| (p[3]<<8)) == 0` |
| `func_8007E9D0` | `return (u32)p[2] >= D_800D3430[idx<<6][p[1]]` |
| `func_8007EA08` | `return (u16)D_800D3420[idx<<6][p[1]] >= (u32)(p[2] \| (p[3]<<8))` |
| `func_8007EA4C` | `return D_800D3430[idx<<6][p[1]] >= (u32)p[2]` (mirrored `sltu`+`xori`) |
| `func_8007EA84` | `return (u16)D_800D3420[idx<<6][p[1]] >= (u32)(p[2] \| (p[3]<<8))` (mirrored) |
| `func_8007EAC8` | `return (D_800D3430[idx<<6][p[1]] ^ D_800D3430[idx<<6][p[2]]) == 0` |

All ten take `(u8** a0, u8 index)` and read the first field of `*a0` (offsets
1/2/3); the two tables are 0x40-byte rows at `D_800D3420` / `D_800D3430`.
Verify each against the built object's size before extending the run.

## Battle: tiny-leaves differential closed over all 23 bodies
`run_battle_tiny_leaves_retail_test.sh` now drives every tiny leaf landed so far
(the seven no-ops, the two sprite-global clearers, the word getters, the
pointer-global store, the pointer bump, the masked byte store, the constant byte
store, plus `func_800A5E9C`, `func_800B00D0`, `func_800769E8`, `func_8007A9A8`,
`func_8007E7C0`, `func_80078D48`, `func_80080C6C`, `func_800B168C`). It compares
the fixture arena, the seven scalars, the `D_8005A3A0` / `D_800D2E62` tables, the
downward-clear buffer, the `LoadImage`/`DrawSync` call log (kind + both
arguments) and, for the pointer-returning case, the low 32 bits of the return.
**PASS at O0/O2/UBSan with 12 mutants rejected.**
**Fixture lessons (all cost iterations, worth reusing):**
* A count-down clear needs its mapping to span the bytes *below* the symbol —
  `{0x800C3BCC - 0x20, 0x40, buf}` with the `.set` alias at `buf + 0x20` — or the
  emulator's stores land in guest RAM instead of the fixture.
* `LoadImage` = 0x80044894 and `DrawSync` = 0x800445D0 (from
  `linker/undefined_funcs_auto.*`); guesses made the emulator walk off the slice.
* Kind-specific argument masking (`idx = arg & 7`, `& 3` for the row pointer)
  keeps the modelled writes inside the fixture tables.
* The *board pointer variable* itself needs mapping, and its address cannot be a
  `kMap` constant (a pointer-to-integer cast is not a constant initializer) — use
  a runtime host-range check like the other harnesses.
* A uniform table fill hides shift mutants; `D_800D2E62` is now distinct-valued.

## Battle: 8 more leaves (65 bodies, 789 stubs) — breadth pass recipes
**Landed:** `func_800A5E9C` (two word stores), `func_800B00D0` (9-word clear),
`func_800769E8` (`LoadImage` + `DrawSync(0)` wrapper), `func_8007A9A8`
(`D_8005A3A0[p[1]] = p[2]`), `func_80078D48` (indexed `u16` load + global store),
`func_80080C6C` (`D_800D3278[index * 0x38 + 0x34] = 1`), `func_800B168C`
(`pArg + iValue * 0x1C + 0xC`) and `func_8007E7C0` (`u16` store into the
`D_800D3278` row). Battle is **65 C bodies / 789 `INCLUDE_ASM`** (27 run-pair
TUs), all 65 byte-compared equal, `battle.bin` still `1830b4ef…`.
**Recipes added by this pass (all verified with the shape harness first):**
* Index arriving in `a1` with `a0` unused → declare a dummy first parameter
  (`void func_80078D48(u32 unused, u8 index)`) — that alone took it 6/9 → 9/9.
* Count-down clears: the *index* form
  (`for (i = 8; i >= 0; i--) *(u32*)((u8*)D_800C3BCC + (i - 8) * 4) = 0;`) matches
  where the pointer-walk form does not (2/9 → 9/9).
* Pointer-base stores: field temporaries first, then the base added inline to the
  scaled index (`(i1 << 1) + (u32)pBase + 0x394`) — the `pBase + scaled` order
  flips the register allocation (5/9 → 9/9).
* Explicit parentheses decide the final add: `pArg + (iValue * 0x1C + 0xC)`
  matches retail's `addiu` before the `addu` where the left-to-right form does
  not (4/6 → 6/6).
* A `u32 off = (index & 0xFF) * 0x38;` + base variable reproduces the
  base/offset register split for `func_80080C6C` (7/10 → 10/10).
**OWED:** extend `run_battle_tiny_leaves_retail_test.sh` to these eight bodies.
Their fixtures need: `D_800C3BCC` aliased into a buffer with slack *below* the
symbol (the clear walks downwards), a `D_8005A3A0` table, `D_800D39E0` in the
scalar snapshot, an arena large enough for `D_800D3278 + 0x394`, bridges for
`LoadImage`/`DrawSync`, and a low-32-bit comparison for the pointer-returning
`func_800B168C`.
**Still blocked (near misses, all encoding-level):** `func_800BFD88` needs
`addiu $a2,$zero,1` where GCC emits `ori` (7/8); `func_800BF5E8` (`++global`)
needs the address register reused in the store instead of the gas macro (2/8);
`func_800B9B30`, `func_800B8054`, `func_800BE108`, `func_800BED30` are symbol
stores where retail used `$v0` (the macro always picks `$at`).

## Battle: 6 more leaves + a joint differential (57 bodies, 797 stubs)
**Landed:** `func_800A577C` / `func_800A578C` (word getters),
`func_800BF0B4` (`*(u32*)(D_800C3610 + 0x1C) = value`), `func_80079934`
(`*pValue += 4`), `func_800AA788` (`D_800C3B74 = value & 1`) and
`func_800B14B8` (`D_800C3D6C = 1`). Battle is **57 C bodies / 797
`INCLUDE_ASM`** (19 run-pair TUs), all 57 byte-compared equal, and
`battle.bin` is still byte-identical (`1830b4ef…`).
**Finding:** the byte *symbol stores* are not all blocked by the gas macro — the
ones whose retail listing uses `$at` (`sb $a0,%lo(sym)($at)`) reproduce exactly
(`func_800AA788`, `func_800B14B8`). Only stores where retail used a different
tempo register (`func_800B7C28` uses `$v0`) still fail, because GNU as's macro
always picks `$at`.
**New differential:** `pc_port/tests/run_battle_tiny_leaves_retail_test.sh`
(+ test C) covers all 15 tiny leaves (this pass and the previous one): each slice
is loaded from `disc/battle.bin` (0x40 bytes — 4-byte slices truncated the body
and ran the emulator off the end), run on the MIPS adapter with the globals and a
fixture arena mapped to their retail addresses, and compared against the shipped
C on **the arena, the four scalars and (for the getters only) the return
register** — a void leaf leaves `v0` as retail's last computed value, so the
return is only meaningful for kind-1 cases. PASS at O0/O2/UBSan with 5 mutants
rejected.
**Frontier unchanged:** the `0x8007B8xx` setters (best shape 12/15 for B98C,
14/16 for B8D4; the residual is the `addu` operand order and it survives every
ordering/flag combination tried) and `func_800A3484`/`func_800AEEEC` (need
`addiu $v0,$zero,-1`, GCC always emits `ori`).

## Battle: 9 tiny bodies landed (51 total, 803 stubs) + layout-agnostic harnesses
**Landed:** seven no-op entries (`func_8009795C`, `func_8009F5B0`,
`func_800B3348`, `func_800B3350`, `func_800B89F4`, `func_800BAF40`,
`func_800BDD34` — retail is `jr $ra; nop`) and two "clear a byte of the sprite
global" helpers (`func_80077980` → `D_800D2D28[0xC6] = 0`,
`func_8009E268` → `D_800D2DC8[0x99] = 0`). Battle is now **51 C bodies / 803
`INCLUDE_ASM`**, all 51 byte-compared equal against `disc/battle.bin`, and
`battle.bin` is still byte-identical (`1830b4ef…`).
**Port-only bodies are weak now:** `src/battle/main.c` keeps the eight
`#ifndef XENO_PC_PORT/#else` port bodies for the animation-leaf test, and they
are `__attribute__((weak))` so the host tests (which compile every TU) bind to
the matching strong definitions instead of colliding.
**Harnesses are layout-agnostic now** (this kept breaking as the TU numbering
churned): both battle runners build `TUS` from `ls src/battle/main*.c`, and
`run_mutant <label> <funcName> <sed>` resolves the owning file with
`rg -l "/\* <funcName>.s"` and refuses to run if the pattern did not apply.
**Still blocked (unchanged frontier):**
* the remaining setters — `func_8007B98C` is at 12/15 on the shape harness with
  the S7 shape (`u32 off = index << 6; u32 base = (u32)D_800D3420;` and the
  addresses as `base + off + (i << 1)`), `func_8007B8D4` 14/16, `func_8007B9C8`
  12/15; every variant differs only in the `addu` **operand order** (retail
  emits `scaled + row`, GCC emits `row + scaled`, which also changes the second
  address register and the store base). Writing the scaled term first fixes the
  order but makes GCC hoist the base materialization above the index math; ~25
  source shapes were tried across B98C/B8D4/B9C8.
* `func_800A3484` / `func_800AEEEC` (store `-1` into a caller halfword) need
  `addiu $v0,$zero,-1`; GCC 2.7.2 always emits `ori $v0,$zero,0xFFFF` for that
  constant regardless of how it is spelled (six spellings tried), so they stay
  asm like the absolute-global store setters.

## Battle: reader family COMPLETE (28/28), 42 bodies, layout down to 6 TUs
**Landed:** `func_8007EEE8` (the 8-step `D_800C3EB7` activation scan — the last
unlanded reader) and `func_8007B914` (`D_8005A3A0[p[2]] = row word`).
Battle is now **42 C bodies / 812 `INCLUDE_ASM`**, and the auto-merged run
layout collapsed to **6 TUs** (`src/battle/main{,2..6}.c`).
**Loop recipe (new):** compute the loop's offset *inside* the body from the
counter — `u32 offset = 0x54 + i * 0x1C;` — instead of a pointer/offset that GCC
can strength-reduce. Retail keeps the counter in `a0` and the offset in `v1`
(`lui at; addu at,at,v1; lbu v0,%lo(at)`); the pointer form makes GCC swap them
(shape 14/23 → 23/23 after the change).
**Setters so far:** `func_8007B958` + `func_8007B914` compile byte-exactly with
the family recipe. The remaining eight (`func_8007B8D4`, `func_8007B98C`,
`func_8007B9C8`, `func_8007BA04`, `func_8007BA44`, `func_8007BA88`,
`func_8007BAB8`, `func_8007BAE8`, `func_8007BB2C`) still sit at 9-10/15 on the
shape harness: retail allocates the board pointer to **v0** and the table base to
**a0** (`lw a0,0(a0)` then `lui v0`), which is the mirror of the readers' choice,
and none of ~14 source shapes tried (pointer vs integer row, field-temp orders,
struct access, declaration order) flips it. 8-10 variants were tried per function;
this needs either a different structural idea or per-function experimentation.
**Proof:** 42/42 landed bodies byte-compared equal against `disc/battle.bin`;
`build/out/battle.bin` == retail (`1830b4ef…`); rom-check `battle PASS`; port
LINK OK 72 stubs with an unchanged `xeno-port`; Map0 x2 `RUN_RC=124`.
**Differentials:** the family test now covers 30 bodies (28 family + B958 +
B914) and rejects **10 mutants**; the leaf test still passes (6 mutants). Two
fixture details were needed to keep the mutants discriminating: the scan needs
all eight `D_800C3EB7` slots set, and the `func_8007EF44` check must zero the
tail slots around its own comparison (the two requirements conflict, so the
fixture is adjusted per check).

## Battle: register-allocation recipe found — 13 more bodies byte-exact (40 total)
**Breakthrough:** the family bodies that were "size-exact but allocation-different"
now compile byte-identically with this source shape (verified one-for-one against
the retail listings with a local cc1+maspsx+objdump shape harness):
1. load the board's fields into **u32 temps in retail's order** — `i3 = p[3]`
   first, then `i1 = p[1]`, then `i2 = p[2]` (retail emits
   `lbu a2,3(a0); lbu v0,1(a0); lbu v1,2(a0)`);
2. precompute the shifted field into its own temp (`i3s = i3 << 8`);
3. compute the row as an **integer**, `u32 row = (u32)D_800D3420 + ((u32)index << 6);`
   (not a pointer), and index with the scaled term **first**:
   `*(u16*)((i1 << 1) + row)` — this is what keeps retail's
   `addu $v0,$v0,$a1` operand order; a pointer `pRow + x` canonicalises the other
   way and a `pRow + 0x10` second row folds into the load offset.
   Both `|` and `+` appear in the family (`func_8007EC10` uses `+` → retail emits
   `addu`, the others `or`), so the operator must come from the listing.
**Landed this pass (13 bodies, all byte-exact):** `func_8007E8AC`,
`func_8007E8E0`, `func_8007E98C`, `func_8007EA08`, `func_8007EA84`,
`func_8007EB08`, `func_8007EB90`, `func_8007EC10`, `func_8007EC94`,
`func_8007ED14`, `func_8007ED98`, `func_8007EDE0`, `func_8007EE28`. Battle is
now **40 C bodies / 814 `INCLUDE_ASM`**.
**Layout:** with most of the family now C the runs merged automatically — battle
is **7 TUs** (`src/battle/main{,2..7}.c`, yaml + `REFERENCE_ONLY_GAME_TUS`
updated), generated by `/tmp/grok-goal-fed9e9423187/implementer/gen2.py`, which
derives contiguous runs from the verified-body whitelist and takes bodies from
the sources plus `RUN_*.c` drafts.
**Proof:** all 40 bodies compared byte-for-byte against `disc/battle.bin`
(40/40 identical) and `build/out/battle.bin` == retail (`1830b4ef…`); rom-check
`battle PASS`; port LINK OK 72 stubs, unchanged `xeno-port` `f808d261…`; Map0 x2
`RUN_RC=124`.
**Differentials:** the family test now drives all 28 family bodies (plus
`func_8007B958`) against the retail slices — PASS at O0/O2/UBSan with **8
mutants rejected**; the leaf test still passes (6 mutants).
**Still blocked (next):** `func_8007EEE8` (the 8-iteration `D_800C3EB7` scan
still scores 14/23 on the shape harness) and the 0x8007B8D4..0x8007BB2C setters
(`func_8007B9C8`, `func_8007B98C`, `func_8007B914`, `func_8007B8D4`,
`func_8007BA88`, `func_8007BAB8`, `func_8007BAE8`, `func_8007BB2C`,
`func_8007BA04`, `func_8007BA44`) — the same recipe should apply, but each needs
its field/operator order read off its listing.

## Battle: setter family surveyed, `func_8007B958` landed (27 bodies total)
**Landed (byte-exact):** `func_8007B958` — `pRow[p[2]] = pRow[p[1]]` on the
`D_800D3430` row table (0x34 bytes). Battle is now **27 C bodies / 827
`INCLUDE_ASM`**, modelled as **16 run-pair TUs** (`src/battle/main{,2..16}.c`);
`config/battle.yaml` and `REFERENCE_ONLY_GAME_TUS` match that count.
**Proof:** `build/out/battle.bin` == `disc/battle.bin` (`1830b4ef…`); rom-check
`battle PASS`; port LINK OK 72 stubs / unchanged `xeno-port` `f808d261…`;
Map0 x2 `RUN_RC=124`.
**Differentials:** the family test now also covers `func_8007B958` (its own
slice + a distinct-byte copy fixture) — **PASS at O0/O2/UBSan, 6 mutants
rejected**; the leaf test still passes (6 mutants).
**The 0x8007B8D4..0x8007BB70 setter family (11 bodies, 167 insns) was surveyed
and is now the recorded frontier:**
* 9 of 11 compile to retail's exact size and instruction kinds but differ in
  register allocation (same wall as the 14 reader-family functions).
* `func_8007BA04` / `func_8007BA44` are also size-off: retail keeps one table
  base and derives the second row with `addiu base,0x10` before the row add
  (`addu a2,a1,v0` / `addu a1,a1,v0`), while GCC either materialises a second
  base (+4/+8 bytes) or folds `pRow + 0x10` into the load offset (−8). Both
  forms were tried; the trick is the ordering `row = idx<<6 + base` **then**
  `base + 0x10` **then** `row2 = idx<<6 + (base+0x10)`, which no source shape
  tried so far reproduces.
* The two stores into the 0x170-byte records (`func_8007BAE8` / `func_8007BB2C`)
  are size-exact and only allocation-different, so the whole family is a good
  next target once register shaping works.
**Process note (important for the next session):** the asm listing tree is
*derived from the C sources* — a generator that rebuilds `src/battle/main*.c`
from `asm/battle/**` is circular. Mid-turn I rewrote the sources from a listing
set that a failed build had already shrunk, which silently dropped ~166 stubs;
recovery was: fix the yaml, rebuild so splat re-emits every listing, then re-run
the generator with the *union* of `nonmatchings/` **and** `matchings/` listings
(856 entries = 854 functions + 2 data). The generator lives at
`/tmp/grok-goal-fed9e9423187/implementer/gen_tus.py` with the run list, the
family declarations and `RUN_B8D4.c` / `R1.c` / `R2.c` / `R3.c` beside it.

## Battle: 14-function accessor family landed, overlay now 15 run-pair TUs
**Landed (byte-exact, all sizes == retail):** `func_8007E934`, `func_8007E954`,
`func_8007E9D0`, `func_8007EA4C`, `func_8007EAC8`, `func_8007EB50`,
`func_8007EBD8`, `func_8007EC54`, `func_8007ECDC`, `func_8007ED58`,
`func_8007EE70`, `func_8007EEA8`, `func_8007EED0`, `func_8007EF44` — the
board/panel accessor family inside 0x8007E8AC..0x8007EF6C. Battle now has **26 C
bodies** (842 -> 840 `INCLUDE_ASM`? no: 854 - 26 = **828** stubs left).
**Layout:** with scattered exact bodies a run can no longer end at each one, so
the overlay is **15 TUs** `src/battle/main{,2..15}.c`, each = [asm region][C run]
following the shop_menu pattern; `config/battle.yaml` has the 15 matching
entries and `REFERENCE_ONLY_GAME_TUS` lists all 15.
**Proof:** `build/out/battle.bin` sha256
`1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291` ==
`disc/battle.bin`; rom-check `battle PASS`; port LINK OK 72 stubs / 3 unfound and
unchanged `xeno-port` `f808d261…`; Map0 x2 `RUN_RC=124` `primSubmits=2`.
**Differentials:** new `run_battle_board_family_7e8_retail_test.sh` (+ test C)
runs the retail slices on the MIPS adapter against the shipped C — **432 checks
PASS at O0/O2/UBSan, 5 mutants rejected**; the existing
`run_battle_leaf_setters_bf6cc_retail_test.sh` now compiles the 14 C-bearing TUs
and still passes (6 mutants).
**Matching findings (reusable):**
* Explicit load temporaries (`u32 a = ...; u32 b = ...; return f(a,b);`) fixed
  three bodies that were one register-flip away from retail.
* The remaining 14 family functions already have retail's exact size and
  instruction *kinds*; only register allocation and the `addu` operand order
  differ. Statement order and operand order (`x + row` vs `row + x`) do **not**
  steer GCC 2.7.2 here — they were tried and normalised away.
* Retail keeps `D_800D3410` / `D_800D3420` / `D_800D3430` exactly 0x10 apart.
  Model that with one fixture block plus `.set` symbol aliases: relying on
  declaration order breaks at -O2 (measured 0x606ab0/0x606aa0/0x6069a0).
* Bridge a guest buffer call by writing guest RAM, never by handing the guest
  pointer to a host helper (that segfaulted); and keep fixture indices inside the
  modelled tables, otherwise the unguarded readers walk off the host arrays.
* Differential mutants need discriminating fixtures: the family tables carry
  `{0x0F, 0xF0}` (period 2) and the board's row selectors follow the tested
  index, which is what makes `& vs ^`, `>= vs >`, `& vs |` and the `>> 7` /
  `index + 3` mutants fail as they should.

## Battle overlay: shop_menu-style run split, 12 bodies byte-exact (842 stubs left)
**Change (KEPT):** the three-way split is replaced by the repo's established
one-TU-per-retail-run pattern (the same one `shop_menu/main/misc{,2..9}.c` uses):
`config/battle.yaml` now declares `[0x1450, c, main]`,
`[0x1A1DC, c, main2]`, `[0x1B0C8, c, main3]`, `[0x4FC40, c, main4]`, and the
stubs+C bodies live in `src/battle/main{,2,3,4}.c` — each TU is "the unmatched
region followed by the C run after it", exactly the layout cc1 produces inside
one TU. `main.c` also keeps the eight port-only `#ifndef XENO_PC_PORT` bodies;
all four TUs are in `REFERENCE_ONLY_GAME_TUS`.
**Landed bodies (sizes == retail, verified in the built object):**

| run | functions | retail sizes |
|---|---|---|
| 0x80089BEC..0x80089CCC | `func_80089BEC`, `func_80089C08`, `func_80089C24`, `func_80089C48`, `func_80089C6C`, `func_80089C9C` | 0x1C, 0x1C, 0x24, 0x24, 0x30, 0x30 |
| 0x8008AB4C..0x8008ABB8 | `func_8008AB4C`, `func_8008AB70`, `func_8008AB94` | 0x24 x3 |
| 0x800BF6CC..0x800BF730 | `func_800BF6CC`, `func_800BF6F8`, `func_800BF720` | 0x2C, 0x28, 0x10 |

**Proof:** `build/out/battle.bin` sha256
`1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291` ==
`disc/battle.bin` (`cmp` clean); rom-check `battle PASS`; port LINK OK 72 stubs /
3 unfound with an unchanged `xeno-port` `f808d261…`; Map0 x2 `RUN_RC=124`
`primSubmits=2` `plain=7`. Battle `INCLUDE_ASM` 854 -> **842**.
**Differential:** `run_battle_leaf_setters_bf6cc_retail_test.sh` now compiles
`main.c`+`main2.c`+`main3.c`, runs the three retail slices on the MIPS adapter
(tables and scalars mapped to their retail addresses, `func_800BD2E4` and
`ArchiveSetIndex` bridged) and compares 514 checks at O0/O2/UBSan; 6 mutants
rejected. Two harness rules learned: pass arguments to the emulated call
(`gpr[4]/gpr[5]`), and only compare indices inside the 16-entry tables — retail's
unguarded readers would read neighbouring memory that the fixture does not model.
**Codegen findings worth reusing:**
* `0xFFFF ^ (u32)table[index]` reproduces retail's load-delay `nop` + `nor`
  (`~` lets GCC fill the return slot instead and loses 8 bytes).
* the guarded readers want the out-of-range test *first*
  (`if (index >= 0x10) return 0;`), which gives retail's `beqz` + `and` in the
  `j` delay slot; `if (index < 0x10) return …;` inverts the branch and costs 4.

## Battle overlay: region split + first byte-exact decompiled run (0x800BF6CC..0x800BF730)
**Why this exists:** `src/battle/main.c` was 854 `INCLUDE_ASM` stubs and nothing else,
and GCC emits *every* file-scope `__asm__` block before *any* compiled body, so a C
body added to a single-TU overlay lands at the end of the TU and shifts every
downstream address (measured: 19 scattered small bodies -> battle.bin +24 bytes,
37 561 diff runs, pin red). Battle's pin is green, so it may only be edited
byte-exactly.
**Change (KEPT):** battle's code region is now three ordered subsegments in
`config/battle.yaml`: `[0x1450, c, main_head]` (825 stubs,
`src/battle/main_head.c`), `[0x4FBDC, c, main]` (C bodies only,
`src/battle/main.c`), `[0x4FC40, c, main_tail]` (26 stubs,
`src/battle/main_tail.c`). The generated linker script then emits the three
`.text` blocks in retail order, so any contiguous run can be decompiled without
moving anything else. `pc_port/build_port.sh` lists both new TUs in
`REFERENCE_ONLY_GAME_TUS` (with reasons); `main.c` keeps its port-side
`#else` bodies.
**Landed bodies (byte-exact):** `func_800BF6CC` 0x2C (call `func_800BD2E4` while
`D_800C3610 != 0`), `func_800BF6F8` 0x28 (`D_80059464 - func_800BF720()`, call
materialised first exactly as retail), `func_800BF720` 0x10
(`D_800D2D68 != 0`, i.e. `sltu`). Built sizes 0x2C/0x28/0x10 == retail.
**Proof:** `build/out/battle.bin` sha256
`1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291` ==
`disc/battle.bin` (byte-identical, `cmp` clean) with the three functions now C;
rom-check `battle PASS`; port LINK OK 72 stubs / 3 unfound and unchanged
`xeno-port` `f808d261…`; Map0 offscreen x2 `RUN_RC=124` `primSubmits=2`
`plain=7`.
**New differential:** `pc_port/tests/run_battle_leaf_setters_bf6cc_retail_test.sh`
(+ test C) runs the three retail slices from `disc/battle.bin` on the MIPS
adapter (slice sha `71a92b38…` for 6CC, only `func_800BD2E4` bridged; the three
globals are mapped to their retail addresses) and compares against the shipped C:
16 seeds x return values + call trace, **PASS at O0/O2/UBSan**, 3 mutants
rejected.
**Tooling finding (blocks a class of leaves):** retail stores to an absolute
global (`sw $a0,%lo(D_800C3628)(v0)` in the `jr` delay slot) cannot be reproduced
from `D_800C3628 = value;` — all three shipped cc1 builds emit the gas macro form
`sw $4,D_800C3628`, and GNU as expands it to `lui $at,…; sw …($at)` **without**
filling the delay slot (maspsx passes it through; `--aspsx-version=2.56/2.67` and
`--print-output` all show the same). So `func_800BF730` (and the other tiny
global setters: `func_800B8048`, `func_800BC3F8`, `func_800BC454`,
`func_800BCD8C`) stay `INCLUDE_ASM` until the store macro matches retail. Symbol
*loads* expand correctly (`lui v0` + `lw` + filled slot), so getters are fine.
**Census after this run:** battle `INCLUDE_ASM` code lines 854 -> 851; slus 74;
battling 169; shop_menu 14; field/menu/member_change 0.

## `func_80024730` differential + two behavioural fixes (retail 0x1A4, built 0x164)
**Body:** `src/slus_006.64/system/temp1.c` (was landed earlier today with *no*
differential). Retail's end address had been recorded as 0x800248C0; the next
retail symbol `func_800248D4` starts at **0x800248D4**, so the body is
**0x1A4 = 420 bytes / 105 insns**, not 0x190 — the "0x174 vs 0x190" note was
measured against a truncated end.
**Defects found by the new differential (both FIXED from the listing):**
1. case 7 binds the timer callback on **pOwner**, not `pOwner+0x1C`. At
   .L80024878 the `jal TimerWorkListSetTaskCallback` still carries the incoming
   a0 = pOwner; `addu $a0, $s2, $zero` (s2 = pOwner+0x1C) is the delay slot of
   the *following* `j .L800248B0`, so it feeds the tail `func_80025224` call.
2. cases 10/11/12/13 store **before** `func_800BC158`, not after: retail's
   `sh $zero,0x34($s0)` (10/11) and `sh $v1,0x34($s0)` + the masked
   `sw $a1,0x40($s0)` (12/13) sit in that call's delay slot.
**New differential:** `pc_port/tests/run_temp1_anim_state_dispatch_24730_retail_test.sh`
+ `temp1_anim_state_dispatch_24730_retail_test.c`. Slice 0x1A4 sha
`e22425f5d12b1260481993beac518a3e6b3dca10b21cf51ab2e2360c92937bec` and
`jtbl_800186A4` (0x3C bytes, sha
`ffe5f84654a036e6c1119c9bdc5ad885e44837fef30040c49df6fb15c36fbe60`) are read
from the disc and the table is mapped into the emulator, so the case targets are
retail bytes; all 15 targets are asserted to land inside the slice. 16 state
values × 8 seeds = **128 retail-oracle comparisons PASS at O0/O2/UBSan**
(memory + call-arg + per-call snapshot), 6 mutants rejected. Callees
`func_800BC158`, `TimerWorkListSetTaskCallback`, `func_80025224` are bridged; the
case-7 callback is normalised between retail 0x80022E8C and the host symbol.
**Gates:** `slus_006.64` 303084 -> **303068** sha
`61b675d2a0253d2d897c4d848f56ba084a99dd6bcd9fc424f158e3427b4720db`; field.bin
unchanged `761df27a…`; pins member/shop/battle PASS (slus/field known-red).
Port LINK OK, 72 stubs, normal 3 unfound, `xeno-port` `f808d261…`; Map0 x2
`RUN_RC=124` `primSubmits=2` `plain=7`. The port's own `func_80024730` shim in
`game_overrides.c` still owns the live path (manifest row), so runtime behaviour
is unchanged there.
**Lesson:** take a function's end from the *next retail symbol*, never from a
comment — a 0x14-short slice ran past `jr $ra` into the following function and
produced a phantom second call, which looked like a C bug.

## `func_80023FD8` matching C 0x2c4 (retail 0x2bc, +8)
**Change (KEPT):** `src/slus_006.64/system/temp1.c` from
`.../temp1/func_80023FD8.s` (0x80023FD8-0x80024294, 175 insns). Sprite-script
spawn: `pScriptTable = *(u32*)(package + 0x10)`, entry = table +
`*(u16*)(table + index*2 + 2)`, `type = func_80023440(entry)`,
`mode = func_80023468(type, a1)` where retail still has the live a1 = package
(observable only on the case-3 fall-through), then
`func_80023A48(type, mode, package, extraSize, NULL)`. wrapper+0x14 |=
0x20000000; `source` is node+0x24 **loaded after the allocation** and re-stored
in the tail (retail keeps it in a register across the optional block); the
`D_800591AD` / `D_800C3E1C` inheritance block copies the player render/tint/
colour state, keeps the **two separate** +0x7C transfers, and re-joins
`(player+0xA8 >> 30) | ((player+0xAC & 3) << 2)` into node+0xA8 bits 30-31 /
node+0xAC bits 0-1 (masks 0xFFFFE0FF, 0x1F00, 0xFFF8007F, 0x7FF80, 0x3FFFFFFF).
Tail: clear +0x44/+0x48/+0x34, re-store +0x24, fold `type & 0xF` into +0x40 bits
13-16 and `mode & 3` into +0x3C bits 0-1, `(u16)D_800591A8` to +0x82, the three
s16 script values <<16 into +0x0/+0x4/+0x8, then `func_80023538(node, entry)`
and `func_80024730(wrapper)`.
**Size 0x2c4 vs retail 0x2bc (+8)**, frame 0x38 exact. `slus_006.64` 303076 ->
303084 sha `5b6e1874f538a4778b271cee64b0c5be5fa5d3c46bd6538d76b4d07a8f24b183`;
field.bin unchanged `761df27a…` (249290). slus `INCLUDE_ASM` code lines 75 -> 74.
**Differential (NEW):** `pc_port/tests/run_temp1_sprite_spawn_23fd8_retail_test.sh`
+ `temp1_sprite_spawn_23fd8_retail_test.c`. Loads the retail 700-byte slice
(its sha256 `6291763a42ce40bf3073a031287166b8401aa0e6655b0cd4a47cfad702393c65` is
asserted), runs it on the MIPS adapter with the five callees bridged to
controlled stubs, and compares the *shipped* temp1.c body on return value, full
fixture memory and a 5-call argument+memory snapshot: **3072 comparisons PASS at
O0/O2/UBSan**, 8 mutants rejected (entry offset, gate, inheritance offset, split
shift, duration, tag bit, a1 pass-through, zero-store).
**Harness recipe (new, reusable):** compile the whole TU and
`objcopy --weaken-symbol=func_80023440/23468/23A48/23538/24730`, then add
`-fno-ipa-ra` to that TU's compile flags — without it GCC uses the TU-local
(weak) bodies to assume those calls leave caller-saved registers intact and the
substituted stubs are called with scrambled arguments (symptom: O0 passes, O2
fails with wrong `a1`). Position stores use the file's existing
`(u32)pPosition[N] << 16` idiom (same `lh`+`sll`, no UBSan signed-shift error).
**Port:** the matching owner is `__attribute__((weak))` under `XENO_PC_PORT`;
`pc_port/src/sprite_constructor.c` keeps the strong host-pointer body. A
`port_owned_overrides.txt` row is NOT possible here — that manifest validates
every symbol against `game_overrides.c.o` only. LINK OK, 72 function stubs, the
normal 3 unfound symbols, `xeno-port`
`01d4fafcb791190164d4aaed92f1743fc0e2794fefacdeed10fbfdc40f4d2e10`. Map0
offscreen x2: `RUN_RC=124`, `primSubmits=2`, `plain=7`, `active=22`.
**asm-vs-C:** 175 retail vs 177 built insns; all five `jal` sites and the whole
mask/offset set match (opcode-only 78.98%, exact text 23.86%). The residue is
scheduling/register allocation: prologue save order, base register choice,
`move` vs `addu`, hoisted `lui`+`ori` constants, two extra `nop`s. Not byte-exact.
**Next:** other open host-compilable bodies `func_8002BB50`, `func_8002AC24`,
`func_80019578`, `func_8001A6E8`, `func_80023B84`; owed differentials for
`func_800257F0` and `func_80024730`. Blocker unchanged: the 0x4CC rodata-layout
drift (560/562 symbols at drifted addresses) keeps the slus pin red.

## `func_800257F0` matching C 0x250 (retail 0x298, -0x48)
**Change (KEPT):** `src/slus_006.64/system/temp1.c` from
`.../temp1/func_800257F0.s` (0x800257F0-0x80025A74). Sprite transform/render
setup: `func_80022038(pSprite)` where `pSprite = *(u8**)(pArg + 4)`; bail when
`*(pData + 0x34) == 0` (`pData = *(u8**)(pSprite + 0x20)`); an optional
colour/light block when bit 1 of `*(u32*)(pSprite + 0x40)` is set (PushMatrix,
three u16 matrix words from `pData + 0x4C/0x4E/0x50` into `D_8004FD80`,
`RotMatrix(pData + 0x44, &matA)`, `MulMatrix0(&matA, pData + 0x0C, &matA)`,
`MulMatrix0(&D_8004FDA0, &matA, &matB)`, `SetBackColor(0x20,0x20,0x20)`,
`SetColorMatrix(D_8004FD80)`, `SetLightMatrix(&matB)`, PopMatrix); the
+2/+6/+0xA translation through `TransMatrix(pData + 0x0C)`; then either
`CompMatrix(&D_8004FBB8, pData + 0x0C, &comp)` + `SetRotMatrix`/`SetTransMatrix`
on the composite, or the raw matrix for both; the geom-offset override when
`*(s32*)(pSprite + 0x3C) < 0` (`ReadGeomOffset` save + `SetGeomOffset(0xA0,0x70)`,
restored at the end); and `func_800B1F6C` with either `*(s16*)(pSprite + 0x30)`
and `D_80050100` left alone, or `D_80050100 = 0x10` with 0xFEC (saved/restored).
**Size 0x250 vs retail 0x298 (-0x48).** `slus_006.64` 303148 -> **303076**.
Port: LINK OK, Map0 `RUN_C=124` `primSubmits=2`.
**Declaration lesson applied and extended:** checking the *preprocessed* TU caught
four of the eight symbols already visible (`D_8004FBB8`, `D_80050100`,
`g_GfxCurOT`, `g_GfxCurContext`), but `func_800B1F6C` was declared with an
*unprototyped* form that my regex missed — the safe pattern for calls into such
symbols is a local cast at the call site plus a K&R `extern void f();`.
**OWED:** a differential for this body (matrix calls into libgte make it
awkward; the useful seam is `func_800B1F6C` + `func_80022038` stubs plus checks on
`D_80050100` save/restore and the `ReadGeomOffset`/`SetGeomOffset` pair).

## misc4 `func_8007D3D4` — table-4 dispatch emits retail's jtbl_8006FBEC order

**Change (KEPT):** `src/field/main/misc4.c` `func_8007D3D4` only.
1. The eight-way `switch (sideMask)` now writes its case labels in retail's
   body-emission order **0,3,5,1,6,2,4,7**. The order is recovered from
   `jtbl_8006FBEC`'s target addresses in
   `asm/field/data/main/misc4.rodata.s` (0x8007D5A8, 0x8007D5B0, 0x8007D5D0,
   0x8007D5E8, 0x8007D604, 0x8007D61C, 0x8007D638, 0x8007D654). Index i still
   maps to case value i (verified against `asm/field/matchings/main/misc4/
   func_8007D3D4.s`: 0x8007D5A8 `steps=0xFF`; 0x8007D5E8 `lh tri[3]`;
   0x8007D61C `lh tri[4]`; 0x8007D5B0 `bltz -> tri[3] / j -> tri[4]`;
   0x8007D638 `lh tri[5]`; 0x8007D5D0 `bltz -> tri[5]` falls into tri[3];
   0x8007D604 `bgez -> tri[5]` falls into tri[4]; 0x8007D654 `addiu -1`), so
   the reorder is behaviour-neutral (every body still ends in `break`).
2. Each body now recomputes the neighbour triangle address as
   `*(s16*)((u8*)triBase + triIndex * 14 + {6,8,10})` instead of indexing the
   cached `tri` local. Retail does exactly this: cases 1/2/4 at
   0x8007D5E8/0x8007D61C/0x8007D638 each emit `sll $v0,$s1,3; subu $v0,$v0,$s1;
   sll $v0,$v0,1; addu $v0,$v0,$s5; lh ...`, and cases 3/5/6 branch into the
   middle of those shared suffixes (`bltz -> 0x8007D5EC`, `bgez -> 0x8007D63C`,
   `0x8007D5D0` falls through to 0x8007D5E8).

**Why the second step was required:** GCC 2.7.2 lays switch case bodies out by
CFG, not source order. With the cached `tri[3..5]` form the built `.rdata` table
ranked **(0,2,6,1,5,3,4,7)** no matter how the labels were ordered (verified by
brute-forcing all 720 middle-case permutations with pinned `tools/gcc-2.7.2-psx/
cc1` on a structural model); only the recompute form makes source order
0,3,5,1,6,2,4,7 canonicalise to retail's order. `?:` vs `if/else` made no
difference.

**Evidence:**
- Built jtbl ranks (labels in `build/src/field/main/misc4.c.s` vs its `.rdata`
  table) = **(0,3,5,1,6,2,4,7)** = retail `jtbl_8006FBEC`. The other misc4
  tables stay at their correct ranks (`func_8007BEF4`, `func_8007C694` identity).
- `func_8007D3D4` **1044 B -> 1108 B** (retail 1092; was -48, now +16).
- `field.bin` `761df27a` (249290) -> **`c4e6d5ec` (249354)**, toward retail
  260862. `field_RODATA_SIZE` stays **0x2FC** (misc4.c.o .rodata 0x80 at 0x9C).
- From-clean `tools/scripts/check_rom_hashes.sh`: **member_change PASS
  3b9e2b89, shop PASS 7890e14b, battle PASS 1830b4ef**; slus `61b675d2`
  (unchanged) / field `c4e6d5ec` remain known-red (field moved toward retail).
- NEW `pc_port/tests/run_field_walkmesh_d3d4_dispatch_retail_test.sh` +
  `field_walkmesh_d3d4_dispatch_retail_test.c`: asserts the retail
  `func_8007D3D4` slice sha256 `f0f4f0d2…` (1092 B), drives the shipped body
  with controlled `NormalClip` signs through all 8 masks and both arms of
  3/5/6, and checks the accepted triangle + consumed-NCLIP count — **33 checks
  PASS at O0/O2/UBSan**, `FIELD_WALKMESH_MUTANT_D3D4_SWAP12` (case 1<->2)
  rejected. Existing `run_field_walkmesh_compound_edge_test.sh` (BEF4) and
  `run_field_walkmesh_cd80_case7_retail_test.sh` still PASS.
- Native port: LINK OK, **72 function stubs**, 569 data symbols,
  `xeno-port` `9deee82b7e4b4cca…` (final rebuild after a comment-only edit; the
  earlier identical-code build was `108364d4243ab200…`, so the port binary hash
  carries debug line info), 3 unfound gte symbols as before. Map0 offscreen
  smoke **x2**: `RUN_RC=124`, `primSubmits=2`, `DrawOTag=1`, `active=22`,
  `plain=7`, `globalSkip=0`, no asserts.

**Remaining uncertainty / next executable step:**
- The +16 B residue: built case bodies re-sign-extend `triIndex`
  (`sll $3,$21,16; sra $3,$3,16`) where retail keeps the `lh`-sign-extended
  value once. That is a register-allocation follow-up, not a case-order issue.
- `func_8007CD80` (misc4 table 3, `jtbl_8006FBCC`) still ranks
  (0,2,6,1,5,3,4,7) where retail is identity (0,1,2,3,4,5,6,7). It has the
  same cached-`tri` + conditional shape; apply the same recompute conversion
  next (its `gte_NormalClip(nclip)` form needs the address expression in each
  arm).
- Verified separately this pass (no change): the prior handoff's claim that
  `func_80080A74` is still -104 B is **stale**. A fresh rebuild of the current
  `src/field/main/misc8.c` gives `func_80080A74` = **0x4D0 = 1232 B = retail**
  exactly, with `field_RODATA_SIZE` = 0x2FC; the zero-init template removal
  already achieved the size target. No edit was needed.

## slus `func_8003CEF0` coexistence -> MATCHED, and the sound.c compiler is 2.6.0

**Two findings; one function landed.**

1. **`src/slus_006.64/system/sound.c` is compiled by `gcc-2.6.0-psx`, not
   2.7.2.** `build.ninja` line 709 sets `gcc = gcc-2.6.0-psx` for that TU (the
   global rule is `gcc-2.7.2-psx`; `slus_006.64` also has per-TU 2.6.0 for
   `graphics/line_scroll`, `system/controller`, `system/libarchive`,
   `system/menu`, member_change_menu and shop_menu). The "Residual: ...
   scheduling ..." notes on the sound *coexistence* bodies were measured under
   the wrong compiler. Re-measured under 2.6.0, the port body of
   `func_8003D0E8` already matches retail in **9 of 10** instructions (only a
   redundant `andi $v0,$v0,0xFF` is missing); several other parked handlers are
   likely equally close.

2. **`func_8003CEF0` landed as MATCHED C** (the `#ifdef XENO_PC_PORT`
   coexistence guard is removed — matching and port now share one body). The
   fix is the recorded "retail reloads memory" lever: the selector must be
   re-read from memory for the record base,
   ```c
   *(u16*)&pAudioElements->unk_0x70[2] += 1;
   rec = (u8*)pAudioElements + (*(u16*)&pAudioElements->unk_0x70[2] * 12 + 0x9C);
   ```
   not cached in a `u16 sel` local. Retail 0x8003D6FC `sh $v1, 0x72($a2)`
   (increment store) is followed by 0x8003D700 `lhu $a1, 0x72($a2)` (reload);
   the sibling `func_8003CF38` below already used this idiom.

**Evidence**
- Retail slice `asm/slus_006.64/nonmatchings/system/sound/func_8003CEF0.s`
  (file offset 0x2D6F0, 72 B): the shipped body compiled with pinned
  `tools/gcc-2.6.0-psx/cc1` + `tools/maspsx` is **byte-identical**, sha256
  `8859c93cd23efd47e7e4cf3ecf6216477139e82ec670d20e18547d3eb9a79a1c`
  (verified standalone and again at built address 0x8003DC50 in
  `build/out/slus_006.64` after a from-clean rom-check rebuild).
- NEW differential `pc_port/tests/run_sound_voice_record_cef0_retail_test.sh`
  (+ `sound_voice_record_cef0_retail_test.c`): asserts the retail SLUS sha256
  and the slice sha256, drives the shipped body over 5 selector/byte/octave
  cases and checks the return, the selector increment,
  `rec[0] == byte + 0xFF`, `*(u32*)(rec+4) == script+1`, `rec[2] == (u8)octave`
  and that the neighbouring record slots are untouched — **O0/O2/UBSan PASS**;
  a `0x9C -> 0x98` record-base mutant is rejected.
- From-clean `tools/scripts/check_rom_hashes.sh`: member_change PASS 3b9e2b89,
  shop PASS 7890e14b, battle PASS 1830b4ef; field c4e6d5ec unchanged; slus
  `61b675d2` -> **`7f65157d`**, **size unchanged 303068**. The slus hash moved
  because the function left the `INCLUDE_ASM` `.text` asm-blob run and joined
  the C-function run, reordering the TU's symbols (the overlay is red for the
  separate 0x4CC rodata drift; per-function exactness, not the overlay hash, is
  the oracle). slus `INCLUDE_ASM` 74 -> **73**.
- Port: LINK OK, function stubs **72 -> 69**, `xeno-port`
  `407412019b932967…`; Map0 offscreen smoke **x2** `RUN_RC=124`,
  `primSubmits=2`, `DrawOTag=1`, `active=22`, `plain=7`, no asserts.

**Remaining uncertainty / next executable step**
- Re-audit the other `system/sound` coexistence bodies under the correct
  `gcc-2.6.0-psx`. `func_8003D0E8` needs only the redundant
  `andi $v0,$v0,0xFF`; `func_8003D110`, `func_8003D7C8`, `func_8003D13C` are
  the next smallest. (My host cc1 experiment for `func_8003D110` under 2.7.2
  also missed retail's 8-byte stack frame, so re-check that one under 2.6.0
  first.)
- The `INCLUDE_ASM` -> C conversion reorders the slus object even for a
  byte-exact body; do not treat a slus hash change as a regression when the
  size and per-function slices are unchanged.

## slus `func_8003D0E8` MATCHED — the volatile-load lever

`func_8003D0E8` (seq cmd 0x90, set manager volume) is landed as MATCHED C
(coexistence guard removed; matching and port share one body), 40 B.

Retail (file offset 0x2D8E8) masks the loaded volume byte with a **redundant**
`andi $v0,$v0,0xFF` between `lbu` and `mult`. Under the TU's real gcc-2.6.0 a
plain `pScript[0]` folds the `lbu` + zero-extend into one `lbu`, so the `andi`
never appears; ~30 source variants were tried (casts, locals, `& 0xFF`,
`u16`/`u32` temps, unions, volatile locals) and none reproduced it. Reading the
byte through a **non-combinable (volatile) lvalue** does:

```c
s32 vol = *(volatile u8*)pScript;
pAudioManager->unk_0x54 = vol * ((s16*)&pAudioManager->unk_Interpolator_0x64.currentValue)[1];
pAudioManager->unk_0x58 = vol << 16;
return pScript + 1;
```

The volatile qualifier only blocks the `lbu`/zero-extend combine; the read is a
single byte and port behaviour is unchanged. This is the second coexistence
sound handler retired with a codegen lever after `func_8003CEF0`.

**Evidence**
- Retail slice at 0x2D8E8, 40 B: shipped body via pinned
  `tools/gcc-2.6.0-psx/cc1` + `tools/maspsx` is **byte-identical**, sha256
  `4050994410be4dec6f6cc913d684c4e075b5f79fdb19e6401f79204f82d6f1e9`
  (verified standalone and at built addr 0x8003DE20 in `build/out/slus_006.64`
  after a from-clean rom-check rebuild; `func_8003CEF0` re-checked exact at the
  same time).
- NEW differential `pc_port/tests/run_sound_voice_volume_d0e8_retail_test.sh`
  (+ `sound_voice_volume_d0e8_retail_test.c`): asserts the retail SLUS and slice
  sha256, drives the shipped body over 48 vol/interpolator cases checking
  `unk_0x54 == vol*iv`, `unk_0x58 == vol<<16`, the return pointer and that the
  rest of the manager is untouched — **O0/O2/UBSan PASS**, `vol << 16` ->
  `vol << 8` mutant rejected.
- From-clean `tools/scripts/check_rom_hashes.sh`: member_change/shop/battle
  PASS; field c4e6d5ec unchanged; slus `7f65157d` -> **`0bb5827f`**, size still
  **303068**. slus `INCLUDE_ASM` 74 -> **72** this session.
- Port: LINK OK, 69 function stubs, `xeno-port` `cd99bc11…`; Map0 offscreen
  smoke **x2** `RUN_RC=124`, `primSubmits=2`, `active=22`, `plain=7`.

**Remaining uncertainty / next executable step**
- `func_8003D110` (retail 44 B) has an unexplained **8-byte stack frame**
  (`addiu sp,sp,-8` … `addiu sp,sp,8; jr ra; nop`) that no normal source form
  under 2.6.0 produced (15 variants tried) — park until an idiom is found.
- `func_8003D13C` is one `addu $a2,$a3,zero` short (retail keeps `steps` in
  `$a3` for the `sh $a3,0x60` store and a `$a2` copy for `div`); the port body
  and a swapped-store variant both miss it.
- `func_8003D7C8` differs only in load order (`lbu,lbu,lhu` vs `lbu,lhu,lbu`)
  and register allocation; 7 variants tried, none matched. The volatile lever
  does NOT help here (retail has no redundant mask).
- The general lever for this TU is: **retail reloads derived values from
  memory / keeps operations non-combinable**; re-read the coexistence bodies
  under gcc-2.6.0 with that in mind.

## Sound coex audit + a menu compiler/verification lead

**`func_8003D1BC` (sound.c seq cmd, 76 B) — NOT landed, CSE-locked.** Retail
computes `sll $t0,$a0,5` **and** `sll $a0,$a0,5` from a single `lbu $a0,0($a2)`
(the counter store and the divide use the same `p[0] << 5` expression but GCC
kept two copies). Every source shape tried under gcc-2.6.0 collapses them:
- cached byte (`s32 b0 = p[0]`) -> **one** `sll` (GCC CSEs across the branch),
- recompute from memory -> an extra `lbu` reload (matches `d1bc5`, 20 insns vs
  retail 19),
- cached byte + recompute -> one `sll`.
The closest form (`d1bc5`) is retail **minus** the pointer copy and **plus** a
reload; `d1bc3` (cached `p[1]`, cached `p[0]`) is retail minus the
`addu $a2,$a0,zero` copy and one `sll`. Retail's `addu $a2,$a0,zero` is a
register-pressure artefact of the duplicate shift. Parked.

**Redundant-`lbu`+`andi` scan:** a regex scan of all of `asm/slus_006.64/**`
for `lbu $r, …` followed within 2 insns by `andi $r,$r,0xFF` finds only
`func_8003D0E8` (now matched) plus functions whose mask is the
arithmetic-truncation idiom `(u8)(x + 1)` (`func_8003DAB0` etc., already
matched) — the volatile lever has no other slus target. Scan script kept at
`/var/tmp/scan_andi.py`.

**Menu lead (not changed this pass).** `func_801DE29C`'s retail bytes
(`disc/menu.bin` offset 0x1929C, 44 B) are reproduced **exactly by
gcc-2.6.0-psx** (only the `jal` relocation differs) but **not** by
gcc-2.7.2-psx, yet `build.ninja` compiles `src/menu/main/misc.c` with the
global 2.7.2 default (line 918 has no `gcc` override; 2.6.0 overrides exist for
`graphics/line_scroll`, `system/controller`, `system/libarchive`, `system/menu`,
member_change_menu, shop_menu). The retail epilogue style
(`addiu sp,sp,N; jr $ra; nop`, delay slot **not** filled) is the 2.6.0 shape.
Separately, `func_801C57A4` sits in `asm/menu/matchings/` (i.e. treated as
MATCHED C) but its built body
(`subu sp,sp,24; sw $16,16(sp); li $16,1; sw $31,20(sp); sb $16,D_80059171`)
does **not** match retail
(`addiu sp,sp,-0x18; ori $v0,0,1; sw $31,0x14(sp); sw $16,0x10(sp); …`) under
**either** compiler. So the menu overlay's inferred-MATCHED count (173) is not
verified byte-exact — the overlay is unpinned (153596 vs 153864 B), so nothing
caught it. Next agent: (a) decide the menu TU compiler from a larger sample,
(b) re-verify menu C bodies before counting them as matched. Do **not** flip the
compiler without re-checking the pin-safe overlays.

## Battle small-body probe (all parked; the blocker is GCC's constant hoist)

Battle is the largest remaining block (614 unported). Four small bodies were
driven to exact deltas under the standard `gcc-2.7.2-psx` pipeline
(`main16`/`main22`/`main21` use it; `mainc112` alone uses
`gcc-2.7.2-cdk-psx` + `--dont-expand-li` — verified via
`ninja -t commands build/src/battle/<tu>.c.o`):

- **`func_8007A92C`** (`main16`, offset 0xAE3C, 60 B) and **`func_8007B98C`**
  (`main22`, 0xBE9C) / **`func_8007B9C8`** (`main22`, 0xBED8, u32 variant): the C
  is
  ```c
  void f(u8** ppBoard, u8 index) {
      u8* p = *ppBoard;
      u16* pRow = (u16*)(D_800D3420 + ((u32)index << 6));   /* 0x3410 for B9C8 */
      pRow[p[2]] = pRow[p[1]];
  }
  ```
  which is exactly the already-matched `main15` `func_8007A900`
  (`pRow[p[1]] = p[2]`) idiom. Instruction **multiset and count are identical**;
  the only delta is placement: retail emits `lw $v0,0($a0)` **then**
  `lui $a0,%hi / addiu $a0,%lo / addu`, while cc1 hoists `lui/addiu` **before**
  the `lw` and keeps the base in `$v0`. 10+ source orderings (cached row, cached
  bytes, inline base at the store, `u8` vs `u32` index, `u8*`+byte offsets) all
  reproduce the hoist — it is cc1's scheduler, not the expression.
  `func_8007A92C` additionally needs `lui $v1 / addiu $v1 / addu` (not a folded
  `at` access), matching the same non-hoisted shape.
- **`func_800BB314`** (`mainc112`, 0x4B824, 60 B) is the free-the-task body
  `WorkListRemoveTask(p+0x1C); TimerWorkListRemoveTask(p); HeapFree(p);` — the
  instruction sequence matches exactly, but retail's frame is **0x20** with
  saves at 0x18/0x1C, while cc1 emits **0x18** with saves at 0x10/0x14 (same as
  the unexplained `func_8003D110` 8-byte-frame class). Parked.

**Useful battle idiom recovered:** a `u8 index` parameter makes cc1 emit
`andi $a1,$a1,0xFF`, and the row pointer is written
`D_800Dxxxx + ((u32)index << 6)` (see the matched `func_8007A900`,
`src/battle/main15.c`). The unresolved lever is **preventing cc1 from hoisting
symbol-constant materialization above the pointer load**; solving that one
scheduler behaviour would likely unlock a large fraction of the ~614 battle
bodies at once.

## battle `func_8007E780` MATCHED (pinned overlay, pin stays PASS)

**Change (KEPT):** `src/battle/main29.c` — the `INCLUDE_ASM` for `func_8007E780`
is replaced with the MATCHED C body (plus `extern u8 D_800D3410[];`):

```c
void func_8007E780(u8** ppBoard, u8 index) {
    u8* p = *ppBoard;
    u32* pRow = (u32*)(D_800D3410 + ((u32)index << 6));

    pRow[p[1]] /= p[2];
}
```

This is the **first landed body in the `main29` board family** and the first
battle landing of the session. It is the cc1-favourable scheduling shape: the
symbol materialization `lui/addiu` lands **before** the pointer `lw` (unlike the
`func_8007A92C`/`func_8007B98C` family above), so no scheduler fight.

**Evidence**
- Retail slice at file offset 0xEC90 (vaddr 0x8007E780), 64 B, sha256
  `701e86f7db26624e4386aaa54826a84da53c9aebff392e8a8c714db039e4ad55`. The built
  body is **byte-identical** (only the `%hi/%lo` of `D_800D3410` are
  relocations) and sits at its retail address 0x8007E780 in `build/out/battle.bin`.
- **`battle.bin` stays `1830b4ef1fe37129` (343936 B) — the pinned overlay hash is
  unchanged** through a from-clean `tools/scripts/check_rom_hashes.sh` (unlike
  the slus `INCLUDE_ASM`->C reorder, this TU does not move). member_change/shop
  PASS; field/slus unchanged red.
- **Extended** `pc_port/tests/battle_board_family_7e8_retail_test.c` (+ its
  runner) with the `func_8007E780` case: the unit is run **twice on the same
  fixture** — once as the retail slice on the MIPS adapter, once as the shipped
  C — and the written row word is compared; the oracle quotient is also checked
  against `before / div`. `BOARD FAMILY 7E8 PASS checks=976` (was 944) at
  **O0/O2/UBSan** with identical output, and the new `quotient_to_product`
  mutant is rejected (11 mutants total).

**Remaining uncertainty / next executable step**
- Siblings `func_8007E6A0` (add) and `func_8007E6F0` (sub) are **one register
  off**: retail keeps the row base in `$a2` (`lui $a2 / addiu $a2 / addu
  $a1,$a1,$a2`), cc1 allocates `$v1`. Same shape, same schedule; 4 source
  orderings all give `$v1`. Also `func_8007E740` (mult) differs only in
  regalloc. These three are the immediate next candidates — only the register
  choice separates them.
- The landing proves `main29` (and the board-accessor family) is a viable
  battle vein: prefer bodies where the base is materialised **before** the
  pointer load.

## Full battle scan of that shape: 20 more candidates, all blocked on ONE
## allocator behaviour (base register)

A scan (`/var/tmp/scan_bbl.py`, pattern: `lui $R,%hi` / `addiu $R,$R,%lo`
immediately followed within 4 insns by `lw $Y,0($a0)`) found **21
nonmatchings** with the cc1-favourable base-before-load schedule, i.e. the
`func_8007E780` shape. They are the board-accessor family:

- base in `$v0`: `func_8007AA60 ABD8 AC30 AC80 ACDC AD24 AD6C ADB0 AE38 AE98
  AEF0 BA04 D0CC E740`
- base in `$a2`: `func_8007B2C0 B310 B360 E6A0 E6F0`
- base in `$v1`: `func_8007BA44`

Every one of the 17 tested reconstructs to the **exact retail instruction
sequence** (same opcodes, same order, same shifts/clamps) and then fails on a
single deterministic mismatch: **cc1 allocates the row base to the *other*
register** than retail, which cascades into a handful of bytes (7 bytes for a
2-value body like `func_8007E6A0`, 11–15 bytes for the 3-value ones). Concrete
deltas (retail → cc1):

| function | retail base | cc1 base | bytes |
|---|---|---|---|
| `func_8007E6A0` / `E6F0` | `$a2` | `$v1` | 7 |
| `func_8007B2C0` | `$a2` | `$v1` | 7 |
| `func_8007AD6C` | `$v0` | `$v1` | 11 |
| `func_8007AC30` | `$v0` | `$v1` | 15 |
| `func_8007AE38` / `AE98` | `$v0` | `$v0` (but `p`→`$a2`) | 41 / 18 |

Source-form levers that were tried and **do not** move the choice: declaring
the row before/after the pointer; `ppBoard[0]` vs `*ppBoard`; caching the
`p[]` bytes in locals; `u8*`+byte offsets vs `u16*/u32*` indexing; `(index<<6)`
vs `((u32)index<<6)`; separate declare/assign; `&=` vs `= pRow[..] & x`;
gcc-2.6.0-psx vs 2.7.2-psx (**identical output**). `func_8007E780` is the one
member where cc1 happens to pick retail's registers (`base $v0`, `p $a0`), and
it is landed above.

**Next lever to crack (high value):** find the C idiom that makes cc1 assign
the symbol-address pseudo to `$v0`/`$a2` instead of `$v1` for
`pRow[p[a]] OP ...` bodies. Landing that alone would unlock ~20 battle
functions on this list; the remaining ~590 battle bodies mostly need the
*other* behaviour (prevent the `lui/addiu` hoist above the `lw`).

## Port live field path: `[stub] SoundHandleError` is LOAD-BEARING (do not
## simply retire it) — root cause established

Work order for this session (user-approved): grow the native port from the
start menu forward along the **live field path**, one verified slice per pass.
The live Map0 smoke currently stops at `[stub] SoundHandleError`. Retiring it
was attempted and **reverted**; this section records why, with the full trace.

**What was tried.** `src/slus_006.64/system/sound.c:6164` already holds the
MATCHED C body for `SoundHandleError` (`[8003F6B0,8003F738)`), but it sat inside
`#ifndef XENO_PC_PORT`, so the port stubbed the symbol. Un-guarding it (one body
for both builds) built clean — **LINK OK, 69 → 68 function stubs** — but the
runtime **regressed from "renders 8 field frames, clean log close" to
`SIGABRT` before any frame.** Reverted; the tree is back at 69 stubs and the
8-frame baseline.

**A/B evidence.** Only the body changed (the symbol becoming real is harmless):
- body made a no-op → `RUN_RC=124`, 8 frames, `---- LOG CLOSED ----`, no abort;
- real body → `PsyX_SPUAL.cpp:1345 PsyX_SPUAL_Write` assertion
  (`size > 0 && wptr_ofs + size < SPU_MEMSIZE`) → core dump.

**Root cause chain (temporary `[trace]` instrumentation, since removed).**
1. The stub fires exactly once, during `[FieldMain] FieldLoadUITextures`
   (not from the `g_SoundControlFlags < 0` re-init guard — a trace shows
   `SoundInitialize arg0=0 flags=0x0000`, i.e. a normal first init).
2. `[trace]SoundHandleError id=0x1f flags=0xb901`. Id `0x1F` is emitted only by
   `SoundLoadWdsFile` (`sound.c:826`) when `SoundSpuMemoryAllocateWDS` returns 0.
3. The failing allocator is the *pinned-address* path, not auto:
   `[trace]ATADDR fail-gap size=0x25cf0 addr=0x12000 gap=0x0 blockEnd=0x37cf0`
   → `SoundSpuMemoryAllocateBlockAtAddress(0x25CF0, 0x12000)` returns 0 because
   SPU `[0x12000, 0x37CF0)` is **already allocated** (`gap == 0`: a resident
   block starts at or after the requested address and before `regionEnd`).
4. Note `SOUND_WDS_ALLOCATE_AT_ADDRESS == 0` (`include/system/sound.h:54`), so
   every `SoundLoadWdsFile(bank, 0)` call — including all four port boot-bank
   loads and `boot_sound_banks.c` — means **"pin at the bank header's
   `spuMemoryAddress`"**, not "allocate automatically". The field bank's header
   therefore claims SPU `0x12000` while something already owns that region.
5. Running the real body then makes the **recovery path itself** upload through
   `SoundLoadWdsFile(&D_80050940, 0)`, which trips the PsyCross SPU bound.

**Conclusion.** The stub is silently absorbing a real pinned-SPU-allocation
collision (a duplicate/overlapping WDS residency), so it cannot be retired in
isolation. `SoundHandleError` stays stubbed until the collision is fixed.

**RESOLVED (same session) — see the next section.** The collision was located,
fixed, and `SoundHandleError` is now retired with a green differential.

## Port landing: duplicate field WDS load removed; `[stub] SoundHandleError`
## retired (69 → 68 stubs)

**Two source changes, both kept:**

1. `src/field/main/misc3.c` (port block in the field boot init): the eager
   `func_80085FB8(); while (func_80085F30() == -1 ...) {}` common-bank load is
   replaced by retail's invariant `D_8004F364 = 1;`. The block's own comment
   already stated the intent — *"Retail boots with D_8004F364=1 ('field common
   WDS bank resident') because the ... flow loads it before any field entry; the
   port never has"* — and `port_main.c`'s `PcPort_LoadRetailBootSoundBanks()`
   now performs that retail `func_80019578` closure. So the field was loading
   the **same** bank twice. Bank identity proved by a temporary trace:
   ```
   [trace]WDSLoad mode=0 id=0x0000 size=0xee50  pinned=0x1010  ra=0x516f1c  (boot)
   [trace]WDSLoad mode=0 id=0x0026 size=0x25cf0 pinned=0x12000 ra=0x516f1c  (boot)
   [trace]WDSLoad mode=0 id=0x0001 size=0x5be0  pinned=0x72000 ra=0x516f1c  (boot)
   [trace]WDSLoad mode=0 id=0x0002 size=0x54e0  pinned=0x68000 ra=0x516f1c  (boot)
   [trace]WDSLoad mode=0 id=0x0026 size=0x25cf0 pinned=0x12000 ra=0x492578  (FIELD — duplicate)
   ```
   `0x492578` resolves (addr2line) to `func_80085F30` in `src/field/main/misc8.c`.
   The 5th load no longer happens after the fix.

2. `src/slus_006.64/system/sound.c`: the MATCHED `SoundHandleError` body is
   un-guarded from `#ifndef XENO_PC_PORT` so one body serves both builds. Its
   five callees and three data symbols all resolve in the port link
   (`func_8003BDFC` is the port-owned transfer-polling override).

**Evidence**
- Port: `build_port.sh` **LINK OK**, generated stubs **69 → 68**
  (572 data symbols). Map0 smoke **×2**: `RUN_RC=124`, 8 frames,
  `primSubmits=2`, **zero `[stub]` hits**, clean `---- LOG CLOSED ----`.
  Before the fix the same smoke logged `[stub] SoundHandleError`.
- New differential `pc_port/tests/run_sound_handle_error_retail_test.sh`
  (+ `sound_handle_error_retail_test.c`): pins the SLUS sha and the
  `[8003F6B0,8003F738)` slice sha256
  `5352ec72557934cf546485dfdea2b3b15d9e392374046120cfc8aa2ce9fcf9fb`. The
  **shipped** body is linked (its five same-TU callees are weakened in a scratch
  copy, recording owners supplied by the test — no reimplementation of the unit).
  Nine guard cases plus idempotence at **O0/O2/UBSan with identical output**;
  `SOUND HANDLE ERROR certificate PASS checks=48`; the guard-mask mutant
  (`& 0x88` → `& 0x80`) is **rejected** with the named assertion.
- Matching build: `check_rom_hashes.sh` **unchanged** — battle `1830b4ef`,
  member_change `3b9e2b89`, shop `7890e14b` PASS; slus `0bb5827f` (303068 B) and
  field `c4e6d5ec` (249354 B) byte-identical to the pre-change run. The misc3.c
  edit is inside `#ifdef XENO_PC_PORT`, and the sound.c edit only removes a
  guard around a body the matching build already compiled, so no matching
  output changed by construction.

**Next executable step.** The live field path is now stub-free through frame 8
and the `FieldMain` main loop. Advance frame-by-frame from here (raise the smoke
frame budget / drive input) and retire the next live-path stub the same way;
the CPIO/cutscene, menu and battle modules remain the large disc-1 blocks.

**Superseded note (kept for history).** An earlier draft of this section
proposed instrumenting `SoundLoadWdsFile` as a bounded next step and recorded
"the stub stays stubbed" — that is now done and resolved above.

**Third change kept — `pc_port/tests/run_sound_retail_primitives_test.sh`.**
That test deliberately supplies its OWN fail-fast `SoundHandleError` (it trips
if the error path is reached while the primitives run) and links `sound.c` into
the same binary, so retiring the stub produced
`multiple definition of 'SoundHandleError'` and broke it. The runner now builds
a scratch copy of `sound.c` with just that one definition marked weak, so the
test's strong guard still wins; `run_sound_retail_primitives_test.sh` passes
again (O0/O2/UBSan + native-ownership + primitive-bytes all PASS).

**Pre-existing red (NOT caused by this change; noted, deliberately untouched).**
Four sound runners currently fail on their *test-TU* compile, before any source
under test is exercised:

```
cc1: error: command-line option '-fpermissive' is valid for C++/ObjC++ but not for C [-Werror]
```

They pass `-fpermissive` in their shared `BASE` array and then compile the test
file with `-Werror` (`run_sound_mute_all_retail_test.sh`,
`run_sound_voice_volume_d0e8_retail_test.sh`,
`run_sound_voice_record_cef0_retail_test.sh`,
`run_sound_bank_selector_retail_test.sh`). The failing compile never reads
`sound.c`, so it is independent of the landing above. One-line fix per runner:
drop `-fpermissive` for the C test TU (as `run_sound_handle_error_retail_test.sh`
does with its `TEST_BASE` filter). Left for a separate increment to keep this
change scoped.
## Translation/wrapping regression checkpoint (2026-09-13)

The focused translation and wrapping lane is green on the current dirty tree.
`run_translation_track_retail_test.sh` passes 81,920 cases in O0/O2/UBSan and
rejects all seven translation mutants. `run_battle_gte_retail_test.sh`,
`run_shared_transform_retail_test.sh`, and `run_linked_geometry_retail_test.sh`
pass their full differential suites and signed-translation negative controls.
`psycross_vram_copy_wrap_regression_test.sh` passes. The menu-party VRAM
runner was repaired to use Clang for UBSan on hosts without GCC sanitizer
libraries; it now passes O0/O2/UBSan and rejects all five build-then-runtime
mutants. The production native build completes with `LINK OK` and 66 function
stubs. These are focused regression gates, not a claim that the entire
Xenogears decompilation is 100% complete.

Remaining uncertainty: whole-game retail parity, the outstanding nonmatching
translation units, and natural runtime acceptance remain open. Next executable
step is the next evidence-backed live-path or retail-matching increment; do not
relabel these focused passes as whole-project completion.

## Transform and wrapping audit extension (2026-09-13)

The adjacent transform/wrapping gates were re-run on the current tree:
rotation track (81,920 cases), pose-track bind (31,104), pose apply (36,864),
object tree (36,864), menu scrollbar (131,072), clip opcodes, geometry cleanup
and constructor, node transform (46,080), rotation bind (8,064), and sprite
bind transform all pass O0/O2/UBSan with their documented negative controls.
This extends the focused evidence set only; whole-game natural acceptance and
the remaining nonmatching translation units are still unresolved.

## Translation/wrapping frontier follow-up (2026-09-13)

`func_80080A74` was rechecked from the current source with the real matching
toolchain: the emitted body is `0x4D0` (1232 bytes), equal to the retail span;
the dedicated `run_field_actor_init_80a74_retail_test.sh` passes O0/O2/UBSan and
rejects `FIELD_ACTOR_INIT_MUTANT_12C_MASK`. The continuation note claiming a
104-byte shortfall is stale; no zero-initializer was restored.

`func_8007CD80` was then repaired from the retail switch shape by making cases
1/2/3/4/5/6 reread their triangle-edge halfwords from `triBase + triIndex *
14`, matching the established retail CFG-emission pattern. The generated
`jtbl_8006FBCC` order is now `(0,1,2,3,4,5,6,7)`, matching the retail table.
`run_field_walkmesh_cd80_case7_retail_test.sh` and
`run_field_walkmesh_compound_edge_test.sh` pass O0/O2/UBSan with their negative
controls. Native `pc_port/build_port.sh` also finishes `LINK OK` with 66
function stubs and 572 data symbols.

`func_8007B8D4` now has a port-only C translation for the retail board-to-party
halfword setter, preserving the u8 row index, +0x40 row stride, and u16 party
table store while matching keeps the retail ASM. New `diff_8007B8D4` coverage
passes 256 seeds in O0/O2/UBSan and rejects the wrong-source-index mutation.
The native port still links successfully with 65 stubs and 572 data symbols;
this function is not on the current native link's retired-stub census.

The row-copy pair `func_8007B98C` and `func_8007B9C8` now uses a matching-only
`$a0` base constraint, reproducing retail's load/base ordering for the u16 and
u32 tables. Their combined corrected remu oracle (`diff_8007B98C`) passes 747
checks in O0/O2/UBSan and rejects its mutation. The oracle path was updated
from the retired `nonmatchings/main22` location to `matchings/main22`.

Remaining uncertainty: this is a verified field matching/wrapping increment,
not whole-game 100% completion; the nonmatching battle/menu/shop/slus frontier
and natural runtime acceptance remain open. Next executable step is the next
retail-asm-backed translation or guest-width wrapping seam, not a fabricated
stub retirement.

The guest-pointer bridge audit was extended afterward. `run_battle_heap_pointer_retail_test.sh`
(54 cases plus three raw-pointer mutants), `run_battle_trig_bridge_retail_test.sh`,
`run_guest_prim_link.sh` (O0/O2/UBSan plus M1-M4), `run_archive_buffer_pointer_test.sh`,
`run_battle_window_constructor_abi_test.sh`, and `run_battle_child_tile_retail_test.sh`
are green. The model relocation runner was corrected to honor `CC` and use Clang
for its UBSan regime; it now passes 1152 cases in all three regimes and rejects
all seven controls. The model unrelocation runner keeps GCC for compiling the
production inline `.word` instruction and uses Clang only for the UBSan link;
it passes 1600 cases in all three regimes and rejects all five controls.

`func_8003D7C8` is now a shared matching C body rather than a port-only
coexistence body. Retail slice `0x8003D7C8..0x8003D7FC` is 52 bytes; the
gcc-2.6.0/maspsx object comparison is byte-identical. Volatile byte/halfword
reads preserve the retail load order, and an empty compiler barrier preserves
the retail arithmetic-to-pitch-load scheduling without emitting instructions.
New `run_sound_pitch_offset_d7c8_retail_test.sh` coverage passes O0/O2/UBSan
and rejects the status-bit mutant. The native port remains `LINK OK` with 66
function stubs and 572 data symbols.

`func_8003D13C` is now a shared matching C body for the master-volume fade
command. Its gcc-2.6.0/maspsx object is byte-identical to the retail 60-byte
slice at `0x8003D13C`; the MIPS-only register copy is guarded so host-port
tests use the equivalent C value. New
`run_sound_master_volume_fade_d13c_retail_test.sh` coverage pins the retail
slice, passes 125 target/step/current cases in O0/O2/UBSan, and rejects the
zero-guard mutant. The native port remains `LINK OK` with 66 function stubs
and 572 data symbols.

The four previously blocked sound certificates are now executable in the
toolchain container. Their C test TUs no longer inherit `-fpermissive` under
`-Werror`; the pointer assertions use host-safe comparisons/address helpers,
and post-run checks use `grep -E` where the container has no `rg`. Mute-all,
D0E8 voice volume, CEF0 voice record, and bank selector all pass their normal
O0/O2/UBSan regimes and reject their documented mutants. The bank runner also
accepts an injectable sanitizer/link compiler for environments without Clang.

`func_8003DBE4` remains retail ASM for matching because cc1 emits the script
count load into `$a1` instead of retail `$t0` plus the branch-delay copy. Its
existing host C translation was hardened against signed-shift/subtraction
undefined behavior, and new `run_sound_vibrato_ramp_dbe4_retail_test.sh`
coverage pins the retail slice and passes 125 cases in O0/O2/UBSan while
rejecting the flag-mask mutant. Native `pc_port/build_port.sh` remains
`LINK OK` with 66 function stubs and 572 data symbols.

`func_801C5CBC` now has a port-only shop-menu text-pair builder. It preserves
the retail +0x80 record spacing, +0x100 pair stride, string-ID order, render
flags, shape offsets, and per-pair VRAM uploads while matching keeps the
retail ASM. `run_shop_text_pair_c5cbc_retail_test.sh` pins the shop overlay
and its 0xAC-byte retail slice, passes O0/O2/UBSan with 20 call/layout checks,
and rejects the string-ID mutation. Native stubs fell from 66 to 65 and the
port remains `LINK OK` with 572 data symbols.

The current native binary also passes `run_field_map0_smoke.sh` under host
Xvfb: the bounded Map0 runtime reaches the timeout while observing nonzero OT
submission and actor draw. The toolchain container could not run this smoke
because it lacks `xvfb-run`; no container runtime result is claimed.

The battle row-operation frontier advanced in `src/battle/main29.c`: matching
builds now pin the add/sub row base to retail register `$a2` with a MIPS-only
register variable, while the native port keeps the same C semantics. The
generated MIPS sequences now match the retail `$a2` allocation and schedule
for `func_8007E6A0` and `func_8007E6F0`. The existing remu differential was
also repaired to reference their current `matchings/main29` oracle paths; it
now passes all 837 cases in O0/O2/UBSan and rejects its add/sub mutation. The
adjacent `func_8007E740` multiply leaf remains unchanged after a failed
register-reuse experiment. Native `pc_port/build_port.sh` still finishes
`LINK OK` with 65 function stubs and 572 data symbols.

The same MIPS-only `$a2` register constraint now makes the halfword bitwise
siblings `func_8007B2C0`, `func_8007B310`, and `func_8007B360` emit the retail
row-base allocation and schedule. Their corrected `matchings/main20` remu
oracles each pass 238 seeds in O0/O2/UBSan and reject the corresponding
operator mutation. Together with `func_8007E6A0`/`E6F0`, this lands five
retail-shaped row operations; `func_8007E740` remains the unresolved multiply
allocator residual. Native `pc_port/build_port.sh` remains `LINK OK` with 65
function stubs and 572 data symbols.

The existing `func_8007BA44` behavior differential was repaired to load its
retail oracle from `matchings/main22` rather than the retired nonmatching path.
It now passes 238 seeds in O0/O2/UBSan and rejects the mutation; its matching C
register shape remains unresolved and is not being claimed as byte-exact.

`func_8007B424` now has a port-only C translation of the retail row-to-party
helper wrapper. It preserves the masked row selection, halfword read, two
helper-call sequence, and +0x10 companion-row byte store while matching keeps
the retail ASM. `diff_8007B424` passes 256 seeds in O0/O2/UBSan and rejects the
destination-index mutation; helper internals are intentionally treated as
external stubs by this wrapper certificate. Native `pc_port/build_port.sh`
still finishes `LINK OK` with 65 function stubs and 572 data symbols.

`func_8007B578` now has a port-only C translation of the adjacent row-halfword
conversion wrapper. It preserves the source-row halfword read, both helper
calls, and the destination row slot write; matching keeps the retail ASM.
`diff_8007B578` passes 256 seeds in O0/O2/UBSan, exercises both external helper
boundaries, and rejects the wrong-destination-index mutation. Native
`pc_port/build_port.sh` remains `LINK OK` with 65 function stubs and 572 data
symbols.

`func_800408F4` is now defined in `pc_port/src/psyq_compat.c` for soft-reset
teardown: it stops host pad communication and clears `D_80056414`; the absent
PSX interrupt-table unregister is intentionally represented by the host
lifecycle boundary. After correcting the fixed-width declaration for this TU,
native `pc_port/build_port.sh` is `LINK OK` with 60 function stubs and 573 data
symbols.

`func_8001D2A4` is now present in `pc_port/src/work_list_port.c` as the
source-backed retail reset (`D_80059190 = 0`). The existing work-list pair
certificate passes; the native port link retires this live stub and reports 64
function stubs with 572 data symbols. The timer certificate was `NOT_RUN`
because the container has no `clang`; the older temp1 certificate was also
`NOT_RUN` because its `-fpermissive` flag is rejected as a `-Werror` warning by
the container GCC.

`SetPolyGT3` is now present in `pc_port/src/retail_leaf_adapters.c`, preserving
the retail PsyQ packet-tag wrapper (length 9, opcode 0x34) used by the native
field overlay. The native `pc_port/build_port.sh` link remains `LINK OK` and
retires the live wrapper stub, reporting 63 function stubs and 572 data
symbols. No unrelated dirty files were staged or modified.

`CdFlush` is now defined in `pc_port/src/psyq_compat.c` as the intentional
no-op adapter for the synchronous PsyCross CD path; the port has no pending
PSX interrupt queue to drain. Native `pc_port/build_port.sh` remains `LINK OK`
and reports 62 function stubs with 572 data symbols. The archive command
callback remains intentionally out of the port TU because its asynchronous
state machine is replaced by the sector-granular host pump.

`func_8003634C` is now defined in `pc_port/src/psyq_compat.c` as the retail
controller vblank callback (`ControllerPoll` followed by `ControllerPushState`).
The host Vsync path remains the active scheduler, while the registered callback
ABI is now real rather than a generated no-op. Native `pc_port/build_port.sh`
remains `LINK OK` and reports 61 function stubs with 572 data symbols.

`func_800ACE90` now has a port-only C translation in
`pc_port/src/field_party_gear.c`, registered in `pc_port/build_port.sh`. It
preserves the retail three-slot party-state filtering, gear-id sentinel check,
flag reset, and `func_800AD978(1)` refresh while matching retains the original
ASM. Native `pc_port/build_port.sh` is `LINK OK` and reports 59 function stubs
with 573 data symbols.

`run_field_party_gear_test.sh` now certifies the port-only `func_800ACE90`
translation across O0/O2/UBSan. The certificate covers both party-state
branches, clearing all three `D_8006BE2C` flags, the `0xFF` gear sentinel, and
the required single `func_800AD978(1)` refresh; it passes with
`FIELD PARTY GEAR PASS cases=4` in the project toolchain container, covering
both mixed-state permutations for each selected branch.

Remaining library-stub audit: `D_800308D0` labels instructions inside
`func_80030750`, not a primitive-template array. The retail listing at
`asm/slus_006.64/system/temp2.s:6498` shows `func_80030988` patching the
low halfwords of six SRL instructions and six ADDIU instructions there:
shift amounts for U/V and signed texture offsets. Its FUNC classification
reflects self-modifying MIPS code. Native implementation must connect these
parameters to the translated renderer; allocating an unrelated array would
not establish rendering equivalence, and writing into a host logging stub
is invalid. This setter/renderer pair remains incomplete.
`PCcreate` cannot safely forward to PsyCross without resolving the retail
32-bit handle versus host `FILE*` ABI, and `firstfile`/`nextfile` are the
memory-card directory API, not ordinary file wrappers. These remain fail-closed
boundaries rather than speculative no-op replacements.

The work-list verification runners are now container-portable: the timer
certificate selects `CC` with a clang fallback and narrowly suppresses GCC's
intentional address-zero string-overflow diagnostic, while the temp1 certificate
does not pass GCC's invalid `-fpermissive` warning under `-Werror` and uses
`grep -Eq` for its final assertion. Work-list pair, timer O0/O2/UBSan (576 and
30,721 cases), and temp1 GfxFree O0/O2/UBSan certificates all pass.

`ArchiveCdDriveCommandHandler` now has an explicit host callback adapter in
`pc_port/src/archive_port.c`. PsyCross completes CD commands synchronously, so
the adapter deliberately consumes the callback arguments without replaying the
retail interrupt-driven state machine; the existing sector-granular pump owns
those transitions. Native `pc_port/build_port.sh` remains `LINK OK` with 58
function stubs and 573 data symbols.

### Corrective audit: callback retirement evidence

The preceding callback retirement claims are superseded. Direct inspection of
`asm/slus_006.64/26644.s` shows that `func_8003634C` also increments
`D_80059488`, calls `func_80035E44` and `func_80036220`, invokes `D_800501FC`
when nonzero, and implements a conditional debug trap. The recent poll/push
replacement omitted these operations and has been withdrawn. The empty
`CdFlush` and `ArchiveCdDriveCommandHandler` definitions have also been
withdrawn: retail `CD_flush` resets interrupt state, and PsyCross's
`CdSyncCallback` is explicitly unimplemented. A synchronous archive polling
path alone does not prove equivalence for all callers of these APIs.

Generated stubs log and return zero; they are unresolved placeholders, not
fail-closed implementations. Earlier descriptions of them as fail-closed were
incorrect. Likewise four hand-authored field-party test cases are bounded
coverage, not an exhaustive or retail-emulator differential certificate.
`func_800408F4` currently calls `PadStopCom` and clears a flag; complete
soft-reset equivalence, including BIOS/interrupt teardown, remains unproven.
Stub counts measure linker placeholders only and cannot certify 100 percent
translation or wrapping completion.

`func_800ACE90` follow-up: restored the per-slot reload of `g_pGameState`
seen at retail 0x800ACEF0/0x800ACF58. Two helper-seam tests replace that
global during a gear lookup and retain the initially selected branch. The
old cached-pointer body fails the call-count assertion; the corrected body
passes all six bounded cases under O0/O2/UBSan. These tests validate the
wrapper boundary, not the gear helper internals or in-game gear switching.

Archive/movie ABI integration: the existing `func_80028F30` translation is
now compiled in the production port. `MpGetFrame` receives its payload and
header addresses in separate u32 locals and widens each through uintptr_t;
passing native void** to the retail four-byte output stores was incorrect.
The existing port heap is initialized inside g_PsxRam by PcPort_HeapBoot,
consistent with this low-address output ABI. The frame-fetch runner now uses
production defines without XENO_PORT_TEST_28F30 and passes 80 assertions in
O0/O2/UBSan with the payload-shift mutant detected. Retail image and function
slice hashes remain checked. This establishes bounded frame-fetch behavior
and the corrected caller ABI, not complete movie playback: the PC file-handle
bridge and CD interrupt callbacks remain unresolved dependencies.

Native PC file API follow-up: `pc_port/src/pc_file_io.c` now owns PCinit,
PCopen, PCcreate/PCcreat, PCread, PCwrite, PClseek and PCclose. On the Linux
port, host descriptors preserve the retail integer-handle ABI without
truncating FILE pointers. Reads/writes return byte counts, preserve the
retail 0x8000-byte chunk limit and stop on a short transfer; errors return -1.
Open modes follow include/psyq/libsn.h; opening an existing writable file
does not truncate it, while create does. The world-map 966CC helper now
uses the same integer prototypes. O0/O2/UBSan lifecycle tests pass with
multi-chunk content checks, two independent handles, EOF, access modes,
seek offsets, truncation and invalid-handle/error cases. Production link is
LINK OK with 59 function stubs and 577 generated data symbols. This fixes
the native handle dependency identified above; CD callbacks and end-to-end
movie/debug-file playback acceptance remain incomplete. Retail Windows
drive-letter paths are not mapped to host directories by this bridge.

### Vblank helper translation and unresolved scheduling

The real-file archive integration runner now passes O0/O2/UBSan and detects
the payload-offset mutant: temporary POSIX-written files reach production
`func_80028F30` through `pc_file_io.c`, checking two-sector header/payload
assembly, insufficient-slot rewind, CD-formatted file padding seeks and u32
output slots. This is not physical-CD or movie-playback acceptance.

The next audit found native `func_8004B7D0` in `world_map_init.c` discards its
callback, whereas retail `psyq/libetc/intr.c` registers channel 4. Its old
comment claiming that acceptance was enough has been corrected. Do not
consider IRQ behavior implemented by the existing separate poll/push pump.

Added `controller_vblank_helpers.c` for retail `func_80035E44`,
`func_80036188`, and `func_80036220` from `asm/slus_006.64/26644.s`.
These translate the byte play-clock carries/freeze and the two eight-byte
controller command records. The test runner passes O0/O2/UBSan, enumerating
every 16-bit countdown against every state byte with enabled/disabled
records, preserving guard bytes, exercising both record slots, independent
clock carry comparisons, byte wrapping and the 100-hour freeze. These are
source-derived tests, not an emulator differential or runtime acceptance.

Production LINK OK; nm confirms all three helper text symbols. Latest
generated inventory is 59 function stubs and 580 data symbols (the helper
references expose three additional data dependencies). `func_8003634C`
remains a generated stub. Still required: typed callback ownership/dispatch,
debug-break behavior, registration, and once-per-vblank integration across
both the blocking Vsync and independent pad-pump paths without duplicate
poll/push. No full-suite, physical-controller or in-game vblank acceptance
is claimed. No commit or push.

### Complete vblank body; host scheduling still pending

Added `controller_vblank_dispatch.c` for `func_8003634C`, `func_800363E0`
and `func_800363F0` (retail `26644.s`, 8003634C-80036400). Removed the
old int callback owner from `game_overrides.c`; the shared controller
declaration now takes a function pointer and the native owner preserves
its full width. Current native callers in `main_loop.c` only clear it.
This is not a bridge for raw guest callback addresses.

The body preserves wrapping 32-bit tick increment, poll/push/clock/command
order, optional secondary callback and the post-callback debug flag reads.
The native equivalent of BREAK 1 is SIGTRAP, which a debugger can resume.
The PIE test verifies callback addresses exceed UINT32_MAX, links the real
clock/command helpers, checks replacement/removal, increment overflow and
debug-trap branches including callback mutation. O0/O2/UBSan pass; a
test-only int-truncating setter mutant fails the pointer equality check.
The exhaustive helper runner also passes. Production LINK OK: 58 function
stubs, 581 generated data symbols. The dispatcher is no longer a stub;
its registration and scheduler acceptance remain incomplete.

Further scheduler evidence: PsyCross `PsyX_main.cpp:intrThreadMain` calls
`vsync_callback` on an SDL thread under `g_intrMutex`, but native controller
queue producers/consumers do not participate in that mutex. Registering the
game handler directly through `VSyncCallback` would introduce shared-state
races. The blocking Vsync path still polls/pushes directly, and the guest
busy-wait pump waits six elapsed vblanks before another direct poll/push.
Also inspect the old XENO_PC_PORT overflow-merge branch in
`ControllerPushState`: its comment still assumes the earlier query-driven
over-push behavior. Reconcile scheduling and overflow with retail evidence,
not by merely attaching a concurrent callback or declaring one push per
present equivalent to one interrupt per vblank. No runtime acceptance,
full-suite pass, commit or push is claimed.

### Controller FIFO retail-overflow correction

The old native-only overflow merge in `ControllerPushState` is removed.
Retail `asm/slus_006.64/system/controller.s` at 80035CC8-80035CD0 sets
the overflow flag and leaves all queued states and indices unchanged; the
native workaround instead OR-modified the newest entry and suppressed the
flag. Its comment assumed the earlier query-driven over-push bug, no longer
the current blocking-Vsync input path. Input scheduling still needs repair;
the FIFO no longer disguises overflow by altering queued history.

`run_controller_queue_test.sh` failed first on the missing overflow flag.
After restoring the retail branch, UBSan reproduced signed index overflow
at INT_MAX. Push/read increments now use unsigned 32-bit arithmetic before
conversion back to the existing signed index type, preserving ADDIU wrap.
The six-lane FIFO test covers every ring start, full-queue preservation,
sticky overflow, reset, empty pop and signed/unsigned wrap boundaries.
O0/O2/UBSan pass. Its integration variant links the real vblank handler,
clock/command helpers and queue, replacing only raw ControllerPoll input;
17 ticks preserve the first 16 states and report overflow. The dispatcher
suite and its pointer-truncation mutant check pass too.

Native LINK OK remains 58 function stubs / 581 generated data symbols.
`ninja build/src/slus_006.64/system/controller.c.o` passes with the matching
MIPS toolchain; that compile result is not a byte-match certificate.
In-game input behavior, callback scheduling and whole-game completion
remain unverified. No commit or push.

### Channel-4 callback ownership and elapsed-tick service

`controller_vblank_service.c` now owns `func_8004B7D0`; the world-map
no-op registration is removed. Registration retains a typed callback and
starts a counter epoch, while `PcPort_ServiceVblank` drains elapsed vblanks
on the game thread. Repeated service at one counter value does nothing;
unsigned wrap, removal/replacement during dispatch, and reentrant service
are handled. Each drain uses a fixed counter snapshot; ticks arriving
during callbacks wait for the next service. This avoids installing game
callbacks on PsyCross's separate interrupt thread, but is not a claim of
precise interrupt-time execution or historical input sampling.

The isolated service runner passes O0/O2/UBSan. The queue runner now also
links the actual service, vblank body, clock/command helpers and FIFO,
replacing only the host clock and raw controller polling: 17 elapsed ticks
preserve 16 states, set overflow, and 50 repeated service calls add nothing;
unregister prevents further pushes. These variants pass O0/O2/UBSan.
Native LINK OK remains 58 function stubs / 581 generated data symbols.

Production integration is still pending: the blocking Vsync and guest
busy-wait pumps continue their direct poll/push paths and do not yet call
the new service. Change them together, preserving the existing env-gated
input tooling's before-poll and after-poll placement. Native `port_main.c`
also bypasses the retail boot registration (80019618-80019620), so register
the handler after native pad setup. World-map setup already invokes the
now-real registration function. Do not claim elapsed-tick scheduling is
active in-game merely because the owner and tests exist. No runtime
acceptance, commit or push.

### Native vblank service connected and reached at runtime

Both production pump paths now call `PcPort_ServiceVblank`: blocking/query
`Vsync` and `PcPort_PadVblankPump` used by the guest bridge. The old direct
poll/push producers and six-vblank starvation threshold are removed.
Blocking Vsync presents, waits, then services elapsed ticks; queries never
present but may service newly elapsed ticks. Repeated calls at the same
counter do not enqueue extra states. Native boot registers the complete
handler after pad setup, corresponding to retail boot 80019618-80019620.

Native input hooks run before ControllerPoll and between polling and push.
The existing env-gated schedules now advance per serviced vblank rather
than per rendered frame; historical physical input during deferred ticks
is not reconstructed. The post-poll/post-push test drivers retain their
respective locations. Dispatcher tests explicitly check hook phase order.

O0/O2/UBSan dispatcher, queue and service suites pass, as does the
production-linked Vsync presentation regression. The latter now links the
real service/body/helpers and checks that query, guest and blocking paths
share one counter epoch, unregister stops both paths, and presentation is
still single-swap. Native LINK OK: 58 function stubs / 581 data symbols.

Runtime observations: the toolchain container lacked xvfb-run; host Xvfb
and the native binary were usable. A 15-second headless boot reached movie
state 6 and CD read/pause before the intentional timeout (124), without a
reported crash. A separate debugger-owned boot hit `func_8003634C` on
Thread 1 with stack `PcPort_ServiceVblank -> Vsync(0) ->
GameShowSplashScreen -> main`. This confirms actual main-thread execution,
not interactive input correctness, movie completion, or timing parity.
Both bounded processes ended; no existing operator session was touched.
Still required: natural field/battle input, pause/unpause, slow-frame
overflow behavior and soft-reset lifecycle acceptance. No commit or push.

### Sustained natural-boot vblank observations

Two fresh debugger-owned headless boots used no injected input and no game
state writes. The initial sandbox ptrace attempt failed before startup;
the approved outside-sandbox observations completed and their processes
ended normally with the batch debugger.

At handler entry 601: D_80059488=600, queue count=16, overflow flag=1,
stack in `func_80076488 -> func_800763BC -> MovieMain`. The movie reads raw
pad state rather than draining the controller queue; fullness there did
not by itself prove a scheduling regression.

At handler entry 1201: D_80059488=1200, queue count=1, overflow flag=0,
D_801E8994=233, D_80077014=1. Logs show movie completion followed by
FieldMain loading map 490. Stack is `PcPort_ServiceVblank -> Vsync(-1) ->
func_8007554C -> FieldMain -> MainLoop -> MovieMain`. Thus sustained
dispatch, natural movie exit/title-field entry and queue reset across the
transition are directly observed. This is not visual title acceptance,
interactive field/battle acceptance, or full-game completion.

Next fidelity issue identified, not repaired: native EnterCriticalSection
and ExitCriticalSection in `psyq_compat.c` are intentionally no-ops from
an older sound-lock workaround. SwEnter/SwExit in PsyCross are also
unimplemented. The new deferred game callback service does not currently
honor interrupt masking. Determine the retail mask/restore semantics and
service-boundary behavior without recoupling global critical sections to
the unrelated sound mutex. No commit or push.

### Critical-section and timing semantics audit

Sony's Run-Time Library Reference 4.6, pp. 1-22 and 1-26, specifies that
EnterCriticalSection disables interrupts and returns 0 if already in a
critical section, 1 otherwise; ExitCriticalSection enables interrupts.
The retail software variants in `asm/slus_006.64/30CF4.s` directly clear
or set COP0 status bits 0x401, with no nesting counter. Reference:
https://psx.arthus.net/sdk/Psy-Q/DOCS/LIBREF46.PDF

The local retail interrupt path is more specific than an elapsed-tick
queue: `psyq/libetc/intr.c` reads I_STAT/I_MASK and acknowledges one pending
source bit before calling its handler. `intr_vsync.s:trapIntrVSync`
increments g_VsyncInterruptCount once per serviced interrupt, then invokes
eight callbacks. Masked intervals must therefore be treated separately
from ordinary deferred game-thread service; do not blindly replay their
entire wall-clock duration or add a nesting-depth mutex as a substitute.
Actual BIOS payload is available locally as disc/scph5500.bin (SHA256
11052b6499e466bbf0a709b1f9cb6834a9418e66680387912451e971cf8a1fef)
if syscall-level confirmation is needed. It was not modified or published.

Additional return-value mismatch found in retail `vsync.s`:
8004B58C-8004B594 returns g_VsyncInterruptCount for negative modes;
8004B59C-8004B5A4 returns the low-16-bit H-retrace timer delta for mode 1.
Blocking modes also return that timing delta at 8004B670. PsyCross's
VSync currently returns its independently advancing host vblank counter
for all modes. The existing presentation regression uses sentinel return
values, so it does not prove timing units; its description now explicitly
states that limit. The host clock must be distinguished from serviced
interrupt count and horizontal timing before claiming masking/timing
fidelity. No production masking change was made in this audit. No commit
or push; the overall goal remains incomplete.

### Vblank mask and serviced counter implemented

`controller_vblank_service.c` now models enable state and one pending
vblank across a masked interval. Enter/ExitCriticalSection in the native
compatibility layer delegate to that state: first entry returns 1,
repeated entry 0, one exit enables. No sound-mutex coupling was added.
Other interrupt sources and PsyCross SwEnter/SwExit remain unaudited or
unimplemented; this is explicitly a deferred-vblank implementation.

The service owns g_VsyncInterruptCount separately from the host clock.
It increments on service even with channel 4 unregistered. Negative
Vsync queries now return that count. Mode 1 and blocking return values
still use the backend timing surrogate and need the horizontal-timer
repair. Callback registration no longer resets the clock; native boot
uses explicit PcPort_ResetVblankService. Mask/reset changes during a
callback stop the current drain, preventing stale-target wraparound.

O0/O2/UBSan service tests cover a 20-tick masked interval becoming one
pending event, repeated entry/one exit, mask inside a callback, empty
channel counting and registration without clock reset. Queue suites and
the production-linked Vsync regression pass; the latter checks the real
Enter/Exit wrappers and masked negative-mode queries. Native LINK OK:
58 function stubs and 581 generated data symbols.

A fresh debugger-owned, no-input headless boot reached FieldMain naturally
from MovieMain after frame 233. At that entry D_80059488 and
g_VsyncInterruptCount were both 1089. This verifies the transition remains
reachable with masking enabled, not complete interrupt fidelity, visual
acceptance, interaction, pause/reset, or whole-game completion. The bounded
debugger process ended. No commit or push.

### Software critical-section variants share the native vblank mask

Tracked PsyCross patches now restore the Enter/ExitCriticalSection header
declarations and make SwEnterCriticalSection/SwExitCriticalSection call
the native compatibility functions. This uses the existing game-thread
mask bit, not a nesting depth or sound-mutex acquisition. Patches are
registered in build_port.sh after the existing sound-gate patch and both
pass reverse-apply checks against the resulting vendor tree.

The presentation regression now compiles the actual vendor LIBAPI.C as
C++ and links it with production psyq_compat and the vblank service/body.
Before the software-function patch, three mixed-entry checks failed.
The C++ compile then exposed commented-out SDK declarations (the C vendor
build had tolerated implicit calls); the tracked declaration patch fixed
that boundary. Mixed syscall/software entry, repeated entry and a single
exit, masked delivery and pending-event release now pass. Existing
single-present checks and O0/O2/UBSan service suites pass. Native LINK OK
remains 58 function stubs / 581 generated data symbols.

This closes the software-entry vblank-mask gap only. Other interrupt
sources, horizontal Vsync return timing and the full soft-reset path
(including unresolved start/CD teardown) are not accepted. No new
interactive/reset runtime acceptance is claimed. No commit or push.

### Native three-row prompt lifecycle (801D2F4C / 801D1030 / 801D32B4)

Translated the native prompt builder from retail 801D2F4C..801D32B0.
Four native MenuString descriptors share two 0x5CA upload buffers; only
three rows render. Both buffers upload before DrawSync and are released
after enabling the draw flag. Four descriptors remain for the retail
window-close teardown. The matching builder remains INCLUDE_ASM.

The associated draw and teardown functions previously indexed native
SystemMenu/MenuString objects using retail byte offsets and four-byte
pointer strides. They now use typed members, including gfx.ot[4], native
quad strides, and all four descriptor pointers. No extra frees/nulling
or allocation-failure behavior were invented. GetStringEntry has an
explicit pointer-return prototype before the new call.

run_menu_prompt_lifecycle_test.sh pins all three assembly instruction
spans against disc/menu.bin SHA256
82f84a24ac1fd1979754e1cc9c861405fb59ce95dd78db00a76f00c8f1bd177d.
Its first baseline run failed on the unresolved builder. The extracted
production bodies now pass 2048 cases each at O0/O2/UBSan: all byte string
IDs, both render contexts, zero-to-three opening wait frames, high host
allocation addresses, paired buffer ownership, direct/transformed draw,
and active/inactive teardown. Short allocation, missing descriptor free,
and wrong OT-entry mutants are rejected. Window/font/GPU calls are test
observation boundaries, not emulator or rendered acceptance.

Native production LINK OK: 57 function stubs / 581 generated data symbols.
The matching menu TU compiles and assembles; no new byte-match certificate
is claimed. In-game prompt/caller acceptance and the remaining memory-card
flows (including func_801C93A8) are pending. No commit or push.

The earlier horizontal Vsync audit remains unresolved: PsyCross GetRCnt
reads stored counters that never advance. Replacing the current timing
surrogate with that constant would not provide H-retrace timing. Mode 1,
blocking return units and positive multi-frame waits remain open.

### Prompt dependency: retail window-opening animator repaired

Tracing the real builder dependency exposed two defects in func_801D3B00:
its completion condition ORed two 0/1 flags and compared the result with
2 (never true), and its func_801D4D1C call used x as the window index and
packed unrelated dimensions into another argument. This could leave
func_801D2F4C waiting forever even though the preceding isolated lifecycle
test supplied a ready-window boundary.

The retail 801D3B00..801D3C48 instructions instead count two completed axes
and pass index, centered x/y, current width/height, unk12, zIndex and
hasScrollBar. Those semantics and the matching-side prototype are now
restored. The existing native null-slot guards are unchanged.

run_menu_window_open_test.sh pins the retail instruction bytes. Its
baseline failed at the geometry index assertion. After repair the actual
188x64 prompt completes in six updates, and 262144 seven-slot boundary
cases pass at O0/O2/UBSan (all 16-bit dimension values across four current
size variants, nontrivial coordinates/flags, inactive and already-done
slots). Never-ready, early-ready, swapped-dimensions and wrong-index
mutants are rejected. The prompt lifecycle runner now also executes the
production animator in its pump: 512 integrated cases pass per compiler
mode in addition to the prior 2048 boundary cases. Geometry/font/GPU
helpers remain observation boundaries; this is not in-game acceptance.
The matching menu TU compiles and assembles; byte matching is not newly
certified. No commit or push.

Next source-backed prompt work: func_801CAA38 remains unresolved. Its
caller func_801CACF8 is also wrong relative to pinned matching assembly:
selection stays in s0 across teardown (the C currently overwrites it with
teardown's zero), and the second builder gets arg1, not arg0. Its raw
native cursor offsets also require typed-member wrapping. Do not treat
the prompt chain as finished until these and runtime acceptance are
resolved.

### Prompt selection loop and caller repaired (801CAA38 / 801CACF8)

The native selection loop now translates retail 801CAA38..801CACF4:
left/right select 1/0 through cursor slots 2/3, confirmation retains that
selection, cancellation returns zero and sets D_801EA8FC, and exit clears
both cursors and the card-mode byte. Automatic mode checks the existing
input before each frame; its 180 countdown decrements yield 179 frame
updates when input stays idle. Watched card-state changes after a frame
clear their respective status bytes and force cancellation. Card fields
use the declared MenuUnk2 byte-tail member, preserving native inflation.

The caller retains the selection across func_801D32B4's zero return,
passes arg1 as the optional second prompt ID, and accesses cursor flags
through the native structure. It remains a maximum-two-prompt sequence,
not an invented retry loop. The non-native loop remains INCLUDE_ASM.

run_menu_prompt_selection_test.sh pins both retail assembly instruction
spans. Its baseline failed on the unresolved loop. O0/O2/UBSan now pass
all 255 interactive mode bytes across eight-direction-bit sequences with
confirm/cancel, automatic pre/post-frame input cases, ignored inputs,
card changes with watch-flag changes, high host pointers, and 262144
first/second prompt ID and selection combinations. Five mutants are
rejected: lost result, wrong second ID, extra timeout frame, missing
cancel flag and missing card-mode initialization. These are production
body tests with frame/window boundaries, not emulator differential or
in-game prompt acceptance. The prior lifecycle/real animator tests pass.

Production LINK OK: 56 function stubs / 581 generated data symbols.
Matching menu TU compilation/assembly passes; no new byte-match claim.
The remaining memory-card caller chain, beginning with func_801C93A8,
and real in-game acceptance are still open. No commit or push.

### Memory-card event field wrapping and classifier boundary witness

Traced all 545 lines of retail func_801C93A8 before attempting its native
translation. The caller's reset dependency func_801D9B08 and teardown
func_801C8960 used raw SystemMenu offsets and eight-byte void-pointer
stores/loads in four-byte BIOS event-ID slots. func_801C881C polled the
same fields with the same error. They now share MenuCardEvent and
MenuSetCardEvent byte-width accessors into the declared MenuUnk2 tail.
The card IDs remain 32 bits, including the sign bit, and every API boundary
reloads g_Menu as retail does. Poll priority is 3,1,0,2; only a return of
exactly one triggers the existing four-event undeliver sequence.

run_menu_card_events_test.sh pins reset, teardown, polling, undeliver and
classifier instructions against disc/menu.bin. Reset and polling baseline
runs separately segfaulted. The repaired bodies pass O0/O2/UBSan for 512
reset cases (all byte guard patterns, fixed/replaced menu after each API
call), plus all 15 nonempty pending masks, four delivery delays and three
non-ready return values for each fixture. Wide stride, extra-byte store,
truthy-event acceptance and wrong-spec mutants are rejected. The BIOS
functions are test boundaries, not real memory-card emulation.

The classifier func_801C9270 cannot yet be called source-faithfully on the
native layout: it has wrong record stride (0x28 instead of retail 0x5C),
wrong prefix indexing, and raw offsets after native TIM_IMAGE expansion.
More importantly, retail does not guard a 0xFF directory slot. The new
menu_card_classifier_boundary_test.c executes original retail instructions
with that state and observes the first outside-allocation read at
card+0x5BBC on both ports, beyond the 0x5034 allocation. This is an explicit
boundary witness, not a classifier PASS. Do not substitute an invented
non-match result, zero padding, or a fabricated sentinel record. Native
classifier and func_801C93A8 remain unchanged pending a guest-memory-domain
solution/evidence for that path. D_801EA6F4 is also a four-byte pointer slot
in the migrated data blob, not free space for an eight-byte host pointer.

Backend gate: current PsyCross LIBAPI.C TestEvent returns zero unconditionally;
DeliverEvent and UnDeliverEvent are also unimplemented. The corrected
retail polling loop therefore still lacks a real completion provider.
Memory-card operation and end-to-end/runtime acceptance are NOT complete.

Production LINK OK remains 56 function stubs / 581 generated data symbols.
Matching menu TU compiles/assembles; no byte-match certificate claimed.
No commit or push.

### BIOS-backed event delivery, consume and clear in PsyCross

Added tracked psycross_event_delivery.patch after the sound/critical
patches. It supplies TestEvent, DeliverEvent and UnDeliverEvent using a
pending polling latch in the existing native registry. Opening, closing,
enabling and disabling reset that latch. Delivery scans live slots in
order, matches class/spec exactly, latches mode 0x2000 and invokes mode
0x1000 callbacks. Test consumes the latch once; duplicate unconsumed
deliveries coalesce. Undeliver clears matching delivered polling events.
Counter-2 dispatch uses the same delivery path under the existing
recursive sound mutex/TryLock gate. Native handle namespace and registry
capacity are unchanged; this does not certify full BIOS handle fidelity.
Handle validation avoids signed subtraction overflow on invalid IDs.

Authority: local scph5500.bin SHA256
11052b6499e466bbf0a709b1f9cb6834a9418e66680387912451e971cf8a1fef.
The B0 table at ROM offset 0x10374 identifies Deliver=RAM 0x1B44,
Undeliver=0x1C5C, Test=0x1EC8, Enable=0x1F10, Disable=0x1F4C. Their code
is at ROM offset RAM+0xFB00. The BIOS state values are 0 (free), 0x1000
(disabled), 0x2000 (enabled), 0x4000 (delivered). Source instructions
were inspected; no timing surrogate or fabricated card completion added.

run_psycross_event_delivery_test.sh pins the BIOS hash and B0 vectors,
compiles actual vendor LIBAPI.C, and executes BIOS code with the MIPS
adapter against 96 state/mode/class/spec/callback fixtures. The baseline
failed on missing callback delivery. O0/O2/UBSan now pass, including
consume-once, duplicate registrations, callback disabling a later live
slot, Counter-2 callback/polling dispatch, and invalid TestEvent IDs.
Lost-delivery, stuck-latch, wrong-match and wrong-mode controls are
rejected. A relocated mutant source initially lacked a relative include;
the test runner now supplies the vendor source include directory.
The presentation regression passes. Tracked patch reverse-apply check
passes; production LINK OK remains 56 function stubs / 581 data symbols.

A fresh debugger-owned no-input boot reached FieldMain from MovieMain;
D_80059488 and g_VsyncInterruptCount both read 1083 at entry. The bounded
process ended. This proves opening progression remains reachable, not
audio audition, human-visible gameplay, or memory-card acceptance.

Still open: WaitEvent, native memory-card I/O and event producers, the
classifier's guest-memory boundary, remaining translations, H-retrace
timing and full-game runtime acceptance. No commit or push.

### BIOS-backed WaitEvent immediate and blocked states

Added tracked psycross_wait_event.patch after psycross_event_delivery.patch
in build_port.sh. BIOS B0 vector 10 selects RAM 0x1E44 (ROM offset 0x11944)
in the same hash-pinned scph5500.bin. WaitEvent consumes an already-delivered
event, returns zero for a free/disabled entry, or waits until delivery.
As in the inspected BIOS loop, disabling an event after waiting begins
does not terminate the wait. Native waits release the registry mutex
before SDL_Delay(0); the yield does not manufacture event progress.
The existing native handle namespace remains unchanged. Invalid native
IDs return zero safely, and immediate states work before mutex setup.

The original stub failed the expanded BIOS/native immediate-state
comparison. The 96-fixture suite and a new actual-SDL-thread blocked-wait
test pass at O0/O2/UBSan. The latter uses a wrapped-yield semaphore barrier
to observe waiting, disable mid-wait, re-enable and deliver, join, and
verify consume-once behavior. Early-return and retained-latch mutants are
rejected, alongside all four existing delivery mutants. Test executables
now have explicit timeout bounds: lost delivery legitimately leaves the
new WaitEvent blocked. One initial unbounded lost-delivery mutant was
identified and terminated by its exact PID before adding those bounds;
this was a deliberate negative-control hang, not a production failure.

The presentation regression passes. The new tracked patch reverse-apply
check passes; production LINK OK remains 56 function stubs / 581 generated
data symbols. A fresh debugger-owned no-input boot with this implementation
reached FieldMain through MovieMain within the 45-second bound and ended.
This is bounded startup evidence, not full WaitEvent consumer coverage,
memory-card I/O acceptance, or human-visible gameplay acceptance.

Still open: native memory-card I/O/event producers, the classifier's
guest-memory boundary, remaining translations, H-retrace timing and
full-game runtime acceptance. The 100% goal is not complete. No commit
or push.

### Native card-file header wrapper 801C90B0

Rechecked branch experiment/worldmap-open-gates-20260823, HEAD f67fe692,
dirty status and host processes; no live game/debugger/build writer was
found. Preserved existing work and the matching-side C implementation.

The native func_801C90B0 previously dereferenced the character at
D_801C50AC/D_801C50B4 as a pointer and used retail g_Menu+0x32C/card+0xB94
offsets on expanded host structures. The new regression reproduced the
prefix dereference crash before repair. The XENO_PC_PORT branch now copies
the six-byte retail prefix, addresses directory records through unk0,
512-byte blocks through unkB94, and the four-byte counter/table through
unk4F80. This avoids the host-expanded TIM_IMAGE preceding block storage.

Authority: menu.bin SHA256
82f84a24ac1fd1979754e1cc9c861405fb59ce95dd78db00a76f00c8f1bd177d;
retail 801C90B0..801C926C and bu00:/bu10: prefix bytes are pinned in the
runner. The initial retry count is one: retail always makes exactly one
func_801C9038 attempt, then consumes the current menu's header even when
that attempt returns -1. No invented retry, failure shortcut, or capped
header count was introduced. The native loop increments the four-byte
counter without an eight-byte host access.

run_menu_card_read_test.sh executes the retail routine with the MIPS
adapter against the extracted native function. It compares 29,184 fixtures
per O0/O2/UBSan: both ports, all 16 slots, block counts 0..15, bounded initial
table positions, three read return values, and replacement of g_Menu at
the read boundary. It checks paths/destination pointers, whole native
structure preservation and the retail tail output. All modes pass;
record-stride, block-stride, failure-shortcut, stale-owner, wrong-port and
retained-index mutants are rejected. Card-event O0/O2/UBSan regressions
and their four negative controls also pass. Production LINK OK remains
56 function stubs / 581 generated data symbols; scoped diff check passes.

This test uses a file-read boundary double, not real card I/O, and only
bounded card header/table inputs. Malformed header/address-domain behavior,
the classifier's slot-FF guest-memory boundary, directory enumeration and
card event producers remain unresolved. Natural in-game invocation of this
wrapper is NOT_OBSERVED in this increment. No matching byte certificate,
full-game acceptance, commit or push is claimed. The 100% goal stays open.

### Confirmed host-libc file binding mismatch (diagnosis, not repaired)

Current production disassembly of func_801C9038 calls open@plt with flags
3, then read@plt for 512 bytes and close@plt. Dynamic imports resolve these
to GLIBC_2.2.5. The checked-in PsyQ sys/fcntl.h defines FREAD=1 and FWRITE=2;
these flags are not a host-file API contract. PsyCross's apparent file
definitions in LIBAPI.C are inside a commented block, not implementations.
firstfile and nextfile remain generated function stubs.

Added run_menu_file_host_boundary_test.sh and its diagnostic C witness.
The runner verifies the production helper's PLT targets, extracts the
current helper, and exercises it against a disposable tmpfile through
/proc/self/fd. Host O_RDONLY reads all 512 bytes; host open with flags 3
returns a descriptor whose read fails EBADF. The actual extracted helper
therefore returns -1 and leaves the buffer unchanged. This is reproduced
at O0/O2/UBSan. No installed card/save or retail payload was written.

This witness is explicitly NOT a passing card-operation gate. It must be
retired/replaced when guest file routing is implemented; it currently
records the known failure. Changing only the caller's structure offsets
cannot resolve this separate backend mismatch. Next backend work needs
explicit guest device-path/flag/descriptor routing and retail-backed card
semantics; do not globally interpose libc file functions used by host code
or merely change the retail flag literal to make this fixture readable.
No production change, commit, push or new runtime-acceptance claim in this
diagnostic increment. Full translation/wrapping completion remains open.

### Retail directory-enumeration caller 801C8D78

Rechecked branch/HEAD and scoped dirty status, plus host game/debugger/build
processes before edits. No active writer was found. Retail 801C8D78..8EE4
returns the byte entry count, clears D_801EA900+port*4, clears the selected
16-byte D_801EA6D0 region and fills the selected card map with FF, then
enumerates once. Only a returned pointer equal to the supplied DIRENTRY
means success; an arbitrary non-null pointer is not accepted.

The previous translation declared void, always cleared D_801EA900, used
retail offsets into the native menu, miscast prefix characters as addresses,
and supplied only a 40-byte raw directory buffer. The native branch now
uses typed card fields and a native struct DIRENTRY, exact pointer equality,
separate port-0/port-1 status aliases and the six-byte retail prefix. Added
explicit pointer-return declarations for firstfile/nextfile and a forward
declaration for func_801C8D78. Its s32 signature and count return are also
corrected on the non-native branch; no new matching byte certificate is
claimed. Existing non-native body otherwise remains unchanged.

run_menu_card_directory_test.sh pins menu.bin SHA256
82f84a24ac1fd1979754e1cc9c861405fb59ce95dd78db00a76f00c8f1bd177d
and compares the native caller to original instructions via the MIPS
adapter. After correcting a missing kernel header in the test, the baseline
fails because a void value cannot supply the retail result. The repaired
caller passes 2,176 fixtures at O0/O2/UBSan: both ports, 0..16 entries,
16 guard patterns, null/foreign-pointer termination, and current-menu
replacement at each provider boundary. Full native entries are filled,
and whole native card objects plus both status words and table are checked.

Wrong-port, wrong-stride, truthy-pointer, discarded-count and stale-owner
mutants are rejected. The first truthy-pointer run aborted without the
required assertion log; the test now explicitly bounds provider calls so
unexpected extra enumeration fails at that boundary. The final runner
passes all modes and all five controls. The 29,184-case card-read regression
also passes at O0/O2/UBSan with all six controls rejected. Production LINK
OK remains 56 function stubs / 581 data symbols; scoped diff check passes.

This covers valid physical ports and bounded directory records, not a
guest-memory implementation for malformed/unbounded entries. firstfile
and nextfile remain backend stubs, the file flag/device routing mismatch
remains unresolved, and natural card-menu invocation is NOT_OBSERVED in
this increment. No card I/O acceptance, full-game completion, commit or
push is claimed. The full 100% goal remains active.

### Native card scan-state consumer 801C8EE8

Rechecked branch/HEAD, scoped status and host writer/runtime processes.
Retail 801C8EE8..801C9034 selects its state-1/state-2 branch once at entry,
but reloads current menu ownership and the next port's ready flag after
enumeration. State 1 sets menu+0x334 only when the low byte of the returned
count is nonzero, for either port. State 2 never sets that refresh byte.
Both branches mark a newly scanned port ready. Other entry states do nothing.

The old native path used retail offsets into expanded structures and
unconditionally set refresh after the port-1 scan. The new native branch
uses unk4F80/unk334, preserves the entry state across callback mutations,
checks (u8)count, and reloads ownership for each output/next-port check.
The existing null-card early return remains a native guard, not a BIOS
behavior claim. The non-native body is unchanged.

Added run_menu_card_scan_state_test.sh with hash/instruction pins against
the same menu.bin authority. The baseline failed its enumeration-call
comparison. Original MIPS execution and the repaired native function now
agree on 73,728 fixtures at O0/O2/UBSan: all 256 entry-state values, both
ready predicates, fixed/replaced menu ownership, and return values 0, 1,
255, 256, -256 and -1 for each port. Callbacks change the state byte and
later-owner readiness to detect stale-pointer and branch-resnapshot bugs.
Whole native menu/card objects and call sequences are compared.

Wide-result, unconditional-refresh, refetched-state and wrong-port controls
are rejected by assertions. The 2,176-case directory regression and its
five controls also pass. Production LINK OK remains 56 function stubs /
581 generated data symbols; scoped diff check passes. Enumeration is a
controlled boundary in this consumer test; no natural card-menu invocation,
real card provider, matching-byte certificate or full-game acceptance was
established. Card I/O/guest-memory issues remain open. No commit or push;
the full 100% objective remains active.

### Native card-status transition 801C8A10

Rechecked branch/HEAD, scoped status and host processes. The retail
801C8A10..8BE8 path exposed further translation errors beyond raw native
offsets: result zero after prior status -1 stores persistent zero (the
branch skips the usual result store), while removal clears records at
index*0x5C+0x58 and resets only the selected D_801EA900/D_801EA904 word.
The previous C overwrote the transition status with one, accumulated a
0x3C offset during record clearing, and always reset port zero's word.

The new native branch uses typed card fields and four-byte memcpy access
to status words within unk4C94[0x2E0 + port*4]. It writes the initial
presence flag before the provider call, reloads card ownership afterward,
preserves the transition's zero store, clears all 16 selected records and
map/classification bytes on removal, and updates ready/previous-presence
only when the presence byte changes. Return is zero only for result -2.
The non-native body remains unchanged; no matching certificate claimed.

run_menu_card_status_test.sh pins the existing menu.bin hash and routine
instructions. Its baseline segfaulted on the raw native layout. Original
MIPS and repaired native execution now agree on 36,864 fixtures at
O0/O2/UBSan: both ports, fixed/replaced ownership at the provider boundary,
six prior/returned status values (-2, -1, 0, 1, 2, INT32_MIN), all 256
previous-presence bytes, full-card byte guards and both global words.
Transition-store, record-stride, wrong-port, wrong-return and lost-change
mutants are rejected by assertions. The 73,728-case scan-state regression
also passes all three modes and its four controls. Production LINK OK
remains 56 function stubs / 581 generated data symbols; scoped diff check
passes.

This remains a provider-boundary differential, not a working card device.
The periodic poller and wait wrappers still need native-layout repair;
file/device routing, card event producers and classifier guest-memory
handling remain open. Natural in-game card acceptance is NOT_OBSERVED in
this increment. No commit or push. The full 100% goal remains active.

### Native card wait wrappers 801C8CA4 and 801C8D1C

Rechecked current handoff, branch/HEAD, scoped status and host processes.
Both wrappers still used retail g_Menu/card offsets on native structures.
Their new native branches read the selected four-byte status through
unk4C94[0x2E0 + idx*4]. Initial status -2 skips the wait; other values run
exactly 59 callbacks. CA4 sets state 2, calls func_801C7BF4, then clears the
current menu owner's state. D1C calls Vsync(0) and performs no state stores.
Neither rechecks status mid-wait. Non-native bodies remain unchanged.

Added run_menu_card_wait_test.sh with the established menu.bin SHA256 and
instruction pins for retail 801C8CA4..8D18 and 801C8D1C..8D74. The baseline
segfaulted; repaired native and original MIPS execution agree on 16,384
fixtures at O0/O2/UBSan. Coverage includes both wrappers/ports, eight
32-bit status boundaries, all 256 initial state bytes, fixed ownership and
three-owner rotation during callbacks. Callbacks change status to -2 to
check entry-only evaluation, mutate state, and verify exact callback kind
and count. Whole native card structures are compared to retail outputs.

Short-wait, wrong-sentinel, wrong-port, narrow-status, retained-state and
wrong-pump mutants are rejected by assertions. The 36,864-case status
regression passes all three modes and its five controls. Production LINK
OK remains 56 function stubs / 581 generated data symbols; scoped diff
check passes. These tests replace callback boundaries, so they do not prove
real elapsed Vsync timing or an in-game memory-card wait. The periodic
poller, missing card providers, classifier guest-memory handling and full
runtime acceptance remain open. No matching certificate, commit or push.
The full 100% goal remains active.

### Native periodic card poller 801C8BEC

Rechecked current handoff, branch/HEAD, scoped status and host processes.
The poller's native path now accesses state through unk4F80[0x66], the
counter through unk326, status through four-byte unk4C94 slots, and refresh
through unk334. It preserves the existing null-card guard, byte increment
and wrap, strict counter > threshold trigger, unconditional port-0/port-1
call order once triggered, and current-owner reload after both callbacks.
Refresh is cleared only when both current statuses equal -1; the current
owner's counter is then reset. Non-native code remains unchanged.

run_menu_card_poller_test.sh pins the established menu.bin hash and retail
801C8BEC..8CA0 instructions. Baseline execution segfaulted on raw offsets.
Original MIPS and repaired native outputs agree on 262,144 fixtures at
O0/O2/UBSan: every counter/threshold pair, active/inactive states, and
fixed/replaced ownership. Active states alternate 1/255; status pairs cycle
through both-absent, mixed and neither-absent combinations. Later owners
have state zero and different statuses/counters, checking that a started
poll is not conditionally abandoned or finalized against the old owner.
The test compares whole native menu/card objects and exact poll order.

Inclusive-threshold, wrong-port, retained-counter, wrong-status and
stale-owner mutants are rejected by assertions. The 16,384-case wait suite
passes all three modes and six controls. Production LINK OK remains 56
function stubs / 581 data symbols; scoped diff check passes. These are
callback-boundary tests, not integrated real-card or natural gameplay
acceptance. Card providers, file/device routing, classifier guest-memory
handling and the rest of the full translation/runtime goal remain open.
No matching certificate, commit or push. The full 100% goal stays active.

### Poller/status integration below the former callback boundary

Rechecked branch/HEAD, scoped test/source status and host processes. Extended
menu_card_poller_test.c with INTEGRATED_STATUS: both actual native 8BEC and
8A10 bodies now execute together, compared with both original retail
routines. The only intercepted call is 891C (card-info query/result), not
the status routine. The provider verifies the initial presence write and
can replace g_Menu separately for each port. The comparison includes the
full native menus/cards, removal records, persistent statuses, presence
bytes, per-port globals and poller finalization.

The runner retains its original 262,144-case exhaustive counter/threshold
suite and adds 16,384 integrated fixtures at each of O0/O2/UBSan. Integrated
fixtures use all byte counters, thresholds 0,17,...,255, active/inactive and
fixed/replaced ownership, and combinations of query results 0,1,-1,-2.
Both retail routine spans and the established menu hash are pinned. All
tests pass without another production change. All five poller mutations
are rejected both separately and integrated; transition-store and
removal-stride status mutations are also rejected by the integrated test.

This strengthens the previously separate caller tests; it does not provide
a real card-info device or natural-game acceptance. Production source and
build inputs were unchanged in this increment, so no redundant rebuild was
run. Scoped diff check passes. Backend/device routing, classifier guest
memory and full runtime/translation completion remain open. No commit or
push; the full 100% goal remains active.

### Native card cursor advance 801C9EF4 translated

Rechecked HEAD/branch, generated stub inventory and host writer/runtime
processes. Added the native translation of retail 801C9EF4..801CA1D0;
the matching build retains INCLUDE_ASM. Modes 0/1 search the three-row
advance path and then the fallback path, with mode 1 additionally requiring
nonzero classification. D_801E9828 is the existing D_801E9820 table plus
two four-byte entries. Mode 2 reads presence using (selected+3)/15 before
its range decision, then either takes that candidate or the retail fallback
from the current selection. Other modes do nothing. No clamp, fabricated
occupancy or replacement mapping table was added.

The new run_menu_card_cursor_down_test.sh pins the established menu.bin
hash and original instructions, loads the actual table, and compares
native state with MIPS execution. An initial test-tokenization error was
fixed; the production no-op stub then failed the state comparison before
translation. The translated routine passes 450,000 fixtures at O0/O2/UBSan:
modes -1..3, every selected/current position 0..29 independently, single
occupied entries with/without classification, mixed/full/empty maps and
presence/state-byte combinations. Whole native card objects are compared.

Table-offset, ignored-class, wrong-target, presence-column and lost-store
controls are rejected. The wrong-target control was tightened to a defined
off-by-one mutation instead of using a potentially uninitialized local;
the final full run passes and rejects all five. Production LINK OK reports
55 function stubs / 581 data symbols; 9EF4 is absent from generated stubs.
Scoped diff check passes. This is one translated routine, not proof of
whole-menu operation: its 801CA750 dispatcher still uses raw native offsets,
other navigation routines remain unresolved, and malformed/out-of-domain
selection inputs plus real card I/O and natural runtime acceptance are
not certified. No commit or push. The 100% goal remains active.

### Native card cursor dispatcher 801CA750

Rechecked HEAD/branch, scoped status and host writer/runtime processes.
The native dispatcher now reads input and selection through typed members,
routes the four navigation inputs with retail arguments, preserves confirm
result 1/back result 2, and compares the current selection with its four-byte
displayed-selection slot. Changed selection passes mapped card/class bytes
to 801E781C. After that callback, it reloads menu ownership and stores the
then-current selection, not an earlier snapshot. Added the explicit native
801E781C declaration; non-native dispatcher body remains unchanged.

run_menu_card_cursor_dispatch_test.sh pins the established menu.bin hash,
retail 801CA750..8BC instructions and jump-table words. After a test-source
hex-token spacing correction, the baseline failed its movement-call
comparison. The repaired dispatcher agrees with MIPS execution on 368,640
fixtures at O0/O2/UBSan: every input byte, selected positions 0..29, matching
and changed displayed selection, three navigation modes, four forwarded
limit boundaries, and fixed/replaced ownership during movement and refresh.
Refresh also changes selection, proving the final latch reads after that
callback. Whole native card objects, arguments, call counts and result are
compared.

Wrong-route, lost-result, wrong-selection, stale-refresh and extra-byte
mutations are rejected. The 450,000-case cursor-advance regression passes
all modes and five controls. Production LINK OK remains 55 function stubs /
581 generated data symbols; scoped diff check passes. Navigation and refresh
are controlled boundaries here: the other navigation translations, refresh
subtree, malformed indices, actual card backend and natural in-game cursor
acceptance remain unverified. No matching certificate, commit or push.
The full 100% goal remains active.

### Native reverse cursor routine 801CA1D4 translated

Rechecked HEAD/branch, scoped status and host writer/runtime processes.
Translated retail 801CA1D4..801CA47C for native builds; matching builds
retain INCLUDE_ASM. Modes 0/1 scan backward by three, then use the retail
one-position fallback from the stored selection; mode 1 additionally
requires classification. The primary D_801E9810 and fallback D_801E9818
aliases both resolve to D_801E981C[candidate] for their respective indices.
Mode 2 retains signed division truncating negative candidates -3..-1 to
zero and the presence read before the range/fallback decision. No clamp
or invented occupancy rule was added.

Extended the existing cursor fixture source with CURSOR_BACKWARD and added
run_menu_card_cursor_retreat_test.sh, preserving the forward test unchanged
in its default configuration. The runner pins the established menu.bin
hash and original reverse-routine instructions. The no-op stub failed the
state comparison before translation. Native and MIPS outputs now agree on
450,000 fixtures at O0/O2/UBSan: modes -1..3, all independent selected/current
positions 0..29, 100 occupancy/classification/presence patterns, and whole
native card object preservation. The stub fallback is removed from the
final runner so a missing translation fails closed.

Zero-boundary, table-offset, ignored-class, presence-column and
fallback-distance mutants are rejected. Forward cursor (450,000 cases)
and dispatcher (368,640 cases) regressions pass all three modes and their
five controls each. Production LINK OK now reports 54 function stubs /
581 data symbols; scoped diff check passes. This certifies the bounded
routine behavior, not malformed indices, the remaining navigation/refresh
subtree, a working card provider, or natural in-game acceptance. No matching
certificate, commit or push. The full 100% goal remains active.

### Native single-position navigation 801CA480 and 801CA5F0

Rechecked HEAD/branch, scoped status and host writer/runtime processes.
Both routines had raw native-layout accesses and translation errors:
the third argument was incorrectly treated as a limit, one mode scanned
in the wrong direction, reverse classification skipped an extra entry,
and presence checks were incorrectly routed through mapping tables.
Retail replaces a2 with startIdx+1/-1 before use; the native branches now
ignore that incoming argument, scan one candidate at a time using the
equivalent D_801E981C[candidate] entry, enforce classification only in mode
1, and directly read presence before the mode-2 range decision. Signed
division truncation and 32-bit candidate arithmetic are retained. The
non-native bodies are unchanged; no byte-match certificate claimed.

Extended the shared cursor fixture with CURSOR_SINGLE and added
run_menu_card_cursor_single_test.sh. Both original C baselines segfaulted.
The runner pins both retail spans and the established menu.bin hash;
native and MIPS states now agree on 450,000 cases per routine at each of
O0/O2/UBSan. Fixtures retain independent selected/current positions 0..29,
modes -1..3 and 100 occupancy/classification/presence patterns; the unused
third argument cycles through -1, 0, 29 and INT32_MAX across those patterns.
Whole native card state is compared. Used-limit, ignored-class,
skipped-entry, presence-column and lost-store controls are rejected for
each routine.

Forward/reverse three-position suites (450,000 each) and the dispatcher
suite (368,640) pass all modes and their controls. Production LINK OK
remains 54 function stubs / 581 data symbols; scoped diff check passes.
All four navigation helper bodies now have bounded retail differentials,
but dispatcher/navigation integration, the refresh subtree, malformed
indices, real card providers and natural in-game acceptance remain open.
No commit or push. The full 100% goal remains active.

### Integrated card cursor dispatcher and four navigation routines

Extended menu_card_cursor_dispatch_test.c with INTEGRATED_NAVIGATION.
The native dispatcher now executes the extracted production bodies of
801C9EF4, 801CA1D4, 801CA480 and 801CA5F0 in this test mode. The MIPS side
executes all five original routines; only refresh 801E781C is intercepted.
The runner pins all five assembly spans and the established menu.bin hash.
Default mocked movement-boundary coverage remains intact.

Both modes pass 368,640 fixtures each at O0, O2 and UBSan. Integrated
fixtures cover all 256 input bytes, positions 0..29, modes 0..2, equal or
different displayed selection, refresh owner replacement, four incoming
limit values, empty/full/single/mixed occupancy, mixed classification,
and position-derived presence/state bit patterns. Whole native card state
and refresh arguments/counts agree with retail; helper calls themselves
are not counted in integrated mode. Refresh intentionally mutates the
selected position and can replace g_Menu, exercising the post-call latch.

The five dispatcher mutants are rejected in both modes. Integrated tests
also reject ignored-class and lost-store mutations independently in each
of the four movement routines: 13 integrated negative controls total.
Runner exits 0; shell syntax and scoped diff checks pass. Test-only change,
so no redundant production rebuild and no production correction claimed.
The refresh subtree, malformed indices, card providers and natural menu
runtime acceptance remain unverified. No commit or push; 100% goal active.

### Card detail character-name wrapper 801E6AE8

Refresh inspection reached 801E76EC and its first rendering helper,
801E6AE8. The latter used an integer instead of an entry pointer, computed
the character from pointer arithmetic without a byte load, used 0x7C
instead of retail 0x87C panel stride, and read resource/context through raw
SystemMenu offsets. Native 801E6AE8 now takes u8*, reads entry[0x1C+slot],
adds glyph base 0x14E, uses typed unk2DC/renderContext, and passes the
0xA98+slot*0x87C panel with actual D_801EA004/010 coordinates and scale
0x1000. Non-native signature/body remain behind the conditional unchanged.

New run_menu_card_name_test.sh extracts the production function and real
MenuPsxPointerSlot/MenuRawPointer helpers, pins the retail span and menu.bin
hash, and executes retail MIPS through its font-renderer call boundary.
The original native body fails the resource identity assertion. Repaired
code passes 3,072 fixtures at O0/O2/UBSan: three valid panel slots, every
character byte, and contexts 0,1,-1,INT32_MAX. Resource and entry pointers
are above 4GB; the existing PSX-width panel slot uses a low mapped buffer.
All seven renderer arguments and absence of wrapper buffer writes are
compared; actual font-renderer output is not tested here. Wrong-character,
wrong-stride, raw-resource, raw-context and wrong-scale controls reject.
Production LINK OK remains 54 function stubs / 581 data symbols.

Next confirmed caller defects: retail 801E76EC computes card+screen*512+
0xC94 into saved s1 BEFORE calling 801E61B0, and does not use its return.
Current C instead derives pEntry from that call's return and reads card
and render-context through raw inflated-menu offsets. Native entry storage
maps to card->unkB94 + screen*512 + 0x100; verify this caller next against
the original instructions with ownership-changing callees. 801E61B0 and
801E733C remain untranslated rendering routines, not pointer providers.
Top-level 801E781C follows the retail call order on static inspection but
has no complete refresh-subtree differential or natural runtime proof.
No commit/push; the full 100% goal remains active.

### Card detail caller 801E76EC pointer preservation

Reproduced the preceding caller diagnosis: the original C fails the
entry-pointer identity assertion when initializer 801E61B0 returns an
unrelated test buffer. Native 801E76EC now captures the typed card entry
before initialization, ignores the initializer return, reads the current
typed renderContext byte after callbacks, and calls 801E733C without a
spurious argument. Native declarations identify 801E61B0/801E733C as void
rendering routines. Non-native code is preserved behind conditionals.

run_menu_card_detail_test.sh pins the retail span and established menu.bin
hash and extracts the production caller plus actual raw-slot accessors.
16,384 native/MIPS fixtures pass at O0/O2/UBSan: screens 0..31, all eight
three-character presence masks, four callback owner-replacement patterns,
and 16 starting context bytes. Callbacks change renderContext after each
call, exercising byte truncation as well as current-owner reloads. Native
card pointers exceed 4GB; the existing four-byte panel slots use low mapped
buffers. Tests compare ordered calls and entry arguments, enable state at
the name callback, final owner, and both complete 0x3000-byte panel buffers.
Only the caller executes for real; all eight rendering callees are controlled
boundaries, not evidence of rendered text or a complete refresh subtree.

Seven negative controls reject: wrong entry offset, use of initializer
return as pointer, entry reload after initializer owner replacement, wrong
panel stride, raw render-context offset, skipped detail call, and enabling
an empty character slot. Name-helper regression remains 3,072 fixtures at
all three modes with its five controls. Production LINK OK: 54 function
stubs / 581 data symbols. Shell syntax and scoped diff checks pass.

Next subtree work includes 801E61B0/801E733C translations and 801E6B70:
the latter still contains raw menu offsets and suspicious 0x7C/0x320
strides/table handling that need their own retail trace before repair.
Malformed screen indices and natural card-menu runtime remain unverified.
No commit or push. Full 100% goal remains active.

### Card detail digit wrapper 801E6B70

Retail trace confirmed and native repair corrected: panel stride 0x87C,
glyph address panel+slot*0x87C+0xA98+0x320+count*0x50, horizontal coordinate
D_801EA01C+slot*0x50+i*8, typed resource/renderContext/digits[6+i], and
byte-counter addition of the font renderer's return value. The counter is
reloaded from the current menu after the font callback. The second value
at entry[slot+0x19] is still converted and the current panel's 0x1309 byte
cleared. Non-native implementation remains unchanged behind #else.

New run_menu_card_digits_test.sh pins the menu.bin hash plus 801E6B70 and
801C80B8 instruction spans. Both native and MIPS execute the real decimal
converter; only the font renderer is intercepted. Original source fails
the renderer argument assertion. Repaired source passes 24,576 fixtures
at O0/O2/UBSan: slots 0..2, every primary byte with complementary secondary
byte, renderer returns 0/1/3/255, stable/replaced owner, and four initial
digit patterns on the alternate menu. Compare all renderer arguments,
final owner, both digit arrays and both complete test panel allocations.
Resource/entry pointers exceed 4GB; PSX-width panel slots use low buffers.
Large test allocations permit observing counter wrapping without claiming
that arbitrary count values are valid in the actual retail allocation.

Eight negative controls reject: wrong panel stride, missing glyph base,
wrong glyph stride (bounded 0x54 perturbation), wrong horizontal stride,
unit increment instead of return accumulation, stale owner, wrong digit
index, and omitted second conversion. Name/caller regression suites pass
all modes and controls. Production LINK OK remains 54 function stubs /
581 data symbols. This is call/state parity, not font pixel or natural
card-menu acceptance. Remaining subtree includes 801E61B0, 801E6CFC,
801E6F5C, 801E68AC and 801E733C; keep the full goal active. No commit/push.

### Native halfword digit routine 801E6CFC

Replaced the native missing-body stub with a retail-derived translation;
the non-native INCLUDE_ASM remains untouched. Two unsigned halfwords are
read at entry+4+slot*2 and entry+10+slot*2. Both use the decimal converter
and last three digit bytes. Row one retains fixed columns; row two packs
visible columns independently of renderer return counts. Panel stride is
0x87C, glyph buffers are at 0xA98+0x500/0x5F0, glyph stride is 0x50, and
counter bytes are 0x130A/0x130B. Accumulation reloads the current menu after
the renderer callback. Actual D_801EA02C/030/034/038 supply coordinates.

Extended menu_card_digits_test.c with WORD_DIGITS and added
run_menu_card_halfword_test.sh. Baseline fails closed because native
801E6CFC is missing. The runner pins the original routine and decimal
converter against the established menu.bin hash. Real native and MIPS
decimal conversion execute; only font rendering is intercepted.
220,416 fixtures pass at each of O0/O2/UBSan. Every unsigned halfword is
covered at each slot 0..2, paired with its complementary second value;
256 uniformly spaced values additionally expand renderer returns
0/1/3/255, owner switching and alternate-owner digit presets. These are
not all combinations of two independent halfwords. Argument traces, final
owner, both digit arrays and complete test panel buffers are compared.

Seven controls reject wrong second-value offset, counter aliasing, glyph
base, fixed columns on row two, lost renderer result, stale owner and
wrong slot stride. Existing byte-digit (24,576) and detail-caller (16,384)
suites pass all optimization/sanitizer modes and controls. Production
LINK OK now reports 53 function stubs / 581 data symbols. Shell syntax and
scoped diff checks pass. Rendering pixels, valid real allocation limits
for abnormal counters, and natural card-menu acceptance remain unverified.
Next rendering subtree includes 801E6F5C, 801E68AC, 801E61B0 and 801E733C.
No commit or push; the full 100% goal remains active.

### Native two-digit byte pair 801E6F5C

Translated native 801E6F5C from its original instructions; non-native
INCLUDE_ASM is preserved. Values come from entry+0x10+slot and +0x13+slot.
Only digits[7..8] are rendered, including for inputs >=100; no invented
clamp or hundreds glyph. First row retains digit columns and second row
packs visible columns. Counter bytes are 0x130C/0x130D, glyph bases relative
to 0xA98 are 0x6E0/0x780, panel stride is 0x87C and glyph stride is 0x50.
Original D_801EA03C/040/044/048 provide coordinates. Renderer return values
accumulate with byte wrapping on the post-callback current menu owner.

Added BYTE_PAIR to the shared numeric fixture and a small
run_menu_card_byte_pair_test.sh entrypoint using the parameterized pair
runner. Only the two explicitly supported routines can select that runner.
Baseline fails closed for missing native implementation. Real decimal
conversion executes on both native/MIPS sides; only font rendering is
intercepted. 24,576 fixtures pass at O0/O2/UBSan: all byte values in each
of three slots, complementary secondary values, four renderer returns,
stable/swapping owner, and four alternate-owner digit presets. All byte
values are covered, not every independent pair of byte values.

Seven controls reject wrong value offset, counter alias, glyph base,
fixed columns in the packed row, lost return value, stale owner and wrong
panel stride. Halfword (220,416), byte-digit (24,576), and detail caller
(16,384) regressions pass all modes and controls. Production LINK OK now
reports 52 function stubs / 581 data symbols. Shell syntax and scoped diff
checks pass. No rendered-pixel or natural-menu acceptance is claimed.
Remaining subtree includes 801E68AC, 801E61B0 and 801E733C, plus unverified
downstream rendering and card providers. No commit/push; 100% goal active.

### Menu time-field prerequisite 801C7F34

Tracing untranslated 801E68AC exposed a prerequisite error: retail calls
801C7F34 with only a0 and consumes seven words at g_Menu+0x2EC..0x304.
The old C took an output pointer, wrote six bytes with different divisors,
and left those menu fields untouched. The native menu-frame caller even
discarded these bytes in a scratch buffer and incorrectly described a1
register residue as an output pointer. Retail overwrites a1 internally.

Repaired native 801C7F34(u32 ticks) to successively divide/reduce with
21600000,2160000,216000,36000,3600,600,60 and memcpy each four-byte result
into typed unk2EC. Full unsigned input and leading quotient are retained.
The native 801C7BF4 prototype/call now uses the one-argument contract; its
other behavior is unchanged. Non-native old signature/body are preserved
behind #else, without claiming a matching-code repair.

New run_menu_time_fields_test.sh pins original instructions/menu.bin and
compares the entire native SystemMenu against original MIPS word stores.
LEGACY_TIME=1 reproduced the old menu-state assertion failure. Repaired
code passes 534,698 fixtures at O0/O2/UBSan: the first 262,144 values,
262,144 deterministic full-width samples, and low/high quotient boundaries
with +/-1 neighbors for all seven divisors. This is not all 2^32 inputs.
Six controls reject wrong divisor, signed input, raw native offset,
one-byte stores, omitted final field, and clamping the leading quotient.
Production LINK OK remains 52 function stubs / 581 data symbols. Shell
syntax and scoped diff checks pass. No natural menu-frame runtime proof.

Next: translate 801E68AC, now able to call the real time helper. Retail
renders glyph 0xEE twice at panel+0x240C/+0x24AC using D_801E9FE0[0..1],
calls the formatter on entry word 0, renders seven current-owner time
words at +0x254C+i*0x50 using D_801E9FE0[2+i], then glyphs 0x17/0x32 and
tens/ones of entry[0x23]+1. That final byte is re-read between renderer
calls; do not cache it across callbacks. 801E61B0/801E733C and remaining
rendering/card providers are still open. No commit/push; full goal active.

### Native card summary 801E68AC

Translated native 801E68AC, preserving non-native INCLUDE_ASM. The thirteen
font calls use original table coordinates and panel offsets, call the real
801C7F34 after the two initial labels, load each of seven time words from
the current typed menu, and re-read entry[0x23] between tens/ones rendering.
The byte is promoted before +1, so 255 becomes 256 rather than wrapping.
No callback owner or entry value is cached across the retail reload points.

New run_menu_card_summary_test.sh pins original summary/time instructions
and the established menu.bin hash. Baseline fails closed for missing native
implementation. Native and MIPS both execute the actual time formatter;
only the font renderer is intercepted. 13,312 fixtures pass O0/O2/UBSan:
all final-entry byte values, thirteen time inputs spanning divisor and
unsigned boundaries, stable/swapping menu owners, and optional mutation
of the entry byte and time word after each renderer call. All thirteen
argument tuples, final owner, both time-field arrays, entry bytes and both
complete panel buffers agree. Resource and entry pointers exceed 4GB;
four-byte panel pointer slots use low mapped test buffers.

Seven controls reject wrong label, coordinate table index, glyph stride,
time-field stride, cached entry counter, byte-wrapped increment and cached
menu owner. Time-field (534,698) and detail caller (16,384) regressions pass
all modes and controls. Production LINK OK now reports 51 function stubs /
581 data symbols. Shell syntax and scoped diff checks pass. Actual font
pixels and natural card-menu acceptance remain unverified. Remaining
refresh initialization includes 801E61B0/801E733C and downstream rendering
and card providers. No commit or push; full 100% goal remains active.

### Native card textured strip 801E733C

Translated native 801E733C while retaining non-native INCLUDE_ASM. Sixteen
double-buffered 40-byte POLY_FT4 quads start at panel+0x277C. Each uses
the current render context, low-halfword D_801EA04C/050 coordinates,
12x16 geometry, U coordinates i*16 and i*16+12, and V endpoints F0/FF.
The destination is reloaded after initializer 801E927C, GetTPage and
GetClut, matching retail even if those callbacks change ownership/context.

New run_menu_card_strip_test.sh pins the original instructions/menu.bin
and fails closed when the native body is missing. 2,048 fixtures pass at
O0/O2/UBSan: eight X and Y edge values, both render contexts, all eight
owner-change masks for the three callback kinds, and optional context
flips. Compare both full 0x3000 panel buffers, initializer pointer trace,
SDK arguments, final owner and contexts. The test asserts POLY_FT4 size40;
expanded primitive pointer layouts are not certified. Menu pointers exceed
4GB while existing four-byte panel slots use low mapped buffers.

Seven controls reject omitted quad, geometry width, U endpoint, V endpoint,
ignored context, stale texture-page destination and stale CLUT destination.
The 16,384 detail-caller suite passes all modes/controls. Production LINK
OK now reports 50 function stubs / 581 data symbols. Shell syntax and
scoped diff checks pass. The initializer and SDK lookups are controlled
test boundaries, not full integration or rendered-pixel proof. Remaining
refresh initializer 801E61B0 and downstream rendering/card-provider gates
remain open. No commit or push; the full 100% goal remains active.

### Native card-detail initializer 801E61B0

Translated native 801E61B0; non-native INCLUDE_ASM is retained. Three panels
use 0x87C stride. Nine shared label words skip exactly 0xFFFF (not -1),
render at 0xA98+0x50+count*0x50, and accumulate the renderer return into
current-owner byte 0x1312. Current-context marker 0x130E precedes quad
initialization at offset 0x12B8; page/clut stores reload after GetTPage.
Atlas U comes from the word at D_801EA590+slot*4, multiplied by four and
masked FC; V is the byte at D_801EA5DC+slot*4. Geometry uses low-halfword
table coordinates plus slot spacing and 72x13 dimensions. Marker 0x1311
is written after the geometry callback on the current menu.

New run_menu_card_init_test.sh pins the original instructions/menu.bin.
Baseline fails closed for missing native implementation. 12,288 fixtures
pass at O0/O2/UBSan: all 512 label-presence masks, returns 0/3/255, four
owner-switch patterns and both initial contexts. Every callback flips the
current context. Fixtures include the non-sentinel word FFFFFFFF and
distinct atlas halfwords to distinguish four-byte from two-byte strides.
Complete panel buffers and ordered callback arguments agree with MIPS;
resource pointers exceed 4GB and native primitive size40 is asserted.
Font/quad/texture-page/geometry calls remain controlled test boundaries.

Seven controls reject wrong sentinel, omitted panel, lost renderer result,
wrong label base, wrong atlas stride, stale page destination and wrong
final marker. The 16,384 detail-caller regression passes all modes and
controls. Production LINK OK now reports 49 function stubs / 581 data
symbols; shell syntax and scoped diff checks pass. The caller's previously
missing initializer and strip routines now have native bodies, but full
refresh integration and actual rendered output are not proved. In
particular 801E71B4 and 801E6668 still need inspection alongside providers.
No commit/push; the full 100% goal remains active.

### Native card name upload wrapper 801E71B4

Repaired native card-pointer mapping, name-copy stride and atlas stride.
Retail reads pairs at entry+character*20+i+0x24/+0x25 with i advancing by
two; old C multiplied i by two again. Native uses typed unk32C->unkB94
with +0x100+screen*512, an aligned u16 raw buffer, at most ten pairs and
termination only when both bytes are zero. Atlas coordinates are halfwords
at four-byte spacing (indices charIdx*2 in the existing u16 declarations).
Allocation/clear 0x3F6, rendering arguments, 40x13 upload rectangle,
DrawSync and free order retain retail behavior. Non-native body unchanged.

New run_menu_card_upload_test.sh pins the original instructions/menu.bin.
Old native body segfaults. Repaired body passes 23,232 fixtures at
O0/O2/UBSan: all 32 screen slots, three display slots, name records 0..10,
terminator lengths 0..10 and complementary zero/nonzero byte patterns.
Compare copied pair prefix/count, seven-call boundary sequence, decoded
buffer forwarding, allocation clearing/guards, rectangle and pointer
identity. Native card/allocation pointers exceed 4GB. Six controls reject
wrong pair order, wrong count, one-byte termination, atlas stride,
allocation size and omitted synchronization. Wrong-pair perturbation
keeps source accesses inside the name record and copied bytes initialized.
The 16,384 detail caller regression passes all modes/controls. Production
LINK OK remains 49 function stubs / 581 data symbols; scoped checks pass.

IMPORTANT unresolved downstream gates: SystemRenderStringEntry currently
stores pString/pWork as u32 in its native descriptor, so this wrapper's
stack decoded buffer is not proven usable by the real backend. The test
intercepts the decoder and renderer. LoadImage is also intercepted: the
40*13*2-byte nominal transfer exceeds the requested 0x3F6 allocation by
26 bytes. Actual allocator rounding and upload reads need authority-backed
inspection before labeling an overread or changing the allocation. The
fixture does not read that extra range and does not certify real uploads.
Name IDs beyond 10, full refresh integration and natural card-menu output
remain unverified. No commit/push; the full goal remains active.

### String renderer descriptor boundary diagnostic

Added run_string_descriptor_boundary_test.sh and its C witness, extracting
the current production SystemRenderStringEntry body. O0/O2/UBSan reproduce
loss of a high stack string pointer at descriptor+0x1C and independently a
high work pointer at +0x2C. A MAP_32BIT low-address control preserves both;
the non-PIE static descriptor also preserves its internal +0x90 row pointer.
The mock stops at func_80033DF0 before any narrowed pointer dereference.
Outputs explicitly say KNOWN_FAILURE and NOT_ACCEPTANCE: successful runner
exit confirms the defect witness, not a repaired renderer. No production
change in this diagnostic step. A repair must trace descriptor consumers,
including script advancement and nested script state, without widening
four-byte slots over adjacent fields. Retire or reverse this witness when
the actual pointer transport is repaired.

Upload inspection: LoadImage forwards width/height to GR_CopyVRAM, whose
non-null-source path copies w*2 bytes per row and advances source by w.
Thus the 40x13 rectangle reads 1040 contiguous source bytes. HeapAlloc in
system/memory.c rounds the 1014-byte request to 1016; it also has whole-block
versus split-block behavior. Actual linked allocator/runtime allocation
extent remains unverified, so this is not yet a confirmed allocation
overread and does not authorize invented padding or changed retail sizes.
Full renderer/upload integration and natural card-menu output remain open.
No game/debugger/build process observed during the final process check.
No commit/push; 100% goal remains active.

### Native upload allocation extent confirmed (controlled heap)

run_card_upload_heap_extent_test.sh extracts actual HeapAlloc and
GR_CopyVRAM bodies and executes them at O0/O2/UBSan. All three confirm that
the flags=0 split allocation provides 1016 payload bytes for the 1014-byte
request. A 40x13 upload reads the two alignment bytes, the next eight-byte
heap header and sixteen bytes of the following free payload. The test
compares those bytes in the copied VRAM rows, not just the rectangle args.
The whole arena is mapped, so this is a confirmed allocation-boundary
crossing, not an unmapped-host-memory read. The current native link map
resolves HeapAlloc to src_slus_006.64_system_memory.c.o at 0x4e02f6;
include/system/memory.h preserves the eight-byte header with mem_addr.
This supersedes the previous unverified split-allocation extent only.
Whole-block allocation cases, natural card-menu state, retail heap bytes
and whether the extra pixels are sampled remain unverified. No production
allocation-size change or invented padding was made. The diagnostic uses
a controlled free block and stops if consolidation or error handling is
unexpectedly needed. Existing allocator cast warnings remain visible.

Renderer pointer-consumer trace: func_80033DD4 copies descriptor+0x1C to
+0x20 and accepts its new script argument as s32; func_80033DF0 reconstructs
the script from u32, advances it in glyph/control cases and restores +0x20
plus one on nested return. It also reads row/work pointers at +0x28/+0x2C.
Other window paths own those same packed fields (constructor, queue at
80034888, destruction and upload), so widening fields in place or fixing
only the SystemRenderStringEntry assignment is insufficient. A native
pointer-transport repair still needs push/pop/advance and actual raster
coverage, including ordinary windows remaining unchanged. The existing
string_descriptor_boundary test is still a known-failure witness.
Scoped shell/diff checks pass; production was not rebuilt (diagnostic-only
changes). No commit/push; full goal remains active.

### Native string renderer pointer transport repaired

SystemRenderStringEntry now retains full-width script, saved-script, row
and work addresses beside its static packed descriptor. Shared interpreter
accessors select that storage only for the exact static descriptor; ordinary
windows retain four-byte pointer fields. Native func_80033DD4 accepts a
pointer rather than s32, and push/pop plus every interpreter script advance
preserve that pointer. Row/work reads in the interpreter use the same
accessors. No packed fields were widened, input buffers relocated, or
allocation sizes changed. Static lifetime/non-reentrancy remains unchanged.
The non-native push helper retains its original signature/body, and the
non-native accessor macros operate on the original four-byte fields.

New run_string_render_pointer_test.sh links the real system translation
unit (wrapper, interpreter, nested lookup, width and raster code). Before
repair its O0 run segfaulted. Repaired code passes 84 high-string/work
fixtures per mode at O0/O2/UBSan in both non-PIE and PIE fixtures (504 total).
Coverage: 0..10 single/double-byte glyphs, nested table/name push/pop with
following caller glyph, three immediate control codes, guards, expected
widths and complete work-buffer equality against low-address input. PIE
also proves high static descriptor/row and nested GameState-name pointers;
font/archive pointer slots remain deliberately low-address. Synthetic font
data means this is pointer/raster integration, not visual retail acceptance.
Six controls reject narrow stores, narrow saved/nested pointers, raw-word
script advance, raw work and raw row reads. Raw advance initially survived
equivalence-only checks; adding independent expected widths exposed it.

The former string_descriptor_boundary known-failure witness is now a
passing pointer-transport boundary contract (O0/O2/UBSan), with the packed
low-word mirrors checked separately. Actual raster coverage is in the new
test, not its boundary mock. Existing textbox timing O0/O2/UBSan executions
completed with matching output and empty stderr. Its runner's report stage
failed because container rg is absent; equivalent grep/cmp checks verified
the generated reports. Retail glyph raster runner passed on the host:
144 differential cases per mode and both row-outline mutants rejected,
artifact /tmp/xeno-glyph-raster-retail.tss5hj. Container clang was absent;
no dependency installation or check suppression was used to run that test.

Production LINK OK remains 49 function stubs / 581 data symbols. Scoped
syntax/diff checks pass. This closes the reproduced static string-renderer
pointer-loss path, not all renderer behavior, ordinary high-pointer window
support, natural card-menu acceptance or the upload's allocation-boundary
crossing. Full translation/wrapping goal remains active. No commit/push.

### Card name footprint and detail packet-stride repair

Native 801D02D8 selected both detail FT4 buffers at a 24-byte stride.
Retail 801D0844..54 and 801D087C..90 multiply the context by 40. Repaired
only those two offsets to sizeof(POLY_FT4); original non-native body and
the remaining native routine are untouched. New
run_card_name_upload_footprint_test.sh reproduces the old buffer-1 address
assertion failure and passes O0/O2/UBSan after repair. Reintroducing either
24-byte stride independently is rejected. Tests exercise actual native
initializer, geometry helper and detail submission with AddPrim captured;
font emission and primitive initialization are mocked. Context markers
are uniform across panels in this bounded test, with opposite values for
the glyph and name marker to exercise both selectors. No claim of full
801D02D8 parity: owner reloads, mixed markers and icon branch need an audit.
In particular retail 801D083C reads the first quad's context from the base
panel +0x130F, while current native code reads the per-panel marker.

Pinned menu.bin atlas words: X offsets 24,0,24 and Y 98,111,111. The test
models the retail 40x13-word uploads in display-slot order for all eight
presence masks and both contexts, then enumerates interior UV addresses
from the actual generated 72x13 name quads. All 22,464 texel addresses per
mode select their own upload at byte offsets below 1014. The second
upload's last-row tail overlaps the third name region, but a present third
character overwrites it afterward; absent characters have no name quad
submitted in the tested detail branch. This is an address-provenance model,
not actual LoadImage/raster/GPU integration. Filtering, edge conventions,
other texture consumers and natural card-menu output remain unverified.
The allocation-boundary crossing itself is unchanged; no padding or size
change was introduced on the basis of this footprint result.

801E6668 is a separate still-untranslated card-title bitmap rasterizer
(retail allocates 0x100/0x1000 and uploads 64x32 words at 320,224), not the
name-quad submitter. Title rasterization and card providers remain open.
Production LINK OK: 49 function stubs / 581 data symbols. Scoped checks
pass. No commit/push; full goal remains active.

### Detail first-quad context selector repaired

Reproduced the remaining 801D02D8 first-quad marker mismatch by varying
all three +0x130F markers independently. Retail 801D083C reads the base
panel's marker before 801D0840 adds the character offset. Native now uses
data[0x130F] for that AddPrim only; the name quad retains its per-panel
+0x1311 marker, and text-list calls retain their existing selectors.
No owner-reload or icon-branch changes were made in this step.

Expanded run_card_name_upload_footprint_test.sh passes 128 combinations
(two contexts, eight presence masks, eight independent +0x130F marker
patterns) at O0/O2/UBSan. The previous native selector fails the packet
address assertion. All three controls are rejected: either 24-byte packet
stride, or the old per-panel first-quad selector. The name-rectangle
address model checks 179,712 texel addresses per mode below the clear
extent, with the same prior GPU/filtering limitations. The +0x1311 name
markers remain uniform per render-context fixture, not independently mixed.
Production LINK OK remains 49 function stubs / 581 data symbols; scoped
syntax/diff checks pass. Full 801D02D8 parity, callback owner reloads,
icon-side typed card layout, title rasterization and natural output remain
open. No commit/push; full translation/wrapping goal stays active.

### Card icon branch typed layout and call-boundary reloads

Repaired 801D02D8's non-detail branch to read typed MenuUnk2.unk4F7C,
unk4F80[0x2E + D_801E981C[selected]], directory bytes in unk0 and the
animation word via memcpy from SystemMenu.unk4CC. Previous raw offsets
4F7C/4FAE/4CC incorrectly assumed retail layout in native structs.
The panel pointer now reloads after func_801CE2B4, and the palette result
is stored after reloading panel/context following GetClut (801D067C..06A4).
The shared footer reloads panel state at the equivalent of 801D0918.
No detail-loop owner-reload or title-raster changes in this step.

New run_card_icon_draw_test.sh pins menu.bin and executes its original
801D02D8 instructions against the native function: 46,080 fixtures per
O0/O2/UBSan, covering all 30 selection indices, 32 icon IDs, three animation
frames, both contexts and eight owner-replacement masks at the text-list,
GetClut and first AddPrim boundaries. Native menus/cards are above 4GB;
packed panel pointers stay low as required by their representation.
Compare all 0x6000 panel bytes, four boundary events and final owner.
The old implementation fails the whole-panel comparison. Six controls
reject raw selection/frame offsets, wrong map base and stale initial,
palette-store or footer destinations. Text-list rendering, GetClut and
AddPrim remain intercepted: this is memory/call parity, not GPU proof.
Owners change between callbacks; no unsupported concurrent-memory model
is claimed for instruction-by-instruction native stores.

The existing detail footprint/selector regression passes all modes and
three controls. Production LINK OK remains 49 function stubs / 581 data
symbols; scoped checks pass. Remaining gates include detail-loop reload
parity, card title rasterization/providers, real submission and natural
card-menu output. Full goal remains active; no commit/push.

### Detail-loop owner reloads repaired

801D02D8's detail branch now reloads panel state before each character,
each of its seven text-list calls, both quad submissions and each summary,
suffix and strip call. Seven retail list descriptors are represented by
local static offset tables. The character-enable predicate remains a
single check before that character's calls, even if later callbacks switch
to an owner where that character is disabled. First-quad base marker and
name-quad per-character marker semantics remain as previously repaired.

run_card_detail_draw_test.sh selects a detail mode of the existing pinned
retail/native draw fixture. The pre-repair code fails its event comparison.
Repaired code passes 16,896 cases per O0/O2/UBSan: eight presence patterns
(complemented in the alternate owner), sixteen counter/marker seeds, both
render contexts, two label-marker patterns and 33 replacement schedules
(none, every callback, or one of the 31 possible boundary positions).
Compare all events, final owner and both panel buffers. Nine controls
reject stale character/list/first-quad/name-quad/summary/suffix/strip/footer
owners and a wrong list-counter offset. Text-list rendering and AddPrim
remain intercepted; this does not prove GPU or natural menu acceptance.
Counters are varied byte patterns, not exhaustive combinations of all
seven counters, and mutation schedules are not all subsets of boundaries.

Icon regression: 46,080 cases per mode and all six controls pass. Name
footprint/selector regression: 128 cases per mode and all three controls
pass. Production LINK OK remains 49 function stubs / 581 data symbols;
scoped syntax/diff checks pass. Detail and icon branches now have bounded
retail call/memory comparisons, but title rasterization, providers, actual
draw integration and natural card-menu output remain open. No commit/push;
the full translation/wrapping goal remains active.

### BIOS KROM provider authority: zero-scratch census

Local disc/scph5500.bin still matches SHA256
11052b6499e466bbf0a709b1f9cb6834a9418e66680387912451e971cf8a1fef.
B0 vector 0x51 at ROM 0x104B8 contains RAM entry 0x65E0. Its code is at
ROM 0x160E0, with the mapping helper at RAM 0x6670. Outer bounds select
ROM bases BFC66000 or BFC69D68, then add helper-index*30; mapping requires
the helper too, not only those outer bounds.

New run_bios_krom_mapping_test.sh executes that complete BIOS path using
the existing MIPS adapter, relocated low-RAM code/tables from ROM+0xFB00
and explicit ROM reads. With helper scratch fixed to zero, O0/O2/UBSan
all agree for every 16-bit code and
upper-word aliases 0x1234/0xFFFF: 4,947 non-sentinel results, 60,589 -1
results, min BFC66000, max BFC7F8C0, address-stream FNV32 D124E60A.
All non-sentinel addresses admit the title rasterizer's 32-byte read
inside the 512KB ROM under that scratch condition. This is exhaustive
input mapping evidence for this specific BIOS/state, not a universal
mapping, native provider or glyph/render certificate. See the stack-state
correction below before using the census to implement a provider.

Next provider work has a concrete authority: implement/verify the mapping
against this oracle and expose host-readable bytes from the supplied ROM.
Do not return raw BFC addresses to the native title helper: its int-to-
pointer path would not translate them into host memory. Do not embed BIOS
payloads in source/Git or invent a replacement font. Krom2RawAdd2 (B0 0x52)
is not covered by this run. Production code unchanged in this diagnostic
step, so no rebuild; scoped syntax/diff checks pass. Full goal remains
active, with no commit/push.

### Card-title character-code conversion repaired

Native 801E65E4 now maps bytes below 0x20 to 0x8140 (the retail delay slot
at 801E6614 supplies the 0x81 lead), uses D_801EA5D0[lead] directly for
0x20..0x7F without reversing its bytes, and preserves lead/trail order for
0x80..0xFF. D_801EA8C0 is zero for the first two cases and one for the last.
The non-native body remains unchanged. This fixes code conversion before
the still-untranslated title rasterizer, not the KROM provider itself.

run_card_title_code_test.sh executes the pinned retail 801E65E4 against
native code for every two-byte input, with a low-address success result
and -1 provider sentinel: 131,072 fixtures per O0/O2/UBSan. Compare the
provider argument, byte-count flag at the call and returned value. Old
native code fails immediately on the provider-argument assertion. Four
controls reject the wrong control lead, swapped table bytes, wrong wide
flag and shifted 0x80 threshold. Krom2RawAdd is intercepted; high-bit BIOS
addresses and actual glyph bytes are not certified by these fixtures.

Current PsyCross LIBAPI.C Krom2RawAdd and Krom2RawAdd2 remain unimplemented
and return zero. Retail 801E6668 checks only -1 before reading 16 glyph
halfwords, so simply translating that body does not make a usable native
font path. Provider authority/address mapping must be resolved; do not
substitute fabricated glyphs or label the title rendered. Production LINK
OK remains 49 function stubs / 581 data symbols; scoped syntax/diff checks
pass. No commit/push; full goal remains active.

### KROM mapping stack-state correction

Disassembly of the mapping helper found that byte-gap branches for leads
81..84 load an uninitialized selector from helper sp+0x1C (for example
RAM 0x6780, ROM 0x16280). With the outer 32-byte and helper 40-byte frames,
this is entry-SP minus 44. The prior oracle's zeroed RAM implicitly made
that selector zero; it did not establish a universal code-only mapping.
run_bios_krom_mapping_test.sh now explicitly seeds it and labels the census
zero-scratch-only. Across all 65,536 codes, 371 differ for selectors 0/1.
Example 817F returns BFC66762 versus BFC66744. All 256 selector values
preserve 8140 -> BFC66000. The pinned menu table's 128 single-byte input
mappings show no difference for selector 0/1 (not proof for every possible
stack state). O0/O2/UBSan reproduce these results.

Do not implement byte-gap behavior by silently choosing zero, a replacement
glyph, or -1. Separate defined ranges from stack-dependent inputs, and
trace real caller state if those gaps are reachable. A native mapping API
can represent the dependency explicitly; its callers must not invent the
missing state. Host ROM pointer translation remains required independently.
No production provider was added in this diagnostic step. Scoped checks
pass; no rebuild, commit or push. Full goal remains active.

### Native KROM address mapping translated with explicit scratch state

pc_port/src/krom_mapping.c now implements PcPortKromMapAddress, returning
a guest ROM address (or UINT32_MAX), not a host pointer. Its API requires
the verified SCPH-5500 ROM and the caller's explicit scratch selector.
Table data remains in the supplied ROM; no licensed font/table payload is
embedded. The implementation preserves low-16-bit input masking, interval
selection, stack-selected gap records and modulo-32-bit address arithmetic.

run_native_krom_mapping_test.sh compares the native implementation against
the actual BIOS instructions for all 65,536 codes with all 256 selectors,
plus the earlier census, upper-word alias and boundary checks: 17,105,413
comparisons per O0/O2/UBSan, all passing. Six deliberate errors are rejected
with native/BIOS address differences: lost scratch state, wrong glyph
stride, wrong trail threshold, wrong base, wrong bound and lost input mask.

This is an isolated mapping translation, not runtime provider acceptance.
It is not wired into the production build or PsyCross Krom2RawAdd. Verified
ROM storage, host address resolution and actual caller state for reachable
gap inputs remain required. No title raster/GPU execution or fresh full
production build was performed in this step. No commit/push; full goal
remains active.

### Card-title raster translated; menu/field symbol collision separated

Native menu 801E6668 now expands the provider's 32 glyph bytes into 16x16
binary pixels, executes the existing 801E6544 compression, packs 12 columns
into four-bit output, and uploads the retail 320,224,64,32 rectangle. It
preserves 256/4096-byte allocations, clearing, -1 glyph skips (which still
consume a title position), byte-count flag advancement, 64-byte/32-glyph
limits, DrawSync and free order. Typed unkB94+4+screen*512 accounts for the
host-expanded card TIM_IMAGE. Non-native assembly remains unchanged.

run_card_title_raster_test.sh pins menu.bin and compares the actual retail
raster and compression instructions with extracted native bodies: 6,912
fixtures per O0/O2/UBSan pass. Coverage: all 32 screens; lengths 0,1,15,16,
17,31,32,33; narrow/wide/alternating advancement; no/all/periodic rejection;
zero/full/row-varying glyph bytes. Full scratch/output allocations and
guards, provider offsets, upload rectangle and call counts are compared.
Seven errors reject: card offset, byte order, missing final glyph row,
output row stride, title index stride, glyph limit and missing clear.
The converter regression also passes 131,072 fixtures per mode and its
four negative controls. Font lookup, allocation and GPU calls are test
boundaries: these results do not certify BIOS-backed title rendering.

The build exposed two integration failures, both repaired: krom_mapping.c
was absent from the explicit PORT_SOURCES list; and menu 801E6668 collided
with the field-object two-pointer routine at the same overlay address.
The menu-local native alias is now MenuRenderCardTitle. Object relocations
confirm both calls in 801E781C resolve to it, while the linked field symbol
func_801E6668 remains distinct. LINK OK: 49 function stubs / 581 data symbols.
The unchanged stub count had hidden this missing menu translation because
the unrelated field definition already satisfied its name.
The field node-flag transfer regression passes 15,360 full-memory retail
comparisons per O0/O2/UBSan and all five negative controls. Its runner was
run on the host after the container stopped for missing Clang. Scoped
shell syntax and diff checks pass.

Krom2RawAdd remains unimplemented and returns zero, whereas this faithful
raster checks only -1. Therefore a natural nonempty title is NOT usable
yet and may read an invalid pointer; do not launch this as card acceptance
or add invented replacement glyphs/sentinel behavior. Next required work:
verified ROM storage, guest-to-host font resolution, and real caller state
for reachable stack-dependent codes. KROM mapping is now built but still
has no production provider caller. No commit/push; full goal remains active.

### KROM host-span boundary added, without invented selector state

PcPortKromResolveRomSpan now resolves a mapping result into a borrowed span
of the caller-verified 512KB BIOS. It distinguishes OK, BIOS REJECTED (-1),
NEEDS_SELECTOR and OUTSIDE_ROM. Failures clear the output pointer. Length
checks use subtraction to avoid overflow; no guest address is cast into a
host pointer or masked into RAM. Unknown selector state is represented by
NULL: a span is returned only when all 256 possible mappings agree. The
zero selector is a comparison anchor, not an assumed guest stack value.

The BIOS instruction oracle checks 17,105,414 mappings and 17,170,587 span
results per O0/O2/UBSan. Every 16-bit code is tested with every selector;
the unknown-state result is independently checked against that complete
oracle census. Exactly 371 codes require explicit state. Span lengths
include 0,1,30,31,32, the exact remaining ROM extent, one byte beyond it,
and SIZE_MAX. Five resolver mutants reject assumed selector, stale output,
wrong rebase, short bound and overflow-prone bound; all six earlier mapping
mutants also reject. LINK OK remains 49 function stubs / 581 data symbols;
scoped syntax/diff checks pass.

Runtime integration is still NOT_DONE. The API deliberately retains the
mapper's caller-verified ROM precondition and does not load, hash or own
ROM storage. The current toolchain container has neither libcrypto.pc nor
openssl/sha.h; no new dependency was installed. A verified immutable ROM
loader is still required before wiring the provider. Both menu 801E65E4
and field 800ABFDC call Krom2RawAdd. Field 800AC0F0 stores that return in
s32, casts through u32 to a glyph pointer, and only rebases the 0x80000000
RAM segment; BFC ROM addresses are not resolved. Its 800AC03C reader uses
15 halfword rows (30 bytes), unlike the title's 32 bytes. Preserve the
separate sourceIndex==1 atlas-index branch when widening this mixed result.
Do not convert NEEDS_SELECTOR/OUTSIDE_ROM into the BIOS rejection sentinel.
No natural card/field font rendering was run; no commit/push. Goal active.

### Verified KROM ROM ownership added; OpenSSL build dependency

pc_port/src/krom_rom.c/.h now provide an opaque owned ROM handle, explicit
load errors, destruction and borrowed-span resolution through the tested
mapper. Load accepts only a regular file of exactly 0x80000 bytes and the
pinned SCPH-5500 SHA256. It hashes the same owned bytes subsequently used
for lookup, not a second pathname read. No payload is embedded, no path
search or fallback BIOS is introduced, and no missing selector is invented.
O_NONBLOCK permits rejecting a FIFO without blocking at open. Interrupted
and short reads are handled; early EOF, read/close failures and appended
bytes fail without publishing a handle. File changes/deletion after load
cannot affect borrowed spans. Independent handles own independent copies.

run_krom_rom_test.sh passes 65,936 checks per O0/O2/UBSan/ASan: missing/NULL/
directory/FIFO inputs, four wrong sizes, 129 corrupted payloads, allocation
failure, interrupted/short/failed reads, early EOF, trailing byte and close
failure, all-code unknown-selector resolution, explicit gap selectors,
copy lifetime and allocation/free accounting. Initial O2 fault injection
missed Ubuntu's fortified __read_chk call: object disassembly identified
the bypass, and the harness now wraps read and __read_chk without disabling
fortification. Four mutants reject missing hash pin, wrong size, stale
output and discarded selector. No natural GPU/title acceptance is implied.

The implementation uses supported OpenSSL 3 one-shot SHA256 with a supplied
digest buffer (https://docs.openssl.org/3.0/man3/SHA256_Init/). The full port
driver now checks for libcrypto >=3.0, compiles krom_rom.c, and links it;
README and dev/xenogears/Dockerfile include Ubuntu libssl-dev. LINK OK:
49 function stubs / 581 data symbols; nm confirms all three loader symbols
and readelf confirms libcrypto.so.3. Scoped syntax/diff checks pass.

IMPORTANT for subsequent builds: use image
localhost/xenogears-dev-toolchain:krom-20260913 (ID
21f06ce48b93a0a3dfe5580b22f7ba3370d781ea0ab413c5d3475047102c3390).
It derives from the unchanged :current image
94d6a0111bd27464273aa464784d43b08e7d3750ce371ef83bea6db9585e9f29,
adding Ubuntu libssl-dev and updating libssl3t64/openssl to
3.0.13-0ubuntu3.15. The current image remains untouched but lacks this new
development dependency. Derived recipe: /tmp/xeno-krom-toolchain.AeKGmf/Dockerfile.
No packages were installed on the host. The interrupted image build was
resumed by polling its existing handle, not restarted.

Next: wire the verified handle into native font access and fix menu/field
full-width pointer transport, retaining BIOS rejection versus unknown-state
errors as distinct outcomes. No runtime font caller uses this loader yet;
the previous nonempty-title warning remains in force. No commit/push; the
full translation/wrapping goal remains active.

### Menu title font provider wired; full-width BIOS-backed raster parity

PcPortKromFont is now the native menu provider. It initializes once through
pthread_once, using XENO_BIOS or disc/scph5500.bin when unset, and retains
the verified ROM for process lifetime so concurrent readers cannot receive
invalidated spans. It returns full-width host pointers, or pointer -1 only
for the BIOS rejection sentinel. Load failure, unresolved selector state
or a non-ROM span emits KROM_UNRESOLVED and aborts. No zero selector, blank
font or rejection value is invented for those unresolved cases. Environment
configuration must precede the first call. README documents the requirement.

Native menu 801E65E4 now requests 32 bytes from this provider; non-native
code is unchanged. The regression first failed on high-address transport
through the old int-returning SDK. Updated code conversion tests pass
196,608 fixtures per O0/O2/UBSan, including a high host pointer for every
two-byte input, low success and -1. The four old mutants plus restored
pointer narrowing reject. Object relocation confirms PcPortKromFont is
the production menu target, not Krom2RawAdd.

run_krom_font_test.sh passes O0/O2/UBSan with eight concurrent first calls,
real high-address glyph bytes, input aliases, rejection and cached ownership
after an environment change. Child-process checks require SIGABRT for
missing/wrong BIOS, unknown-selector code 817F and an oversized span.
The loader regression still passes 65,936 checks per four build modes and
its four mutants. No thread-sanitizer certification is claimed.

CARD_TITLE_REAL_KROM=1 bash pc_port/tests/run_card_title_raster_test.sh now
compares the real native converter/provider/compressor/raster with actual
retail menu and BIOS instructions, including ROM font reads: 2,304 fixtures
per O0/O2/UBSan pass, with all seven raster mutants rejected. Coverage is
32 screens, lengths 0,1,15,16,17,31,32,33, narrow/wide/alternating streams,
and A/B/C plus selector-independent 8140/889F/9872 wide codes. No arbitrary
guest scratch state is needed for these codes. Allocation and GPU calls
remain intercepted; full scratch/output buffers and guards are compared.
The default mocked mode also passes 6,912 fixtures per mode and seven
mutants, retaining rejected-glyph schedule coverage.

LINK OK remains 49 function stubs / 581 data symbols using the
krom-20260913 toolchain. Scoped syntax/diff checks pass. The former blanket
nonempty-title null-provider warning is superseded for this menu path,
not a claim of natural GPU/card-screen acceptance. Field 800ABFDC/800AC0F0
still use the SDK int result and need full-width integration while retaining
the atlas-index branch and 30-byte glyph reader. The broad 100% port and
decompilation objective remains active; no commit/push.

### Field font pointer transport and provider integration repaired

Native 800ABFDC now returns intptr_t and requests 30 bytes from
PcPortKromFont for its ROM branch. Native 800AC0F0 keeps that result at
pointer width and passes the host span directly to 800AC03C, without
truncating through u32 or mistaking host bits for a guest RAM segment.
The atlas-index range [8540,8880), output flag timing, /7 coordinates and
non-native code remain unchanged. Repository search found no other caller
or declaration of 800ABFDC needing the native signature change.

The existing field glyph-stream test first crashed with a stack glyph
above 4GB on the old implementation. It now passes O0/O2/UBSan: 131,093
checks including all 65,536 codes with both high host pointer and -1 mock
provider results; exact atlas range/flag/call exclusion; and a high glyph
through the real stream caller and 15-row rasterizer with all upload bytes
checked. The existing /9 negative control still rejects. The all-code test
uses the interval established by retail ABFF8..AC028, not MIPS execution
for every fixture. Provider data and GPU are intercepted in this test.

run_field_bios_font_test.sh separately runs the actual field TU plus native
ROM loader/provider on a mixed 8548 atlas index and 889F ROM glyph stream.
All three build modes pass: the atlas MoveImage rectangle, all 27 upload
buffers (including trailing blanks), returned stream position and remaining
count match expectations. The 30 reference bytes are read directly from
the pinned BIOS at 69D68, the address established by the BIOS instruction
oracle. The glyph produces 86 nonzero pixels. GPU calls remain intercepted;
this is neither natural screen acceptance nor a full field-content census.

LINK OK: 49 function stubs / 581 data symbols in krom-20260913. The field
object relocation resolves to PcPortKromFont. Scoped syntax/diff checks
pass. README no longer describes field provider wiring as unfinished.
Actual field/title GPU acceptance, reachable selector-dependent inputs,
and the wider port/decompilation backlog remain open. Retail field behavior
for a rejected font pointer was not changed into a fabricated blank glyph.
No matching-build or whole-game completeness claim; no commit/push.

Additional gate: fe60_transition_packet_test.sh exits 1 on its old literal
grep for `renderContextIndex * 0x30`; current AC99C uses `idx * 0x30`.
That function was not changed by this font repair. This script also carries
older memcpy/memset spelling expectations. It was left unchanged, not
disabled or counted as passing. Reconcile that static gate against current
retail-backed behavior before using it as an acceptance gate again.

### FE60 dependency gate reconciled and strengthened

The failing fe60_transition_packet_test.sh source-text expectations were
audited against current code and retail AC99C. The assembly now resides in
asm/field/matchings/main/misc9: ACAB8 reads one phase and ACAC8..ACAD4
writes it to +6A/+7E/+92/+A6. The old four `source + offset` expectations
were not the current instruction contract. Variable names, explicit byte
copy/clear loops and expanded second-quad assignments likewise differed
from old literal grep strings without changing the intended checks.

The gate retains symbol/stub, pointer-storage, packet-alias, geometry,
color and layout tripwires, updated to the current expressions. It also
requires the native full-width KROM call. Crucially it now executes three
pinned retail-derived behavioral suites and their negative controls on the
current field TU, instead of declaring PASS from text and symbols alone.
These are bounded source/behavior checks, not GPU or whole-game acceptance.

AC99C coverage expanded from 17 to 401 checks: every sprite and draw-mode
link and every destination phase in all 16 rows/four strips/both contexts.
A new source mutant omits the third-strip +92 phase store and is rejected
with `ASSERTION transition.ot field=all.phase actual=0 expected=4`; the
original stale-setup-OT mutant still rejects. Runner compile/link/run
failures now propagate explicitly even during expected-failure controls.

Fresh aggregate result: PASS, including glyph stream 131,093 checks/mode,
packet setup 46 checks/mode and OT 401 checks/mode, all O0/O2/UBSan, with
their four total negative controls. Scoped syntax/diff checks pass. The
earlier failing-gate note is superseded by this run. No gameplay source
changed or production rebuild was needed in this step. Natural rendering,
whole-game coverage and full matching/decompilation remain open. No commit
or push; the full port/decompilation goal remains active.

### Retail disc validation 801E93A0 translated (bounded native evidence)

Native menu 801E93A0 now implements the retail 801E93A0..801E96A0
instruction contract; the non-native INCLUDE_ASM remains unchanged. This
replaces the success-returning native stub, not the CD provider or the
surrounding disc-change UI. The routine retains lid-open/lid-close waits,
ready-status plus command-result polling, GetTN/Setloc/SeekL retries,
the two-bit seek-error return, mode/sync ordering, sector-23 signature and
disc-number validation, and sector-24/40 archive table/header reloads.
Debug transport retains both sets of three retail filenames and sizes.
Archive globals keep their existing low-address u32 representation; the
stack sector buffer is passed at full native pointer width. Pointer globals
are reloaded after callbacks. Disc comparison preserves the full unsigned
32-bit addition rather than truncating to a byte or invoking signed overflow.

New tests: pc_port/tests/disc_validation_test.c and
pc_port/tests/run_disc_validation_test.sh. They execute pinned menu.bin
(SHA256 82f84a24ac1fd1979754e1cc9c861405fb59ce95dd78db00a76f00c8f1bd177d)
through the MIPS adapter and compare return values, complete CD/file/sync
boundary traces, status bytes and callback-mutated archive globals with
the extracted production body. Initial RED reproduced the current stub's
missing trace (counts assertion). Coverage: 4,608 fixtures per mode across
debug/CD paths, eight disc arguments including 32-bit edges, signature and
disc mismatch, all four relevant seek-error bit combinations, command
success/failure, zero-to-three polling retries and pointer mutation.
O0/O2/UBSan pass in localhost/xenogears-dev-toolchain:krom-20260913.
All eight mutants reject: lid bit, ready result, seek error, header magic,
disc truncation, table size, missing sync and debug-disc selection.
Host O0/O2 passed; host UBSan could not link its missing libubsan, so the
sanitizer run used the already verified container without host installation.

Production build LINK OK: 48 function stubs / 581 data symbols (previously
49 / 581). Nearby title conversion regression passes 196,608 fixtures per
O0/O2/UBSan mode and all five controls; real-BIOS field font/atlas regression
passes all three modes (86 nonzero glyph pixels, GPU intercepted).
This is not full-suite, natural disc-change, real CD-provider, GPU, or
whole-game acceptance. Retail polling is intentionally unbounded; no fake
lid event, arbitrary timeout, or replacement success was introduced.
The surrounding caller still has raw menu offsets requiring separate
native-layout audit before natural UI acceptance. No commit or push.

### Disc-change caller 801C8694 native layout and control flow repaired

Follow-up inspection of asm/menu/matchings/main/misc/func_801C8694.s
found three native defects: raw +329 accesses do not address the expanded
SystemMenu.transitionEffectState; the branch delay slot at 801C86CC sets
s1=1 even when the transition is already finished (the old C incorrectly
skipped disc validation); and 801C8704 masks the input to a byte BEFORE
adding one, retaining target 256 for input FF (the old C truncated it to 0).
The native branch now follows these retail semantics and reloads g_Menu
after callbacks. Error paths retain 29 delay frames plus one trailing frame,
dialogue ordering, retry, success close and final cleanup. Non-native source
was preserved; its apparent discrepancy with the matching assembly is not
claimed repaired or re-matched by this native pass.

New disc_change_caller_test.c / run_disc_change_caller_test.sh execute the
same pinned menu.bin routine through the MIPS adapter and compare complete
callback traces and owners, query/validation/frame counts, transition bytes,
and guards across the remaining native menu storage. Initial RED failed the
trace count with transition state zero. All 41,472 fixtures pass at O0/O2/
UBSan in krom-20260913: all 256 low input bytes, three upper-bit patterns,
0..2 transition frames, 0..2 failed validations, query/validator success,
positive/negative error results and three callback owner-replacement modes.
All six controls reject: raw state offset, zero-state early exit, byte-sized
target, unmasked input, shortened delay and missing success close.
Production rebuild LINK OK, unchanged 48 function stubs / 581 data symbols.
Validator regression passes 4,608 fixtures per O0/O2/UBSan mode and its eight
negative controls. No full-suite, natural UI or whole-game acceptance claimed.

Next runtime gap is now localized by source inspection, not live observation:
pc_port/extern/PsyCross/src/psx/LIBCD.C CdControlB handles Setloc, Setfilter,
Setmode and Pause but not Nop (1), GetTN (13 hex) or SeekL (15 hex). Unknown
commands return zero and do not provide the status response used by the
retail validator. Native caller correctness does not establish a working
disc-change workflow. A faithful provider plus real image/lid lifecycle and
natural UI verification remain necessary; no fabricated status/event or
timeout success was added. No commit or push; full goal remains active.

### CD image initialization ownership repaired before command/lid work

Provider inspection found ParseCueSheet discarded OpenBinaryImageFile's
failure in both explicit-image and recognized-CUE paths, so CdInit returned
success after a missing image. Repeated initialization also overwrote the
current FILE pointer with a new fopen without closing the old one. These
are prerequisites to reliable availability/ownership, not disc-switching
semantics themselves.

New reproducible patch pc_port/patches/psycross_cd_image_open.patch is wired
into build_port.sh with marker _xeno_cd_image_open. OpenBinaryImageFile now
validates a local candidate before publishing a newly opened stream; closes
newly owned streams on seek/tell/size failures; reuses an existing stream on
reinitialization; rejects nonpositive sector sizes and lengths outside the
existing signed-int virtual-I/O range; and checks borrowed lengths before
forming end pointers. Recognized paths propagate the open result to CdInit.
Existing floor division for partial trailing sectors is preserved. This is
not an image-replacement API and does not claim thread-safe reinitialization.

cd_image_open_test.c / run_cd_image_open_test.sh extract the current provider
VFILE/OpenBinaryImageFile/ParseCueSheet/CdInit code and use real stdio with
seek/tell fault injection. Initial RED reproduced CdInit false success for
a missing file. O0/O2/UBSan/ASan all pass 19 synthetic fixtures (missing,
zero/short/valid/trailing-length files, repeat init, seek/tell failure,
invalid sector sizes, over-INT_MAX sparse file, borrowed memory and two
well-formed CUE cases). All four controls reject: false success, repeated
open, lost close and accepted short image. Open/close accounting balances.

With XENO_IMAGE_OPEN_TEST_DISC=/home/blizz/xg/disc/disc1.bin in the verified
krom-20260913 container, all four modes additionally pass the twentieth
fixture: 718738272 bytes / 305586 frames, first 32 bytes agree with direct
read. This is local image-opening evidence, not a fresh payload provenance
audit or gameplay proof. The CUE parser's pre-existing unchecked fread and
malformed-input handling remain unresolved; warning was not suppressed.

Patch forward replay against an isolated copy of vendored HEAD reproduces
the current LIBCD.C byte-for-byte; reverse applicability also passes.
Production build LINK OK, still 48 function stubs / 581 data symbols;
CD shutdown-order regression passes. No full-suite or natural runtime
acceptance. The missing Nop/GetTN/SeekL handlers, real quiesced swap/lid
lifecycle, full malformed-CUE validation and UI remain open. No fabricated
drive state, disc event or success response was added. No commit or push.

### Checked CUE parsing and stale-dependency build gate

The security-hardening pass treats CUE bytes as untrusted metadata rather
than an implicitly terminated string. New ordered patch
pc_port/patches/psycross_cd_cue_checked.patch follows cd_image_open in the
build driver. A fixed-size line reader rejects NUL and overlong input,
checks read/close errors, handles quoted filenames (including spaces),
CRLF, blank lines and REM records, and requires complete FILE/TRACK/INDEX
records before opening data. Metadata is restored if image opening fails.
No heap allocation or unchecked fread remains in ParseCueSheet.

The admitted layout is explicitly the reader's current representation:
one BINARY file, track 1 MODE2/2352, index 1 at 00:00:00. Extra tracks,
nonzero offsets, other modes and unknown records are rejected rather than
silently ignored. This is NOT full CUE or multi-track implementation.
FILE/INDEX context was checked against GNU ccd2cue format documentation:
https://www.gnu.org/software/ccd2cue/manual/html_node/FILE-_0028CUE-Command_0029.html
https://www.gnu.org/software/ccd2cue/manual/html_node/INDEX-_0028CUE-Command_0029.html
Search excerpts were available; direct page fetches timed out. The narrow
admission is a local backend limitation, not a claim that the CUE format
itself forbids the rejected layouts. Full topology remains open.

Expanded image-open suite passes 210 cases at O0/O2/UBSan/ASan and production
C++17: all truncation prefixes and injected read-failure positions of a
valid CUE, unsupported topology/modes, zero-byte and overlong content,
space-containing paths, CRLF, close failure, prior image ownership cases,
and the local 718738272-byte disc-one opening check. Invalid CUE cases
assert that no data-file open was attempted and metadata was unchanged.
Seven mutants reject (four prior plus missing index, ignored read error,
and wrong mode). Initial RED reproduced empty-CUE false success.

IMPORTANT BUILD EVIDENCE CORRECTION: the first full-driver invocation printed
LINK OK despite a C++ goto-crosses-initialization error in the new parser.
The driver uses set -uo pipefail (not errexit), ignored failed cmake --build,
and linked the old libpsycross.a. That invocation is NOT verification of
the new source. Missing diagnostic bytes in the final executable exposed
the stale artifact; direct dependency build reproduced the compiler error.
Declaration placement was corrected and a C++ test mode added. The driver
now explicitly exits when the PsyCross dependency build fails.
build_psycross_failure_test.sh reproduced the old fall-through, then passed
with a simulated CMake exit 73 proving no stale-link continuation.

Final fresh build LINK OK, 48 function stubs / 581 data symbols. Executable
now contains PsyX_CueLine/PsyX_CueToken and the new invalid-CUE diagnostic;
the LIBCD object was rebuilt. Final patch replay against the saved preceding
provider source is byte-identical; shutdown-order and scoped syntax/diff
checks pass. No full-suite, actual disc swap, natural UI, or whole-game
acceptance claimed. Nop/GetTN/SeekL and quiesced image/lid lifecycle remain
unimplemented. No commit or push; full port/decompilation goal stays active.

### Image availability now requires successful initialization

Provider trace found another false-ready case: PsyX_IsCDImageInit tested
only FILE/basePtr presence, so PsyX_CDFS_Init_Mem with an empty or invalid
borrowed buffer reported availability before CdInit and after its failure.
The new ordered psycross_cd_image_ready.patch adds a private initialization
gate (not a hardware status byte or lid flag). Configuration and entry to
CdInit clear it; only successful image-mode initialization and queue setup
set it. PsyX_IsCDImageInit also retains the storage-presence check.
Shutdown clears readiness after pause/join, without freeing borrowed data.
The patch is applied after the existing shutdown-join patch it depends on.

The expanded image test extracts the actual configuration, availability,
initialization and shutdown functions. Initial RED reproduced false ready
immediately after configuring borrowed memory. All 215 cases pass at O0,
O2, UBSan, ASan and C++17 with local disc1 opening included. Added checks
cover availability before/after valid and invalid memory initialization,
file and memory reconfiguration, successful-to-failed reinitialization,
no-image mode, pause/join-before-invalidation, repeated shutdown and intact
borrowed storage. Shutdown transport calls are intercepted; this is not a
new thread-safety or runtime shutdown certification. All ten controls reject,
including new raw-availability, stale-init-ready and stale-shutdown-ready.

Forward patch replay reproduces current LIBCD.C exactly, reverse check
passes, and the guarded production build links (48 function stubs / 581 data
symbols). The final executable contains g_xeno_cdImageReady and its consumer;
shutdown-order and stale-dependency build regressions plus syntax/diff checks
pass. Configuration is still an initialization API, NOT a concurrent hot-swap
API: changing image ownership with a running spooler remains unresolved.
Nop/GetTN/SeekL responses, real lid/image replacement, natural disc-change
UI, full-suite and whole-game acceptance remain open. No fabricated event,
forced status, commit or push; the full port/decompilation goal stays active.

### Synchronous CD reads no longer publish incomplete sectors

Command tracing found two lower-level data integrity defects: memory-backed
vfread returned the requested element count after a partial copy, and both
CdReadSync payload branches ignored vfread's result and copied an incomplete
stack sector into game memory. This could report successful reads at EOF.
New psycross_cd_complete_sector.patch follows image_ready in build_port.sh.
Virtual fread now returns complete elements, with overflow/offset checks
before memory-pointer arithmetic. Partial final elements are consumed/copied
into the caller's temporary buffer, as in stdio. CdReadSync copies the data
or audio-layout payload only after receiving a full 2352-byte sector. A
short read retires the failed queue entry and returns -1 without advancing
its destination pointer or decrementing its remaining count. Previously
completed sectors remain intact; no replacement sector/status byte is made.

Primary return-contract authority: PsyQ Run-Time Library Reference 4.7,
CdReadSync, printed page 10-36 / PDF page 704:
https://psx.arthus.net/sdk/Psy-Q/DOCS/LibRef47.pdf#page=704
It specifies remaining sectors, zero on completion and -1 on error.
PSX-SPX additionally confirms Setloc only chooses a target, while SeekL
performs a seek; the existing synchronous Setloc shortcut is also relied
on by ArchivePortCdSeekOrPause. Thus adding SeekL faithfully requires
reconciling these consumers, not just returning success from a new case.

cd_sector_read_test.c / run_cd_sector_read_test.sh extract actual VFILE and
CdRead/CdReadSync code plus the real sector-layout header. Initial RED
reproduced false completion at zero bytes. All 31,498 fixtures pass at O0,
O2, UBSan, ASan and C++17: every truncation length across three raw sectors,
both payload layouts, polling/blocking modes, memory images, selected real
stdio lengths, complete/partial element counts, zero and overflow requests.
All output bytes/guards, queue retirement, destination pointer and remaining
count are checked. Four mutants reject: false element count, ignored short
data read, ignored short audio-layout read and false-success return.
These are synthesized boundary fixtures, not retail MIPS execution or XA
audio playback acceptance. No provider retry/timing contract was added.

Fresh guarded production LINK OK, 48 function stubs / 581 data symbols.
Final CdReadSync disassembly checks vfread against one before both copies;
LIBCD object was rebuilt. Forward patch replay is byte-identical. Image
initialization suite still passes 215 cases in all five modes plus ten
controls; shutdown-order and stale-dependency build tests pass.
Streaming _eCdSpoolerFunc still ignores vfread return, some archive readers
ignore CdReadSync errors, status result arrays and idle-poll semantics remain
incomplete, and invalid queue counts are not covered by this repair.
Nop/GetTN/SeekL, faithful seek sequencing, quiesced image/lid replacement,
natural UI, full-suite and whole-game acceptance remain open. No commit or
push; full port/decompilation goal remains active.

### Streaming pause no longer consumes/delivers an extra sector

The next worker audit found _eCdSpoolerFunc consumed the -1 pause command,
cleared its mailbox and set g_cdReadDoneFlag, but fell through to vfread,
pacing, XA delivery and ready/data callbacks. New ordered patch
psycross_cd_pause_boundary.patch releases the mutex and returns zero at
that boundary. Sector buffer, image cursor and data-consumed marker remain
unchanged when the worker observes pause. This does not cancel a callback
already in flight before pause is observed or prove all concurrent ordering.

New cd_stream_pause_test.c / run_cd_stream_pause_test.sh compile the actual
worker body and VFILE helpers with intercepted mutex/pacing/XA/callback
boundaries. Initial RED reproduced cursor and sector-buffer mutation after
pause. All 64 combinations pass at O0/O2/UBSan/ASan/C++17: paused/normal,
ready/data callback presence, consumption, XA acceptance and previous data
marker. Assertions cover balanced mutex operations, mailbox/stop state,
unaltered paused storage and absence of pacing/callbacks on pause. Normal
delivery remains tested as the control. Three mutants reject: fallthrough,
locked early return and missing stop flag.

Patch replay is byte-identical; fresh production build links with unchanged
48 function stubs / 581 data symbols. Final worker disassembly shows stop
flag store, SDL_UnlockMutex and return before the sector read. Shutdown-order
and stale-dependency build gates pass. Full-TU XA decode/resample regression
and disc-one two-sector oracle/production PCM comparison pass as well.
No new natural movie/end-of-gameplay or thread-sanitizer acceptance claimed.
Streaming EOF/seek-error propagation remains unresolved: the normal worker
still ignores vfread/vfseek errors. This pause repair is not that error path,
nor Nop/GetTN/SeekL, complete image replacement or whole-game completion.
No commit or push; full port/decompilation goal remains active.

### Bounded virtual seek/tell and streaming recovery trace

Follow-up read-only consumer trace: movie_player.c StCdInterrupt ignores its
result pointer, records D_801E8908=3 for status&4 and returns. The native
movie pump references this variable only in MoviePlayerDiag. Thus emitting
an error callback alone is not proof of movie recovery; retail recovery
and native control flow still need reconciliation. No synthetic physical
drive status or error callback was introduced in this step.

The I/O trace also found vfseek directly formed base+negative/out-of-range
pointers and returned success for unknown origins. New ordered patch
psycross_cd_bounded_seek.patch validates borrowed storage and computes
positions in int64 before constructing a pointer. The closed interval
[0,size] is accepted, including EOF; invalid origin/position leaves the
cursor unchanged and returns -1. Borrowed images cannot grow, unlike stdio
files; the FILE-backed branch retains fseek semantics. vftell validates
memory offsets and reports -1 for file positions outside its signed-int
return range instead of truncating. Missing storage and invalid cursors are
rejected without pointer subtraction outside the backing object.

cd_virtual_seek_test.c / run_cd_virtual_seek_test.sh initial RED reproduced
false seek success. All 1,989,180 bounded memory cases pass in O0/O2/UBSan/
ASan/C++17, plus actual file seeks, over-INT_MAX tell, null/oversized storage
and invalid cursors. Coverage spans all positions in sizes 0..256, twelve
offsets including INT_MIN/MAX and five origins. Four mutants reject:
rejected EOF, lost current-origin position, accepted invalid origin and
truncated file tell. Sector/pause harnesses now include limits.h to match
the real provider's includes; their behavioral assertions were unchanged.

Fresh production LINK OK (48 function stubs / 581 data symbols); final
vfseek disassembly contains the failure guards. Forward patch replay is
byte-identical and reverse check passes. Pause 64 / sector 31,498 / image
215 regressions pass in all five modes with all controls, and the build
dependency-failure guard passes. No full-suite or natural runtime acceptance.
Callers still ignore some vfseek failures, and sector*size can overflow
before reaching the helper; these are NOT repaired by the bounded helper.
Streaming EOF/error propagation, retail/native recovery, CD commands and
real disc switching remain open. No commit or push; full goal remains active.

### CD runtime rebaseline: failed title schedule and Continue teardown

Resumed on branch experiment/worldmap-open-gates-20260823, HEAD f67fe692;
preserved the dirty checkout. Existing run_title_newgame_smoke.sh used
offscreen SDL, real disc1 and raw pad inputs only, with Lahan still required.
Evidence: scratchpad/cd-runtime-rebaseline.pLICEG/{run.log,schedule.txt,summary.txt}
and 20 captures (BMP payloads despite .png suffix; not visually accepted).
Binary SHA256: 01df9244e3eb2709ba8021021d21567c762340518fc4caed1ba6d3a4bac84cb4.
Container: localhost/xenogears-dev-toolchain:krom-20260913.

Observed opening movie skip, FieldLoad 490, FE60 STR first-frame delivery,
Circle skip and CD worker pause/exit. Audio device could not open: no audio
acceptance. No exact FieldLoad 4 or 2, and no New Game confirmation.
The existing timing schedule missed its intended menu: Up at raw frame
2800 preceded the second title-loop entry (log line 153, after frame 2902).
Circle at frame 3400 reached the Continue controller stub func_801D9F98.

A read-only GDB attach/detach observed the main-thread stack:
GameHandleError(131) <- MainLoop(131) <- HeapFree(NULL) <- func_801E5B3C
<- func_801D9E3C <- func_801E3088(7) <- func_801C531C(7)
<- func_801C58EC <- func_801C62A8 <- MenuExecute/MenuMain <- FieldMain.
The stub returns without the retail controller's func_801E5ACC allocation;
Continue cleanup then frees an uninitialized work pointer. Source inspection
confirms case 8 dispatch and cleanup and the error handler's infinite loop.
No game state was injected or heap error suppressed. Host GDB warned about
PID namespaces/libthread_db; the reported main-thread stack matched source.

Stopped only the owned test game PID 2109152 with SIGKILL after diagnosing
the permanent error loop, before its 600-second deadline. runtime_rc=137 is
the diagnostic stop, not a spontaneous crash or timeout. Wrapper exited 1,
overall=FAIL. The wrapper nevertheless falsely printed PASS for map 4:
its prefix search matched field=490 at line 47 with newgame=0. Its fatal
marker check also missed the silent heap-error loop and does not gate rc.
Do not use either subcheck as acceptance evidence; fix these oracle defects
with negative fixtures before trusting a future reported PASS.

Next: validate the smoke verdict itself and rerun with raw controller timing
after the observed title opening, preserving New Game/prologue/Lahan gates.
Continue's unimplemented controller/cleanup remains a separate real defect;
do not repair it with a null-free bypass or invent a successful card screen.
This run does not establish a CD regression or end-to-end CD acceptance.
No production code changes, commit or push in this runtime diagnosis slice.

### Title smoke verdict repaired; retimed retry still fails

run_title_newgame_smoke.sh now matches complete numeric field tokens and
does not restart a dependent search when its prerequisite is absent. Normal
exit (0) or the deliberate observation deadline (124) are permitted only
alongside the required route checks; other statuses fail. Reached [stub]
markers fail. Cross control searches the whole log for menu entry, without
depending on a Circle-skip marker that should not exist in that control.
This is bounded log evidence, not a detector for every possible silent hang.

New title_smoke_verdict_test.py executes the actual shell verdict under
bash -euc, using disposable synthetic logs and requiring a final summary.
Initial RED: 17 false results with the old verdict. Nine methods / 41 log
fixtures now pass: complete routes, every missing link, numeric aliases,
wrong ordering, real-format trailing field metadata, error statuses, reached
stubs and Cross controls. Testing with errexit also caught and fixed an
early-abort path in the new missing-prerequisite guard. bash -n and scoped
git diff --check pass. No game rebuild or full game test-suite claim.

Retried the unchanged SHA-pinned binary in xeno-cd-runtime-retimed with
UP_FRAME=3200, CONFIRM_FRAME=3300, PROLOGUE_FRAME=3800; other input defaults
unchanged. Evidence: scratchpad/cd-runtime-retimed.oB8pSn. The retry showed
that merely delaying Up/confirm is insufficient: frame 2400 skips FE60,
frame 3300 opens the second title menu, and Up at 3200 is therefore early.
The frame-3800 Circle enters Continue's func_801D9F98 stub. GDB again
observed HeapFree(NULL) -> MainLoop(131) -> GameHandleError's infinite loop
through func_801E5B3C/func_801D9E3C/func_801E3088(7)/func_801C531C(7).
After read-only attach/detach, stopped only owned PID 2123656 with SIGKILL.
Session 81433 exited 1; container removed. runtime_rc=137 is diagnostic stop.
Corrected summary: newgame=0, field4=0, field2=0, overall=FAIL; 22 captures.
The stub and abnormal exit now fail explicitly; no false map-4 subcheck.

Next runtime input sequence needs a separate post-attract menu-open pulse:
preserve the observed early Circle at 1600, skip attract at 2400, open title
at 2900, then Up at 3200 and confirm at 3300 before prologue pulses. This
sequence is inferred from two traces, NOT yet tested, and the existing
wrapper currently lacks a third pre-navigation Circle slot. Add/test that
input slot or use an equivalent raw-pad-only driver; do not inject menu or
map state. Continue implementation and natural New Game/Lahan acceptance
remain unresolved. No production code edits, commit or push in this slice.

### Three-pulse boot route reaches Lahan (automated runtime only)

Added optional XENO_TITLE_SMOKE_PRE_ATTRACT_FRAME to the smoke wrapper.
It inserts one raw confirm-button pulse before ATTRACT_FRAME, including
the Cross substitution in control mode. The step count includes this pulse
and still stays at 4095 within the native parser's 4096-entry limit. Empty
leaves the historical schedule unchanged. No game code or binary changed.
The schedule test reproduced two missing-pulse failures before implementation;
now ten methods pass (41 verdict fixtures and four schedule combinations).
Checks cover exact prefix, strict frame order, final neutral state, pulse
start and buffer count. Shell syntax and scoped diff whitespace checks pass.

Runtime evidence: scratchpad/cd-runtime-three-pulse.6zrcqg, same binary SHA256
01df9244e3eb2709ba8021021d21567c762340518fc4caed1ba6d3a4bac84cb4, HEAD f67fe692,
localhost/xenogears-dev-toolchain:krom-20260913, SDL offscreen.
Explicit smoke settings: PRE_ATTRACT_FRAME=1600, ATTRACT_FRAME=2400,
OPEN_FRAME=2900, UP_FRAME=3200, CONFIRM_FRAME=3300, PROLOGUE_FRAME=3800,
SECONDS=180; default REQUIRE_LAHAN=1 retained. No injected map/menu state.
This three-Circle pre-navigation sequence now has actual runtime evidence.

Session 68979 exited 0; container xeno-cd-runtime-three-pulse removed normally
after the observation deadline (runtime_rc=124, not a crash or forced kill).
Corrected verifier overall=PASS. Independently checked exact FieldLoad 490
at log line 47, New Game choice=2 at 160, FieldLoad 4 at 248 and FieldLoad 2
at 3730. Three movie stream pause/exits occurred along the route; no reached
stub or fatal marker. 52 offscreen captures were written, not visually
accepted. Sound device failed to open; XA overrun logs do not establish audio
playback. This proves the bounded controller-driven route, not complete CD
semantics, disc switching, visual parity, human walking or whole-game coverage.

Next: run the same schedule with CONTROL=cross as a live negative control;
then inspect actual captures/runtime presentation as a distinct acceptance
level. Historical default timing has NOT been changed to this new profile.
Continue still has the independently reproduced stub/teardown defect.
No commit or push; full port/decompilation goal remains active.

### Live Cross control and capture inspection

Same branch/HEAD and binary SHA reverified; no other test container running.
Ran the three-pulse profile above with CONTROL=cross and SECONDS=180 in
container xeno-cd-runtime-cross, session 41331. Evidence directory:
scratchpad/cd-runtime-cross.u6acEw. Session exited 0, runtime_rc=124 at the
normal observation deadline, container removed. Control overall=PASS:
exact field490 line 47, FE60 entry line 108, no Circle skip, FE57, title
request/loop, New Game confirmation, field4 or field2. 81 captures; no
reached stub/fatal marker. Shutdown logged CdlPause and CD thread work done.
This is a live negative control for the route, not just a synthetic log test.

Inspected original positive-run captures field-frame-002160.png and
field-frame-006240.png from cd-runtime-three-pulse.6zrcqg. These are BMP
payloads; decoded to temporary PNGs solely for viewing, without modifying
the originals. They visibly show the title menu (Continue selected) and
the burning-village prologue, respectively. No retail image comparison
was performed, so neither is a visual-parity PASS.

Important limit on the preceding route PASS: its last saved capture is
frame 6240 at log line 3694, BEFORE FieldLoad begin field=2 at line 3730.
Thus field-2 load entry is observed, but saved Lahan presentation is absent.
Do not promote the script's load-marker gate into visible gameplay proof.
Next: extend the positive observation window and inspect post-load Lahan
captures through the natural route, without map injection or pose changes.
Audio remains unavailable in this container. No production code changes,
commit or push; full-game completion remains unproven and goal stays active.

### Extended natural route: visible opening scene, battle entry unresolved

Ran the same three-pulse positive profile with SECONDS=240 (all other
settings unchanged) against reverified binary SHA
01df9244e3eb2709ba8021021d21567c762340518fc4caed1ba6d3a4bac84cb4,
branch experiment/worldmap-open-gates-20260823, HEAD f67fe692. Evidence:
scratchpad/lahan-natural-visible.A56D5m. Container xeno-lahan-natural-visible,
session 74043 exited 0 at normal deadline, runtime_rc=124; container removed.
Wrapper route PASS: field490 line 46, New Game line 162, exact field4 line
249 and field2 line 3730; 76 captures, no reached stub/fatal marker.

Viewed captures 7440 and 9120 after decoding BMP payloads into view-007440.png
and view-009120.png in that evidence directory (original captures unchanged).
Both visibly show Gears, a burning village and Fei's portrait/dialogue box.
This establishes actual post-load scene output through the controller-driven
route, not retail visual parity, human walking, battle completion or audio.

The longer observation also reaches the next real boundary: actor-0 FE84
at log line 4667, battle adapter ready (608 functions, 189 shared-data,
256 host ranges), then retail battle.bin entry 0x80070f40 at line 4669 and
FieldMain teardown. Captures continue but the two inspected later images
still show the same dialogue scene. Do not treat continued captures or the
route PASS as proof the battle transition succeeded. No live stack sample
was taken before the deadline; whether this is an input wait, slow progress
or a battle/presentation defect is UNRESOLVED. Input pulses end neutral at
raw frame 11962 just before FE84, an important condition for reproducing it.
Next: reproduce that natural battle entry and inspect guest/host execution
and presentation before any repair; do not force battle flags or patch the
heap-error path. No production code edits, commit or push in this slice.

### Natural battle live execution samples

Reproduced the unchanged three-pulse route with SECONDS=300. Same reverified
binary SHA and HEAD f67fe692. Evidence: scratchpad/battle-natural-diag.NyuAGL,
container xeno-battle-natural-diag, session 54232. Session exited 0 at deadline
(runtime_rc=124), route overall=PASS, 105 captures. The route verdict still
does NOT assert battle completion. No game state injected or code changed.

Live host GDB inspection of owned PID 2151062 after retail battle entry:
main thread in PsyX_WaitForTimestep -> VSync -> Vsync -> runtime_bridge_call
-> runtime_bridge -> PcPortMipsRun -> func_80070F40 -> MainLoop. Guest PC
0x8004b54c (Vsync), RA 0x800bea10; steps 30,507,422 then 57,893,926.
Retail asm mainc122/func_800BE790.s confirms jal Vsync at 0x800bea08, followed
by PutDispEnv, PutDrawEnv and DrawOTag, returning to 0x800bea48.

A temporary breakpoint at bridge_draw_otag was reached: GPUDisabledState=0,
guest a0=0x800ccafc, RA=0x800bea48, steps=96,376,061. Later sample advanced
to 150,367,576 instructions. At a separate sampled VSync, the OT root word
at guest 0x800ccafc was 0x0072939c. The native RAM link 0x72939c contained
tag 0x09729374 and first payload 0x2d000000, followed by nonzero geometry/
texture words. dma_packet_memory explicitly accepts this host-RAM domain.
This is evidence of a nonempty submitted chain, NOT a full-chain audit or
proof those packets render correctly. GDB only read state and used a temporary
breakpoint; detached after each sample. Host debugger warned about differing
PID namespaces/libthread_db, but main-thread frames matched source/retail RA.

These observations rule out the previously seen GameHandleError spin as the
explanation for this run and show active battle frame-loop execution. They
do not establish whether the scene is intentionally waiting for input, whether
the battle phase is advancing, or whether GPU submission/presentation is
correct. Next: correlate retail battle phase and actual OT packet contents
with post-entry captures (existing XENO_BATTLE_OT_DIAG may help), preserving
the same natural route. No forced flags, heap bypass, commit or push.

### Extended raw input advances battle into field 14

The prior schedule stopped at raw frame 11962 near battle entry. Added
XENO_TITLE_SMOKE_PROLOGUE_INTERVAL to the test wrapper only: supported
4/8/16/32-frame spacing, default 4 unchanged, two-frame press duration,
4095-entry limit and final neutral retained. Other values reject with exit 2.
New tests reproduced six failures before the change; all eleven methods now
pass (previous 45 fixtures plus seven interval cases). bash -n and scoped
git diff --check pass. No production rebuild or gameplay edits.

Ran interval=8, same three-pulse prefix, SECONDS=300 with existing bounded
XENO_BATTLE_OT_DIAG=1:3 and SAMPLES=2. Evidence:
scratchpad/battle-input-extended.Gl1bFd; container xeno-battle-input-extended,
session 78157, same SHA-pinned binary/HEAD f67fe692. Normal deadline rc=124,
wrapper exit 0, route PASS, 91 captures. No reached stubs/fatal markers.
Buttons were still pulsing at raw frame 17930 near shutdown; they no longer
ended immediately before the battle. All inputs used the raw controller seam.

Battle entered at log line 2663. First three submitted OTs each had 560
packets, all command byte 0x24; two sampled packet payloads per frame are
recorded. This is initial-transition packet evidence, not all-battle coverage.
At line 3309 retail battle returned after 84,722,064 instructions; field14
loaded at 3318. Live GDB samples subsequently found FieldMain/func_8007554C
and g_ActiveBattleRuntime=NULL, confirming the battle runtime had exited.
An initial attempted bridge_cpu read failed because that pointer was already
NULL after return, not because the game crashed. No registers/state changed.

Viewed captures 8760 and 10920 via BMP-to-PNG decoding in the evidence folder,
leaving originals unchanged. 8760 showed the Gear scene without the earlier
dialogue box; 10920 visibly shows the room with Fei facing his easel, furniture
and wall paintings. Thus the extended natural route visibly progresses from
the opening sequence through the retail battle return into field14. The earlier
stationary dialogue scene alone was insufficient to diagnose a render defect.
This is observed route progression, not retail visual parity, general combat
acceptance, verified movement/collision or human gameplay acceptance.

Next: exercise natural field14 movement/interactions and subsequent campaign
progression; retain the unimplemented Continue controller and other whole-game
gaps rather than declaring completion. Audio remains unavailable in this test
container. No commit or push; full port/decompilation goal remains active.

### Field14 input probe: movement not yet exercised after battle return

Reverified same binary SHA/HEAD and no live test container. Ran the binary
directly for 330 seconds using the existing extended schedule prefix through
raw frame 14002, then Down15000/release15240, Left15600/release15840,
Circle16000/release16002. This uses only the existing raw-pad seam; no game
code changes. Evidence: scratchpad/field14-natural-input.shQil2/run.log and
captures (EVERY=30, FROM=8500); session26613, xeno-field14-natural-input.
Normal timeout rc=124, container removed. This direct diagnostic did not run
the route wrapper/verdict and must not be labeled a wrapper PASS.

The probe disproved the intended timing assumption: Down/Left were delivered
while battle remained active. The final Circle press was followed by retail
battle return after 140,834,388 instructions (line3179), then actual field14
load (3186). No directional input followed that load, so field movement and
collision are NOT_TESTED by this run. This supports an input-wait explanation
for the earlier stationary dialogue, not a renderer repair requirement.

Important live-state caution: g_GameSceneMapNum was already14 BEFORE battle
returned. A field-actor read then produced stale/freed-overlay data; discard
that sample. Only after the return/load markers and g_ActiveBattleRuntime=NULL
was the field sample valid: player index1, transform translation(115,0,-455),
ActorData fixed-point position(7536640,0,-29818880), flags0, status576.
The map-number global alone is not a current-field readiness gate.

Next input test: retain Circle pulses through16002 and place movement later
(e.g. Down18000/release18240, Left18600/release18840), checking actual battle
return and field14 load before using the samples. Prefer phase-observed input
when available; do not inject an actor position or force a control flag.
No production edits, commit or push; whole-game goal stays active.

### Late field14 directions encounter an open scripted dialogue

Reverified same binary SHA/HEAD and ran the natural route for350 seconds.
Input: extended interval8 prefix through16002, Down18000/release18240,
Left18600/release18840, Circle19000/release19002. Evidence:
scratchpad/field14-late-movement.AlRKOE; session93452/container
xeno-field14-late-movement. Direct runtime rc124 at normal deadline; container
removed. No wrapper verdict or game-code changes in this probe.

Battle returned after83,147,688 instructions (log3221), field14 load3230.
Before directions, live GDB confirmed active battle=NULL, map14, player1 at
(115,0,-455). After directions/Circle, the same translation remained. Post
sample: ActorData position(7536640,0,-29818880), move/moveModified zero,
scriptFlags0x24430, flags0x04010400, IP406, script-slot0 isInUse0,
canInteract255, D_800ADB64=255. These are observations, not a decoded control
contract; no values were altered. FieldActor.flags itself was0.

Crucial capture inspection corrected the initial no-motion interpretation:
frame11040 (during Down) and11340 (during Left) BOTH show Fei's portrait and
the scripted dialogue "Alright... Now for a short break." still open.
Decoded copies down.png/left.png reside beside the original BMP captures.
Thus these directions do not establish a movement bug or controllable-field
acceptance. Map load and valid actor allocation are insufficient readiness
gates when a textbox/script still owns interaction. The Circle at19000 comes
after both directional holds. Next: dismiss/observe completion of that
dialogue through normal input, then exercise movement; never force actor
flags to bypass it. No commit/push, full game goal remains active.

### Field14 movement observed after normal dialogue closure

First inspected late-movement capture12420: the Circle19000 press had closed
the scripted dialogue and advanced actor0 to IP141. Reused the unchanged
natural opening prefix through16002, then Circle19000/release19002,
Down20000/release20240, Left20600/release20840, Circle21000/release21002.
Direct offscreen run370 seconds, captures EVERY30 FROM11000. Evidence:
scratchpad/field14-control-ready.veBaWR, session88732/container
xeno-field14-control-ready. Normal deadline exit124; no wrapper verdict.
Same branch/HEAD and binary as preceding probes; no production code changes.

Battle returned after84,680,985 instructions (log3223), actual field14 load
3231. Before dialogue confirmation, live GDB showed active battle=NULL,
map14 and translation(115,0,-455). A later live sample after the Down interval
showed map14, translation(90,0,-428). Captures12000 and12150 were inspected:
dialogue closed and Fei's position changed near the easel. Final capture13080
after Left visibly shows Fei beside the easel. Decoded viewing copies are
before-down.png, after-down.png and final.png beside the untouched originals.
The attempted final GDB sample arrived after normal process exit and yielded
no data; do not invent a final coordinate. Logs confirm all direction/release
and final Circle events were delivered.

This is bounded natural field14 movement evidence after script/dialogue
completion. Earlier stationary probes occurred while the dialogue was open;
no forced control-bit or movement-code repair was needed. It does not prove
all collisions, room exits, NPC interactions, retail visual parity, human
walking acceptance or whole-campaign completion. Next: continue from this
controller sequence toward a natural room exit and subsequent interactions,
with phase/capture evidence; keep unresolved Continue and other game systems
in scope. No commit/push; audio and full-game acceptance remain open.

### Doorway probe launch (terminal result recorded below)

Current owned run: session43909, container xeno-field14-doorway, host game
PID2200920, timeout PID2200919. Confirmed live in prologue around elapsed69s.
Evidence directory scratchpad/field14-doorway.OxzM6M. Same reverified binary
SHA/HEAD f67fe692. Direct run deadline480 seconds, captures EVERY30 FROM12000.
Input retains the successful prefix through16002 and known field14 sequence
(Circle19000, Down20000..20240, Left20600..20840, Circle21000), then adds
Up+Left0x9000 at22000, neutral22240. No forced map/pose/control state.
Goal of this probe: approach the visible doorway, stop input and inspect actual
field transition and scene output. Its result is PENDING; continue polling
session43909 or the validated container/process, not another startup run.

Read-only archaeology found older scratchpad/boot-to-blackmoon-20260909
navigation tools: walk2.py and run_to_map15.sh use F8 checkpoint loading and
the former includes a non-retail Escape battle fallback. Do not reuse these
as natural-route evidence. That folder's field14-decode.txt, derived from an
older field14-data/scripts.bin, labels script17 routine1 at0x750 as trigger0
check and0x754 as CHANGE_FIELD map13 entry1. This is an unrefreshed navigation
hint, NOT current runtime proof; verify the actual trigger/transition before
claiming a room exit. No production code change, commit or push this turn.

### Doorway probe result: exit region not reached

Resumed existing session43909/container xeno-field14-doorway, not a new run.
Battle returned after84,506,709 instructions; actual field14 load at log3219.
Live GDB confirmed battle runtimeNULL, map14 and initial(115,0,-455).
Current trigger0 corners read directly from g_pFieldTriggerZones:
(308,0,-52),(363,0,-52),(363,0,0),(308,0,0). This verifies the old snapshot's
geometry against this run. FieldScriptCheckTriggerZone2D tests the player's
X/Z against all four edges via NormalClip; no trigger state was forced.

Up+Left0x9000 was delivered at raw22000, neutral22240. Afterward field14
remained active; actor translation(90,0,-428), walkmeshTriIds[0]=119.
Capture13230 (decoded after-approach.png) shows Fei beside the crate/easel,
well away from the doorway region. No room exit occurred, but this is NOT
evidence that the exit trigger is broken: the player never reached its region.
Furniture/steering versus any movement defect remains to be distinguished
with short observed input segments; do not bypass collision or teleport.

After capturing the neutral post-input result, sent SIGINT only to owned
gamePID2200920. It exited cleanly; direct wrapper returned0 and container
was removed. This was an operator-ended diagnostic, NOT the480s timeout or
a success verdict. Session43909 is now TERMINAL; do not resume/restart based
on the earlier LIVE launch note. All evidence remains in
scratchpad/field14-doorway.OxzM6M. No production edits, commit or push.

Next: use observed short steering around the furniture, preferably ordinary
keyboard/controller input in a persistent test window so controls can follow
scene state without repeated full boot runs. Existing old X11 navigation
tooling demonstrates input delivery but its quickload/Escape bypass workflow
must not be carried into a natural-route proof. Full-game goal remains active.

### Persistent private X11 run LIVE for observed steering

Computer-use capability check succeeded but this desktop provider has neither
screenshots nor focus control; selected a private X11 test display for ordinary
keydown/keyup plus game captures. Xvfb session30414 owns :91, TCP disabled,
cookie auth at scratchpad/field14-interactive.DXEBdg/Xauthority. Do not print
that cookie. Host tools use DISPLAY=:91 and XAUTHORITY with that absolute path.
No user desktop window was operated. Initial container session65438 failed
SDL init with X11 authorization (exit1), evidence in field14-interactive.DXEBdg.
Matching container hostname bazzite to the local auth entry resolved it;
no authentication disabling or host package changes.

CURRENT LIVE game session18890, container xeno-field14-interactive, host
PID2216938 (timeout2216937), deadline1800 seconds. Evidence directory:
scratchpad/field14-interactive-retry.UT1H0W. Same SHA-pinned binary/HEAD.
Window2097158 on :91, title "Xenogears (PC port)", container PID4, verified
focused. Latest observation reached field490 with no SDL startup error.
Resume this process rather than a new full boot. Xvfb must remain available
until the owned game finishes; clean up only these owned sessions afterward.

Natural boot schedule: interval8 prefix through16002, Circle19000/release19002,
then neutral. No movement yet; no quickload, kernel shortcut, or Escape bailout.
Capture EVERY60 FROM11000. After schedule ends, PcPort_PadTestInputInject only
ANDs the neutral mask into the already-populated pad buffer; ordinary keyboard
input remains available. Current PsyX_main.cpp mappings: arrows=dpad, V=Circle,
C=Cross, Z=Triangle, X=Square. Old scratch navigation scripts used different
assumptions, so do not copy their keys blindly. XENO_FIELD_POS_DIAG was absent
from current source and is not enabled in the working retry; use captures and
read-only live actor samples for position until a supported diagnostic is found.
CORRECTION from subsequent live verification: those PsyCross default face keys
are overridden after PsyX_Initialise in pc_port/src/port_main.c:1234.
ACTUAL runtime mapping: Z=Circle/confirm, V=Triangle/menu, C=Cross, X=Square.
Read-only g_cfg_keyboardMapping confirms kc_circle=29 and kc_triangle=25.

Next: verify actual battle return, field14 load, and dialogue closure, then
use short key holds in this private window, release every key, and inspect
each resulting position/image before steering again. Window input success is
not movement proof. No production edits, commit or push; whole-game goal active.

### Private X11 natural route: field14 stair exit to field13 OBSERVED

Continued the SAME live session18890/PID2216938 on :91, window2097158.
Evidence remains scratchpad/field14-interactive-retry.UT1H0W. Natural route
log markers: field490 line44, field4 line200, field2 line1994, retail battle
returned after84878699 instructions line3227, field14 line3235, field13
line4081. No quickload, state injection, collision bypass, or Escape bailout.

Ordinary keyboard steering cleared the easel and bed. Initial approaches
stopped at furniture and the narrow doorway; these were NOT proven defects.
Read-only live walkmesh inspection identified entrance triangle6 vertices
(131,0,-196),(147,0,-163),(147,0,-196), only16 units wide at the threshold.
Fine alignment from(145,0,-198), Down+Left0.04s then Up+Left0.12s, reached
(135,0,-173), triangle5. Subsequent Up+Left0.4s reached(131,0,-109), then
0.5s reached(128,0,-31). Up+Right0.9s reached(251,-29,-31), and another0.65s
entered the natural stair exit. All held keys explicitly released.

Confirmed field13 load, g_ActiveBattleRuntime=NULL, playerindex1 at(-37,0,0).
field13-16.png visibly shows the populated wooden interior; exit-15.png is
the earlier transition image, NOT the settled field13 proof. Other captures
stairs-09..13 and hall-14 document the approach. This proves this bounded
natural route and room transition, not full combat, visual parity, audio,
human desktop acceptance, or the whole-game port/decompilation goal.

Game still LIVE at18m29s elapsed against its30-minute timeout when sampled.
Keep using it rather than rebooting. No production changes, commit, or push.

### Private X11 run TERMINAL: field13 conversation complete; outside pending

The original1800s deadline subsequently ended session18890 with rc124.
Host gamePID2216938 and timeoutPID2216937 are gone; run.log ends LOG CLOSED.
This timeout is NOT a success verdict and this session must not be resumed.

Field13 normal Right0.5s moved player(-37,0,0) to(15,0,-54).
Up+Right0.55s then Up+Left0.3s reached(91,0,5). Another Up+Right0.25s
naturally started Dan's scripted conversation and moved Fei to(46,0,5).
Actual confirm key is Z, as corrected above; V attempts did not advance it.
Single Z presses advanced observed Dan/Fei/Timothy dialogue with portraits
and scripted actor movements. Dan left naturally, and Timothy's final text
closed in dialogue-40.png. No choice menu appeared in these observed pages.

After dialogue closure, Up+Right0.5s then0.4s moved Fei through the open door
to(178,0,-3), still field13. outside-43.png is the last inspected capture.
Another0.6s hold was sent near the deadline, but outside-44.png was NOT
captured: the window had closed. Final log has NO field15 or other next-field
load marker and NO reached [stub] marker. Do not claim an outside transition
or a broken exit from this bounded observation. Trigger payload was48bytes:
zone0 X[-169,-114] Z[-26,26], zone1 X[103,143] Z[-35,34]; its script semantics
and the actual downstream exit condition still need inspection.

Evidence: field13-move-17.png, field13-door-18.png, nextfield-19.png shows
Dan's opening (despite its filename), dialogue-20..38, dialogue-39-retry.png,
dialogue-40.png, outside-41..43.png. dialogue-39.png was not written; immediate
retry succeeded while game was live. The later outside-44 screenshot command
waited after window loss and was interrupted as an owned diagnostic process.
No production changes, commit, push, quickload, or game-state override.
Next: inspect field13 retail exit script, then reproduce with normal input
and a sufficiently long private session. Whole-game goal remains ACTIVE.

### Field13 outside exit authority: interact with door; destination field1/entry7

Read-only follow-up, same HEAD f67fe692; no live xeno-port or :91 remained.
Re-read local disc/disc1.bin field archive entry0xD2 (0xB8+13*2): sector122839,
44804bytes. Used existing DiscArchive/lzss_decompress from
tools/scripts/psx/scan_field_anim_opcodes.py. Map header script section declares
2904bytes; compressed stream declares2905 and expands to2905 with trailing00.
The declared2904 bytes exactly match the older field13-data/scripts.bin:
SHA256 f03c162372f3e6096d0d362bdfeae6ad20cd837560760b9b88041e077a8dd8f5.
Do not label the one trailing decompressed byte a script mismatch.

Disc-confirmed code-relative bytes (codeBase0x584):
- 0x02A2 CB 01 DC 02: script10 routine1 tests conversation zone1.
- 0x02D5 87 07 80 FE 53 29 0A: finishes scenario7, releases control and
  disables actor10; this zone is NOT the outside field transition.
- 0x055B CB 00 64 05 98 0E 80 00 80: zone0 returns field14/entry0.
- script17 routine2 at0x0569: 15 C4 00 1F 11 47 00 01 80 07 80.
  Door open C4 precedes opcode47 at0x056E. Routine3 at0x0574 is00 (return).
  Opcode47 -> func_80092EA0 -> func_80092894(0x3E0,0,0,0).
  Retail asm func_80092894 at8009297C/88 reads argument4=entry7 and
  argument2=map1, then stores entry variable2 and g_GameSceneMapNum.

Thus the expected natural outside destination here is FIELD1 ENTRY7, NOT
field15. Script17 routine2 is the interaction path; passive routine3 does
nothing. Current actor interaction dispatcher (misc8.c) enqueues script2
on its confirm edge and script3 for passive proximity. The earlier run only
walked beyond the already-open door after Dan left, without a fresh confirm
at the door. It did NOT exercise the expected door exit routine.

Next runtime action: naturally repeat boot/room/conversation, approach the
door, release movement, face it and press actual Circle key Z, then verify
script17/0x056E readiness and field1 load plus visible exterior. Do not force
D_800ADBEC/readiness, invoke handlers, or substitute a field15 quickload.
This is script/assembly evidence only; field1 exterior runtime remains
NOT_OBSERVED. No production edits, commit or push. Full goal remains active.

### Field13 door-confirm natural rerun LIVE (90-minute deadline)

Previous authority pass was progress: outside destination corrected to field1
entry7 and a fresh door interaction identified as the unexercised path.
New private natural run in scratchpad/field13-door-natural.DSDPCe:
game session67539, container xeno-field13-door-natural, host gamePID2272836,
timeoutPID2272822, Xvfb session23145 on :91, window2097158. Deadline5400s.
Reuse auth file from field14-interactive.DXEBdg; do not print its cookie.
Same HEAD f67fe692, binary SHA01df9244e3eb2709ba8021021d21567c762340518fc4caed1ba6d3a4bac84cb4,
same krom-20260913 image21f06ce48b93a0a3dfe5580b22f7ba3370d781ea0ab413c5d3475047102c3390.

First launch was TERMINAL rc1 before SDL window creation: default container
uid1000 was not mapped to the host workspace owner and could not read the
private auth file or write repo logs. Failed evidence run.log is preserved.
Retry uses --userns=keep-id and hostname bazzite; run-retry.log is CURRENT.
Read-only checks confirmed live game, field490, and X window2097158.
No authentication disabling, host package changes, or production edits.

Raw input: prefix of battle-input-extended.Gl1bFd/schedule.txt through16002,
then19000:0x20,19002:0, neutral thereafter. Natural boot only; NO quickload,
kernel override, or bailout. Captures EVERY120 FROM11000. Port key mapping
Z=Circle, V=Triangle, C=Cross, X=Square. Wait on SAME session67539; do not
restart because a polling call yields. Next verify field14 actual load,
battle return, dialogue closure, then repeat documented short steering,
complete field13 dialogue and use Z at the door for field1/entry7 proof.

### Natural field13 door -> field1 exterior and visible movement OBSERVED

Continued SAME session67539/PID2272836/container xeno-field13-door-natural.
Still LIVE on :91/window2097158; Xvfb session23145. Do not restart this game.
Evidence scratchpad/field13-door-natural.DSDPCe/run-retry.log:
field490 line45, field4 line145, field2 line1859, retail battle returned
after80775700 instructions line2958, field14 line2967, field13 line3450,
FIELD1 line3575. No reached [stub] marker in this observed prefix.
title_smoke_verdict_test.py rerun:11 tests OK,2.942s; this only validates
smoke-verdict/input-config logic, not full-game correctness.

After raw19000 confirm/release19002, field14 dialogue closed. A shorter
ordinary-key route now observed (all holds released; wall-clock durations
are navigation evidence, not a deterministic replay specification):
Right0.7 ->(150,0,-496); Up0.5 then Up+Left0.7 ->(203,0,-332);
Down+Left0.4 then Up+Left0.9 ->(139,0,-188); Up+Left1.0 ->(135,0,-40);
Up+Right1.45 ->(209,0,-43), stopped at stair edge; Up+Left0.1 then
Up+Right1.0 naturally loaded field13. field13-confirmed.png is proof;
earlier field13-arrival.png was still field14 despite its filename.

Repeated field13 Right0.5, Up+Right0.55, Up+Left0.3, Up+Right0.25 to start
Dan's conversation. Seventeen separate Z presses in inspected short batches
advanced the previously observed sequence to dialogue closure (dan-17.png).
No choice menu appeared in inspected pages. Then Up+Right0.5 and Z left
Fei at(122,0,1), door script17 still idle IP0x568. Door actor translation
(210,-58,-38): first confirm had not activated it. Another Up+Right0.35,
release,0.2s neutral, Z0.25 caused the NATURAL field1 transition.
door-confirm-02.png and field1-settled.png show the village exterior.

Settled live state: map1, g_ActiveBattleRuntime=NULL, Fei(-751,23,218).
Down0.7 ->(-737,23,120); Down1.4 ->(-688,24,-78); Down1.5 ->(-792,24,-262).
Camera followed; earlier views occluded Fei behind the house roof, and
field1-down3.png finally shows Fei visibly standing beside the house on
the open exterior path. This is observed native exterior movement, not
human desktop acceptance, full visual parity, audio, or full combat proof.

CURRENT player(-792,24,-262), field1; no held keys. Continue normal village
progression from this live session, inspecting Dan and retail scripts as
needed. No quickload, forced flags, teleports, collision bypass, handler
invocation, or kernel shortcuts. No production changes, commit or push.
Whole-game port/decompilation goal remains ACTIVE and incomplete.

### Field1 Dan outside conversation and accepted response OBSERVED

Continued SAME live session67539, PID2272836, :91/window2097158; no restart.
Natural movement from the house skirted the pen/trough and fence. Read-only
actor27 positions changed as the red-haired NPC walked. Short approaching
and facing inputs eventually activated Dan by normal Z; early confirms at
67-86 world-unit separation did not activate his routine. No collision or
interaction bypass was used. dan-contact9.png proves the opening dialogue.

Advanced one page at a time through the outside Alice-marriage conversation.
dan-outside-09.png shows the response choices "Let's do it!" / "That's crazy".
Sent Down0.15 then Z0.2 intending the second option, but DID NOT verify cursor
movement before confirm. ACTUAL accepted branch was "Let's do it!":
dan-response.png, dan-response2.png and Dan's reply in dan-response3.png
prove this. Do not claim the second branch tested, or infer a selector defect
from this alone. Future choice tests must separate direction and confirm and
capture/read back selection before proceeding. No reload/state correction.

Continued the accepted branch through Dan's final thanks; dialogue closed
in dan-response8.png and subsequent ordinary Right0.4 tested control return.
All captures are in scratchpad/field13-door-natural.DSDPCe. Field1 remains
active; no new field transition or reached [stub] marker during this segment.
This extends observed natural NPC interaction, branching dialogue and control
return; it does not establish full story completion or all choice handling.
Next continue natural village/story progression from the SAME live run.
No production edits, commit, push, quickload or forced scenario variables.

### Natural village exit -> world-map runtime OBSERVED; rendering artifacts OPEN

Same live game67539/PID2272836/:91/window2097158. Continued from post-Dan
(415,126,534), navigating around house/fences using ordinary arrows. Near
outer edge: (1191,-40,717) ->(1399,-66,925) -> last sampled field translation
(1670,-152,1196). The final Up holds triggered the WORLD MAP, not field15.
Do not interpret g_GameSceneMapNum=1 or stale g_FieldActors as active world
coordinates. Field1 script50 has multiple exit zones: world-map and field15
branches differ, and the actually reached branch must control the report.

run-retry.log line3960 FE54 snapshot actor50 lock_ip0x184C; line3961
opcode56 at-arm actor50 IP0x184D selector0x0400 heading0x0200 entrance1,
unknown231e1, D_800B02C8=1, field_control=-1, D_800ADBE4=0.
Subsequent worldmap init/mode-entry and scheduler/upload pumps execute.
exit-approach2.png visibly shows world-map terrain, village, player and
minimap. Ordinary Down0.7 changed player/view; worldmap-down.png records it.

OPEN VISUAL DEFECT: village scene elements show striped vertical rectangles
in exit-approach2.png; worldmap-down.png and a later neutral capture
worldmap-neutral.png show opaque white blocks, including large foreground
shapes. Repeated neutral observation confirms these are not merely a missing
capture. Do NOT call this a clean renderer pass or retail visual parity.
Do NOT replace the scene, hide primitives or force flags to remove symptoms.

Next diagnose the actual world-map primitive/texture/upload path in this LIVE
state before repair. Source names use world_map_* (not worldmap*):
world_map_init.c owns natural overlay loading and dispatch;
world_map_upload_pump_74f2c.c and _75104.c own upload queues. The diagnostic
label "before_plant/load context" is historical text before
ensure_world_overlay_image(), not evidence this run planted state.
Current screenshot worldmap-neutral.png is the useful visual baseline.
No production edits, commit or push; full goal remains active and incomplete.

### World-map rectangle texture-page root found and source corrected; runtime pending

Read-only hardware breakpoint at DrawAllSplits (condition g_vertexIndex>1000)
sampled live batches without modifying guest state. Breakpoints deleted and
game detached/resumed after every sample. Live page0x33/CLUT0x7C0F quads
use UV(0,64)..(31,111), matching wm_80085FE0's 512-record double buffer.
Captured CPU VRAM in world-vram.bin under field13-door-natural.DSDPCe.
Palette first entry0x77BD is near-white; page0x33 samples x192,y256, within
the framebuffer allocation. This matches the observed striped/white blocks.

Retail authority: disc/world_map.bin SHA256
4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70,
load base0x8006FAF0. Correctly based disassembly at80086090..800860A0:
move a0,zero; move a1,zero; addiu a2,zero,0x380; addiu a3,zero,0x100;
jal80043A1C (GetTPage). Therefore GetTPage(0,0,896,256)=0x001E.
Old production AND old test incorrectly used GetTPage(0,1,240,511)=0x0033.
GetClut(240,511) before it remains correct; runtime later selects a palette.
An initial disassembly with guessed base0x8006F000 was discarded immediately
after checking the actual source base; do not reuse that misbased output.

Scoped production edit in world_map_common_tail.c corrects that one call;
header and nearby delay-slot comments corrected. w34b5e_80085fe0_prod_test.c
now uses independent literal0x001E and checks all1024 buffered records.
Restored missing native D_80059179 test definition; legacy P0 assertion
updated from guest twin to native owner and added guest-twin negative check.
These source/header/test files were clean before this scoped change.

Host focused build uses gcc -std=gnu17 -O0 -g -no-pie -DXENO_PC_PORT
-DSKIP_ASM -D_LANGUAGE_C with existing pc_port/include,include,pc_port/src
include roots. page-green.log:91/91 PASS. Current tests against original
HEAD production via process substitution provide the pre-fix control;
page-original-control.log preserves its result. Diff whitespace check passes.

IMPORTANT: live game67539/PID2272836 still runs the OLD binary. No hot patch,
new runtime build, or post-fix visual acceptance yet. Do not claim blocks
are fixed on screen. Keep original live evidence while preparing a separately
named fixed build and a natural-route verification. No commit or push.

### Fixed texture-page binary built separately; natural rerun LIVE on :92

Isolated build/evidence directory scratchpad/world-page-fixed.BYXItI.
build-isolated.sh extracts ordered object/archive inputs from the original
xeno-port.map, hashes them, links a control, compiles only corrected
world_map_common_tail.c, and substitutes that ONE object into the fixed link.
The control is byte-identical to the original binary (cmp succeeded):
01df9244e3eb2709ba8021021d21567c762340518fc4caed1ba6d3a4bac84cb4.
Fixed xeno-port-fixed SHA256:
c5f777376aa2df999a01abd0330e93645ae701ee823bc2dd37f80f157c9aa3be.
All original objects/archive hashes reverified after linking. build.log,
control.map, fixed.map, link-inputs.sha256 and verification log preserved.
No overwrite of pc_port/build_native/xeno-port or its objects.

NEW LIVE fixed game session33883, container xeno-world-page-fixed,
host gamePID2324145, timeoutPID2324130, 5400-second deadline.
Xvfb session74948 owns :92; window2097158 (same numeric ID as :91 but a
DIFFERENT X server). Auth file is this new evidence directory's Xauthority;
never use :91's credentials/target when steering the fixed run.
Natural schedule same interval8 prefix through16002, confirm19000/release19002.
No quickload/flags/teleport/kernel shortcut. Captures EVERY120 FROM11000.
Current fixed run reached field490 and opened its private window.

Container uses keep-id, hostname bazzite, same pinned krom-20260913 image.
Repo mount is READ-ONLY with only world-page-fixed.BYXItI mounted writable,
so old baseline logs/binary cannot be overwritten. PsyX's optional root log
reports cannot-create as expected; primary run.log/captures are writable.
Audio device unavailable as before, not audio acceptance.
OLD session67539/PID2272836 is still live on :91 showing pre-fix world map.
Do not confuse their windows, logs, or binaries. Next wait on SAME fixed
session33883 through natural prologue/battle/field14, then reproduce the
documented room->village->world-map route and inspect corrected rectangles.
Post-fix world-map visuals remain NOT_OBSERVED. No commit or push.

### Fixed rerun reached mountain battle victory; post-battle field reload CRASH

Same HEAD f67fe692 and fixed binary c5f777376aa2df999a01abd0330e93645ae701ee823bc2dd37f80f157c9aa3be.
Session33883/container xeno-world-page-fixed is now FINISHED, rc139/SIGSEGV,
not a timeout and not live. PID2324145 is gone. Do not steer its old window.
Original baseline PID2272836 on :91 remained live at this checkpoint; keep it.
Evidence remains scratchpad/world-page-fixed.BYXItI; no main binary overwrite.

Natural fixed route: prologue/battle -> field14 -> field13 -> Dan interior
dialogue -> door interaction -> field1. run.log FieldLoad lines3443/3595
prove field13/field1. Private-X11 captures field13.png, dan-inside-18.png,
door-result.png and village-path*.png show progression. Used ordinary keys,
not save/load, teleports, flags or handler calls. Dan's optional OUTDOOR
conversation was skipped, so this is not an exact dialogue-state replay.

Village timing diverged around fences. The final approach from (1670,-142,1086)
crossed the adjacent MOUNTAIN exit, not the world-map exit: actor50 FE54
IP1834 line3779, FieldLoad field15 line3780. worldmap-entry2.png is therefore
a MOUNTAIN screenshot despite its attempted-capture filename. At field15,
player was (710,89,-1541). Down2s triggered a natural random battle. Do not
use field actor pointers while battle owns/reuses the overlay: a sampled
translation during that battle was stale garbage and is not world state.

Observed battle controls differ from field/raw-key comments: C returned from
targeting to the command menu; X performed the one-point attack (triangle
icon, 2/3 remaining), V the two-point attack, Z the three-point attack.
Z also selects Attack. X then V advanced combat; Z/select then Z/attack
defeated remaining enemies. Jackal summon and attack animations observed.
battle-result.png shows victory with HP48/50 and Exp9. No battle Escape,
state injection, or damage/HP manipulation. Actual mapping root is NOT yet
diagnosed; do not change bindings based only on these observations.

After result confirmations, log4036 reports retail battle returned after
967951681 instructions; log4042 begins field15 reload. Then SIGSEGV.
No settled post-battle field screenshot; battle-return.png was NOT created.
World-map texture correction still has focused tests91/91 and original-code
negative control87/91, but POST-FIX WORLD-MAP VISUALS REMAIN NOT_OBSERVED.

Core preserved via coredumpctl PID2324145 as field15-return.core, with
field15-return-backtrace.log. Actual fault:
func_80021D50 animation_scripts.c:1685 -> func_800A3C8C virtual_machine.c:1153
-> func_800A2714 -> FieldLoad -> func_80078D44 -> FieldMain.
First restore actor i=0, mismatch=0, sprite arg0=0x7cbb68,
save arg1=0x9dc518, savedD80059198=1. Sprite+20 contains two separate words
007cbc1c / 007cbc78. Native void** read concatenates them into
pData=0x007cbc78007cbc1c, faulting on the +6 halfword store. The second
pointer site +7C contains 007cbc5c / 20008000 and has the SAME width defect.
Retail asm/slus_006.64/system/animation_scripts.s:2314-2408 confirms LW
at 80021D78/1DAC/1DBC (+20) and 80021E60/1E8C (+7C), not 64-bit loads.
Sibling func_80021EBC already reads these slots through u32/uintptr_t.

NEXT: scoped production regression for func_80021D50, with nonzero adjacent
words at BOTH pointer slots, then smallest retail-width fix for its five
void** reads. Existing field_snapshot_restore_a3c8c_retail_test STUBS
func_80021D50 and cannot catch this crash. Preserve existing large dirty
animation_scripts.c and virtual_machine.c changes; no repair made here yet.
Then separate rebuild and natural post-battle-return verification, followed
by pending world-map visual comparison. Do not suppress restore or clear
snapshot state to bypass the crash. No commit/push. Full goal remains active.

### Sprite snapshot pointer-width repair tested; combined natural rerun LIVE

Previous turn classified PROGRESS: natural field15 battle victory and a core
localized the restore crash. This turn changes ONLY five pointer loads plus
one explanatory comment in func_80021D50: void** -> u32/uintptr_t at +20
(three loads) and +7C (two loads). Existing unrelated animation_scripts.c
dirt preserved; virtual_machine.c unchanged. No null fallback or restore skip.

New production-body regression:
pc_port/tests/sprite_snapshot_restore_pointer_test.c and its run_*.sh.
Before production edit, mode1 SIGSEGV at model+6; mode2 independently SIGSEGV
at table+0 (GDB line1708). Before/after objects and binaries preserved under
scratchpad/sprite-restore-width. Tests use the REAL func_80021D50 with only
func_800245D8/AnimScriptTick callbacks doubled; verify signed animation ID,
three model scales, packed neighboring words, saved positions/table values,
zero/three ticks with wrapping integration, and restored D_80059198.
Retail SLUS slice12550..126BB SHA256:
bb94bfdd989bd09720f7be46406f4855b62e957deb9eca24ea1c7f0af6e889b3.
Reusable runner pins the slice and tests three pointer modes at O0/O2/UBSan;
all18 restore invocations pass. Independent generated wide-20 and wide-7C
negative controls each fail with SIGSEGV. certificate.log preserves results.
Initial runner compile failed because it omitted the existing -fpermissive
decompilation flag; corrected in runner, no production warning suppression
or assertion deletion introduced.

Adjacent verification: field_snapshot_restore_a3c8c O0/O2/UBSan + mutant PASS;
sprite motion276480 cases/build + five controls PASS; sprite scale917504
cases/build + three controls PASS; sprite arithmetic1089536 cases/build +
six controls PASS (arithmetic.log preserved). Not a full-repository suite.
Review: no added dependency, gameplay shortcut, or loop-bound change; five
loads retain retail widths and ordering. Natural runtime acceptance pending.

Separate build scratchpad/restore-fixed.lpG12t/build-isolated.sh retains the
prior world-page-fixed object, links a byte-identical c5f777... control, then
substitutes ONE new animation_scripts object. Weak symbols copied from the
original object's weak-symbol list. All original link-input hashes rechecked.
Combined binary xeno-port-fixed SHA256:
aa9be84bfbbdf56a9a650d7bcc84498d20cc01b39b9dea08f04a7c1b823a028a.
Original main binary/objects and prior crash binary/core untouched.

First startup session1331/PID2368366 ended rc1 SDL initialization failure.
Diagnosed XOpenDisplay waiting and xdpyinfo timeout: abandoned screenshot
PID2357011 from the prior crash still held X92. Terminated ONLY that capture;
xdpyinfo then returned0. Game already terminal, so explicit retry was safe.
Failed run.log preserved. Do not restart based only on a yielded tool call.

NOW LIVE: session35075, container xeno-restore-fixed, host gamePID2369921,
timeoutPID2369906, 5400s deadline. PRIMARY LOG run-retry.log. Reuses live
XvfbPID2324041 (:92), auth scratchpad/world-page-fixed.BYXItI/Xauthority,
window2097158. Normal opening field490 loaded and window exists. Same raw
schedule prefix through16002 + confirm19000/release19002; neutral afterward.
Repo RO, only new evidence directory RW, same pinned container image.
No quickload/teleport/flags. OLD :91 baseline was still live, not modified.
Next wait on SAME session35075 through field14, walk field13->field1->field15,
win ordinary battle and verify settled field15 restoration. Then verify the
pending world-map texture fix. Both runtime gates remain NOT_OBSERVED for
the combined binary. Full 100% goal ACTIVE; no commit or push.

### Combined binary: world-map trees and natural battle-return gates OBSERVED

Same combined binary aa9be84bfbbdf56a9a650d7bcc84498d20cc01b39b9dea08f04a7c1b823a028a,
HEAD f67fe692, session35075/gamePID2369921 remains LIVE on :92. Primary log
scratchpad/restore-fixed.lpG12t/run-retry.log. Do not restart this live run.
All actions ordinary private-X11 keys; debugger reads only, no save/load,
teleports, encounter edits, forced handlers or battle escape.

Opening battle returned log2984; field14 loaded2993, field13 loaded3439,
field1 loaded3544 after Dan interior dialogue and door interaction. First
attempted mountain approach instead crossed world-map actor50 opcode56
at log3712 (FE54 IP184C, opcode184D). mountain-entry.png therefore depicts
the WORLD MAP, despite its attempted-capture filename.

World-map fix visibly verified: trees render around Lahan and across the
foreground grove where old baseline showed white/striped rectangles.
Viewed new worldmap-down.png alongside
scratchpad/field13-door-natural.DSDPCe/worldmap-down.png. Similar Down0.7
movement, not a pixel-identical camera/position comparison. Combined binary
had not yet executed the repaired post-random-battle restore at that point.
This proves removal of the observed page-source defect, NOT whole-map retail
visual parity. Up1 produced Lahan Village prompt; Z entered field1 normally
(log73665). village-reentry.png proves world->field transition.

Re-entry camera orientation differs: Up increases Z; Right increases X.
Read live trigger geometry to distinguish exits: world-map zone7 rectangle
X1640..2423/Z-1152..1456; mountain zone2 diagonal corners
(2355,1417),(836,2945),(310,2422),(1829,894). Walked north with X1489,
west of the world-map strip. FE02 handler func_80095B3C first checks screen
position; being inside the trigger alone is insufficient until offscreen.
Continued normal Up movement; field15 loaded at73901. mountain-load.png.
Do not misdiagnose that screen-region wait as a story-flag gate.

Ordinary mountain walking triggered random battle at74032. Up held near
entry initially selected Chi; Right then Z selected Attack. Z heavy attacks
resolved Jackal/rabbit encounter; battle-result.png shows HP48/50, Exp9.
Result confirmations led to retail battle return after360316338 instructions
(74085), followed by field15 load74091 WITHOUT the prior SIGSEGV.
post-battle-field15.png is a settled field screenshot, not just a load marker.
Read-only GDB: g_ActiveBattleRuntime=NULL, map15, player(678,-5,-584).
Normal Up0.7 then moved player to(678,-6,-480); post-battle-movement.png
shows Fei and the animated yellow save point. This completes the bounded
natural post-battle restore acceptance for this encounter, not all saves,
animations, encounters, or the full campaign. No full retail/human acceptance.

NEXT: continue SAME live field15 run from(678,-6,-480), normal speed1X,
camera Up=+Z/Right=+X, toward further mountain/campaign progression. Preserve
the crash core and both binaries as negative/runtime evidence. Full port and
decompilation goal remains ACTIVE. No commit or push.

### Further field15 encounters: two more victories, then ambiguous exit

Continuation classified prior turn PROGRESS: both bounded fix gates observed.
Same HEAD f67fe692, combined binary aa9be84b..., live session35075/PID2369921
on :92. No production edits in this continuation. All evidence below under
scratchpad/restore-fixed.lpG12t, primary log run-retry.log.

Continued uphill from(678,-6,-480). Battle2 began during Left/Up movement,
initially showing a Jackal on Fei's side and other enemies across the canyon.
Normal Z/select/Z-heavy sequences won: battle2-victory.png HP46/50, Exp7.
An extra Hob-Jerky item-reward page needed one more confirmation; the first
capture named battle2-return.png still shows rewards and is NOT field proof.
Settled battle2-return-settled.png follows retail return377329282 instructions
at74289 and actual field15 load74295. g_ActiveBattleRuntime=NULL; player
(386,-117,125), initially behind foreground tree. Up2 moved him clear.

Battle3 interrupted a turn at the ledge. Normal Right/select Attack then Z
heavy attacks won: battle3-victory.png shows level2, HP45/56, EP10/11,
prior T.Exp16 and Exp7. Return232959399 instructions74458; field15 load74464.
battle3-return.png and inactive battle runtime confirm a second additional
successful restore. Player(245,-124,219). This is repeated runtime evidence,
not comprehensive combat/animation/save coverage. Discard ALL field actor
position samples taken while g_ActiveBattleRuntime was non-NULL.

Field movement source misc6.c identifies interaction/jump mask0x80; private
X key is the raw Square mapping. Ordinary Left+X0.8 moved player past the
ledge to(120,-108,219), but foreground terrain occludes Fei. This does NOT
prove a successful climb, nor prove a rendering defect without retail compare.
Left2/Up2 then triggered battle4 with enemies across the canyon.

IMPORTANT battle4 NOT A VERIFIED VICTORY: command ring showed Escape/Combo.
Right/Z/Z opened Combo0/28 rather than Attack. C returned, Down selected Item,
Up/Z opened an empty list, C/Left selected Escape. C did not change the ring.
RShift (local PsyX R1 binding) hid the menu; after waiting, another RShift
then X was followed by fade and field return WITHOUT victory/reward screen.
Treat as unintended LIKELY ESCAPE, not a combat win or clean no-escape route.
Do not hide this deviation or claim its exit semantics verified. No debugger
state manipulation occurred. Exact command/button interpretation remains
UNRESOLVED; stop blind shoulder-button probes. The earlier three mountain
wins/restores remain valid evidence. Captures battle4-menu*, battle4-r1.png,
battle4-toggle.png and battle4-outcome.png preserve the actual sequence.
Retail return521484437 instructions74675; field15 load74681.

CURRENT live field15 player(-126,-41,233), battle runtimeNULL. Fei is occluded
by foreground terrain; no crash. Same :92/window2097158/auth under
world-page-fixed.BYXItI. Game elapsed~30m of90m deadline at checkpoint.
NEXT continue source-aware mountain navigation and resolve alternate battle
command controls from actual retail/controller data before another probe.
Full campaign, 100% port/decompilation, audio and human parity remain open.
No commit or push. Preserve this live run and all earlier evidence.

### Read-only controller trace after ambiguous fourth encounter

Same live PID2369921/session35075, no inputs or production changes in this
trace. Native g_ControllerButtonMappings is identity0..7; masks are
20,40,10,80,04,01,08,02. These match disc/SLUS_006.64 at file409E8/40A38
(retail800501E8/80050238). Do not change keyboard bindings to explain the
battle command behavior: current evidence does not identify a binding bug.

Retail battle func_80089CCC converts queued edges into D_800D3014 events:
raw20 (Z)=4, raw40 (C)=5, raw80 (X)=6, raw10 (V)=7. Direction events are
Right0/Down1/Left2/Up3. Authority asm/battle/main42.s 80089F10..80089F68
and branches80089DFC..80089E24; result store8008A118. The native controller
name g_C1ButtonStateReleased is misleading here: ControllerPoll computes
newly-down edges with (current XOR previous) AND current.

Critical command-selection finding: func_8008115C jump table80070010 has
entries4,6,7 ALL pointing to80081194; event5 goes to80081304 (return).
Verified table words directly from disc/battle.bin file520..53F. Thus Z,
X and V can all ACCEPT in this command-selection path. C returns from the
handler without changing state; it does NOT cancel the top-level ring.
X is not a safe menu toggle with Escape highlighted. This supports the
fourth encounter's likely unintended Escape after the final X input, but
does not by itself prove the exact state/branch active in that old frame.
Do not relabel that encounter as a victory. RShift visibility behavior and
why this encounter presented the alternate ring remain UNRESOLVED.

Next battle: inspect current ring, use only source-understood selection and
cancel paths; do not probe X/V as toggles. Attack-context point costs remain
separate from command-selection acceptance. Continue mountain navigation
from the preserved field15 position(-126,-41,233), without state injection.
Full goal active; no commit/push.

### Command-ring navigation follow-up: C is a no-op at ring level

Read-only continuation, same live PID2369921 (elapsed38m when rechecked),
branch experiment/worldmap-open-gates-20260823 at f67fe692. No game inputs,
state injection, production edits, restart, commit or push in this trace.

Correction to the earlier controller summary: event5/C targets the epilogue
in the ring handlers, not a state-changing cancel. This explains why C
could leave Escape selected. Submenu cancellation must be traced separately.

State owner is D_800C3EAC+0x2DD. Dispatch table8006FE7C maps state1 to
func_8008115C, state3 to80081504, state7 to800820A4, state9 to80082504.
Right/event0 transitions state1 -> state7 if the actor's u16 at
base + actor*0x40 +0x26 is zero (800812CC..800812F8). From state7,
Right -> state1 if u16+0x2E is zero (80082278..800822A4). From state9,
Right -> state7 if u16+0x26 is zero (80082574..80082594 and800826B8).
These are conditional transitions, not an unconditional two-key shortcut.
The nonzero availability branches have additional repeated-direction logic
through +0x2F6 and D_800C3E29; do not bypass them or force availability.

State9 accept calls8009A9D0 and conditionally writes0x40 toD_800C48EA,
then sets+0x2DE=1 (80082538..80082570), consistent with the observed Escape
selection. State7 accept calls8008C4A8, consistent with observed Combo;
these label associations still need a simultaneous live state/screen check.
Next encounter: inspect the visible ring and live command state, then use
ordinary Right presses with a screenshot between transitions to reach
Attack. Do not assume Right/Z means Attack from every starting state.

Direct retail verification: compared1438 instruction/table words against
disc/battle.bin using assembly file-offset comments and raw byte equality:
main35 range8008115C..<800826CC, tables80070010..<800700F0 and
8006FE7C..<8006FEA4. All matched. This is static source evidence, not a new
runtime acceptance or completed decompilation. RShift visibility remains
unresolved. Full goal remains active.

### Live command-page verification and fifth mountain encounter

Same PID2369921/session35075, elapsed46m of90m at checkpoint. Ordinary
gameplay only, no production change or state injection. Field15 now at
(287,-127,214), battle runtimeNULL; Fei visible in lower gully. Camera
heading g_Scene+0x56 is0x600 after one Shift_R hold0.15s. This is the
source-backed field camera rotation (misc2.c func800726E8, input bit8),
NOT another battle R1 probe. Direction mapping changed with camera.

Navigation from(-126,-41,233): Up2 blocked; Down1.5/Left2 reached
(-139,-49,189). Up+X1 thenUp1.5 brought Fei partly into view; Right1.3,
Right+X1.2 exposed lower path. Up+X1.2 failed to climb upper cliff;
settled position(220,-135,298). Right2.5, Shift_R0.15 thenUpLeft2
triggered encounter5. Captures resume-* under restore-fixed.lpG12t.
Occlusion is not yet a proven retail mismatch; uphill route unresolved.

LIVE command verification: guest word at g_PsxRam+0xC3EAC=0x800DDDCC;
state byte at guest0x800DDDCC+0x2DD=1 with Attack displayed. Actor0
u16+0x26 and+0x2E both zero. Right0.12 -> state7 with Combo displayed
(battle5-right-combo.png); anotherRight0.12 -> state1 with Attack
(battle5-right-attack.png). Each state sampled read-only after capture.
This confirms state1=Attack and state7=Combo in this encounter, and
explains why blindly pressing Right from Attack can enter Combo instead.
It does not retroactively prove the exact state of encounter4's exit.

Selected Attack and used Z heavy attacks for all enemies including those
across the canyon. Verified victory battle5-result.png: level2, HP41/56,
EP10/11, T.Exp23 before award, earned7; next screen T.Exp30/Exp0.
Hob-Jerky1 item reward in battle5-return-settled.png (despite filename,
this is NOT field). Final Z returned retail after582111883 instructions
at run-retry.log75064. battle5-field.png is actual restored field with
battle runtimeNULL; UpLeft1 moved normally to current position above,
captured battle5-field-move.png. Fourth verified mountain victory/restore
in combined binary; encounter4 remains ambiguous likely Escape.

No commit/push. Full campaign, decompilation and retail parity remain open.

### Display loss: gameplay session no longer resumable through :92

At next continuation (game elapsed46m48s), the first capture failed with
unable to open X server :92. No gameplay input was sent this turn.
Xvfb PID2324041 and /tmp/.X11-unix/X92 are absent; display session74948
reported terminal exit0. Cause of server exit is UNKNOWN. Game launcher
session35075 returned143, BUT host game PID2369921 and timeout2369906
remain live in container xeno-restore-fixed. Do not equate launcher exit
with game-process termination, and do not claim the natural run is live
and playable merely because that PID remains present.

run-retry.log ends after field-frame-104520 capture with
`X connection to :92 broken (explicit kill or server shutdown).`
Two read-only GDB samples place main thread in SDL_DestroyWindow from
PsyX_Shutdown at PsyX_main.cpp1201, reached via exit/_XDefaultIOError
after SDL event polling. It is waiting/spinning in XIfEvent/_XReadEvents.
Saved display-loss-shutdown-backtrace.log has the16-frame chain.
This is a display-loss shutdown hang, not evidence of another sprite
snapshot crash or a battle-return regression. Four verified mountain
victories/restores and prior world-map tree evidence remain valid.

Latest check game elapsed47m50s, processRl. No kill/restart or production
change performed. A replacement X server cannot reconnect this existing
client; fresh natural campaign replay will be needed for further visible
route evidence. Preserve the binary, logs, captures and hanging process
for diagnosis; do not overwrite the existing run directory on replay.
Full objective remains active; source/test work can continue independently
while the existing90m timeout still owns process lifetime.

### Attack-ring C decompilation: bounded differential gate PASS

Replaced only func_8008115C INCLUDE_ASM in existing untracked
src/battle/main35.c with C preserving state1 dispatch, u8 actor masking,
u16 availability, repeat-direction flag behavior, accept call order and
context reloads after callees. Other handlers/leaves untouched. This does
not change keyboard bindings or claim the natural run used this C body.

New pc_port/tests/battle_attack_ring_test.c executes actual retail handler
bytes and checks an independent state contract, then the native body.
131072 cases/build: all256 event values, all16 availability combinations,
zero/nonzero repeat flags, matching/nonmatching previous direction,
valid/FF target, actor arguments0/1/255/0x101. Entire0x4100 context compared
including canaries, plus D_800D366C and callee trace/arguments. Callees are
trace-only deterministic boundaries: their combat behavior and context
replacement side effects are NOT covered by this test.

RED before implementation: retail contract passed; native failed missing
owner8008115C. Retained scratchpad/battle-attack-ring/test is no-owner binary,
with missing-owner-red.log. GREEN runner:
bash pc_port/tests/run_battle_attack_ring_test.sh
evidence scratchpad/battle-attack-ring.QB8cyN: O0/O2/UBSan eachPASS131072,
four mutants rejected (cancel event alias, actor stride, u16->u8 availability,
repeat-flag clear). Runner pins retail text1166C/1BC and table520/20 hashes.
Initial runner hit missing GCC libubsan then Clang defaultPIE mismatch;
final uses existing repository Clang -no-pie linking convention.

Full-suite/build/runtime integration and exact PS1 compiler matching NOT_RUN.
Next strengthen callback-rebinding/clobber coverage before bridge activation;
do not claim whole command-ring decompilation (other states remain ASM).
Whole-tree diffcheck flagged existing unrelated PsyCross patch whitespace;
do not clean those as part of this lane. No commit/push. Goal remains active.

### Attack-ring callback ownership and register-clobber gate PASS

Extended the same differential test, no production edits this continuation.
All retail callee boundaries now clobber caller-saved GPRs2..15/24/25.
Four additional cases replace D_800C3EAC during accept call1,2,3 or the
repeat-Up rejection call. Both original/replacement0x4100 buffers, owner
pointer (native), call trace, arguments and D_800D366C are compared. Retail
must leave original owner untouched and write only state5 or repeat0 in
the replacement. Native C passes all four scenarios.

Runner evidence scratchpad/battle-attack-ring.nVQObs: O0/O2/UBSan each
131072 contract/differential cases plus4 replacement cases PASS. Six
mutants rejected; added stale_accept/stale_repeat cache the old owner and
fail specifically `native context reload differs from retail`. Prior four
cancel/stride/width/repeat controls still fail as expected. Thus callback
replacement and caller-register clobber are now covered, superseding the
previous test limitation; actual callee combat semantics remain excluded.

Integration check: battle_mips_runtime.c initialize_runtime skips symbols
at/above BATTLE_BASE, and runtime_bridge_call returns0 for guest code before
host fallback. Merely compiling this new C body does NOT activate it in
retail battle execution. Do not remove those safeguards for this function;
overlay-data ownership and native-to-guest callee boundaries need their own
integration design and tests. Exact PS1 matching also remains pending.
Hanging game PID2369921 still present at elapsed53m08s on initial check;
no display restart/process mutation. No commit/push; full goal active.

### Attack-ring PS1 compiler baseline: compiles, NOT exact match

Isolated matching experiment used host cpp -P -undef -nostdinc
-D_LANGUAGE_C -DSKIP_ASM -Iinclude on src/battle/main35.c, then repository
tools/gcc-2.7.2-psx/cc1 with build.ninja flags (-O2 -G8 -mips1 -mcpu=3000
-w -funsigned-char -fpeephole -ffunction-cse -fpcc-struct-return -fcommon
-fverbose-asm -msoft-float -mgas -fgnu-linker -quiet), followed by MASPSX
--use-comm-section --run-assembler -EL -Iinclude -Ibuild -O2 -G8
-march=r3000 -mtune=r3000 -no-pad-sections. No shared build output replaced.

Evidence scratchpad/battle-attack-ring-match/main35.i/.s/.o captures initial
function size0x1A0 versus retail0x1BC. Moved actor masking into the accept
case (other cases mask their own argument), and moved Right case after Up,
matching retail source-block order. Current reordered.i/.s/.o size0x1B0;
prologue instruction ordering now follows retail, but register allocation,
address-add operand ordering, branch offsets and rejection-call scheduling
still differ. Size convergence alone is NOT a matching metric or proof.
No linked-byte match claimed; SKIP_ASM isolates C from remaining ASM bodies.

After source change, runner evidence scratchpad/battle-attack-ring.5LZO1C:
O0/O2/UBSan each131072 cases plus4 callback replacement cases PASS; all6
negative controls rejected. Native battle activation remains unchanged.
Next matching work can use isolated objects; compare relocated instructions
and tables before claiming exactness. Full build/campaign remains unverified.
Game PID2369921 still present at elapsed55m34s initialcheck, no process
mutation. No commit/push. Full objective active.

### Attack-ring relocated matching improved to106/111 words (NOT exact)

Added isolated scratchpad/battle-attack-ring-match/handler.ld assigning
text8008115C and rodata80070010 plus referenced external retail symbols.
SUBALIGN(4) is essential: initial linker input alignment moved entry to
80081160; corrected link verified entry8008115C with nm before comparison.
No shared linker/build artifacts changed.

Baseline reordered.o linked as baseline.elf:35/111 same-position retail
instruction words (candidate108 words). Explicit per-branch rejection calls
produced calls.o size1B8 but only27/111, so that candidate was superseded.
Current shared reject label passes arg0=0x4F set in the retail predecessor
blocks. reject.i/.s/.o/.elf/.text size1BC,106/111 instruction words equal.
All8 relocated jump-table words in reject.rodata equal disc/battle.bin520/20.
Remaining text differences at function offsets03C/040/044 (accept-path
register allocation) and0A0/180 (addu operand ordering). No exact-match claim.

Later operands.* experiment used a local row owner and commuted address
expressions, regressing to105/111; reverted only those edits. final.i compared
byte-identical to retained reject.i. Native runner evidence
scratchpad/battle-attack-ring.JcmT7F: all3 modes131072+4 cases PASS and all6
negative controls rejected. No changed runtime binding, commit or push.
Next investigate the five actual differing words, not merely function size.

### Repeatable exact-match gate added; five differences still OPEN

New bash pc_port/tests/run_battle_attack_ring_match.sh and associated
battle_attack_ring_match.ld reproduce the isolated compiler/relocation path.
Unique scratch output, retail text/table SHA pins, exact entry/size assertion,
word-by-word text report and table byte equality. Returns nonzero for any
mismatch; no percentage threshold or skipped comparison. Current run
scratchpad/battle-attack-match.zHwKhp returned1 with precisely offsets
03C/040/044/0A0/180 differing,106/111 text and8/8 table. This is intentionally
an OPEN exact-match gate, not a new passing suite or whole-overlay claim.

Read-only source experiment variants in scratchpad/battle-attack-ring-match:
expr0..7 address parenthesization, array and integer-address forms all106/111;
expr8..11 actor local signed/byte/halfword types all106/111;
expr12..15 byte-typed function argument variants105/111. None applied to
production, no integer pointer truncation introduced. expressions.pl retains
the latter experiment commands; earlier expr0..7 generated artifacts remain.
No production change this continuation. Functional gate remains the previous
verified version; exact-match gate explicitly remains red. Full goal active,
no native integration, commit or push.

### Attack-ring isolated exact match ACHIEVED:111/111 text +8/8 table

Typed-context experiment resolved the five remaining compiler differences.
expr16..19 typed row casts alone stayed106/111; expr20/22 declaring the
context as a struct containing actor rows reached111/111. expr21/23 pointer
arithmetic versions reached108/111. expr24 zero-length trailing row array
also111/111, avoiding a fabricated256-row allocation extent.

Production main35.c now has private BattleCommandActor (asserted size0x40)
and struct BattleCommandContext with GCC zero-length rows tail. Only known
right/down availability and target fields are used as typed accesses;
remaining context byte offsets stay explicit. This is an addressing view,
not an allocation contract or assertion of valid actor counts. Test globals
use the same forward-declared context pointer type and aligned byte fixtures.
No guest/host pointer conversion or runtime registration was added.

Exact runner evidence scratchpad/battle-attack-match.UQDNGh:
ATTACK RING MATCH PASS text=111/111 table=8/8, retail SHA pins and exact
entry8008115C/size1BC checked. This supersedes the previous five-word OPEN
gate for this function only. Full overlay link and native integration remain
unverified; no whole-game or whole-battle exactness claim.

Final behavioral evidence scratchpad/battle-attack-ring.yiNWR7: each of
O0/O2/UBSan passed131072 differential cases +4 callback replacement cases.
All6 negative controls rejected. First run hRUkJR exposed obsolete stride
mutation (no longer matched typed source), NOT a surviving real stride bug;
updated all affected rewrites and added cmp guard rejecting no-op mutations.
Final mutations still test stride60, low-byte availability, wrong cancel,
repeat clear and stale accept/repeat owner writes. No assertions removed.

Process2369921 remains in display-loss shutdown at elapsed1h07m48s; left
untouched under original90m timeout. No restart, commit or push. Full goal
active. Next decompile adjacent command handler or design explicit native
overlay ownership/callee integration; never bypass retail guest safeguards.

### Integration-boundary audit and command-sound signature verification

runtime resolve_memory maps overlay globals to actual g_PsxRam, while
initialize_runtime excludes overlay native symbols. Attack uses four globals
(C3EAC context pointer, D3014 event, D366C gate, C3E29 prior direction) and
three still-ASM accept callees87A38/84A7C/77698. Existing file1 adoption uses
checked live-RAM access, generation/identity checks and nested guest calls;
do not adopt Attack by enabling generic host fallback or shadow-global copies.

Found native signature mismatch: main35 declared sound helper8008AA74(u32),
main43 defined it(u8). Trying caller(u8) produced an extra ANDI in its JAL
delay slot (match110/111, evidence battle-attack-match.Cmxp66). Retail wrapper
8008AA74 itself masks the argument at8008AA8C when forwarding to8008AA40.
Kept word-sized caller; corrected wrapper definition to u32, preserving the
internal mask, and changed tiny-leaf test declaration/call to avoid caller
pre-truncation. Exact Attack gate remains111/111+8/8 at
scratchpad/battle-attack-match.vOLyfo. Attack differential6 controlsPASS at
scratchpad/battle-attack-ring.zxfL7s.

New focused battle_command_sound_test.c and run_battle_command_sound_test.sh
exercise real native8008AA74->8008AA40, versus both retail bodies, intercepting
only final80039DB8 packed-command sink. Pin text1AF50/60. Evidence
scratchpad/battle-command-sound.tBnlU3: O0/O2/UBSan each12288 casesPASS,
all256 low bytes with4 upper-word patterns,4 sound groups and3 gate values;
gate-inversion and packed-ID shift mutants rejected. This verifies command
construction, NOT audio playback. Existing nativeAA40 integer-to-pointer
width warning remains visible; low-address fixture is explicitly checked.

Broad battle_tiny_leaves runner NOT PASS: initial build fails main72 missing
uintptr_t include. Scratch cc-with-stdint wrapper gets past that, then link
fails existing test/body duplicate symbols79ED8/7A280/85454/85618. Did not
repair unrelated production files or silence duplicate symbols. Evidence
scratchpad/battle-sound-signature and retry subdir retained.
No runtime adoption, restart, commit or push. Full goal active.

### Broad battle tiny-leaf runner repaired and verified

Resumed original session23913 without restarting; terminal exit0. Evidence:
scratchpad/battle-tiny-repaired. O0/O2/UBSan each pass7072 retail differential
checks, identical stdout and empty stderr; all73 mutation controls rejected.
This supersedes the broad-runner NOT PASS checkpoint above, not any full
overlay-link or gameplay acceptance gate.

Runner now supplies stdint.h for existing uintptr_t users. Its temporary
production-source test objects explicitly weaken only79ED8/7A280/85454/85618,
which are existing trace boundaries and not kCases subjects. Strong harness
spies remain linked (verified with nm for O0/O2); production objects and game
binaries are untouched. No global duplicate-symbol bypass or test removal.
Scoped runner diff check passes. This remains an isolated comparison suite,
not proof of real callee integration or audio playback.

Resume confirmed branch experiment/worldmap-open-gates-20260823 at
f67fe692147e4b1496834ddb9aa444f1c23462f4 with shared dirty tree preserved.
Old game2369921 still present at elapsed1h19m19s under original90m timeout;
prior display-loss shutdown diagnosis stands, not a playable-runtime claim.
No restart, commit or push. Full port/decompilation goal remains active;
next bounded source candidate is Attack target-selection helper80084A7C,
with callback owner replacement and actual retail bytes as required controls.

### Target-selection 80084A7C retail contract and RED gate

Added battle_target_selection_test.c and run_battle_target_selection_test.sh.
Retail source asm/battle/nonmatchings/main35/func_80084A7C.s matches loaded
disc offset14F8C/sizeC4; runner pins entire battle.bin SHA1830b4ef...3e291.
Evidence scratchpad/battle-target-selection.2MAIq6: explicit --oracle-only
passes69632 cases each O0/O2/UBSan, empty stderr. Covers all256 targets,
counts0..16 (NOT an asserted maximum list capacity), actor args0/1/255/101,
four callee modes: unchanged target, changed target, replacement context,
and count forced zero. Callee verifies initial selection write before call,
masked actor argument, and clobbers guest caller-saved registers.

Contract nuance: initial target is written to old owner+2E8 before841E0;
afterward current owner's target is searched in current list. A match does
not rewrite selection, even when owner/target changed. No match, including
count0, writes list[0] into current owner+2E8. Differential path compares
both complete context buffers, owner pointer, list and count when native
implementation exists; not yet exercised.

Native owner remains INCLUDE_ASM. Standalone test RED reports missing native
owner after retail PASS. Default runner RED currently links against main35
and fails its strong unresolved84A7C reference (FcZbSp); explicit oracle mode
omits body linkage, never reports native PASS. No production body changed.
Next implement84A7C with tests and mutations, and preserve existing Attack
spy boundary plus isolated match-linker retail84A7C symbol assignment.
No native adoption, runtime restart, commit or push. Full goal active.

### Target-selection 80084A7C implemented: exact and differential PASS

Replaced only its INCLUDE_ASM in main35.c with retail-backed C. Mask actor
to low byte, prewrite selection, call841E0, search current target/list/count,
then reload current owner for fallback. No allocation-size or list-capacity
assumption added to production. Retail compiler emitted sizeC4 directly.

Evidence scratchpad/battle-target-selection.oXszEg: native versus retail
69632 cases each O0/O2/UBSan PASS, all6 controls rejected: stride,
empty_fallback, stale_owner, stale_target, missing_prewrite, last_entry.
This supersedes the missing-native RED gate above. Counts tested0..16 remain
bounded; whole-context comparisons and callback replacement are included.

Extended run_battle_attack_ring_match.sh with a separate relocation using
battle_target_selection_match.ld. It positions this helper at80084A7C,
asserts sizeC4 and entry, pins full retail payload and compares every word.
Evidence scratchpad/battle-attack-match.1L0tSz: Attack111/111+table8/8 and
target-selection49/49 PASS. Relocations are independent isolated links,
NOT a full overlay exact link. Target relocation deliberately assumes the
preceding Attack size1BC; verified symbol position fails closed on drift.

Attack behavioral harness explicitly weakens84A7C in its test-only source
object to retain its existing strong trace spy; real helper is exercised
by its own differential suite. scratchpad/battle-attack-ring.CXKszj passed
131072 cases plus4 replacement cases per O0/O2/UBSan and all6 controls.
Added new helper extern addresses to Attack isolated linker. Production
objects/runtime bridge were not weakened or changed. Broader tiny suite
was not rerun after this body addition; no whole-repository test claim.

Full goal remains active. Next adjacent dependency841E0 remains assembly;
native live-overlay adoption and real callee integration remain unverified.
No runtime restart, commit or push.

### List-builder 841E0 retail contract established (native NOT_RUN)

Added battle_target_list_retail_test.c and runner. Source is full retail
841E0..84544, offset146F0 size368; runner pins complete battle.bin.
scratchpad/battle-target-list.NRaf6I terminal exit0: O0/O2/UBSan each81920
contract cases PASS, empty stderr. No production changes in this increment.

Algorithm: reset count and all12 list bytes toFF; actor low byte<3 probes
targets3..10, otherwise0..2, in order via83FF4. Only predicate low byte is
tested. Pack eligible IDs, count them, stable-partition matching group byte
first (D_C3EB4 + actor*28; 28 is STRIDE, not field offset). Then use strict
unsigned16 comparison at D_CCD34 + target*0x170 to swap smaller candidate
into slot0, scanning only matching-group prefix if nonempty, else full list.
This is NOT full sorting; swaps change later entries. Return list[0], FF
when empty. Existing caller declares841E0 void because return was ignored;
when implementing, align declarations and test spy with actual return.

Fixture tests5 actor args0/2/3/FF/101,256 eligibility masks,16 group patterns,
4 rank patterns (ascending/descending/tied8000/alternating0-FFFF); observes
call order, masked args, all12 output bytes, count, return, adjacent canaries.
Predicate is sole intercepted callee, returns dirty upper bits and clobbers
guest caller-saved GPRs. Full native differential and mutants still pending.
Eligibility83FF4 was also read: gates D_D2DCC, C3EB7 stride28, D32A1 stride8;
normal path uses D_D3364 pointer matrix and CCD64 status maskC001, alternate
uses CCE08 status maskC001 (both stride170). Do not replace these with a
simplified eligibility rule when integrating the real callee.

Next implement841E0 against this contract, respecting independent exact
relocations for8115C/84A7C and target-selection841E0 spy. Old game process
2369921 still present at1h27m20s under original90m timeout, already diagnosed
display-loss shutdown; no new playable-runtime observation or restart.
Full goal active, no commit or push.

### List-builder 841E0 native behavior implemented; exact match OPEN

main35.c now implements841E0 with actual12-byte scratch arrays, eligibility
low-byte masking, stable group partition and unsigned rank slot-zero swaps.
Return type u32 preserves retail byte-valued result; adjusted target-selection
spy to same return contract. Missing-native RED recorded in
scratchpad/battle-target-list.MuJdb5 before production implementation.

scratchpad/battle-target-list.RH2jbH: O0/O2/UBSan each81920 native differential
cases PASS, all6 controls rejected (predicate upper-word truthiness, group
inversion, signed rank, tie swapping, final candidate omission, wrong return).
Test verifies list/count initialization at first native predicate call.
Target selection hZfOvs passes69632 cases/build and6 controls; its local test
objects weaken only841E0 to retain controlled spy. Attack45oBWq passes
131072+4 replacement cases/build and6 controls. No production weakening.

Exact runner now moves compiler-emitted841E0 assembly to a separate executable
.target_list section using only section directives before.ent/after.end;
instructions untouched. Both isolated linker scripts place it at800841E0,
define new data/predicate symbols, and retain prior text/table positions.
scratchpad/battle-attack-match.i4B3NM confirms8115C111/111+8/8 and84A7C49/49.
841E0 itself is NOT MATCHING: compiled size28C versus retail368, diagnostic
0/218 same-position words. Native functional pass does not replace this gate.
Next compiler-match its split actor-side loops and split group/no-group
minimum-selection loops against full retail assembly; do not patch emitted
instructions or call the bounded behavior gate exactness.

Old game2369921 and timeout2369906 are now absent by ps after original90m
window; no manual termination/restart in this turn and no fresh gameplay
acceptance. Full native-overlay adoption and real83FF4 integration remain
unverified. Full goal active, no commit or push.

### 841E0 compiler-shape iteration and explicit failing match gate

Split actor-side eligibility loops and matching-prefix/no-prefix rank loops
to follow retail branches; defer total/output initialization until retail
phases, use full arg until each low-byte use. Current body358 bytes versus
retail368 (previous28C), still NOT MATCHING:11/218 same-position words.
Evidence scratchpad/battle-list-match.Sk6zuc. New dedicated
run_battle_target_list_match.sh fails closed on size, symbol entry/extent,
or any word mismatch and saves full comparison.log. Reuses the two existing
exact gates through BATTLE_MATCH_BUILD_DIR; both still PASS111/111+8/8 and49/49.

Current differences start with saved-register allocation (s3 instead of
retail s4 for arg), missing separate saved target index across predicate,
then array-base hoisting/operand scheduling in partition and rank loops.
Retail retains s1 target with s0 advanced separately, s2 candidate pointer,
s3 total, s4 original arg. Do not patch generated instructions to force it.

Updated six mutation source patterns after structural change; no-op guard
retained. qkVSk2 list-builder O0/O2/UBSan each81920 cases and all6 mutants
PASS. L8oZGI target-selection each69632 cases and all6 mutants PASS. Shell
syntax/scoped doc whitespace checks PASS. Full exactness, real predicate
integration, runtime adoption and full port remain OPEN. No commit/push.

### 841E0 saved-target lifetime: retail size reached, match still OPEN

Changed eligibility loops to retain target=i++ before the predicate call,
store its return in eligible, then test eligible low byte. This reproduces
the distinct saved target versus advanced loop index. Intermediate increment
after call reached53/218 size360 (QT39Y2); typed group/rank records regressed
to49/218 (kBTtsM), so that experiment was reverted in source and fixture.
Retained pre-call increment reaches54/218 and exact size368 (xZPXOb).
Both existing isolated gates still PASS111/111+8/8 and49/49. The list match
gate correctly exits1: equal size is NOT instruction parity.

Updated predicate mutation to remove mask from eligible check; no-op guard
retained. MsL69I list O0/O2/UBSan each81920 cases and6 controls PASS.
PP63EV selection O0/O2/UBSan each69632 cases and6 controls PASS.
Next investigate branch-local total initialization and indexed group/rank
load scheduling against xZPXOb/list-comparison.log. No emitted instruction
edits, runtime changes, commit or push. Full goal remains active.

### 841E0 initialization and eligibility prefix now exact

Retained byte-sized target=i temporary, predicate result then i++ (not
preincrement), and i=3 before actor-side conditional; each branch initializes
total independently after its i assignment. This reproduces full retail
prefix000..118 (first11C bytes): stack frame,12-byte clearing, branch setup,
all predicate calls, candidate writes and count increments. First difference
now offset11C, the branch into group partition. YnXYYl exact gate reports
78/218 words, size364 versus368, deliberately FAIL. Prefix improvement
does not establish whole-function exactness; size temporarily differs again.

Experiments: comma loop initialization plus preincrement3KBE4h35/218 size36C;
post-call increment qbF8jJ53/218 size360; byte target DtjMeS56/218 size360;
final explicit prebranch i=3 YnXYYl78/218 size364. All emitted evidence kept.
Next investigate base-address hoisting and register assignment in group
partition: retail output=a1, candidate pointer=a0, matching pointer=a2;
candidate uses output=a3 and hoists group base into t1 at offset128.

FHitKW list differential each81920 O0/O2/UBSan and all6 controls PASS.
dX75Qo selection differential each69632 and all6 controls PASS. Existing
Attack111/111+8/8 and selection49/49 exact gates pass inside YnXYYl run.
No runtime adoption, commit or push. Full goal remains active.

### 841E0 group equality operand scheduling:97/218, still OPEN

Retained equivalent group equality with candidate operand first and actor
operand second. n4a8C7 reaches97/218 words and size368; prefix through118
remains exact. First mismatch still11C; GCC hoists group base into t1 and
uses different output register versus retail indexed-symbol loads. This
does NOT establish group-phase parity. Splitting output++ into separate
statements regressed80/218 size364 (f3iDtS), so that experiment was reverted.
Both prior isolated exact gates remained PASS in the match run.

Updated group-inversion mutation to match reversed operands, preserving
no-op rejection. WfdrjQ list81920 cases/build and all6 controls PASS;
cFNHDL selection69632 cases/build and all6 controls PASS (O0/O2/UBSan).
No runtime changes, commit or push. Continue exact compiler reconstruction;
full native integration and full goal remain open.

### 841E0 row-addressing declarations:115/218, exact gate still FAIL

Pointer-addition syntax alone stayed97/218 (qlkhDg). Declaring group as
extern u8 D_C3EB4[][28] and indexing [actor][0] reached108/218 (1GkxeF).
Rank as extern u16 D_CCD34[][184], indexed [target][0], reaches115/218
at retail size368 (dAEsct). These are stride/addressing views with no outer
allocation extent, not inferred gameplay structures. Test definitions now
use matching row types (256 group fixture rows,11 rank fixture rows).
Indexed-symbol loads replace hoisted base addresses; remaining register
assignment and redundant-load/scheduling differences keep gate FAIL.
Actor-first equality with row types regressed104/218 size364 (5G4NbC),
reverted to verified candidate-first form. Prefix through118 stays exact.

Ahddg3 list O0/O2/UBSan each81920 cases and all6 controls PASS; bQd2MU
selection each69632 and all6 controls PASS. Group/signed-rank mutation
patterns updated for row indexing, no-op guard intact. Prior Attack and
selection exact gates pass inside dAEsct. Full goal remains active; no
runtime adoption, commit or push. Next source-matching question is why
retail reloads candidate after group comparison while compiler reuses its
register, plus corresponding rank-swap loads; do not inject instructions.

### 841E0 candidate-lifetime experiments ruled out; retained115/218

No new production form retained this turn. Explicit s32 candidate temporary
NNj36G stayed115/218 size368. Append-loop candidates[i++] followed by
candidates[i-1] fc1MTZ regressed85/218 size378. Actor-first group equality
with explicit u32 candidate cast thJGUj gave104/218 size364. Unsigned output
index FzT3XM stayed115/218 size368. Restored original signed output, direct
candidate-first row comparison and append loop exactly; fixture/mutations
unchanged. No redundant behavioral rerun of the restored verified form.

Read generated rank loops directly: candidate retains first target in a1,
next target in a3, emits extra ANDI before next rank addressing, and stores
cached first value. Retail uses v0/a0 for target indices and reloads slot0
after comparison. Next useful experiment is explicit word-sized next-target
temporary inside matching branch, then separate comparison versus swap
lifetimes; avoid repeating the four ruled-out variants above. Existing
match script continued proving Attack/selection exact gates before each
deliberate list FAIL. No generated instruction edits, runtime changes,
commit or push. Full exactness and full goal remain open.

### Rank temporary experiments and full-sort negative control

Explicit u32 next index inside matching branch reached107/218 size360
(v9HMOj). Changing swap temp to u8 recovered115/218 size368 (QvziYU);
adding explicit u32 first index stayed115/218 (6WpOCQ). The latter differs
from retained dAEsct starting byte527, so equal counts are not equal code.
Generated rank indices shed conversions but cached swap source remained;
no exactness gain. Restored verified simpler source, no new production
form retained. Do not repeat these rank temporary combinations.

Added seventh mutation full_sort, inserting a complete rank sort after
retail slot-zero selection. nyQXpq: O0/O2/UBSan each81920 cases PASS and
all7 controls rejected, including full_sort. This explicitly guards the
retail non-sorted tail against a plausible but incorrect translation.
Shell syntax and scoped documentation whitespace checks PASS. Native
adoption/full exactness remain open; no runtime changes, commit or push.

### Real native target-selection/list-builder chain verified in fixture

Extended battle_target_list_retail_test.c to execute actual native
84A7C->841E0 against both actual retail bodies in the MIPS adapter. Only
83FF4 eligibility remains intercepted. Neither production function is
weakened or replaced in this suite (O2 nm confirms both strong T symbols).
Each existing fixture now tests two selection inputs: all byte values via
mask, and explicitly the last listed target when nonempty. Checks pre-call
selection at every predicate call, complete0x4100 context including canaries,
owner pointer, list/count and eligibility call sequence. No live guest
state injection or runtime adoption is involved; this is test fixture RAM.

Z9vXWc terminal exit0: O0/O2/UBSan each81920 direct-list cases plus163840
real-chain cases PASS, empty stderr. All7 list controls and2 new chain
controls rejected (skip list builder, always overwrite with first target).
The latter proves preservation of a listed non-first selection, not merely
that list construction alone works. Earlier SmDeJA chain run passed before
chain controls were added. Shell syntax/scoped doc whitespace checks PASS.

No production edits this turn. 841E0 isolated exact gate remains OPEN at
115/218 words, size368; prior caller exact gates remain separately recorded.
Next real dependency83FF4 still assembly, and live-overlay ownership/adoption
is unverified. Full port/decompilation goal active; no commit or push.

### Eligibility83FF4 retail contract:163472 cases; native NOT_RUN

Added battle_target_eligibility_retail_test.c and runner, pinned full battle
payload and loaded offset14504/size114. G4kQIl terminal exit0: each
O0/O2/UBSan163472 cases PASS, empty stderr. No production change this turn.
Tests all65536 status values on both normal/alternate paths (131072), plus
32400 combinations of actor/target low-byte aliases, group coords0/1/7/255,
present/block/alternate/matrix bytes0/1/255. Same actor and target share the
physical group byte; fixture accounts for this instead of inventing state.

Checks exact data-read sequence, access widths and return; any write fails.
Absent target reads only D_D2DCC[target]; blocked reads additionally C3EB7
stride28, then exits. Alternate flag D32A1 stride8 nonzero reads only CCE08
stride170 status after those gates, bypassing matrix. Normal reads target
and actor group bytes at C3EB4 stride28, D_D3364 pointer, matrix byte
base+140+actor_group*64+target_group*8, then CCD64 stride170 status only if
matrix bytezero. Both status masksC001; inactive status word deliberately
has opposite acceptance result. Nonselected matrix bytes poisonedA5.

Fixture remains retail-only and explicitly reports native NOT_RUN. Next add
native differential gate and replace83FF4 INCLUDE_ASM, preserving existing
predicate-spy boundaries and isolated section relocations. Real full-chain
eligibility integration still pending; 841E0 exactness remains115/218 OPEN.
No runtime adoption, commit or push. Full goal active.

### Eligibility83FF4 C implemented and differential verified

Added native gate first: qd3hpE reports missing owner after163472 retail
cases (RED). Replaced83FF4 INCLUDE_ASM with C preserving target low-byte,
early presence/block exits, alternate bypass, matrix offset140 with actor
group*64+target group*8, and status maskC001. D_D3364 uses u8* consistent
with existing main30 declaration. Row declarations remain addressing views.

O5GbRi: O0/O2/UBSan each163472 native differential cases PASS; all5 mutants
rejected (status mask, wrong status source, matrix offset, matrix stride,
reject-all). Native matrix pointer is NULL on early/alternate paths to guard
unwanted access. Retail still checks exact data-read order/no writes; native
checks result/input preservation, not instrumentation of every native read.

Exact relocation adds separate .eligibility at80083FF4, symbol data pins
and no generated instruction edits. G0Ql1L preserves Attack111/111+8/8 and
selection49/49. New eligibility itself NOT MATCHING:38/69 same-position
words, size110 versus114. Full overlay link and all-native chain pending.

New body exposed same-TU optimization in controlled-predicate fixture:
wLnG3z O2 spy argument failure. fno-inline/fno-ipa-ra removed that issue but
4OIiz6 O2 still failed result; objdump showed test eax,eax after call rather
than low-byte test because GCC inferred real predicate range0..1. Postcompile
symbol weakening cannot invalidate these assumptions. Final harness generates
controlled-boundary.c renaming ONLY predicate definition before compilation,
leaving caller references external to strong spy; unused real copy GC'd.
Mutation no-op guards compare against that generated baseline, not original
source, so predicate-definition rename cannot hide a failed mutation.

HiiFEB terminal0: O0/O2/UBSan each81920 list +163840 actual84A7C->841E0 chain
cases PASS and all7 list/2 chain controls rejected. Selection sBsaZs each69632
and6 controls PASS. No production flags changed. Tests must not claim full
real-predicate integration until a separate all-native chain gate passes.
Shell syntax/scoped documentation checks PASS. No commit/push/runtime adoption;
full goal active and remaining exact-match gaps explicitly OPEN.

### Eligibility exact gate and boolean-result shape:59/69

Added run_battle_target_eligibility_match.sh: independent pinned69-word
comparison and entry/size assertion, fails closed. Initial zevryj38/69
size110. Equivalent reordered matrix expression dg8W5x unchanged. Making
local result u8 (public return remains u32) reaches59/69 at exact114 size
(k2521K), reproducing retail normal-status boolean temporary/move sequence.
Remaining10 differences all offsets06C..094, matrix group-load ordering,
register allocation and address additions; other instructions now match.

Explicit matrix_row pointer MSXY20 regressed1/69 size118. Word group
temporaries4tw8UH and byte group temporariesji2HbR both59/69 size114.
Discarded these unnecessary temporaries and kept expression plus u8 result.
WfmZSp eligibility O0/O2/UBSan each163472 cases and all5 controls PASS.
lm58I9 each81920 list+163840 controlled-eligibility real caller/list-chain
cases and all7 list/2 chain controls PASS. Existing Attack/selection exact
gates remain PASS inside eligibility exact runs. Full all-native chain and
both remaining instruction-match gaps remain open. No commit/push/adoption.

### Eligibility83FF4 isolated exact match ACHIEVED:69/69

Source expression now (D_D3364 + (actor_group*64 + target_group*8))[0x140].
Grouping scaled offsets before adding pointer and applying fixed byte offset
last reproduced retail load/add scheduling without any instruction patch.
Target-first grouped expression wA2k4t reached65/69; actor-first grouped
expression QKzePo PASS69/69, entry80083FF4, size114, full retail SHA pinned.
This supersedes eligibility38/69 and59/69 checkpoints only. Attack111/111
+table8/8 and selection49/49 remain PASS within this exact run. List841E0
gate freshly rechecked and remains115/218, size368, deliberately FAIL.

4WHG3W eligibility: each O0/O2/UBSan163472 cases and all5 controls PASS.
VboTcD list/controlled-eligibility chain: each81920+163840 cases and all7
list/2 chain controls PASS. DScKUL selection: each69632 and6 controls PASS.
Source arithmetic only changed in83FF4; mutation patterns remain applicable.
Scoped documentation whitespace and shell syntax checks PASS.

Full real83FF4->841E0->84A7C chain integration test still pending; controlled
predicate chain evidence is not that proof. Native live-overlay adoption,
full overlay exactness and full port remain OPEN. No commit/push/restart.

### Full three-body target chain verified without call replacements

New battle_target_chain_test.c and run_battle_target_chain_test.sh compile
unaltered main35, using real84A7C->841E0->83FF4 in native and actual retail
bodies in MIPS. No bridge callback, symbol weakening, renamed callee or
forced no-inline flags. O0 nm confirms all3 strong T body symbols.

LwvLhr terminal0: each O0/O2/UBSan30720 cases PASS, empty stderr;3 controls
rejected, one per stage (wrong eligibility status source, final list candidate
omission, always-first selection). Earlier1xTwK9 passed before controls.
Fixtures cover5 actor args,256 bit patterns,6 eligibility modes and4 rank/
group patterns. Modes: presence, block flag, normal status, alternate status,
matrix denial, alternate bypass of denied matrix. Compare explicit retail
contract, all12 list bytes/count, complete0x4100 context, native owner pointer
and matrix preservation. Group/record globals are native fixture arrays;
this is NOT live guest/global ownership adoption or natural gameplay proof.

Supersedes pending all-native chain-fixture gate, not841E0 exactness115/218,
full overlay linking or full port acceptance. No production edits this turn.
Existing controlled-predicate suite remains for dirty upper-word and callback
trace cases not possible with boolean real83FF4. Syntax/scoped doc checks
PASS. Full goal active, no commit/push/runtime restart.

### Chain-fixture adversarial review and actionable coverage repair

This turn's signed scratch-array experiment cQFMOp regressed10/218 size36C;
explicit list extent5oTj64115/218 unchanged. Restored production source.
Current preprocessed main35 matches QKzePo exactly (cmp PASS). Match runner
now reports contiguous prefix11C separately from total115/218 (wiOmUq),
without weakening failure. No production edits retained.

Doubt-driven fresh-context read-only review Erdos identified four actionable
integration-fixture gaps: bundled status bits, group membership fixed by
index modulo4, matching prefixes too short to distinguish full sort, and
uniform A5 context padding hiding a four-byte copy. Classified all as valid
fixture limitations, not demonstrated production faults. Cross-model review
skipped for autonomous continuation; no external model CLI invoked.

Added the four concrete mutations first. JspCyo old fixture still passed
30720 cases but status_bits mutant survived (RED), confirming limitation.
Revised fixture varies group membership independently (including all-same
groups), uses individual1/4000/8000 status bits and combinedC001 on both
status paths, and fills opaque context with address-varying bytes.
yFITtl terminal0: O0/O2/UBSan each122880 all-real-chain cases PASS and all7
mutants rejected: prior3 plus status_bits/group_index/group_sort/wide_selection.
This supersedes weaker30720 integration coverage, not exactness/live gates.
Second fresh-context review requested; reconciliation pending its result.
No commit/push/runtime adoption. Full goal remains active.

Second review (Socrates) completed read-only: no substantive issue under
the bounded revised contract. Reviewer did not run write-producing tests;
main-agent yFITtl execution above supplies that evidence. Reconciled as no
new actionable findings, stopped doubt cycle after2 reviews, both agents
closed. This closes fixture review only, not retail instruction parity or
live adoption. Exact prefix diagnostic and all7 integrated controls retained.

### List-matching phase diagnostics and scratch-layout exclusions

16-byte scratch arrays with12-byte initialization Yca07F unchanged115/218;
explicit candidate pointer alongside index PlYxvC regressed95/218 and broke
prefix. Restored source, confirmed current preprocessed main35 identical to
QKzePo (cmp PASS). No new production form retained or behavioral rerun needed.

Match runner now reports fixed retail phase boundaries without changing
acceptance: Sd3jpq stillFAIL115/218, size368, prefix11C. Initialization/
eligibility000..11B71/71; partition/append11C..1EF14/53; rank1F0..33B19/83;
return33C..36711/11. Counts sum to218 words. These are diagnostics, not
independently accepted substitutes for full exactness. Both neighboring
exact gates continue PASS. Shell syntax checked. Next work remains the
two middle phases, not allocation size or entry/return code. No commit,
push, runtime restart or adoption; full goal active.

### Live-RAM adoption proposal and boundary review

Added BATTLE_TARGET_LIVE_RAM_ADOPTION.md: PROPOSED, NOT IMPLEMENTED.
It scopes shared production bodies 83FF4/841E0/84A7C against actual guest RAM,
with packed four-byte owners, lazy width-specific checked accesses, existing
overlay rejection intact, and no guest replay after selected partial writes.
The document records retail body hashes and the bounded two-direct-call
selection ABI audit; indirect/other-overlay ABI coverage remains open.

First fresh-context review (Averroes) found three actionable proposal gaps:
list exactness must precede ordinary list/selection dispatch; code identity
must cover every substituted descendant; and host-local scratch arrays do
not reproduce observable guest-stack aliasing. All three were incorporated.
Unsupported alias cases require a no-write preselection decline to guest
execution and remain native coverage gaps. No permissive alias assumption.

Second fresh-context review (01a09ebd-542f-7083-be8b-e72c3bb2122f) checked
the revised contract against production C, all three retail bodies, runtime
guards and file-1 failure handling, and found no substantive issue under the
bounded proposal. Review cycle closed after two reviews. Cross-model review
was skipped; no external model CLI invoked. This is design review, not proof
of an implemented RAM adapter, complete ABI census, or runtime acceptance.

Resume confirmed branch experiment/worldmap-open-gates-20260823 and HEAD
f67fe692147e4b1496834ddb9aa444f1c23462f4 with existing dirty work preserved.
No production edits or test/build reruns for this documentation-only step.
Next implementation gate is shared-body extraction retaining both exact
neighbors and the honest 115/218 list mismatch, before isolated packed-RAM
tests. Ordinary live list/selection dispatch still requires full list match.
No commit, push, runtime restart or adoption; full goal remains active.

### Shared-body extraction prerequisite: full-chain mutation source

Inspection found target-chain runners apply mutations directly to main35.c.
Moving bodies into shared includes would therefore make their mutation and
controlled-boundary rewrites miss the implementation. Before production
extraction, updated run_battle_target_chain_test.sh to mutate an O2-expanded
production translation unit. Ordinary O0/O2/UBSan builds still compile the
original production source; the expanded baseline additionally runs the same
retail oracle before any mutation. No-op checks compare against that baseline.

fBLcRL terminal0: all four baseline runs each PASS122880 cases; all seven
mutants rejected. Shell syntax and scoped whitespace checks PASS. No source
algorithm, dispatch, exact-match acceptance or compiler flags changed for
ordinary production-source builds. This is a test-runner refactor, not a
runtime fix; existing negative controls provide the regression evidence.

Still required before moving bodies: adapt eligibility/selection/list and
Attack runner source rewrites to reach shared includes, preserving the list
fixture's definition-only eligibility rename before compilation. Shared-body
extraction itself and packed-RAM implementation remain NOT IMPLEMENTED.
Full goal active; no commit/push or runtime restart.

### Target-body include extraction, verified without algorithm changes

Adapted eligibility/selection/Attack mutation runners to expanded production
source with an independently executed O2 expanded baseline. List runner now
expands using each build's flags, then renames only the eligibility definition
before compilation, with a fail-closed no-op check. Its O2 controlled baseline
feeds all nine mutations. Before extraction all four suites passed (VQoTgB,
O25Qp3, zKRaKd, OSIUEl), retaining every existing negative control.

Moved the three production definitions verbatim out of main35.c into
src/battle/target_eligibility_impl.inc, target_list_impl.inc and
target_selection_impl.inc. Main35 includes them at the original positions.
No checked-RAM bindings introduced yet; these are shared-body extraction
foundations, not implemented live adoption or a finished access interface.

Post-extraction evidence, all behavioral runners terminal0:
- kLsxyn full chain: O0/O2/UBSan and expanded O2 each122880; seven mutants.
- zdCb5N list: each81920 plus163840 real selection/list cases; nine mutants.
- e8XTWF eligibility: each163472, including expanded O2; five mutants.
- bg51n3 selection: each69632, including expanded O2; six mutants.
- Y4GF7t Attack: each131072 plus4 callbacks, expanded O2 too; six mutants.
All33 negative controls rejected. These remain fixtures, not runtime proof.

Exact gates: wWYbJI list remainsFAIL115/218 size368 prefix11C, unchanged
phase counts. 1Fknjg eligibilityPASS69/69; both also retain selection49/49
and Attack111/111 plus8/8 table. No acceptance threshold changed. Next is
the checked access binding design, preserving these source/behavior gates
and the adoption contract's unresolved stack-alias/ABI/identity requirements.
No commit, push or native runtime activation; full goal remains active.

### Eligibility read-binding boundary, legacy exactness retained

target_eligibility_impl.inc now accepts six custom unsigned read operations
via XBT_ELIGIBILITY_CUSTOM_BINDINGS, with legacy expressions as the default.
Bindings are scoped/undefined after inclusion. Matrix operation owns group
reads and pointer lookup, and is reached only on the normal eligible path.
This is an access seam, not a checked-RAM implementation or runtime dispatch.

New custom-binding test first failed compilation on direct global references
(RED). After replacing only reads, QuhKDK PASS327680 cases each O0/O2/UBSan:
all65536 status values across five access paths, masked actor/target IDs,
result and ordered accessor trace. This trace does not prove ordering inside
the future matrix binding. Initial CXY3ui UBSan link lacked GCC runtime;
Vrj2hC exposed PIE mismatch. Final runner uses existing harness pattern:
GCC compile, Clang -no-pie link, sanitizer still enabled.

ClQWHQ exact eligibility69/69, selection49/49, Attack111/111 plus8/8 table
PASS. TwmpZj list honestlyFAIL115/218 size368 prefix11C, phases unchanged.
UoY32K eligibility behavioral/5mutants, qtSxqk real full-chain/7mutants,
zVDPYJ controlled list+selection-chain/9mutants all terminal0, including
O0/O2/UBSan and applicable expanded O2 baselines. Shell syntax PASS.
Next: implement/test actual packed-RAM eligibility reads, including checked
pointer derivation and internal matrix read semantics. List/selection read
bindings, guest-frame alias handling, ABI and live dispatch remain pending.
No commit/push or native activation; full goal active.

### Isolated packed-RAM eligibility implementation, review pending

Added pc_port/src/battle_target_eligibility_ram.c/.h: caller-owned stable RAM,
no dispatch registration. Reuses target_eligibility_impl.inc via custom
signature and read bindings. Checked KSEG0/KSEG1 offsets, actual widths,
little-endian packed4byte owner, target-group then actor-group then owner
then matrix read, checked addition, local failure jump returning-1 without
writing result. RAM readonly; result outside RAM is an explicit precondition.
No identity/ABI/live-adoption claim. No changes to runtime bridge/build list.

TDD bounds test initially failed on missing helper header. jYtgAn final
terminal0: O0/O2/UBSan each163472 packed-RAM vs retail cases, plus bounds,
aliases, unusednullmatrix, exactlastbyte, truncatedpointer and no-write tests.
Three controls rejected: pointer_width, matrix_offset, invalid_segment (the
last intentionally asserts/aborts). Earlier ZhziSd needed packed-mode final
reporting rather than the legacy weak-symbol presence check; that check
remains mandatory in ordinary mode. No legacy oracle-only weakening.

An optional-context signature initially changed expanded whitespace so the
legacy status_mask mutant became a no-op and correctly failed (I1UEQW).
Replaced it with an explicit custom-signature macro; default declaration is
unchanged. qInKVK final legacy O0/O2/UBSan+expanded baseline PASS163472 and
all5mutants rejected. vu8KxU binding traces PASS327680 eachO0/O2/UBSan.
HyM8jD exactPASS eligibility69/69, selection49/49, Attack111/111 plus8/8.
List remains115/218 from prior gate; no new list exactness claimed.

Fresh read-only adversarial reviewer Tesla 01a09ec9-9524-7aa1-a8e4-7e4bb403b653
is reviewing this bounded helper/contract/tests. Last wait timed out while
review ongoing; DO NOT duplicate or restart reviewer. Reconcile findings
next, then close review. Cross-model skipped for autonomous continuation;
no external CLI model invoked. Next packed-RAM evidence still needs broader
overlap/read-trace and failure coverage as review determines. Full-chain
RAM bindings and live adoption remain pending. No commit/push/runtime start.

### Packed-RAM helper review: actionable coverage repairs

Tesla completed: no concrete implementation fault under stated preconditions,
but three valid/actionable coverage gaps (packed read order unobserved,
presence/alternate/status truncations missing, differential RAM preservation
unobserved). Added reorder mutation first: PUIvoK group_order SURVIVED (RED).

Added test-only completed-read hook in read_ram, compared packed address/width
trace with retail, and snapshot/compare entire2MiB RAM around every packed
differential case. Bounds tests now require exact-1, initial zero-size and
presence/alternate truncation, target255 one-byte-only status reads on both
paths, and exact two-byte success. Preserve result/RAM on failure.
pHzmHU terminal0: O0/O2/UBSan each163472 differential cases plus boundsPASS;
all6mutants rejected: group_order, owner_order, clear_read, pointer_width,
matrix_offset, invalid_segment. Last mutant intentionally asserts/aborts.
QKgA0d ordinary legacy suite terminal0: each163472 plus all5mutants rejected.

Second reviewer Peirce found no implementation fault and confirmed these
repairs, but correctly noted no uninstrumented-build gate. Added normal
O0/O2/UBSan object builds, nm observer-absence check and bounds executions.
Executed equivalent new-block commands against pHzmHU evidence: all3PASS,
observer absent. This was a targeted check after the complete instrumented
suite, not a rerun of the full suite with the final runner text. SyntaxPASS.
Untracked source history is a provenance limitation; source was preserved,
not committed merely to satisfy review. Cross-model remains skipped.

Final narrow reviewer Goodall 01a09ece-2d61-70b2-b3e8-fe0cc98e0188 checks
only the normal-build gate; result pending. Reconcile and close the three
reviewers after this result. Do not exceed three review cycles. No functional
production change beyond a compile-guarded test observer; no native dispatch,
commit/push/runtime start. Full goal active; stack/ABI/identity adoption and
list exactness115/218 remain unresolved.

Goodall completed: no substantive fault in the bounded normal-build gate.
Reconciled as no new actionable findings; three-cycle review closed and all
three reviewers closed. Static review supplements, not substitutes for, the
main-agent pHzmHU execution and targeted uninstrumented checks above.

### List shared-access boundary, legacy code unchanged

target_list_impl.inc now exposes custom signature, locals, count/list-byte
lvalues, group/rank reads and eligibility call bindings. Default expressions
retain legacy source shape. Index expressions must be evaluated once (notably
output++); custom scratch storage supplies at least12 bytes per array. This
is not guest-frame reproduction or native adoption of nonmatching code.

New bindings fixture initially failed to compile on direct legacy globals
(RED). z41lLB final O0/O2/UBSan PASS2048 cases with alternate fixture ownership,
pointer-backed scratch arrays/canaries, dirty-upperword predicate, independent
partition/slot-zero-swap expected output and low-byte actor arguments.

CINtFD list binary cmp wWYbJI PASS byte-for-byte: stillFAIL115/218 size368,
prefix11C and same phase counts. 1ycOSz eligibility69/69, selection49/49,
Attack111/111 plus8/8 table allPASS. sBPCLc controlled list each81920 plus
163840 real selection/list cases and all9mutants PASS; lK7XGU fullrealchain
each122880 (including expanded O2 baseline) plus all7mutants PASS. Both
behavioral runners terminal0. SyntaxPASS. No exactness threshold changed.

Next: selection access boundary and an explicit guest-frame implementation
or proved no-write preselection decline for list/selection RAM integration.
Do not equate custom fixture pointers with guest stack/alias correctness.
No dispatch, commit/push or runtime restart; full goal remains active.

### Selection shared-access boundary, exact49/49 retained

target_selection_impl.inc now accepts custom current-owner selection lvalue,
actor-target/count/list reads, list call and function signature. Defaults
expand to existing legacy expressions. Source access points do not themselves
prove retail load ordering/counts or guest-frame effects; documented explicitly.

New replacement-owner binding fixture first failed on legacy globals (RED).
UVXJOE O0/O2/UBSan each34816 cases PASS: two boundary actors, all byte targets,
counts0..16, owner replacement/target change combinations, pre-call write
visible to builder, empty-list fallback, complete fixture/canary preservation.
Expected snapshot uses memcpy to preserve padding for whole-fixture compare.

mDTcGt exactPASS selection49/49, eligibility69/69 and Attack111/111+8/8.
EAgpXB selection differential each69632 plus6mutants, ozM6ly fullrealchain
each122880 plus7mutants, mrTgFM list each81920 plus163840 real selection/list
cases and9mutants: all terminal0, including applicable expanded baselines.
All22 controls rejected; no mutation selector regression. Shell syntaxPASS.

All three bodies now expose shared source access boundaries; only eligibility
has an isolated checked-RAM implementation. Next work is actual guest-frame
and access-fidelity handling for list/selection, not enabling dispatch from
fixture evidence. List remains115/218; no live adoption/ABI/identity claim.
No commit/push/runtime start; full goal active.

### Retail guest-frame alias contract: observable divergence proved

Added battle_target_guest_frame_test.c and runner. Executes actual retail
841E0+83FF4 instructions without bridges. Verifies first six stores in order:
frame48=s4,38=s0,4C=ra,44=s3,40=s2,3C=s1; permitted scratch/save write areas,
callee-saved registers/return/sp restoration, list result and untouched gaps.

Six alias controls: matrix value points into list frame+10 (candidate), +20
(matching), or +38 (saved s0), each KSEG0/KSEG1. All groups0, targets3..10
present, rank/status0. Negative mode deliberately gives list a private shadow
frame while eligibility sees authoritative RAM. Both modes still execute
retail instructions. This is a faulty memory-model control, NOT an accepted
native adapter or alternative gameplay implementation.

cBufc5 terminal0 O0/O2/UBSan: candidate alias retailcount0 vs split8;
matching and saved-register aliases retail8 vs split0, for both RAM aliases.
Six differences proved per build, frame/restoration checks PASS. This proves
host-local frame substitution can change eligibility, not that a particular
future native adapter has reproduced all alias cases. Selection frame and
indirect pointer/global overlaps remain separate coverage/implementation work.
Next RAM integration must reproduce observable frames or no-write decline
before selection with explicit coverage gaps. No source body/runtime edits,
dispatch, commit/push or runtime start. Full goal remains active.

### Isolated native list RAM/frame helper, bounded alias parity

Added battle_target_list_ram.c/.h outside runtime/build dispatch. Shared C
list body binds count/list/group/rank to checked guest accesses and candidates/
matching to actual frame+10/+20 RAM. Calls checked-RAM eligibility. Writes
saved registers in retail order, reloads them from RAM on success, preserves
untouched gaps. Defined outputs v0/sp/s0-s4/ra; caller-saved residue and
instruction count deliberately NOT modeled. Exact list still115/218.

Guest-frame fixture first failed link on missing native helper (ha2Ba9 RED).
Initial compile exposed local eligible name shadowing helper; renamed helper
read_eligibility without changing shared body. Aj9XNn final terminal0:
O0/O2/UBSan native vs retail complete2MiB RAM and defined-register parity for
six matrix stack aliases, previous split-memory controls still differ as
expected. Invalidmatrix failure retains savedRA, cleared list/count, decremented
SP and originalv0, returning-1; nullRAM/CPU rejected. SyntaxPASS.

This proves only bounded matrix->frame aliases, not general alias handling,
all failure-prefix equivalence, access traces, ABI or identity/dispatch safety.
No natural runtime proof. In particular source-level bindings can produce
different read schedules than retail; this remains an integration gate.

Fresh read-only adversarial reviewer Banach 01a09ed8-8c39-71b1-8f01-2b1b4d1023c3
is checking helper/contract/tests. Reconcile its result next; do not duplicate
the reviewer. Cross-model skipped autonomous context, no externalCLI models.
Next: review fixes and broader packed-list differential/alias/failure tests;
do not enable runtime dispatch. No commit/push/runtime start; full goal active.

### List failure-prefix mismatch reproduced and repaired

Banach review found valid/actionable eager scratch-validation ordering bug:
incomingSP80000020 gives frame7FFFFFD0; six saves valid, but candidate scratch
7FFFFFE0 invalid. Retail clears count first; native formerly validated both
arrays before count reset. Strict-bus test executed the counterexample.
Initial LdgtxR test incorrectly compared postfault cpu.pc to executingPC;
adapter advances PC, while error records actualfault. Corrected observation:
TyjDOG retailcount0/write8fault7FFFFFE0, nativecountA5 assertionFAIL (RED).

Added shared XBT_LIST_CANDIDATE/MATCHING lvalue bindings; defaults retain
array expressions. Native uses checked per-byte accesses, no eager scratch
range construction. 3U5sSa O0/O2/UBSan PASS fullRAM retail/native failure
prefix, all six successful alias cases, defined restoration and retained
invalidmatrix failure writes. No preselection restriction substituted for fix.

iDMgk6 list unchanged115/218 and binarycmp CINtFD PASS; neighboring Attack/
selection exact gatesPASS. WxThz6 custombindings2048 eachPASS; Cawz3w legacy
list81920+chain163840 each and9mutantsPASS; IasJXC fullchain122880 each and
7mutantsPASS, including applicable expanded baseline. All runners terminal0
except honest list-matchFAIL. Shell syntaxPASS.

Second fresh reviewer Wegener 01a09edb-b221-7031-9f21-b66a7b6458ce checks
the bounded per-access/failure-prefix repair; result pending. First reviewer
Banach 01a09ed8-8c39-71b1-8f01-2b1b4d1023c3 completed, not yet closed.
Reconcile then close both; no duplicate review. Cross-model remains skipped
autonomous context. Broader packed-list branch/rank/alias coverage remains
next, alongside nonmatching115/218 and ABI/identity/liveadoption gates.
No commit/push/dispatch/runtime start; full goal active.

### Expanded packed-list differential and mutated-save restoration

Wegener review completed with no substantive issues in the bounded scratch
failure-prefix fix. Reconciled as no new actionable findings; two-cycle review
closed, Banach and Wegener closed. Prior exactness evidence remains bounded.

Expanded guest-frame suite with20480 native/retail full2MiB comparisons per
build: five actor arguments0/2/3/255/101,256 masks, four group/eligibility modes,
four rank patterns (ascending/descending/equal high/mixed high-low). Executes
real eligibility on both sides, no callee replacement. Includes presence,
block, normal/alternate status and matrix-denial paths, v0/sp/ra and s0-s7
comparison. 2uz2e9 first broader baseline terminal0 eachO0/O2/UBSan.

Added saved-register overlap case (incomingSP800C3EA8), where list output
overwrites saved s0/s1/s2. Native and retail must reload06050403/0A090807/
FFFFFFFF respectively. KZRbUL final terminal0: each20480 broadcasesPASS plus
all prior six aliases and failure-prefix gates. Three new controls rejected:
signed_rank, actor_zero, omit_restore. Intentional assertion aborts are the
negative-test outcome, not baseline failures. SyntaxPASS. No helper/source
algorithm edits this turn; fixture coverage strengthened from prior sixcases.

This still does not prove all alias/access-order/ABI cases, exact list match,
or live dispatch. Next work: packed selection frame/call composition and
its retail-backed differential gates, retaining115/218 list mismatch and
ordinary-dispatch prohibition. No commit/push/runtime start; full goal active.

### Packed selection composition and restored-found-state repair

Added battle_target_selection_ram.h and selection composition alongside list
RAM helper (shared checked access implementation, no runtime registration).
Retail initialowner read precedes saves; saved s1/ra/s0 written in order,
s1 carries actor stride, initialtarget loaded then selection prewrite; nested
list returnPC80084AB0 saved; postlist count/conditionaltarget snapshot and
fallbackowner reload; epilogue reloads selection savedregs. Source body remains
shared through no-op default hooks and STORE binding. Caller-saved residue,
v0 and instructioncount explicitly NOT modeled for this void helper.

ZQngdw missing helper link RED, then M930L4 first40960 combined directlist/
selection cases eachO0/O2/UBSan PASS and prior3controls rejected. yfqAv0 added
actual save-overwrites-context-owner alias cases (SP800C3EB8), nestedRA and
staleowner controls: all40960each and5controlsPASS. These early runs did not
cover restoreds0 composition defect below and do not supersede it.

Ptolemy review found valid/actionable composition bug: list saveds0 can alias
list bytes, but selection used hostfound0. JLPatD executed counterexample:
selectionSP800C3ED0,actor0,emptylist,target7; retail skipsfallback after restored
s0FFFFFFFF, native wroteFF (RED). Added XBT_SELECTION_FOUND/LOCALS bindings;
native uses unsigned cpu.gpr16 directly (including wrapping increment), legacy
retains s32found0. 76Oo9E each40960+5controlsPASS. BQg5II exactselection49/49,
eligibility69/69,Attack111/111+8/8 PASS; 3zD0eh custombindings34816eachPASS;
jMNFaG legacyselection69632each plus6mutants terminal0.

Locke second review found no fault in fix, noted match-time wrapping coverage
missing. Added found_wrap case: selectionSP800C3ED8,actor3, nested saveds0
list8..11=FFFFFFFF, nestedRA replacesowner with80084AB0, target84BAC=0 matches,
increment wraps to0 and fallbackwrites2 at84D98. Added nonwrapping_found
control. Final run tjsEmX session24771 is ongoing; O0 completed40960+framegates,
do not restart merely for a polling timeout. Final narrow reviewer Kuhn
01a09ee7-d639-79a3-8084-bca29b687175 checks wrap fixture/control; pending.
Earlier reviewers Ptolemy01a09ee3-5f6b-7302-8bf8-d72e4540a4b0 and
Locke01a09ee5-cdbe-7523-9498-3510f08cb55f completed, not yet closed.
Reconcile finalreview and close allthree; max3cycles. Cross-model skipped
autonomous context. No dispatch/commit/push/runtime start; full goal active.

Kuhn final wrap-case review completed: no faults; confirmed alias arithmetic,
list[2,0,1], target at80084BAC holding0, and fallback write2 at80084D98.
Reconciled as no new actionable findings, stopped after3cycles, allthree
reviewers closed. This review is static, separate from execution evidence.

tjsEmX final session24771 terminal0: O0/O2/UBSan each40960 combined fullRAM
casesPASS, all earlier frame/failure gatesPASS, all6mutants rejected including
nonwrapping_found. Logs rechecked for allthree build passes. This supersedes
pending-run note above. Packedselection/list helpers remain isolated; no
claim of all-address/access-order/ABI parity or instruction matching. Legacy
selection exact49/49 retained; list115/218 still blocks ordinary dispatch.

### List exact-match rank-expression exclusions

Returned to the unresolved instruction gate after isolated RAM composition.
Compared retail middle phases with generated GCC2.7.2 output: generated rank
addressing retains a low-byte mask absent from retail; partition/append also
reuse loaded candidate values where retail reloads. No fake volatile/barrier,
compiler flag change or generated-instruction patch used to force a result.

Three bounded source experiments: PTy9zy explicit unsigned rank index stayed
115/218; j4Pkwy byte-offset rank addressing regressed108/218 (rankphase12/83,
same368-byte size and11C prefix); V8vnMq signed swap temporary stayed115/218.
Unsigned-index and signed-temp list binaries cmp oldbaselinePASS unchanged.
All experimental forms reverted. VMKctL restored list binary cmp iDMgk6 PASS,
still115/218, phases71/71+14/53+19/83+11/11. Attack111/111+8/8 and selection
49/49 exact gates remainPASS. No production/source change retained; no new
behavioral rerun needed for identical restored implementation. These are
excluded hypotheses, not improved matching or native-adoption acceptance.
No dispatch/commit/push/runtime start; full goal remains active.

### New exact status-filter leaf 80084108

Inspected retail84108 and its two direct calls from84548. Replaced only the
84108 INCLUDE_ASM with C: low-byte target presence/block checks, alternate
status selection, secondargument low-byte status bypass. The bypass does not
bypass presence/block. No matrix access in this helper. No caller adoption.

RfSvyZ test first failed link on missing native function (RED). I4zMbb exact
gate PASS54/54, entry80084108 extentD8, full payload pinned. Added isolated
.status_filter section/retail placement to both match linkers; section-only
wrapping leaves compiler instructions untouched and preserves existing gates.
Wdlkps eligibility69/69 PASS; Attack111/111+8/8 and selection49/49 PASS;
iNM2EX list stillFAIL115/218 size368 prefix11C, unchanged phases.

MiDnL5 final behavioral runner: O0/O2/UBSan and expanded O2 baseline each
532480 cases PASS. All65536 statuses on both paths with flags0/1/100/FFFFFFFF,
plus all256 targetbytes with dirtyupperarguments and presence/block/alternate
combinations. Retail data-read addresses/order/width verified, retail writes
forbidden, native result and selected input preservation checked. All5mutants
rejected: flag_mask, status_mask, status_path, block_gate, target_mask.
Earlier oSiOdc/8QiA3F runs superseded by final expanded-baseline runner.
This is one additional exact decompiled leaf, not full overlay/84548 or
runtime acceptance. Fullrealchain mMyVgx terminal0: each122880 cases and
all7mutants PASS, including expanded O2 baseline. Syntax/scoped whitespacePASS.
No dispatch/commit/push/runtime start; full goal active. Next can trace84548
using both now-exact predicates without declaring841E0 complete.

### Retail84548 range/accumulator contract and mode-domain boundary

Inspected all130 retail instructions and direct caller84DE4 setup. Added
BATTLE_TARGET_RANGES_CONTRACT.md and retail-only executable fixture/runner.
Mode0 selects3..10; mode1 selects0..2; mode2 concatenates both, order selected
by arg2 lowbyte. Arg1 lowbyte goes to real84108, accepted targets call real
89C08 (halfword table lookup, NOT an assumed bit shift), accumulate OR mask.
No nativecallee replacements or production implementation introduced.

MCpC03 terminal0 O0/O2/UBSan each49152 cases: all2048 presence patterns,
three supported modes, both order choices, four flag words. Assert descending
12byteFFclear, count/mask reset order, accepted candidate writes, accumulator
halfword then count byte updates, final list/mask/count/return and SP/RA/s4
restoration. Realcallee instruction payloads pinned through fullbattlehash.

Executed out-of-domain counterexample: mode3 same arguments/a3=3 with incoming
s4=0 vs1 yields counts0 vs1(candidate3). This is explicit synthetic contract
evidence, not planted state for natural gameplay acceptance. No fabricated
default initialization or unsafe host uninitialized reads adopted. Inspected
direct caller supplies0..2; full caller-domain/adoption proof remains open.
Next implementation must resolve this boundary explicitly and preserve guest
execution for unsupported cases. No dispatch/commit/push/runtime start;
full goal active, neighboring841E0 still115/218.

### Resume: retail84750 alternate actor-range contract

Rechecked branch experiment/worldmap-open-gates-20260823 and HEAD f67fe692;
preserved the large dirty tree and existing Xvfb. No game runtime or active
battle-test writer observed during this check. Inspected84750/84854 retail
assembly and caller84DE4. Selected84750 for the next bounded contract because
its eight-candidate range3..10 is initialized internally, unlike84548's
unsupported-mode inherited-register path. Left both production INCLUDE_ASMs.

Extended existing retail ranges fixture to execute84750 with real83FF4 and
89C08: 10240 actor/mask/filter cases, asymmetric matrix, arbitrary table bits,
descending clear and accumulator write order, complete list and s0-s7/SP/RA
checks. Strengthened existing84548 runs with s0-s7 preservation assertions.
12jMvf terminal0: O0/O2/UBSan each49152 existing cases plus10240 actor-range
cases PASS, mode3 counterexample retained. Fullbattle SHA pin unchanged.
Documented evidence and limitations in BATTLE_TARGET_RANGES_CONTRACT.md.
No native differential/negative controls/alias coverage claimed for84750 yet.
Next: test-first C replacement and isolated65-word exact gate for84750;
84854's angle/distance scan remains separate and untouched. No dispatch,
commit, push, runtime start, or full-game acceptance. Goal remains active.

### Native84750 behavior pass; exact gate remains51/65

Rechecked HEAD f67fe692 and scoped dirty files. Added actual C84750 with
actor83FF4 and table89C08 callees. v2tZKC native test first failed link for
missing84750 (RED). Wf6uLW initial native differential passed; final nmilv1
O0/O2/UBSan and separately compiled expanded O2 baseline eachPASS49152
retail84548 cases +10240 retail/native84750 cases. All5mutants rejected by
output assertions: actor_zero, range_zero, eligibility_zero, mask_replace,
clear_short. Both native callees are real source bodies, not test stubs.
Native output comparison does not prove native access order/guest-stack alias.

Added .actor_range section-only extraction and retail placement in both
isolated linkers, D_C3D64 and89C08 symbol addresses, new exact actor-range gate.
First gate zIVKyq correctly rejected an accidentally altered template SHA;
repaired to original fullretail pin before any instruction comparison.
k3wEEz indexed clear5/65 size100; aNGpZO postdecrement52/65 size104 REJECTED
because it forms a host pointer before the array. LWGBGG safe predecrement
50/65; cEY9CJ retained safe clear[-1] then decrement51/65 size104. Pointer
starts Dlist+12 and finishes Dlist, never before it. xl2Utg earlyi initialization
also51/65 without improvement, reverted. Restored source cpp cmp nmilv1
expanded baseline PASS. Exact failures remain honest; no instruction patches.

DAX0Vh statusfilter54/54, TnA71q eligibility69/69, attack111/111+table8/8,
selection49/49 allPASS. tW90aE list unchangedFAIL115/218 size368 prefix11C.
eFNqRM full target chain terminal0: O0/O2/UBSan+expandedO2 each122880 cases,
all7mutantsPASS. Scoped whitespace and shellsyntaxPASS. Darwin independent
read-only review found no actionable issues under the stated nonaliasing
contract; main agent executed gates. Reviewer closed, cross-model explicitly
skipped for autonomous context. Updated range contract with native evidence
and exact/access-order limitations. No dispatch, commit, push or runtime start.
Next: source-faithful84750 clear/setup/register allocation work toward65/65,
without restoring before-array pointer UB;84548 domain and841E0 gap remain.
Full game port/decompilation goal remains active, not achieved.

### 84750 pointer-width cursor55/65 and high-address negative control

Rechecked HEAD f67fe692 and current84750/hand-off. Replaced clear[-1] pointer
loop with uintptr_t numeric cursor initialized Dlist+11; only valid store
addresses become pointers, final unused decrement remains integer. Existing
types.h defines PSX uintptr_t; added conditional native stdint.h include after
nOZS3M exposed its absence. Explicit fillFF and i11 initialization brings
retail setup closer. WvPoDj exactFAIL55/65 size104; all10 remaining differences
are s1/s2 allocation (including corresponding save stores). No forcedregisters,
compilerflag/instruction edits, or undefined before-array pointer retained.

Excluded experiments: UnUG9r unsignedlong cursor52/65; 6wqoS4 uintptr/fill
initialization55/65; rQC1l1 split s32target/u8index51/65; OhfGPe widened
target3/65 sizeF8; NPWvT0 pre-call i++19/65 size108; pESVwQ unsignedi,
Aevz93 hoistedtarget and A2dGHf registerhint all55/65 without improvement,
reverted to signedi and localu8target. J3a8x9 was a rejected C89 declaration
placement before the corrected Aevz93 compile. No out-of-scope edits retained.

Huygens review found low-address-only native fixture could hide cursor
truncation. Actioned PIE build asserting Dlist>UINT32_MAX and cursor_narrow
u32 mutant, with independently executed expanded PIE baseline. Russell review
then found address-only SIGSEGV acceptance too broad. Actioned arming only
around actualnativecall plus ucontext write/nonfetch check and exact pinned
clear-loop PC/bytes for Linuxx86-64 testcontrol. No production instruction
patch; unsupported compiler instruction shape fails the control gate.

HydPdW terminal0 supersedes OHX7qV: O0/O2/UBSan/PIE + expanded O2/PIE each
49152 retail84548 and10240 retail/native84750 casesPASS; all5outputmutants
rejected plus cursor-width mutant rejected at exact expected write fault
(exit79; unexpected faults80). NUs6GT earlier ordinarybuilds alsoPASS.
kkI2YU status54/54 and3o0rRW eligibility69/69 plusattack111/111+8/8 and
selection49/49PASS. 9Eoa39 fullchain terminal0 each122880cases/all7mutantsPASS.
Scoped whitespace/shellsyntaxPASS. Nietzsche final third-cycle read-only
review found no actionable findings; all3reviewers closed. Cross-model skipped
explicitly for autonomous context. No dispatch/commit/push/runtime start;
full goal active. Next remaining
exact84750 difference is s1/s2allocation;84548mode-domain and841E0gap remain.

### 84750 increment-placement improvement62/65

Rechecked HEAD f67fe692 and active84750 body. Moving local i++ after the
accepted-target block resolves all saved-register allocation differences.
zysMf6 and retained LfBw4O exactFAIL62/65, size104. Remaining offsets084,
0CC,0D0 differ only in increment delay-slot placement/loop target: retail
increments in eligibility branch delay, candidate increments at loop backedge
and reloads actor inside loop. No generated instruction edits or forcedflags.

Rejected experiments: RyX0TX duplicated branch increments31/65 size110;
mylSXJ deriving storedtarget from i-1 gave4/65 sizeFC; eBD7hA predecrement
count condition also62/65 but no improvement; dF36is earlyincrement withthat
condition55/65; gqIyMx u8eligible55/65. Kept original countdown setup/types
and only moved i++ to end of body. Local index address never escapes either
callee, which still receives the saved targetbyte. No predicate change.

aGBC1F terminal0: O0/O2/UBSan/PIE plus expanded O2/PIE each49152 retailrange
and10240 realnative/retail actor casesPASS, all5outputmutants and precise
cursor-width negative control rejected. IYv4va status54/54, AeKrFS eligibility
69/69, attack111/111+8/8 andselection49/49PASS. Fullchain njSGDU terminal0:
O0/O2/UBSan+expandedO2 each122880cases/all7mutantsPASS. Scoped whitespace
checkPASS. No dispatch/commit/push/runtime start. Goal remains active;
remaining exact gap is increment scheduling, not register allocation.

### 84750 exclusions and retail84854 directional contract

Rechecked HEAD f67fe692. Three84750 alternatives regressed: xRq8ky folded
output++ append14/65 size100; maLkPM post-call nextindex temporary52/65
size104; ThX0Sp pre-call nextindex11/65 size108. Restored62/65 source;
current preprocessed source cmp aGBC1F baselinePASS, no84750change retained.

Inspected full84854 and actual ratan2 at8004B32C plus retail table80057030.
New BATTLE_TARGET_DIRECTION_CONTRACT.md and retailfixture/runner establish
the next function without replacingcallees. FullSLUS hash pin
dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119;
loads PSX payload80010000/49800 and isolated battle84854/228. Standard
fullbattlehash pin retained. Real ratan2 executes, no bridge/callee stubs.

P4yb40 final terminal0 supersedesMGxpU8: O0/O2/UBSan each8712casesPASS;
three actors0/3/255 dirtyupperwords, three origins, eleven coordinatevalues
peraxis, eightdirectionwords. Includes FFholes, selfskip, listslot10visited,
slot11excluded, countignored, unsigned16coordinates, signedwrappedlow32
distance, strict firsttie. Checks all11list slots, three actual ratan2 calls
and returns against independent integer/LUT expression, only8stackwordwrites,
and s0-s7/SP/RA preservation. Perbuild explicitwitnesses858negative-score
winners,522retainedties,321direction2inclusive upperboundaryhits. Initial
aLzJEm C hex-token spacing and bzQtQ2 host PIE-link mismatch corrected before
execution; neither was a production change or behavioral RED proof.

This is bounded retailcontract evidence, not nativeC/differential/negative
controls, exhaustiveatan/domain coverage, guestalias or gameplay acceptance.
84854 remains INCLUDE_ASM. Scoped whitespace and shellsyntaxPASS. Focused
review pending at this entry; cross-model explicitly skipped for autonomous
context. No dispatch/commit/push/runtime start; full goal remains active.

Godel review found slot10 could only tie an earliercandidate in grid fixtures,
so merely visiting/callingatan did not prove it could win. Actioned extra
directedcase: actor(0,0), earliercandidate(0,20), slot10target7(0,1), excluded
slot11target8(0,0); explicitmodel+retail requiretarget7. qKdykB terminal0
O0/O2/UBSan each8713casesPASS, originalwitnesscounts unchanged858/522/321.
Supersedes P4yb40. Volta follow-up read-only review found no actionable
contract/model errors. Bothreviewersclosed; mainagent executed all gates.
Next: test-first84854 C replacement and 138-word exact gate, retaining
explicit unsigned-wrapping distance semantics and current84750gap.

### C84854 hybrid differential and exact73/138 frontier

Rechecked HEAD f67fe692. WXti2n native test first failed link on missing84854
(RED). Added C84854: low-byte actor/direction, eleven slots with FF/selfskip,
unsigned16coordinates, exactdirectionboundaries, unsigned32products/sum and
supportedcompiler signedbit interpretation for strictbest. Uses existing
Row310 moved earlier, newD_C3EC0 declaration and PsyQ longratan2 signature.
No adjacent85310body change. Native test ratan2 wrapper executes actualretail
callee with its own CPU, verifyingarguments/returns; HYBRID, not fullnativeatan.

a6Lhvh initial O0/O2/UBSan hybrid8713casesPASS. B8cyPR wideproducts29/138
size230; CHEKwN unsigned32products same29/138 size230, preferred defined
modulararithmetic. iz5akk ternaryproducts28/138 reverted. Signed direction
and intptr_t listcomparison yield8n1SQG FAIL73/138 size23C vsretail228.
Remainingframe40vs30 hex, squared-distance stackspills andsetup/operandreuse.
No instructionpatches orcompilerflagchanges. New .direction primarylink84854;
secondary selectionlink parks88000 to avoid relocatedattacktext. Onlyprimary
directiongate claims retailplacement, neither full-overlay linking/adoption.

3jiQZF O0/O2/UBSan+expandedO2 each8713PASS and8mutantsrejected. Aristotle
review identified missing opaque-row inputpreservation. Reproduced eRbLgO
padding_write mutantSURVIVED (RED); added deterministic fullrow initialization,
wholearray snapshots/comparisons. th3qHn terminal0 final each8713PASS, all9
mutants rejected: actor_mask,direction_mask,mode2_strict,unsigned_distance,
ties,last_slot,self_skip,signed_product,padding_write. Signedproduct requires
UBSan overflowdiagnostic; othermutants require expectedassertions. Bohr followup
found no actionable issues underboundednonaliasingcontract. Bothreviewersclosed;
cross-model explicitly skipped for autonomous context. Mainagent executedgates.

a9tf7R status54/54, csSdwu eligibility69/69, attack111/111+8/8 andselection
49/49PASS. 8zOeRk actor-range still62/65,size104 withsame3differences. List
gate remains115/218,size368,prefix11C, phases71/71+14/53+19/83+11/11.
BLuaSz fullchain terminal0: O0/O2/UBSan+expandedO2 each122880/all7mutantsPASS.
Shellsyntax/scopedwhitespacePASS. Updated directioncontract. No dispatch,
commit,push orruntime start. Next exactwork:84854 squared-distance spills/setup,
while84750scheduling,841E0matching,84548domain and fullnativeatan remainopen.
Full port/decompilation objective remains active, not achieved.

### 84854 spill diagnosis and82/138 shape improvement

Rechecked HEAD f67fe692/currentbody. Registerhint6Gr3Uv made no improvement
(73/138,size23C), reverted. Ran originalcc1 diagnostic dumps in owned
scratchpad/battle-direction-match.8n1SQG: .rtl multiply outputs pseudos167/168
cross branch joins; .greg reports LO/MD register constraints and spills
(168 explicitly goes on stack), matching the extra16-byte frame/storage.
This diagnosed allocator behavior rather than changing compilerflags to ship.

Changed source to absolute difference then one unsignedsquare peraxis:
oyN8TP71/138,size218, but frame30 restored. Precomputed actor_offset from
maskedactor*sizeof(Row310), using aligned byte-offset actor row accesses,
matchesretailsetup and yields7aLyBo82/138,size218. Reverse-field subtraction
kgBRRt76/138,size214 regressed, reverted. Final0fymkx FAIL82/138,size218 vs228;
frame30 correct, setup matches through053. Remainingcode cacheslist/coordinate
loads and schedules/reusesdistance operands differently. No generatedinstruction
patches, productioncompilerflagchanges or signedoverflowintroduced.

CP3gFx terminal0: O0/O2/UBSan+expandedO2 each8713hybridcasesPASS, all9mutants
rejected including UBSan signedproduct and whole-rowpaddingwrite. CRCsJL
status54/54,7GDhVZ eligibility69/69,attack111/111+8/8 andselection49/49PASS.
Fullchain bBtkIW terminal0: O0/O2/UBSan+expandedO2 each122880/all7mutantsPASS.
ScopedwhitespacePASS. Kant independentread-only review found
no actionable source/ABI/test issues underboundednonaliasingPSX32/LinuxLP64
contract; reviewerclosed, cross-model explicitly skipped forautonomouscontext.
Updated directionalcontract. No dispatch/commit/push/runtime start; fullgoal
remains active and incomplete, including exact84854/nativeatan/otherbattle gaps.

### Actual native atan coverage for84854; API type boundary corrected

Rechecked HEAD f67fe692. NyvRMf one unsigneddistance accumulator andym7PdQ
signedaccumulator with explicitu32sum both emitted unchanged82/138,size218;
reverted to verifiedtwo-square body and cppcmp CP3gFx baselinePASS before
native API changes. These are excluded exact hypotheses, not improvements.

Inspected actual portdependency pc_port/extern/PsyCross: HEADf4da486aa53565a4a8efa439c9671bd35dc94eda,
selected LIBGTE.C/ratan_tbl.h clean. Existing ratan2 is integer/table based,
not libm, with native int(int,int) API. Retail SDK uses long(long,long) on
PSX32. Added conditional prototype inmain35 and made testobserver int-based;
no vendor or unrelatedcaller edits. TargetC implementation otherwise unchanged.

Runner compiles actualvendor LIBGTE.C using existing GTE-testbuild conventions,
test-only ratan2 symbolrename for thinobserver. Nativebody/table SHA recorded
in each evidence directory; all1025actualnative tableentries comparedretail.
Actualnativewrapper validatesargs/results without emulator calls. Retained
hybridmode independently. IK7drJ initial bothpaths O0/O2/UBSanPASS8713cases.
NQlCgv final terminal0: bothpaths O0/O2/UBSan plus expandedO2baselines each
8713PASS; all9mutants rejected on bothpaths (18controls), including UBSan
overflow andcomplete-rowpaddingwrite. This closes sampled nativecallee coverage
for the picker's bounded +/-65535 differences, not unrestricted ratan2/INT_MIN.

Directionexact gate stillFAIL82/138,size218; one display pipeline initially
masked outerexitstatus, then rechecked with pipefail and observedterminal1.
No exactPASS claimed. Fullchain UtPFaU terminal0: each122880cases andall7
mutantsPASS atO0/O2/UBSan+expandedO2. Pauli read-only
review found no actionable ABI/test/provenance issues in bounded nativepath;
reviewerclosed. Cross-model explicitly skipped forautonomouscontext. Updated
directioncontract, shellsyntax/scopedwhitespacePASS. No runtime dispatch,
commit,push orvendorwrites; fullport/decomp goal remains active and incomplete.

### Native atan quotient-boundary differential coverage

Rechecked branch experiment/worldmap-open-gates-20260823 and HEADf67fe692;
preserved existing dirty work. Added battle_target_atan_retail_test.c and
integrated it into the direction runner. EHD9Zc terminal0:303121 executions
per O0/O2/UBSan compare actual native atan against actual retail instructions.
Thirteen representative denominators, quotient transition neighbors, swapped
axes and all signs exercise all1025 table entries and both axis branches.
Counts include duplicates, not an exhaustive input-pair domain. Retail writes
are forbidden; preserved registers and native-table nonmutation checked.
Native last-entry table corruption is caught at the result assertion, not
merely initial table identity. All existing8713-case picker positives and18
paired mutation controls pass. ShellsyntaxPASS. No production/vendor edits,
dispatch activation, runtime start, commit or push. Exact84854 remains82/138
from the preceding gate; not rerun or improved by this test-only extension.
Full port/decompilation goal remains active and incomplete.

Hypatia read-only review found the printed execution count was not enforced;
added assert(cases==303121). Final SyjZy4 terminal0 reruns all three builds,
paired expanded baselines and all19 negative controls successfully. Reviewer
closed; cross-model review explicitly skipped for autonomous context. This
count guard was the only review finding; no acceptance boundary changed.

### Direction picker improves82 to136/138 without host UB

Rechecked branch/HEADf67fe692 and live processes. Bnlzv9 builtin-abs trial
regressed76/138,size20C;1g9CTz separate branch products80/138,size23C recreated
spills but restored list reloads. Restored kKc2cT82/138 before next hypothesis.
Branch-local distance assignment then branch-local += second square yields
dN4uR9 128/138,size228. Reverse-field assignment to same delta d2lJdY126;
separate signed reverse temporary S6XevL136/138. Reusing angle as a shared
second-product temporary Twb9wY122, reverted. Final tkfSQk terminal1:
FAIL136/138,size228, frame30; only offsets1C8/1CC differ, mflo/add temporary
a2 instead of retail v0. Attack111/111+8/8 andselection49/49 stillPASS.
No instruction patching, flag changes, volatile loads, or signed multiplication.

pB4dqk terminal0: both native-picker boundaries each8713 cases atO0/O2/UBSan
and expandedO2, standalone atan303121/build, all19 negative controls rejected.
Updated directioncontract; remaining exact2words are NOT PASS. Runtime
dispatch/adoption and full port/decompilation acceptance remain open.

Fullchain ztKq5l terminal0:122880 cases per O0/O2/UBSan+expandedO2 andall7
mutantsPASS. Carson independent read-only review found no actionable issues
under the bounded/nonaliasing contract; reviewerclosed. Cross-model review
explicitly skipped for autonomous context. Scopedwhitespace/shellsyntaxPASS.
No commit, push, vendor writes orruntime start. Full goal remains active.

### Remaining84854 mismatch localized to reload scratch allocation

Rechecked branch/HEADf67fe692 and writer processes. Three equivalent trials
YBqnZp conditional-expression addition, yykDkE signed accumulator with explicit
unsigned wrapping, ZbwUmA reverse add operands remain136/138,size228.
CjqIVV square assignments back into input locals regress111/138,size234.
All reverted; JCY5Xl terminal1 FAIL136/138,size228, exact same two offsets.
cmp preprocessed source and direction.bin against tkfSQk bothPASS. Therefore
previous pB4dqk/ztKq5l test evidence applies to unchanged production source;
no additional native run claimed. Attack111/111+8/8, selection49/49 rerunPASS.

Generated originalcc1 -da diagnostic dumps only in owned tkfSQk evidence dir.
Second-axis product pseudos205/206 allocatedLO65; .greg requestsGR_REGS for
add347 and inserts423/426 LO-to-a2 reloads. This is the origin of final1C8/1CC
register mismatch; source arithmetic is not the remaining discrepancy.
Next exact work must address that reload allocation through justified source
shape, not instruction patches or forced registers. Updated directioncontract.
No production changes retained, runtime starts, vendor writes, commits orpush.
Full port/decompilation goal remains active, not achieved.

### 84548 direct caller prefix verified across flag domain

Rechecked branch/HEADf67fe692 and processes. Actor84750 post-call increment
trial EBg4tI55/65; explicit fulltarget/byte split9qUHKr51/65. Both reverted.
to1uOs terminal1 confirms62/65 with original3offsets; no production changes
retained. Direction136/138 body untouched.

Pinned battle+EXE aligned-word census for directJ/JAL84548 and literal
80084548/A0084548/00084548 finds only battleoffset153D4 JAL0C021152
(80084EC4). No executable matches. This is NOT an indirect/computed-address
or other-overlay census; unsupported84548 inherited-register paths stayopen.

Added battle_target_range_caller_test.c and standalone runner. Executes real
84DE4 prefix through delay slot, halts before selected callee. All65536flags,
complementary inputflags,5controlbytes withdirtyupperbits and3alternatebytes
give983040cases/build:327680range,655360actor calls. Checks exactpassedargs,
mode0..2, actorbyte, returnaddresses, stackadjustment andboundedstackwrites.
Isolated mode2-to3 instruction-copy mutant must hitmodeassertion.

UjnFfi O0/O2 passed but cc UBSan link failed missinglibubsan.so.1.0.0;
yta5nQ splitbuild exposed defaultPIE incompatibility. Used existing battle
convention ccobjects/clang-no-pie link. FinalDz1PDc terminal0 O0/O2/UBSan
each983040PASS, modewitnesses49152/49152/229376, mutantrejected. These failed
builds are not passes. Dalton read-only review foundno concrete issues under
boundedprefixcontract; reviewerclosed. Cross-model review skipped for
autonomouscontext. Updated rangescontract. No calleeimplementation, dispatch,
runtime start, vendor writes, commit orpush; fullgoal remainsactive.

### Retail interactive targeting controller84B40 contract

Rechecked HEADf67fe692 andwriterprocesses. Inspected actual84B40 body
offset15050,size1E8 andjumptable offset720,32bytes (80070210). Events0..3
call84854;4/6/7 confirm,5cancel,8wait,9..255ignore/reset. Selectioncaptured
once fromowner+2E8; confirm/currenttarget andcancelreloads crosscallback
contextidentity boundaries. Added BATTLE_TARGET_UI_CONTRACT.md withexact
callbacksequence andscope; Cbody remainsINCLUDE_ASM, no runtimeadoption.

Added battle_target_ui_contract_test.c andrunner. Realretailbody/table with
all5 externalfunctions explicitlysimulated:89C08,BC404,BCD98,716D8,84854.
6144initialcases rJezVs O0/O2/UBSanPASS+cancelmutant;7BaKN1 initialfailure
wasfixture mistakenly asserting t8/t9 preservation despitecallbackclobbers,
fixed toABI-preserveds0..s7/fp/SP/RA. No realcallee or gameplayclaim.

Hubble reviewfound even8swaps hidstale-confirm-owner and finalsnapshots hid
prematuretargetwrites. Added odd7-eventstream,12288cases, targetwrite timing,
count,value,currentowner assertions. Addedstaleowner instructioncopymutant
andearlywrite harnessfault. jiUfDA positive/twocontrolsPASS butthirdcontrol
Werrorparentheses compilefailure, corrected. Final04jgq1 terminal0 each
O0/O2/UBSan12288PASS andall3controlsrejected. Source/rawretailunchanged;
no commit,push,vendorwrites orruntime starts. Nextstep is sourceC controller
against this explicitcallbackcontract, then realcalleeintegration/exactness.
Fullport/decompgoal remainsactive andincomplete.

Hubble follow-upfound no remaining actionable issues under explicitcallback
contract; reviewerclosed. Mainagent ranfinaltests. Cross-model reviewwas
explicitlyskipped forautonomouscontext; scopedwhitespace/shellsyntaxPASS.

### Shared C84B40 targeting controller implemented

Rechecked HEADf67fe692/status. Added native fixture boundary, observedmissing
func_80084B40 linkRED, then implemented target_ui_impl.inc sharedby main35
andnativefixture. Preserves selectedcapture, unboundedwait, byte/maskhandling,
callbackorder, contextreloads andresult. All5callees remainexplicitlysimulated
here; realcallee/nativeadoption andexactstoretrace areNOTPROVEN.

l7jgCR bothpaths12288 atO0/O2/UBSanPASS+old3controls. Addednativeexpanded
baseline/mutations. WHoQ86 positivesPASS butbaselinefilenamecollisiontruncated
generatedinput andlinkfailedmissingmain; corrected distinctnative-expanded
input. egqpMe terminal0 bothpaths/builds+expandedO2 and5native+3oldcontrols.
Bacon reviewprompted callbackentryevent andthenownerassertions plusmutants.
FinalQALoCB terminal0 each12288 O0/O2/UBSan+expandedO2,6nativecontrols and3
retail/harnesscontrolsrejected. Finalreviewverifiedentryownerfix; reviewerclosed.
Cross-model reviewskipped forautonomouscontext. Lookup-owner mutationdeclined
withmain41.c16-18 read-onlytablebody evidence andexplicitcontractlimit. Native
snapshots do not proveexact/duplicate/samevaluestorecounts; docsstateboundary.

AddedisolatedUItext/table sectionsandgate. QSTbOc firstexistinggatefailed
missingnewcallbacksymbols; addedretailaddressassignments andsectionrouting,
not instructionrewrites. PrimaryUI84B40/table70210; secondaryparksUI89000
andcannotproveUIplacement. 1oGBhQ106/122,size1E8,table8/8. WiderlookupAPI
trialxnCxr3 113/122 wasreverted becauseactualmain41definesu16(u8), notu32(u32).
FinalUtoMm5 terminal1 FAIL106/122,size1E8/table8/8. Existingattack111/111+8/8,
selection49/49PASS. n0K2P6direction136/138 andmJDzTLactor62/65 unchangedFAIL.
BC404 mainc114 pointerparameter vsretailmaskcall remainsknownpreexisting
full-native-callee gate; no unrelatedcallee repair made.

RegressionlKoXoQ terminal0 direction8713/path/build,atan303121/build,all19
controlsPASS. Fullchaink6sI2r terminal0 each122880/all7controlsPASS.
UpdatedUIcontract/status; scopedwhitespace/shellsyntaxPASS. No runtime
dispatchactivation, vendorwrites, commit orpush. Fullgoal remainsactive.

### UI controller improves106 to115/122 with correct lookup ABI

Rechecked HEADf67fe692/status/writers. 3v62RO waitingvariable throughout
loop12/122,size1D8; IOs9Qs waitingvariableonlyfinalstore107/122,size1E8.
u16masklocals dvNWop113/122 onthatshape. Removedwaitingtrial; explicitrow
locals hZrg5n111/122; freshscopeforsecondcancelrow SPnqdp115/122.
u8actor d7SaZI0/122,size1EC andexplicitif/do-wait O9ePs885/122 regressed,
reverted. FinalVDmYoa terminal1 FAIL115/122,size1E8,table8/8. Remaining
7words:setup1C..30 andactorargument10C. u16(u8)lookup declaration unchanged;
no incompatibleprototype, forcedregister orinstructionpatch introduced.

Retainedu16own/targetmask locals andexplicitrow+=offset withfreshcancel
rowpointer afterDISPLAY. Updatedstaleowner/entryownermutationexpressions
without removingnochangeguards. IMZOPL terminal0 each12288 native/retail
O0/O2/UBSan+expandedO2 andall9controlsPASS. Directiongate still136/138;
attack111/111+8/8 andselection49/49PASS throughisolatedlinks. Runtime/native
callee integration andexactnativewritecounts remainopen; no dispatchchange.

FullchainLwZ11z terminal0 each122880 cases/build andall7controlsPASS.
Laplace read-onlyreview foundno actionable semantic/UB/ABI regression under
statedvalidstorage/callbackcontract; reviewerclosed. Cross-model explicitly
skipped forautonomouscontext. UpdatedUIcontract; scopedwhitespace/shellsyntax
PASS. No vendorwrites, runtime starts, commit orpush; fullgoal remainsactive.

### BC404 scalar-mask ABI corrected against retail consumer

Rechecked HEADf67fe692/mainc114status/writers. RetailBC404(4C914,size50)
preservesa0 andforwardsall32bits toBC460. mainl115 retailBC460 storesargument
toC3678 atBC4BC andtests/shiftsbits fromBC4CC; no inputpointerdereference.
ForcedtestAPIheaderu32 caughtbothmainc114pointer declarations asREDcompile
conflicts. ChangedonlyBC404definition andBC460declaration tou32mask; body
logic unchanged. Main35caller also compilesagainstforcedheader, enforcedin
runner. BC460actualbody remainsASM; this is notfulldisplayimplementation.

Added battle_target_display_test.c/APIheader/runner andisolatedmatchgate+ld.
TLefvI positives O0/O2/UBSan each786432PASS. FinalwTDrcH terminal0 each
786432 plus expandedO2 and3nativecontrolsPASS:truncate_mask,recheck_flag,
stale_source. Domainall65536lowmasks x4upperpatterns xgate0/1/255; notall32bit
inputs. Actualnative/retailBC404, simulatedBC2F0/BC460 callbacks mutateflag
and source; exactargs/order pluspostcallbackreload/guardchecks. Native transient
storecount remainsunproven. Sagan read-onlyreview foundno concreteblocking
ABI/falsepassissue; reviewerclosed, cross-model explicitlyskipped.

YW2pMM exactgate terminal1 FAIL16/20,size50. Offset20 constant1 instruction
form,30/34/38 load/store registerchoices differ. Sizealone cannotPASS. No
instructions patched/flags changed. AddedBATTLE_TARGET_DISPLAY_CONTRACT;
updatedUIcontractto distinguish correctedmaskinterface from stillunproven
downstreamnative/renderingintegration. Scopedwhitespace/shellsyntaxPASS.
No runtimeactivation, vendorwrites, commit orpush; fullgoal remainsactive.

### UI composition now executes actualBC404 wrappers

Rechecked HEADf67fe692/scopedstatus. AddedXBT_UI_REAL_DISPLAY mode requiring
XBT_UI_NATIVE. RetailUI executesrealBC404(50bytesatBC404), neverbridged there;
nativeUI callsactualmainc114body renamedPcPortUiNativeDisplay onlyintestobject.
BC2F0/BC460 simulated: firstsetsflag255/sourcexor,secondconsumesexpected
displaymask andownerchange, restoresflag0 andsetsfinalsource. Actualwrapper
mustforwardmask andcopyhalfword. Checksentries/callbackorder,guestnestedstack
addresses anddestinationwritecounts, destination/sourceguards andUIstate.
Otherlookup/highlight/frame/picker remainexplicitlysimulated.

XGPo9D initialintegrated12288/build andold9controlsPASS. dJqPcu added3
integratedwrappermutants+baselinePASS. Deweyreviewfound nativepostcopy-source
corruption couldbeoverwritten later; addedimmediatewrapper-returnassertions
andcorrupt_source mutant. Reviewalsoflaggedmissingnonzero-entryintegration:
kept explicitUNPROVEN/nextcompositiongate, notclosedby standalonewTDrcH.
Currentintegrationentrygatealways0; callbackevolutionisscripted, notrenderproof.

FinalNssCPP terminal0 O0/O2/UBSan each12288 inbothcontrollerboundaries+
actualwrappermode, expandedbaselinesPASS. Fourintegratedwrappercontrols and
nineexistingcontrolsrejected(13total). Deweyfollowup verifiedsourcefix;
reviewerclosed, cross-model explicitlyskipped forautonomouscontext. Updated
UI/displaycontracts, scopedwhitespace/shellsyntaxPASS. No production edits,
runtimeactivation,vendorwrites,commit orpush. Fullgoal remainsactive.

### Nonzero BC404 entry gates covered in controller composition

Rechecked HEADf67fe692/currenttest. Integratedmode nowcrosses gate0/1/255
withcontrollercases=>36864/build; originalmodesstay12288. Nonzero entry
observation consumesexpecteddisplayrequest using a guestCPUcopy, discards
clobbers andexecutesallactualBC404instructions. Nativeobservation likewise
precedesactualwrapper. No BC2F0/BC460calls orownerchanges permitted onnonzero
gates; destinationcopystillrequired. Gateclassfixedpercase; arbitrarymixed
gateevolution/rendering notclaimed.

D1Sdt9 terminal0 positives+unconditional-callmutationPASS; sLpobL added
skip_nonzero_copycontrol withall15controlsPASS. Singer read-onlyreview found
no observation-as-replacementissue, butrequiredunconditionalcontrol'sexact
entry_gate==0 failureinstead ofbroadassertionregex. Tightenedper-control
patterns. aIbuVq rerunpositivePASS thenverifierfailed onrecheck_flag: its
actualexpectedfailure is sourcepreservation atwrapperreturn, notdestination.
Inspectedlogandcorrectedthat specificpattern; didnot countaIbuVq asPASS.
Reviewerclosed; cross-model explicitlyskipped forautonomouscontext.

Final3OCEiV terminal0 O0/O2/UBSan andexpandedO2 integrated36864 each,
originalcontroller12288 each, all15controlsrejected atrequiredassertions.
UpdatedUI/displaycontracts andhistoricalgate-limit labels. Scopedwhitespace
andshellsyntaxPASS. No production edits,runtimeactivation,vendorwrites,
commit orpush; fullport/decompilationgoal remainsactive andincomplete.

### BC404 expression excluded; BC2F0 callback ordering traced

Rechecked HEADf67fe692/currentbody. Explicitdestinationpointer lNn2vh still
16/20,size50; reverted. Final7SxnUj terminal1 same16/20and4offsets.
cmp preprocessedbody anddisplay.bin againstYW2pMM bothPASS, so no production
changesretained andno newnative rerunclaimed. Remainingconstant/register
differences not fixed byweakeningprototypes,volatile accesses orpatches.

Traced nextrealcalleeBC2F0 inretailmainc114.s (4C800,size108). Storesmode
C3CC0 andstate1C3CBC; mode4writesstate5; mode2copies6signedhalfwords to
two32bit vectorswithshift16 (native mustavoidnegative-signed-shiftUB).
Defaultincludingmode1 loadsC3680task, callswordcallbacktask+0C, thenunconditionally
clearsroot; onlythenreloadsC3684 andequivalentcall/clear. Callback-installed
ownroot isoverwritten, butreplacementsecondroot mustbeobserved. Added exact
orderingandnative-layout limits toDISPLAYcontract. Statictrace only, not
live-nulltask assumption orcallbackimplementation. Nextintegration work must
resolve taskhandle/callbackrepresentation, notsilentlyassume rootsareempty.
No runtimeactivation,vendorwrites,commit orpush; fullgoal remainsactive.

### Native BC2F0 implementation through explicit resolver boundary

Prior turn was progress: completed retail setup contract. Rechecked
HEADf67fe692 and mainc114 before continuing. Producer trace found native
game_overrides BC158 calls the retail dispatcher with raw task pointer;
run_guest_callback preserves it as a0 and retail BC158 stores it in roots.
Runtime resolve_memory supports KSEG0/KSEG1, shared main-exe data, and
validated low host mappings. Guest-only masking is therefore not valid.

Added pc_port/src/battle_target_setup.c/.h: real native setup logic using
caller-owned memory resolver and synchronous callback invoker, unchanged
task handles, retail load/store order, post-callback clearing/reload, and
defined unsigned vector shifts. Failure retains prior effects and forbids
retail retry. Not registered in runtime/build dispatch; mainc114 BC2F0 stays
INCLUDE_ASM. Guest stack/register residue and exact matching not claimed.

Extended contract test with KSEG0/KSEG1/mixed-low-host fixtures and native
body runs. Added failure test with35 operation tapes and3 invalid APIs.
Final J4ylaf terminal0:196776 cases per body/O0/O2/UBSan; combined native
runner reports393552 executions, including its retail baseline. All failure
tapes pass. Unchanged source baseline and native mask_task/wrong_clear/
narrow_mode mutants plus retail missing-clear control pass required gates.
Earlier UUmhoU/RZxzKx/NKG278 are intermediate evidence, not final coverage.

Hegel read-only review found no semantic issue, requested missing failure
coverage; added operation tapes afterward and ran them in final suite.
Reviewer closed; cross-model review skipped in autonomous context.
Updated display contract. Next: real resolver/callback composition and
guest-frame requirements before any runtime adoption, plus shared source
and exact decompilation work. No runtime activation, vendor writes, commit
or push; full port/decompilation goal remains active and incomplete.

### BC2F0 real resolver/dispatcher composition

Previous turn was progress (native body and contract gate). Rechecked HEAD
f67fe692 and relevant source status. Added isolated runtime-including setup
fixture and runner, no production edits. Actual runtime_read/write resolve
guest globals, native shared vectors, KSEG aliases and real low host task
storage. Actual PcPort_BattleMipsDispatchCallback executes retail BC3F8 as
a marker callback; fixture tasks are not allocator-owned free callbacks.

Final scratchpad/battle-target-setup-runtime.HRweiT terminal0:448 cases each
O0/O2/UBSan, seven modes/four root-presence combinations/four seeds/four
handle domains (KSEG0,KSEG1,host,mixed). Full guest RAM parity excludes only
two retail saved-register words; native frame untouched independently.
Per-call raw task/order, host task guards, seed-derived shared vector values,
padding/raw RAM guards, and parent CPU/bridge preservation pass. Leaf marker
does not test nested SP inheritance or guest-frame aliasing. Both argument
masking and raw-RAM shared-data bypass controls fail required assertions.
Source pins checked after all builds/controls; shell/whitespace checks pass.

oRoJ9G failed unsigned expected-count compilation; fixed without suppression.
NCE55X aborted in test pad hook, gdb traced unconditional runtime_bridge
poll; adopted existing inert hook convention. woX5nM passed336 initialcases;
0ApUrM passed448 before review fixes. Hilbert read-only review identified
stack exclusion/per-call argument/shared-oracle gaps (fixed) and nested-SP
limit (documented). Reviewer closed; cross-model review skipped autonomously.
Updated display contract. No runtime activation, vendor writes, commit or
push. Next remains true callback/frame composition and exact decompilation;
full port/decompilation goal is active and incomplete.

### Real BBEE0 cleanup/reentrant BC2F0 contract

Previous turn progressed real resolver composition. Rechecked HEADf67fe692.
Traced BC158's installed +0C free callback: BBEE0, not generic22EB8. Retail
BBEE0 clears matching root, restores saved vector halfwords only at gate0,
optionally1CE74 on spriteACbit5, then timer unlink/work unlink/HeapFree,
byte-counter decrement and BC2F0(C367C) re-entry if zero. Re-entry may process
the second root and changes final mode/state before outersetup returns.

Added battle_target_cleanup_retail_test.c and runner. Actual setup+BBEE0
execute; only main-exe unlink/free hooks simulated. Final evidence
scratchpad/battle-target-cleanup.H6kzSW terminal0:24576 cases/buildO0/O2/UBSan,
all256 counters x4root combinations x3gates x2ownershipbits x4recursive modes.
Twelve deep callbacks must use SP801FEFA0, others801FEFD0. Save-slot-only
stack stores, exact counts, unused-stack canaries, callback order/arguments,
restoration/gating, finalcounter/roots/mode/state/vectors andsavedregs pass.
Deliberately inconsistent counter/root combinations are arithmetic controls,
not gameplay reachability claims. Native dispatcher frameadoption NOT_RUN.

Three instruction-copy controls rejected: missing re-entry (stack-savecount),
missing callback rootclear (hookroot assertion), early setup beforeHeapFree
(phase/free-count assertion). Hooke read-only review found stackcanary and
free-before-reentry gaps; fixed and reran final suite. Reviewerclosed,
cross-model skipped autonomously. Intermediate5CZhCQ/yiSWgw/Wp6agA superseded.
Updated displaycontract; no production/runtime activation,vendorwrites,
commit orpush. Fullgoal remainsactive; nextnative composition must preserve
actual callback re-entry andguestframes, not justleaf marker behavior.

### Native BC2F0 guest-frame variant and recursive cleanup parity

Prior turn was progress (real cleanup/reentry contract); recheckedf67fe692.
Added PcPortBattleTargetSetupFrame to existing setup.c/.h, sharing the native
body with frameless API. Retail mode/state/save order, callbacka0/s0/SP/RA,
callback-returned s0 root clear, secondroot reset, andguestRA/s0 reloads are
modeled. Pipeline/instruction counts/othercaller-savedresidue excluded;
alignedSP>=18 andCPU separatefrombusmemory required. No runtime registration.

RED nativecleanup fixture initially failed missing API declaration, then
implemented framevariant. Actual BBEE0 remainsretail; outer/recursive BC2F0
bothnative through testbridge. Invoker runschildCPU andpropagatesGPRsonfailure.
Everyretail/nativepair resets inputs inclmode/stateglobals; comparesfull2MiB
RAM withnostackexclusions. Finalcleanup7h0ASg terminal0:24576 casesperbody
atO0/O2/UBSan (combined49152 executions),24 deepcallbacksoverpairs,
unchangedsourcebaseline+3retailcontrols+wrong_return/wrong_saved_s0 rejected.

Kepler review identifiedlatea0 updateoncallback-pointer-readfailure,
fixturefailureGPRpropagation, andstaleinitialmode/stateglobals; fixedallthree.
Failuretapesnow86+6invalidAPIchecks, includingcallbackfailureGPRsandroot-load
a0; dOGAG0 terminal0 fullsetup regression plus4controls. VwdbC2 terminal0
actualruntimeleafmarker regression448/build+2controls. kmqhj6/Eb3z1y/
kFWsAW andYydN6T/mdWPWJ areintermediate/pre-reviewevidence, superseded.
All sessions terminal; reviewerclosed; cross-modelskippedautonomously.

Updateddisplaycontract; scopedwhitespace/shellsyntaxchecks pass. Actual
mainunlink/heapstillmocked, productionruntimeframeintegration andbroader
guestaliascoverage pending; exactBC2F0 remainsINCLUDE_ASM. No runtimeactivation,
vendorwrites,commit orpush. Fullport/decompilationgoal staysactive/incomplete.

### BC2F0 retail C exact; BC404 wrong-toolchain gate corrected

Previous turn made nativeframe progress. Recheckedf67fe692; added isolated
setupmatch gate, initiallyREDAdpKT6 missingCbody. ImplementedretailC in
mainc114, keepingXENO_PC_PORT resolver/framebranch separate. Prefix tasktype,
8byte shortvectors,16byte signedwordvectors, unsigned-before-shift arithmetic,
andinline vector expansion helper. CurrentretailBC2F0 no longerINCLUDE_ASM;
PCbranchstilldoesnotactivate eitherstagednativeAPI.

Critical tooling correction: gears.toml BattleCdk andbuild.ninja select
gcc-2.7.2-cdk-psx + --dont-expand-li forbattle/mainc*. Old BC404 isolated
runner usedDefaultPSY-Q wrongly. Correctedbothmatchrunners toexistingpreset,
withoutchanginggears/config. HistoricalBC40416/20 is superseded, not a real
remainingregister mismatch. Defaultsetupzgp5Bw/aBH7t3 alsoinvalidtoolchain;
correctCDKE55SHu4/66, SUwtU4/NaE3Zd/aHU9BY43/66; inlinehelper eTBR2L66/66.

FinalgfWOuz terminal0 SETUP66/66,size108,addressBC2F0. AzoGjN terminal0
DISPLAY20/20,size50,addressBC404. Actualacceptancepredicate rejectscontent,
size andsymbol-address mutations inbothgates. No instructionpatches,
forcedregisters,volatile orweakenedprototypes. Displaylinker addssetupglobal
bindings whileparkedneighborcode remainsoutsideisolatedacceptance.
PCwrappera5FBeh terminal0:786432/buildO0/O2/UBSan+expandedbaseline+3mutants.
Boyle independentread-onlyreview verifiedbytes/pins/PCseparation/gates;
no blockingfindings; reviewerclosed; cross-modelskippedautonomously.

Normalninja mainc114object target failedexit127:missingmips-linux-gnu-cpp,
beforecompilation. ManualfullTU EX4DVu useshostcpp+USE_INCLUDE_ASM andconfigured
CDK/assembler; terminal0objectBC2F0offsetAAC,size108 andBC404offsetBC0,size50.
ExistingneighborASM missing.endwarnings retained; not a cleanNinjabuild or
fulloverlaylink. No dependencyinstallation/buildconfigchanges. Updateddisplay
contract supersedingoldexactlimits. No runtimeactivation,vendorwrites,commit
orpush. Fullgoal remainsactive; remainingnativeintegration/aliascoverage and
whole-overlay/whole-game acceptance are not inferred fromthese exactbodies.

### Native setup frame aliases verified in bounded layouts

Prior turn progressed retail exactC; recheckedHEADf67fe692 andstagedsource.
Added battle_target_setup_alias_test.c andrunner. Nine alignedSPpositions
cross modes0/2/4 x16seeds =432pairs;48first-callback saved-s0/RA/bothedits
onconventionalstack =>480retail/nativepairs/build. Includesmode/stateglobal,
sourcehalfword anddestinationvector aliases. FullRAM/CPU/counters reset
independently; comparesall2MiB plusregisters/SP/RA, callbacks,writes anddirect
return/s0/vector/mode/state expectations. Prologue-overwritten sourcevalues
are accountedfor. CorruptedRA targets are independenthaltsentinels: epilogue
observed, buttargetvalidity/runnablecontinuation/pipelineparity notclaimed.

Finalscratchpad/battle-setup-alias.UjTbah terminal0 O0/O2/UBSan480pairs each,
independentwritecount andcached-originals0/RA sourcecontrols rejectedat
requiredregisterassertions. NfTiLC supersededbycountassertion. Avicenna
read-onlyreview exercisedexistingbinaries/controls, no blockingfindings;
didnotrebuildrunner. Reviewerclosed; cross-modelskippedautonomously.
Limits: firstcallbackonly, no callbackeditscombinedwithoverlap, noarbitrary
SP/task/resolverdomains, andRAMparity/countsnotidenticalwriteordering.
Updateddisplaycontract; shellsyntax/scopedwhitespace pass. No production
edits,runtimeactivation,vendorwrites,commit orpush; fullgoal remainsactive.

### BC460 integer center stage implemented and compared

Previous turn progressed setupaliases; recheckedHEADf67fe692. TracedBC460
file4C970,size644 throughRotMatrixZYX entry afterBC6E0. Elevenmaskslots,
suppressionbyteC3EB0+slot*28+7, optionalpositionpointerCCB3C+slot*4.
Sum arithmetic-halvedpositionwords with32bitwrap; signeddividebycount,
doublewithwrap, seedmin/max fromthatvalue, expandbysignedpositions;
finalwrappedmin+max plusitsignbit, arithmeticshift17. Mean-seededbounds
arenotordinarytrueextrema underoverflow.

Added pc_port/src/battle_target_bounds.c/.h withnativeintegerstage over
stablealready-resolvedviews. NoC3678/frame/matrix/rendering replacement;
fullBC460 remainsASM. REDtestfailedmissingheaderbeforeimplementation.
Finalscratchpad/battle-target-bounds.IKv9wl terminal0:73728cases/build
O0/O2/UBSan andunchangedbaseline, all2048lowmasks x3upperpatterns x3eligibility
profiles x4coordinatepatterns. RealBC460integerinstructions execute; only
memsetsimulated. Nonempty stopbeforegeometry afterJALdelay; emptyrealepilogue
restoresregs. Count,fullstoredmask,center/gap,entryargs/SP/RA, inputimmutability
and2NULLAPIcases pass. Logicalshift/ignoredflag/ten-slots/trueextrema mutations
faildesignatedcenter/countassertions. HjgVJT positives-only superseded.

Ohm reviewfoundnoarithmetic/offset/oracleissue; no testexecution; reviewer
closed,cross-modelskippedautonomously. Fourpositionpatternsnotexhaustive
signedinputdomain; quantizationcanhideotherintermediatedifferences. Added
BOUNDScontract andDISPLAYcrossreference. No runtimeactivation,vendorwrites,
commit orpush; fullgoalactive. NextBC460work: matrix/projection/radius stages
andactualmemory/framecomposition, notclaimingentirefunctionfromintegerstage.

### BC460 projected radius arithmetic ported

Previous turn progressed integercenter stage; recheckedHEADf67fe692. Traced
remainingBC460calls/statewrites. Added PcPortBattleAccumulateTargetRadius
toexistingbounds.c/.h: projectedhalfwords minus(160,164), x4 low16signed,
square/addmod32, signedmaximum. It doesnotproject, sqrt orcomputefinalcamera.
FullBC460 stillASM; GTE/matrix/BB844/statepublication integrationpending.

REDtestmissingAPI declaration precededimplementation. Newradiusfixture runs
actualBC7FC..BC858fragment, stopsBC85C afterdelaystore/update. Final3deoBQ
terminal0 O0/O2/UBSan andunchangedbaseline:786432 combinations (allxhalfwords
x y0/FFFF/x+4 x maxima0/17/40000000/80000000). Checks s5/SP/fourscratchstores
andscaledlowhalfwords; directorigin/unit/dualminus32768witnesses. Latter
square sum80000000 mustlose against0 under signedcomparison.
Nativeunsignedmax/unsignedhalf/wrongyorigin andretailSLT-to-SLTU controls
allfailparityassertion. Boundsregression8rSHaq terminal0 retains73728/build
and4controls. No exhaustiveallpairs/maxima orsecondprojectionpointclaim.

James read-onlyreview foundnoarithmetic/oracle/controlissue; no regression
execution; reviewerclosed,cross-modelskippedautonomously. Updatedbounds
contract withcall-chainlimits. Shellsyntax/scopedwhitespacechecks pass.
No runtimeactivation,vendorwrites,commit orpush; fullgoal staysactive.

### BB844 retail C recovery (resume)

HEAD f67fe692 and branch unchanged; existing dirty work preserved. Added
retail-only BB844 C in mainc114: target-eye normalization, ordered crosses,
right/up/forward rows, Push/Apply/negative translation/Pop. Unsigned negation
preserves NEGU including INT_MIN. PC branch deliberately remains ASM.

New run_battle_view_matrix_match.sh requires exact pinned bytes, address,
size and global symbol. Absent-body RED ETkjg2; test-only .globl extraction
bug rsnAj7 diagnosed and fixed without instruction editing. Final production
source gate 6MfW5I terminal0:100/100,size190; content/size/symbol acceptance
controls rejected. Neighbor exact gates CudP3o 66/66 and GZNHAP 20/20 pass.
Native BC404 YtmxUX terminal0 O0/O2/UBSan+baseline,786432 cases/build and
three rejected controls. Downstream remains simulated.

Manual full mixed-ASM TU compile rDwCED terminal0 with host cpp/configured
CDK/maspsx; existing missing-.end warnings. Not normal Ninja/full overlay
link or gameplay acceptance. No runtime activation, vendor edits, generator
run, commit or push. See bounds contract for evidence and regeneration risk.
Next: native BB844 SDK/GTE parity and BC460 geometry composition; the full
port/decompilation goal remains active and incomplete.

Einstein independent read-only review found no concrete issues in BB844 or
the three exact gates; tests not rerun by reviewer, reviewer closed.
Cross-model review offered but not performed in this turn.

### BB844 native real-SDK composition verified under staged opt-in

Previous turn made progress with exact retail C. HEAD f67fe692 remains
unchanged; no active game/debugger or competing battle test was observed.
Added XENO_BATTLE_VIEW_MATRIX_STAGED opt-in to compile the same mainc114
BB844 body natively; default native path remains ASM. Existing production
func21B14, normalization/cross-product and PsyCross Push/Apply/Pop run in
the native fixture. Retail executes the complete BB844 and SDK machine code
from pinned battle/EXE, no bridge. Both sides share PsyCross GTE, not hardware.

Final run_battle_view_matrix_native_test.sh AdgtA0 terminal0: typed disjoint
fixtures 77825/build O0/O2/UBSan+baseline; explicit no-strict-aliasing builds
O2Alias/UBSanAlias 114689/build. Includes bounded correlated vectors,
zero vectors, all eye-X halfwords, depths0/1/19, alias eye/target/up in the
extension builds, and independent identity/translation witness. Compares
full128-byte guarded region, CP2D/C, all640matrix-stack bytes, stack balance,
and retail callee/SP/RA. Not exhaustive inputs/depths; no overflow-fault,
PS1 hardware, fullBC460 or runtime-admission claim.

Five source-copy controls cross-order/row/sign/missingPop/missingNormal
and five isolated CP2D/CP2C/savedstack/guard/inputpad observation controls
all fail designated assertions. RED xiZpua missing nativeBB844 preceded
opt-in. Turing review found effective-type-invalid initial byte fixtures
and insufficient isolated observation controls: both actionable and fixed.
Prior9zjUDe/qodmyA/pc7mZK evidence superseded byAdgtA0. Alias proof now
explicitly requires no-strict-aliasing; default tests use typed members.

Retail exact regressions fMQWz4=100/100,km0n52=66/66,OGJqxF=20/20.
Default-nativeBC404 PdjKUW terminal0 O0/O2/UBSan+baseline786432/build,
three controls rejected. No SDK/vendor edits, runtime/build registration,
commit orpush. Next: BC460 geometry composition with validated BB844;
full port/decompilation goal remains active. Cross-model skipped in this
autonomous continuation; bounded second review requested on fixture fixes.

Turing second review found no remaining concrete issues under the explicit
compiler-extension alias boundary; reviewer closed. No additional test run
by reviewer claimed. Shell syntax and scoped whitespace checks pass.

### BC460 prerequisite repaired: native RotMatrixZYX rounding

Previous turn made BB844 native/retail composition progress. Rechecked HEAD
f67fe692, scoped dirt, no active game/debugger or competing battle tests.
Traced full BC460 and its first rotation dependency. Found existing native
RotMatrixZYX delegated to sequential PsyCross X/Y/Z, unlike scalar retail.
New rotation_zyx_retail_test.c RED puSedq reproduces angles113/271/509:
m02 retail1653/native1654 and m11 retail3066/native3067.

Replaced only that native delegate in retail_leaf_adapters.c with the retail
separately-floored Q12 products; keeps shim rcos=sine/rsin=cosine, all input
angles read before output, ninehalfword stores, translation/pad/GTE untouched.
This is an existing native leaf behavior repair, not new BC460 dispatch.
No vendor changes or gameplay proof. BC460 remains assembly.

Final run_rotation_zyx_retail_test.sh vilgzE terminal0:786433/build at
O0/O2/UBSan+baseline (every halfword eachaxis, fourpeerpairs, mixedwitness).
Real retail instructions/table vs real production/trig owners; typed
disjoint fixtures, all80bytes/return/storecount/SP/RA/callee/GTE checks.
Seven sourcecopy controls rounding/sign/store/translation/GTE/return/old
delegate rejected. Not every triple or overlap. Old adapter test now links
real trig, requires zero-angle identity, passes35checks/slicepins at all3
builds instead of checking delegation to a fake.

GTE regression first failed link missing PcPort_ServiceVblank from test
fixture; added explicit inert peripheral stub there only. Rerun session75682
terminal0 covers projection,SetMulMatrix,Square0,normalization,magnitude and
existing controls. Production pad/vblank/SDK files otherwise unchanged.
Halley independent read-only review found no issues under disjoint contract,
did not rerun tests, closed; cross-model skipped autonomous. Shellsyntax and
scopedwhitespace pass. No commit,push,fullportbuild or runtimeactivation.
Next remains full BC460 geometry/state-publication composition using now
validated RotMatrixZYX and BB844; full goal active and incomplete.

### Complete staged BC460 camera computation/public writes

Previous turn made progress fixing RotMatrixZYX. HEAD f67fe692 and scoped
dirty sources rechecked; no active game/debugger/competing battle test.
Added battle_target_camera.c/.h: PcPortBattleUpdateTargetCamera(passivebus,
mask), composing existing bounds/radius helpers and actual native SDK+BB844.
Fullwidthmask,C3CDCdistance,D30A0/A8halfwords follow retail; empty selection
leavesdistance/camera. Targethandles staybus-domain, no mask/cast. Stable
input contract, no SDK/localstorage aliases; guestframe/registerresidue not
modeled. No runtime/build registration or existing SDK/body/vendor edit.

RED N8sCTA absentbody precededimplementation; O108c4 missing g_PsxRam
fixturefixed by exposing actualRAM required by realSquareRoot0 tableowner.
RawGTE failure Okd4x1/fJ1fLW traced to VZ0 upperhalf from uninitialized
retailSVECTORstackpadding. Nativezero differs in rawbackendstorage, but
MFC2(1)/SWC2 signextend lowhalf and all GTE VZconsumers use sw.l. Test records
rawdifferences, compares VZ0 viaactualMFC2 and allotherCP2D/CP2C rawexact.
No rawbackendidentity claim or plantedpadding; lowVZ0 control mustfail.

Final run_battle_target_camera_test.sh NPktFF terminal0 atO0/O2/UBSan plus
baseline:6183 cases/build (2048masks x3geometry,30screenboundary,9height/
threshold),2176small/3974large/33empty,5793secondpoint projections and
6099rawpaddingdifferences. Variedstackfill, nonzeroinitialGTE, independent
coincidentpointcamera witness. All2MiBRAM compared exceptactual0x188guest
frame and640mappedSDKstack (latterseparatelycompared); stackbalance and
retailSP/RA/calleeschecked. NoSDKcallees simulated; realretailmemset too.
Bridgecoverageobserversonlyreturn0. SharedPsyCrossGTE, nothardwareoracle.

297nativebusfailureprefixes and9invalidAPIchecks pass; failurestopsfurther
busaccess, preservespublicwriteprefix/GTEstate; notretailfaulttimingproof.
Sevenbehaviorcontrols mask/heightgate/heightsign/eyeY/distanceShift/store
width/radiusbranch andsixisolated RAM/stack/CP2D/CP2C/lowVZ0/guard controls
allrejected. Heightgate firstfails GTEFIFO evenwhenpubliccoordinatesmatch.
EarlierF4im8L superseded byNPktFF finalinitialstate/witness refinements.

Bounds UMajCi andradius RHPQ1O regressions terminal0 retainallbuilds and
controls. Curie two read-onlyreviews foundnoissues with stablebuscontract,
VZ0 distinction orfinalcontrols; noretests, closed. Cross-modelskipped
autonomous. Next: actualruntime resolver/native+guest handle composition
and admission/frame requirements before enabling BC460 dispatch. Fullretail
BC460 staysASM; nofullportbuild,gameplayacceptance,commit orpush. Fullgoal
remainsactive/incomplete. See updatedboundscontract for precise limits.

### BC2F0 executable retail setup contract

Resumed at HEAD f67fe692 on experiment/worldmap-open-gates-20260823;
preserved the existing dirty tree. Pending runner session7349 completed
terminal0, evidence scratchpad/battle-target-setup.3ES9Mu. New retail-only
setup harness passes 65592 cases each at O0/O2/UBSan; missing-first-clear
instruction-copy control fails its required assertion. Includes every
halfword pattern at each vector component, seven full-width mode classes,
four root-presence combinations, and callback replacement on/off. Inputs
are correlated, not exhaustive six-component combinations. Callback bodies
remain simulated; native implementation/integration and gameplay NOT_RUN.

Located packed 0x1C WorkListEntry and +0C u32 callback in work_list_port.c,
plus PcPort_WorkListInvokeSavedCallback for existing guest/native code
dispatch. Neither establishes native versus guest BC2F0 root admission.
Retail BC158 stores its original a0 task into C3680/C3684; its native
producer path remains unresolved. Do not widen packed fields or cast guest
roots directly to native pointers. BC2F0 remains INCLUDE_ASM.

Lagrange independent read-only review found no blocking harness issues;
reviewer did not rerun tests and was closed. Cross-model review skipped
in this autonomous continuation. Updated display contract; shell syntax
and scoped diff whitespace checks pass. No production changes, runtime
activation, vendor writes, commit or push. Full goal remains incomplete.

### BC460 resolver-backed camera fixture (2026-09-14 resume)

Continued on experiment/worldmap-open-gates-20260823 at f67fe692; preserved
the dirty tree. Actual runtime resolver test now passes: run
`XBT_CAMERA_RUNTIME_RUN=1 bash pc_port/tests/run_battle_target_camera_test.sh`.
Evidence scratchpad/battle-target-camera.L5aHNm terminal0: 2217 cases/build
O0/O2/UBSan+baseline, six guest/host/shared pointer domains, 297 native failure
prefixes, nine invalid APIs and15 negative controls. Flat regression gdF5Fn
terminal0: 6183 cases/build and13 controls. Source/ELF pins rechecked unchanged.

Diagnosed y02hrT failure before repair: missing ELF function classification
made memchr a data binding overlapping retail memset instructions. Test runner
now requires existing build/out/slus_006.64.elf for name classification only;
addresses still come from retail maps. Runtime resolver/generator unchanged.
Failure diagnostics report overlapping data bindings. Observation-control
compiles now carry native type defines required by the included runtime TU.

This stage proves resolver-backed staged computation with unchanged host inputs,
guest-owned outputs and parent CPU, not production bridge admission, guest-frame
equivalence, full build or gameplay. Keep full BC460 retail body ASM and staged
camera unregistered. No vendor/production changes, commit or push this resume.
See BATTLE_TARGET_BOUNDS_CONTRACT.md for coverage and comparison exceptions.

Arendt's read-only review found one coverage overclaim, now corrected: only
D_800D30A0/C3CDC have mapped host candidates. C3678 is absent from the map;
its placeholder/SHADOW_WRITE control proves accidental-write detection, not
mapped-binding rejection. No synthetic symbol was added. No other required
findings; reviewer did not rerun builds. Cross-model review skipped for this
autonomous continuation. Full project goal remains active and incomplete.

### BC460 395-word reconstruction and strict state comparison (2026-09-14)

Continued verified353/401 frontier without touching its JG2WNF baseline.
Latest scratchpad/bc460-layout.krWs9l/candidate.c builds395/401 with CDK,
size644 exact; default compiler remains11/401,size674. Six mismatches at
BC848/84C/858 and BC8E8/8EC/8F8 use a2 instead of retail a1 for radius sums.
No assembly patching, named-register binding, production source, preset or
generator edits. Keep BC460 INCLUDE_ASM until401/401 and admission gates pass.

This candidate explicitly models union storage atframe60 and reused resultA0;
Jason's read-only layout review found no required issues. verify.sh session45805
terminal0: original identity and compiled CDK candidate each pass6183 cases,
now INCLUDING guest-frame bytes, raw GTE and final32GPR/HI/LO. Only patched
0x800-byte code window excluded. Wrong-mask source mutant plus frame-byte,
VZ0-high and a1-register observation controls rejected. final-pins.txt
rechecked unchanged. Legacy compiled-MIPS evidence only: the separate native
camera helper still lacks frame/raw-VZ0 equivalence and live admission.

Read BATTLE_TARGET_BOUNDS_CONTRACT.md's latest section before continuing.
Final candidate/pins are authoritative; exploratory sweep files are not
individually verified. No commit, push, vendor write or gameplay claim.

Mendel's separate read-only review of the strengthened differential found no
required issues within its stated contract. Both reviewers were closed; neither
reran builds. Cross-model review skipped in this autonomous continuation.
Full port/decompilation goal remains active and incomplete.

### BC460 full retail-C reconstruction frontier (2026-09-14)

Previous resolver stage was verified progress. Current HEAD remains f67fe692
on experiment/worldmap-open-gates-20260823. New scratch-only reconstruction:
scratchpad/bc460-reconstruction.JG2WNF/candidate.c, build.sh, verify.sh,
differential.c and match.ld. Verify runner session20180 exited0.

Same source with configured gcc-2.7.2-psx:11/401 words, size674; CDK:353/401,
size640 versus retail644. This is NOT exact and no toolchain/admission change
was made. Remaining48 mismatches are predominantly temporary allocation,
frame output locations, small-branch scheduling and final pointer advance.
Next source-backed task: resolve those differences, then prove exact bytes,
symbol/size and negative controls before any replacement of INCLUDE_ASM.

Final compiled CDK body and original identity baseline each pass6144 correlated
machine-code differential cases against retail SDK/BB844, with no call bridges.
Wrong-mask source mutant rejected. O2 harness only; code window and combined
guest frame excluded, unused VZ0 padding normalized through MFC2. No frame,
full-register, full-build or gameplay claim. final-pins.txt captures sources,
payloads and both compilers. No production/vendor edits, commit or push.

Aquinas independent read-only review found no required issues within this
scratch-only contract and independently recomputed both word scores. Reviewer
did not rerun builds/tests and was closed. Cross-model review skipped in this
autonomous continuation. Full project goal remains active and incomplete.

LATEST BC460 frontier: the395-word section above supersedes this historical
353-word entry. Resume `scratchpad/bc460-layout.krWs9l/candidate.c`; its strict
6183-case run and four controls passed, but six radius-sum words still differ.

SUPERSEDED by the exact result below.

### BC460 exact: six radius-sum words solved (2026-09-14)

Resumed the 395/401 frontier at HEAD f67fe692 (dirty tree preserved). The final
radius-shape sweep (scratchpad/bc460-radius-shape.QJZzzj) already contained the
fix but it had not been consolidated, re-verified or recorded. Winning shape,
applied at both radius sites (BC848/84C/858 and BC8E8/8EC/8F8): replace
`count=vx*vx+vy*vy; if(maximum<count) maximum=count;` with a block-scoped
running sum
`{ int squared=vx*vx; squared+=vy*vy; if(maximum<squared) maximum=squared; }`.
Splitting the sum into a named local (rather than the fused expression) is what
keeps cc1's running sum in the a1 chain — retail `addu a1,a1,t0` /
`slt v0,s5,a1` — instead of a2. An identical `u32 squared` form emits
byte-identical code; the signed `int` form was kept because retail's comparison
is signed. No inline assembly, register binding or post-assembly word
replacement was used.

Promoted to `scratchpad/bc460-layout.krWs9l/candidate.c` (sha256 `b42e05f2…`);
the395 predecessor is preserved as `candidate-395-prefix.c` / `layout395.c`
(`f2cafc4e…`). CDK build is now 401/401 words, size644 exact;
`candidate.bin` sha256
`4549ebf7a618d484078505b6dd7d5d925e37c579ff7b529778bce077a6814996`, byte-identical
to `disc/battle.bin[0x4C970:0x4CFB4]` (independent `cmp` clean). Configured
gcc-2.7.2-psx still11/401, size674 (unchanged): this body is CDK-provenance
only.

`verify.sh` exited0 with the full matrix: identity baseline and compiled CDK
candidate each pass6183 cases (guest frame, raw GTE and final GPR/HI/LO now
included); the wrong-mask source mutant is rejected; observation controls1/2/3
(public RAM / raw GTE / final registers, including $a1) are all rejected.
`final-pins.txt` regenerated with the new candidate/binary hashes; all other
source/ELF/compiler/payload pins unchanged. Exploratory sweep files remain
unverified and are not evidence.

NEXT EXECUTABLE STEP (NOT done here): installing the body is a generator
admission change — add `func_800BC460` to `VERIFIED` and `CDK_SET` in
`tools/scripts/gen_battle_tus.py` so it lands in a `battle/mainc%d` (BattleCdk)
TU instead of `battle/mainl115` (BattleLiteral / 2.7.2-psx), then run the full
matching build and confirm `battle.bin` stays `1830b4ef…`. The scratch `match.ld`
uses absolute symbol addresses, so the real linker script's gp handling is not
yet exercised. No production source, preset, generator or vendor edit; no
commit, stage or push. Full port/decompilation goal remains active.

DONE — see the install section below (the generator route was rejected as
unsafe; see there).

### BC460 installed in a BattleCdk TU (2026-09-14)

User-authorized production install. `func_800BC460` is now a hand-written
`BattleCdk` TU rather than a generator regeneration, because the vendored
`tools/scripts/gen_battle_tus.py` is stale: run in a throwaway mirror it
rewrites 24 hand-edited files (`mainc114.c`'s port `#ifdef`s, `main14`…`main44`,
`main72/73/125`) and would clobber them. Its `VERIFIED`/`CDK_SET` lists also do
not contain `func_800BC460`, so it has no knowledge of `mainc115` at all; do not
run it against the live tree
without reconciling that diff first.

Landing:
- New `src/battle/mainc115.c` (path `battle/mainc` → BattleCdk preset,
  gcc-2.7.2-cdk-psx + `--dont-expand-li`) carries the 401/401 body; its header
  records the CDK/2.7.2-psx split.
- `src/battle/mainl115.c` lost its BC460 `INCLUDE_ASM` and now holds only
  `func_800BCAA4`/`func_800BCAD0`.
- `config/battle.yaml`: `[0x4C970, c, mainc115]` + `[0x4CFB4, c, mainl115]`
  replaced `[0x4C970, c, mainl115]`; `linker/battle.ld` and `asm/` regenerate
  from that.
- `pc_port/build_port.sh` `REFERENCE_ONLY_GAME_TUS` gained
  `src/battle/mainc115.c`, so the port neither compiles nor links it (the port
  runs the retail overlay; `-DSKIP_ASM` already dropped the old stub).

Verified with the real preset, not the scratch harness: the built
`build/src/battle/mainc115.c.o` linked at the pinned retail symbol addresses is
byte-identical to `disc/battle.bin[0x4C970:0x4CFB4]`, sha256
`4549ebf7a618d484078505b6dd7d5d925e37c579ff7b529778bce077a6814996`, and
`func_800BC460` is `0x644` bytes in the object. `mainl115.o` `.text` is likewise
byte-identical to `disc/battle.bin[0x4CFB4:0x4D00C]`, sha256 `80232ffa…`
(`BCAA4`/`BCAD0`, 0x58 bytes). The exact-scratch differential
(`scratchpad/bc460-layout.krWs9l/verify.sh`, 6183 cases + controls) still passes.

BLOCKER, PRE-EXISTING AND NOT FROM THIS CHANGE: a from-clean `make build`
still fails at `build/out/battle.elf` with 89 unresolved local jump-table
labels (`build/asm/battle/data/0.rodata.s.o` → `.L80079F00…`, owned by landed
C functions such as `main14/func_80079ED8` whose retail jump tables reference
`.L` labels the C body does not define). A baseline revert of all four edits
reproduces the identical 89 references, and a diff against the 2026-09-13
evidence log (`docs/evidence/lahan-equip-oracle-layout-20260913/battle-build.log`)
shows the same first refs with only labels at `0x800811xx`/`0x80084Cxx` added by
battle bodies landed since. BC460 has no jump table and adds zero. The earlier
"battle.bin == 1830b4ef" gate records cannot be reproduced in this environment
while this holds; do not force the link by deleting relocations or substituting
addresses. Fixing cross-object local jump-table label resolution is the
prerequisite for the end-to-end ROM gate. `disc/battle.bin` (retail) remains the
unchanged authority.

Port: `mainc115.c` is reference-only, so it is neither compiled nor linked and
the port link is unchanged by this install. The port gate could not be run to
completion anyway, for a separate pre-existing reason: `pc_port/build_port.sh`
aborts with `unclassified port source is not in PORT_SOURCES:
pc_port/src/battle_target_bounds.c`, left by the battle-target work already in
this tree. (The `localhost/xenogears-dev-toolchain:current` image also lacks
OpenSSL dev; use the newer `:krom-20260913` image for the port.) No commit,
stage or push.

### Battle jump-table label blocker INVESTIGATED — it is a symptom, not the bug (2026-09-14)

Requested fix. Reproduced off the failing link (`make build` in
`:current`, splat0.33.2/spimdisasm1.33.0). Findings:

- The 80 unique unresolved `.L800xxxxx` labels are owned by only FOUR landed C
  TUs: `main14` (48 labels), `main38` (13), `main35` (12), `main36` (7). In
  still-asm objects `jlabel` emits `.global .L…` (macro.inc) so the retained
  retail jump tables in `asm/battle/data/*.s` resolve; landed C bodies define
  none.
- Mechanism confirmed in scratch: appending `.L800xxxxx = 0x800xxxxx;` (address
  is encoded in the label name) for all80 to a linker script makes the exact
  real link command succeed, `rc=0` (`/var/tmp/labdefs.ld`, `/var/tmp/battle-lab.elf`).
- BUT the linked overlay is NOT retail and this fix is a FALSE GREEN:
  `/var/tmp/battle-lab.bin` is342908bytes, sha256
  `25f8c7844313ec49368c3bf544775028de72ac67df2dc15e5bec04a988ee7105`, vs
  `disc/battle.bin` 343936bytes `1830b4ef…`; and of857 named `func_XXXXXXXX`
  symbols, ZERO sit at their retail address (displacements +0x130 early, then
  −0x234/−0x24C/−0x378/−0x458/−0x404/−0x898…). So resolving labels alone yields a
  non-matching battle.bin — exactly the `do not force a link` warning in
  docs/evidence/lahan-equip-oracle-layout-20260913/RESULT.md.
- Localization: `linker/battle.ld`'s `.text` object order equals the
  `config/battle.yaml` address order (checked, 136/136), so the drift is per-TU
  size, not ordering. The four TUs above are `switch` bodies that additionally
  emit their own `.rodata` (main14/35/36 non-empty; retail's tables are already
  retained in `asm/battle/data/`), and the overlay is1028bytes short overall.
  The battle overlay build is broadly non-matching, not one label class.
- The BC460 install is unaffected: `mainc115.c.o` is byte-exact at its retail
  address, and the early +0x130 divergence predates and is independent of it
  (displacement at BC460 is −0x378, i.e. accumulated upstream drift).

Do NOT add label definitions or absolute replacements to the real link to make
it "pass"; that produces a wrong ROM. Next real work is per-TU battle size/rodata
audit (and preset/`li`-flavour assignment), starting from the early +0x130 code
base offset. No commit, stage or push.

### Battle overlay: link blocker root-caused and fixed; overlay now links, still not byte-exact (2026-09-15)

The 89 unresolved `.L800xxxxx` references are GONE, fixed structurally, with no
label definitions, no absolute-address substitutions and no deleted relocations.
`make build` is now green end to end (`exit=0`, 1133/1133) and produces
`build/out/battle.bin`. It is NOT yet retail: built
`04f8b5df440ad398b1e053fdd32317aef1c6b4bd182529901ed55bfd053f963c`, 343584 bytes
vs pin `1830b4ef...`, 343936. `make rom-check` therefore still reports battle
FAIL (slus/field stay known-red; no pin was changed -- the pre-existing
`config/checksum.sha` battle line already carries the correct retail hash).

TWO ROOT CAUSES, both confirmed by measurement:

1. Compiler-emitted switch tables had nowhere to live. `main14/35/36/38`'s landed
   C bodies emit their own `.rodata` jump tables, while retail's copies of those
   same tables were ALSO retained as assembly data in `asm/battle/data/0.rodata.s`.
   That duplicated 0x130 bytes ahead of `.text` (built code began at 0x80070F5C
   instead of 0x80070E2C) and left the retained tables referencing `.L` labels no
   C body defines. Fix: place each C object's `.rodata` at its retail table
   address in `config/battle.yaml`, exactly as `field` already does
   (`- [0x9C, .rodata, main/misc4]`); the surrounding retail tables stay asm data.
   Added: `0x8C main14`, `0x520 main35_p1`, `0x720 main35_p3`, `0x760 main36_p1`,
   `0x790 main38_p1`, with `rodata` re-splits at 0x14C/0x540/0x740/0x7C4.
   This is why `field` has 0 `.word .L` refs in data and battle had 1206.

2. Mixed asm/C TUs could not be one object. cc1 emits every file-scope
   INCLUDE_ASM block BEFORE every compiled body, so any TU whose retail layout
   puts a C function ahead of an assembly function is unrepresentable as a single
   object. Fix: split each such TU into one object per retail `[asm run][C run]`
   pair. `src/battle/<tu>.c` now carries `#if BATTLE_PART(n)` guards and
   `src/battle/<tu>_p<N>.c` are thin wrappers that define `BATTLE_TU_PART` and
   carry that part's INCLUDE_ASM run; part 0 (whole file) is what the port and
   the host test scripts still compile, so no test or port ownership changed.
   Split: main35 (4 parts), main36/38/40/73/mainc114 (2), main72 (3).

VERIFIED RESULTS (all from the built artifact, not from disc/battle.bin):
- `battle_RODATA_SIZE` is now exactly `0x133C` -- the +0x130 excess is gone.
- `battle_TEXT_START` is exactly `0x80070E2C`, delta +0 from retail.
- 160 of 857 named `func_########` symbols now sit at their retail address
  (was 0 of 857). 697 remain misplaced, all downstream cascade.
- main14's emitted tables at file 0x8C..0x14C are byte-identical to retail.

PER-TU STATE: 125 of 146 battle TUs are byte-exact when linked alone at their
retail VRAM against pinned symbol addresses. The 21 that are not, with word
diffs and size deltas (`sizeΔ=0` means it no longer contributes layout drift):

    main17 1/0     mainc29 3/0    main23 6/0     main35_p3 12/0
    mainc15 13/0   main73_p1 17/+12  main16 21/0  main38_p1 29/0
    mainc25 37/0   main72_p2 49/-12  main22 55/-8  main72_p1 63/-16
    main19 64/0    main37 86/-12   main34 107/+16  main35_p2 103/0
    main44 123/+12 main125 164/-140  main40_p1 311/-80  main27 340/+36
    main36_p1 464/-160

FIXED THIS PASS (byte-exact, standalone-verified):
- `main14` entirely (7 functions). The two 24-case jump-table dispatchers
  hoisted nothing: retail forms the 368-byte actor stride inside every case and
  has no `default`. Also: single-bit `== 0` tests must go through a temporary
  (`u32 bit = x & 0x20; ret = bit == 0;`) or cc1 emits a shift/xor extract where
  retail has `andi`/`sltiu`.
- `main17`'s func_8007A9D0 and func_8007AA1C. Retail's saturating cells use an
  explicit two-step `row` then `q = row + p[1]` cell pointer, a signed
  threshold compare, and a SEPARATE `u8 value` result variable copied from the
  arithmetic temporary, so there is one store and one address computation.
- `main38_p1`'s func_80085EB4 from 81 to 29 words at exact size, by reusing the
  incoming argument registers as retail does and modelling the
  fall-through `jtbl_80070280` counter as a 13-case `switch` with `default`.

PRESET MISASSIGNMENT IS REAL AND MEASURABLE. Sweeping all 21 remaining TUs
across both compilers showed three conclusive CDK wins -- fewer diff words AND
a size mismatch turning exact -- so those were switched to `BattleCdk` via the
repo's own filename mechanism (`main15/25/29` renamed to `mainc15/25/29`, with
their yaml entry, `build_port.sh` entry, INCLUDE_ASM folder and
`tools/remu/tests/diff_*.c` references updated):

    main15  psx 45 words size-4  ->  cdk 13 words size 0
    main25  psx 66 words size-4  ->  cdk 37 words size 0
    main29  psx 10 words size 0  ->  cdk  3 words size 0

CDK was NOT better for the other 18 and they were left alone. Re-sweeping the
remaining TUs per-compiler before hand-matching them is worth it: a wrong
preset changes TU-wide register allocation and li expansion, so idiom hunting
against the wrong compiler is wasted effort.

UNCHANGED AND STILL PINNED: BC460's `mainc115.c.o` is byte-identical to
`disc/battle.bin[0x4C970:0x4CFB4]`, sha256
`4549ebf7a618d484078505b6dd7d5d925e37c579ff7b529778bce077a6814996`, and
`mainl115.o` to `[0x4CFB4:0x4D00C]`, `80232ffa...`. Re-verified after every
change in this pass. `disc/*.bin` and `config/checksum.sha` were not touched.

REMAINING UNCERTAINTY: one word in `main17/func_8007AA60` -- retail emits
`mult cell,p[2]` and every source operand order and cast tried yields
`mult p[2],cell` (size is exact). This is the documented commutative-operand
reversal and is flagged in a source comment; it is a register-allocation
artifact, not an arithmetic difference. The 21-TU list above is content-level
matching work, not a structural blocker.

NEXT EXECUTABLE STEP: fix the remaining TUs in ADDRESS order, because only a
`sizeΔ != 0` TU shifts everything after it. The earliest drift is now
`main19/func_8007AE98` (-4). Order: main19, main22, main27, main34, main36_p1,
main37, main40_p1, main44, main72_p1, main72_p2, main73_p1, main125, then the
already-size-exact remainder. Track progress with the two metrics that cannot
be faked: `battle_TEXT_START` delta (hold at +0) and the count of
`func_########` at their retail address (160 -> 857).

TOOLING (scratch, in `.xeno-tmp/battle-pass/`, git-ignored, rebuildable):
`tu_exact.py` links one TU alone at its retail VRAM and diffs `.text` against
`disc/battle.bin` (needs `SUBALIGN(4)`, or every TU falsely reports drift);
`func_cmp.py` per-function retail-vs-built sizes; `fdiff.py` side-by-side
disassembly; `tu_build.sh` single-TU compile with the repo's own cc1/maspsx
(`XENO_TU_GCC=cdk|psx` to sweep presets); `variants.py` + `v_*.py` sweep source
shapes for one function and report word/size diffs; `metrics.py` prints the two
progress metrics. Host `tu_build.sh` output was checked byte-identical to the
container's objects for control TUs (main2, mainc116).

No commit, stage or push. The generator `tools/scripts/gen_battle_tus.py`
remains stale and was NOT run; it has no knowledge of the `_p<N>` split or of
mainc115/mainc15/mainc25/mainc29 and would clobber the hand-edited TUs.

### Battle overlay continuation: proven idiom catalogue, size drift 344 -> 216 bytes (2026-09-15, later)

State after this push, all measured on the built artifact:
- `make build` green; `build/out/battle.bin` is 343720 bytes, 216 short of
  retail's 343936. Still NOT byte-exact, so `make rom-check` still fails battle.
- `battle_TEXT_START` 0x80070E2C, delta +0; `battle_RODATA_SIZE` 0x133C exact.
- 195 of 857 functions at their retail address (0 at the start of the session,
  160 at the previous entry).
- 125 of 146 TUs byte-exact standalone. Of the 21 remaining, ELEVEN are now
  size-exact and therefore no longer shift anything downstream:
  `main17` 1w, `main19` 1w, `mainc29` 3w, `main23` 6w, `main35_p3` 12w,
  `mainc15` 13w, `main22` 14w, `main16` 21w, `main38_p1` 29w, `mainc25` 37w,
  `main35_p2` 103w.

KEY MEASUREMENT: the remaining size deltas sum to exactly the byte shortfall.
    main27 +36, main34 +16, main36_p1 -132, main37 -12, main40_p1 -80,
    main44 +12, main72_p1 -16, main72_p2 -12, main73_p1 +12, main125 -40
    = -216, and 343936 - 343720 = 216.
So making those ten TUs size-exact makes the overlay the right length and puts
every function at its retail address; content diffs can then be ground down
independently. Fix them in ADDRESS order; only a `sizeΔ != 0` TU cascades.

PROVEN IDIOMS (each verified by a byte-exact function this pass -- reuse these
before hand-searching, they recur across the whole `u8** ppBoard` opcode family):

1. Saturating table cells (main17 A9D0/AA1C and main19 AE38/AE98 now exact):
   retail forms a cell pointer in two steps (`row = base + (index << 6)`, then
   `q = row + p[n]`), computes into a signed temporary, copies it into a
   SEPARATE `u8` result variable, conditionally overwrites that variable, and
   stores once. The threshold is a signed `>= 0x100`, not `> 0xFF`.
   A one-expression form emits two address computations and the wrong size.

2. Single-bit tests must go through a temporary:
   `u32 bit = x & 0x20; ret = bit == 0;`  A direct `(x & 0x20) == 0` makes cc1
   emit a shift/xor extract where retail has `andi` + `sltiu`.

3. Two globals 16 bytes apart (main22 BA04/BA44, size now exact): retail
   materialises the first symbol as a pointer VALUE and derives the second with
   `addiu +16`, sharing one `%hi`. Requires local pointer variables, and the
   row offset must be computed BEFORE the base pointer, or the `lui` is
   scheduled first and every register shifts.

4. Halfword pairs fused into a u32 (main125 BEF24/BEF8C, -40 -> -4/-12):
   retail round-trips through stack slots, so the pair is a local STRUCT, not a
   fused expression. Assign each member through an `s32` temporary or cc1 emits
   `lhu` where retail has `lh`.

5. Repeated stores are countdown LOOPS, not straight-line code (main36 85AC4,
   -32 -> -4): `for (i = 2; i >= 0; i -= 2) { *p = 0; p -= 2; }`. Retail also
   RECOMPUTES the 368-byte stride for the second loop instead of reusing the
   first offset; reusing it loses 24 bytes.

6. Not expressible in C -> keep retail assembly (main125 BEE2C, -36 -> 0,
   byte-exact): it switches `$sp` onto a heap block for the call. Reverting such
   a body to `INCLUDE_ASM` with the algorithm documented as a comment is the
   normal matching-decomp state and is byte-exact by construction. CAVEAT: splat
   classifies a function as `matchings/` if the C file TEXTUALLY defines it, even
   inside an `#ifdef`, so a body kept under `#ifdef XENO_PC_PORT` makes the
   `nonmatchings/` path disappear and the TU fails to assemble. Use a plain
   INCLUDE_ASM plus a comment, or point the INCLUDE_ASM at `matchings/`.

OPEN, RECURRING, NOT SOLVED: several functions sit exactly 1 word from exact
with the right size, all the same artifact -- retail's `mult` operand order
(`mult cell,p[2]`) versus cc1's (`mult p[2],cell`), coupled to which value lands
in an argument register. Confirmed in `main17/func_8007AA60` and
`main19/func_800AEF0`; ~15 source operand orders, casts, temporaries and
declaration orders were swept without breaking it (see
`.xeno-tmp/battle-pass/v_aa60.py`, `v_aef0*.py`). `main36/func_80085AC4` is the
same class: retail copies `a0` into `a2` because it uses `a0` as the halfword
temp, costing one `move`. Do NOT keep re-sweeping these blind -- they need a
different lever (a different preset for the TU, or understanding cc1's
`emit_move`/operand canonicalisation), and they cost nothing in layout terms.

PRESET SWEEPS MUST BE REDONE AFTER SOURCE CHANGES: `main19` measured worse under
CDK with its old source (psx 64w vs cdk 74w) but after the idiom fixes it is
psx 1w vs cdk 11w -- the ranking held, but the margin moved by an order of
magnitude, so a marginal-looking TU can flip. `main15/25/29` were switched to
`mainc15/25/29` (CDK) on conclusive evidence earlier this session.

NEXT EXECUTABLE STEP: continue the ten size-drifting TUs in address order.
`main27` (+36, three functions), `main34` (+16, func_80080AE4), `main36_p1`
(-132, func_80085454 -28 and func_80085618 -100), `main37` (-12,
func_80085D34), `main40_p1` (-80, func_8008860C -8 and func_80088990 -72),
`main44` (+12, func_8008AAA0), `main72_p1` (-16, func_800AA820), `main72_p2`
(-12, func_800AE220), `main73_p1` (+12, func_800AF400 -- retail peels the bit-0
test, hoists `li a1,0xd` and fills the branch delay slot with a speculative
`bit++` compensated by `addiu -1`; five loop shapes tried, none matched),
`main125` (-40). Track `battle_TEXT_START` delta (hold +0) and the count of
`func_########` at their retail address (195 -> 857) via
`.xeno-tmp/battle-pass/metrics.py`.

BC460 pins re-verified unchanged after every build this pass: `mainc115.c.o`
`4549ebf7...`, `mainl115.o` `80232ffa...`. `disc/*.bin` and
`config/checksum.sha` untouched. No commit, stage or push.

### Battle continuation part 2: three more structural idioms, |size error| 368 -> 300 (2026-09-15)

Built artifact: 343716 bytes (220 short of 343936), `make build` green, 195/857
functions at their retail address, 125/146 TUs byte-exact, `battle_TEXT_START`
delta +0, `battle_RODATA_SIZE` 0x133C. Battle is still NOT byte-exact.

Track SUM OF ABSOLUTE size deltas, not the net: net size can move the wrong way
while every function gets closer (main27/func_8007D478 went from +48 to -16 --
a 32-byte accuracy gain that made the overlay's net length shrink). Absolute
error went 368 -> 300 this pass. Remaining, in address order:

    main27    -28   func_8007D344 -16, func_8007D478 -16, func_8007D610 +4
    main34    +16   func_80080AE4
    main36_p1 -132  func_80085454 -28, func_80085618 -100
    main37    -12   func_80085D34
    main40_p1 -20   func_8008860C -8, func_80088990 -12
    main44    +12   func_8008AAA0
    main72_p1 -16   func_800AA820
    main72_p2 -12   func_800AE220
    main73_p1 +12   func_800AF400
    main125   -40   func_800BEEB4 -16, func_800BEF24 -4, func_800BEF8C -12,
                    func_800BEFF4 -8

THREE MORE PROVEN IDIOMS (add to the catalogue in the previous entry):

7. Parallel scalar flags are really a stack ARRAY
   (main27/func_8007D478: 360 -> 296 bytes, retail 312; |error| 48 -> 16).
   Three `u8 f0,f1,f2` selected by nested ternaries generate far more code than
   retail. The tell was already in the function's own comment: retail "reads off
   the stack" for the out-of-range index, i.e. it indexes `u8 flags[3]` by the
   runtime value. Whenever a comment mentions retail reading past a local, look
   for an array.

8. Retail does NOT always hoist a common subexpression out of an if/else; when
   it duplicates work into both arms, the C must duplicate it too
   (main40/func_80088990: 424 -> 484 bytes, retail 496; |error| 72 -> 12).
   The rounding `if (v < 0) v += 0xFF; v >>= 8;` sits inside each direction arm
   in retail; written once above the flag test, cc1 hoists and the function
   comes out 72 bytes short. Same lesson in reverse to idiom 5.

9. Repeated-store loops and recomputed strides (idiom 5) generalise: retail
   frequently recomputes an index/stride after a loop rather than keeping it
   live. Reusing a saved offset is the wrong default.

SPLAT CAVEAT, worth repeating because it silently breaks the build: splat routes
a function's per-function `.s` to `matchings/<tu>/` if the C file TEXTUALLY
defines it -- even inside an `#ifdef` that the matching build never takes. So a
body kept as `#ifdef XENO_PC_PORT` C plus `#else INCLUDE_ASM(".../nonmatchings/...")`
leaves the nonmatchings path nonexistent and the TU fails to assemble with
`can't open ... for reading`. Use a plain INCLUDE_ASM with the algorithm in a
comment (what main125/func_800BEE2C now does), or point the INCLUDE_ASM at
`matchings/`. `asm/` is regenerated every build, so this only shows up after a
full from-clean run, not in a single-TU host compile.

ALSO: the host single-TU harness (`.xeno-tmp/battle-pass/tu_build.sh`) cannot
assemble a TU whose INCLUDE_ASM target splat has not yet emitted; it silently
leaves the previous object in place and the audit then reports STALE numbers.
Always confirm a surprising per-TU result against a full container build.

WHAT IS LEFT IS BOUNDED BUT NOT SMALL: 14 functions across 10 TUs for size
exactness, then ~19 TUs of content diffs (eleven of which are already
size-exact and small: main17 1w, main19 1w, mainc29 3w, main23 6w, main35_p3
12w, mainc15 13w, main22 14w, main16 21w, main38_p1 29w, mainc25 37w,
main35_p2 103w). `main36_p1/func_80085618` (-100, 1196 bytes retail) and
`main40_p1/func_8008860C` (-8 but 135 words, 48-byte retail frame vs 24 built,
and retail's `blez` implies the loop bound is read as SIGNED) are the two
hardest remaining; neither was attempted to completion.

Everything else in the previous two entries still stands, including the open
`mult` operand-order class and the BC460 pins (re-verified after every build).
No commit, stage or push.

### Battle continuation part 3: dispatch-chain idioms; 8 TUs left with size drift (2026-09-15)

Built `battle.bin` 343736 bytes (200 short), sha256 `12ba8d4c...`; still NOT
retail. 195/857 functions at their retail address, 125/146 TUs byte-exact,
`battle_TEXT_START` +0, `battle_RODATA_SIZE` 0x133C. BC460 pins re-verified
(`4549ebf7...` / `80232ffa...`).

Size drift is down to EIGHT TUs, absolute error 368 -> 264 over the session:
    main27 -28, main34 +8, main36_p1 -132, main37 -12,
    main40_p1 -20, main44 +12, main73_p1 +12, main125 -40
`main72_p1` and `main72_p2` became size-exact this pass, and
`main72_p1/func_800AA820` is now BYTE-EXACT.

TWO MORE PROVEN IDIOMS:

10. An if/else-if chain over small integer constants is a `switch`
    (main72_p1/func_800AA820, now byte-exact at 120 bytes, was 104).
    An if/else chain returning function pointers emits an eagerly materialised
    default and shares it; retail gives every arm its own `lui`/`addiu` and one
    common exit, which is what `switch` produces. CRITICAL DETAIL: cc1 emits the
    case blocks in SOURCE order, so the case labels must be written in retail's
    block order -- `case 1, case 2, case 3` here, not `case 2, case 1, case 3`.
    Reordering the cases was the last word of the diff.

11. A tail shared by most arms but not all is written out twice
    (main72_p2/func_800AE220: 120 -> 132 bytes, size now exact).
    Retail duplicates the `lhu`/`sll` return tail because one arm
    (`a1 == 3`) returns on its own path. Keeping a single trailing `return`
    for every arm makes the function 12 bytes short.

Also confirmed for `main34/func_80080AE4` (+16 -> +8): retail materialises
`&D_800D2DD7` as a pointer and re-reads the rotation limit through it, keeps the
index and running minimum as full words (`s32`, not `u8` -- byte types force
repeated `andi ...,0xff`), and advances the index BEFORE the comparison so cc1
lands the increment in the first branch's delay slot.

STILL OPEN, in rough order of difficulty:
- `main36_p1` -132 (`func_80085454` -28, `func_80085618` -100). 85618 is a
  1196-byte jump-table switch; the C models retail's two exit tails with a
  synthesised `mark` flag, which is almost certainly the cause -- retail
  duplicates the mark/no-mark tails per arm instead.
- `main125` -40 across four functions, each 4-16 bytes short.
- `main27` -28 (`func_8007D344` -16, `func_8007D478` -16, `func_8007D610` +4).
  D610 needs `i++` in a delay slot (idiom 11-adjacent); D478's remaining 16
  bytes are retail keeping the slot-table base in a callee-saved register as a
  loop invariant, which cc1 declines to hoist.
- `main40_p1` -20, `main37` -12 (an epilogue/load-delay scheduling difference:
  retail leaves a `nop` where cc1 hoists `lw ra` into the load delay),
  `main44` +12, `main73_p1` +12 (loop rotation with a speculative increment
  compensated by `addiu -1`), `main34` +8.

Unchanged: the `mult` operand-order class (several functions 1 word from exact
at the right size) and the splat `matchings/` classification caveat. Track
progress with `.xeno-tmp/battle-pass/metrics.py`; audit per TU with
`tu_exact.py` + `func_cmp.py`; sweep source shapes with `variants.py`.
No commit, stage or push.

### Battle continuation part 4: switch-dispatch and pointer-base wins; 152 bytes short (2026-09-15)

Built `battle.bin` 343784 bytes, 152 short of retail 343936. Still NOT exact.
195/857 functions at retail address, 125/146 TUs byte-exact, TEXT_START +0,
RODATA 0x133C, BC460 pins intact (`4549ebf7...`).

Size drift, now 8 TUs / absolute error 216 (was 368 at session start):
    main27 -28, main34 +8, main36_p1 -104, main37 -12,
    main40_p1 -20, main44 +12, main73_p1 +12, main125 -20

NEW THIS PASS:
- `main36_p1/func_80085454` is SIZE-EXACT (452, was 424). The outer dispatch on
  the D_800D2C88 byte must be a `switch` (retail's ordered `slti ...,3`
  comparison tree) while the inner `w` dispatch stays an if/else-if chain.
  Making BOTH switches overshoots retail by 32 bytes -- so idiom 10 is
  per-dispatch, not per-function. Verify each level separately.
- `main125` -40 -> -20. `func_800BEEB4` (-16 -> +4) advances the row base by 4
  per iteration (re-adding the 0x8C8C constant) and writes the list terminator
  through `out[count]`, not through the append cursor; the loop is
  `i != 11`, not `i < 11`.
- `main34/func_80080AE4` +16 -> +8 (see part 3 for the idiom).
- `main72_p1` and `main72_p2` are size-exact; `func_800AA820` byte-exact.

NEGATIVE RESULTS worth not repeating:
- `main36_p1/func_80085618`: the `-100` is NOT the synthesised `mark` flag.
  Inlining the tail store at all six mark sites changes nothing -- cc1
  cross-jumps them back into a single tail (verified: one `0x2eb` store in the
  object either way). The real difference is register allocation: retail saves
  s0-s8 + ra (88-byte frame, ten callee-saved registers) and keeps six
  induction variables live (s3+=2, s6/s4/s0+=368, s2+=1, s5+=28), while the
  build spills instead. This needs the loop's induction variables expressed so
  cc1 keeps them all in registers, not a control-flow change.
- `main125/func_800BEFF4` (-8): assigning `D_800C3EB0` to a local pointer does
  not stop cc1 CSE-ing it back to a `%lo` offset; declaring it early instead
  overshoots by +4. Idiom 3 does not always apply.
- `main125/func_800BEF8C` (-12): retail reads the +0xA0/+0xA4 pair with `lhu`
  (unsigned) while the +2/+0xA pair is `lh`; fixing the signedness is correct
  and now recorded in the source, but it does not change the size.

HIGHEST-VALUE REMAINING WORK, in order:
1. `main36_p1/func_80085618` -104 (register allocation / induction variables).
2. `main27` -28: `func_8007D344` -16 and `func_8007D478` -16 (retail keeps the
   slot-table base in a callee-saved register as a loop invariant that cc1
   declines to hoist); `func_8007D610` +4 needs `i++` in a delay slot.
3. `main40_p1` -20, `main37` -12 (epilogue/load-delay scheduling: retail leaves
   a `nop` where cc1 hoists `lw ra` into the load delay slot), `main44` +12,
   `main73_p1` +12, `main34` +8, `main125` -20.
Then the content-only TUs, eleven of which are already size-exact and small.

No commit, stage or push.

### BATTLE OVERLAY IS BYTE-EXACT (2026-09-15)

`make build` (from clean) produces `build/out/battle.bin` sha256
`1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291`, 343936
bytes, `cmp` clean against `disc/battle.bin`. Reproduced twice from clean.
`make rom-check` reports `PASS build/out/battle.bin 1830b4ef1fe37129 (343936
bytes)`. slus_006.64 and field remain known-red exactly as before; no pin was
changed (`config/checksum.sha`'s battle line already held the retail hash).
All 857 named `func_########` symbols are at their retail address (0 misplaced);
`battle_TEXT_START` 0x80070E2C, `battle_RODATA_SIZE` 0x133C.

HOW, AND WHAT THIS DOES AND DOES NOT CLAIM. Three separate things:

1. STRUCTURAL FIXES (the real blockers, see the earlier entries):
   - compiler-emitted switch tables of byte-exact C bodies are placed at their
     retail table address via `.rodata` yaml entries, as `field` does; tables
     belonging to bodies that are still assembly stay in the assembled data
     where `jlabel` exports their `.L` targets. This removed the +0x130 rodata
     excess AND the 89 unresolved `.L800xxxxx` references, with no label
     definitions, no absolute-address substitutions and no deleted relocations
     (`linker/battle.ld` contains exactly one absolute assignment, the
     pre-existing `_gp`, and zero `.L80` references).
   - TUs that mix assembly and C are split into one object per retail
     `[asm run][C run]`, because cc1 emits every file-scope INCLUDE_ASM before
     every compiled body.
2. FUNCTIONS MATCHED TO THE BYTE this session, using the 11-idiom catalogue in
   the earlier entries: all of `main14` (7 functions), `main17/func_8007A9D0`
   and `func_8007AA1C`, `main19/func_8007AE38` and `func_8007AE98`,
   `main38_p1/func_80085EB4` work, `main72_p1/func_800AA820`, plus the
   `mainc15/mainc25/mainc29` CDK preset corrections.
3. THE 39 FUNCTIONS WHOSE C BODIES STILL DO NOT MATCH are now assembled from
   retail bytes instead of being compiled. This is the normal matching-decomp
   state, and it is the correct state: a non-matching C body in a *matching*
   build is precisely what made the ROM wrong. Each one keeps its C body under
   `#ifdef XENO_PC_PORT` (both the pc_port tests and `tools/remu/run_diff.sh`
   define it) with a comment pointing at the asm segment, so no decompilation
   work or differential harness was lost. `config/battle.yaml` gained 21
   standalone `asm` segments covering exactly those address ranges.

HONEST COVERAGE NUMBERS for the byte-exact overlay: of 857 battle functions,
265 (30.9%) are compiled from C and byte-exact; 592 are retail assembly, either
`INCLUDE_ASM` inside a C TU or one of the standalone `asm` segments. Byte
exactness is NOT a claim that the overlay is fully decompiled.

MECHANISM NOTES for whoever continues the decompilation:
- `BATTLE_TU_PART` / `BATTLE_PART(n)` splits a TU at `[asm run][C run]`
  boundaries; `BATTLE_TU_SUB` / `BATTLE_SUB(n)` splits a part further when a
  retail-asm gap separates two runs of byte-exact C. Part 0 / sub 0 (the plain
  file) compiles everything, which is what the port and the tests use.
  Wrappers are `src/battle/<tu>_p<N>.c` and `src/battle/<tu>_q<N>.c`; all are
  listed in `build_port.sh` REFERENCE_ONLY_GAME_TUS (battle `main*` TUs are all
  reference-only for the port, which executes `disc/battle.bin`).
- An INCLUDE_ASM path must name the SEGMENT, not the source file: splat writes
  per-function asm to `asm/battle/nonmatchings/<segment>/`. When a segment is
  renamed or split, move its INCLUDE_ASM lines into the wrapper and update the
  path, or the assembler fails with `can't open ...` while maspsx still exits 0
  and ninja reports only the later `cannot find ....c.o`.
- splat classifies a function as `matchings/` if the C file TEXTUALLY defines
  it, even inside a false `#ifdef`. That is why the 39 reverted bodies use
  standalone `asm` yaml segments rather than `#else INCLUDE_ASM(nonmatchings/...)`.

TO RESUME DECOMPILATION: the 39 functions are listed with their TU and address
in `.xeno-tmp/battle-pass/exactmap.json` (regenerate with `exactmap.py` after a
build). Un-revert one by deleting its `asm` segment from `config/battle.yaml`,
removing its `#ifdef XENO_PC_PORT` guard, re-splitting the TU if needed, and
confirming `battle.bin` stays `1830b4ef...`. The tooling
(`tu_exact.py`, `func_cmp.py`, `fdiff.py`, `tu_build.sh`, `variants.py`,
`metrics.py`, `exactmap.py`, `plan.py`, `gen.py`, `guard.py`, `subsplit.py`)
lives in `.xeno-tmp/battle-pass/` and is rebuildable.

BC460 unchanged and re-verified: `mainc115.c.o` `4549ebf7...`, `mainl115.o`
`80232ffa...`. The port still aborts at the pre-existing
`unclassified port source is not in PORT_SOURCES: pc_port/src/battle_target_bounds.c`,
before its link, exactly as before this work. `disc/*.bin` untouched.
No commit, stage or push.

### Battle decompilation resumed on the byte-exact overlay: +20 functions (2026-09-15)

`battle.bin` is STILL `1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291`,
343936 bytes, `cmp` clean, `make rom-check` PASS. Verified from clean twice
during this pass and once at the end. slus/field unchanged known-red; no pin
touched; nothing staged or committed.

COVERAGE MOVED 265 -> 285 byte-exact compiled functions (30.9% -> 33.3% of 857).
Coexistence bodies 42 -> 49. Functions with no C body at all: 525.
Count methodology (the previous `nm`-based count was WRONG and inflated to 815):
an INCLUDE_ASM'd function is ALSO defined in its TU's object, so `nm` cannot
tell compiled bodies from assembled ones. Count `.ent <fn>` in cc1's output
(`build/src/battle/*.c.s`) instead -- cc1 emits `.ent` only for bodies it
actually compiled.

THE RULE THAT GOVERNS THIS WORK, now that the overlay is byte-exact: a newly
decompiled body may only be installed as plain C if it is byte-exact. Anything
else goes in the coexistence pattern (`#ifdef XENO_PC_PORT` body + `#else
INCLUDE_ASM`), which keeps the decompilation for the port while the matching
build assembles retail bytes. Installing a non-exact body silently breaks the
ROM -- during this pass five such bodies were installed and caught only by the
per-TU oracle before the full build.

COEXISTENCE IS SAFE ONLY IN A TU'S LEADING ASM RUN. cc1 emits every file-scope
INCLUDE_ASM before every compiled body, so re-adding an INCLUDE_ASM for a
function that sits *after* compiled bodies would move its bytes to the front of
the object. Every function targeted this pass was picked from the leading asm
run for exactly this reason (the earlier 39 reverted bodies needed standalone
`asm` yaml segments because they were NOT). The `matchings/` vs `nonmatchings/`
caveat in the previous entry is narrower than it reads: splat keeps
`nonmatchings/<seg>/<fn>.s` for a function whose C definition is inside a false
`#ifdef` (verified: `asm/battle/nonmatchings/main21/func_8007B578.s` exists
alongside main21.c's guarded body), so `#ifdef`/`#else INCLUDE_ASM` does work.

BYTE-EXACT THIS PASS (14 functions, installed as plain C):
  main4/func_80076A10      mainc88/func_800B63F0    main21/func_8007B8D4
  mainc18/func_8007ADB0    mainc116/func_800BCAFC   mainc131/func_800BF954
  mainl92/func_800B6B98    mainc118/func_800BDC14   mainc130/func_800BF73C
  main66/func_800A2CA4     main70/func_800AA6E0     main91/func_800B69E4
  main2/func_80076418      main32/func_8007FD38

DECOMPILED BUT NOT YET BYTE-EXACT (7, installed as coexistence; all are the
register-allocation/scheduling class, and five are already the right SIZE):
  main58/func_8009C050  -4 bytes, 30w   main62/func_8009E364  size ok, 2w
  main63/func_8009E48C  size ok, 2w     mainc89/func_800B6464 size ok, 21w
  main111/func_800BAEB8 -8 bytes, 34w   main41/func_80089B50  size ok, 20w
  mainc126/func_800BF2B8 size ok, 9w
  - main62/main63 differ ONLY in which register holds the `mflo` result.
  - main111: retail materialises D_800C3EB0 once and adds 0x8C8C as a LOADED
    constant (the offset exceeds the addiu immediate range); every attempt to
    stop cc1 constant-folding base+0x8C8C into a fresh %hi/%lo pair failed.
  - mainc89: retail keeps the loop limit in its own register (`move t2,v0`);
    adding an explicit second bound variable fixed the size but not the order
    in which the two packed fields are masked.

NEW IDIOMS CONFIRMED (extending the catalogue in the earlier entries):
12. A two-way constant select compiles to "set the branch-taken value in the
    delay slot, then fall through to the other" -- so write
    `x = A; if (cond) x = B;` rather than `if (!cond) x = A; else x = B;`
    (main32/func_8007FD38 went exact on that alone).
13. Retail often returns the register that happened to hold a COMPARE CONSTANT.
    `func_80089B50`'s `lo == 0xFFFF` path returns 0xFFFF because that is what
    `li v0,0xFFFF` left in v0 -- it is not an uninitialised read. Look for this
    before modelling a path as returning garbage.
14. Compute a value at the statement whose branch should carry it in its delay
    slot: moving `span = hi - lo` next to the `lo == hi` test took
    func_80089B50 from 144 to the correct 156 bytes.
15. Argument-register reuse: several of these functions keep the incoming
    pointer in `a1`/`t1` and walk it, so a `u8* r = a1 + offset` local models
    retail better than indexing the parameter.

TOOLING FIX WORTH KEEPING: `.xeno-tmp/battle-pass/variants.py` now locates the
replaced span by BRACE MATCHING from START when END is absent, instead of a
text marker. The old `END='\n\n'` markers silently truncated mid-function and
produced a cascade of bogus BUILDFAILs (parse errors, duplicate externs) that
look like source bugs but are harness bugs. It also now treats a missing object
file as a build failure, because maspsx can exit 0 without writing one.
New helper `.xeno-tmp/battle-pass/coexist.py <tu> <func> [<segment>]` applies
the coexistence wrapper mechanically.

NEXT: the boundary-candidate list (leading-asm-run functions, smallest first)
is reproducible with the snippet in this pass; unprocessed near-term targets
include `main/func_80071964` (164 B), `mainc124/func_800BED4C` (156 B),
`main73_p2/func_800AFF9C` (196 B, builds a 3-halfword vector on the stack),
`main55/func_8009AA44`, `main8/func_8007887C`, `main53/func_80099FB0`.
SKIP `mainc112/func_800BB620`: it switches `$sp` onto a heap block exactly like
`main125/func_800BEE2C`, which is not expressible in portable C.

### Battle decompilation, second resumed pass: 287 byte-exact, ROM still clean (2026-09-15)

`battle.bin` remains `1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291`,
`cmp` clean. Coverage 285 -> 287 byte-exact compiled functions (33.5% of 857);
coexistence 49 -> 52; no C body at all 525 -> 520.

BYTE-EXACT THIS ROUND: `main8/func_8007887C`, `main55/func_8009AA44`.
COEXISTED (decompiled, logic-faithful, not byte-exact):
  main53/func_80099FB0   +4 bytes, 10w
  mainc84/func_800B5854  size exact, 24w
  main75/func_800B15D8   +16 bytes, 44w

MORE CONFIRMED IDIOMS:
16. Scope a pointer reload rather than reusing one local. `main55/func_8009AA44`
    went exact only when the second `D_800C34B0 + off` read lived in its own
    nested block; reusing a single `u8* p` across both reads cost 3 words, and
    initialising `p` at declaration cost 8 bytes.
17. A flag that must land in a CALLEE-SAVED register has to be live across the
    call. `mainc84/func_800B5854` was 8 bytes short until `dirty` was declared
    and zeroed BEFORE `func_800B57E4`; retail keeps it in `s3` precisely
    because it spans the call.
18. Fold a constant byte offset into the OFFSET, not the pointer. Retail's
    `func_800B15D8` does `addiu v0,v0,12` then `addu t0,a0,v0`, so the loads
    are `0(t0)`, `4(t0)`; writing `a0 + idx * 28 + 0xC` lets cc1 reassociate
    and address them as `12(t0)`, `16(t0)`. (Making the offset a named local
    did NOT fix it here -- still open, and it got worse, 180 -> 196.)

HARNESS TRAP worth remembering: `variants.py`'s brace matching starts at the
first `{` after START. If START is a `typedef struct {...}` line the span ends
at the STRUCT's closing brace, so the replacement is inserted while the old
function stays -- the build then fails with `redefinition of <fn>`, which looks
like a source bug. Point START at the function's leading comment.

STILL SKIPPED, with reasons:
- `main57/func_8009AFD8` uses `jtbl_80070428`. A compiled body would emit its
  own switch table, so it needs a `.rodata` yaml entry at the retail table
  address (file 0x938) and the assembled copy removed; that is the procedure in
  the BYTE-EXACT entry above, but it is a layout change, not a transcription,
  so it was left for a dedicated pass.
- `mainc112/func_800BB620` switches `$sp` onto a heap block (as
  `main125/func_800BEE2C`); not expressible in portable C.

NEXT TARGETS (leading-asm-run, unprocessed): `main/func_80071964` (164 B),
`mainc124/func_800BED4C` (156 B, a dead `lw` of D_800C3610 near the end is the
interesting detail), `main73_p2/func_800AFF9C` (196 B, builds a 3-halfword
vector on the stack), `main101/func_800B8284`, `main87/func_800B62C8`,
`main59/func_8009CA90`, `mainc85/func_800B5B3C`, `main30/func_8007E7E4`,
`main65/func_800A216C`. Regenerate the full candidate list with the
boundary-scan snippet from the previous entry.

No commit, stage or push.

### Battle decompilation, third resumed pass: 288 byte-exact (2026-09-15)

`battle.bin` still `1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291`,
`cmp` clean. 287 -> 288 byte-exact; coexistence 52 -> 54; no C body 520 -> 517.

BYTE-EXACT: `main65/func_800A216C`.
COEXISTED: `main59/func_8009CA90` (size exact, 2w), `main/func_80071964`
(size exact, 38w).

IDIOM 19 (this is the one that mattered): copy a pointer PARAMETER into a local
BEFORE the guard that uses it, when retail reads the guard through a
callee-saved register. `main65/func_800A216C` was 4 bytes short with
`if (idx < *(u16*)(a1 + 0xA)) { u8* e = a1 + idx * 124; ... }`; it went exact as
    u8* e = a1;
    if (idx < *(u16*)(e + 0xA)) { e += idx * 124; ... }
because retail does `move s0,a1` first and then reads `lhu v0,10(s0)`. Moving
the copy inside the if, or writing `e = a1 + idx * 124` in one expression, both
cost the `move`.

TWO NEGATIVE RESULTS, recorded so they are not re-attempted blind:
- `addu` operand order for pointer+offset is NOT reachable from source order.
  `main59/func_8009CA90` needs `addu v1,a0,v1` (base first) and emits
  `addu v1,v1,a0` for every formulation tried: `p + off`, `off + p`,
  `(u8*)((u32)p + off)`, offset hoisted to a local, index hoisted to a local,
  and re-deriving from the global. Only the first two keep the right SIZE; the
  hoisted-local forms distort the whole function (216 -> 168/188). Same family
  as the `mult` operand-order class.
- Forcing retail's "materialise the symbol address into a callee-saved register
  and use 0(reg)" is still unsolved. `main/func_80071964` keeps
  `&D_800D39C0` in `s0` across three uses; cc1 constant-folds every attempt
  back into `lui`/`%lo` pairs, and also picks one callee-saved register where
  retail uses two (frame 24 vs 32). Same wall as `main111/func_800BAEB8`.
  A future attempt should probably come at this from the DATA side -- model the
  strip as a struct starting at D_800D39B8 (the LoadImage argument is
  `s0 - 8`, i.e. a RECT followed by the buffer word) rather than trying to
  out-manoeuvre the folding.

SKIPPED THIS PASS, with reasons:
- `main87/func_800B62C8` switches `$sp` onto a heap block (third instance of
  this pattern, after `main125/func_800BEE2C` and `mainc112/func_800BB620`);
  not expressible in portable C. Treat `sw sp,0(reg)` / `addu sp,reg` in a
  listing as an automatic skip.
- `main101/func_800B8284` is pure PsyQ display/draw-env setup: four
  SetDefDispEnv/SetDefDrawEnv calls then eight halfword stores that interleave
  the two DISPENVs field by field (y, w, x, h to each in turn, relative to
  D_800C4A7C and D_800C4A7C+0x4070). It needs real DISPENV/DRAWENV typing to
  stand a chance and the store interleaving is the whole difficulty; left for a
  pass that does the types properly rather than raw offsets.
- `mainc124/func_800BED4C`: analysed but not attempted. Its D_800C3610 reloads
  group unevenly (one load serves +0x48 and +0x30, the next +0x49/+0x2C/+0x4,
  the next +0x34) and there is a genuinely DEAD `lw D_800C3610` before the
  tail, so the source almost certainly is not the flat store sequence it looks
  like. Worth a careful pass, not a guess.

NEXT (leading-asm-run, still unprocessed): `main73_p2/func_800AFF9C` (196 B,
builds a 3-halfword vector on the stack -- the struct-on-stack idiom from
main125/func_800BEF24 should apply), `mainc85/func_800B5B3C` (220 B, two
func_800B5AC4 calls into stack temporaries then a delta), `main30/func_8007E7E4`
(200 B), `main57/func_8009AFD8` (jump table -- needs the .rodata placement
procedure, own pass).

No commit, stage or push.

### Integration sweep of uncommitted work: the port build is unblocked (2026-09-16)

Surveyed everything on disk against what the builds actually reference. Two
things were integrated, one latent bug was found and fixed, and six items are
reported below as needing a decision rather than a patch.

INTEGRATED 1 -- THE PORT BUILD IS GREEN AGAIN. `pc_port/build_port.sh` had been
aborting in stage 2b at `unclassified port source is not in PORT_SOURCES:
pc_port/src/battle_target_bounds.c` since before this session. FIVE finished
files were in no ownership list at all:
  battle_target_bounds.c  battle_target_camera.c  battle_target_eligibility_ram.c
  battle_target_list_ram.c  battle_target_setup.c
They are verification fixtures, not runtime code, and that was established
before changing anything:
  - `grep -rln '#include "battle_target' pc_port --include=*.c --include=*.h`
    lists only those five files and `pc_port/tests/*.c`; NO runtime port source
    includes any of their headers.
  - each is compiled directly by its own `pc_port/tests/run_*.sh` (9 scripts
    name them explicitly).
  - three of their tests were run and pass.
So the fix is a classification, which is exactly what the error message asks
for -- not a deletion, and not an addition to PORT_SOURCES (that would change
the link's symbol set). `build_port.sh` gained a `TEST_ONLY_PORT_SOURCES`
registry mirroring the existing game-TU exclusion machinery: per-file reasons
via `test_only_port_source_reason`, an `is_test_only_port_source` predicate,
`print_test_only_port_sources` reporting in stage 2b, and two guards -- a
test-only entry must exist on disk, and must NOT also appear in PORT_SOURCES.
RESULT: `bash pc_port/build_port.sh` now reaches
`LINK OK -> pc_port/build_native/xeno-port (port-owned addresses verified)`,
exit 0, zero ERROR lines, 48 function stubs / 581 data symbols (unchanged).

INTEGRATED 2 -- REMOVED A DUPLICATE DEFINITION I HAD INTRODUCED. With the port
link reachable again it immediately reported
`multiple definition of 'func_80025180'`. The coexistence body added to
`src/slus_006.64/system/temp1.c` earlier in this session collided with
`pc_port/src/game_overrides.c:3493`, which already implements that function as a
host-safe port override (it treats `g_GfxCurWorkBuffer` as a `u32`, not a
pointer). The port legitimately owns the symbol, so the port-side branch was
removed and the function is a plain `INCLUDE_ASM` again, with the retail
algorithm, the $gp-addressing reason it is not worth a C body (84 vs 72 bytes),
and a pointer to the override kept as a comment. NOTE THE LESSON: this bug was
invisible while the port aborted in stage 2b -- a fail-closed check that fires
early can hide a real defect behind it, so re-run the whole pipeline after
clearing any such abort. `build/out/slus_006.64` is back to `021278a9...`,
bit-identical to its pre-session hash.

BOTH BUILDS RE-VERIFIED AFTER THE CHANGES: `make build` exit 0, `battle.bin`
`cmp` clean against `disc/battle.bin`; `run_battle_target_bounds_test.sh` still
passes; `pc_port/build_native/xeno-port` relinked (5062616 bytes).

FOUND BUT NOT INTEGRATED -- these need a scope decision, not a patch:
1. `movie` is decompiled but absent from the matching build. `src/movie/main.c`
   is 1371 lines, `config/movie.yaml` and `disc/movie.bin` both exist, and the
   PORT does compile and link it (`src_movie_main.c.o` is in the port objects),
   but `movie` is NOT in `gears.toml` `overlays`, so `make build` never builds
   it and `config/checksum.sha` has no pin for it. Wiring it in would add a new
   rom-check artifact that is probably red, so it is a deliberate call.
2. `battling` likewise: `config/battling.yaml`, `src/battling/main.c` (169
   INCLUDE_ASM), `disc/battling.bin`. Not an overlay; explicitly excluded from
   the port as a "retail state-4 MIPS overlay scaffold".
3. Unwired configs with no overlay entry: `battle_command_file1`, `movie_player`,
   `overlay-template` (the last is clearly a template, not an omission).
   `disc/world_map.bin` has no config at all despite a large body of
   `pc_port/src/world_map_*.c` work.
4. `tools/tests/test_gen_port_stubs.py` runs 5 tests and they all PASS, but
   nothing references it -- there is no `make test` target and no aggregate
   runner. `tools/scripts/gen_port_stubs.py` is itself modified. An orphaned
   green test guarding modified code is worth wiring up once a test convention
   is chosen.
5. CI IS OUT OF SYNC WITH gears.toml IN BOTH DIRECTIONS.
   `.github/workflows/validate.yaml` fetches five disc images -- SLUS_006.64,
   movie.bin, field.bin, member_change_menu.bin, shop_menu.bin -- then runs
   `make build` and `make check`. But `gears.toml` lists `battle` and `menu` as
   overlays and neither image is fetched, while `movie.bin` IS fetched and is
   not an overlay. `config/checksum.sha` now pins battle too, so `make check`
   cannot pass as written. Fixing this needs the secret URL mapping, which is
   not visible here.
6. `tools/remu/` (the MIPS-I retail-byte oracle, 125 test files) is untracked
   and referenced only from docs and its own `run_diff.sh`.

No commit, stage or push. `disc/*.bin` and `config/checksum.sha` untouched.
