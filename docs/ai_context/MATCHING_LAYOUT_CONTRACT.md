# Overlay byte-matching: layout constraints

Read this **before** planning work to byte-match an overlay. Each rule below
cost real debugging time to find, and most of them are invisible in the C: the
overlay is wrong by thousands of bytes while every individual function body is
already correct.

Byte parity is a quality gate, not the critical path — see
`PORT_GOAL_AND_PLAN.md`. The point of this document is to let you decide *not*
to spend a day on an overlay, with a measured reason.

---

## Rule 0 — GCC 2.7.2 hoists INCLUDE_ASM above all compiled C

**A single translation unit that mixes `INCLUDE_ASM` with hand-written C
function bodies can never match retail ordering.** This is a property of the
compiler, not of the split config or of the C, and no amount of decompilation
fixes it.

`INCLUDE_ASM` (`include/include_asm.h:23`) expands to a **file-scope**
`__asm__(...)`. GCC 2.7.2 buffers file-scope asm and emits all of it at the top
of its assembly output, ahead of every compiled function body. So a TU holding
N asm functions and M C functions emits

    [ all N INCLUDE_ASM bodies ] [ all M compiled C bodies ]

with zero interleaving, regardless of the order they appear in the source.
Retail interleaves them, so almost every function lands at the wrong address
and essentially the whole overlay differs.

### How to check, in one command

Read the **compiler's own output**, not the `.c`. Regexing the `.c` is
unreliable: `#ifndef XENO_PC_PORT` / `#else` pairs make the same function
appear as both an `INCLUDE_ASM` and a C body, so the naive counts overlap and
exceed the real function count.

```sh
python3 - <<'PY'
import re
seq, seen = [], set()
for line in open('build/src/<overlay>/<file>.c.s'):
    m = re.search(r'\.include\s+"[^"]*?/(func_[0-9A-F]{8})\.s"', line)
    k = 'A' if m else None
    if not m:
        m = re.match(r'^(func_[0-9A-F]{8}):', line)
        if not m: continue
        k = 'C'
    if m.group(1) in seen: continue
    seen.add(m.group(1)); seq.append(k)
print(''.join(seq))
PY
```

A clean run of `A`s followed by a clean run of `C`s is the failure signature.
Interleaved, or all-one-letter, is fine.

### The remedy

Give the overlay **homogeneous `c` subsegments**: split the `c` subsegment in
`config/<overlay>.yaml` so that each TU is either all-asm or all-C. Then the
hoist has nothing to reorder.

Do not do this reflexively. Re-splitting has splat regenerate over the existing
`src/` file, and in this repo those files carry live port work inside
`#ifndef XENO_PC_PORT` / `#else` branches. Trading executing port code for byte
parity on a UI overlay is the trade `PORT_GOAL_AND_PLAN.md` warns against.

### Evidence — three cases, measured 2026-09-21

| overlay | shape | result |
| --- | --- | --- |
| `menu` | **one** TU: 139 INCLUDE_ASM + 168 C bodies, all 0x24388 of text | 305 of 307 functions link away from their retail address; 133,762 bytes differ |
| `shop_menu` | 23 INCLUDE_ASM + 24 C bodies, split across **9** `c` subsegments | matches retail exactly (0 differing bytes) |
| `battling` | 168 INCLUDE_ASM + 3 unguarded C bodies in one file | matches retail exactly — because `linker/battling.ld` **never links** `main.c.o`; its `.text` is 100% asm and the C file exists for the port only |

`battling` is the case that makes the rule credible: it looks like a
counterexample until you check the linker script. If you are about to assert
this rule about some overlay, check which objects its `linker/<overlay>.ld`
actually lists — the `ld` ninja rule has no `$in`, the linker script decides.

---

## Rule 1 — the leading subsegment must be `rodata`, not `data`

`section_order` is `[".rodata", ".text", ".data", ".bss"]`, so a leading `data`
subsegment links *after* the code and the overlay mismatches from byte 1.
Fixing this took `world_map` from 154,742 differing bytes to 2,743.

## Rule 2 — a non-multiple-of-4 overlay length needs a `databin` tail

Without it the build comes out short: `world_map` by 2 bytes, `battling` by 1,
`battle_command_file1` by 4. Check `stat -c%s disc/<overlay>.bin % 4` first —
`menu` is 0x25908, a multiple of 4, so the rule does not apply there.

## Rule 3 — trailing data must be its own `data` subsegment

Left inside the `asm` subsegment, splat emits it with `dlabel`, which does not
export, and the link fails on undefined data symbols.

## Rule 4 — bss symbols must be pinned, not linker-allocated

Otherwise every `%lo()` referencing bss is a few bytes high. Pin both:

1. each symbol at the address encoded in its own name, in
   `config/symbol_addrs.<overlay>.txt` (`D_8009C894 = 0x8009C894;`) —
   generate the list, do not hand-type it;
2. the section VMA itself, in the `Makefile` patch block, because splat 0.33.2
   emits a nested bss subsegment address-less and `.bss` alignment rounds it up.

Nudging the bss *start* is not a substitute and makes it worse. Verify in
`build/out/<overlay>.map` that the section and a couple of named symbols landed
where their names say.

## Rule 5 — never put a `;` inside a `//` comment in a symbol_addrs file

splat 0.33.2 asserts `line.count(";") == 1` on the raw line, before stripping
the comment. It fails at the *first* splat invocation, so **every** overlay
goes dark, not just the one whose file is malformed.

---

## Known and accepted mismatches

### `menu.bin` — ACCEPTED, do not re-open as a bug

Measured 2026-09-21: built 153,472 vs retail 153,864, **133,762 differing
bytes**, sha256 `b01df05e…` against `config/checksum.sha`'s `82f84a24…`.
Full evidence: `docs/evidence/menu-bin-red-20260921.md`.

`config/menu.yaml` is **correct** and needs no change — rodata (0x31c), data
(0x1264) and bss (0x801ea908) all land exactly as declared, Rules 1–3 are
already satisfied, and the entire 392-byte shortfall is confined to `.text`.
Two causes, both deliberately not fixed:

1. **Rule 0.** `menu` is the monolithic-TU case above. The remedy is a `src/`
   restructure that would regenerate over live port work.
2. **Codegen.** 228 of 307 functions compile to exactly the retail size; 79 do
   not. All 79 are hand-written C; zero are `INCLUDE_ASM` (those are verbatim
   retail bytes and cannot change size). Deltas are small and **two-signed**,
   −0xa4 … +0x5c.

That delta distribution is the load-bearing detail — but read the next section
before drawing a verdict from it.

### A size deficit is a reason to triage, never a verdict

Two-signed deltas of tens of bytes (menu: −0xa4 … +0x5c) are register
allocation and inlining: a byte-parity gap with **no evidence of a correctness
gap**. That much is safe to conclude.

A *large one-directional* deficit is **not** safe to conclude anything from,
and the intuition that it must be is wrong. It was asserted during the
2026-09-21 wave — "you cannot lose 1,192 bytes of function to regalloc,
therefore it is missing behaviour" — and then **disproved by measurement**. The
`field` cohort of 18 functions short by more than 200 bytes, worst −1,192, came
back **18 of 18 innocent**. The dominant cause was on nobody's suspect list:

> **GCC 2.7.2 retail re-materialises global base addresses at every single
> use** — `lui`+`addiu` per access. `g_FieldTextBoxes` appears 132 times in
> `func_8007E1C0` alone (264 instructions). Cohort-wide the `lui` count falls
> 543 → 313.

On top of that, retail inlines small callees — the C made *more* calls than
retail in 14 of 18 — and duplicates loop tails and switch arms (one −16 was
retail duplicating an OT-link tail into all 8 arms, 8×2).

**Rule out, in this order, before calling anything missing.** Count the `lui`
delta first; it is cheap and it was decisive in `field`. Then retail inlining,
then tail/switch-arm duplication, then dead code retail carried.

**The two tests that actually decide it:**

1. Does retail **call** anything the C never calls?
2. Does retail **write** any global the C body never names?

In `field`, 0 of 18 failed either. These are not a rubber stamp: the same two
tests run over the `slus` cohort failed 1 of 17, and that one was a genuine
user-visible defect — `func_800347C0` computing a text row from a hardcoded
stride of 4 instead of the row stride at +0x14 (which `func_80032F54` sets to
0x0E), putting the Lahan dialogue continue-arrow ~3.5× too high and stopping it
following scrolled text. Fixed; the corrected body is 64 bytes, exactly retail's.

**Correct your deficit figures before reasoning from them.** Two artifacts
inflated the `slus` cohort until they were found: static helpers charged to no
caller (`func_80034FFC` calls two invented helpers retail had inline — folding
them back in gives 97% of retail with a 52-byte residual, not the "1,204
missing / 29%" first reported), and inter-function `nop` padding charged to the
preceding symbol because the parser ignored `.size`, which dropped five cohort
members to a true deficit of **exactly zero**. The cohort went 17 functions /
2,424 B → 12 / 1,156 B on those corrections alone.

`PORT_GOAL_AND_PLAN.md:73` calls `menu` one of the cheapest, most visible
HD-2D wins precisely because correctness beats byte parity there. Spend effort
on menu's behaviour, not its bytes.
