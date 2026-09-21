# field rodata surplus — linker-map attribution (2026-09-10)

## Question
`field_RODATA_SIZE` = 0x37C vs retail 0x2FC, i.e. **0x80 bytes of surplus
`.rodata`**. Which input objects contribute them?

## Method (reproducible)
Artifact: `build/out/field.map` (produced by the real link; `make rom-check`
regenerates it, and the ninja link uses `-Map build/out/$ldtarget.map`).

```
sed -n '262,345p' build/out/field.map
```

## Result — every `.rodata` contribution to `.field`, in link order
`field_RODATA_START = 0x8006faf0` (`field_VRAM`; `.field` vaddr 0x8006faf0,
size 0x3d4d4, load 0x0).

| # | input object | output addr | size | offset from start |
|---|---|---|---|---|
| 1 | `asm/field/data/0.rodata.s.o`            | 0x8006faf0 | 0x9C | 0x000 |
| 2 | `src/field/main/misc4.c.o`               | 0x8006fb8c | 0x7C | 0x09C |
| 3 | `asm/field/data/11C.rodata.s.o`          | 0x8006fc08 | 0x7C | 0x118 |
| 4 | `src/field/main/misc.c.o`                | 0x8006fc84 | 0x20 | 0x194 |
| 5 | `src/field/main/misc11.c.o`              | 0x8006fca4 | 0x74 | 0x1B4 |
| 6 | `asm/field/data/22C.rodata.s.o`          | 0x8006fd18 | 0x3C | 0x228 |
| 7 | `src/field/scripts/virtual_machine.c.o`  | 0x8006fd54 | 0x2C | 0x264 |
| 8 | `asm/field/data/294.rodata.s.o`          | 0x8006fd80 | 0x68 | 0x290 |
| 9 | `src/field/main/misc8.c.o`               | 0x8006fde8 | **0x68** | **0x2F8** |
| 10| `src/field/main/misc5.c.o`               | 0x8006fe50 | **0x1C** | **0x360** |

Objects listed with no address (empty `.rodata`, contributing 0 bytes):
`misc3, fade, init, misc2, main, fade_render, text_box_render, camera_movement,
gold, input_script_handlers, misc10, scenario_flags, stats, misc7, text_box,
variable_handlers, misc6, distortion, particles, misc9`.

Terminators:
```
. = ALIGN (., 0x4)
field_RODATA_END = .            -> 0x8006fe6c
field_RODATA_SIZE = 0x37C       -> 0x8006fe6c - 0x8006faf0
field_TEXT_START = 0x8006fe6c   (misc3.c.o(.text) begins here)
```

## Attribution of the surplus
Retail's `.rodata` ends at **0x2FC**. Therefore:
- Items 1-8 total `0x9C+0x7C+0x7C+0x20+0x74+0x3C+0x2C+0x68 = 0x2F8`.
- Item 9 (**`misc8.c.o`**, 0x68 = 104 bytes) spans offsets `0x2F8..0x360`.
  Its **first 4 bytes** (0x2F8..0x2FC) fall inside retail's rodata; the remaining
  **100 bytes** (0x2FC..0x360) are surplus.
- Item 10 (**`misc5.c.o`**, 0x1C = 28 bytes) spans `0x360..0x37C` and is
  **entirely surplus**.
- Surplus total = 100 + 28 = **128 = 0x80** — exactly the discrepancy. Consistent.

So the disputed interval decomposes **by owner**, not by shape:
- `[0x2FC,0x35C)` 96 zero bytes  -> tail of **`misc8.c.o(.rodata)`**
- `[0x35C,0x360)` the 4-byte zero word -> also **`misc8.c.o(.rodata)`**
- `[0x360,0x37C)` the referenced 28-byte table -> **`misc5.c.o(.rodata)`**

Note the dispatch loader found earlier (`func_800A5C40`, `addu at,at,v0;
lw v0,-432(at)`) is consistent with item 10: `0x8006fe50` is misc5's table and
`func_800A5C40` is a misc5-range function. The previous "distortion" guess was
wrong; the map settles it.

## Secondary finding (also from the map)
`misc4.c.o(.rodata)` is **0x7C**, but the retail range declared for it
(`0x9C..0x11C`) is **0x80**. So items 1-8 are 4 bytes *short* of retail's 0x2FC
before items 9-10 are added (0x2F8 vs 0x2FC). Any future edit must account for this
4-byte shortfall rather than assume the declared boundaries are exact.

## Object-level refinement of the two owners
```
mips-linux-gnu-readelf -S -s build/src/field/main/misc8.c.o
mips-linux-gnu-readelf -S -s build/src/field/main/misc5.c.o
```
- **`misc5.c.o`**: `.rodata` size `0x1c`, **alignment 8**, and it has a
  **`.rel.rodata` of size `0x38` = 56 bytes = 7 relocations**. So misc5's 28 bytes
  are a **7-entry relocated address table - a real jump table** from a `switch` with
  7 cases. This is the table at output offset 0x360 referenced by `func_800A5C40`.
- **`misc8.c.o`**: `.rodata` size `0x68` (104 bytes), alignment 4, **no `.rel.rodata`
  and no symbols other than the section symbol**. So misc8's 104 bytes are
  **non-relocated anonymous data** - not a table of addresses; consistent with the
  observed all-zero content. Prime suspect: an uninitialised or zero-initialised
  aggregate emitted into `.rodata`.

## RESOLVED - misc5 half fixed; misc8 half traced to a compiler template
**misc5 (28 bytes, 0x1C):** retail's `asm/field/data/294.rodata.s` contains
`jtbl_8006FDAC` at 0x2BC with **7 entries spanning 0x2BC..0x2D8 = 0x1C**, including a
duplicated target at indices 2 and 4. Our `misc5.c.o(.rodata)` is 0x1C / 7 entries
with the same duplicate shape, so it is a duplicate of a table retail already
freezes in asm. Fixed by declaring `[0x2BC, .rodata, main/misc5]` in
`config/field.yaml`; `field_RODATA_SIZE` went **0x37C -> 0x360 (-0x1C, exactly
predicted)**; pins PASS; native port LINK OK.

**misc8 (104 bytes, 0x68):** traced to the *generated assembly*, not to a source
aggregate:
```
build/src/field/main/misc8.c.s (lines 27-33)
        .rdata
        .align  2
$LC0:
        .half   0
        .space  102
        .text
        .globl  func_80080A74
```
`.half 0` + `.space 102` = 104 bytes, matching `misc8.c.o(.rodata)` exactly, emitted
immediately before `func_80080A74`. `misc8.c:131` is
`s16 stateBuf[0x34] = { 0 };` and `0x34 * 2 = 104`. So the bytes are a
**compiler-generated constant template for a local aggregate initialiser** - which is
why no `static`/`const` grep finds it and why the object has no relocations/symbols.
Retail's `.rodata` has only 4 unaccounted bytes at 0x2F8..0x2FC, so retail's build
did not emit this template: the remaining fix is a **source** change, not a config
change.

## Not yet established
The correct *source form* for `stateBuf`'s preparation in `func_80080A74` (see the
handoff's ordered options: drop the initialiser if retail leaves it unwritten, or
reproduce retail's actual mechanism). Do not delete the zeros or relocate anything
to reach 0x2FC, and do not treat reaching 0x2FC as proof of byte-exactness.
