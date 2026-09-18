# `func_80034FFC` retail differential — 2026-09-06

This is a scratch-only differential for the native `src/slus_006.64/system/system.c`
implementation of `func_80034FFC` against the supplied raw retail instruction
slice.  No repository, build configuration, runtime, UI, or production files
were changed.

## Pinned inputs

* Retail oracle: `retail-glyph.bin`, 1696 bytes, SHA-256
  `3591ad76c469c1d359529e264ad5aa81c0f1814327c68307c0ddd69e41e91a34`.
  It is loaded at guest `0x80034FFC`; the pinned range is
  `0x80034FFC..0x8003569C`.
* Retail disassembly: `retail.disasm.txt`, SHA-256
  `739ddba8a2cecc8ed0a876f240b238644faef0a025a9bfff0b02091e1b92bc17`.
  The body returns at `0x80035694` (`jr ra` plus delay slot at `0x80035698`).
* Native source at verification: `src/slus_006.64/system/system.c`, SHA-256
  `6e9b2c746304d38cdb72de0cb22f112a6c2809e90cbbd42ef02e43297a66bdc8`.
  The target and its two page helpers are source lines 1110-1312; the public
  body starts at line 1253.
* Adapter used by the oracle: `pc_port/src/battle_mips_adapter.c` and `.h`,
  SHA-256 `4e0e896434e6d2f5452b8ac6f82799414ce8378bf9278fa7403cd083a4f2bf22`
  and `acd0133574e1cb546ce2b33ee582d7b8dff48c988298c6ac4fa6cb65e4b5f12a`.

## Harness and coverage

`glyph_differential_test.c` compiles and calls the actual native function from
`system.c`.  In the same process it runs the raw retail body through
`battle_mips_adapter`, with an independent 3 MiB guest bus and guest addresses
for the work buffer, glyph backing, globals, and fifth stack argument.
The host fixture uses static buffers and a `-no-pie` executable; it aborts if
the native glyph pointer cannot be represented by the retail `u32` global.

Each case compares every `u16` in all 13 rows, including every word in the
requested stride, between native and raw retail output.  It also checks both
host and guest redzones, preserves the page-inactive bits from the initial
work pattern, and verifies the complete glyph backing allocation is unchanged.
The negative `trail=0x40` path is backed by real preceding allocation and gets
the same glyph pattern as the normal `trail=0x41` path.

The matrix is 144 independent cases:

```
pages:          0, 1
strides:        4, 28, 42 words
glyph inputs:   blank, full, one-hot, mixed (11 u16 rows each)
work patterns:  0x0000, 0xFFFF, 0xA55A
trails:         0x41, 0x40 (negative glyph offset with D_80059364=0x41)
```

The native and oracle executions do not share output or glyph storage.  The
native source uses its normal global data path; the oracle receives equivalent
guest globals at `0x8005934C`, `0x80059350`, `0x8005935C`, and `0x80059364`.

## Verification

Runner: `run_glyph_differential.sh`.  It uses Clang for the production TU,
test, adapter, and link, with the repository's required header shims and
`--gc-sections`; the source TU's pre-existing incompatible-pointer and
pointer-to-32-bit conversion diagnostics are suppressed only for that legacy
TU, as in the existing production-linked tests.  Each invocation verifies the
full `disc/SLUS_006.64` hash and the 1696-byte slice before compiling, records
source/oracle/compiler hashes in `manifest.txt`, and writes to a fresh
`mktemp` directory under this scratch directory.  `GLYPH_RETAIL_TEST_OUT` may
be supplied when an operator needs a named output directory.  It executes:

```
O0       PASS
O2       PASS
UBSan    PASS (Clang, -fsanitize=undefined, fail-closed)
```

All normal stderr files are empty, and the common stdout is:

```
GLYPH RETAIL DIFFERENTIAL PASS cases=144 pages=2 strides=3 glyphs=4 fills=3 trails=2
```

Both required negative controls are rejected by the raw retail oracle:

```
WM_34FFC_MUTANT_REVERSE_ROW0_OUTLINE  DETECTED (case 1, page 0 blank glyph)
WM_34FFC_MUTANT_REVERSE_ROW1_OUTLINE  DETECTED (case 73, page 1 blank glyph)
```

The final runner certificate is:

```
GLYPH RETAIL DIFFERENTIAL CERTIFICATE PASS; row0/row1 mutants rejected
```

## Limits

Glyph words are synthetic inputs selected to cover bit patterns and retained
work data.  This proves the native function's full-buffer behavior against the
retail instruction body; it does not prove whole-renderer texture uploads,
font archive contents, or final pixels.
