# Open Issues

Rules:

- If it's in this file, it's OPEN. Closing means DELETING the entry, not annotating it.
- Every item carries a reproducer command, not just a description.
- Every item carries `Last verified @ <commit>`. Stale verification = suspect claim.
- Every claim is tagged by evidence class:
  proven | build-verified only | observed | inferred

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

## Unimplemented opcodes in func_800248D4

Opcodes: 0x85, 0x8E, 0x98, 0xBE, 0xC8, 0xD4, 0xE2, 0xFA
Evidence: proven (assertion in source, enumerated)
Repro: unknown which scenes dispatch these — needs a dispatch sweep first
Last verified @ ed61298

## CompMatrix PsyQ decomp match

Audit is DONE (semantics verified vs retail 0x8004931C-0x80049478).
The MATCH is outstanding — handwritten GTE sequence, codegen-focused work.
Evidence: proven (audit); match not attempted
Last verified @ ed61298

## func_8002C700 / ModelPrimQuadFT4Variant0

Host-specific structural rewrites, no byte-match proof. Hardest remaining.
Evidence: proven (known port-only, not matched)
Last verified @ ed61298

## Map015 entrance 0 — func_8008399C assertion

Repro: launch Map015 entrance 0; assertion fires, map not runnable
Evidence: observed (surfaced during ABR scene sweep)
Last verified @ ed61298

## LIBGTE.C invalid UTF-8 byte

Blocks patch-editor modification of the file. Maintenance item.
Repro: iconv -f utf-8 -t utf-8 < pc_port/extern/PsyCross/src/psx/LIBGTE.C > /dev/null
Evidence: proven
Last verified @ ed61298
