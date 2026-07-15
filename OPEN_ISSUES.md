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

## Sound is activated but the retail initialization tree is unported

`src/slus_006.64/system/sound.c` is linked and all ten audited sound layouts are
retail-correct, but the port's field-test boot bypasses retail's
`SoundInitialize(0)` call. Consequently `D_800595D8`, the packed 32-bit current
audio-manager address, remains zero while field code calls the sound API.
Replacing the live oracle stubs with their exact-matched bodies therefore faults
on the first manager-element access. Retail has no null guard; adding one would
hide the missing initialization rather than restore it.

Retail calls `SoundInitialize(0)` from `func_80019578` at
`0x80019668-0x8001966C`. `SoundInitialize` spans
`0x80037B88-0x80037DBC` and stores `func_8003B148(0x10)` into
`D_800595D8` at `0x80037D50-0x80037D68`. Its complete known dependency
tree has four legs:

- **Manager allocation:** `func_8003B148` calls `func_8003B32C` at
  `0x8003B194`; that calls real `SoundInitializeAudioManager` at
  `0x8003B358`; it calls stubbed `func_8003B930` at `0x8003B37C`; and
  `func_8003B930` calls stubbed `SoundHeapFree` at `0x8003B958`.
  Port leaf-up: `SoundHeapFree`, `func_8003B930`, `func_8003B32C`, then
  `func_8003B148`. The top function's allocation-failure exit also calls
  stubbed `SoundHandleError` at `0x8003B184`. That handler is not a leaf: when
  control flags `0x88` are clear, retail `0x8003F6E0-0x8003F724` reaches
  stubbed `SoundLoadWdsFile` and `func_80039E60` in addition to real
  `SoundSpuMemoryFreeBlock`, `SoundAddSedsEntry`, and `func_8003BDFC`.
- **CD mix:** `SoundInitialize` calls unported `func_800386C4` at
  `0x80037D04`; that path reaches unported `SoundSetupCdMix`.
- **Reverb:** real `SoundSetReverbModeWithAllocation` still reaches stubbed
  `SpuGetReverbModeType`, `SpuSetReverbModeType`,
  `SpuSetReverbModeDelayTime`, and `SpuSetReverbModeFeedback`.
- **Timer tick:** `SoundInitialize` registers unported `func_8003C020` as the
  recurring callback at `0x80037C7C-0x80037C84`. Its sound-tick graph reaches
  at least `func_8003E900`, `func_8003AE84`, `func_8003A838`,
  `func_8003EBF0`, and `func_8003EB5C`, all still unported.

`SoundSpuMemoryAllocateBlockAtAddress` (`0x800395B8-0x800396DC`) is an
independently portable leaf: its only call is the real
`SoundSpuMemoryGetFreeBlock` at `0x80039678`.

Hard sequencing constraint: route `SoundInitialize(0)` into the port boot
**only after all four legs are real**. Earlier routing would construct a
partially initialized manager and register a do-nothing tick callback, turning a
loud null dereference into quiet wrong state. Four live playback helpers
(`func_8003A20C`, `func_8003A344`, `func_8003A450`, and `func_8003A55C`)
already have objdiff-`{}` C transcriptions, but they must remain oracle-stubbed
until initialization is complete.

Repro: locally replace `func_8003A20C` with its exact-matched C body, rebuild,
and run `timeout 20s ./scratchpad/run_map001.sh`; GDB faults in
`func_8003A20C(arg0=8)` with the derived element address `0xB54`, proving
`D_800595D8 == 0`. Restore the oracle stub after the diagnostic.
Evidence: proven
Last verified @ b4244a7

## Unported member-change and shop menu overlays

Both menu `misc.c` TUs now compile after the retail-proven `POLY_FT4` window
border correction and the `ShopMenuBuyMenu` declaration fix. Their compile
errors had been masking 15 link-reachable `INCLUDE_ASM` dependencies: three in
the member-change overlay and twelve in the shop overlay. Matching ELFs now
exist and there is no port-symbol collision. The architecture decision is to
compile both TUs normally and use generated oracle stubs, with actual menu-route
activation and validation as the remaining implementation task.

Repro: compile both menu TUs with the port GFLAGS/INC and compare their undefined
symbols against their `INCLUDE_ASM` declarations, or remove the holds locally
and verify that the typed stub manifest regenerates and links.
Evidence: proven
Last verified @ fdd86e7

## Work-list runtime routing: decomp source vs. host-safe port override

`src/slus_006.64/system/work_list.c` now compiles, but it cannot be linked
alongside `pc_port/src/work_list_port.c`: nine definitions overlap. The port
file is a deliberate, partial host-layout override, not a full replacement.
It uses PSX-layout 0x1C entries with 32-bit stored pointers because the original
decomp layout is unsafe for the 64-bit host; it was added to restore the
runtime-critical work-list paths that were previously stubbed.

The runtime-layout decision is now made: keep `work_list.c` excluded as
matching/reference source and make `work_list_port.c` the canonical native
owner. Exclusion alone is not completion; remaining exports must be ported into
the packed-u32 implementation as live paths require them. See
[`Port-Coexistence-Architecture`](docs/wiki/Port-Coexistence-Architecture.md).

Repro: compile `src/slus_006.64/system/work_list.c` with the port GFLAGS, then
compare its defined symbols against `pc_port/src/work_list_port.c`; the shared
definitions produce duplicate-definition link errors if both objects are linked.
Evidence: proven (source comments, symbol comparison, and runtime-recovery
history)
Last verified @ fdd86e7

## Port-only source compilation is not fail-closed

The game-TU compile loop aborts on unexpected failures, but the port-only loop
only prints `FAILED` and continues. A failed `game_overrides.c`,
`archive_port.c`, `work_list_port.c`, data source, or other port source is
omitted from `GAME_OBJS`; the trial link can then satisfy its missing symbols
with generated stubs. `port_main.c` likewise reports a compile failure without
aborting or deleting a prior object, allowing a stale entry object to be linked.

Repro: inspect the port-source loop and `port_main.c` compile command in
`pc_port/build_port.sh`, or induce a temporary compile error in an isolated
worktree. The driver reaches the trial link instead of exiting at the failed
compile.
Evidence: proven (control-flow inspection)
Last verified @ fdd86e7

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

## Map015 entrance 0 — func_8008399C assertion

Repro: launch Map015 entrance 0; assertion fires, map not runnable
Evidence: observed (surfaced during ABR scene sweep)
Last verified @ ed61298

## LIBGTE.C invalid UTF-8 byte

Blocks patch-editor modification of the file. Maintenance item.
Repro: iconv -f utf-8 -t utf-8 < pc_port/extern/PsyCross/src/psx/LIBGTE.C > /dev/null
Evidence: proven
Last verified @ ed61298
