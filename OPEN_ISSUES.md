# Open Issues

Rules:

- If it's in this file, it's OPEN. Closing means DELETING the entry, not annotating it.
- Every item carries a reproducer command, not just a description.
- Every item carries `Last verified @ <commit>`. Stale verification = suspect claim.
- Every claim is tagged by evidence class:
  proven | build-verified only | observed | inferred

---

## Port build silently omits failed game translation units

`pc_port/build_port.sh` suppresses each game-TU compiler stderr and treats a
failure as `skipped`; the final link then permits stale auto-stubs to stand in
for definitions that would otherwise have come from that TU. This can produce a
linking, runnable binary with real game code absent.

Current normal-build inventory:

- `src/member_change_menu/main/misc.c`: PsyCross `setRGB0` is used on `P_TAG`,
  which has no `r0/g0/b0` fields.
- `src/shop_menu/main/misc.c`: same `P_TAG`/`setRGB0` type mismatch.
- `src/slus_006.64/system/sound.c`: conflicting implicit declarations and
  invalid post-increment lvalues.
- `src/slus_006.64/system/work_list.c`: `uintptr_t` is unavailable in the
  current header setup.

Current field-test trial-link impact:

- menu dispatcher entries `func_801C62A8`, `func_801CB0A8`, `func_801CBDBC`,
  `func_801CCD28`, and `func_801CE024` are unresolved/stubbed;
- sound functions `SoundEnableAllSpuChannels`, `SoundMuteAllSpuChannels`,
  `SoundLoadWdsFile`, `SoundFreeWdsEntry`, and `SoundHandleError`, plus five
  sound globals, are unresolved/stubbed;
- `WorkListsFreeAllEntries` and the work-list globals are unresolved/stubbed.
  `pc_port/src/work_list_port.c` does provide the live update/add/remove paths,
  so this is partial replacement rather than total loss of work-list behavior.

Repro: `./scratchpad/run_build_port.sh` prints `compiled=43 skipped=4`; compile
each listed file with the build driver's GFLAGS/INC to see its suppressed error.
Evidence: proven (build-driver control flow and current compiler output)
Last verified @ 3c90cd9

## Port stub generation cannot converge without matching ELFs

When `build/out/*.elf` is absent, `pc_port/build_port.sh` reuses the existing
`pc_port/build_native/stubs.c`. It only prunes function stubs now supplied by
compiled objects; it does not generate stubs for the current `undef.txt` and
does not iterate trial-link generation. Re-running an unresolved configuration
therefore cannot converge.

Repro: build with a configuration that changes undefined symbols while matching
ELFs are absent; the driver prints `reusing existing stubs.c`, then final-link
errors repeat on every run.
Evidence: proven (build_port.sh trial/stub control flow; diagnostic-build test)
Last verified @ 3c90cd9

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
