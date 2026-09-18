# Encounter weight table aliasing: D_80065ADC is D_800658DC + 0x200, 2026-09-06

**Proof limit up front.** The fix compiles and the new regression test passes
(2 cases, 40 assertions, 3/3 mutation controls correctly rejected), and the
aliasing is confirmed at object level with `nm`. What is **not** shown here: a
full native port link (this machine has no `xenogears-dev` distrobox and no
SDL2/OpenAL, so `pc_port/build_port.sh` cannot run) and any in-game observation
of an encounter actually firing. Claims are tagged **proven** (retail asm /
retail symbol data / existing ported code), **build-verified** (compiled,
linked and executed here), **observed** (seen at runtime in the game), or
**inferred** (reasoned from source, not executed).

Environment note for whoever picks this up: the shell was dead for most of this
session (every Bash command, including `true`, exited 1 with no output) and
recovered part-way through; `/tmp` is at 80% with a 9.9G `xeno-field-asmonly-*`
directory from a concurrent agent, and two test builds died on
`Disk quota exceeded` / `No space left on device` before the runner was made to
share one `misc4.o`. Check `/tmp` space before blaming a build failure here.

## The retail object

Retail keeps the per-map encounter section as ONE contiguous blob at
`0x800658DC`:

| span | contents |
| --- | --- |
| `+0x000 .. +0x1FF` | 16 formation records, stride `0x20` |
| `+0x200 .. +0x20F` | 16 per-formation encounter weight bytes == `D_80065ADC` |
| `+0x210 .. +0x211` | 2-byte tail (map 2 decodes `0x212` bytes total) |

- **proven** — record stride `0x20`: `asm/battle/nonmatchings/main/func_80070F40.s`
  (~lines 139-152) loads `%hi/%lo(D_800658DC)`, reads the selected formation
  index from `D_80059508`, does `sll $a1, $a1, 5` and calls `memmove`.
- **proven** — weights live at `+0x200` in the record blob: the already-ported
  world-map selector `pc_port/src/world_map_helper_75e7c.c:104-135` computes
  `weight_addr = record_base + 0x200 + bucket * 0x10`, reads 16 weight bytes,
  and then copies only the leading `0x200` record bytes.
- **proven** — the retail span is `0x230`: `config/symbol_addrs.slus_006.64.txt:1004-1006`
  gives `D_800658DC` size `0x200`, `D_80065ADC` size `0x30`, flush to
  `D_80065B0C`.
- **proven** — the writer already runs and targets the unshifted base:
  `src/field/main/misc3.c:831-833` (FieldLoad) calls
  `FieldLZSSDecompress(NULL, D_8005A4E0 + *(u32*)(D_8005A4E0+0x148), D_800658DC)`.
  Retail correspondence `80071054..8007106C` and the 530-byte map-2 decode are
  established in `docs/evidence/opening-battle-encounter-20260905/README.md`.

## The defect

The port split that single retail object into two unrelated native objects:

- `pc_port/build_native/stubs.c:232` — `unsigned char D_800658DC[1024]`,
  emitted by `tools/scripts/gen_port_stubs.py:145-148` as `max(elf, symbol_addrs,
  16) * 2` = `0x200 * 2`.
- `pc_port/src/game_overrides.c` (old line 543) — a standalone `u8 D_80065ADC[16]`.

**inferred** — consequence: the whole `0x212`-byte decode landed inside the
first object, so the weight table was never written. The reader
`src/field/main/misc4.c:314-333` sums `D_80065ADC[0..15]`, gets `sum == 0`, and
the descending weighted pick at lines 327-332 requires `D_80065ADC[i] != 0`, so
`selected` stays `-1` and line 333 returns early. No random encounter could
fire anywhere in the port, including the mountain path (map15) and Blackmoon
Forest (map16). This is a fidelity gap, not a hang: the early return happens
before `D_800ADBDC` / `D_800ADBD0` are touched.

## The fix

`pc_port/src/data_field.c` (inserted immediately before the existing
`#undef FIELD_BSS_ALIAS`):

```c
unsigned char g_SlusBss_800658DC[0x230] __attribute__((aligned(8)));
FIELD_BSS_ALIAS(D_800658DC, g_SlusBss_800658DC, 0x000);
FIELD_BSS_ALIAS(D_80065ADC, g_SlusBss_800658DC, 0x200);
```

This is the port's established offset-aliasing mechanism, defined at
`pc_port/src/data_field.c:1155-1156` as
`asm(".globl " #name "\n.set " #name ", " #block " + " #offset)` and used for
every other shared BSS block in that file. Size `0x230` is the retail span, so
the `0x212`-byte decode fits with its tail intact. The naming follows the
existing `g_FieldBss_<addr>` convention, with `g_SlusBss_` marking that this
block is main-executable BSS rather than field-overlay BSS.

`pc_port/src/game_overrides.c` — the standalone `u8 D_80065ADC[16]` definition
is removed; the surrounding comment now records why it must not come back.
`D_80059508` / `D_800594F8` are untouched.

Nothing else changed: the `FieldLZSSDecompress` call is unmodified,
`func_80079288` is unmodified, no weights are seeded, and no structure that
belongs to the byte-matching build was widened.

- **proven** — `tools/scripts/gen_port_stubs.py` needs no change. Its only
  explicit exclusion list (`helper_names`, line 171) applies to *function*
  stubs; there is no data-symbol skip list. `pc_port/build_port.sh:911-913`
  derives `undef.txt` from the **trial link's** `undefined reference to` errors,
  not from `nm -u` over objects, so a real definition in a linked port TU stops
  the symbol from ever reaching the generator.
- **proven** — `D_80065ADC` is not in `pc_port/port_owned_overrides.txt` (that
  manifest contains only functions), so removing its definition cannot trip the
  build's ownership-retirement check.

## Known follow-up: the stale generated stub

**unverified / action required.** `pc_port/build_native/stubs.c:232` still
contains `unsigned char D_800658DC[1024]`. That file is a generated build
artifact and was deliberately not hand-edited here. Until it is regenerated:

- with matching ELFs present, `build_port.sh` regenerates `stubs.c` from the new
  trial link and the competing definition disappears (**inferred** from
  `build_port.sh:911-929`);
- without matching ELFs, the fallback validator at `build_port.sh:937-971`
  will reject the existing manifest as carrying a **stale stub**
  (`D_800658DC` is stubbed but no longer undefined) and refuse to link — which
  is the intended fail-closed behaviour, not a new bug;
- if the stale `stubs.o` were linked anyway, the final link would fail with
  `multiple definition of D_800658DC`.

So the fix takes effect at link time only after a `build_port.sh` run that
regenerates `stubs.c`. The regression test prints a loud warning while the
stale line is still present.

**Also noted, not changed (out of scope):** `pc_port/build_native/stubs.c:233`
emits a separate `D_80065B08[32]`, but retail places `0x80065B08` *inside* the
`D_80065ADC` region (offset `+0x2C` of `0x30`). The current decode only reaches
`+0x212`, so it cannot collide today, and no defect was proven for it.

## Regression test

- `pc_port/tests/encounter_weight_aliasing_prod_test.c`
- `pc_port/tests/run_encounter_weight_aliasing_test.sh`

The test links the **real production owners** — `pc_port/src/data_field.c` for
the aliased storage and `src/field/main/misc4.c` for the real `func_80079288` —
with an explicit-spy driver (`rand`, `func_8008E718`, `func_80281204`,
`LoadGameStateOverlay`, plus the gate globals normally supplied by `stubs.c`).
The roll is not re-implemented. Unported references from the two production TUs
are tolerated via `--unresolved-symbols=ignore-all`; the exercised path only
touches symbols the test defines or `data_field.c` owns.

Case 1 writes a realistic `0x212`-byte section at `D_800658DC` (the exact thing
`FieldLZSSDecompress` does) and asserts `D_80065ADC[i] == source[0x200+i]` for
all sixteen bytes, that `&D_80065ADC[0] == &D_800658DC[0x200]`, that the record
area and tail survive, that the weight sum is 60 rather than 0, and that with
`rand() == 24500` the roll selects formation 5 and runs the full commit path
(`D_80059508`, `D_800594F8`, `D_800B2290`, `D_800ADBDC`, `D_800ADBD0`, the
overlay load, and the battle handoff argument).

Case 2 uses an all-zero weight set and asserts the retail early return still
happens: sum 0, no formation selected, no overlay load, no battle handoff,
`D_800ADBDC` and `D_800ADBD0` untouched. This is the guard against seeding
non-zero weights to make encounters "work".

Three controls rebuild the same test against mutated copies of `data_field.c`
and **must** fail: `split-objects` (the exact pre-fix layout — a standalone
`D_800658DC[1024]` plus a separate `D_80065ADC[16]`), `weights-at-0x1F0`, and
`weights-at-0x210`. A structural pin additionally fails if `game_overrides.c`
ever regains a standalone `D_80065ADC`, or if the backing block shrinks below
`0x230`.

### Result — build-verified

```
ENCOUNTER WEIGHT STRUCTURE OK (block 0x230, aliases +0x000/+0x200)
ENCOUNTER WEIGHT CASES 2 ASSERTIONS 40
ENCOUNTER WEIGHT PASS
ENCOUNTER WEIGHT CONTROL REJECTED split-objects
ENCOUNTER WEIGHT CONTROL REJECTED weights-at-0x1F0
ENCOUNTER WEIGHT CONTROL REJECTED weights-at-0x210
ENCOUNTER WEIGHT CONTROLS REJECTED 3/3
ENCOUNTER WEIGHT OK
```

Controls failed with 16 / 28 / 14 assertion failures respectively, and for the
right reasons. `split-objects` reproduces the original defect exactly:

```
ASSERTION alias/D_80065ADC-is-D_800658DC+0x200
ASSERTION weights/D_80065ADC[0] expected=20 actual=0
ASSERTION roll/weight-sum expected=60 actual=0
ASSERTION roll/selected-formation expected=5 actual=238   (238 = untouched 0xEE sentinel)
```

`weights-at-0x210` reads the 2-byte tail instead of the weights
(`actual=171 / 205` = `0xAB / 0xCD`), confirming the window is aimed by the
alias offset and nothing else.

### Object-level confirmation — build-verified

`nm -S` on the production `data_field.o` built with the port's own flags:

```
0000000000014d98 0000000000000230 B D_800658DC
0000000000014f98 0000000000000230 B D_80065ADC
0000000000014d98 0000000000000230 B g_SlusBss_800658DC
```

`0x14f98 - 0x14d98 == 0x200` exactly, both in `.bss`, both resolving to the one
`g_SlusBss_800658DC` block. `nm --defined-only` on the production
`game_overrides.o` reports **0** definitions of `D_80065ADC` and still defines
`D_80059508` and `D_800594F8`.

### No overlap with the existing battle-side test

**build-verified** — `pc_port/tests/battle_encounter_alias_test.c` covers a
different concern (retail `func_80070F40` copying one `0x20`-byte record to
guest `8006F9DC`) and declares its own local `uint8_t D_800658DC[0x200]` as
scaffolding, so it never links `data_field.c`. It was re-run after this change
and still passes all three optimisation modes with 9 rejected mutants. It
independently corroborates the `0x20` record stride and the `0x200` record area,
but says nothing about the weight table at `+0x200`.

### Still outstanding

- **not verified** — the full native port link. `pc_port/build_port.sh` needs
  the `xenogears-dev` distrobox (absent here), so the end-to-end link with the
  regenerated `stubs.c` has not been exercised.
- **not observed** — no encounter was seen firing in game. Proving retail
  behaviour on map15 / map16 still requires a native run after the port link is
  rebuilt.
