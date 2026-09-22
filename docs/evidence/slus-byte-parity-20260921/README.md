# Where slus_006.64's 224,139 differing bytes actually are

> **CORRECTION (2026-09-21, same day).** The short-function cohort figures in
> this report — "17 functions, 2,424 bytes" and `func_80034FFC` at "29% of
> retail" — are inflated by two measurement artifacts found while triaging
> them: static helpers charged to no caller (1,152 B) and inter-function `nop`
> padding charged to the preceding symbol (72 B, because the retail parser
> below ignores `.size`). Corrected: 12 functions with any deficit totalling
> 1,156 bytes, and `func_80034FFC` at 97% of retail with a 52-byte residual.
> One function in the cohort was genuinely missing behaviour and is now fixed.
> See `../slus-codegen-triage-20260921/README.md`. The per-TU totals here are
> unaffected — they come from the linker map, which has neither problem.

Date: 2026-09-21 UTC. Analysis only; no C source change. Measured on branch
`Blizz127/wt-battling-layout` at `c1ea7b87`, the commit that first made
`build/out/slus_006.64` link.

Nobody has been able to measure this before: until `c1ea7b87` the main
executable did not link, so there was no built artifact to compare.

## VERDICT: layout, not codegen

**The image is overwhelmingly correct code placed at the wrong addresses.**
967 of 1,246 comparable functions (**65.20% byte-weighted**) are byte-exact
once relocations are masked, yet **not one single function sits at its retail
address**. Every `%hi`/`%lo`/`jal` field in the image therefore encodes a
different number than retail, which is what produces a 74% raw byte-difference
rate on code that is largely right.

This is *not* the field overlay's situation. field.bin was **18,240 bytes
short** with 161 functions compiling shorter than retail
(`docs/evidence/field-byte-parity-gap-20260906/`). slus is **388 bytes short**
in total and its code section is only **1,584 bytes** short. There is no large
cohort of missing bodies here.

Because the verdict is layout rather than hand-written C diverging, the
dispatch said to fix it. **I did not**, and the reason is in
"Why this is not a one-line layout fix" below: the residual is not a single
misplaced subsegment, it is ~1,600 bytes of code spread across 19 TUs plus a
~1,200-byte data surplus. That is a matching campaign, not a config edit.

---

## Evidence line 1 — where the differing bytes sit

Uniform, not clustered. Density of differing bytes against
`disc/SLUS_006.64`, in 16 KiB buckets:

| file offset | differing | % of bucket |
| --- | --: | --: |
| 0x0008000 | 10,980 | 67.0% |
| 0x000C000 | 13,954 | 85.2% |
| 0x0010000 | 13,898 | 84.8% |
| 0x0014000 | 14,203 | 86.7% |
| 0x0018000 | 14,569 | 88.9% |
| 0x001C000 | 15,104 | 92.2% |
| 0x0020000 | 14,406 | 87.9% |
| 0x0024000 | 14,003 | 85.5% |
| 0x0028000 | 14,396 | 87.9% |
| 0x002C000 | 13,956 | 85.2% |
| 0x0030000 | 14,110 | 86.1% |
| 0x0034000 | 14,061 | 85.8% |
| 0x0038000 | 12,480 | 76.2% |
| 0x003C000 | 13,630 | 83.2% |
| 0x0040000 | 15,026 | 91.7% |
| 0x0044000 | 13,968 | 85.3% |
| 0x0048000 | 1,395 | 17.9% |

The first 34,948 bytes (through 0x8883) are **identical** — that is the 2 KiB
PSX-EXE header plus the leading pure-`asm` subsegments, which are assembled
verbatim and cannot drift. From 0x8884 the rate jumps straight to a plateau of
76–92% and stays there. Codegen divergence would produce spikes around
specific bodies with quiet regions between; a shifted image produces exactly
this flat plateau. The final bucket drops to 17.9% because the tail is data
that partially realigns.

Corroborating placement measurement, per function:

```
functions at their retail address: 0 of 1246 (0.0%)
distinct shift values: 257
most common shifts (bytes, count):
        -132  176
        -324   76
        +992   64
        -368   58
       +1196   50
```

257 distinct shift values means the drift accumulates and partially cancels
along the image rather than being one displacement. Note `+1196` — it reappears
in the section accounting below as the data surplus.

## Evidence line 2 — per-TU size deltas, and the −388 reconciliation

Retail TU size is the gap between consecutive code subsegments in
`config/slus_006.64.yaml`; built size is the `.text` contribution of that
object in `build/out/slus_006.64.map`. Only nonzero deltas shown.

| delta | retail | built | unit |
| --: | --: | --: | --- |
| −1316 | 10056 | 8740 | `system/system.c` |
| −376 | 9916 | 9540 | `system/rendering.c` |
| −200 | 10472 | 10272 | `psyq/libgte.c` |
| −68 | 3084 | 3016 | `system/work_list.c` |
| −56 | 11880 | 11824 | `system/animation_scripts.c` |
| −56 | 288 | 232 | `system/graphics.c` |
| −32 | 1448 | 1416 | `psyq/libapi.c` |
| −32 | 208 | 176 | `psyq/libapi_2.c` |
| −16 | 252 | 236 | `psyq/libcard.c` |
| −12 | 432 | 420 | `system/asset_loader.c` |
| −4 | 16 | 12 | `psyq/libapi/WaitEvent.c` |
| −4 | 16 | 12 | `psyq/libapi_3.c` |
| −4 | 5524 | 5520 | `system/archive.c` |
| +4 | 1228 | 1232 | `graphics/line_scroll.c` |
| +8 | 5216 | 5224 | `system/libarchive.c` |
| +48 | 2924 | 2972 | `psyq/libc.c` |
| +132 | 27760 | 27892 | `system/temp2.c` |
| +188 | 31664 | 31852 | `system/sound.c` |
| +212 | 21048 | 21260 | `system/temp1.c` |

**Sum of per-TU deltas = −1,584 bytes.** Confirmed independently by totalling
both sides: retail code span 218,476 vs built `.text` across all objects
216,892, difference −1,584 exactly.

Reconciling with the file:

```
.header    2,048
.main    299,324
._49AC0    1,344
built    302,716      retail 303,104      delta -388
```

Code is −1,584, the file is −388, so the non-code part of `.main`
(rodata/data/sdata) is **+1,196 bytes larger than retail**. That is the same
`+1196` that shows up as one of the five most common function shift values, so
the data surplus is directly displacing a large block of code. Closing it is
likely the single highest-leverage layout item, and it is a rodata/data
question — the same class as
`docs/evidence/field-exact-20260910/rodata-surplus-linker-map.md`.

Note the deltas go **both ways**: 6 TUs are larger than retail, 13 smaller.
That two-directional pattern is itself evidence against "missing bodies" — an
incomplete decomp is short in one direction only.

## Evidence line 3 — how many functions are already byte-exact

Two independent measures were run, bracketing the answer, plus objdiff.

| measure | exact functions | byte-weighted | control-set behaviour |
| --- | --: | --: | --- |
| masked (`analyze.py`) | 967 / 1,246 | **65.20%** | 57/57 known-exact pass — cannot under-count |
| objdiff (`make report`) | 806 / 1,223 | **42.98%** | independent tool |
| resolved (`strict.py`) | 608 / 1,118 | **25.41%** | 20/57 known-exact pass — heavy false negatives |

Excluding the 57 INCLUDE_ASM functions (retail asm pasted back in, exact by
construction), the masked figure for genuinely-compiled C is **51.75%**
(112,640 of 217,668 bytes).

**The cross-check does not agree the way the field worker's 34.53% vs 34.71%
did, and I am not going to pretend otherwise.** The three figures bracket
rather than converge. What makes the bracket trustworthy is that each measure
was calibrated against a known-answer control set — the INCLUDE_ASM functions,
which are byte-identical to retail by construction:

- The **masked** pass scores all 57 correctly. It cannot under-count, but it
  ignores *which* symbol a relocation names, so a `lui/%lo` pair loading the
  wrong global still scores exact. It is a genuine upper bound.
- The **resolved** pass recomputes each relocated immediate from the symbol's
  retail address and compares the full word. It scores only 20 of 57, i.e. it
  wrongly rejects ~65% of truly-exact functions, mostly on `.rodata`-relative
  jump tables and static-call targets whose retail addresses are not
  recoverable from our layout. It is a firm lower bound.
- objdiff's 42.98% sits between the two bounds, which is the expected ordering.

So: **between 25.41% and 65.20%, with objdiff's 42.98% the best single
estimate.** Denominators agree closely across all three (217,668 vs 218,072),
so the disagreement is in the numerator — i.e. in how strictly a relocation
counts as matching — not in what is being measured.

## Evidence line 4 — remaining INCLUDE_ASM

**61 live `INCLUDE_ASM` directives** in `src/slus_006.64`, across 61 distinct
function names, counted with `//` and `/* */` comments stripped. For scale,
those 61 placeholders are a small part of the picture: they are already
byte-exact in the built image (the retail asm is pasted in), so closing them
changes the *port*, not the byte gate. The same trap the field report called
out applies here — the completion metric would move and the checksum would not.

---

## BLAST RADIUS of my own `gears.toml` change — small, and measured

`c1ea7b87` removed `system/temp1.` and `system/animation_scripts.` from the
`SdataSmall` preset to clear the GPREL16 truncation. That changes codegen in
those two TUs, so some of the 224,139 could be self-inflicted.

Both TUs were rebuilt under **both** presets and each compared against retail
at the object level (variant A cannot be linked, but it can be compiled and
measured). Full output in `ab-summary.txt`.

| | temp1 A (SdataSmall) | temp1 B (committed) | anim A | anim B |
| --- | --: | --: | --: | --: |
| exact functions | 22 | **19** | 15 | **16** |
| exact bytes | 4,752 | **4,480** | 444 | **488** |
| bytes short of retail | 932 | **776** | 1,056 | **1,000** |
| built .text | 20,868 | **21,204** | 11,724 | **11,808** |
| functions whose bytes changed A→B | 32 of 72 | | 16 of 44 | |

**Net effect of my change: −2 exact functions, −228 exact bytes.** temp1 lost
3 functions; animation_scripts gained 1.

That is **0.16% of the 141,920 byte-exact bytes** and **0.10% of the 217,668
comparable code bytes**. It is not a meaningful fraction of the 224,139.

This number is independently confirmed by objdiff, which knew nothing about the
A/B: the Main Executable went from **808 functions / 93,960 bytes** (recorded in
`docs/evidence/decomp-campaign/DASHBOARD.md` before this change) to **806
functions / 93,732 bytes** after it. That is −2 functions and −228 bytes —
matching the A/B measurement exactly, to the byte.

**Honest caveat, because it cuts against my change:** on temp1 specifically the
old preset was *better* — 22 exact vs 19, and its built size (20,868) was
closer to retail's 21,048 than mine (21,204). temp1 is genuinely mixed: some of
its plain externs sit inside the ±32 KiB window around gp and retail reaches
them gp-relative, others sit outside it and retail reaches them absolutely.
Neither per-TU setting is correct for it. The right fix is the narrower,
**per-symbol** one the dispatch anticipated. I did not attempt it here — the
preset mechanism is per-TU, so per-symbol control would need either a maspsx
change or source-level annotation, both outside this dispatch's scope.

The trade as it stands: 2 functions and 228 bytes of byte-parity, in exchange
for the main executable linking at all and `make check` producing a result.
I think that is worth it, but the cost is real and is now measured rather than
assumed.

## Short-function cohort — small, and concentrated in one TU

The class that matters for the port: functions **short by a large margin** are
missing behaviour, not register allocation. Full lists in
`short-functions.tsv` (168 rows) and `oversized-functions.tsv` (66 rows).

All functions whose built body is under 75% of retail:

| deficit | symbol | retail | built | built/retail | TU |
| --: | --- | --: | --: | --: | --- |
| 1204 | `func_80034FFC` | 1696 | 492 | **29%** | `system/system.c` |
| 396 | `func_80034888` | 1572 | 1176 | 74% | `system/system.c` |
| 316 | `func_80032F54` | 1240 | 924 | 74% | `system/system.c` |
| 88 | `func_80023804` | 312 | 224 | 71% | `system/temp1.c` |
| 80 | `func_800234AC` | 140 | 60 | **42%** | `system/rendering.c` |
| 68 | `func_80030A30` | 228 | 160 | 70% | `system/system.c` |
| 60 | `GfxLoadImageAccelerated` | 100 | 40 | **40%** | `system/graphics.c` |
| 48 | `func_8003014C` | 124 | 76 | 61% | `system/system.c` |
| 44 | `func_800347C0` | 64 | 20 | **31%** | `system/system.c` |
| 32 | `func_80033B34` | 120 | 88 | 73% | `system/system.c` |
| 20 | `NormalClip` | 48 | 28 | 58% | `psyq/libgte.c` |
| 12 | `func_8004E9FC` | 36 | 24 | 66% | `psyq/libgte.c` |
| 12 | `func_8004BED8` | 24 | 12 | 50% | `psyq/libgte.c` |
| 12 | `func_80040C20` | 16 | 4 | 25% | `psyq/libc.c` |
| 12 | `Lzc` | 28 | 16 | 57% | `psyq/libgte.c` |
| 12 | `AverageSZ4` | 28 | 16 | 57% | `psyq/libgte.c` |
| 8 | `func_8004BEC0` | 16 | 8 | 50% | `psyq/libgte.c` |

**17 functions, 2,424 bytes total.** Compare the field overlay's equivalent
cohort: 161 functions and 20,844 bytes. This one is an order of magnitude
smaller and is dominated by a single body, `func_80034FFC` at 29% of retail,
which alone is half of it.

`system/system.c` supplies 7 of the 17 and 2,032 of the 2,424 bytes — and it is
also the largest per-TU deficit (−1,316). **If one file is worth a bounded
pass, it is `system/system.c`, starting with `func_80034FFC`.**

`NormalClip`, `Lzc`, `AverageSZ4` and the `libgte` entries are Psy-Q SDK maths
routines; short bodies there are more likely deliberate C simplifications of
hand-tuned assembly than missing behaviour, and should be judged separately.

**Caveat carried over from the field report, and it bites here:** per-function
deltas sum to −4,012 (168 short by 6,152, 66 long by 2,140) but per-TU deltas
sum to −1,584. The difference is the static-helper artifact — `nm` attributes a
helper to its own symbol, so a TU that factors retail's inlined code into
helpers shows a fake deficit on the parent. **Per-TU is authoritative; do not
prioritise off the per-function table alone.**

## Method

- Retail bytes per function: the `/* fileoff vram WORD */` comment on every
  instruction line in the splat-generated `asm/slus_006.64/**/*.s`. A function
  appearing in both the per-function `matchings/` file and the whole-TU file is
  counted once (first wins) — appending both silently doubles every length, and
  did on the first run of this analysis.
- Built bytes per function: `mips-linux-gnu-objdump -d --section=.main` on
  `build/out/slus_006.64.elf` (the section is `.main`, not `.text`).
- Relocation masking: immediates of `lui/addiu/ori/andi/xori`, all loads and
  stores, and the coprocessor load/stores. `jal` is **not** blanket-masked —
  it must reach the same callee symbol, and a local `j` the same in-function
  offset, or a call to the wrong function would score as exact.
- Per-TU sizes: `build/out/slus_006.64.map` against the subsegment spans in
  `config/slus_006.64.yaml`.
- objdiff: `make report` then `build/progress.json`.
- All measurements taken inside the pinned toolchain container with the
  worktree mounted as project root.

Scripts, re-runnable from the project root inside the container:
`analyze.py` (masked pass, per-TU tables, placement, density),
`strict.py` (relocation-resolved pass), `ab_sdatasmall.py` (the A/B blast
radius), `objdiff_summary.py` (objdiff figures).

## Build state after this analysis

`make check` re-run from clean at the end: **7 of 10 OK**, unchanged.
`slus_006.64`, `field.bin` and `menu.bin` FAILED; nothing regressed.

## Not established

- Whether the +1,196 byte data/rodata surplus is one block or many. It is the
  next thing to measure and probably the largest single layout lever.
- Whether any of the 66 oversized functions is a real defect rather than
  scheduling.
- Any runtime consequence of `func_80034FFC` being 29% of retail was not
  tested. It is flagged on size alone.
- The correct per-symbol replacement for the `SdataSmall` per-TU preset.
