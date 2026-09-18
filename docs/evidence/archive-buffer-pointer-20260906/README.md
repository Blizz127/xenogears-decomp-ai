# Archive destination pointer repair — 2026-09-06

`ArchiveReadFileToBuffer` accepted its destination as `s32`, while native
callers pass `void*`. A real high-address mapping reproduced truncation in the
actual production function. Changing only that parameter to `void*` preserves
the complete native address and the existing retail call order.

The original source fails at the first valid read with
`native destination pointer truncated` in
`/tmp/xeno-archive-buffer.YycCmX7T`. The corrected full translation unit passes
seven focused cases at O0, O2 and mixed GCC/Clang UBSan; recreating the old
signature in that full source is rejected by the durable runner. Cases cover
invalid index/size/null destination, high pointers, unchanged invalid-request
state, return propagation, full-width channel/flags, and the offset reload
after CD sync. Final output: `/tmp/xeno-archive-buffer.netSf8oA`.

Decode, CD sync and read helpers are explicit spies. The runner weakens only
those helper symbols in the compiled object, keeping the actual wrapper.
Inlining and GCC interprocedural register assumptions are disabled so those
substituted boundaries use the ordinary ABI. This does not validate actual
CD I/O or decompression. A fixture setup attempt using linker wrapping did
not replace same-translation-unit helpers; that attempt was not a product
failure. The reproduced truncation above is from the corrected test setup.

The entire GCC 2.6 archive translation unit emits identical assembly before
and after, apart from its input filename. Private `make check` completes 472
tasks and retains exactly the preceding bytes of all four checked modules:
SLUS, field and shop still fail their checksums, while member menu is exact.
See `matching-after.json`. This change introduces no matching regression;
it is not a new claim of whole-game parity.

The ongoing natural gameplay process retains its preceding copied binary.
High-address archive reads in that process have not been observed. Native
build evidence is recorded separately in `final-build-pins.json` once linked.
The shop loader's isolated tests also use an archive-read spy and therefore
do not independently prove this corrected dependency.

Scratch evidence: `/tmp/xeno-archive-buffer-pointer-20260906`.
No staging, commit or push.
