# Battle overlay: byte-exact against retail (2026-09-10)

Worktree: `/var/home/blizz/Projects/xenogears-decomp-ai`, branch
`experiment/worldmap-open-gates-20260823`, HEAD `3a3e7aac`. No commit, stage or
push. The whole-game / byte-for-byte objective stays active.

## Outcome

`build/out/battle.bin` now reproduces `disc/battle.bin` exactly, from a clean
`make build`, and is pinned in the repository's ROM gate:

```
rom-check: clean rebuild (rm -rf build linker; make build) ...
PASS  build/out/battle.bin                 1830b4ef1fe37129 (343936 bytes)
```

`make rom-check` full output this pass: slus_006.64, field.bin and shop_menu.bin
FAIL (the pre-existing known-red entries, hashes unchanged: `f7c1f169`,
`c012e0d8`, `770921de`), member_change_menu and battle PASS. `menu.bin` is the
usual WIP line (`0f6d8aac`). `make build` = 483/483 tasks, 0 failures.

## Starting state (authoritative, re-measured this pass)

`gears.toml` already listed `battle` in `overlays` (uncommitted lane work), but
the matching build could not finish: `ninja` failed at the battle link with 16
undefined references (`D_800C204C`, `D_800C2050`, `D_800C2054`, `D_800C207C`,
`D_800C2080`, `D_800C2084`, `D_800C2F4C`, `D_800C3238`, `D_800C3250`,
`D_800C34B0`, `D_800C3544`, `D_800C354C`, `D_800C3620`, `D_800C3624`,
`D_800C3664`, `D_800C3750`). `make build` stopped there, so the checksum gate
could not be evaluated at all.

## Four defects, all in the new lane's modelling of the overlay

**1. The data tail was modelled as code.** `- [0x1450, c, main]` ran to the BSS
boundary, so `src/battle/main.c` carried `INCLUDE_ASM` entries
(`func_800C204C`, `func_800C2FCC`) over data. Code ends at file `0x5255C` /
VRAM `0x800C204C` (last real function `func_800C11CC`): `0x800C204C` is a byte
flag read/written with `lbu`/`sb` (`80080C08`, `80080E24`, `80081130`), never a
`jal` target, and `0x5255C..0x534DC` is pointer/parameter data (1672 words, 139
in-overlay pointers, e.g. a self-referential `.word 0x800C2FCC` at `0x53544`).
Splat cannot decode it as MIPS, which is why it invented those "functions" and
why the code's `D_*` references never resolved. Now `- [0x5255C, data]`.

**2. The leading block was placed after `.text`.** It was `- [0x0, data]`; gears
puts `data` subsegments after `.text`, so the built file led with code while
retail leads with that block. Now `- [0x0, rodata]`, the idiom every other
overlay uses. First differing byte moved from `0x0` to `0x8D0`.

**3. Compiled C bodies cannot live in this TU.** `INCLUDE_ASM` expands to a
file-scope `__asm__` block, and the pinned compiler emits *all* of those before
*any* compiled body (measured: `build/src/battle/main.c.o` symbols run
`func_80070F40..func_800C11CC` then the ten C functions at `0x51070+`). Any C
body therefore leaves its retail slot and shifts every later address — even
after defects 1 and 2 were fixed the file still differed in 27,388 runs because
of it. All ten are now the repository's standard split:

```c
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main", func_800B16A4);
#else
/* port-side body */
#endif
```

`func_800B2AEC` already lived that way: its body is
`src/battle/primitive_colors.inc`, included by `pc_port/src/game_overrides.c`
for the port and no longer by the matching build.

**4. The BSS was placed 4 bytes high.** All `.bss` inputs carry 16-byte
alignment (the GNU `as` default for this target), so the address-less
`.battle_bss (NOLOAD)` was rounded from `0x800C3A6C` to `0x800C3A70` and every
BSS symbol — and every `%lo` reference to it — was 4 bytes high (460 labelled
symbols misplaced; `D_800C3EA4` assembled as `0x800C3EA8`). Splat emits an
explicit section address only for a segment whose entries are all no-load, so
the BSS is now its own top-level segment:

```yaml
  - name: battle_bss
    type: code
    start: 0x53f7c
    vram: 0x800c3a6c
    align: 4          # must not advance __romPos (all-NOLOAD)
    subsegments:
      - { vram: 0x800c3a6c, type: bss }
```

That emits `.battle_bss_bss 0x800C3A6C (NOLOAD)`. `align: 4` matters: with the
default 16 the segment's end alignment advanced `__romPos` from `0x53F7C` to
`0x53F80`, sliding the trailing databin and growing the ROM to 343,940 bytes.
`type: bss` at top level is not usable in pinned splat 0.33.2 — `CommonSegBss`
dereferences `self.parent` — hence the `type: code` wrapper.

## Verification

- `make build` (pinned podman toolchain `localhost/xenogears-dev-toolchain:current`,
  `. /.venv/bin/activate`, splat 0.33.2 / spimdisasm 1.33.0): 483/483, 0
  failures. Logs: `scratchpad/battle-overlay-split-20260910/build10.log`,
  `.../port-build.log`.
- `make rom-check` (the repo's own from-clean gate): `PASS build/out/battle.bin
  1830b4ef1fe37129 (343936 bytes)`. `config/checksum.sha` gained this pin; no
  existing pin was changed.
- Symbol identity: 705 labelled symbols in `battle.elf`, **0** whose address
  differs from the address encoded in its own name (was 460).
- Byte equality: `build/out/battle.bin` == `disc/battle.bin` == SHA-256
  `1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291`.
- No cross-overlay drift: every recorded overlay hash is unchanged.
- Port side unaffected: `./pc_port/build_port.sh` -> `LINK OK`, 69 function
  stubs / 547 data symbols, and `func_800B2AEC` is still provided by the port
  (`nm pc_port/build_native/xeno-port` -> `T func_800B2AEC`).
- `pc_port/tests/run_battle_primitive_colors_retail_test.sh` -> `B2AEC GREEN`:
  4230 cases in each of O0/O2/UBSan against the actual `disc/battle.bin` leaf and
  the actual SLUS clamp, 7 negative controls rejected.
- `pc_port/tests/run_battle_animation_leaf_test.sh` -> `PASS` (28 index/wrap and
  65 variable-record cases, 3 controls) — this is the test that consumes the
  guarded `func_800B168C` / `func_800B16A4` bodies.
- NOT_RUN: any runtime/visual observation of the battle overlay. This pass is
  build/gate evidence only.

## What this does and does not establish

Establishes: the battle overlay is a byte-exact retail reconstruction, gated
from clean, and the port still works.

Does not establish: that any of it is *decompiled*. Every non-trivial function
in the overlay is still `INCLUDE_ASM` (retail instructions re-assembled). The
next unit for this lane is matching C, starting with the two functions that
already had bodies:

- `func_800B2AEC` (retail 2140 bytes / 535 instructions) — the port body in
  `primitive_colors.inc` is behaviour-audited
  (`docs/evidence/opening-battle-encounter-20260905/f2-primitive-colors-audit.md`)
  but an 844-byte clean rewrite cannot be a matching transcription. Note that
  installing a matching body also requires giving it a TU that emits it in
  retail position (defect 3 above).
- `func_800B16A4` (retail 76 bytes) — the port body is now retail-shaped (a
  rising index loop; retail uses `addiu a1,a1,1` / `bne a1,a3`) and matches 18
  of 19 instruction words. The residual is the accumulator init: retail
  `addu a2,a1,zero`, emitted `move a2,zero`. Both `gcc-2.7.2-psx` and
  `gcc-2.6.0-psx` fold the source-level copy, so this needs either the right
  compiler regime or a different source shape.

