# menu.bin does not match retail — layout-vs-codegen verdict

Measured 2026-09-21 in `wt-worldmap-bss`, from a clean `make build` (splat
semicolon fix applied, commit 8fbd1e99) followed by `ninja build/out/menu.bin`.

## The four numbers

| measurement | value |
| --- | --- |
| `stat -c%s build/out/menu.bin` | 153472 |
| `stat -c%s disc/menu.bin` | 153864 (392 bytes larger) |
| `cmp -l disc/menu.bin build/out/menu.bin \| wc -l` | **133762** (87% of the overlap) |
| `sha256sum build/out/menu.bin` | `b01df05ef95c73ad08c7784a70c2f4bc98f34ffaf5f008cc02292d2b56c9e813` |
| `config/checksum.sha` expects | `82f84a24ac1fd1979754e1cc9c861405fb59ce95dd78db00a76f00c8f1bd177d` = sha256(disc/menu.bin) |

So `menu.bin` is genuinely red. The project's "known-red" note is correct, and
`config/checksum.sha` does record the true retail hash — menu **is** checked.

## Verdict: NOT the layout-rule class that battling / world_map / bcf1 were

`config/menu.yaml` is already correct and needs no change:

- section geometry all lands where the yaml says it should — `.rodata` 0x31c
  (= the `[0x0, .rodata]` .. `[0x31c, c]` span), `.data` 0x1264
  (= 0x246a4..0x25908), `.bss` at 0x801ea908;
- the leading block is already `.rodata`, not `data` (split rule #1);
- menu's length 0x25908 is a multiple of 4, so no `databin` tail is needed
  (split rule #2 does not apply);
- **only `.text` is wrong**: 0x24200 built vs 0x24388 retail, i.e. the entire
  392-byte shortfall is in code.

There are two independent defects, and neither is a splat split rule.

### Defect 1 — GCC hoists INCLUDE_ASM, so a mixed TU cannot match (dominant)

`INCLUDE_ASM` (include/include_asm.h:23) expands to a **file-scope**
`__asm__(...)`. GCC 2.7.2 buffers file-scope asm and emits all of it at the top
of its assembly output, ahead of every compiled function body. Ground truth is
the compiler's own output, `build/src/menu/main/misc.c.s`: parsing it in
emission order gives 307 functions in the pattern

    AAAA...(139 INCLUDE_ASM)...AAAA CCCC...(168 compiled C)...CCCC

with zero interleaving, while retail interleaves them. Consequence: **305 of
307 functions link away from their retail address** (`build/out/menu.map`), and
that is what produces the 133,762-byte diff — including the very first one, at
offset 0x48, where a function-pointer table reads 0x801c7350 in retail and
0x801c6a34 in the build.

`src/menu/main/misc.c` is a single TU carrying all 0x24388 of menu's text:
139 INCLUDE_ASM + 168 real C bodies. That shape cannot match.

Why the other overlays are unaffected, checked rather than assumed:

- **battling** (0 differing bytes) has 3 unguarded C bodies mid-order, which
  looks like a counterexample — but `linker/battling.ld` never links
  `build/src/battling/main.c.o` at all. Its `.text` comes entirely from
  `CB8.s.o` + `193A0.s.o`. The C file exists for the port only, so it never
  enters the matching build.
- **shop_menu** (0 differing bytes) mixes 23 INCLUDE_ASM with 24 C bodies but
  is split across **9** `c` subsegments, so each TU is effectively homogeneous
  and the hoist has nothing to reorder. This is the shape menu would need.
- **world_map / movie** are 100% INCLUDE_ASM — no C to hoist past.

Secondary, same class: `misc.c`'s own C bodies are not in retail order either —
`func_801C90B0` is defined before `func_801C8D78`.

### Defect 2 — genuine codegen in hand-written C bodies

Independent of ordering, function *sizes* are wrong. Comparing each function's
ELF symbol size against its retail size (retail sizes derived from the spacing
of consecutive retail addresses, which the `func_XXXXXXXX` names encode):

- **228 of 307 functions compile to exactly the retail size.**
- **79 do not.** Every one of the 79 is a hand-written C body; **zero** are
  INCLUDE_ASM — as expected, since INCLUDE_ASM bodies are verbatim retail bytes
  and cannot change size. Deltas are small and two-signed (−0xa4 … +0x5c), the
  signature of register allocation / branch / delay-slot differences rather than
  a shift.

This is the part the dispatch says not to start on. Full list below.

## Why this is not mine to fix

Fixing defect 1 means giving menu the shop_menu shape: splitting the single
`c` subsegment into homogeneous runs. But the hand-written C in `misc.c` is
real port work (much of it inside `#ifndef XENO_PC_PORT` / `#else` pairs), and
re-splitting would have splat regenerate over it. That is a `src/` restructure
with port risk, not a `config/menu.yaml` edit. And even done perfectly it would
leave defect 2's 79 functions needing genuine byte-matching.

Per PORT_GOAL_AND_PLAN.md, byte-matching menu's UI logic is explicitly not
worth holding work for — menu is called out there as a cheap, visible HD-2D win
where correctness beats byte parity. Escalated rather than started.

## The 79 functions

79 functions compile to a size different from retail: 79 hand-written C, 0 INCLUDE_ASM.

| function | retail addr | retail size | built size | delta |
| --- | --- | --- | --- | --- |
| `func_801C57A4` | 0x801c57a4 | 0x148 | 0x130 | -0x18 |
| `func_801C6400` | 0x801c6400 | 0x1f4 | 0x180 | -0x74 |
| `func_801C6D90` | 0x801c6d90 | 0x7c | 0x78 | -0x4 |
| `func_801C7F34` | 0x801c7f34 | 0x184 | 0xf8 | -0x8c |
| `func_801C8694` | 0x801c8694 | 0x130 | 0x138 | +0x8 |
| `func_801C881C` | 0x801c881c | 0x100 | 0xc0 | -0x40 |
| `func_801C891C` | 0x801c891c | 0x44 | 0x4c | +0x8 |
| `func_801C8960` | 0x801c8960 | 0xb0 | 0x70 | -0x40 |
| `func_801C8A10` | 0x801c8a10 | 0x1dc | 0x1c4 | -0x18 |
| `func_801C8BEC` | 0x801c8bec | 0xb8 | 0xc0 | +0x8 |
| `func_801C8D78` | 0x801c8d78 | 0x170 | 0x168 | -0x8 |
| `func_801C8EE8` | 0x801c8ee8 | 0x150 | 0x158 | +0x8 |
| `func_801C90B0` | 0x801c90b0 | 0x1c0 | 0x1a4 | -0x1c |
| `func_801C9270` | 0x801c9270 | 0x138 | 0x148 | +0x10 |
| `func_801C9BCC` | 0x801c9bcc | 0x168 | 0x154 | -0x14 |
| `func_801C9D34` | 0x801c9d34 | 0x1c0 | 0x170 | -0x50 |
| `func_801CA480` | 0x801ca480 | 0x170 | 0x12c | -0x44 |
| `func_801CA5F0` | 0x801ca5f0 | 0x160 | 0x15c | -0x4 |
| `func_801CACF8` | 0x801cacf8 | 0xb8 | 0xc0 | +0x8 |
| `func_801CB184` | 0x801cb184 | 0x108 | 0xd8 | -0x30 |
| `func_801CB28C` | 0x801cb28c | 0x78 | 0x70 | -0x8 |
| `func_801CB8AC` | 0x801cb8ac | 0x13c | 0x150 | +0x14 |
| `func_801CD710` | 0x801cd710 | 0x10c | 0xf8 | -0x14 |
| `func_801CEB5C` | 0x801ceb5c | 0x58 | 0x5c | +0x4 |
| `func_801CEBB4` | 0x801cebb4 | 0x8c | 0x84 | -0x8 |
| `func_801D01D0` | 0x801d01d0 | 0x108 | 0x10c | +0x4 |
| `func_801D0D90` | 0x801d0d90 | 0x90 | 0x94 | +0x4 |
| `func_801D0E20` | 0x801d0e20 | 0x18 | 0x1c | +0x4 |
| `func_801D0EBC` | 0x801d0ebc | 0x18 | 0x1c | +0x4 |
| `func_801D0F54` | 0x801d0f54 | 0x80 | 0x7c | -0x4 |
| `func_801D10DC` | 0x801d10dc | 0x84 | 0x80 | -0x4 |
| `func_801D14B0` | 0x801d14b0 | 0x4c | 0x54 | +0x8 |
| `func_801D17C4` | 0x801d17c4 | 0x150 | 0x130 | -0x20 |
| `func_801D22C4` | 0x801d22c4 | 0x30 | 0x40 | +0x10 |
| `func_801D25E4` | 0x801d25e4 | 0x38 | 0x30 | -0x8 |
| `func_801D2968` | 0x801d2968 | 0x40 | 0x48 | +0x8 |
| `func_801D32B4` | 0x801d32b4 | 0x90 | 0x94 | +0x4 |
| `func_801D3B00` | 0x801d3b00 | 0x14c | 0x148 | -0x4 |
| `func_801D3C4C` | 0x801d3c4c | 0x164 | 0x160 | -0x4 |
| `func_801D6194` | 0x801d6194 | 0x1a4 | 0x1a8 | +0x4 |
| `func_801D7CFC` | 0x801d7cfc | 0x254 | 0x1e0 | -0x74 |
| `func_801D83AC` | 0x801d83ac | 0x108 | 0x10c | +0x4 |
| `func_801D84B4` | 0x801d84b4 | 0x128 | 0xfc | -0x2c |
| `func_801D85DC` | 0x801d85dc | 0x68 | 0x64 | -0x4 |
| `func_801D8DE4` | 0x801d8de4 | 0xc0 | 0xc8 | +0x8 |
| `func_801D9B08` | 0x801d9b08 | 0x17c | 0x130 | -0x4c |
| `func_801D9C84` | 0x801d9c84 | 0x1b8 | 0x1ac | -0xc |
| `func_801D9F34` | 0x801d9f34 | 0x64 | 0x54 | -0x10 |
| `func_801DD5E8` | 0x801dd5e8 | 0x1a8 | 0x104 | -0xa4 |
| `func_801DE2C8` | 0x801de2c8 | 0xa4 | 0xb0 | +0xc |
| `func_801DE36C` | 0x801de36c | 0x94 | 0x8c | -0x8 |
| `func_801DE400` | 0x801de400 | 0x74 | 0x64 | -0x10 |
| `func_801DE474` | 0x801de474 | 0x158 | 0x154 | -0x4 |
| `func_801DFE2C` | 0x801dfe2c | 0x130 | 0xf0 | -0x40 |
| `func_801E0434` | 0x801e0434 | 0x19c | 0x190 | -0xc |
| `func_801E0F78` | 0x801e0f78 | 0x9c | 0xac | +0x10 |
| `func_801E1418` | 0x801e1418 | 0x12c | 0x118 | -0x14 |
| `func_801E20C8` | 0x801e20c8 | 0x188 | 0x1e4 | +0x5c |
| `func_801E2250` | 0x801e2250 | 0xd4 | 0xf0 | +0x1c |
| `func_801E2368` | 0x801e2368 | 0x64 | 0x50 | -0x14 |
| `func_801E2AE0` | 0x801e2ae0 | 0xa0 | 0xb4 | +0x14 |
| `func_801E2B80` | 0x801e2b80 | 0x64 | 0x50 | -0x14 |
| `func_801E35BC` | 0x801e35bc | 0x118 | 0xec | -0x2c |
| `func_801E41C0` | 0x801e41c0 | 0x98 | 0x88 | -0x10 |
| `func_801E4258` | 0x801e4258 | 0x54 | 0x4c | -0x8 |
| `func_801E42AC` | 0x801e42ac | 0x90 | 0x80 | -0x10 |
| `func_801E4928` | 0x801e4928 | 0x70 | 0x68 | -0x8 |
| `func_801E4998` | 0x801e4998 | 0x90 | 0xd8 | +0x48 |
| `func_801E5178` | 0x801e5178 | 0x254 | 0x250 | -0x4 |
| `func_801E5924` | 0x801e5924 | 0x1a8 | 0x1b0 | +0x8 |
| `func_801E5ACC` | 0x801e5acc | 0x70 | 0x80 | +0x10 |
| `func_801E5B3C` | 0x801e5b3c | 0x4c | 0x50 | +0x4 |
| `func_801E6450` | 0x801e6450 | 0x4c | 0x58 | +0xc |
| `func_801E649C` | 0x801e649c | 0x44 | 0x38 | -0xc |
| `func_801E64E0` | 0x801e64e0 | 0x64 | 0x54 | -0x10 |
| `func_801E6AE8` | 0x801e6ae8 | 0x88 | 0xbc | +0x34 |
| `func_801E6B70` | 0x801e6b70 | 0x18c | 0x174 | -0x18 |
| `func_801E71B4` | 0x801e71b4 | 0x188 | 0x14c | -0x3c |
| `func_801E781C` | 0x801e781c | 0xac | 0x98 | -0x14 |
