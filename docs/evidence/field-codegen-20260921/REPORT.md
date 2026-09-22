# field.bin is genuinely red, and it is CODEGEN — not layout

Measured 2026-09-21 in worktree `wt-bcf1-layout` at commit `f6991592` (which
carries the `symbol_addrs.slus_006.64.txt` semicolon fix, so splat actually
ran: `grep -icE "Traceback|AssertionError"` over the build log = **0**).

Unlike `battling`, `world_map` and `battle_command_file1`, this one is real.

## The four numbers

From a clean `make build` followed by `ninja build/out/field.bin`:

| | value |
|---|---|
| `stat -c%s build/out/field.bin` | **249,442** |
| `stat -c%s disc/field.bin` | **260,862** |
| `cmp -l disc/field.bin build/out/field.bin \| wc -l` | **219,198** (+ EOF after 249,442) |
| `sha256sum build/out/field.bin` | `e91869555a1b88d6e9c8dcd6a20d468a0404cdf6a3f8e75461e795fb313575aa` |
| `sha256sum disc/field.bin` | `38a1ce829a6f094c505f67143d6ace2d328418c65425a7383991179467e1fdfc` |

The retail hash is what `config/checksum.sha` pins, so the gate is correctly
red. field is **11,420 bytes short** and 87.9% of the overlapping bytes differ.

## Verdict: codegen. Four independent lines of evidence.

### 1. All three world_map split rules are ALREADY satisfied in `config/field.yaml`

- leading subsegment is `rodata` at `0x0`, not `data`
- trailing data is its own subsegment: `- [0x3DF78, data]`
- the non-multiple-of-4 tail already has its `databin`:
  `- [0x3FAFC, databin]` / `- [0x3FAFE]`, and `0x3FAFE` = 260,862 = retail size

There is no layout rule left to apply. The config already describes retail's
geometry exactly.

### 2. The deficit is spread across 20 of 26 C translation units, in BOTH directions

Per-TU retail text size (from consecutive `c` subsegment offsets in
`config/field.yaml`) vs built `.text` size (from `build/out/field.map`):

| TU | retail | built | delta |
|---|---|---|---|
| main/misc3 | 7,288 | 7,400 | **+112** |
| effects/fade | 1,156 | 1,156 | 0 |
| main/init | 600 | 600 | 0 |
| main/misc2 | 21,632 | 19,976 | -1,656 |
| main/main | 5,532 | 5,488 | -44 |
| main/misc4 | 19,936 | 18,540 | -1,396 |
| effects/fade_render | 956 | 956 | 0 |
| dialogue/text_box_render | 11,076 | 8,664 | -2,412 |
| main/misc8 | 25,936 | 24,708 | -1,228 |
| main/misc | 36,012 | 35,256 | -756 |
| camera/camera_movement | 7,948 | 7,940 | -8 |
| main/misc11 | 17,888 | 17,976 | **+88** |
| game_logic/gold | 340 | 340 | 0 |
| main/input_script_handlers | 412 | 412 | 0 |
| main/misc10 | 1,072 | 1,076 | **+4** |
| game_logic/scenario_flags | 512 | 512 | 0 |
| party/stats | 2,736 | 2,740 | **+4** |
| main/misc7 | 18,456 | 17,808 | -648 |
| dialogue/text_box | 5,220 | 5,284 | **+64** |
| scripts/variable_handlers | 2,732 | 2,732 | 0 |
| main/misc6 | 15,636 | 15,040 | -596 |
| scripts/virtual_machine | 12,452 | 11,408 | -1,044 |
| effects/distortion | 3,556 | 3,100 | -456 |
| main/misc5 | 15,548 | 14,336 | -1,212 |
| effects/particles | 5,992 | 5,996 | **+4** |
| main/misc9 | 9,288 | 9,048 | -240 |
| **TOTAL** | **249,912** | **238,492** | **-11,420** |

The total text deficit (-11,420) is *exactly* the file size deficit
(260,862 - 249,442 = 11,420). So no rodata, data or bss bytes are missing —
the entire shortfall is compiled C code. A layout bug produces one shift or a
wrong section order; it does not produce 20 independent per-TU deltas, six of
them **positive**.

### 3. field contains ZERO `INCLUDE_ASM` directives — every function is hand-written C

`grep -rn INCLUDE_ASM src/field --include=*.c` returns 3 hits and **all three
are comments** (`misc.c:2311`, `misc.c:2806`, `misc4.c:277`, each describing a
function that *used to be* INCLUDE_ASM). There is not one live `INCLUDE_ASM`
site in the overlay.

This is the structural difference from the three overlays this wave cleared.
`battling` / `world_map` / `battle_command_file1` are ~99% INCLUDE_ASM, so
their bytes were retail bytes by construction and only placement could be
wrong. In field, **all 879 functions are compiler output**, so every byte is
subject to codegen divergence.

### 4. 554 of 878 functions are already byte-exact — impossible under a layout bug

Per-function comparison of each object's `.text` against
`asm/field/matchings/<tu>/<func>.s`, with relocated words masked out (so
link-time fixups are not counted as differences):

| | functions |
|---|---|
| byte-exact (mod relocations) | **554** |
| right size, wrong instructions | **90** |
| wrong size | **234** |
| **divergent total** | **324 of 878 (36.90%)** |
| match rate by function | 63.10% |

**Cross-check:** byte-weighting that same result gives 86,424 / 250,280 retail
text bytes in byte-exact functions = **34.53%**, against the handoff's
independently-derived objdiff figure of **34.71%** for field. Two unrelated
methods agreeing to 0.2pp says the comparator is sound.

If the mismatch were layout, essentially zero functions would be byte-exact.
63% being exact means placement is right and specific *bodies* are wrong.

The 219,198 differing bytes in the linked binary are a downstream consequence,
not the cause: the first short function shifts every later address, so even
byte-exact functions land at the wrong offset and read as different.

## How many distinct functions: 324, across 19 TUs

| TU | divergent / total |
|---|---|
| main/misc | 81 / 198 |
| main/misc11 | 38 / 102 |
| main/misc8 | 37 / 58 |
| main/misc7 | 34 / 91 |
| main/misc2 | 23 / 44 |
| main/misc4 | 21 / 42 |
| main/misc6 | 17 / 63 |
| main/misc5 | 17 / 36 |
| main/misc9 | 13 / 27 |
| dialogue/text_box_render | 10 / 17 |
| scripts/virtual_machine | 10 / 29 |
| main/misc3 | 5 / 11 |
| main/main | 5 / 14 |
| dialogue/text_box | 3 / 19 |
| camera/camera_movement | 3 / 30 |
| main/misc10 | 2 / 9 |
| effects/distortion | 2 / 4 |
| party/stats | 2 / 16 |
| effects/particles | 1 / 15 |

Full list with per-function retail size, built size and divergent-word count:
`divergent_functions.txt` (same directory).

Worst offenders by byte delta:

| TU | function | retail | built | delta |
|---|---|---|---|---|
| dialogue/text_box_render | func_8007E1C0 | 3,148 | 1,956 | -1,192 |
| main/misc5 | func_800A84C0 | 1,764 | 664 | -1,100 |
| scripts/virtual_machine | func_800A28D4 | 1,772 | 1,196 | -576 |
| main/misc2 | func_80076AC0 | 1,776 | 1,232 | -544 |
| main/misc4 | func_8007C694 | 1,736 | 1,276 | -460 |
| effects/distortion | FieldDistortionDraw | 2,060 | 1,612 | -448 |
| main/misc4 | func_8007BEF4 | 1,948 | 1,504 | -444 |
| dialogue/text_box_render | func_8007F8DC | 1,804 | 1,396 | -408 |
| dialogue/text_box_render | FieldTextBoxInitializePrimitives | 1,952 | 1,584 | -368 |
| main/misc2 | func_800748E8 | 2,340 | 1,972 | -368 |

Deltas of -1,192 and -1,100 are not register allocation. Those C bodies are
missing whole branches of behaviour — they are incomplete reconstructions, not
near-misses.

## One structural oddity worth a separate look

`main/misc3` has a function the retail overlay does not: **`IsLiveHeapBlock`**
(879 built symbols vs 878 retail). It accounts for misc3's +112. Either retail
inlined it or it is an invention of the reconstruction. Cheap to check and it
is the only extra symbol in the overlay.

## Recommendation — not started, per dispatch constraints

Per `PORT_GOAL_AND_PLAN.md`, byte-matching gameplay logic is explicitly not
worth holding work for, so no function bodies were touched. But note the two
findings do not point the same way:

- The **byte-parity** story is a 324-function register-allocation grind, and
  the plan says don't.
- The **port-correctness** story is different and more urgent: several of these
  bodies are short by 400-1,200 bytes, which means the C that the PC port
  actually executes is missing logic present in retail. That is a
  *behavioural* gap in shipped code, not a cosmetic byte gap, and it is not
  covered by the "don't chase register allocation" exemption.

Suggested triage: ignore the 90 same-size/different-instruction functions
(that really is regalloc), and audit the ~20 functions short by >200 bytes for
missing behaviour.

## Reproducing

```
python3 scratch_fieldanalysis/bytecmp.py     # per-function comparison
```
Requires `build/field_disasm.txt`, produced inside the toolchain container by
`scratch_fieldanalysis/dump.sh` (`mips-linux-gnu-objdump -d -r --section=.text`
over every `build/src/field/**/*.c.o`).
