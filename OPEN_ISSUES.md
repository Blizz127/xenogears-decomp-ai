# Open Issues

Rules:

- If it's in this file, it's OPEN. Closing means DELETING the entry, not annotating it.
- Every item carries a reproducer command, not just a description.
- Every item carries `Last verified @ <commit>`. Stale verification = suspect claim.
- Every claim is tagged by evidence class:
  proven | build-verified only | observed | inferred
- For PSX symbol-width findings, classify the retail storage pattern before
  proposing a fix: direct data array, packed 32-bit pointer slot loaded/stored
  with `lw`/`sw`, or embedded array addressed with `lui`/`addiu`. Commit
  `1229ea1` is the reference implementation for all three forms.

---

## Missing matching ELFs block typed stubs and full matching

`build/out/slus_006.64.elf`, `field.elf`, `member_change_menu.elf`, and
`shop_menu.elf` are not available to the port build. They are required to
classify undefined symbols safely as functions or correctly-sized data before
generating `stubs.c`. The driver now fails explicitly when its typed stub
manifest differs from the current undefined set; it no longer silently reuses a
stale manifest.

The supported `xenogears-dev` MIPS toolchain is available and compiles the
matching graph through 446 of 459 Ninja steps. The main SLUS link then fails in
`src/slus_006.64/psyq/libgte.c`: calls such as `gte_SetRotMatrix`, `gte_ldlvl`,
and `gte_rtpt` remain unresolved because the matching dependency set includes
`psyq/libgte.h` but not the inline GTE macro definitions. Repair that matching
build integration before expecting ELFs to exist.

This blocks safe incremental activation of the sound/menu overlays, full-project
objdiff (including the missing `fade_render.c.o` path), and automatic typed-stub
regeneration.

Repro: `distrobox enter xenogears-dev -- bash -lc 'cd
/var/home/blizz/Projects/xenogears-decomp && make -B build'`; the SLUS link
reports unresolved `gte_*` helpers from `libgte.c` and produces no ELF. Then run
`./scratchpad/run_build_port.sh` after any configuration changes the undefined
set; the port driver refuses to link without a matching typed manifest.
Evidence: proven
Last verified @ a0c5461 (matching build run locally)

## Unported sound subsystem behind the compile allowlist

`src/slus_006.64/system/sound.c` now compiles after retail-confirmed prototype
and byte-cursor corrections, but activating it exposes 39 unresolved
`INCLUDE_ASM` sound-path definitions. It remains intentionally allowlisted as a
temporary compile-success blocker until those functions can be supplied by real
ports or typed stubs generated from matching ELFs. This is a sound-subsystem
porting workstream, not a residual C compile-error task.

Repro: compile `sound.c` with the port GFLAGS/INC, then run
`./scratchpad/run_build_port.sh`; the driver refuses to let the allowlisted TU
silently enter the link.
Evidence: proven
Last verified @ f849676

## Unported member-change and shop menu overlays

Both menu `misc.c` TUs now compile after the retail-proven `POLY_FT4` window
border correction and the `ShopMenuBuyMenu` declaration fix. Their compile
errors had been masking 15 unresolved `INCLUDE_ASM` dependencies: three in the
member-change overlay and twelve in the shop overlay. They remain on the
allowlist until those dependencies are ported or matching ELFs can generate
typed stubs; they must not be made live merely because they compile.

Repro: compile both menu TUs with the port GFLAGS/INC and compare their undefined
symbols against their `INCLUDE_ASM` declarations. The guarded build then rejects
the newly compiling allowlisted TU before link.
Evidence: proven
Last verified @ a0c5461

## Work-list runtime routing: decomp source vs. host-safe port override

`src/slus_006.64/system/work_list.c` now compiles, but it cannot be linked
alongside `pc_port/src/work_list_port.c`: nine definitions overlap. The port
file is a deliberate, partial host-layout override, not a full replacement.
It uses PSX-layout 0x1C entries with 32-bit stored pointers because the original
decomp layout is unsafe for the 64-bit host; it was added to restore the
runtime-critical work-list paths that were previously stubbed.

Do **not** resolve this by simply adding `work_list.c` to an exclusion list.
That would discard the exports only present in the decomp TU (including
`WorkListsFreeAllEntries`, allocation helpers, getters, and other nonmatching
functions), which are currently stubbed and would need the same layout/routing
treatment before becoming usable.

Open design question: should the complete work-list subsystem adopt the
PSX-layout port model, or should the remaining `work_list.c` exports be ported
individually into the host-safe implementation?

Repro: compile `src/slus_006.64/system/work_list.c` with the port GFLAGS, then
compare its defined symbols against `pc_port/src/work_list_port.c`; the shared
definitions produce duplicate-definition link errors if both objects are linked.
Evidence: proven (source comments, symbol comparison, and runtime-recovery
history)
Last verified @ 37a3022

## Unimplemented sprite-animation opcodes in func_800248D4

`func_800248D4` is the **sprite animation-script dispatcher**, not the field
script VM. Its dedicated unimplemented set is now `0x85, 0x8E, 0x98, 0xC8,
0xD4, 0xE2, 0xFA`. Opcode `0xBE` was implemented from the shared retail
`0x80` handler at `0x80024A84-0x80024B9C`.

The strict static scanner in
`tools/scripts/psx/scan_field_anim_opcodes.py` decoded all 730 field maps,
3,234 per-map sprite packages, and 16,382 animation entries with zero aborts.
Only the now-implemented `0xBE` is reachable from a per-map animation entry:
seven distinct sites in Map047 package 0 animations 0/1/2, Map048 package 0
animation 2, and Map334 package 0 animations 0/1/2.

The other seven are **not reachable in any per-map animation package**. Do not
call them unused: global party, battle, and special-animation packages were not
part of this field-map scan and remain a real coverage gap.

Repro: `python3 tools/scripts/psx/scan_field_anim_opcodes.py --json
scratchpad/field_anim_opcode_scan_all.json`; expect 730 maps, 3,234 packages,
16,382 fully-decoded entries, zero aborts, and seven reachable `0xBE` sites.
Evidence: proven for per-map packages; global animation-package coverage open
Last verified @ 4d7cb57

## CompMatrix PsyQ decomp match

Audit is DONE (semantics verified vs retail 0x8004931C-0x80049478).
The MATCH is outstanding — handwritten GTE sequence, codegen-focused work.
Evidence: proven (audit); match not attempted
Last verified @ ed61298

## Audit remaining populated D_8004FE50 host walkers against retail

The populated model-primitive routing table still contains host-specific
walkers whose packet layout, transform count, culls, coordinate writes, and OT
depth derivation have not all been compared instruction-by-instruction against
their retail table targets.

Two defects have now demonstrated that correctness-by-inspection is not enough
for this family:

- `func_8002E688` used the wrong depth source and carried port-only culls before
  the retail minimum-SZ path was restored (`fea685a`).
- `D_8004FE50[0x0C].proc[2]` routed a four-vertex F4 packet through a
  three-vertex walker, leaving `xy3` stale and producing Map334's giant black
  wedges (`6076cd4`).

Audit each remaining populated entry by reading its target from
`asm/slus_006.64/data/3F290.sdata.s`, then comparing the complete retail walker
in `asm/slus_006.64/system/temp2.s` against the host override. Keep
`func_8002C700` and `ModelPrimQuadFT4Variant0` in this scope; do not treat a
shared packet shape as proof that transform, cull, or depth behavior is shared.

Repro: enumerate non-null `D_8004FE50[].proc[]` entries in
`pc_port/src/game_overrides.c`, map each to the retail target table beginning at
`3F290.sdata.s:2712`, and record per-entry parity for packet stride, vertex
count, GTE sequence, FLAG/NCLIP/screen checks, SXY writes, and OT derivation.
Evidence: proven need (two live defects found in the host walker family); the
remaining entries are unaudited, not known-bad
Last verified @ 6076cd4

## Map015 entrance 0 — func_8008399C assertion

Repro: launch Map015 entrance 0; assertion fires, map not runnable
Evidence: observed (surfaced during ABR scene sweep)
Last verified @ ed61298

## LIBGTE.C invalid UTF-8 byte

Blocks patch-editor modification of the file. Maintenance item.
Repro: iconv -f utf-8 -t utf-8 < pc_port/extern/PsyCross/src/psx/LIBGTE.C > /dev/null
Evidence: proven
Last verified @ ed61298
