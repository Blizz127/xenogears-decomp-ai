# Missing field matrix routine restored

Added the previously absent `func_800759E4` to `src/field/main/misc2.c` at its
retail function ordering point, plus its native seed vector `{0,0,4096,0}`.
The PSX build continues to reference its existing retail rodata at8006FB70.
The function forms two normalized cross products around the input axis and
writes the three matrix rows, preserving padding and translation.

The full production translation unit compiles the function to all292 exact
retail bytes at800759E4..80075B08, SHA-256
`b4f76d0b3dd362838d4eb82c2dfefab068628db24f11de43f7fe0504c8b04cf3`.
Root verified the entire extracted function and16-byte seed directly against
`disc/field.bin`, and verified installed source equals the frozen candidate.
The isolated link pins all three dependencies used by this body; unrelated
full-TU externs use placeholder addresses only to permit extracting the
function. This does not prove the other functions or whole field module.

Root reran actual full-production-TU native tests at O0/O2/all-Clang UBSan:
200cases,21078checks,all73 retail instruction slots,all four semantic controls
rejected. Controls alter seed, axis row, cross-product order and normalization.
The source, fixture, runner, adapter, retail and header pins remain stable.
The preserved source-before is missing the function/seed and fails the link.
Root run: `/tmp/xeno-field-axis-matrix.w8zti0wd`.

Helpers are explicit OuterProduct12/VectorNormal spies. The fixture compares
helper order, pointer roles, input vectors, helper-side axis changes, complete
matrix/axis state and padding/translation guards. It does not prove the SDK
math, a rotated actor, rendered pixels, or natural execution of this function.
The rotated-actor assertion is in the matching-only branch; native uses its
already implemented `FieldRenderActorSpriteTail`. No direct field caller of
this separate292-byte helper was found. Similar matrix work already exists
inline in `func_800764B4`; restoring the missing standalone body advances the
full decompilation and does not establish a new native actor capability.
See `caller-audit-correction.md`. No story/map/position flags were changed.

Run `bash pc_port/tests/run_field_axis_matrix_retail_test.sh`.

Private global makecheck completes all472 tasks and still fails only SLUS,
field and shop checksums. Field grows by precisely292 bytes (242330→242622);
member menu remains fullyexact, SLUS/shop unchanged from the preceding frame
repair check. `matching-after.json` retains current sizes/hashes. The field
inventory separates relocated tables, extra rodata, compacted C functions and
this absent body; replacing table targets with convenient literals would not
repair the underlying code. The early lighting candidate remains nonmatching.

The final native build passes; see `final-build-pins.json`. Actual execution
of this matrix routine remains unobserved. `prior-natural-route.md` records
the preceding15-minute run reaching field4 only; its window-focus driver bug
is being corrected before another natural traversal. Fullforest and fullgame
completion remain open.
