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

That delta distribution is the load-bearing detail. Two-signed deltas of tens
of bytes are register allocation and inlining — a byte-parity gap with **no
evidence of a correctness gap**. Contrast the `field` cohort (numbers from the
parallel field investigation, not measured here): functions short by more than
200 bytes, one by 1,192. You cannot lose that much function to regalloc, so
there the diagnosis is missing behaviour and it *is* worth chasing. Same
symptom, opposite verdict, decided by a number rather than a hunch — which is
the point of measuring the size distribution before committing to an overlay.

`PORT_GOAL_AND_PLAN.md:73` calls `menu` one of the cheapest, most visible
HD-2D wins precisely because correctness beats byte parity there. Spend effort
on menu's behaviour, not its bytes.

---

# Port bodies: 32-bit PSX assumptions are silently wrong on this 64-bit host

**Everything above this line is about matching-side byte parity. This
section is a different axis — it is about PORT C bodies being wrong at
runtime — and it matters more, because byte parity fails loud (a diff) and
this fails silent.**

Read this **before writing any port C body that reaches retail-mapped data
by raw offset, or by treating a retail 32-bit value as a host address.**
Two independent workers found this class the same day, from opposite
ends of the codebase — one in `menu` (reverting `func_801D261C` before it
shipped), one in `battle` (fixing five function bodies that had *already*
shipped wrong). Two known sub-classes so far, both below: **Class A**
(struct layout drift — a struct's compiled field offsets differ between
the 32-bit matching build and this 64-bit port build) and **Class B**
(address-width confusion — a 32-bit guest/PSX address used directly as a
64-bit host pointer, or truncated the other way). Expect more sub-classes;
the underlying cause is always the same: **retail was written for a
32-bit machine, and nothing about that assumption is checked when the
same C compiles for a 64-bit one.**

## Class A — struct layout drift

A struct is declared once, with field offsets that are supposed to match
the retail PSX memory layout. On the PSX (32-bit MIPS target), a pointer or
a `long`/`unsigned long` is 4 bytes; on this port's host build (`gcc -m64`,
gp_flag notwithstanding — the *host* toolchain, not the MIPS one), the same
field is **8 bytes**. The struct's *matching-build* (MIPS, 32-bit) layout
therefore still equals retail's, but its *port-build* (host, 64-bit)
compiled layout silently drifts the instant such a field appears anywhere
before the offset you care about — every field after it shifts, by more
than the 4-byte difference if there is more than one such field, or if the
struct contains an array of a type that itself drifted (a `GfxEnvironment
gfxEnvs[2]` array drifts by 2× its own per-element growth).

**The symptom is not a crash and not a build error.** A raw-offset body
compiles clean under both toolchains, links clean, and passes casual
inspection — the offset is *right there in the retail disassembly*, so it
looks verified. At runtime, on the port build only, it silently reads or
writes the wrong field: adjacent struct data, not the intended one. Nothing
signals this except wrong game behavior (or nothing at all, if the
misread bytes happen to satisfy whatever check follows).

### Measured example: `func_801D261C` and `SystemMenu`

Written, differentially tested, and **reverted** in this run
(`docs/evidence/port-baseline-20260921/README.md`, "Addendum ... closing
ranked item #3"). The body computed `g_Menu + menu2Choice*0x80 + {0xAE8,
0xB5D, 0xB5E, ...}` — retail's own numeric offsets into `SystemMenu` — with
no named field for that scratch region. Measured directly with `offsetof`
on this host build:

```
offsetof(SystemMenu, renderContext)        = 0x3a8   (retail 0x308,  drift +0xa0)
offsetof(SystemMenu, menu1Choice)          = 0x3e2   (retail 0x336,  drift +0xac)
offsetof(SystemMenu, menu2Choice)          = 0x3e4   (retail 0x338,  drift +0xac)
offsetof(SystemMenu, pManager)             = 0x3e8   (retail 0x33C,  drift +0xac)
offsetof(SystemMenu, unk32C)               = 0x3d0   (retail 0x32C,  drift +0xa4)
offsetof(SystemMenu, transitionEffectState)= 0x3c9   (retail 0x329,  drift +0xa0)
offsetof(SystemMenu, pGfxEnv)               = 0x260  (retail 0x1D4,  drift +0x8c)
sizeof(SystemMenu)                          = 0x20c8 (retail 0x1E98, drift +0x230)
```

`SystemMenu` has several native (8-byte) pointer fields ahead of these
(`pGfxEnv`, `unk32C`, `unk330`, `unk2DC`, `unk2E0`, `unk2E4`, …), each
retail-4-byte, so the drift accumulates field by field. It is **not
confined to pointers**: `gfxEnvs[2]` (a `GfxEnvironment[2]`, no pointer
fields of its own) *also* drifts — `sizeof(GfxEnvironment)` measures 0xf8 on
this host against retail's 0xb4 — because it embeds `unsigned long ot[16]`,
and `unsigned long` is 8 bytes on this LP64 host, 4 bytes on PSX. Read
"pointer" throughout this section as shorthand for "pointer or native
`long`/`unsigned long`" — anything host-width.

### Detection method

**A differential test against retail bytes, that actually exercises the
offending offset, is the only detection method that works.** Static review
does not catch this — the offset matches the disassembly, that is the whole
problem. Concretely:

1. Extract or write the port C body.
2. Run the **real retail MIPS bytes** for the same function (from
   `disc/<overlay>.bin`) through this project's MIPS interpreter
   (`PcPortMipsRun`, see `pc_port/src/battle_mips_adapter.h`) against an
   emulated PSX-address bus, from the same initial state.
3. Compare final memory state field-by-field.

A whole-struct `memcmp` between the host struct's raw bytes and the
interpreter's retail-address memory does **not** work once any field in
the struct has drifted — the two byte layouts are no longer even the same
shape. Compare through the host struct's own named field (letting the
compiler resolve its real, current offset) against the interpreter's
literal retail numeric offset instead. `pc_port/tests/
menu_misc_coexistence_test.c` does exactly this for `MenuUnk2` (which has
the same hazard via an embedded `TIM_IMAGE`, four pointers deep) — worth
reading as the template.

A quick, cheaper pre-check that does not require the differential harness:
`offsetof(StructType, fieldName)` on the *host* build, compared by hand
against the retail numeric offset in the disassembly or the struct
comment. Disagreement proves the hazard exists for that field; agreement
only proves it for that one field, not the rest of the struct.

### The safe pattern

Access every field through its name: `g_Menu->pManager`, never
`*(void**)((u8*)g_Menu + 0x33C)`. The compiler resolves the *real* offset
on whichever target it is building for, so named access is correct on both
the 32-bit matching build and the 64-bit port build unconditionally — this
is not a workaround, it is the only representation-independent way to
write the access.

If the target byte range genuinely has no named field (an unmapped
"scratch" region, as `func_801D261C` needed), that is a decompilation gap,
not a layout problem: the struct needs the field named and sized correctly
before a raw-offset body can be written safely. Do not paper over the gap
with a raw offset "because it's only used once" — see the next section.

**Struct fields containing only bytes/half/word arrays with no pointer or
`long` members never drift**, and can be safely reached by raw offset
(`g_GameState`, declared as a flat `unsigned char[0x4600]`, is the working
example already in this codebase — `func_801E4A28`/`func_801E4D10` reach it
by raw offset throughout and are differentially verified). The hazard is
specifically: raw offset **past** a pointer-or-`long` field, in a struct
that is not just a flat byte array.

### Known-affected: do not assume this is theoretical

A prior session already discovered part of this exact hazard for `SystemMenu`
and built a partial, self-documenting workaround: `MenuPsxPointerSlot()`
(`src/menu/main/misc.c:3524`, called from `MenuRawPointer`/
`MenuStoreRawPointer`) carries the comment *"Retail g_Menu+off is a 4-byte
pointer slot. Native SystemMenu inflates, so raw byte offsets land in
unrelated fields ... Map the known PSX-width slots onto their struct
members"* and a `switch` mapping twelve specific retail offsets to their
correct host fields. **Its `default` case still does
`(u32*)((u8*)g_Menu + retailOffset)`** — safe today only because every
current caller passes one of the twelve enumerated offsets (verified: all
~80 call sites of `MenuRawPointer` use only 0x34C/0x358/0x35C/0x360/0x434/
0x44C/0x3A8+4i). A future caller passing any other offset hits the unsafe
fallback with no warning.

Separately — and not behind that helper — this run found **five port-
reachable functions in `src/menu/main/misc.c` that read or write `g_Menu`
by raw retail offset past the drift point, unconditionally compiled into
the actual port build, with no `MenuRawPointer`-style guard**:

| function | file:line | retail offset used | field | measured host offset |
| --- | --- | --- | --- | --- |
| `func_801C57A4` | `misc.c:326` | `0x329` | `transitionEffectState` | `0x3c9` |
| `func_801CD710` | `misc.c:2976` | `0x338` | `menu2Choice` | `0x3e4` |
| `func_801D9E3C` | `misc.c:6847` | `0x32C` | `unk32C` | `0x3d0` |
| `func_801D0D90` | `misc.c:4015` | `0x1D4` | `pGfxEnv` | `0x260` |
| `func_801D6194` | `misc.c:6004` | `0x308` | `renderContext` | `0x3a8` |

Every one of these is a currently-shipping function reading or writing the
**wrong field** on this host build, right now — not a hypothetical. See
`docs/evidence/port-baseline-20260921/README.md` ("pointer-drift
characterisation" addendum) for the full survey, method, and what remains
unchecked. These are reported, not fixed, in that pass; fixing them (route
through the named field, or extend `MenuPsxPointerSlot`-style mapping) is
future work.

## Class B — address-width confusion (`battle`, measured 2026-09-21)

A second, independently-discovered sub-class, found the same day by the
`battle` worker fixing six divergent port bodies against the differential
prover. Same root cause as Class A — retail assumes a 32-bit machine, this
host is 64-bit — different mechanism: not a struct's *compiled layout*
drifting, but a **32-bit PSX address or index being used as if it were
native to this host**. Five of six bodies were wrong, and — same as
`func_801D261C` — **none were logic errors**; retail control flow was
already correct in every case. Four distinct failure shapes, all found and
fixed in `func_800AA7DC`, `func_800885D0`, `func_800AA79C`,
`func_800AA760`, `func_800BEDE8`:

- a **guest (PSX) address dereferenced directly as a host pointer** — valid
  on the PSX, where every address is native; wrong on the host, where a
  guest address is just a small integer that happens to look like one;
- an **unbounded index that runs past the end of host RAM** — retail's
  `lw`/`lbu` and this project's MIPS interpreter both wrap inside the
  emulated 2 MiB PSX RAM; a host body computing the same index without that
  wraparound reads or writes past its buffer instead;
- a **64-bit host pointer truncated to `u32` and then used as an address**
  — the inverse of Class A's problem, and easy to introduce by habit if
  you are used to Class A's "cast down to a plain integer" fix;
- a **pointer-typed alias where the retail code meant the word** — a
  shared symbol (`D_800C3610[0]` in this case) declared as a pointer type
  where retail's `lw`/`sw` operates on the 32-bit word stored there, not on
  whatever it points to; indexing or dereferencing the alias reads/writes
  the pointee instead of the value retail actually touches.

Two details that sharpen the detection advice from Class A:

1. **A partial fix can still look plausible and still be wrong.** One body
   had a wild pointer *and* a wild index feeding it; translating only the
   dereference left the prover rejecting the result, because the index was
   the other half of the bug. A body passing "looks more correct now" is
   not evidence it is correct — only the differential result is.
2. **A correct fix can create a new instance of this class.** `func_800885D0`'s
   double-scaled index was introduced by an *earlier, correct* type fix (a
   symbol retyped to a pointer-to-array-of-28-bytes) that changed how far
   an existing raw index computation actually reached. Type-fidelity
   improvements are not free of this hazard; re-run the differential prover
   over everything downstream of a type change, not just the function it
   was made in.

The safe pattern is the same shape as Class A's: the fix lives inside the
`XENO_PC_PORT` branch, the retail-shaped body (which is correct for its own
32-bit target) stays untouched in `#else`. Verified by the differential
prover; `battle.bin` remains byte-identical (matching-side unaffected, as
expected — this is a port-side-only class, like Class A). Not
independently re-verified by this worker; cited here because it is proven
by the same detection method this document already recommends, converging
with Class A from a different overlay on the same day.
