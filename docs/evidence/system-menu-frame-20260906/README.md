# Retail menu frame routine

`func_8001C074` now uses typed graphics environments and ordering tables,
selects the first environment whenever the current pointer is not the first,
and preserves the retail nested debug checks and fresh pointer loads after
helpers. The old code used PSX byte offsets inside enlarged host structures
and left the second environment selected on successive frames.

All 308 emitted bytes match retail at `8001C074..8001C1A8`. GCC2.6 compiles
the actual full translation unit; its emitted function assembly is linked at
the expected retail address with retail helper/global symbols. This proves
the function instructions, not the still-shifted whole executable layout.
See `exact-function.json` and scratch `/tmp/xeno-system-menu-retail-20260906`.

The header's debug-index byte is ordinary RAM again. Only the input routine
uses volatile lvalue accesses to preserve its existing retail load/store
schedule. No volatile-qualified object is accessed through a nonvolatile
lvalue. This qualifier change does not alter structure offsets. Separate
before/after emitted-instruction comparisons cover the full system, member
and shop menu translation units; only assembler comments/file names differ.

The actual native production function passes 300 cases and 6,020 checks in
O0, O2 and mixed GCC-instrumented/Clang-runtime UBSan modes. All 77 retail
instructions execute. Tests cover both graphics buffers, other/null entry
pointers, five render-context values, debug sentinels and helper-side menu/
debug pointer changes. The test replaces controller input and GPU/heap helpers
with spies, compares their order/arguments and final normalized state, and
checks unrelated native menu bytes. It does not prove GPU pixels or controller
input behavior. Exact emitted-code comparisons cover input qualifier effects.

Four compiled controls fail semantically: stuck buffer, wrong OT entry,
wrong page toggle, and missing debug sentinel recheck. The actual old source
fails on its first invalid native pointer. Final root run:
`/tmp/xeno-system-menu-frame.ZlpYY7iG`; old source:
`/tmp/xeno-system-menu-frame.GYO2KEkg`. Reviewer independently reran the suite.

Run `bash pc_port/tests/run_system_menu_frame_retail_test.sh`.

Isolated global matching completes all 472 tasks and still fails only SLUS,
field and shop checksums. Member-change menu remains fully byte-exact and
field/shop artifacts remain unchanged. SLUS gains the four missing bytes in
this function and remains nonmatching. See `matching-after.json`.

Final native build passes, SHA-256 `4ab179081703f3808e21a4414ce49dab23944d6b54808e6ab328c2daa22bbcb6`.
Source/log pins are in `final-build-pins.json`. The ongoing natural story
replay uses preceding copied binary `1c9e2c73...`; it cannot establish execution
of this newly repaired menu routine. This repair is instruction-, test- and
build-verified; actual menu-frame runtime and GPU pixels remain unobserved.

## Review resolution — 2026-09-06

The qualifier review is resolved. The final full-production-TU comparison
shows the adjusted `unk1E95` declaration and local volatile input accesses emit
the same 308 retail bytes; normalized emitted `menu.c` assembly is unchanged
apart from assembler comments/file names. The corresponding shop and member
menu instruction streams are also unchanged. The isolated global 472-task
result retains the exact member-change-menu artifact and unchanged shop hash;
see `/tmp/xeno-system-menu-retail-20260906/global-after-frame.json` and
`check-header.sh`.

The frame fixture remains deliberately scoped after controller input. Its
300-case/77-instruction result does not constitute runtime coverage of the
L1/L2 `unk1E95` decrement/increment path; that remains a separate input-routine
concern. The new runner is executable for direct invocation as well as
`bash pc_port/tests/run_system_menu_frame_retail_test.sh`.
