# Guest-RAM generator: what was inexpressible, what is fixed, what is not

Target: the blocker from `docs/evidence/battle-leaf-adoption-20260921/` — 6
`src/battle` TUs that would not compile under
`-DXENO_BATTLE_OVERLAY_HOST_BODIES`, gating 9 of 26 leaf candidates.

Fixed at `tools/scripts/gen_battle_overlay_guest_ram.py`, not by hand-patching
TUs. Result: **failing TUs 14 → 9, five fixed, zero newly broken**, and
**adopted leaves 100 → 103**.

## Diagnosis: five distinct defects, not one

The generator's `parse_extern()` assumed one declarator per `extern`:

```python
names = re.findall(r"\b(D_\w+)\b", raw)
name = names[-1]              # only the LAST declarator is ever considered
before = raw.split(name)[0]   # everything before it becomes the "type"
type_tok = before.replace("const","").replace("*"," ").strip().split()[-1]
```

**A. Only the last declarator in a multi-declarator extern got a macro.**
`extern u8 D_800D3014, D_800D366C, D_800C3E29;` produced a macro for
`D_800C3E29` only, so `main35.c:64`'s `switch (D_800D3014)` failed with
`'D_800D3014' undeclared`.

**B. The type was captured by splitting the text before the name**, so it
absorbed the preceding declarators. Emitted output, verbatim:

```c
#define D_800C3E29 (*((D_800D366C, *)PSX_ADDR(0x800C3E29)))
#define D_800C3EB4 ((D_800C3E90[], *)PSX_ADDR(0x800C3EB4))
#define D_800C3EC0 ((D_800C3EBE[], *)PSX_ADDR(0x800C3EC0))
#define D_800C3730 ((D_800C3740[], *)PSX_ADDR(0x800C3730))
```

`D_800D366C, *` is not a type. Four macros were syntactically invalid.

**C. Per-declarator array dimensions were lost.** `kind` was computed from the
whole statement, so `extern u8 D_800C3E90[], D_800C3EB4[][28];` made both
plain arrays and dropped the `[28]`. `D_800C3EB4` needs `u8 (*)[28]`.

**D. `struct`/`union`/`enum` tag keywords were dropped**, because `type_tok`
took only the last token before the name: `extern struct BattleCommandContext
*D_800C3EAC;` became `(BattleCommandContext *)`, which is not a type —
producing `request for member 'rows' in something not a structure or union`.

**E. An `extern` glued to the preceding `}` was never wrapped.** This one is
the interesting find, and it is a *source corruption* the wrapper could not
see:

```c
}extern u8* D_800C34B0;      /* src/battle/main55.c */
}extern u32 D_800D3368[];    /* src/battle/main70.c */
```

`wrap_externs()` only matched an extern that starts its own line, so these two
never got their `#ifndef XENO_PC_PORT` guard. In host-bodies mode the
declaration therefore survived *alongside* the generated macro, and the macro
expanded **inside the declaration** — which is why the compiler reported
`expected ')' before '*' token` at the macro's own definition line
(`battle_overlay_guest_ram.h:172`) rather than anywhere near the real problem.
That misdirection is why my previous report guessed at this one.

## Fixes

- `split_declarators()` + `parse_externs()` replace `parse_extern()`: a real
  base-type / declarator-list split returning one entry per declarator, with
  its own stars and dimensions. Handles tag keywords and multi-word base types
  (`unsigned long`).
- Element type for `D[][N]` is now `base (*)[N]`, and the emitter uses the
  type as-is when it already contains `(*)` rather than appending another `*`.
- `wrap_externs()` normalises `([;}])[ \t]*(extern\s)` onto its own line
  before wrapping, and now wraps when **any** declarator in the statement is
  aliased, not just the first.
- Struct tags are forward-declared at the top of the header so a pointer cast
  is parseable in every TU.

Generated-header diff is 6 corrected/added macros and one forward declaration.
No existing macro's element type changed.

## One fix attempted and deliberately REVERTED

I tried preferring a named type over the decomp's `u8` placeholder when two
TUs declare the same address with different types, so `D_800C3EAC` would cast
to `struct BattleCommandContext *` and `main35.c`'s `->rows` would compile.

**It broke 12 previously-working TUs** — `main.c`, `main34`, `main36`,
`main37`, `main45`–`48`, `main8`, `main9` and others — with `invalid use of
undefined type 'struct BattleCommandContext'`. The struct is defined in
exactly one TU; every other TU legitimately uses the byte-pointer view, and a
forward declaration is not enough because they dereference it. `main36.c`
owns adopted leaves, so this would have broken shipped coverage.

Reverted, with the reason recorded in the source. **This is the real
structural limit: a single shared header cannot carry per-TU types.** Several
addresses are genuinely declared differently in different TUs —
`u8 D_800D32A1[]` vs `u8 D_800D32A1[][8]`, `u8 D_800CCD34[]` vs
`u16 D_800CCD34[][184]` — because each TU takes a different view of the same
bytes and indexes accordingly. I also removed my first tie-break attempt
("prefer the longer type string") for the same reason: it silently changed
`D_800D32A1` to `u8 (*)[8]`, which would have altered `main32.c`'s pointer
arithmetic from a stride of 8 to a stride of 64.

## Measured effect on TU compilability

Controlled before/after sweep over every `src/battle/*.c` with
`-DXENO_BATTLE_OVERLAY_HOST_BODIES`, generator + header + sources reverted to
HEAD for the "before" run:

| | count |
|---|---|
| TUs failing before | **14** |
| TUs failing after | **9** |
| fixed | `main35_p3_q1.c`, `main35_p4.c`, `main39.c`, `main55.c`, `main70.c` |
| **newly broken** | **none** |

### The 9 that still fail, and why — four classes, none a parsing bug

| TU | first error | class |
|---|---|---|
| `main32.c`, `main75.c` | `lvalue required as left operand of assignment` | **ptr alias is an rvalue.** `D_800D367C = buf;` needs a host→guest pointer writeback; the alias is `(u8*)PSX_ADDR(*(u32*)PSX_ADDR(a))`. No such idiom exists in the tree, a host pointer is 8 bytes and the guest word is 4, and the source would have to change to use a setter — so it cannot be fixed at the generator alone. |
| `main35.c`, `main35_p1.c`, `main35_p3_q2.c` | `request for member 'rows' …` | **per-TU struct type** — see the reverted attempt above. |
| `main35_p2.c` | `subscripted value is neither array nor pointer` | same root: `D_800C3EAC` byte-view vs struct-view. |
| `mainc115.c` | `expected ')' before '*'` | **anonymous struct declarator**: `extern struct { SVECTOR eye,target; } D_800D30A0;`. `EXTERN_RE`'s `[^;]+` stops at the `;` *inside* the braces, so the statement is never parsed or wrapped. Also conflicts with `mainc114.c`, which declares the same address as `BattleSetupShortVector[]`. |
| `mainc130.c`, `mainc84.c` | `conflicting types for 'func_800B57E4'` | **unrelated to guest RAM** — two incompatible prototypes for one function. Pre-existing. |

## Adoption: 100 → 103

Re-ran the eligibility pipeline end to end. Eligible pool 24 (22 carried over
after last batch's 4 adoptions, plus 2 newly eligible: `func_80084854`,
`func_80085310`). Same criteria and same bar as last time.

**Adopted (3), all proven equivalent to the retail bytes, all from TUs this
generator fix unlocked:**

| function | TU | interpreted callers |
|---|---|---|
| `func_8009AA44` | `main55.c` | 3 |
| `func_8009AB00` | `main55.c` | 2 |
| `func_800AA788` | `main70.c` | 1 |

Rejected: 5 still TU-blocked (`main32`/`main35`/`mainc115` above); 6 with no
`jal` caller so execution cannot be demonstrated; 1 (`func_800BC2F0`)
deliberately excluded in source, caught again by the `[2c/5]` inert-entry
gate; 6 rejected by the prover for real divergence; 2 `UNPROVEN`.

Two of the prover rejections are **new information** that only became
reachable because the TUs now compile: `func_800885D0` (`main39.c`) and
`func_800AA760` (`main70.c`). Both have C bodies that do not match retail.

## Gates

```
# 103 adopted leaves reach no generated stub (77 stubs checked)
    LINK OK -> pc_port/build_native/xeno-port (port-owned addresses verified)
    overlay host allowlist verified against the linked binary

BATTLE OVERLAY DIFFERENTIAL PASS checks=2521 inconclusive=442 hand-only=8 leaves=103
BATTLE OVERLAY DIFFERENTIAL bridge-placeholders=25
BATTLE OVERLAY DIFFERENTIAL FAIL func_80079934 ...   <- the wrong-result control, correctly rejected
BATTLE OVERLAY DIFFERENTIAL PASS (O0/O2/UBSan + wrong-result control rejected)

BATTLE OVERLAY LEAF PASS checks=40   (x3: O0/O2/UBSan)
BATTLE OVERLAY LEAF PASS (O0/O2/UBSan + always-interpret control)

BATTLE MIPS ADAPTER PASS
RETAIL_LEAF_ADAPTERS PASS checks=35
BATTLE RETURN STATE PASS
```

Differential checks 2431 → **2521**.

### Byte parity — proved, not assumed

This change edits `src/battle/main55.c` and `src/battle/main70.c` (splitting
the glued externs and guarding them), so parity had to be re-proved:

```
battle.bin  built=343936 retail=343936
battle.bin IDENTICAL to retail
124703d415f570f9cd8d7dad89c6a4eddfd7e6f0  build/out/battle.bin
124703d415f570f9cd8d7dad89c6a4eddfd7e6f0  disc/battle.bin
battle_command_file1 IDENTICAL
field.bin 249310 diffbytes=218716   (unchanged)
```

Splat tracebacks in the matching build: **0**. Wrapping an extern in
`#ifndef XENO_PC_PORT` is inert for the matching build, which does not define
that macro; the source edits are whitespace plus guards.

## Suites that could NOT run on this host — not passes

- `run_battle_guest_call_test.sh` — **NOT RUN**. Wants
  `asm/battle_command_file1/matchings/message_controller/xeno_battle_command_file1_controller.s`;
  `config/battle_command_file1.yaml` uses whole-TU `asm` subsegments and emits
  no per-function `matchings/` tree. Config is owned by a sibling.
- `run_textbox_timing.sh` — **NOT RUN**. Host lacks
  `/usr/lib64/libubsan.so.1.0.0`, so the UBSan link fails.

## Carried forward durably (not fixed here, by instruction)

Both are now in `OPEN_ISSUES.md`, and the do-not-adopt list is *also* in
`pc_port/src/battle_overlay_host_leaves.inc` — the file an adopter actually
edits — so it is hit before the mistake, not after. The names there are
deliberately unquoted: `battle_overlay_host_tus.py` scrapes `"func_..."`
string literals from that file, so a quoted name in a comment would be read as
an adopted leaf. Verified the count stays 103 after adding the comment.

1. **Six C bodies that diverge from retail** — the original four
   (`func_800BEDE8`, `func_800AA7DC`, `func_800AA79C`, `func_8008887C`) plus
   two newly reachable (`func_800885D0`, `func_800AA760`). Latent only because
   the interpreter still runs those paths.
2. **`run_battle_native_callback_map_test.sh`** — "generated stub was
   dispatched". Not fixed: it is a callback-map/stub-dispatch problem with no
   connection to the guest-RAM generator, so it was neither cheap nor in scope
   once I was in the generator.
3. **`run_battle_graphics_abi_test.sh`** — newly noticed, also pre-existing
   (verified at the 100-leaf baseline). Its O0 binary exits nonzero after two
   PASS lines, so `set -e` skips the O2 and UBSan regimes *and* the
   translation-mutant control entirely.

That is now three stale or silently-failing fixtures in this one area. A
dedicated pass over `pc_port/tests/` for gates that are green only because
they never reach the failing check looks overdue.

## Next batch

`main32.c`/`main75.c` need a host→guest pointer writeback idiom, which is a
source-shape change, not a generator change. `main35.c`/`mainc115.c` need
per-TU type views, which means moving from one shared header to per-TU alias
headers — a real redesign, and the only remaining change that would unlock
multiple candidates at once.
