# Retail-exact character-change menu

The complete 26,624-byte `member_change_menu.bin` now matches the pinned retail
module. Root independently compared the entire generated file against
`disc/member_change_menu.bin`; both SHA-256 values are
`3b9e2b890c27ae0de97fe343ac75cd35c05fe7f9605ae387d2166da0be78109c`.

The final source is `src/member_change_menu/main/misc.c`, SHA-256
`15a4091854071fcb92c75236165cf836d4ce0a2dea0d47c822d7a0dd616a02cb`.
No compiler flags, assembly patches, symbol layouts or retail checksum pins
were changed for these repairs.

## Source changes and exact ranges

| Function | Retail address | Exact bytes | Change |
|---|---|---:|---|
| Label drawer `func_801C59E0` | `801C59E0` | 432 | Typed items/RECT copy, signed division, fresh work-pointer loads after each lookup, original loop ordering |
| Label initializer `func_801C5B90` | `801C5B90` | 92 | Use the same typed item array and work-buffer field as rendering/freeing |
| Name renderer `func_801C95A0` | `801C95A0` | 252 | Restore retail calculation order and unsigned shifts, including negative host inputs |
| `MemberChangeMenuSwapCharacters` | `801CAB48` | 460 | Replace the final assembly placeholder with exact C and shared game-state restriction flags |

The assembly placeholder had been hoisted to the start of the C object by
GCC 2.6. The exact C replacement and restoration of the label drawer's missing
32 bytes place every function back at its retail address. A final comparison
then exposed the name renderer's remaining scheduling/shift difference;
repairing it made the entire module exact.

The label work pointer is retail `g_Menu+0x558`, the first typed menu string's
`pVramBuffer`. The native structure expands for host pointers, so raw offsets
did not address the same fields used by rendering/freeing. Explicit lookup
temporaries also prevent host argument evaluation from reading the work
pointer before `GetStringEntry`, contrary to retail's order.

## Verification

The initial isolated `make check` completed all 472 build/link tasks and failed
four unchanged checksum gates. The final isolated run still completes all
472 tasks and exits 2, now with only three checksum failures: SLUS, field and
shop menu. The character-change menu checksum passes. Exact commands, source
pins and log hashes are in [full-module-matching.md](full-module-matching.md).
The dirty shared checkout was not used for global matching builds.

Native swap regression compiles the actual production translation unit and
tests the real flag helper against its retail instructions: 1,389 swap cases,
80 helper cases, O0/O2/Clang UBSan, all seven compiled controls rejected.
Root final full-source run: `/tmp/xeno-member-change-swap-retail-test-20260906/run.gEf8GJWa`.
Its source pins are retained in `swap-test-manifest.json`.
Only the helper definition is renamed for boundary tracing; the swap body is
unchanged. Native/guest menu layouts and unrelated bytes are checked.

Native label regression compares the actual production drawer against all 108
retail instructions: 240 cases per O0/O2/UBSan mode, including odd counts,
negative layout offsets and helper-side menu/work/table rebindings. It checks
widths, rectangles, untouched native bytes, call order, upload-time rectangle
values, and typed initializer storage/arguments. All five semantic controls
are rejected. Root run: `/tmp/xeno-member-labels-test.OdMRMi4A`.
UBSan uses GCC-instrumented production/fixture objects with the Clang runtime.

Native name-renderer regression passes 44 cases and 727 checks per
O0/O2/Clang UBSan mode, with all 63 retail instruction words executed.
It compares allocation/clear sizes, both game-state string pointers, render
flags/heights, actual retail U/V tables, upload-time rectangles, DrawSync/free
order, and complete work buffers. Four semantic controls are rejected. A
one-site reconstruction of the former signed slot shift fails UBSan on -256.
The durable runner generates this control from the current full translation
unit and does not require temporary historical source files. Root run:
`/tmp/xeno-member-change-name-retail-test-20260906/run.0mINLSTv`.
The earlier run `run.QChHQlx9` additionally rejected the preserved actual old
source, but that historical file is not a prerequisite for future tests.

The initializer check is native layout/boundary coverage; its exact PSX bytes
come from the whole-module comparison. Render/GPU calls are test boundaries,
so these tests do not prove menu pixels or a natural gameplay visit to this
menu. The opening controller has its own separate observed runtime report.

Reproduce the focused tests with:

```sh
bash pc_port/tests/run_member_change_swap_retail_test.sh
bash pc_port/tests/run_member_change_labels_retail_test.sh
bash pc_port/tests/run_member_change_name_retail_test.sh
```

The final native build passes, SHA-256
`1c9e2c73427c0ed20177b5db49cdd62ec488cc03b717269c68f57ac3b3e5b215`.
See `final-build-pins.json` and `final-test-pins.json`. This binary includes the
later name-renderer repair; the separate ordinary-opening observation used
preceding binary `f59f7f87...`. No repeat gameplay run of this final binary is
claimed. The verified recording audio correction remains present.
Full-game and forest completion remain unproven; this is one exact module
within the larger goal.
