# File-1 controller: exact C reconstruction installed

The controller at `801E6CE8..801E71D4` now compiles to all1,260 retail bytes
with the original GCC2.6/PSYQ pipeline. Root independently rebuilt the entire
19,516-byte archive0x20/file1 module with this C function and assembly for the
other functions: the complete result equals the pinned retail payload.

The PSX wrapper now compiles the C implementation by default. The native
build still leaves it inert under SKIP_ASM; the port continues interpreting
this controller until loaded-module identity, packed-RAM bindings and the
retail seven-argument interface are adapted. This is one exact C function,
not a claim that all functions in the module are decompiled or natively used.

## Independent checks

- `python3 tools/scripts/check_battle_command_file1.py --with-c`: exit0,
  full ASM reconstruction PASS, C build/entry/size PASS, C function bytes PASS,
  and C-containing full module PASS. Output `/tmp/xeno-file1-build-check-fsrl8h4c`.
- C entry `801E6CE8`, size1,260, SHA256
  `1f11c9ac7117c0d702c6829cb3fb57806d73c6413eaec5f9fbf1ecca2b427a2e`.
- Full-module SHA256
  `64668d85bf48dea46cf6d5f04b38e38ca887877cfef53961f0c26cd2d363b670`.
- `python3 pc_port/tests/run_battle_command_file1_controller_retail_test.py`:
  exit0;463 cases at O0/O2/ClangUBSan, three yields, all315 instruction slots
  visited and all21 branch sites exercised both ways; all11 semantic controls
  compile and are rejected. Root output `/tmp/xeno-file1-controller-retail-spl9ppge`.

The copied module/differential manifests and installed-source pins are next
to this report. Retail payloads and generated binaries remain in ignored or
external local outputs, outside the source artifacts.

## What resolved the last13 words

The control record now has a partial typed view with measured halfword fields
at7F6/7F8/7FA/7FC/7FE and phase byte802. The opaque prefix does not claim a full
record decompilation or allocation size. Ordinary typed accesses preserve
GCC2.6's field-load scheduling; no volatile qualifiers, handwritten assembly,
binary patches or changed compiler flags were required. The existing separate
GetStringEntry full expression still preserves the following global reload.

Astra's exact-matching report, typed-layout assertions and compiler RTL evidence
are recorded in `file1-controller-exact-c-20260906.md` and its source pins.
The installed implementation SHA256 is
`a1dfab198ebc494cb3db6f41da124d55c442ae3c49bee543ea2ebbbbf89c2f45`.
Only the constructor-row negative-control text anchor changed in the runner.

Earlier302/315-word reports are historical and superseded for this controller.
Global game matching, other module functions, native adoption, the route through
the forest and full-game retail parity remain incomplete.
