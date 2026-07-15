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

## Sound cold-init is blocked below the decomp by hollow PsyCross SDK primitives

`src/slus_006.64/system/sound.c` is linked and all ten audited sound layouts are
retail-correct, but the port's field-test boot bypasses retail's
`SoundInitialize(0)` call. Consequently `D_800595D8`, the packed 32-bit current
audio-manager address, remains zero while field code calls the sound API.
Replacing the live oracle stubs with their exact-matched bodies therefore faults
on the first manager-element access. Retail has no null guard; adding one would
hide the missing initialization rather than restore it.

The current gate is below `SoundInitialize`: several PsyCross SDK symbols on
the successful cold-init path link cleanly but are implemented only as
`PSYX_UNIMPLEMENTED()` no-ops. **Symbol-resolved does not mean implemented.**
In particular, the earlier dependency classification called
`SpuSetReverbModeType` "real" because it was not in the generated stub manifest;
that was wrong. Its PsyCross body is a hollow implementation.

Cold-init reaches these hollow SDK primitives without taking an error path:

- `OpenEvent` (`pc_port/extern/PsyCross/src/psx/LIBAPI.C:140`) returns zero and
  retains no callback. Retail uses it at `SoundInitialize` `0x80037C84` to
  register `func_8003C020`.
- `EnableEvent` and `DisableEvent` (`LIBAPI.C:152-161`) are no-ops used by sound
  heap allocation and audio-manager list insertion.
- `SpuSetIRQCallback` (`LIBSPU.C:356`) and `SpuSetIRQ` (`LIBSPU.C:344`) are
  unconditional at `0x80037CCC` and `0x80037CD4`.
- `SpuSetCommonAttr` (`LIBSPU.C:191`) is reached synchronously through the
  cold-init `SoundSetCdAttr` call and later by the timer callback.
- `SpuSetReverbModeDepth` (`LIBSPU.C:236`) is unconditional in
  `func_800386C4`, and `SpuSetReverbModeType` (`LIBSPU.C:230`) is reached by
  the normal first reverb-allocation branch. Both are no-ops.

There is also a missing infrastructure layer rather than a single stub:
PsyCross's `SetRCnt`/`StartRCnt` record counter state, but there is no event pump
that invokes the counter-2 callback. Fixing `OpenEvent` alone would therefore
allow registration while `func_8003C020` still never fires.

This also blocks the three already exact-matched PsyQ leaves
`SpuGetReverbModeType`, `SpuSetReverbModeDelayTime`, and
`SpuSetReverbModeFeedback`: their retail behavior depends on reverb state that
the hollow type/depth functions never maintain. Exposing those three symbols
without the state layer would create a symbol-complete but behaviorally partial
initialization path—the same partial-init trap in a quieter form.

Retail calls `SoundInitialize(0)` from `func_80019578` at
`0x80019668-0x8001966C`. `SoundInitialize` spans
`0x80037B88-0x80037DBC` and stores `func_8003B148(0x10)` into
`D_800595D8` at `0x80037D50-0x80037D68`. Its complete known dependency
tree has four legs:

- **Manager allocation:** `func_8003B148` calls `func_8003B32C` at
  `0x8003B194`; that calls real `SoundInitializeAudioManager` at
  `0x8003B358`; the lower chain `SoundHeapFree`, `func_8003B930`, and
  `func_8003B32C` is now exact-matched. `func_8003B148` remains unported. Its
  allocation-failure exit also calls
  stubbed `SoundHandleError` at `0x8003B184`. That handler is not a leaf: when
  control flags `0x88` are clear, retail `0x8003F6E0-0x8003F724` reaches
  stubbed `SoundLoadWdsFile` and `func_80039E60` in addition to real
  `SoundSpuMemoryFreeBlock`, `SoundAddSedsEntry`, and `func_8003BDFC`.
- **CD mix:** `SoundInitialize` calls unported `func_800386C4` at
  `0x80037D04`; that path reaches unported `SoundSetupCdMix`.
- **Reverb:** real `SoundSetReverbModeWithAllocation` still reaches generated
  stubs for `SpuGetReverbModeType`, `SpuSetReverbModeDelayTime`, and
  `SpuSetReverbModeFeedback`; its symbol-resolved `SpuSetReverbModeType` and
  `SpuSetReverbModeDepth` dependencies are PsyCross no-ops, not real backends.
- **Timer tick:** `SoundInitialize` registers unported `func_8003C020` as the
  recurring callback at `0x80037C7C-0x80037C84`. Its sound-tick graph reaches
  at least `func_8003E900`, `func_8003AE84`, `func_8003A838`,
  `func_8003EBF0`, and `func_8003EB5C`, all still unported.

`SoundSpuMemoryAllocateBlockAtAddress` (`0x800395B8-0x800396DC`) is an
independently portable leaf: its only call is the real
`SoundSpuMemoryGetFreeBlock` at `0x80039678`.

The corrected sequencing is:

1. implement and behaviorally validate the PsyCross SDK integration layer
   (coherent reverb state, `SpuSetCommonAttr`, IRQ state/callbacks, and actual
   event/timer delivery);
2. exact-match the ten cold-init decomp functions;
3. route `SoundInitialize(0)` only as a diagnostic proof, verifying manager
   allocation, timer delivery, reverb-transfer completion, and zero stub hits;
4. then restore and extend the live playback path.

The SDK work has no objdiff oracle; validate it as host integration, like
`game_overrides.c` and the host walkers, using state/behavioral probes. Do not
resume the ten-function decomp or route boot on top of hollow primitives.
Earlier routing would construct a partially initialized manager and register a
non-delivered tick callback, turning a loud null dereference into quiet wrong
state. Four live playback helpers
(`func_8003A20C`, `func_8003A344`, `func_8003A450`, and `func_8003A55C`)
already have objdiff-`{}` C transcriptions, but they must remain oracle-stubbed
until initialization is complete.

Repro: `sed -n '140,245p' pc_port/extern/PsyCross/src/psx/LIBSPU.C` and
`sed -n '135,165p' pc_port/extern/PsyCross/src/psx/LIBAPI.C` show the
`PSYX_UNIMPLEMENTED()` happy-path bodies. `rg 'func_8003C020' pc_port/extern/PsyCross/src`
finds no counter/event delivery path. The original runtime repro remains:
locally replace `func_8003A20C` with its exact-matched C body, rebuild, and run
`timeout 20s ./scratchpad/run_map001.sh`; GDB faults with `D_800595D8 == 0`.
Restore the oracle stub afterward.
Evidence: proven
Last verified @ 609c426

## Map143 dialogue path crashes in the shared tile/sprite renderer

Map143 has legal entrances `{0, 1}`. From entrance 0, real d-pad input can move
the player to shop actor 14 and a real Circle/talk input selects that actor's
authored talk routine. The routine opens dialogue, then remains at
`WAIT_DIALOG` through at least frame 500 even after authentic close inputs. The
unmodified renderer SIGSEGVs at frame 358, before the script reaches its shop
opcode and before any menu entry point or menu oracle stub executes.

The fault stack is:

```text
GR_UpdateVRAM
AddSplit
BeginTexturedSplit
ProcessTileAndSprt
ParsePrimitive
DrawOTag
func_8007554C
FieldMain
```

The frame-340 pre-fault capture at `/tmp/menu143_prefault.png` is a coherent
field scene with the dialogue box still open; it is not a menu frame. This is
therefore an independent field renderer/dialogue-path defect, not a menu
failure. `GR_UpdateVRAM` and the tile/sprite processing path are shared, so the
same mechanism may affect other maps and needs a separate diagnostic pass.

Repro: build the normal port, then run
`env XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 XENO_FIELD_MAP=143
XENO_FIELD_ENTRANCE=0 SDL_VIDEODRIVER=x11 DISPLAY=:0
pc_port/build_native/xeno-port`; use the d-pad to move from the entrance to
actor 14 and press Z (the port's Circle/talk mapping). The authored interaction
opens the dialogue and reaches `WAIT_DIALOG`; capture under GDB to obtain the
stack above. Entrance 1 is structurally legal as well, but entrance 0 is the
verified reproducer.
Evidence: observed (authentic input and authored actor script; fault occurs
before menu dispatch)
Last verified @ e3f4b49

## Field-test cold boot does not reach surveyed authored shop branches

Menu routing and authentic keyboard input are now working, but the field-test
harness's default game state does not satisfy the authored conditions leading
to any shop opcode found in the surveyed maps. Extended field-script opcode
`0x58` is the authentic shop trigger: `func_800799D4` performs the overlay,
party-state, and render-buffer setup before `MenuMain` dispatches to the real
`ShopMenuMain` body. Do not bypass that setup by direct-calling the menu, and do
not force an individual script PC or branch merely to produce a menu frame.

Map292 is the clearest live reproducer. Its only legal entrance is 0, actor 15
routine 2 contains an authored `0x58` at script PC `0x3BF`, and real movement
plus Circle/talk input repeatedly selects `FieldScriptGetBytecodeOffset(15, 2)`.
The actor nevertheless remains in its idle `0x7FFF` slot and never reaches
`func_80093824`, `MenuMain`, or `ShopMenuMain`. No story flags or script state
were forced. The same outcome was observed at statically identified shop sites
on Maps 209, 593, 301, 282, and 52 across their tested legal entrances: each
surveyed `0x58` is behind authored state-dependent control flow that cold boot
does not take. None of the 15 inner menu oracle stubs fired.

This is a harness/state reachability gap, not an input or symbol-routing defect.
The next bounded diagnostic is a whole-archive reachability scan from real
routine entries, classifying every `0x58` site as cold-reachable or gated by a
state-dependent branch. If an ungated shop exists, use its legal entrance as
the authentic live repro. If every site is gated, reaching a menu requires
either reconstructed playthrough state or an explicit, documented menu-test
state scaffold—not an ad hoc flag poke.

Repro: build the normal port, then run
`env XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 XENO_FIELD_MAP=292
XENO_FIELD_ENTRANCE=0 SDL_VIDEODRIVER=x11 DISPLAY=:0
pc_port/build_native/xeno-port` under GDB with symbol breakpoints on
`FieldScriptGetBytecodeOffset`, `func_80093824`, `MenuMain`, and
`ShopMenuMain`; move to actor 15 with real d-pad input and press Z (Circle).
The `(15, 2)` routine selection fires, while all three downstream menu-path
breakpoints remain silent.
Evidence: observed (authentic input/routine selection and surveyed authored
scripts) + inferred (the missing prerequisite is cold-boot game state)
Last verified @ 5b68551

## Unported member-change and shop menu overlays

Both menu `misc.c` TUs now compile after the retail-proven `POLY_FT4` window
border correction and the `ShopMenuBuyMenu` declaration fix. Their compile
errors had been masking 15 link-reachable `INCLUDE_ASM` dependencies: three in
the member-change overlay and twelve in the shop overlay. Matching ELFs now
exist and there is no port-symbol collision. Both TUs now compile and link
normally with generated oracle stubs. Port-only routing reaches the real menu
entry bodies; live coverage of the 15 inner stubs now awaits an authentic shop
state as tracked above.

Repro: compile both menu TUs with the port GFLAGS/INC and compare their undefined
symbols against their `INCLUDE_ASM` declarations, or remove the holds locally
and verify that the typed stub manifest regenerates and links.
Evidence: proven
Last verified @ 5b68551

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
