# Where field.bin's 18,240-byte shortfall actually is

Date: 2026-09-06 UTC. Analysis only; no source change. No commit/stage/push.

## The headline correction

`tools/scripts/decomp_status.py` reports the field overlay at **99.0% done**
(878 functions, 9 unported, 14 coexistence). That number is **misleading for
byte parity**, and the tool says so itself in its own Limitations section:
"MATCHED {} is inferred, not oracle-verified... This assumes every
non-INCLUDE_ASM'd function is byte-exact."

It is not. **161 functions that the tool counts as matched compile SHORTER than
retail**, by a combined **20,844 bytes**. A further 64 compile LONGER by 1,292
bytes. Net ≈ 19,552 bytes of code deficit, which accounts for the
`disc/field.bin` (260,862) vs `build/out/field.bin` (242,622) gap of **18,240
bytes** once data/alignment is included.

This also explains the "functions are relocated" observation in
`docs/evidence/blackmoon-object-overlay-verification-20260906/`: the text
section is short, so everything downstream shifts.

## AUTHORITATIVE VIEW: per translation unit (evidence class: proven)

The per-function table further below is **distorted** and should not be used to
prioritise: `nm` attributes a static helper to its own symbol, so a TU that
factors retail's inlined code into helpers shows a large fake "deficit" on the
parent function while the TU's total is unchanged. Per-TU totals are immune to
that. Built `.text` per object comes from `build/out/field.map`; retail TU size
is the gap between consecutive `c` subsegment offsets in `config/field.yaml`.

| deficit | translation unit | retail | built |
| --: | --- | --: | --: |
| 4100 | `src/field/main/misc2.c` | 21632 | 17532 |
| 3988 | `src/field/dialogue/text_box_render.c` | 11076 | 7088 |
| 3784 | `src/field/scripts/virtual_machine.c` | 12452 | 8668 |
| 2012 | `src/field/main/misc8.c` | 25936 | 23924 |
| 1764 | `src/field/main/misc4.c` | 19936 | 18172 |
| 1156 | `src/field/main/misc.c` | 36012 | 34856 |
| 848 | `src/field/main/misc6.c` | 15636 | 14788 |
| 556 | `src/field/main/misc7.c` | 18456 | 17900 |
| 456 | `src/field/effects/distortion.c` | 3556 | 3100 |

**Net TU deficit = 18,528 bytes**, which reconciles with the file-level gap of
18,240 (the remainder is rodata/alignment). The top three TUs are 11,872 bytes
= **64% of the entire shortfall**.

Sanity check on the measurement: `func_800A1364`, independently proven
byte-exact this session, measures exactly 396 bytes (0x18C) in the built ELF —
so the built-size side is sound.

## Method (evidence class: proven, with a stated caveat)

- Retail size per function: count instruction lines in the generated
  `asm/field/**/*.s` (one `/* offset addr word */` comment per instruction),
  x4 bytes. `asm/` is gitignored and produced by `gears matching`.
- Built size per function: `mips-linux-gnu-nm -S build/out/field.elf`
  (symbol sizes), from the pinned-splat matching build described in
  `docs/evidence/matching-baseline-pinned-splat-20260906/`.
- 887 retail asm functions, 1065 built text symbols, **878 comparable**.

**Caveat:** this compares symbol SIZES, not bytes. A function of identical size
may still differ instruction-for-instruction, so these 161 are a lower bound on
non-exact functions, not the complete set. Conversely a few entries could be
measurement artifacts (jump tables attributed differently, alignment padding).
The magnitude of the top entries — 2,004 bytes on a single function — is far
beyond that noise.

## The worklist

Full data: `short-functions.tsv` (161 rows), `oversized-functions.tsv` (64 rows),
both sorted by deficit. Top offenders:

| deficit | symbol | retail | built | ratio |
| --: | --- | --: | --: | --- |
| 2004 | `func_8007E1C0` | 3148 | 1144 | built is 36% of retail |
| 1656 | `func_800A3474` | 2072 | 416 | 20% |
| 1520 | `func_800A3F4C` | 2044 | 524 | 26% |
| 1276 | `func_80075B44` | 2416 | 1140 | 47% |
| 792 | `func_800748E8` | 2340 | 1548 | 66% |
| 736 | `func_8008004C` | 1448 | 712 | 49% |
| 736 | `func_80076AC0` | 1776 | 1040 | 59% |
| 620 | `func_800A28D4` | 1772 | 1152 | 65% |
| 548 | `func_8007CD80` | 1620 | 1072 | 66% |
| 512 | `func_8007BEF4` | 1916 | 1404 | 73% |

These are not near-misses. A body at 20–36% of retail size is an **incomplete
implementation** that compiles and links — the same class as the approximate
`func_801E7D14` that made every archive-6B9 object draw near-black until it was
transcribed properly (see `blackmoon-map16-render-20260906/`). Each is a latent
behavioural gap, not merely a matching gap.

Note `func_800A3474` and `func_800A3F4C` are two of the three `D_800AFC50`
cursor walks identified in `docs/evidence/field-asm-only-20260906/`; the third,
`func_800A3C8C`, is still unconditional `INCLUDE_ASM`. That whole family is
under-implemented.

**The top 20 alone account for 14,260 of the 20,844 deficit bytes**, so a
bounded pass over them would close roughly 68% of the gap.

## Why this matters for the goal

The field checksum gate cannot pass while the overlay is short, so "retail byte
verification" for `field.bin` is blocked behind these 161 bodies rather than
behind the 9 remaining `INCLUDE_ASM` placeholders. Closing the 9 placeholders
would take the overlay to a nominal 100% while leaving ~20KB of incomplete
bodies — the completion metric would look finished and the checksum would still
fail. Prioritise by deficit, not by placeholder count.

## Not established

- Which of the 878 same-size functions are byte-exact (needs an instruction-level
  diff, e.g. `make report` / objdiff, not size comparison).
- Whether any of the 64 oversized functions indicates a real defect rather than
  compiler scheduling.
- Any runtime consequence of a specific short body was not tested here.
