# Text subcodes 6, 7 and 8: explicit retail table loads

The text interpreter now loads the three archive pointer slots directly at
`g_SystemDataEntries + 5C`, `+60` and `+64`. The retail loads are at
`80034304`, `8003432C` and `80034354`, respectively. The prior local three-byte
offset array caused GCC's matching build to emit a new `.sdata` object absent
from the retail layout. Its relocations targeted a discarded section.

Each case preserves the script advance, nested string push and continuation.
The linker layout has not been expanded to accommodate the invented table.
The separate matching-only `INCLUDE_ASM_USE_MACRO_INC=1` flag in `gears.toml`
aligns embedded assembly jump-label visibility with standalone assembly.
The isolated global matching result is reported separately.

Root independently ran the existing textbox timing runner with unique build
and scratch directories. Output:
`pc_port/build_native/textbox-table-root-bwe1nlij`.
O0 and O2 pass, including three distinct tables/entries, nested glyph
selection, saved return PC and return restoration.

The standard runner exits 1 while linking UBSan because the machine lacks
`/usr/lib64/libubsan.so.1.0.0`. Root linked those already instrumented GCC
objects with Clang's UBSan runtime, ran the resulting executable successfully,
and verified identical O0/O2/UBSan output and empty stderr in every mode.
`verification.json` and `runner.log` preserve this distinction. The runner
and global toolchain installation were not modified.

The native build with this correction and A9 links successfully; see
`animation-a9-integration-20260906.md` and the accompanying build pins.
This source-level repair does not establish that the whole text interpreter
or full executable matches retail bytes.
