# Full decompile and PC-port scope checkpoint — 2026-09-05

Current continuation (2026-09-06): the opening now has captured Fei dialogue
and a direct return to field 14. The file-1 dialogue controller compiles
byte-for-byte to retail; its native substitution remains disabled. The later
battle's A9 animation command passes production and outer-interpreter tests,
with fresh gameplay observation separate. Native build `f4dcc43c...` links
with 79 function stubs and 566 data symbols. The isolated matching build now
links all artifacts but fails the four existing retail checksum gates.
See [the current opening evidence](../opening-battle-encounter-20260905/README.md)
and the active handoff for source pins, live ownership and newer work.
The dated inventory below is historical; full-game acceptance remains open.

Objective: **decompile, build and port the game to completion with retail
accuracy**. The opening Gear scene remains an immediate integration target,
not a replacement for this full objective. No completion percentage is
supported by this checkpoint.

Current branch: `experiment/worldmap-open-gates-20260823`; HEAD:
`3a3e7aac03a2f166fb924945a489e392d706f282`. This is a large dirty tree;
HEAD alone does not identify the current implementation. No commit/push,
reset, or unrelated cleanup was performed.

## Distinct completion requirements

| Requirement | Evidence needed | Current status |
| --- | --- | --- |
| Decompilation | Complete scoped symbol/data coverage, retail instruction/data comparisons, documented intentional hardware adapters | Incomplete; assembly fallbacks remain |
| Native PC integration | Actual linked owners and correct call/data boundaries, no unresolved required stubs or invented fallback behavior | Incomplete; generated and authored stub frontiers remain |
| Retail behavior | Differential tests covering relevant state, branches and side effects; source provenance | Partial; passing bounded tests are not exhaustive fidelity |
| Visible gameplay | Natural boot, menus, opening, fields, world, battles and later progression observed against retail | Incomplete; opening Gear scene not accepted |
| Audio/video and persistence | Correct movie/presentation/audio and save/load state verified in runtime | Not accepted globally |
| Reproducibility | Build/test inputs pinned; clean reproducible verification without disc payload publication | Current native build succeeds; fresh full matching report absent |

Interpreting retail code is not the same as decompiling it to C. Likewise,
a C definition is not proof it is linked, and a linked implementation is
not proof of correct gameplay. Keep these labels separate throughout.

## Current native artifact

`pc_port/build_native/xeno-port` SHA-256:
`a772cd10e27ecfdf6bc0df7e898b9d38ca984262c453756f3a4e24eec26c4cc6`.
Build log: `pc_port/build_native/battle-sprite-reentry-port-build.log`.
Generated manifest: **85 function stubs and 575 data symbols**, SHA-256
`6f28e2aea1b79347334f216a53e7f7420a09eed57f464d5a58b7414f940e1b5a`.
These are link-frontier counts, not the total remaining decompilation work.
Generated data storage needs ownership/size/initialization review; its mere
presence does not prove the data is wrong or reached during ordinary play.

The visible debugger/game run at `scratchpad/opening-sprite-reentry.hwQNVyd6/`
is alive at this checkpoint. The prior successful build is progress, not
scene acceptance. Its C11CC callback executes actual loaded retail bytes;
native decompilation of C11CC remains unfinished.

## Source fallback census

Lexical `INCLUDE_ASM(` occurrences at line starts in `src/**/*.c`:

| Source area | Occurrences |
| --- | ---: |
| battle | 847 |
| battling | 169 |
| menu | 139 |
| slus_006.64 | 102 |
| field | 20 |
| shop_menu | 16 |
| member_change_menu | 1 |
| Total | 1294 |

This deliberately **is not** a count of missing native functions. It includes
conditional matching fallbacks with separately implemented native paths,
SDK replacements, and reference-only overlays. A semantic owner map plus
fresh matching output is needed before deriving a progress percentage.
`build/progress.json` is absent. The existing progress dashboard consumes
that report; generating dashboard HTML cannot establish missing evidence.

## Fresh field-dispatch and runtime-frontier audit

Executed the existing auditor against the current tree and native objects:

```sh
python3 pc_port/tools/audit_field_script_vm.py \
  --json pc_port/build_native/field-audit-current-20260905.json \
  --csv pc_port/build_native/field-audit-current-20260905.csv
```

JSON SHA-256: `45f7e9cb94a3c6c9886096278455605581e5797b4648956e56c3b7cdd97a5dd8`.
CSV SHA-256: `5cf7ce68ea304fb869ab9cfd53b6e3fdef28b72efcb9f6a41af3f6424c956dd5`.
Summary log: `pc_port/build_native/field-audit-current-20260905.log`.

- 483 dispatch slots, 481 unique handlers, **zero table mismatches**.
- No handler itself is a generated/authored stub, but **three handlers
  directly depend on generated no-ops**:
  - FE1F: `func_8009FDD4` → `func_800AD4D4`.
  - FE20: `func_8009FE4C` → `func_800ACFD0`.
  - FEB1: `func_80087FD4` → `func_800A8BA4`.
- Across compiled field code: 19 callers, 25 direct generated-stub edges,
  17 distinct targets. One direct authored-stub edge is
  `func_80079288` → `func_80281204`.
- Four native-may-compile `assert(0)` sites remain: `func_800748E8`, two in
  `func_80075B44`, and one in `func_800A84C0`. Reachability/retail ownership
  must be investigated; do not simply remove the assertions.
- Generated-data frontier: 694 callers, 1954 edges, 289 targets, including
  276 default-32-byte storage classifications. These are conservative static
  review targets, not 1954 independently proven runtime defects.
- Matching-object comparison was **not run for all 481 handlers**, because
  no fresh isolated matching root was supplied. Nine native overrides are
  classified unproven by this audit; other bounded tests may cover some
  behavior, but this run does not establish full matching parity.
- Auditor summary: **runtime FAIL; retail FAIL**. Default CLI success only
  means the dispatch check passed; it must not be reported as a parity pass.

The older 2026-09-04 audit has matching evidence for its own pinned inputs.
It must not silently stand in for fresh matching of this dirty tree.

## Work order and acceptance boundaries

1. Observe the current opening build through the scripted Gear scene; keep
   unresolved rendering/animation boundaries explicit. Do not force HUD,
   script completion, actor state or scene flags to manufacture acceptance.
2. Close observed dependencies with retail-backed C or transparent existing
   hardware/execution boundaries. Finish native battle-animation dependency
   closure separately from the temporary retail execution integration.
3. Advance the full source/owner/matching inventory and close menu, field,
   world, battle, shop/member-change and remaining system gaps. The field
   frontier above provides concrete review targets, not automatic permission
   to patch by guessing.
4. Require fresh matching evidence and runtime/platform acceptance for each
   subsystem. Maintain audio, video, persistence and later-game coverage;
   do not redefine completion as a working opening or zero generated stubs.

This checkpoint is an evidence inventory and work map, not an exhaustive
audit of every script or a certification that currently written code is
free of invented behavior. No full-goal acceptance is claimed.

## First follow-up: FE1F/FE20 gear-transition call arguments

The retail callers preserve a party-slot value in a0 before calling
`800AD4D4` or `800ACFD0`. The native `misc6.c` call sites supplied no
arguments, despite another VM path already declaring the correct signatures.
The test reproduced actor index **100** arriving where slot **0** was required.
The public field header now declares the slot contract, and the two native
call sites pass the lookup result or script slot respectively.

Authority: `field.bin`, `[8009FDD4,8009FEE4)`, SHA-256
`6af0be6042c29d442177bf2e59950fd726a6bb92769e4f59f8ff7ced6bcbd0b5`;
pinned by `pc_port/tests/run_field_gear_dispatch_test.sh`. Its production-TU
test checks all three valid slots, riding/non-riding, present/absent party
members, no matching actor, and instruction-pointer advancement/wrap.
It is a native boundary test with recording callees, not a differential
execution test of the transition implementations. O0/O2/UBSan pass, and
wrong-actor-index/fixed-slot mutants are rejected. Log:
`pc_port/build_native/field-gear-dispatch-final.log`.

Initial test compile issues (script-data declaration and assert header)
were resolved before the behavioral RED above. No production assertion
was removed. `func_800AD4D4` and `func_800ACFD0` are still unimplemented;
the static dependency frontier is therefore **not closed**. This source
repair has not yet been rebuilt into the running game, so the preceding
audit/artifact hashes continue to describe their earlier snapshot only.

## Gear boarding implementation

Implemented native `func_800AD4D4` in the existing native late-field section
of `src/field/scripts/virtual_machine.c`. Exact retail function boundary is
`[800AD4D4,800AD898)`, not `800AD978`: `800AD898` begins another routine.
The `0x3C4` byte slice hashes to
`98fa2e22aeb3ccae9c04d4661febd4dd771e108639b87b8f5a2db1e205bf5f0f`.

The implementation performs the retail sprite-pointer swap, copies the
gear actor position into its newly owned sprite, updates the actor/status
and game-state ride bits, clears the paired actor flags, synchronizes both
animation fields, then issues the two animation calls, particle-stop call
and party-state update in retail order. Actor lookup is repeated after the
first animation call rather than assuming callbacks preserve the table.

`run_field_gear_board_retail_test.sh` compiles the actual production TU and
executes the original field routine through the MIPS test adapter. Eighteen
cases cover three slots and six current actor indices, including aliases.
It compares the complete fixture's actor/data/sprite/game-state bytes and
all four ordered callback targets/arguments. The three dependency callees
are recording boundaries in this test; their real implementations and
effects are not validated by this comparison. RED failed to link because
the native owner was missing. O0/O2/UBSan pass.

Position, flag and particle-mode mutants are rejected. The first flags
mutant survived because the fixture pre-set the required bit; clearing
that bit in the initial fixture repaired this coverage hole before final
verification. Caller-dispatch regressions also pass. Logs:
`pc_port/build_native/field-gear-board-final.log` and
`pc_port/build_native/field-gear-dispatch-after-board.log`.

This is a native behavioral transcription, not a PSYQ byte-matching claim.
No complete-game rebuild or live gear-boarding acceptance yet. Disembarking
(`func_800ACFD0`) remains unfinished, and the running executable still has
the older stub manifest. Do not subtract this source change from the
recorded built-artifact count until rebuilding and verifying the link.

## Gear disembarking implementation

Implemented native `func_800ACFD0` alongside boarding. The retail slice is
`field.bin` `[800ACFD0,800AD4D4)`, length `0x504`, SHA-256
`4a0c2c56dd0051c0c7db399d66b66c82e304869bd3b20058dfa349b96c953ac5`.
The return and delay slot are at `800AD4CC/800AD4D0`.

This is not a symmetric reversal of boarding. Retail selects the party
actor from `D_8006F990[slot]`, clears the ride byte first, swaps sprite
ownership, updates status/flags, and calls `800A0524(party,gear)` before
refreshing gear sprite position. It copies actor-data halfwords `+108`
and `+106` from gear to party, synchronizes `+E8` from `+E6`, explicitly
selects animation **6** for the party actor, then the gear's signed `+E6`
animation. Finally `8009FEE4(slot)` precedes `800A98E8(party,0)`; boarding
uses the opposite order for these last two calls. Actor lookup is repeated
after callbacks. No substitute state or invented fallback is introduced.

The existing production-TU retail differential harness now also supports
`bash pc_port/tests/run_field_gear_board_retail_test.sh --exit`.
RED failed to link the missing native owner. GREEN compares complete
actor/data/sprite/game-state fixtures and five ordered dependency calls
for all three slots and six current actor indices. These 18 cases confirm
that disembarking does not accidentally use the current script actor.
Distinct `+106/+108` values expose wrong-source copies. O0, O2 and UBSan
pass. Position, flag, particle-mode, animation and copied-angle mutants
are all rejected. Boarding and FE1F/FE20 caller regressions also pass.

Logs: `pc_port/build_native/field-gear-exit-red.log`,
`field-gear-exit-final.log`, `field-gear-board-after-exit.log`, and
`field-gear-dispatch-after-exit.log` in the same directory.

Limitations: the four dependency callees are recording boundaries, not
real side-effect implementations, in this test. Callback-induced actor
table mutation, full dependency integration, PSYQ byte matching, and
in-game disembarking are not proven. The entire game was not rebuilt in
this increment: live PID 261416 was preserved, and the executable still
hashes to `a772cd10e27ecfdf6bc0df7e898b9d38ca984262c453756f3a4e24eec26c4cc6`.
Earlier missing-owner statements describe earlier snapshots; both gear
transition owners now exist in source, but the recorded built stub count
and audit remain unchanged until the next verified build.

## Gear-transition build and refreshed audit

The subsequent complete native build passed (`LINK OK`, port-owned
addresses verified). Both `func_800ACFD0` and `func_800AD4D4` are strong
text symbols in the executable at `0049480F` and `00494B34` respectively.
Generated function stubs decreased from 85 to **83**; data symbols remain
**575**. This is a build-frontier change, not a completion percentage.

- Build log: `pc_port/build_native/gear-transitions-port-build.log`.
- Executable SHA-256:
  `2704189685cb34d6164dde4affad6781b0f1dcf3d2ee3c70f6d3251a19c0678b`.
- Stub manifest SHA-256:
  `b0e47f6873108480eef73a4b4f9abe4adfea22d5428cda09c88ec272dd5591ff`.
- Audit JSON: `pc_port/build_native/field-audit-after-gear-20260905.json`,
  SHA-256 `0e4f395bccc35fb94be6f5e317721f085dc2705517d8fc4afe706aa5ebd20920`.
- Audit CSV in the same directory/basename, SHA-256
  `06df0db7e0d0051ecef55638ab6fb302bedda1b30dd23118d2e0216f0718376a`.

Fresh audit: 483 slots, 481 unique handlers, zero table mismatches;
one directly dispatched generated-stub dependency remains
(`FEB1/func_80087FD4 -> func_800A8BA4`). Field-wide direct generated
dependencies: 16 callers, 21 edges, 15 targets. Four native assert-zero
sites and one authored-stub edge remain. Data frontier: 696 callers,
1962 edges, 289 targets, 276 default32 placeholders. Newly compiled
owners expose additional real data references; their presence must not
be concealed to improve the report. Matching is **NOT_RUN for 481**;
runtime and retail gates both remain **FAIL**.

The previous executable was preserved as
`pc_port/build_native/opening-sprite-reentry-hwQNVyd6-xeno-port`, hash
`a772cd10e27ecfdf6bc0df7e898b9d38ca984262c453756f3a4e24eec26c4cc6`,
before stopping owned debugger/game PIDs 261406/261416. The new visible
run is `scratchpad/opening-gear-transitions.lKzHOrjQ/`, debugger PID
301191, game PID 301201, interactive session 62236. It uses the existing
read-only packet-trace debugger script and field-input replay schedule;
no title-menu input or game-state override was added. Capture frame 420
was inspected (lossless BMP-to-PNG conversion) and shows the Squaresoft
logo. That proves this boot frame renders, not movie/audio parity or the
opening Gear scene. Manual New Game selection and actual gear-transition
acceptance remain outstanding.

The workspace-wide whitespace check flags pre-existing patch formatting;
no unrelated patches were changed during this increment.

## FEB1 overlay initializer prerequisites

Full retail trace identifies `func_800A8BA4` as `[800A8BA4,800A8EAC)`,
`0x308` bytes, SHA-256
`0ad29bfc1b27aeafdff5fa8b850bc1485fe92167ed61bb93b380306c06fd69d3`.
It loads archive 4/0 file `AA` through `800A8314`, allocates two `0x1108`
buffers, and initializes/copies **109 40-byte textured quads** before
setting `D_800AF278=1`. Blend selection retains its prior value for
unrecognized high nibbles; the low nibble selects four UV orientations.
Do not substitute a generic rectangle or assume every record uses the
same UV/blend mode.

Its source-data dependency is not currently available through native
symbols: `D_800AEB68` and `D_800AEF14` are still independent 32-byte
zero-filled generated placeholders. The actual texture rectangle table
starts at `800AEB68`, followed by the placement table at `800AEF10`.
The 109 placement records are four halfwords each (x,y,texture,flags),
and the maximum referenced texture index is 116. The contiguous retail
table span `[800AEB68,800AF278)` is `0x710` bytes, SHA-256
`0dff25f38e14a98cb98a48bfced5e53b75b4bd9f860ff650a5b824fe58050e57`.
Placement bytes alone hash to
`27981996f3f951c5839ab42d75c9819464ff8a794acbf90bbe14eadf6b1f0115`.
Restoring these real tables and their interior aliases is required before
enabling the initializer. This transcription is still **NOT IMPLEMENTED**.

An independently reproducible ABI defect in its existing cleanup path
was repaired first. `data_field.c` correctly places `D_800AFC60/64/68`
four bytes apart, but `misc5.c` declared the first two as host pointers.
`func_800A83B4` consequently passed merged neighboring words to HeapFree
on LP64. The test reproduced addresses `2345600012345000` and
`abcdef0123456000` instead of `12345000` and `23456000`. Retail cleanup
uses `lw`, as verified in `[800A83B4,800A8408)`, SHA-256
`813fc87d3838c34aac26237f42a57de2a3c83bd9be4b484fe96ce20f48acacd8`.

The declarations are now `u32`, with explicit pointer reconstruction in
cleanup and the existing quad-update helper. The real production misc5
and data_field TUs are tested together by
`run_field_overlay_pointer_slots_test.sh`: active/inactive cleanup,
ordered free addresses, DrawSync count and adjacent-word preservation.
O0/O2/UBSan pass; reinstating host-width declarations is rejected.
Logs are `pc_port/build_native/field-overlay-pointer-{red,green,final}.log`.
This is a native ABI regression test with recording DrawSync/HeapFree,
not full retail differential execution or visible overlay acceptance.
The live game remains PID 301201 on the preceding executable; this
pointer repair has not yet been rebuilt into it.

## FEB1 retail layout data restored

`data_field.c` now owns the complete `[800AEB68,800AF278)` table span
as 226 four-halfword records: 117 texture rectangles and 109 placements.
Values were extracted mechanically from the pinned `disc/field.bin`
slice, not inferred from screenshots. Seven aliases preserve the exact
interior offsets: AEB68/+0, AEB6A/+2, AEB6C/+4, AEB6E/+6,
AEF10/+3A8, AEF14/+3AC and AEF16/+3AE. No neighboring BSS was moved.

`run_field_overlay_tables_test.sh` pins the slice SHA-256 and compares
all 1808 bytes in the production data TU against the source file. It
checks every alias and all 109 texture indices. After correcting an
initial test declaration warning, RED reported the missing native owner.
O0/O2/UBSan pass; deliberate table-byte and alias-offset corruptions are
rejected. The packed-pointer cleanup regression also still passes.
Logs: `pc_port/build_native/field-overlay-tables-{red,green,final}.log`
and `field-overlay-pointer-after-tables.log`.

The runtime initializer `800A8BA4` remains unimplemented. These restored
data owners and the previous pointer repair have not yet been included
in a complete-game rebuild; no new visual acceptance or stub-count
reduction is claimed for the running PID 301201.

## FEB1 initializer native implementation

Implemented native `func_800A8BA4` in misc5.c, retaining the matching
build's assembly fallback. It uses the restored retail placement and
texture tables, two packed u32 buffer-pointer slots, and existing asset
loader and UV-clamp helper. It creates both 109-quad buffers, preserves
blend selection for unsupported high nibbles, leaves UV bytes untouched
for unsupported low nibbles, copies all 40 bytes per primitive, and sets
the active flag only after initialization.

`run_field_overlay_init_retail_test.sh` executes the original field
instructions and actual field UV-clamp instructions through the MIPS
test adapter. The native side compiles full production misc5/misc4/data
TUs. It compares both entire 0x1108 buffers for original retail records
and synthetic flag records, and checks allocation size/count, load/user
counts, buffer pointers and initialization globals. SDK primitive helpers
and heap operations are shared test boundaries; asset loading is a
recording boundary. This is not independent GPU, CD or allocation proof.

RED reported the missing owner. The first GREEN link exposed the same-TU
loader interception limitation of --wrap; the test now weakens only the
loader definition in its scratch object and supplies a recording owner.
Production source and builds do not use that replacement. O0/O2/UBSan
pass. Quad-count, blend-retention, UV-endpoint and copy-size mutants are
rejected. The first blend mutant survived because the fixture's preceding
blend value hid the error; rearranging synthetic selectors now tests
retention of initial zero and blend one as well as two.

Logs: `pc_port/build_native/field-overlay-init-{red,green,final}.log`,
`field-overlay-pointer-after-init.log`, `field-overlay-tables-after-init.log`.
Pointer and table regressions pass. This native implementation, its table
owners and packed-pointer repair still await full-game rebuild and live
overlay acceptance. The existing active game is deliberately untouched.

## Overlay initializer build and remaining renderer dependency

The full build now includes the initializer, real tables, and packed
pointer repair. `field-overlay-port-build.log` reports LINK OK, **82**
generated function stubs and **570** data symbols. Executable SHA-256:
`b8d60aed2be6a95d4b413421375c17398f9dbd45bdc3bc358932195b42cd9281`.
Stub manifest SHA-256:
`330618deafa1aed0a5643c0ea9379c209aa425d6e67c5cdd2600bc60b405c943`.
The initializer is strong text at `0047B6AA`; table aliases occupy the
expected byte offsets beginning at native `005F6820`.

The new audit `pc_port/build_native/field-audit-after-overlay-20260905`
(.json/.csv/.log) has zero table mismatches and zero generated-stub
edges directly from dispatched handlers. This does NOT establish full
dependency closure: field-wide dependencies remain 15 callers, 20 edges,
14 targets; four native assert-zero sites and one authored-stub edge
remain. Data frontier: 695 callers, 1957 edges, 284 targets, 272 default32
placeholders. All 481 matching comparisons remain NOT_RUN. Runtime and
retail gates both FAIL. JSON SHA-256:
`f9ecd1fc1e8b226529bc655f81bb085801b4f09ccfc128832246a4918f82d3af`;
CSV SHA-256:
`d75899ec2ac231cc61f1dde9d8c32d366b5f95dfa18b6301c748c22f143a30e2`.

In particular, misc2.c's per-frame renderer calls `func_800A84C0`.
Its current body asserts when `D_800AF278` is active. Therefore executing
FEB1's newly implemented initializer still reaches an unfinished renderer;
the overlay is NOT playable/accepted yet. Do not remove the assertion or
suppress the active flag. The exact retail renderer range is
`[800A84C0,800A8BA4)`, `0x6E4` bytes, SHA-256
`2b9ff04c20456340d56f8fa7df6c553297fbe6f568892fafad3b6c804cd2f724`.
Initial tracing identifies camera-derived calculations, repeated calls to
`800A8408` for UV updates, and ordering-table submission. Full body and
dependency behavior still need transcription and differential tests.

Old PIDs 301191/301201 were stopped only after preserving their binary
as `opening-gear-transitions-lKzHOrjQ-xeno-port` (same earlier hash).
Visible relaunch directory: `scratchpad/opening-overlay-init.jAtpDFoA/`;
debugger PID 326227, game PID 326237, interactive session 82126. The same
existing capture/input/trace setup is used, without forced scene state.
No new overlay runtime acceptance is claimed.

Frame 1500 of this new run was inspected and is entirely black. Logs
subsequently show the title idle timeout and another field/menu cycle.
This capture is not a successful title-screen visual check; whether the
black frame is transient, capture timing, or a display defect is unresolved.
Do not infer a regression cause from that single frame.

## Black-capture comparison and renderer UV dependency repair

Current frame 3960 is also black, but preserved pre-overlay run
`opening-gear-transitions.lKzHOrjQ` frame 1500 is black as well. Both
logs (and the older sprite-reentry log) show identical backdrop diagnostics:
zero source/destination/framebuffer samples, texture sample count 227,
texture sum 696674. The symptom predates the initializer build; this does
not prove its cause or establish that the actual desktop is black. The
capture hook calls PsyX_TakeScreenshotPath after EndScene via Vsync.
Title display/capture investigation remains unresolved.

Tracing the complete renderer exposed a faulty existing UV helper.
`func_800A8408` retail `[800A8408,800A84C0)` SHA-256
`de23a6240991b148db8a4e979c7d3b1d7423754f932cb8fde494086bd5ebe25e`
selects the active buffer and submits corners
`(x,y+h-1),(x+w,y+h-1),(x,y-1),(x+w,y-1)` to the clamp helper.
Native code always selected buffer zero and used different endpoints.
Both differences are now repaired. Native buffer lookup uses the owning
`g_FieldBss_800AFC60` array, rather than indexing past a scalar alias.
The first repair's scalar-based indexing triggered UBSan object-size
checks on page one; no sanitizer suppression was added.

`run_field_overlay_init_retail_test.sh --uv` compares original retail
execution against the production helper and clamp code: all 109 quads,
both buffers, offsets (0,0), (-17,-9), and (255,255), for **654 cases**.
It compares both entire buffers, including untouched bytes. RED failed
at page0/index0/offset0; O0/O2/UBSan now pass. Wrong-page, off-by-one
endpoint, and vertical-orientation mutants are rejected. Initializer
regressions also pass. Logs: `field-overlay-uv-{red,green,final}.log` and
`field-overlay-init-after-uv.log` under pc_port/build_native.

Renderer trace obligations for the next increment: camera-derived masked
angles; UV-update groups 0..15, 16..28, 29..33, 34..38, 39, 40..41;
conditionally submit index42 using frame bit0x10; always submit43..108;
prepend to the active context word at +0x80E4, preserving high tag bytes;
update frame/three-step animation counters in retail order. The renderer
itself still asserts when active. The UV fix is not yet in the running
executable; live PID326237 was preserved throughout this investigation.

## Active overlay renderer transcription

Replaced the native `800A84C0` active-path assertion with the full retail
submission sequence; matching builds now retain the original assembly
fallback. The inactive path still returns without drawing. The native
body preserves wrapped 32-bit camera differences before signed shifts,
both masked angles, all six UV-update groups, the three-step animation
counter, index42's frame-bit blink, all remaining quads, OT high bytes,
and modulo-32-bit frame counter increment. Buffer selection uses the
actual packed backing array. No active flag or scene state is forced.

New production-TU test `run_field_overlay_render_retail_test.sh` executes
the original `[800A84C0,800A8BA4)` instructions through the MIPS adapter,
including the real retail UV-update/clamp routines. Native execution uses
production misc5/misc4/data code. It compares both complete buffers, the
entire context (including the OT bucket), counters, and math-call inputs.
There are 32 cases: both pages, inactive/active, frame values
0/1/15/16/31/32/7FFFFFFF/FFFFFFFF. Camera inputs include signed-subtraction
wrap boundaries. The low-24-bit packet address requirement is asserted.

After correcting the test's ratan2 prototype to the native SDK's int ABI,
RED reproduced the existing active-path assertion. O0/O2/UBSan pass.
Quad-count, blink-bit, angle-shift, OT-high-byte and frame-increment
mutants are rejected. Initializer, 654-case UV, and packed-pointer
regressions pass. Logs under pc_port/build_native:
`field-overlay-render-{red,green,final}.log`,
`field-overlay-init-after-render.log`, `field-overlay-uv-after-render.log`,
and `field-overlay-pointer-after-render.log`.

Limits: magnitude and ratan2 results are controlled recording boundaries;
their numerical implementations are not validated here. Tests cover one
yaw/pitch result combination, not exhaustive angle space. Actual GPU
submission, natural script activation, PSYQ matching, and visual overlay
acceptance remain unverified. No full-game rebuild of this renderer yet;
the live game remains on the preceding executable.

## Renderer build and math boundary follow-up

Complete build `field-overlay-render-port-build.log` passed LINK OK.
Executable SHA-256:
`857117db267fb29c713fd6adc9fbd6461a815410e2e37865d9490cf633e22d35`.
Generated counts remain 82 function stubs/570 data symbols: replacing an
authored assertion does not retire a generated stub. Fresh audit basename
`pc_port/build_native/field-audit-after-overlay-render-20260905` reports
three native assert-zero sites (previously four), zero dispatched direct
stub edges, and unchanged 15/20/14 field caller/edge/target counts. Data
frontier is now 697 callers/1960 edges/284 targets/272 default32 entries.
Both gates still FAIL; all 481 matching comparisons remain NOT_RUN.
JSON SHA-256 `1f0ffd963092c8584b6de6aaf93900432c37c5d8ad672be3e415be291e8eeffb`;
CSV SHA-256 `d75899ec2ac231cc61f1dde9d8c32d366b5f95dfa18b6301c748c22f143a30e2`.

Previous PIDs326227/326237 were stopped after preserving their executable
as `opening-overlay-init-jAtpDFoA-xeno-port` with the original hash.
New visible run: `scratchpad/opening-overlay-render.tDqD2mgV/`, debugger
PID360380, game PID360390, interactive session42693. Frame420 was
inspected and shows the Squaresoft logo. This verifies one boot frame,
not the title menu, actual overlay activation or gameplay parity.

The next dependency check is concrete: native `Square0` in psyq_compat.c
uses three plain signed C multiplications, while retail
`[8004A414,8004A43C)` loads GTE IR1/2/3, executes word `4AA00428`, and
stores MAC1/2/3. Slice SHA-256:
`99cb1c8d0c79529e8c6f1a56f640ebd7400c2eb0f57312cdc2e252c1e34f201b`.
`FieldGetVec2Magnitude` calls this SDK routine before SquareRoot0, so
the controlled math results in the renderer test do not cover that
hardware behavior. Operand width, register side effects and overflow
behavior need a retail/GTE differential test before any repair.

## Square0 retail SDK adapter repair

The new test first failed to compile because PsyCross does not declare
Square0; an explicit test prototype corrected that setup issue. Behavioral
RED then failed at input bits0, separate destination: the existing C
multiplications did not reproduce retail output/GTE state. The adapter
now uses `gte_ldlvl`, `gte_sqr0`, and `gte_stlvnl`, exactly corresponding
to retail IR loads, SQR(sf=0), and MAC stores, and returns its output
pointer. This preserves register effects and all-inputs-before-writes
behavior without signed C multiplication overflow.

The existing battle GTE retail suite now checks 262144 Square0 cases per
configuration: every low16 pattern with two upper-word variants, three
different lane patterns, and separate/in-place destinations. It compares
24 output/guard bytes, return identity, and all CP2D/CP2C state against
execution of the pinned retail leaf. O0/O2/UBSan pass, along with the
suite's existing normalization/matrix/projection cases. Wrong square
scale and omitted result stores are rejected. Logs:
`pc_port/build_native/square0-retail-{red,green,final}.log`.

Both sides use the same PsyCross GTE backend, so this establishes wrapper
equivalence, not independent PS1 hardware accuracy. Full magnitude-chain
testing (including SquareRoot0 and signed addition semantics), all other
Square0 consumers, and visible gameplay remain to be verified. The live
PID360390 is untouched; this adapter repair is not yet in its executable.

## Full two-component magnitude chain

The GTE suite now executes the actual field magnitude routine plus the
retail Square0, SquareRoot0 and square-root table, with no recording
math boundary. Native execution links production misc7.c, psyq_compat.c
and PsyCross. Eighty cases span zero, signs, 181, signed16 limits,
high-word truncation and INT32_MAX, comparing result and CP2D/CP2C state.

RED at (0,0) had matching numeric result but mismatched GTE state:
PsyCross SquareRoot0 used a pure leading-bit-count calculation, whereas
retail writes LZCS and reads LZCR. The tracked patch
`psycross_sqrt_lz_registers.patch` restores those register operations;
build_port.sh applies it. The vendor file contains non-UTF8 bytes, so
direct apply_patch could not read it: the tracked patch was created with
apply_patch and applied using the repository's git-patch mechanism, without
re-encoding unrelated vendor text. Reverse-apply check passes.

All 80 cases pass O0/O2/UBSan, alongside the existing Square0,
normalization, matrix and projection tests. Omitting the LZCS write is
rejected; Square0 scale/store mutants remain rejected. Logs:
`pc_port/build_native/magnitude2-retail-{red,green,final}.log`.
The runner pins these additional retail slices:

- Field `[80099A4C,80099A8C)`: `63e8d69bc64a73a15ad3495fe8f13d4a792e16110ee37a8e4466a7105182674b`.
- SDK `[80048C4C,80048CD0)`: `fda79f78e9d9d817a0a8e1612f3a55e14e3783c220b4d6e26bff8fdaf32ed391`.
- Table `[80056A00,80056B80)`: `45ca1b7b619b33961c03c82243b8baf59fc50c49b4d7eefc94b76edbd417a5c8`.

Explicit domain limit: both narrowed operands equal to -32768 produce
sum0x80000000. The retail sqrt then addresses before its normal table;
that pair is deliberately excluded from the normal-domain comparison,
not marked passing. Its exact read address/data and native handling still
require investigation. Other two/three-component overflow cases are not
certified. This is shared-GTE equivalence, not independent hardware proof.
Neither math repair has been rebuilt into live PID360390 yet.

## Magnitude overflow authority and native reproducer

The previously excluded pair now has an explicit retail-only observation,
not a guessed conventional square root. Executing the original magnitude
and SDK instructions with the actual source halfword loaded produces one
two-byte read at `80056880`, value `7350`, and result `00039A80`.
The read is recorded by the test bus. Source halfword SHA-256:
`eff6e7a1bc97fe7fd326c73642feefb306607803ff7d45a15beda347e3bc9c86`.
The GTE runner pins that source along with the existing routine/table
hashes. This observation passes in O0/O2/UBSan; normal-domain cases and
existing regression/mutation checks remain green. Log:
`pc_port/build_native/magnitude2-overflow-oracle.log`.

The explicit native diagnostic command
`XENO_MAGNITUDE_OVERFLOW_PROBE=1 pc_port/build_native/battle_gte_retail_test/UBSan`
fails in misc7.c on `1073741824 + 1073741824` signed overflow. Log:
`pc_port/build_native/magnitude2-overflow-native-ubsan.log`. It is an
unresolved failing probe, not counted as native acceptance. Merely wrapping
the addition is insufficient: negative SquareRoot0 currently indexes
outside its C array, whereas retail reads actual preceding executable data.
The implementation needs both defined 32-bit accumulation and an
authority-backed memory lookup. No clamp, fabricated edge result, or
production suppression was introduced in this diagnostic increment.

## Magnitude overflow implementation

The existing PsxMemory static-data loader already copies executable sdata
`[8004EA90,800576E4)`, including the sqrt table and its prefix; port_main
calls it before boot. The repair uses that actual guest-memory region.
misc7's two-component sum now explicitly wraps as u32 before converting
back to s32. The tracked Xenogears integration patch
`psycross_sqrt_retail_memory.patch` makes SquareRoot0 perform its original
address-based signed-halfword lookup in g_PsxRam, with defined unsigned
left shifts and the retail logical final shift. It does not special-case
the observed return constant. This is game-specific SDK integration,
not a generic replacement table for other PsyCross consumers.

The normal comparison now includes the previously excluded pair: all
**81** magnitude cases pass O0/O2/UBSan. Three additional test-only source
halfword variations (zero, 8001, FFFF) also match retail execution,
including sign extension and 32-bit shift behavior. The explicit UBSan
overflow probe now exits successfully. Existing Square0, normalization,
matrix, projection and mutation tests pass. Logs:
`magnitude2-overflow-{green,final}.log` and
`magnitude2-overflow-native-fixed.log` under pc_port/build_native.

The existing static-data production certificate passes all three regimes
and five negative controls in the toolchain container using
`W34C2_PSYCROSS_LIB=pc_port/build/libpsycross.a`. Initial host invocation
lacked libraries; the first container attempt exposed an unnecessary
`-fpermissive` flag rejected for C by its GCC. Removing that flag preserved
all warning-as-error and assertion gates. Log:
`magnitude2-static-data-regression-container.log`. The memory patch's
initial hunk-count typo was corrected before application; reverse-check
passes. Vendor source was patched without re-encoding non-UTF8 content.

The specific overflow boundary is now behaviorally covered, not globally
certified: three-component accumulation, arbitrary negative SDK inputs,
independent GTE hardware fidelity, missing-media behavior and live game
effects remain open. Full rebuild of these math changes remains pending;
live PID360390 was not touched.

## Math integration build and visible relaunch

Complete build `pc_port/build_native/retail-magnitude-port-build.log`
passed LINK OK with 82 generated function stubs and 570 data symbols.
Executable SHA-256:
`cd748feed8ffa6564b307d3a4a18a9f371cb512f128d5a4d2ac298a37185354d`.
Native text owners: FieldGetVec2Magnitude `00484279`, Square0 `0052076E`,
SquareRoot0 `00589328`. Both tracked SDK patches are in this build.

Fresh audit basename `pc_port/build_native/field-audit-after-magnitude-20260905`
retains zero direct dispatched stub edges and table mismatches, but still
has 15 field callers/20 edges/14 generated-function targets, three native
assert sites, one authored-stub edge, and 284 generated-data targets.
All 481 matching comparisons remain NOT_RUN; runtime and retail FAIL.
JSON SHA-256 `50ef95275f2adf71ebed875d6e33876d574f077f91fa96b513fcf1cfb8c27681`;
CSV SHA-256 `393649baca8a80b01fb8d855fc8a9af7a8b6ab53c31ddae19862cb120cc85f84`.

The previous run was stopped after preserving
`opening-overlay-render-tDqD2mgV-xeno-port` (hash857117db...633e22d35).
New visible run: `scratchpad/opening-retail-magnitude.FlBP6qZo/`, debugger
PID391058, game PID391068, interactive session24429. Log line22 confirms
retail sdata `[8004EA90,800576E4)` loaded from disc/SLUS_006.64 before
boot; line45 enters movie16. The process is live and generating captures.
No manual New Game, title-display or scene/overlay acceptance is claimed.

Remaining native assertions were rechecked: the field PC-HDD timing
marker, plus actor double-render and rotated-actor paths in 80075B44.
The latter two remain substantive rendering dependencies, not assertions
to suppress. Retail tracing and tests are the next repair step.

### Actor split-render dependency trace (2026-09-05)

Read-only runtime recheck: debugger391058/game391068 remain live. Canonical
executable still hashes to cd748feed8ffa6564b307d3a4a18a9f371cb512f128d5a4d2ac298a37185354d.
No rebuild, input injection, restart, or new visual acceptance in this pass.

The existing assertion label "rotated actor" and parameter name `angle`
are misleading: retail leaves 8001EE88 and 8001F1D4 clip sprite geometry
and UVs on opposite sides of a vertical coordinate. There is no rotation
calculation in either leaf. The third argument is shifted by sprite+40's
scale exponent and compared with each primitive's vertical extent.
This is disassembly-derived semantics, not yet native differential proof.

Authority pins (end-exclusive):

| File / mapped span | SHA-256 |
| --- | --- |
| field.bin [80076118,80076300), base8006FAF0 | 66b6038c32ce09208817040ebddbacc8ff85835886f5b1a116e903a277a9b342 |
| SLUS_006.64 [8001EE88,8001F1D4), EXE header800/load80010000 | 0eb777659e58db935469d9c2414d85e6fc6f8434e2d80d1c86f9e54f13445c0c |
| SLUS_006.64 [8001F1D4,8001F530), same mapping | cc1fa7d97c9c15195a062e2be867d115201f0b97597ca94de0993dae3fcf7103 |

Caller details established from actual instructions:

- Actor+E8 values FFDE/FFDF select the double-render path. Actor+04 bit
  02000000 skips it. Otherwise set color from +FC/+FD/+FE directly, set
  sprite+3D=EF, draw at OT+(initialIndex*4)-40; project center(0,300,0),
  set color from +FF/+100/+101 directly, set sprite+3D=F7, draw at the
  newly projected depth shifted by D80050100. Then skip ordinary drawing.
  Neither color call uses the fog-conditional wrapper; the second depth
  does not receive the ordinary minus-two adjustment.
- For ordinary actors clear sprite+3D before testing actor+04 bit02000000.
  Actor+134 bit20 selects 8001E2F8: tint from +FC, project a center whose
  signed-halfword Y is twice (actor+EE minus sceneDip/3), shift depth and
  subtract two only when depth>=2, then pass actor+EE as third argument.
- Actor+134 bit40 independently selects 8001E368: tint from +FF, use the
  original adjusted OT index and the same signed-halfword third argument.
  Both bits may be set; this is not an exclusive if/else choice.

Dependency closure is incomplete: E2F8/E368 have C wrappers in
src/slus_006.64/system/rendering.c, but their EE88/F1D4 leaves are
INCLUDE_ASM and generated native stubs (stubs.c lines600/601 in this build).
Both wrappers also conditionally call 8001E9BC, requiring separate coverage.
Enabling the caller alone would therefore route draws into missing code.

Both clipping leaves consume 24-byte primitive records, reserve 40-byte
packets, and call only 8004A7BC (RotTransPers4). The reservation check is
strictly work+count*40 < end; equality rejects. Each packet is reserved
and initialized before vertical rejection, so rejected geometry still
advances the work pointer. Scratch XY halfwords at 8004FB98 are written
without clearing the other vector fields. Texture corrections depend on
projected X order; tag updates preserve upper bytes and use low24 links.
These effects must be compared, not only the final visible quad.

Next implementation gate: execute each pinned leaf in the existing retail
MIPS test adapter, record the projection boundary, and compare production
packet bytes, scratch vectors, OT links and work-pointer movement. Cover
both split sides, mirrored dimensions, boundary equality, zero/count63,
scale shifts, UV wrap and rejected geometry before replacing the stubs;
then test and restore the caller branches. No stub/assert count reduction
or retail/runtime PASS is claimed by this trace.

### Sprite clipping native leaves and integration (2026-09-05)

Implemented native 8001EE88 / 8001F1D4 in
src/slus_006.64/system/rendering.c, sharing the two leaves' common packet
setup/projection/UV/link sequence while retaining their distinct clipping
arithmetic. Matching builds retain both INCLUDE_ASM owners. MIPS shifts and
wrapping additions/subtractions are explicit unsigned32 operations; packed
screen results are copied from host-sized SDK locals as four-byte words.
The existing persistent quad scratch is shared, not replaced by fresh vectors.

New test: pc_port/tests/sprite_split_retail_test.c and
pc_port/tests/run_sprite_split_retail_test.sh. The fixture includes the real
rendering TU to inspect its static scratch; only its conflicting legacy
strlen declaration is renamed within the test. The real MIPS interpreter
executes both pinned leaf byte ranges. RotTransPers4 is a recording boundary
with two deterministic projection variants, not emulated hardware proof.

RED: missing native leaf, after resolving the fixture's legacy declaration
compile conflict. GREEN: 21,120 cases per O0/O2/UBSan, all passing. Cases cross
both leaves, 16 mirror/dimension/projection variants, counts0/1/3/63,
scale exponents0/1/4/15/31, eleven signed split coordinates including extrema,
and exact/one-byte-above/one-byte-below allocation limits. Comparisons cover
the entire fixture (including untouched packet/input guards), scratch vectors,
ordered projection inputs/call count, OT link bytes and work cursor.

Five negative controls rejected: strict allocation equality, reservation
advance, clipping equality, mirrored corner selection and OT high-byte
preservation. Several fail even with zero submitted quads, proving that
rejected-primitive side effects are actually observed. Logs:

- pc_port/build_native/sprite-split-retail-red.log
- pc_port/build_native/sprite-split-retail-green.log
- pc_port/build_native/sprite-split-retail-final.log
- pc_port/build_native/sprite_split_retail_test/{capacity,reservation,equality,mirror,tag}.log

Regressions PASS: portrait rendering (including negative controls),
sprite-create wrapper (65,601 cases per regime), battle graphics ABI and
sprite reentry (including no-op mutant). Separate logs use
sprite-split-{portrait,create,graphics-abi}-regression.log under build_native.
These are selected relevant suites, not an exhaustive project test run.

Full container build PASS: pc_port/build_native/sprite-split-port-build.log,
LINK OK, 80 generated function stubs (was82), 570 data symbols.
Executable SHA-256:
7c826b6f106d5269fb021a5d7042686fe59493a09baf7c71c5ce8527780e835c.
Native strong text owners EE88=004D3C4C, F1D4=004D3C82; shared helper004D3532.
Preserved prior executable scratchpad/opening-retail-magnitude-FlBP6qZo-xeno-port
(cd748feed8ffa6564b307d3a4a18a9f371cb512f128d5a4d2ac298a37185354d),
stopped debugger391058/game391068 and verified both absent before rebuilding.

Fresh visible run scratchpad/opening-sprite-split.96m7Nam8/:
debugger429498, game429508, interactive session39453. Log confirms movie16;
frame360 capture (BMP despite .png suffix, converted losslessly to
view-frame-000360.png for inspection) shows the Squaresoft logo. This is
boot-output observation only; no natural split-sprite scene or title/New Game
acceptance is claimed. No gameplay state was forced to exercise the new leaves.

Audit field-audit-after-sprite-split-20260905 retains 483 slots/481 handlers,
zero dispatch mismatches/direct stub edges, but 15 field callers/20 edges/
14 generated function targets, three native assertions and 284 generated data
targets. Matching NOT_RUN481; runtime FAIL and retail FAIL remain.
JSON SHA-256 7df43d131526bafc7f13ee36790aa6b05319c736c7bee9cd6dd22ff92688143d;
CSV SHA-256 393649baca8a80b01fb8d855fc8a9af7a8b6ab53c31ddae19862cb120cc85f84.

Next: caller-branch differential proof and restoration in 80075B44, including
the double-render branch and independent bit20/bit40 split draws. Conditional
8001E9BC behavior remains an additional dependency to verify. This increment
does not certify complete actor rendering, matching decompilation, or the port.

### Actor sprite caller branches restored (2026-09-05)

src/field/main/misc2.c now routes the native post-transform draw tail of
80075B44 through private FieldRenderActorSpriteTail. The matching-build
source path remains unchanged. The private helper implements the pinned
[80076118,80076300) branch behavior documented above: FFDE/FFDF double
draw, hidden actors, ordinary draws, and independent bit20/bit40 clipping.
Diagnostic counters classify hidden/plain/special; ordinary per-actor
logging is retained after the helper returns. No assertion is simply
suppressed in the native path.

New production-TU differential test:
pc_port/tests/actor_sprite_tail_retail_test.c, run by
pc_port/tests/run_actor_sprite_tail_retail_test.sh. RED was the unresolved
private helper before implementation. The oracle enters actual retail
code at80076118 and halts at80076468; the color wrapper80075B08 executes
retail instructions, while projection, color setter and sprite submissions
are recording boundaries. These are caller-control-flow tests, not a full
80075B44 frame test or hardware/leaf integration proof.

43,200 cases per O0/O2/UBSan PASS: five animation states around FFDE/FFDF,
all clipping-bit combinations, hidden/fog flags, five signed heights,
six signed depths, shifts0/1/2/31/32/-1 and three first-draw callbacks
(unchanged flags, set bit40, clear bit40). The callback mutations are
test-only and verify the retail flag re-read after the first clipping draw.
Checks compare ordered calls, projection XYZ, colors, OT arguments, split
arguments, sprite mask and all fixture bytes. Tests reject five controls:
wrong EF mask, extra depth subtraction, fog-gating the double draw,
making the clipping bits exclusive, and caching flags across the first draw.
Logs: actor-tail-retail-{red,green,final}.log under pc_port/build_native.

Sprite-split regression PASS: 21,120 cases per regime and five controls
(actor-tail-split-regression.log). NPC-event suite initially could not
link UBSan on the host (missing /usr/lib64/libubsan.so.1.0.0). The unchanged
suite passed all three regimes and M1-M3 in the toolchain container;
grep -E was used for the script's compatible rg invocations because the
container lacks rg. actor-tail-npc-regression-container.log records the
result. Optional historical live logs were absent in the isolated evidence
directory; their collection warnings do not constitute new live proof.

Full container build PASS: actor-tail-port-build.log, LINK OK,
80 function stubs/570 data symbols. Executable SHA-256:
5ef51d7025157efcb2e4cf309895a09402d3492767634b1ffaff4dc1ad4b60fa.
Native helper0046E0E5, caller0046E446. Previous executable preserved as
scratchpad/opening-sprite-split-96m7Nam8-xeno-port (7c826b6f...780e835c);
debugger429498/game429508 were stopped and verified absent before rebuild.

Fresh visible run scratchpad/opening-actor-tail.YluePCHV/:
debugger445796, game445806, interactive session82900. Process and capture
generation confirmed live; no natural double/split actor acceptance claimed.
The earlier transform, fog and object-actor portions of the full caller
were not covered by this bounded differential test.

Fresh audit field-audit-after-actor-tail-20260905: native assertion count
falls3 to1 (the remaining PC-HDD timing marker), but runtime FAIL/retail FAIL
remain. Other counts unchanged: 483 slots/481 handlers, matching NOT_RUN481,
15 field callers/20 edges/14 generated-function targets, 284 data targets.
JSON SHA-256 89de53a4e9cd43413d8ade84d5733387f7a210ebe598ff916882714a53a50955;
CSV SHA-256 393649baca8a80b01fb8d855fc8a9af7a8b6ab53c31ddae19862cb120cc85f84.

Remaining shared sprite dependency8001E9BC is still a generated stub,
conditionally called by ordinary and both clipping wrappers. Retail span
[8001E9BC,8001EE68),1196 bytes, SHA-256
ae85540de0cc7aabc12334f1860d166b96827448b47aa6326d68d633f9c898be.
Its direct calls are8004974C ScaleMatrixL,80049CEC ApplyMatrix,
80049EFC SetRotMatrix,80049F8C SetTransMatrix,8004A7BC RotTransPers4.
Trace/decompile/test that dependency next; full actor rendering and natural
opening-scene fidelity remain unproven, not complete.

### Shared sprite shadow routine restored (2026-09-05)

Native 8001E9BC now exists in src/slus_006.64/system/rendering.c;
matching builds retain INCLUDE_ASM. Retail behavior is a black textured
quad pass using a scaled matrix and X/Z geometry, consistent with sprite
shadows. It installs rotation/translation even on zero-count or capacity
rejection, uses direction masks, and averages projected rows with signed
halfword wrapping before truncating division by two. This body replaces
the generated no-op, not a new approximate shadow effect.

Authority: previously pinned1196-byte leaf ae85540d...f9c898be. Separate
scratch [8004FAD8,8004FAF8) is32 zero bytes in the retail EXE, SHA-256
66687aadf862bd776c8fc18b8e9f8e20089714856ee233b3902a591d0d5f2925;
direction masks [8004FAF8,8004FB08) are1,2,4,8,16,32,64,128, SHA-256
4c0ed3ab5045f9ec560b3d482485fb7c664fde704241a992ffeab246c6cbf426.
Both data pins and the leaf pin are enforced by the new runner.

pc_port/tests/sprite_shadow_retail_test.c and its run_sprite_shadow_retail_test.sh
reuse the split fixture's low-address bus/projection recorder. The fixture
adds an opt-in shadow projection variant to generate signed/wrapping Y
averages; ordinary split tests are unchanged. ScaleMatrixL, ApplyMatrix,
SetRotMatrix, SetTransMatrix and RotTransPers4 are controlled recording
boundaries. Matrix calls/arguments/results, including order and zero-draw
side effects, are compared, but this is NOT independent hardware/GTE proof.

RED: missing shadow scratch/body (after correcting a test macro collision).
GREEN: 16,128 cases per O0/O2/UBSan. Cases cross mirror/dimension/projection
variants, counts0/1/8/63, scale shifts0/1/15/31, signed scales including -3
and extrema, seven direction masks including EF/F7/FF, and exact/above/below
capacity. Whole packet/input fixture, scratch, matrix boundary records,
projection inputs, OT and work cursor are compared.

The first average-rounding mutant survived because geometry generated only
even sums. The fixture was strengthened with odd widths; all eight controls
then failed: half-scale rounding, height source, omitted matrix install,
direction mask, average rounding, missing halfword wrap, nonblack tint and
wrong UV endpoint. No production behavior was changed to satisfy a mutant.
Logs: sprite-shadow-retail-{red,green,final}.log under pc_port/build_native;
individual negative-control logs in sprite_shadow_retail_test/.
Split and actor-tail regressions PASS all three regimes and their controls
(sprite-shadow-{split,tail}-regression.log). No exhaustive project-suite or
natural shadow-scene acceptance claimed.

Full container build PASS: sprite-shadow-port-build.log, LINK OK,
79 function stubs/570 data symbols. Executable SHA-256:
3033e0edd5519bd425106c97eb8b731670da9a4283067baeb9e7fe7eb5c89909.
Native owner004D37B8; shadow scratch006019C0. Prior executable preserved
at scratchpad/opening-actor-tail-YluePCHV-xeno-port (5ef51d70...ad4b60fa);
debugger445796/game445806 verified absent before rebuilding.

Visible relaunch scratchpad/opening-sprite-shadow.SwIipLfS/:
debugger468855, game468865, interactive session55917. Live process and
boot capture generation confirmed. No new title, natural shadow or opening
cutscene visual acceptance is claimed.

Audit field-audit-after-shadow-20260905: runtime FAIL/retail FAIL, matching
NOT_RUN481, native assertion1; field dependency/data counts unchanged.
JSON SHA-256 d20ae1ae998faac0d78084774fe0e9081940415decf8bd1f61475a85921f0798;
CSV SHA-256 393649baca8a80b01fb8d855fc8a9af7a8b6ab53c31ddae19862cb120cc85f84.

Next confirmed non-retail behavior: native80075B44 actorFlags4 bit2000
still falls through to ordinary sprite transforms/drawing. Its comment
relies on the obsolete wrong-overlay interpretation explicitly corrected
in pc_port/src/field_object_overlay.c. Retail80075C68 branches instead to
[80076300,80076468),360 bytes, SHA-256
e21be205a231d5806218bcd367350b0292d8cf90b1afd7c06cdb027ef228fb3f.
That branch updates overlay object/node flags, visibility, rotation, scale
and position with no renderer call. Restore/test this exact state path next,
including packed slot advancement and global skip behavior, rather than
retaining a substitute sprite draw. The entire port remains incomplete.

### Object-actor sprite fallback removed (2026-09-05)

Replaced the native actorFlags4 bit2000 fallthrough in80075B44 with the
retail object-state update. Removed its obsolete wrong-overlay justification.
Private FieldUpdateObjectActor implements the pinned360-byte branch and
returns the packed object-slot cursor. The caller starts this cursor at0
each frame, advances it only for processed object actors, and continues
the actor loop without ordinary sprite transforms/drawing. The global
D8004F380 skip does not consume a slot. Matching-build behavior unchanged.

The helper preserves object flags except bit0, controls visibility from
actor status, transfers rotation in either direction according to flag20000,
uses wrapped MULT/MFLO scale arithmetic before SRA12, updates object/node
position, and clears actor flag200. The original packed scale data is read
at owning BSS g_FieldBss_800B20A8+164+slot*4, not invented per-object values.
The live slot table remains the existing u32 D801E8670 owner.

Tests: actor_object_state_retail_test.c and actor_object_loop_retail_test.c,
runner run_actor_object_state_retail_test.sh (default or --loop).
RED: missing helper before implementation. State comparison PASS7,680
cases per O0/O2/UBSan, covering all ten slots,64 flag/status combinations,
global skip, signed coordinate/angle/scale boundaries and32-bit multiplier
extrema. Full fixture bytes, packed table/BSS guards and both retail cursor
registers are checked. Global-skip cases have null slot entries, so unwanted
object dereferences do not silently pass.

The loop suite executes full retail [80075B44,800764B4), SHA-256
a1f058817eeb40031c36bf07396c7a6dcf6bcbb620adbd1ad83f2496a1392a10,
and the real native caller, comparing three actor records, object/node
state and their guards over consecutive frames. PASS256 cases per regime:
all active/inactive and visible/hidden combinations, plus global skip.
Only camera-direction lookup is bridged to a fixed value in this object-only
fixture; matrix copy executes retail instructions. Any native sprite
transform/draw/GTE call is a named test failure. This proves the object-only
control path, not mixed active ordinary sprites or the downstream overlay
renderer's visual output.

Seven state negative controls rejected: skip cursor, flag preservation,
scale shift, rotation sign, position offset, actor flag clear, slot lookup.
Three whole-loop controls rejected: restoring sprite fallthrough, failing
to advance the cursor, and retaining the cursor between frames. Logs:
object-state-retail-{red,green,final}.log and object-loop-retail-{green,final}.log
under pc_port/build_native; individual controls in their respective test dirs.
Sprite-tail regression PASS43,200 cases per regime and five controls
(object-state-tail-regression.log). These are bounded suites, not full-game
or exhaustive-project acceptance.

Full container build PASS: object-state-port-build.log, LINK OK,
79 function stubs/570 data symbols. Executable SHA-256:
f253b1ad63a8f0201b70fbb945c4de040af736ea083dc843cfe573ee9ccafbd7.
Native helper0046E0E5 and caller0046E611. Preserved prior executable:
scratchpad/opening-sprite-shadow-SwIipLfS-xeno-port (3033e0ed...b5c89909).
Debugger468855/game468865 verified absent before rebuilding.

Fresh visible run scratchpad/opening-object-state.XdaZAq5l/:
debugger500501, game500513, interactive session18319. Live process and
boot capture generation confirmed; no natural object-actor scene acceptance
claimed. The downstream object overlay draw path is still a separate audit.

Audit field-audit-after-object-state-20260905 remains runtime FAIL/retail FAIL,
matching NOT_RUN481, native assertion1,15 field callers/20 edges/14 generated
function targets. Data census now698 callers/1961 edges/284 targets because
the helper adds a read of existing D8004F380; default32 targets remain272.
JSON SHA-256 cdb892d84e03ce354ab89c34ffc6a84e983b6c61f2d337d4fabd295cd3d7d041;
CSV SHA-256 393649baca8a80b01fb8d855fc8a9af7a8b6ab53c31ddae19862cb120cc85f84.

Remaining renderer audit lanes: downstream object-overlay transforms/draws,
the ordinary actor transform/fog path outside the tested tail, and the
PC-HDD timing-marker branch in800748E8. Full decompilation/port fidelity
and natural opening/world/battle gameplay acceptance remain unfinished.

### Object-overlay draw/animation audit: closure is not retail (2026-09-05)

Read-only code/disassembly investigation after the object-state repair;
game500513/debugger500501 remain live. No executable change or new visual
acceptance in this audit. Current func_801E7D14 in field_object_overlay.c
is NOT a source-faithful implementation of its retail entry. Its manual
node-draw loop does not implement the retail multipass dependency sequence.
Do not describe that file's existing instantiate/draw path as fully ported.

Authority reconfirmed directly from raw disc sectors231361..231385 (25
Mode2 user payloads,2048 bytes at raw+24), local archive6B9/global860,
retail mapping801DC000. Padded payload SHA-256 remains
14395a9f54c124fed016f07906cc882b2a9256d5ade2d10eef99e10fe9366523.
Retail [801E7D14,801E7FD4),704 bytes, SHA-256
2e84156fa452d4cfcb0c90c98f11650394193b0bd2403d10da52c4950140d419.

Verified retail pass order:

1. Update signed/wrapping accumulator801E8640 by1+fifth argument; clamp
   values>=7 to6, repeatedly subtract2 while>=2 to derive the tick count.
   Advance halfword801E869C by56*ticks, call rsin8003F8CC, and write
   halfword801E8698 from the result plus4096 divided by800, plus4.
   Ordinary field caller passes fifth argument1; in the usual remainder
   state this yields one tick. The general routine is not constant-one.
2. For every nonnull slot, snapshot root translation into object11C/120/124;
   call801E36BC(object,801E86A8,ticks,renderContext,1), including the fifth
   argument. The outer pass does NOT first reject hidden objects.
3. Separate slot pass: call801E37D0 for nonnull objects with byte5C != FF.
4. Call801E1880(slotTable), then SetLightMatrix80049F5C(pointer801E8644).
5. Separate slot pass: compute wrapped snapshot-minus-current root deltas
   into128/12C/130; call801DCEC8(object,scene,arg1,1,1,prim,renderContext).
6. Call801E0398(801E86A0,scene,ticks,prim,renderContext).

Current source mismatches confirmed by inspection: ignores arg1/fifth arg;
hard-codes one tick; skips hidden/low-node-count objects in the outer loop;
rebinds actor positions; has no linked-object pass/global lighting pass or
retail auxiliary draw pass; forces model+6 to1 and D80050104 to0; substitutes
a direct CompMatrix/C700 loop for801DCEC8. These findings establish source
divergence, not which individual visible symptom each one causes.

Animation dependency801E36BC [801E36BC,801E37D0),276 bytes, SHA-256
9277585ee654c29ae0768e3551fcab8ef33c94b168c7eedd832f38cbffc71fe3:
retail takes FIVE arguments and ORs per-tick results. For a nonzero object
header, visible objects first call7298, select DC848/DC5C0 by byte37,
then run DDBF8 and E5D44 per positive tick. It calls E39F0 even when hidden
or ticks<=0, passing the original ticks and fifth argument. Current native
four-argument body skips hidden objects, coerces ticks<=0 to1, then calls
approximate OvlyClipTick/OvlyRebuildTree. This is not an ABI/behavior match.

OvlyClipTick itself explicitly implements only selected opcodes and skips
payloads for others; it also clamps ticks to0..8. That is an incomplete
substitute, not retail VM coverage. Do not count synchronized IP alone as
implemented opcode semantics. Replacing7D14 alone would not close this gap.

Additional pinned dependencies:

- [801E37D0,801E39F0),544 bytes, SHA-256
  00001a8d7b974caec5c8bdf0288340aa367b46d2d89ad899eea2a59682340646:
  linked-object visibility/matrix handling, scratchpad1F800000, GTE command
  4A480012 and duplicate MAC stores to root20/5C. Null parent clears the
  attachment selector toFF. This routine is absent as a native owner.
- [801E7298,801E72CC),52 bytes, SHA-256
  3a9efc540796f2edd33f6a239cab221829ca39af7daabf9b045ec704654fd7fc:
  when object byte36 is zero, sign-extend object halfword60 into root+60.
- E1880 uses overlay globals8648 onward and object/node transforms;
  E0398 consumes the auxiliary owner header86A0; DCEC8 gates visibility
  internally and uses scratchpad matrices. Their entry blocks were traced,
  but complete bodies/ranges are not yet claimed audited.

Capstone needed skipdata to display the GTE word at801E3994; stopping its
default iterator at an unknown COP2 instruction would falsely truncate the
linked-object function. The command word and following stores were inspected.

Implementation order must close actual dependencies: linked-object routine
and animation setup/update helpers, exact clip-VM semantics, retail object
draw/auxiliary passes, then replace7D14 with its real orchestration and test
the whole chain. Preserve a differential test of the current failures before
repair. No new implementation, stub retirement, build, or fidelity PASS is
claimed by this audit; the live binary remains f253b1ad...9ccafbd7.

### Linked-object dependency implementation and GTE signed-translation guard

The subsequent source increment implements func_801E37D0 in
pc_port/src/field_object_overlay.c against the 544-byte archive slice pinned
above. It preserves parent detachment, inherited visibility, signed joint
selection, matrix call ordering, scratchpad use, GTE transform, and both
root translation stores. This supersedes only the prior statement that this
leaf lacks a native owner; the approximate 7D14 orchestration still does not
invoke it. No cutscene integration or stub-count reduction is claimed.

run_object_link_retail_test.sh executes the pinned retail leaf in the MIPS
interpreter and compares fixture memory, scratchpad, matrix boundary records,
and GTE registers with native execution. O0, O2, and UBSan each pass 5120
cases. Six negative controls are rejected: detach selector, visibility mask,
joint selection, orientation copy, offset component, duplicate destination.
Matrix SDK calls are controlled boundaries; the two sides share the GTE
backend. This is not independent hardware arithmetic proof or exhaustive
alias coverage. Log: pc_port/build_native/object-link-retail-final.log.

The initial UBSan run exposed negative signed translation left shifts in
PsyCross MVMVA's default branch. Three CV components now multiply signed
64-bit values by4096: all signed32 translations fit without overflow.
The tracked psycross_gte_mvmva_signed_translation.patch is registered in
build_port.sh, and reverse-application checking passes. A scratch-only mutant
restoring the shifts fails at runtime on -1390751315, as required. Its initial
scratch compilation lacked the local GTE header path; adding that include
path fixed the test plumbing, without treating compilation failure as a
successful negative control. Other GTE branches are not claimed UB-free.

The existing battle GTE retail regression runner also exits0 after this
change; its log is pc_port/build_native/object-link-gte-regression.log.
The canonical executable remains SHA-256
f253b1ad63a8f0201b70fbb945c4de040af736ea083dc843cfe573ee9ccafbd7,
with game PID500513 still live. Full port rebuild and natural scene acceptance
of this increment are NOT_RUN. Next work remains the actual animation and
render dependency closure described above, not an arbitrary call inserted
into the provisional renderer.

### Root-height dependency and hierarchy trace

func_801E7298 now has a native source owner in field_object_overlay.c.
Its pinned 52-byte retail body reads object halfword60, checks byte36,
and writes the sign-extended height to root60 only when that byte is zero.
The new object_height_retail_test executes that entire retail leaf directly
without bridged callees and compares all object/root bytes. It does not
compare dead guest stack scratch or volatile return registers for this void
helper. The initial RED was a named missing-owner failure.

run_object_height_retail_test.sh passes all65536 halfwords with gate bytes
0,1,128,255:262144 cases each at O0/O2/UBSan. Unsigned-height, inverted-gate,
and wrong-destination mutants all fail. Final log:
pc_port/build_native/object-height-retail-final.log, SHA-256
cf4fbcd40b34fd45786e0402097f61b906a291563b2948569528185d23c9212e.
Like the linked-object leaf, this is a dependency awaiting the correct
updater; it is not inserted into the provisional update path. Full native
build and natural scene acceptance remain NOT_RUN for these increments.

The next complete hierarchy leaf was inspected directly from the same
archive: [801DC5C0,801DC848),648 bytes, SHA-256
36086b6f0d87dd02bb67c9f407c8a0cc29c4dc43edd036bdf2e5e474b37dce44.
It copies root5C/60/64 to40/44/48, chooses one of two rotation callees by
root byte6, builds a diagonal scale matrix in scratchpad with wrapped
multiply/shift/halfword stores, and uses MulMatrix0 to produce root+C.
Root translation then comes from40/44/48. The child pass follows each
node's word0 parent pointer and bytes4/5/6: conditional local rotation,
parent-dirty propagation, local translation update, then CompMatrix using
parent+2C or a local32-byte matrix copy when parentless. A final pass clears
child byte4, and the function returns the unsigned halfword node count.

Current OvlyRebuildTree instead always rebuilds child rotations, composes
every child with root+C into child+C, and imposes a40-node cap. These are
specific behavioral mismatches, not a claim about which one causes a
particular unseen frame. The retail hierarchy leaf needs a differential
fixture covering parent chains, both rotation selectors, dirty propagation,
scale truncation, parentless copies, and the final clear pass before it can
replace that substitute. No new runtime scene result is claimed here.

### Retail hierarchy leaf implemented; dependency build relaunched

func_801DC5C0 now implements the pinned648-byte hierarchy routine in
field_object_overlay.c. It retains root setup, the two rotation selectors,
scratch diagonal matrix, per-node parent relationships, dirty propagation,
local versus composed matrix destinations, deferred clearing, and node-count
return. The provisional OvlyRebuildTree and outer updater are NOT replaced
yet: the alternate DC848 path and remaining animation dependencies still
need closure. This function is a real linked native owner, not a live-path
integration claim.

New object_tree_retail_test.c executes the entire retail routine and compares
all fixture nodes, scratchpad, matrix-call input records, and return value.
RED: missing native owner. GREEN:36864 cases each at O0/O2/UBSan, covering
counts0..5,256 flag patterns, parentless/root-child/chained topologies, and
eight scale inputs including signed32 extremes. Seven mutants are rejected:
scale shift, forced-root composition, incorrect parent-dirty predicate,
rotation selector, final flag clear, root translation source, parentless
matrix copy. SDK rotation/matrix operations are deterministic controlled
boundaries, not independent arithmetic/hardware parity. Counts above5,
cycles, future-node parents, and arbitrary pointer aliasing are not covered.
Log: pc_port/build_native/object-tree-retail-final.log.

The prior executable f253b1ad...9ccafbd7 was preserved at
scratchpad/opening-object-tree.oymYKIGz/previous-xeno-port before debugger
500501/game500513 were stopped. Their termination was checked before build.
Container build pc_port/build_native/object-tree-port-build.log exits0 with
LINK OK,79 function stubs and570 data symbols. New executable SHA-256:
657376bf7cb7af93f31834d9e2ac83380ef655a54ec0175fe955c75d72f58559.
nm confirms strong native owners for DC5C0,E7298,E37D0.

Visible debugger581204/game581214 were launched using the existing packet
trace script and field input schedule, exec session80153. Run directory:
scratchpad/opening-object-tree.oymYKIGz/. Boot log shows movie16 and CD read
at106105; captured frame300 was inspected and displays the Squaresoft logo.
The capture is BMP despite its .png name; view-frame-000300.png is a lossless
conversion for inspection. This establishes rendered boot output only.
Natural New Game, village/gear scene, and audible output acceptance are
NOT_OBSERVED for this binary. No headless gameplay, forced scene state, or
substitute cutscene content was used.

### Scale-compensated hierarchy dependency

Archive6B9 [801DC848,801DCC34),1004 bytes, SHA-256
5ccf892d0ec38181361a279503da3497bf101eab4cfaef848420fd223df3813e,
was disassembled through its return delay slot. Native func_801DC848 now
preserves this alternate hierarchy path: root setup matches DC5C0, but child
byte5 and byte4 inherit independently when the corresponding parent byte is
exactly1. Dirty rotations apply the child diagonal scale and, with a parent,
left-multiply by the diagonal signed quotient0x1000000/parentScale. Both
child dirty bytes clear only in the final pass. Matrix call order and alias
destinations are retained.

Retail explicitly executes BREAK7 after a zero divisor at801DCA68,
801DCAB4,or801DCB00. Native uses __builtin_trap as a transparent terminal
exception adapter, not a clamped divisor or success return. The positive
constant numerator cannot encounter signed INT_MIN/-1 division overflow.
This does not emulate a resumable PSX exception handler.

The shared object_tree_retail_test fixture has a --scaled runner mode.
RED: missing native owner. Final results:36864 cases each at O0/O2/UBSan,
with eight positive/negative nonzero local scale values and original global
scale extremes, plus three explicit zero-divisor exceptions in each regime.
Each exception checks the interpreter's exact BREAK address/error and the
native child's SIGILL; test children disable core dumps. All eleven mutants
are rejected: the original seven hierarchy controls plus missing byte5
inheritance, wrong reciprocal constant, incorrect byte5 clear, and a zero
divisor success fallback. Matrix arithmetic remains a controlled boundary;
there is still no independent hardware arithmetic or large-node coverage.

Test plumbing fixes were necessary: host process headers conflict with the
unused PsyQ uid_t/gid_t typedefs, so only those names are isolated around the
included production TU. The exception test originally checked cpu.pc after
PcPortMipsRun advanced it; it now checks PC_PORT_MIPS_UNSUPPORTED and the
exact captured instruction error string. No interpreter behavior changed.

Logs: pc_port/build_native/object-scaled-tree-retail-final.log and
pc_port/build_native/object-tree-retail-regression.log. Both hierarchy
regimes and their negative controls pass. Live game581214 is unchanged on
binary657376bf...72f58559. Full-port rebuild and runtime integration of DC848
are NOT_RUN. Actual36BC/animation VM dependency closure remains next; these
helpers do not justify calling the provisional renderer retail-complete.

### Animation dependency boundaries and track-slot release

The next dependency trace pinned three complete bodies rather than treating
adjacent helper code as part of the same function:

- DDBF8..DEF10: compressed per-node track update, SHA-256
  d17b35c9d57add1f9f53f67d2f9b7a0cb7da37cce10542c2d3b545430308a5d8.
- E39F0..E59D4: clip VM, SHA-256
  e6e99115c22e250c6fb388ac09e484537fea5412a2b5f7852639df03354900c5.
- E5D44..E632C: timed-event dispatcher, SHA-256
  760f9f457085e59b63a6ae781297309641b8a4e429cc7ced348c1cda0f6c4c19.

DDBF8 includes signed-byte delta decoding with -128 escape to a packed
signed16 absolute component, axis-skip flags, several decoding modes, and
calls to SquareRoot0,8004B32C,ApplyMatrix,and DF7A8. These are additional
semantics missing from the current partial clip substitute; synchronized IP
is not evidence that these tracks are decoded.

The event dispatcher uses the nine-entry retail table at801DC208 for command
values1..9. Command8 loads halfword from its stack+50 at801E6014, and the
inspected body does not initialize that slot before this read. That value
chooses E8394 versus E8330 while walking selected object slots. Its provenance
is UNRESOLVED, including prior stack/callee effects: do not invent a zero
default or assert either branch is unreachable. Event counter/frame looping
and event packet processing are distinct from the clip VM.

Native func_801DF7A8 now closes the track-slot release leaf
[801DF7A8,801DF7F4),76 bytes, SHA-256
8f70f0f618f542a463cf5615e738dde96473747f1eb56ebce732f2318e3ea393.
It preserves null-entry return -1 without dereferencing the pool, unsigned
wrapped pointer difference divided by20, minimum free-index halfword update,
single entry-byte clear, and index return. The reciprocal multiply sequence
in retail implements unsigned division by20; no native pointer subtraction
between unrelated allocations is used.

New track_release_retail_test executes that complete retail leaf directly.
RED: missing native owner. GREEN:720896 cases each at O0/O2/UBSan, all65536
free-index values across ten offset boundary/wrap cases and a null case.
The test compares the full fixture and return register. Four mutants fail:
wrong stride, wrong cleared byte, reversed minimum comparison, null return.
Log: pc_port/build_native/track-release-retail-final.log.

Full compressed-track decoder and event/clip VM remain unfinished. This
release helper is not yet invoked by a native DDBF8 owner. Live581214 and
executable657376bf...72f58559 remain unchanged; rebuild/runtime acceptance
of the new release leaf is NOT_RUN. No substitute events or forced state
were introduced to conceal the unresolved dependencies.

### Complete rotation-track stage of the compressed decoder

FieldDecodeRotationTrack now implements DDBF8's complete rotation stage,
[801DDC4C,801DE3EC),1952 bytes, SHA-256
dccbaf18ec9fed9ebc0ab175122824cb16fb40ca45821fd9bafc265209ba048b.
It is an explicitly named extracted stage, not a claim that DDBF8 is fully
ported. The translation and scale stages still must follow it in the native
owner before this decoder can replace the provisional clip path.

The retail nine-entry table maps modes0..8 to absolute halfwords,
signed-byte delta/absolute escape, halfword delta, timed interpolation,
convergence, acceleration, lifecycle-only, pitch/yaw tracking, and yaw-only
tracking. Selectors9..15 take the retail lifecycle-only default. Axis masks
apply to the stream modes. Tracking modes7/8 have their separate frame update
and do not take ordinary completion/status processing. Ordinary completion
releases or rewinds the slot and preserves event-tag-dependent status bits.
The implementation calls actual SDK SquareRoot0/ratan2 and the native release
leaf; none of the test's deterministic SDK values are production behavior.

rotation_track_retail_test enters the actual instruction block with the
parent function's s1,s5,and stack arguments populated and stops exactly at
the translation-stage entry. It compares the full node/pool/track/stream
fixture, accumulated status, and recorded SDK arguments. DF7A8 executes as
retail instructions in the oracle, not as a test bridge. SquareRoot0/ratan2
are controlled boundaries for this stage test. Callee arithmetic, dead
registers, arbitrary pointer aliasing, and full decoder invocation are not
covered by this comparison.

RED was missing native stage. GREEN:81920 cases each at O0/O2/UBSan,
all256 flag bytes, eight lifecycle/absence combinations, eight frame values,
five data patterns including convergence, and signed duration variants.
Eight mutants fail: escape sentinel, axis mask, convergence snap predicate,
stream rewind, released pointer, completion status, tracking-frame threshold,
and angle wrapping. Division guards preserve zero/INT_MIN-overflow traps,
but explicit exception tests for this new division helper are not yet run.
Log: pc_port/build_native/rotation-track-retail-final.log.

Live game581214/executable657376bf...72f58559 remain unchanged. Full-port
build and natural scene acceptance of this extracted stage are NOT_RUN.
Next: translation/scale decoding, then full DDBF8 differential integration;
event dispatcher and complete clip VM remain separate unfinished work.

### Translation-track stage

FieldDecodeTranslationTrack now implements [801DE3EC,801DEAD0),1764 bytes,
SHA-25608b1ac1913ecb0d87685d236ed288a9e9aa3b2adc66cce01d9a49c7636ce6acd.
Unlike rotation, outputs are wrapped32-bit coordinates. Absolute stream
values sign-extend, byte deltas wrap, and escaped halfwords replace rather
than add. Mode2 consumes masked local deltas, scales each to a truncated
signed16 component, calls ApplyMatrix with the current node+2C, then applies
global scaling with32-bit product truncation before adding coordinates.
Mode4 convergence tests narrowed steps and adds sign-extended16-bit steps;
mode5 likewise narrows velocity before coordinate addition. Only node byte4
is marked dirty by this stage. Modes6..15 retain lifecycle-only behavior.

The differential fixture executes the exact stage with the parent frame's
current-node and global-scale arguments populated. ApplyMatrix is a controlled
boundary; its matrix and three input components are compared in full along
with all fixture memory, status, and lifecycle state. It does not prove SDK
arithmetic or full-decoder integration. RED was missing native stage.
O0/O2/UBSan each pass81920 cases, covering all flags/modes, signed-coordinate
limits, five scale/data patterns, frame boundaries, convergence and absence.
Eight mutants are rejected: escape, absolute signedness, convergence narrowing,
velocity signedness, matrix source, scale shift, dirty byte, and rewind.

The matrix-source mutant initially survived: both candidate matrices had an
identical A5 fill. The fixture now initializes node words distinctly before
setting its fields. The unchanged production implementation passes this
stronger fixture and the wrong-matrix mutant fails. No assertion was weakened.
Final log: pc_port/build_native/translation-track-retail-final.log.

The scale-stage body DEAD0..DEEB0 was also inspected. Its mode4 convergence
uses track halfwords4/6/8, not the current node scale; it updates those track
halfwords on a nonzero step. Its completion loop always sets frameFFFF,
unlike stream-mode rewinds in the earlier stages. These differences must be
preserved in the pending scale implementation. Do not clone rotation without
changing those semantics. Live581214/binary657376bf...72f58559 remain untouched;
full-port rebuild and scene acceptance of the translation stage are NOT_RUN.

### Scale stage and full DDBF8 decoder integration

FieldDecodeScaleTrack implements [801DEAD0,801DEEB0),992 bytes, SHA-256
523d133906e84817afe9b4a18b5a16406d1f8acf29dcf2748826a35784e44e7e.
It preserves the track-local convergence state, signed16 interpolation,
acceleration, both dirty bytes, and the unconditional frameFFFF loop rule.
Only modes3/4/5 update components; other selectors still execute lifecycle.
RED: missing stage. O0/O2/UBSan each pass81920 cases. Six mutants fail:
wrong convergence source, missing track writeback, snap predicate, loop frame,
released pointer, and dirty byte. Log:pc_port/build_native/scale-track-retail-final.log.

Native func_801DDBF8 now composes all three stages in the retail node loop.
It samples the unsigned halfword count once, initializes status0, visits each
node with stride7C, and performs rotation then translation then scale while
carrying status across stages and nodes. This supersedes the prior statement
that the complete decoder has no native owner. It does not supersede the
unfinished outer36BC/event/clip-VM integration.

track_decoder_retail_test executes the complete pinned4888-byte retail
function, including retail DF7A8 release calls, against the native full owner.
The fixture covers all4096 combinations of mode nibbles across three stages,
eight lifecycle/count/scale variants, up to four nodes with mixed selectors,
and two successive ticks. O0/O2/UBSan each pass65536 comparisons of full
node/track/pool/stream memory, return status, and ordered SDK call inputs.
SquareRoot0,ratan2,and ApplyMatrix remain controlled SDK boundaries; no
independent SDK arithmetic or rendered-scene parity is claimed by this test.
Zero divisors/overflow traps, arbitrary track aliasing, and larger node counts
are not yet covered in the full decoder fixture.

Six full-owner mutants are rejected: omitted rotation, omitted translation,
omitted scale, discarded accumulated status, missing node stride, and swapped
rotation/scale ordering. Log:pc_port/build_native/track-decoder-retail-final.log.
The initial RED was a named missing native owner failure.

Live581214/binary657376bf...72f58559 remain unchanged; full-port rebuild and
natural-scene acceptance of these decoder additions are NOT_RUN. Next work
is the real36BC dependency closure, including E5D44 and E39F0, followed by
replacement of the provisional updater. This is source-backed decoder
progress, not completion of the battle cutscene or the full port.

### Decoder exception proof and accumulated full build

The event8 stack hypothesis was checked against the entire decoder body.
With the same caller SP, E5D44's frame88+offset50 corresponds to DDBF8's
frameB8+offset80. DDBF8 has no direct write to that slot; its named locals
stop at50 before saved registers90..B4. This does not resolve the event value
or establish a default. Prior stack/callee provenance remains UNRESOLVED.

The complete decoder fixture now tests terminal exception behavior in
addition to successful cases. Retail zero-divisor BREAK7 sites are DDEE8,
DE758,and DEB58 for rotation/translation/scale interpolation. The translation
convergence INT_MIN/-1 case reaches BREAK6 atDE860. For each, the test checks
the exact interpreter error/address, then executes the native decoder in a
child and requires SIGILL from the explicit host trap. Core dumps are disabled
for those children. All four cases pass at O0/O2/UBSan; this verifies terminal
failure mapping, not resumable PSX exception-handler emulation. A seventh
full-decoder mutant that returns0 instead of trapping is rejected.
Log:pc_port/build_native/track-decoder-exceptions-final.log. Normal65536-case
comparisons and the prior six mutants continue to pass in the same run.

Before rebuilding, binary657376bf...72f58559 was preserved at
scratchpad/opening-track-decoder.p49YIhVx/previous-xeno-port. Owned debugger
581204/game581214 were stopped and absence verified. The container build
pc_port/build_native/track-decoder-port-build.log exits0 with LINK OK,
79 function stubs and570 data symbols. New executable SHA-256:
9eee82e130e9d1ddc0b64c5cbd489ababf9d88acf5d65769339560de61dd04f2.
nm confirms native DC848,DDBF8,and DF7A8 owners. This supersedes the pending
full-build status for the accumulated decoder/hierarchy source additions.

Visible debugger635513/game635523 were relaunched using the existing packet
trace script and input schedule, exec session32558, directory
scratchpad/opening-track-decoder.p49YIhVx/. No scene state was forced. Natural
New Game/gear-cutscene acceptance is NOT_OBSERVED for this binary. The
provisional36BC/clip/event path is not replaced by this build, and no stub
retirement or whole-port fidelity PASS is claimed.

### Clip movement distance and target-selector boundary

Native func_801E6338 implements [801E6338,801E63A8),112 bytes, SHA-256
1cd7b9c67a4b1252a296aedc1d58a30072009e776872ded476621fbff93c4eca.
It reads signed16 target coordinates at object88/8A/8C and root32-bit
position5C/60/64, preserves wrapped subtraction/square/sum, and calls the
actual SquareRoot0 adapter with that exact32-bit sum. No clamped distance
or widened substitute arithmetic is introduced.

object_distance_retail_test executes the complete retail leaf with a
controlled SquareRoot0 boundary and compares its input, call count, return,
and full unchanged fixture. RED: missing native owner. GREEN:262144 cases
each at O0/O2/UBSan, sweeping all target halfwords with rotated patterns and
32-bit position extremes. Five mutants fail: unsigned target, incorrect root
offset, missing axis, discarded sum, and modified return. This does not add
new SquareRoot0 hardware parity proof. Log:
pc_port/build_native/object-distance-retail-final.log.

The complete adjacent target-resolver body [801E63A8,801E6578),464 bytes,
SHA-2564ddc077241fbfd1dea89eb13ff3500e1f20af9b3c45c4f70c32fd4d239e3973d,
was traced but not implemented. Signed selector58 maps FF to the first set
low-eight bit of10A (or8 if none); FE reads global86B0 as unsigned16; FD reads
object20; FC reads object21; FA selects10; selectors1..127 select value-1;
other values retain initial slot0. It then reads a packed pointer from
801E8670+slot*4, skips null/self, resolves signed joint5A, transforms offset64,
and writes target88/8A/8C. A present root rotation track is retargeted only
when its entire mode byte is exactly7 or8, not merely its low nibble.

The current native slot array has ten entries. Retail selector results can
address data beyond it, so copying the lookup as an unchecked C array access
would introduce host UB; clamping to slot0 would invent behavior. The correct
backing-data/address mapping and real caller constraints remain to be closed.
This is an explicit unresolved dependency, not proof the selector is unused.
No target-resolver fallback was added. Full-port rebuild and scene acceptance
of the distance leaf are NOT_RUN; live635523/binary9eee82e1...61dd04f2 remain
unchanged. The outer clip/event updater is still incomplete.

### Track-pool lifecycle closure and header correction

The address-mapping search did not identify an existing native adapter for
out-of-range object-table reads. Separately, retail738C was traced: it calls
DF5F4 on86A8 with its argument and E0064 on86A0 with capacity16, then resets
specific slot/light data. The provisional738C currently omits these pool
allocations. It is not yet replaced, pending the auxiliary pool and data
owners; do not initialize an invented pool size in its place.

Native DF5F4(create),DF668(destroy),DF6A8(reset),and DF6F0(claim) now cover
[801DF5F4,801DF7A8),436 bytes, SHA-256
81d5fc3a664b9ff11609c9dd60d64a9236cf43ab0955278c885fa9ee400a1c62.
Creation rejects nonpositive counts unchanged, stores the truncated capacity
before heap calls, requests wrapped32-bit count*20 bytes, and preserves the
retail failure-state writes. Reset clears record flags only, not whole
records. Destruction clears the free-index before HeapFree and leaves
capacity unchanged. Claim returns NULL for an occupied initial slot; on a
free slot it advances the cursor past subsequent occupied records without
marking the returned record occupied. This is deliberately not a generic
allocator policy.

track_pool_retail_test executes all four complete retail bodies, comparing
the entire header/record allocation, return pointers, heap call arguments,
and header state observed during HeapFree. Heap allocation is a controlled
boundary, including injected failure and large32-bit size requests; no huge
real allocation is performed by the test. RED: missing owners. O0/O2/UBSan
each pass23096 cases, including count65535/65536/65537/INT_MAX, nonpositive
counts, and all256 occupancy patterns for an eight-record scan. Seven
mutants fail: capacity, allocation size, reset byte, destructor ordering,
premature occupied flag, missing cursor advance, and occupied-slot test.

The native86A8 owner was declared128 bytes despite its eight-byte retail
layout (pointer,next-index,capacity), ending at the separate86B0 selector.
A named header-extent test failed first; declaration is now eight bytes.
This corrects the owner size, not the still-unresolved arbitrary table-index
address mapping. Full pool suite passes after this change. Existing cleanup
regression also passes at O0/O2/UBSan with37 checks and its three mutation
controls; compiler warnings remain and are not claimed eliminated.
Logs:pc_port/build_native/track-pool-retail-final.log and
pc_port/build_native/track-pool-cleanup-regression.log.

Live635523/binary9eee82e1...61dd04f2 remain unchanged. Full rebuild and natural
scene acceptance of the pool additions/header-size change are NOT_RUN.
Native738C integration requires the remaining retail auxiliary owner and
global reset layout; no guessed event state or fallback slot was added.

### Auxiliary-pool lifecycle closure

Restored native 801E0064 (constructor), 801E00DC (destructor), and 801E011C
(reset) from archive 6B9 at 801DC000, retail range [801E0064,801E0248),
484 bytes, SHA-256
`587c6c424093b1ce08f561d48477b1176b36a89236f27f7dbd1bb8e3aecdc621`.
The existing archive payload pin is rechecked by the new runner. SDK targets
are identified by the repository symbol map: SetPolyFT4 80043CB0,
SetSemiTrans 80043BFC, GetClut 80043A58, GetTPage 80043A1C.

Constructor always selects heap user 4, including nonpositive counts; its
allocation request wraps `(count + 1) * 124` in 32 bits. It stores the
truncated capacity and zero next index before allocation. Reset uses signed
capacity plus one, initializes only fields 16/1E and two FT4 packets per
record, and leaves the other bytes intact. Destructor clears both index
halfwords before calling HeapFree and clears the pointer afterward.

`aux_pool_retail_test.c` executes the complete retail bodies against the
native functions. RED reported missing native owners. Its 156 cases per
O0/O2/UBSan cover allocation failure, signed/truncated/extreme counts,
32768-record initialization, empty reset, and null/non-null destruction.
Full fixture memory, return pointer, ordered SDK/heap arguments, and header
state at heap calls are compared. SDK operations and heap allocation are
controlled boundaries, not proof of hardware packet semantics or actual
multi-gigabyte allocation. Seven negative controls fail: omitted extra
record, wrong stride, omitted second packet, wrong UV, wrong allocation
size, wrong free ordering, and packed-header aliasing.

The first optimized run exposed halfword writes being invisible to the
word-sized header observer under C alias analysis. Byte-copy stores now
preserve the retail updates without disabling compiler checks. The alias
negative control recreates that failure. The 86A0 header comment was also
corrected: it contains eight bytes, not six.

Logs: `pc_port/build_native/aux-pool-retail-final.log` and
`pc_port/build_native/aux-pool-track-regression.log`. The track-pool
regression passes all 23096 cases at O0/O2/UBSan and its seven negative
controls. These are focused tests, not a full-port test-suite pass.

Live game PID 635523 and debugger 635513 were verified and left untouched.
Canonical rebuild and visible acceptance of these additions: NOT_RUN.
The provisional 738C initializer is unchanged; its remaining global reset
owners and consumers must be traced before integration. No scene state,
event defaults, or rendering shortcuts were introduced.

### Initializer ownership trace and destructor integration

Re-read retail 738C through 742C (160 bytes), SHA-256
`5287e22284932db7e892ba7743bf4f2de91317d355b3392ec5aa96a27395f728`.
It clears word8640 and halfword869C, constructs the track pool with its
unchanged argument, constructs the auxiliary pool with capacity16, clears
ten object slots in reverse order, clears the first word of eight records
at85F4 with stride8, and clears halfwords864E/8662. It does not blanket-clear
the adjoining bytes. Native main.c currently passes D_800B234A: misc3.c
initializes that value to64 and misc6.c's 800A0DC0 changes it from a script
argument. These caller-source observations are not newly byte-matched proof.

Retail742C at76B4..7734 scans the eight85F4 registry entries, passes one to
DC22C, and stores the selected record pointer in the object. Thus this is
not the current ten-element, host-pointer-sized s_ptrTab representation.
Retail1880 reads the two stride20 effect records with active halfwords at
864E/8662; event dispatch5F50/5F7C sets/clears those fields. Retail7D14
consumes8640 as a timing accumulator and869C as an advancing phase. These
consumers establish why dropping those resets or substituting a generic
memset would be wrong. Full initializer/registry integration remains open.

During this trace, corrected the existing7FD4 cleanup wrapper to invoke
the now-tested DF668 and E00DC destructors rather than duplicate their
stores in the wrong order. Added two cleanup assertions that snapshot
header bytes at HeapFree. The pre-change runner failed; its temporary
failure log was removed by its existing EXIT trap. An attempted standalone
diagnostic compile omitted the runner's incompatible-pointer warning flag
and failed to build, so that command is not counted as behavioral evidence.
After integration, the canonical cleanup runner passes O0/O2/UBSan with39
checks, existing three mutants, and retail slice/full-disc hash checks.
Log: `pc_port/build_native/pool-cleanup-order-final.log`.

No canonical rebuild or visible acceptance was performed. The live game
and its debugger remain preserved. The broader provisional object registry,
initializer, update loop, and VM are not declared retail-complete.

### Packed model-registry constructor and host adapter

Implemented retail801DC22C through801DC2D0 (164 bytes), SHA-256
`ed37d38e4361912726b17fd6aeb8902f96e13650ba9caf105aa5347b7aa4e971`.
The native owner writes the eight-byte packed registry entry, always calls
the allocator with wrapped count*4 after model relocation, preserves the
count on allocation failure, and generates group pointers with unsigned
32-bit arithmetic at model+10+i*38. It adds no count predicate or invented
empty-allocation rule. It reloads the registry pointer during the loop as
retail does. Arbitrary overlapping entry/allocation memory is not covered
by this fixture and is not claimed proven.

Added model_registry_retail_test and its runner. First RED: missing native
owner. Complete retail instructions run in the MIPS adapter; model relocation
8002C3E8 and heap calls are controlled boundaries. Both memory and ordered
call arguments/header observations are compared. O0/O2/UBSan each pass2053
cases: counts0..1024 with allocation success/failure, plus signed extremes
with allocation failure. Successful enormous allocations/loops are NOT_RUN.
Six deliberate faults are rejected: group offset, stride, allocation size,
count store, zero-count shortcut, and omitted relocation.

Extended the same comparison to the active OvlyBuildPtrTab host adapter;
it failed first at count0 because the provisional sanity filter skipped
the retail allocation call. Replaced that helper with a call to801DC22C
and an explicit packed-PSX-pointer to host-pointer conversion. Removed its
count filter and duplicate construction logic. All2053 cases and six
negative controls pass after the adapter integration. The existing
slot-indexed s_ptrTab allocation/selection and node consumers remain
provisional; this is not a claim that the eight-entry registry is integrated.

Logs: `pc_port/build_native/model-registry-retail-red.log`,
`pc_port/build_native/model-registry-adapter-red.log`, and
`pc_port/build_native/model-registry-adapter-final.log`.
Cleanup regression: `pc_port/build_native/model-registry-cleanup-regression.log`.
The canonical live binary is unchanged; full rebuild and visible acceptance
of the constructor/adapter change remain NOT_RUN.

### Visible rebuild: registry, pool lifecycle, and cleanup additions

Verified old debugger635513/game635523 live, preserved the prior executable
as `scratchpad/opening-registry-build.d1wtVwlD/previous-xeno-port` (SHA-256
9eee82e130e9d1ddc0b64c5cbd489ababf9d88acf5d65769339560de61dd04f2),
then stopped only that owned session and verified both PIDs gone before
rebuilding. The container build completed with LINK OK. Log:
`pc_port/build_native/registry-pools-port-build.log`. Remaining generated
stub census:79 function stubs and570 data symbols; not a completion metric.

New executable SHA-256:
`c272ca8d4f7b148ac5fd5e13c96df3b1209aae4e9c22a22ede613dd14d7b11e9`.
Its symbol table confirms strong text owners for C22C, DF5F4, E0064, E00DC,
E011C, and E6338. Visible launch uses debugger660913/game660923,
exec session23708, console and captures in
`scratchpad/opening-registry-build.d1wtVwlD/`. Captures now run every300
frames. Field test inputs remain ordinary button pulses, not scene-state
injection, and do not select New Game at the title.

Inspected frame300 via lossless conversion of the BMP capture to
`view-frame-000300.png`: Squaresoft logo rendered. This establishes visible
boot output only. Logs report SPU/XA initialization and movie16, but audible
sound, natural New Game progression, and the changed model construction
path are NOT_OBSERVED in this verification. No claim of complete cutscene,
audio, or registry fidelity follows from this boot capture.

### Retail initializer reset and pool integration

Replaced provisional738C with the complete previously pinned retail reset
sequence. It now passes the unchanged caller argument to DF5F4, constructs
the auxiliary pool with16, clears the exact slot/registry/active-field
locations, and preserves other registry words and effect-record bytes.
Added packed data owners85F4[8][2], word8640, forty-byte8648 effect block,
and halfword869C. Their initial bytes were checked as zero in the archive.
Host-only s_ptrTab bookkeeping is still cleared separately; the remaining
slot-indexed registry selection is explicitly not claimed retail-complete.
Removed the provisional742C lazy738C(0) call and its s_inited flag: the
retail742C entry does not initialize the overlay or replace field capacity.

New object_initializer_retail_test executes738C and both pool constructor
bodies in the retail interpreter. All eight native data regions are mapped
to their retail addresses for comparison. Distinct controlled allocations
and independent failure bits exercise both pools; full allocations and
global regions (including preserved bytes) are compared with ordered heap
and SDK calls. Model/gameplay state is not forced. RED: missing native data
owners. O0/O2/UBSan each pass2056 cases (counts-1..512 and four allocation
failure masks). Seven negative controls fail: forced capacity64, omitted
auxiliary pool, clearing registry count words, blanket effect reset,
missing timing/phase reset, and omitted object slot. Large/truncated counts
remain covered by the separate pool suites, not this initializer fixture.

Two-allocation support initially changed the auxiliary fixture's optimizer
inlining enough for its alias mutant to survive. Kept the original auxiliary
boundary unchanged unless XENO_TEST_SPLIT_POOLS is explicitly enabled by
the initializer fixture. Reran both suites: all controls pass, including
the original alias regression; no assertion was removed. Cleanup regression
also passes39 checks at O0/O2/UBSan and its existing three mutants.

Logs: `pc_port/build_native/object-init-retail-final.log`,
`pc_port/build_native/object-init-aux-regression-final.log`, and
`pc_port/build_native/object-init-cleanup-regression.log`.
The visible c272ca8d...14d7b11e9 binary and game660923 were left untouched.
Full rebuild and natural scene acceptance of this initializer change are
NOT_RUN. These focused fixtures do not prove full registry/update/VM fidelity.

### Node-constructor dependency audit: unresolved native shortcuts

Read complete retail C2D0..C5C0 (752 bytes), SHA-256
`d10a984d37284d898fcea03ff7360ef9d0cb0a0e0e41046775aa7020c1681e6a`.
The current OvlyBuildNodes is NOT an exact translation. Concrete differences:

- Retail takes eight arguments: packed registry, pair list, build mode,
  texture-setup flag, and four signed texture/CLUT coordinates. The native
  helper combines build mode and texture setup into one argument.
- Retail counts both valid model indices and FFFF entries until another
  out-of-range index terminates the list. FFFF denotes a model-less node,
  not the end of the list. The native helper treats FFFF as termination and
  fabricates a list of all models when it finds no records.
- Retail parent index p resolves to root+(p+1)*7C, with FFFF meaning NULL.
  Native uses root+p*7C and imposes its own range filter.
- Retail initializes specific node fields and leaves matrix bytes intact;
  native zeroes the entire node and writes identity matrices. Root byte6
  is1, child byte6 is0, and child byte7 is1 in retail.
- Retail has no write forcing model+6 to1. The native command-stream
  heuristic does exactly that. This is an unsupported content mutation.
- Retail calls CB54, optional CC10/CC74, C8CC, and memcpy. Native adds
  texture-page/CLUT global writes and packet restamping after C8CC.
- Retail clears node+70/+74/+78. Native writes a model pointer to node+70;
  its drawing loop consumes that private shortcut at line1817 (line number
  snapshot only), while the restored track decoder interprets70 as a track.
  Fixing only the constructor store would break that remaining draw adapter.
- Retail allocation failure calls CD8C and returns NULL; native continues
  with a partially built array. Payload/count cutoffs are also native-only.

Caller evidence at E7738..77D8: flag40 selects buildmode0/no texture setup;
otherwise object flag4 selects buildmode2/no texture setup; otherwise
buildmode2/texture setup1 with the four supplied coordinates. This is why
merely splitting the current native argument with a guessed flag is invalid.

Failure dependency CD8C..CE18 (140 bytes) SHA-256
`4b9ad7ffc3d4742466f4ef70efbc8e210d78f0b8ce6dcec9ece683933a4bfcba`:
NULL root returns; unsigned root count drives the whole node scan; a nonnull
node68 is freed and then68/6C cleared, but null68 leaves6C untouched; root
count is cleared before freeing root. The current cleanup helper differs.

Next implementation must cover the whole constructor/failure dependency,
then migrate the node70 drawing consumer to the actual registry lookup.
Do not interpret the still-rendering native scene as retail proof. No
production change was made during this audit; live game660923 stayed running.

### Node-constructor failure dependency: CD8C restored

Implemented the complete previously pinned CD8C node destructor. It reloads
the unsigned root count during iteration, frees nonnull68 pointers and then
clears68/6C, preserves6C when68 is null, clears the root count before the
final free, and handles NULL root. No1024-node cutoff remains in this path.

New node_cleanup_retail_test runs the complete retail body against both
the native function and the active OvlyFreeOwnedNodes wrapper. First RED
reported a missing native function. After adding it, the old wrapper
failed at count1/empty68 because it cleared6C. Replaced that wrapper with
a call to the retail function. Each entry path passes2313 cases at
O0/O2/UBSan: all256 occupancy masks for counts0..8, counts1023/1024/32768/
65535 with empty/full occupancy, and NULL root. Full node bytes, ordered
free pointers, and root count observed at every free are compared. HeapFree
is a controlled recording boundary; real freed-memory reuse is not covered.
Five faults are rejected:1024-node cutoff, clearing6C with null68, omitted
count reset, count reset after free, and wrong node stride.

Logs: `pc_port/build_native/node-cleanup-retail-final.log` and
`pc_port/build_native/node-cleanup-owner-regression.log`. Existing owner
cleanup passes39 checks at O0/O2/UBSan and its three mutation controls.
No full rebuild or visible acceptance of CD8C integration: NOT_RUN.
Constructor C2D0 and the private node70 drawing shortcut remain open; this
closes their cleanup dependency, not the complete node-construction lane.

### Complete node constructor and active pointer-width adapter

Implemented previously pinned C2D0..C5C0 as an eight-argument native owner.
The complete function handles FFFF model-less records, out-of-range list
termination, parent index+1, distinct build/setup arguments, exact partial
node initialization, packet generation/copy, and CD8C failure cleanup.
No fallback list, count/payload clamp, identity-matrix fill, mesh-count
mutation, or post-generation packet restamping is added by this owner.

Added node_constructor_retail_test and runner. RED: missing native owner.
The retail interpreter executes the full constructor and CD8C cleanup.
Heap/model packet construction and texture-coordinate setters are controlled
boundaries; memcpy executes a real copy. O0/O2/UBSan each pass5760 cases:
0..8 records, sixteen model/model-less patterns, buildmode0/2, setup0/1,
root allocation failure, successful allocation, and each of eight packet
failure indices. Texture coordinates include signed16 extremes. Full fixture
bytes, return pointers, ordered model/heap calls, and root count at free are
compared. Dirty untouched matrix bytes are preserved in the fixture; future
uninitialized records on failure are compared using a recording-only free
boundary, not dereferenced or actually freed by the fixture. Larger lists,
malformed unterminated lists, arbitrary aliasing, and real heap behavior are
not claimed proven by this test.

Replaced OvlyBuildNodes/OvlyInitNode's provisional implementation with a
packed-entry pointer-width bridge to C2D0. Added the separate texture-setup
argument at the active caller. Moved the existing texture-header object
metadata load before construction because retail E75F4 writes obj4A before
E7738..77D8 chooses setup. General E742C instantiation still has unresolved
source-selection and ownership behavior; this move does not prove that body.

Retail DCCC8..DCD40 draws models through registry[node+8], skipping FFFF.
The active draw adapter now uses that lookup through the existing host-width
registry. Removed its node70 model-pointer dependency and its second forced
mesh-count mutation. The fixture verifies lookup with deliberately poisoned
node70, and compares the active constructor adapter against the same retail
results. Node70 is now initialized to zero as retail requires, for the
animation-track owner. The rest of7D14's transforms/timing/drawing remain
provisional and are not covered by this lookup check.

Nine negative controls fail: FFFF termination, parent offset, wrong build
mode, conflated setup flag, nonzero track field, matrix overwrite, omitted
failure cleanup, drawing through node70, and forced mesh count. Cleanup
regression passes39 checks at O0/O2/UBSan and its existing three mutants.
Logs: `pc_port/build_native/node-constructor-retail-final.log` and
`pc_port/build_native/node-constructor-cleanup-regression.log`.

The live c272ca8d...14d7b11e9 executable/game660923 is unchanged. Full build
and visible opening acceptance of the initializer/node constructor changes
remain NOT_RUN. Removing unsupported mutations may expose missing upstream
model decoding or drawing behavior; address that through retail evidence,
not by reinstating the mutations.

### Full build and visible launch of retail initializer/node path

Preserved c272ca8d...14d7b11e9 at
`scratchpad/opening-retail-nodes.wjykKxsF/previous-xeno-port`, verified its
hash, stopped the owned debugger660913/game660923, and verified both gone
before the canonical build. Build log:
`pc_port/build_native/retail-nodes-port-build.log`; LINK OK, still79 function
stubs and570 data symbols. New executable SHA-256:
`314e2100931873407fae7de672e245a1e6f0f9ab20dfe0e6079de7c36702eff3`.
Strong text owners C2D0, CD8C, and738C are present in the executable.

Visible launch: debugger678634/game678654, exec session92506, logs/captures
under `scratchpad/opening-retail-nodes.wjykKxsF/`, same ordinary field-input
pulses and every300-frame captures. Frame300 inspected: Squaresoft logo.
Frame1200 inspected: black capture, despite console reporting title-loop
entry. Therefore title rendering is NOT verified by this run. A framebuffer
capture is not necessarily a desktop screenshot; do not infer visible title
success from its console alone. No New Game/cutscene/audio acceptance yet.

Loaded the computer-use skill and exact installed Orca guide to try normal
desktop menu input. Initial Orca status was not_running and capabilities
returned runtime_unavailable. `orca-ide open --json` briefly reported a
running app678932; the next status reported not_running/stale_bootstrap.
No keyboard/menu action was sent and no scene state was modified. Desktop
control remains unavailable through that provider; this is not a blocker
to further source-backed implementation. Game678654 remained live.

### Simple object-draw entry restored; actual field caller distinguished

Restored DCC3C..DCD8C (336 bytes), SHA-256
`093893dc891eb7d18aaadd3b381a442ca54da0c363d5d69ea041a83567b0d508`.
DCC34 is a distinct return-only entry. The restored function composes the
root light/view matrices into scratch20/40, then child lighting and view
matrices into scratch0, sets light/rotation/translation per child, skips
FFFF model indices, and calls C700 with the exact requested packet buffer
and forwarded arguments. It does not silently substitute the other buffer
or skip a null selected packet.

New object_draw_retail_test executes the complete retail body. After fixing
an initial fixture prototype mismatch with C700's void-pointer third
argument, RED reports missing native owner. O0/O2/UBSan each pass1536 cases
over counts0..5, all16 model-less masks, both packet buffers, distinct matrix
contents, null second buffers, and forwarded argument variants. Compare
the entire scratchpad, unchanged fixture memory, ordered matrix contents
at SDK calls, and model/packet/draw arguments. Matrix functions, setters,
and C700 are controlled boundaries: not proof of hardware lighting or pixels.
Eight faults are rejected: wrong root/child matrix, omitted lighting,
wrong buffer, swapped arguments, skipped children, null-packet skip, stride.
An initial mutation-script sed syntax error was fixed and the full runner
rerun successfully; no negative control was omitted.

Caller tracing found no direct DCC3C call in this overlay. More importantly,
the actual main field loop E7D14 calls DCEC8 at E7F78. Therefore DCC3C was
NOT substituted into the active draw adapter. The former lookup evidence
supports registry indexing, not interchangeability of the two draw routines.
The actual DCEC8 body extends to DDBF8 and calls E22F8, E1258, E0248 in
addition to matrix/GTE/model boundaries. Its seven-argument caller passes
object, the incoming matrices, a3=1, stack arg4=1, and the two stored caller
values s5/s3. Full rendering integration must follow that body and its
effects dependencies rather than select the simpler tested routine.

Log: `pc_port/build_native/object-draw-retail-final.log`.
The live314e2100...6702eff3 executable/game678654 is unchanged. Rebuild and
visible acceptance of DCC3C: NOT_RUN; no active field-renderer fix claimed.

### Actual draw dependency: auxiliary claim/release

Restored E0248 and E0354 from retail [801E0248,801E0398),336 bytes,
SHA-256 `5ff4f732dc907fb123d331f6d44b62452489a74bf5dd11f8aae6c5de175fb5b5`.
DCEC8 calls E0248 at DD80C and DDA4C. This dependency is therefore on the
actual field draw path, not an inferred replacement draw entry.

E0248 compares signed next/capacity halfwords, claims only when candidate16
is-1, advances the next cursor past occupied records, and calls SetSemiTrans
on both packets using a signed16 argument. It does not mark the claimed
record occupied. Full/occupied-candidate cases return base+capacity*124,
the extra record allocated by E0064, without changing its packet flags.
This sentinel policy is explicit retail behavior, not a native fallback.
E0354 computes the unsigned wrapped address difference divided by124,
conditionally lowers the signed next cursor, marks entry16=-1, and returns
the computed index. Its reciprocal-multiply division agrees with the direct
unsigned division over the exercised range, including wrapped negatives.

Added aux_pool_claim_retail_test/runner. RED: missing native owners.
O0/O2/UBSan each pass18785 cases: signed capacities/cursors-8..8, sixteen
occupancy patterns, four transparency arguments including8000/FFFF, and
release indices-8..8. Compare all8192 bytes covering the fixture header and
all addressed records plus guard space, returned pointers/indices, and
ordered SetSemiTrans calls. The SDK remains a controlled boundary. Large
cursor wrap-around scans and arbitrary unaligned releases are not exhaustively
covered; no general whole-renderer claim follows.

Seven faults are rejected: null instead of sentinel, premature occupancy
mark, omitted cursor scan, wrong second packet, unsigned transparency,
wrong release stride, and reversed cursor predicate. Existing auxiliary
lifecycle passes156 cases at O0/O2/UBSan and all seven of its controls.
Logs: `pc_port/build_native/aux-claim-retail-final.log` and
`pc_port/build_native/aux-claim-lifecycle-regression.log`.
Live game678654/binary314e2100...6702eff3 is unchanged. Full build and visible
acceptance of these additions: NOT_RUN. DCEC8 and its other effects
dependencies remain to be implemented; no placeholder effect was introduced.

### Effect update arithmetic dependencies: E1708/E17B8

Traced E1258's type dispatch: type4 calls E1708 and type5 calls E17B8.
Restored complete range [801E1708,801E1880),376 bytes, SHA-256
`930c868225e060b0e36f6166c70e8fe3caaac7e9ce348ea7b573ab2c5bcafeaf`.
E1708 multiplies unsigned16 source values by a signed16 factor and divides
by32 with truncation toward zero. E17B8 performs that operation on the
difference between unsigned16 sources, then adds the second source. Both
write low16 results through owner1C plus signed origins, a six-byte row
stride, and a two-byte column stride, using signed width/height at2C/2E.
Neither creates an effect or chooses its frame/factor. Products fit signed32
for the complete unsigned16 source/signed16 factor domain; no clamp is added.

Added effect_blend_retail_test/runner. RED: missing native owners.
O0/O2/UBSan each pass15120 comparisons against both complete retail bodies:
width/height-1..4, origins-3..3, ten signed/truncated factors including
32767/-32768/65535/65536, and independent/A-overlap/B-overlap destinations.
The full fixture memory is compared, with no SDK or arithmetic mocks.
Six faults are rejected: row stride, signed source load, arithmetic-shift
rounding, source stride, unsigned factor, and missing interpolation base.
Alias coverage is source/destination overlap, not arbitrary owner-header
overlap or malformed addresses. Large loop bounds are not exhaustively run.
Log: `pc_port/build_native/effect-blend-retail-final.log`.

E1258 remains unimplemented: it has a callback-driven frame update, type0/1
decode/upload paths, linked-record processing, and negative-frame cleanup
through E165C. E165C was traced but not implemented in this increment.
E22F8's larger particle/effect drawing body also remains open. The arithmetic
helpers are dependencies, not a claim that either caller now works.
Live game678654/binary314e2100...6702eff3 stayed running; full build and
visible acceptance of these additions are NOT_RUN.

### Effect negative-frame cleanup: E165C

Restored [801E165C,801E1708),172 bytes, SHA-256
`5ef30c514c52e541640ab7ab25be5e59c48a14278a899750b4056e9bfedff91d`.
Inactive (halfword1A==0) effects are untouched. Active effects with nonnull
buffer4 restore its image via LoadImage only for type byte10<4, then reload
and free buffer4. Nonnull buffers8/C are freed in order, each pointer cleared
after its free; active1A is cleared last. E1258 invokes this path when its
callback returns a negative signed16 frame.

Added effect_cleanup_retail_test/runner. RED: missing native owner.
O0/O2/UBSan each pass12288 cases: all256 type values, three active values,
all eight buffer-presence masks, and a controlled upload callback that either
preserves or changes buffer4. Full record memory and record snapshots at
every image/free call are compared against complete retail instructions.
The callback mutation checks the retail post-upload pointer reload; it is
not a claim that normal LoadImage changes this owner. No real VRAM upload
or heap free occurs in this fixture. Six faults are rejected: active gate,
type boundary, wrong image buffer, skipped free, early active clear, and
cached pre-upload pointer. Log:
`pc_port/build_native/effect-cleanup-retail-final.log`.

Finished tracing E1258 through E165C (1028 bytes), SHA-256
`bde5842f0f7b095691ed3a3be6284607384e7883073524e62cefd7bdfb2ebeff`.
Its remaining linked-effect path computes signed16 rectangle offsets/extents
and copies into linked buffer4, choosing sourceC with source width for types
below4 versus source1C with fixed six-byte row stride for other types. It
sets linked byte11 only when both computed extents are positive. Do not
replace those branches with a generic rectangle intersection/clamp without
matching the actual halfword arithmetic. Full E1258 implementation remains
next, including its callback and existing decode/upload boundary contracts.

The live game remains unchanged; full rebuild and visible acceptance of
this effect cleanup addition are NOT_RUN.

### Frame-updater dependency check: missing type-1 GTE blend

Before implementing E1258, checked its decode contracts. E1348 calls
80026F44(count,factor,destination,source), which has a native GTE adapter.
E136C calls80026FE8(count,factor,destination,sourceA,sourceB). Its retail
MIPS words are present in temp1.c but excluded under XENO_PC_PORT; no native
source owner or symbol exists in the currently linked executable. This must
be implemented, not allowed to become a generated zero-return stub when
E1258 introduces a reference.

Read complete retail executable [80026FE8,8002709C),180 bytes, SHA-256
`df91dc9c5881d3488381d84c56ca367b3f5a46aa7c77877ae6e87450814eabaa`.
The first172-byte inspection omitted the final JR/NOP; the complete range
above is the authority. Factor is upper-clamped at32 by signed comparison,
then its wrapped left-shift7 is written to IR0. For each pixel, unsigned16
sourceB minus sourceA is computed per BGR555 channel mask (001F/03E0/7C00),
loaded into IR1/IR2/IR3, and passed through GPF12(0198003D). Masked GTE
results are added to sourceA channels, ORed together, and stored low16.
Do not substitute ordinary RGB interpolation, preserve bit15 arbitrarily,
or copy26F44's separate nonzero-pixel fallback into this routine.

The branch delay slot at27010 reads sourceB even when the count has reached
zero, including an initial zero count. A faithful host translation must
account for that trailing read rather than silently redefine the memory
contract. Negative counts are not clamped by retail; malformed runs are
not acceptance inputs. Next test should execute the complete retail leaf
against a native implementation sharing the real PsyCross GTE backend,
comparing output and complete GTE state, with source/destination alias cases.
Existing battle_gte_retail_test demonstrates the backend/bus/build pattern.

No production code changed in this dependency audit. The live game and
its binary remain unchanged. E1258 integration still awaits this native
leaf and a transparent callback-address contract; no default callback or
frame value was inserted.

### Type-1 color blend: native implementation and differential verification

Implemented func_80026FE8 in retail_leaf_adapters.c against the complete
180-byte executable range pinned above. color_blend_retail_test executes
that retail body and the separately compiled native translation with the
real PsyCross GTE backend. O0/O2/UBSan each pass115200 cases, comparing
the complete fixture and GTE register state: counts0..8, 256 pixel seeds,
ten signed factors including extremes, and five destination/source alias
arrangements. Six deliberately incorrect implementations are rejected:
clamp, channel direction, channel mask, GTE command, alpha preservation,
and omitted trailing source read.

The trailing-read negative control passes all115200 valid-memory cases,
then fails the dedicated count-zero/null-sourceB check. The real retail
body bus-faults; native O0/O2 SIGSEGV; the UBSan child intentionally reports
a null-pointer load and exits1. The runner requires that exact diagnostic;
it is expected invalid-input evidence, not a clean sanitizer execution of
that malformed case. Valid-memory cases pass under UBSan. Negative counts
and independent PS1 GTE hardware behavior are not established by this test.

Logs: pc_port/build_native/color-blend-retail-final.log and
pc_port/build_native/color_blend_retail_test/{O0,O2,UBSan,trailing}.log.
The existing retail-leaf regression also passes29 checks in
pc_port/build_native/color-blend-leaf-regression.log. Scoped diff whitespace
check passes. The live PID678654 executable still hashes
314e2100931873407fae7de672e245a1e6f0f9ab20dfe0e6079de7c36702eff3;
it was not rebuilt or replaced. Full integration and visible acceptance of
the new color blend remain NOT_RUN.

### Effect callback provenance recovered from the retail event path

Re-read archive6B9 sectors231361..231385; padded payload SHA remains
14395a9f54c124fed016f07906cc882b2a9256d5ade2d10eef99e10fe9366523.
At E613C the event loads unsigned script byte7 and calls E34BC at E6140.
Its result moves to t6 at E6168 and is stored as constructor stack argument
48 at E6254. E0A00 loads incoming stack48 through its frame+150, then
stores that exact word at effect+24 (E0B1C). This supplies the function
called indirectly by E1258. The constructor does not invent a callback.

Complete resolver [E34BC,E3534) maps selector1 to E08D4, selector2 to
E0938, selector3 to E0988, and every other value to E0850. That last branch
is an explicit retail default, not permission to default unknown native
callback addresses. No native owners for these five functions were found
in the current field source search; their translations and differential
tests remain necessary before the updater can use a host callback bridge.
Resolver SHA-256:
2fb20201f7d265633f6235f707532ae68c58acc866fab5c437170cef5753f73b.

Complete callback bodies and SHA-256 pins:

- [E0850,E08D4): ebff99d63bd52cb38f17d8f3e9525ccc5737ce288099b65b0f5acb00ea798a01.
  Calls8003F8CC with signed16 phase, adds4096, divides by signed16 argument1,
  adds argument2 and returns signed16. Preserve SDK trig and divide traps.
- [E08D4,E0938): f6a61aee076e45313301919d85d50f34e2e97d0864782fef87c76e87fea0b860.
  Signed16 phase/divisor quotient plus argument2, truncate signed16, return
  minus1 only when that signed result exceeds32.
- [E0938,E0988): e43f3ce8f4cdf522d12cbcdba2f1f5440b629e8b87b0dd0e38fa1d7e032e5f72.
  Argument2 minus signed16 phase/divisor quotient, return signed16.
- [E0988,E0A00): 92002d1b6fe561eb31a285fb7e618dddb64e7bdf59cad9d7b630d4ca24eefe9b.
  Signed16(32 minus phase/divisor quotient), lower-bounded by signed16
  argument2, then returned signed16; do not substitute an unsigned clamp.

All four bodies contain division exception checks; a zero divisor must not
be replaced with a convenient value. Callback mapping recovery is source
evidence only: E1258 and the actual DCEC8 renderer remain unintegrated.

### Retail effect callbacks implemented and instruction-tested

Added native E0850/E08D4/E0938/E0988 and selector E34BC in
pc_port/src/field_object_overlay.c. Their complete retail ranges are pinned
above and asserted by run_effect_callback_retail_test.sh. E0850 uses the
existing header-adapted rsin/8003F8CC entry; the others retain signed16
division, truncation and signed thresholds. All use the existing explicit
divide-exception adapter rather than substituting a nonzero divisor.
E34BC returns the exact guest address, not a truncated host function pointer.
The eventual E1258 indirect-call bridge must explicitly resolve these
addresses; no bridge or guessed unknown-address fallback was added here.

RED was confirmed as missing native owners after correcting an initial
fixture indentation warning without disabling the strict test warnings.
O0/O2/UBSan each pass2097152 arithmetic cases: all65536 phase bit patterns,
eight divisor/offset combinations, four callbacks, and alternating nonzero
upper phase bits. Both return values and trig boundary arguments/call counts
are compared with complete retail instruction execution. The controlled
trig result spans -4096..4096; this does not independently validate the
underlying trigonometric implementation or PS1 hardware.

Each mode also compares98304 signed selector inputs (-32768..65535) and
four exact retail BREAK locations for a divisor whose low16 bits are zero.
Native children must SIGILL with core dumps disabled. This is fail-stop
host exception behavior, not a resumable PS1 exception implementation.
Seven production mutations are rejected: unsigned phase, wrong trig bias,
32 boundary, subtraction direction, reversed lower bound, wrong default
selector, and divisor substitution. The last mutation is specifically
rejected by the exception test.

Logs: pc_port/build_native/effect-callback-retail-final.log and individual
negative controls under pc_port/build_native/effect_callback_retail_test/.
Effect-blend regression passes15120 cases in each O0/O2/UBSan mode and all
six existing mutations; log effect-callback-blend-regression.log in the
same build_native directory. Scoped diff whitespace check passes.

The live game remains PID678654 with its existing executable. Full port
rebuild, updater/renderer integration, and visible retail acceptance of
these additions are NOT_RUN. Next is the complete E1258 update path with
an explicit callback-address adapter, followed by its actual DCEC8 caller;
the simpler DCC3C renderer is not a substitute for that caller.

### Complete effect updater E1258: native body and callback-address adapter

Implemented [801E1258,801E165C) in field_object_overlay.c against the
1028-byte body with SHA bde5842f0f7b095691ed3a3be6284607384e7883073524e62cefd7bdfb2ebeff.
FieldEffectCallback resolves the four previously proven field-overlay guest
addresses explicitly. Other addresses emit an unresolved diagnostic and
trap; no guessed callback/frame and no raw guest-to-host function cast.
This is a transparent fail-stop adapter for unresolved execution, not proof
that every possible callback address has been enumerated in all game data.

E1258 retains the active gate, wrapped phase increment with ticks+1,
signed16 callback result, negative-result cleanup, repeated-frame gate,
type0/1 GTE blend calls with their exact source order and upload gate,
type4/5 matrix helpers, and linked-effect update. The linked owner's type
selects C-buffer/width-stride versus 1C-buffer/six-byte-stride reads.
Rectangle differences and extents truncate to signed16; they are not
replaced by a generic clipped rectangle intersection. Pixel copies remain
sequential, including overlapping buffers.

Added effect_update_retail_test/runner. RED: missing native owner. The
runner compares complete retail updater, callback, cleanup and matrix-helper
instruction execution with native functions. GTE blend, trig, LoadImage
and HeapFree are explicitly controlled boundaries, recording their arguments
and effect-owner snapshots; no physical GPU upload or free is performed.
Complete fixture memory and return values must also agree. This suite is
not independent GTE/hardware, actual renderer, or end-to-end gameplay proof.

O0/O2/UBSan each pass25920 cases: four callbacks, types0..7, absent/inactive/
active linked owners, six phase/state configurations, five tick values
including signed extremes, and nine rectangle layouts. Layouts include
signed16 coordinate-wrap cases with deliberately valid backing addresses;
the high-coordinate cases use the linked pixel path, while ordinary layouts
cover both linked pixel and matrix paths. State5 also overlaps sourceC with
the linked destination. Unknown-address host execution must SIGILL in a
core-disabled child, independently of the valid retail comparisons.

Verification failures were handled before acceptance: the initially copied
runner had a stray unfinished mutation loop (removed by completing its own
loop); expanded high-coordinate matrix-source inputs addressed outside the
fixture (retail read fault, fixed by using the valid linked pixel path for
those cases); and a repeated-frame mutation survived because the fixture
never supplied its actual prior result (fixed to provide frame4). No
production behavior or assertion was weakened for these test repairs.

All11 negative controls now fail: tick increment, active gate, repeat gate,
cleanup omission, type1 source swap, upload gate, linked-type selection,
matrix row stride, dirty flag, invented rectangle clamp, and unknown-callback
fallback. Logs: pc_port/build_native/effect-update-retail-final.log and
effect_update_retail_test/*.log. Cleanup regression also passes12288 cases
per O0/O2/UBSan plus its six negative controls, recorded in
pc_port/build_native/effect-update-cleanup-regression.log. Scoped whitespace
check passes.

The running game/executable was not changed. Full port build and visible
acceptance remain NOT_RUN for these additions. E1258 now exists but its
actual DCEC8 renderer caller, constructor/event ownership, and broader
animation VM integration remain unfinished. Do not equate this helper
closure with the scripted opening cutscene or full port being complete.

### Actual renderer integration audit: ordering and remaining owners

Revalidated archive6B9 payload SHA14395a9f54c124fed016f07906cc882b2a9256d5ade2d10eef99e10fe9366523.
Complete renderer [801DCEC8,801DDBF8) is3376 bytes, SHA
bcbea29d659d3aa05fe447139193d17267308952b37b8151181b105796f173eb.
This turn inspected its entry, direct calls, and full tail D440..DBF8;
it is not yet a fully translated or tested renderer. Its object+34 gate
skips the entire body, not only mesh submission. Beyond the mesh loop:

- Object byte10D counts 0x24-byte particle owners at pointer114. Active
  owners transform anchor arrays and per-node records with root/node
  matrices and GTE MVMVA before calling E22F8. Root yaw and global8698
  produce offsets; object3E supplies the vertical offset.
- Byte10E counts 0x30-byte effect records at pointer118. Each receives
  E1258(effect, incoming stack argument10). The actual 7D14 caller supplies
  that argument as1; this is distinct from its animation tick accumulator.
- Byte10C counts 0x70-byte trail owners at pointer110. Negative first
  halfwords skip owners. Byte2 selects projected XY history versus world
  XYZ history. The ring cursor decrements modulo8; the XYZ branch uses its
  low bit. Age, lifetime and existing pointer determine E0248 allocation.
  Pool sentinel behavior and the ring buffer cannot be replaced by NULL
  skipping or a generic particle effect.

Retail7D14 has distinct ordered passes: compute its capped accumulator and
tick count; snapshot root translations and call five-argument36BC for all
nonnull slots; run37D0 attachments; runE1880; set color matrix; calculate
movement deltas and callDCEC8; finally process the auxiliary pool. Current
native7D14 instead skips hidden/small/large owners, binds actor positions,
hardcodes one animation tick, then submits meshes directly. This inspection
does not authorize retaining those differences as a retail implementation.
Replacing that path must close the real owners, not wire the simplerDCC3C
helper or insert a standalone E1258 call at a convenient location.

Newly traced prerequisite [801E1880,801E1A14),404 bytes, SHA
66166223611c2503d903567f301fe314e8a346c3124b4a4cf1f3856589fa2be9:
updates exactly two 20-byte records rooted at D8648. Record+6 is the active
halfword; +10 is signed object slot, +12 signed child index (hex offsets).
For an active record with nonnegative slot and nonnull table[slot], compose
root+C with root+A8+child*7C into scratchpad0, set rotation/translation,
MVMVA(00480012) the vector at record+8, and store low16 MAC1/2/3 into
record+0/2/4. Otherwise active records copy source+8/A/C directly to+0/2/4.
Inactive records remain untouched. No retail upper slot clamp was found;
the eventual test must provide addressed table/node backing, not invent
one. E1880 has no native owner yet and is the next bounded prerequisite.

The particle renderer's complete boundary is [801E22F8,801E3438),4416
bytes, SHA93902b9a120ae364d9084a84c16cc079481eb89acc84a62ec13e8f94f3e43445.
The earlier360-byte range endingE2460 is only a prefix, not its full hash.
This turn inspected its entry, direct calls, normal-accumulation section,
and epilogue; remaining internal paths still require detailed translation.
E3438 is a separate destructor. Particle direct dependencies include
SquareRoot0 three times and VectorNormal at80048D7C, plus matrix setup.
The symbol map confirms48D7C is VectorNormal, not SquareRoot12.

VectorNormal in psyq_compat.c currently calls VectorNormalWork, unlike the
already restored VectorNormalSS that explicitly preserves GTE state.
This is a fidelity gap to investigate with the actual VectorNormal retail
body and caller register usage; this audit alone does not prove a visible
symptom or establish the exact repair. Do not assume matching vector output
alone establishes complete hardware-boundary behavior.

No production code or running executable changed in this integration audit.
The live game PID678654 remains operator-visible; source findings above are
not new runtime or human acceptance. Renderer, shared pass, particle owner,
VM and constructor integration remain incomplete.

### Shared transform E1880 implemented against complete retail instructions

Added func_801E1880 to field_object_overlay.c, preserving the complete
404-byte body pinned above. It handles exactly two packed records, active
halfword gates, signed object slots and child indices, root+C/child+A8+7C*n
composition, scratchpad0, rotation/translation setup, MVMVA(00480012), and
low16 MAC1/2/3 output. The fallback copies record+8/A/C to+0/2/4 only for
active records without a linked object; inactive records are untouched.
The input includes both packed vector words, including its padding word.
No arbitrary ten-slot upper bound or child clamp was introduced.

Added shared_transform_retail_test/runner. RED was missing native owner.
O0/O2/UBSan each pass16128 cases: all four two-record active masks,
slots-2..15, children-1..5, object-present/absent tables, and16 matrix/vector
seeds. Deliberately addressed sixteen-slot backing verifies absence of an
invented ten-slot clamp; it does not assert that every retail caller owns
sixteen slots. Child-1 addresses root+2C, as the original arithmetic does.

Tests compare complete records, unchanged owner/table/node fixture, full
scratchpad, ordered matrix boundary input snapshots, and entire GTE register
state against complete retail instruction execution. CompMatrix is a
controlled boundary; register transfers and MVMVA use the real shared
PsyCross backend. These results do not independently prove matrix SDK math,
PS1 GTE hardware fidelity, or correct game-scene integration.

Seven mutations are rejected: one-record count, bypassed active gate,
invented slot cap, wrong child matrix base, wrong vector input word, wrong
GTE command, and zeroed fallback output. Log:
pc_port/build_native/shared-transform-retail-final.log, with individual
controls in shared_transform_retail_test/. Existing simple-draw regression
passes1536 cases per build mode and its eight controls; log
pc_port/build_native/shared-transform-draw-regression.log. Scoped whitespace
check passes.

The live game remains unchanged. E1880 is not yet called by the provisional
native7D14; full build and visible integration acceptance are NOT_RUN.
The remaining actual renderer/particle path and VectorNormal boundary must
be closed before replacing the broader caller as retail-accurate.

### VectorNormal: repaired fullword wrapper and GTE side effects

Retail entry [80048D7C,80048DA8),44 bytes, SHA
110dd369388dc66e9dc7a19b713c1a23b4644224cedf24e837a3b88fe9db9149,
loads three full words, calls the shared core [80048DD8,80048E94),188
bytes, SHA7e6f8fdab6570a20c556da4d6e50ae83ff8934ea0592ba60da26d5a76e4e4db6,
then stores three full words. The intervening SS wrapper is not part of
this call's executed body. Added both pins to the existing GTE runner;
its existing table-region pin covers the reciprocal-square-root table and
the zero-vector prefix lookup.

Extended battle_gte_retail_test before changing production. Initial test
compilation needed an explicit VectorNormal declaration. Its loader also
needed extension from48DA8 down to48D7C; without that, a zero-filled entry
fell through to the SS wrapper and produced invalid comparison evidence.
After correcting the loader, RED reproduced a real mismatch for
(-32768,0,0): equal squared return40000000 but unequal complete GTE state.

Replaced only VectorNormal's call to arithmetic-only VectorNormalWork with
the retail SQR0, signed-sum exception boundary, LZCS/LZCR, table lookup,
GPF0 and arithmetic-shift sequence. Inputs are captured before any stores,
and stores remain fullword. Zero retains the retail pre-table halfword1C6C
instead of bypassing GTE execution. VectorNormalS still uses the old helper;
it has not been silently declared repaired. Existing VectorNormalSS remains
unchanged and is regression-tested.

O0/O2/UBSan each pass786432 fullword normal cases: all65536 signed lowword
values along each axis and a mixed vector with nonzero high input bits,
each with separate, in-place and four-byte-overlapping outputs. Complete
guarded48-byte memory, return value and GTE state agree with retail
instructions using the same PsyCross backend. Three near-overflow vectors
also pass per mode. Three overflowing native inputs must SIGABRT with core
dumps disabled; the test interpreter does not model the retail signed ADD
exception, so this is explicitly a separate host fail-stop check, not
retail exception parity.

The full existing battle GTE suite passes, including524288 SS cases,
262144 Square0 cases, magnitude, matrix and projection/alias checks.
All four new mutations are rejected: incorrect LZCS input, zero-vector
lookup value, GPF scale command and z-output store. Existing Square0 and
magnitude negative controls also pass. Evidence:
pc_port/build_native/vector-normal-retail-red.log and
pc_port/build_native/vector-normal-retail-final.log; individual fault logs
under battle_gte_retail_test/. Scoped whitespace check passes.

This closes the VectorNormal wrapper mismatch required by E22F8, but does
not independently prove PsyCross hardware accuracy or integrate E22F8 into
the native renderer. No live executable replacement or full port build was
performed; visible acceptance of this change remains NOT_RUN.

### E22F8 remaining branches traced: linked geometry, normals and packets

Revalidated the complete E22F8..E3438 hash
93902b9a120ae364d9084a84c16cc079481eb89acc84a62ec13e8f94f3e43445.
Read the previously outstanding motion/constraint and packet branches, not
just their direct calls. The routine uses linked geometry and triangle
normals; calling it a generic particle emitter must not lead to inventing
particles, timing or a new simulation. Seven arguments are observed:
owner, signed16 offset vector, matrix, ordering table, buffer selector,
fullword scale, and signed16 height limit. DCEC8 supplies those owners and
arguments; no host animation/event defaults are justified.

Entry returns when owner+14 is zero. Otherwise code byte comes from the
first face packet at owner20->+F. Signed owner4 counts chain pointers at
owner1C. Each chain follows24-byte records until record0 is zero. For each
record, the normalized direction is next-position(+1C/1E/20) minus current
position(+4/6/8), plus incoming offset; Y additionally includes signed
record2. Wrapped squares feed SquareRoot0; nonzero distance divides each
wrapped delta<<8 by that distance, while zero distance explicitly produces
zero components. Multiply signed record0 by each component and fullword
scale with intermediate low32 wrapping, then divide by2^20 truncating
toward zero and add current coordinates. Store low16 into the next record's
position. Clamp next Y only at the initial update against the signed height
argument, exactly where the instructions place it.

Signed ownerA counts16-byte constraint records at owner18. Their signed
positions are+8/A/C and radius+E. Radius*scale is low32, biased for negative
division by4096, then truncated signed16. If the distance from updated next
position is below that radius, compute the boundary point (or constraint
position directly at zero distance), normalize its displacement from the
current chain record, and recompute the next position with the same record
length/scale sequence. The initial height clamp is not repeated after these
constraint updates. Preserve zero-distance branches and actual divide
exceptions, not epsilon normalization or guessed projection corrections.

Normal pass: dereference the first chain pointer as the common vertex base.
Signed owner8 counts24-byte vertex records; clear halfword+A and words+C/10/14.
Signed owner6 counts88-byte face records at owner20, with three signed16
vertex indices at0/2/4. Form two edge differences, run GTE OP0(0170000C),
divide its three MAC outputs by8 truncating toward zero, and call the now
restored VectorNormal. Accumulate that result into each indexed vertex's
three normal words and increment its halfword count, including repeated
indices. Finally divide each vertex's accumulated words by its signed16
count. There is no zero-count fallback: uncovered vertices reach the
explicit retail division exception path. Fixtures must either cover the
vertices or deliberately test that fault, not silently supply normal zero.

Packet pass sets the incoming rotation/translation matrix. Packet pointer
is face+8+buffer*40. RTPT(00280030) projects the three vertices; FLAG bit
00040000 branches straight to the loop-count update atE33F0. Crucially,
face and packet pointers advance only on successful submission, atE33A4,
E33A8 andE33E0. A skipped face is therefore retried for remaining loop
iterations with the same pointers. This is observed disassembly behavior;
do not rewrite as an unconditional indexed face loop or claim it is a host
bug without separate authority.

For accepted projection, NCLIP(01400006) determines the signed orientation,
SXY outputs are stored into packet+8/14/20, and AVSZ3(0158002D) supplies OTZ.
OTZ is arithmetic-right-shifted by D_80050100 using MIPS shift-count masking.
Negative orientation selects owner RGB+C/D/E and the vertex normal; other
orientation selects RGB+F/10/11 and negated low16 normal components. Each
vertex runs NCCS(0108041B); RGB2 goes to packet+4/10/1C. Command byte is the
first face's previously loaded code, not an invented packet-type constant.
Ordering-table linkage preserves the top byte and replaces only low24
address bits in the packet and OT slot. No guessed OT bounds clamp appears
in this body. Tests must cover both buffers, orientations, projection skip,
normal averaging, and preserved command/address bits.

Current native ownership still reports that extended objects with+110/+114/
+118 are not constructed by its provisional loader; the destructor refuses
those foreign owners. This routine cannot honestly be integrated by merely
adding a draw call to those empty owners. Next is a complete E22F8 fixture
and translation using the source-backed layouts above, then constructor,
destructor and actual DCEC8/caller integration. This turn changed only the
evidence ledger; no production behavior or running executable changed.

### E22F8 full native translation and first complete-body differential suite

Added func_801E22F8 in field_object_overlay.c against the complete4416-byte
body pinned above. The implementation covers linked-chain movement and
constraints, normal clearing/accumulation/averaging, incoming matrix setup,
RTPT/NCLIP/AVSZ3/NCCS, both packet buffers and low24 OT linkage. Small local
helpers express wrapped distance arithmetic, signed division, next-position
stores and GTE vector loads; they are not alternate host simulation systems.
The projection-rejection branch retains the retail non-advancing pointers.
No arbitrary object/vertex/OT clamp, default normal or forced state added.

Added linked_geometry_retail_test/runner. The initial retail-only baseline
completed768 fixtures then failed for the missing native owner. Native
O0/O2 now match those complete-body executions; after the projection UB fix
below, UBSan matches too. Fixtures cover disabled/enabled owners, motion
off/on, zero/one/two constraints, two packet buffers, eight orientation/
depth/layout variants and scales0/4096/-4096/8192. Four vertices are covered
by two faces. Per mode the retail path executes768 RTPT commands,108 taking
the projection rejection branch. Complete owner/geometry/packets/OT memory,
ordered SDK boundary snapshots and full GTE state are compared.

SquareRoot0, VectorNormal and matrix setup are controlled test boundaries;
the GTE commands use the real shared PsyCross backend. The normal boundary
snapshot covers only its three input words, not an unconsumed stack padding
word. The individually restored production SDK functions are not invoked by
these controlled geometry comparisons, so this is not an end-to-end SDK,
independent hardware or visible scene proof. Malformed chains, uncovered
vertex division traps, extreme-index/alias domains and exhaustive geometry
coverage remain additional gates, not claimed PASS by this768-case suite.

Six mutations are rejected: movement divisor, face-normal divisor, buffer
selection, unconditional pointer advancement after projection rejection,
orientation colors and discarded OT high byte. Log:
pc_port/build_native/linked-geometry-retail-final.log; individual cases and
controls under linked_geometry_retail_test/. The complete battle GTE
regression suite and existing controls also pass in
pc_port/build_native/linked-geometry-gte-regression.log.

### Projection negative-translation UB exposed by the geometry fixture

The initial UBSan run stopped at PsyX_GTE.cpp:384: signed negative C2_TRZ
left-shifted12 in GTE_RotTransPers. Replaced the three translation shifts
with signed64 multiplication by4096. Signed32 translation scaled this way
fits signed64, preserving the intended fixed-point arithmetic. No camera
offset, clipping rule or visual adjustment was introduced. Added tracked
psycross_gte_projection_signed_translation.patch and registered it beside
the earlier MVMVA translation patch in build_port.sh.

The first patch had insufficient context for the git application check;
adding its actual neighboring lines fixed that packaging issue. Reverse
application check now passes against the vendor source. A test-only source
variant restoring the original negative Z shift is compiled under UBSan
and must fail with the exact negative-left-shift diagnostic. Its initial
compile needed the vendor source-header include directory; after correcting
that test path, the negative control passes. The ordinary geometry run
remains UBSan-clean. Other untested GTE operations are not declared UB-free.

No live executable was rebuilt or replaced. Full build, constructor/
destructor ownership, actual DCEC8 integration and visible scene acceptance
remain NOT_RUN/incomplete. E22F8 now exists as source, but wiring it into
empty extended owners would still not implement the retail scene.

### Geometry ownership: retail destructor E3438 restored

Read complete [801E3438,801E34BC),132 bytes, SHA
40f036f5def72dd96f6fdf1357db7bc47c35e26655b956a79bdbba78fd3794a5.
Implemented func_801E3438: zero owner14 returns untouched; otherwise free
owner14, the first pointer in owner1C's table, the table itself, owner20,
and optional nonzero owner18, in that order. Clear only owner14 last.
Reload owner pointers across heap calls. Do not clear the whole record or
skip the unconditional table/geometry frees merely because their values
are zero; those are differences from the supplied retail body.

New linked_geometry_cleanup_retail_test/runner first failed for the missing
native owner. O0/O2/UBSan each pass512 cases, comparing full owner/table
memory, ordered free addresses and owner snapshots at every call. Coverage:
active/inactive, all eight optional/data pointer masks, sixteen distinct
pointer seeds, and a controlled first-free callback changing the table
pointer to verify the later reload. Heap calls record only, not actual free;
callback mutation is a boundary-observation fixture, not asserted normal
allocator behavior. Four mutations fail: active gate, freeing table instead
of its first target, unconditional optional free, and clearing the whole
record. Logs: pc_port/build_native/linked-geometry-cleanup-final.log and
linked_geometry_cleanup_retail_test/*.log. Geometry renderer regression
passes its three modes, six mutations and original-projection-shift UBSan
negative control in geometry-cleanup-render-regression.log. Scoped diff
whitespace check passes.

Also read complete parent destructor [801E8030,801E8330),768 bytes, SHA
e15a7ea45c5adf850542501ced1817d2f6bda71e2b355251dd2381cb3c3d95c7.
Its extended cleanup is ordered after node/track cleanup: free trail owner110
when byte10C is nonzero; iterate byte10E effects at118 with30-byte stride,
callingE165C then free the array; iterate byte10D geometry owners at114 with
24-byte stride, callingE3438 then free that array. Finally free the parent
and clear its global slot. Earlier branches include conditional registry
cleanup and distinct handling for slots8/9, so the existing provisional
8030 must not simply call the new destructor without closing those owners.

This addition is not yet wired into that full parent destructor. Geometry
construction, real parent teardown and DCEC8 integration remain unfinished.
No live executable replacement, full build or visible acceptance occurred.

### Geometry constructor E1A14: allocation-failure boundary resolved

Constructor [801E1A14,801E22F8), 2276 bytes, SHA-256
d2f25c7c9728b4929a214cf8dbc4db756e45d4d0ca75d405356eae1a130f3f03.
Re-extracted from the pinned overlay sectors, not inferred from the native
renderer. Native constructor implementation and its differential test are
still pending. The following failure-path instructions were re-read directly:

| Failed allocation | Retail operations before returning, if calls return |
| --- | --- |
| Chain-pointer table | Clear owner14; call HeapFree(0) at801E1C30 |
| Vertex block | Clear owner14 in call delay slot; HeapFree(0) at801E1C9C; reload and free owner1C at801E1CB0 |
| Face array | Clear owner14 in call delay slot; HeapFree(0) at801E1DF4; reload and free owner1C at801E1E08; free vertex base in fp at801E1E10 |

These paths do not free the previous anchor pointer. Replacing the null
arguments with that pointer, or adding conventional rollback, would differ
from the supplied retail instructions. Controlled heap-call fixtures must
label their returning behavior rather than assert these paths always return
in actual gameplay.

The reason is now resolved at the allocator boundary. Complete HeapFree
[800320E8,8003218C),164 bytes, SHA-256
172bdbe744af67620d606319e150a97318f12d59d115f81d13748c22a7bcf7ac,
read from disc/SLUS_006.64 using file offset RAM address minus8000F800:
when a0 is zero, GP+1C0 nonzero returns1; otherwise it records allocation
size0 and caller ra-8, then calls80019ACC with argument83 at80032124.
The symbol map identifies GP+1C0 as g_HeapIsErrorHandlerOff at80059330
and80019ACC as MainLoop. The current decompiled memory.c:HeapFree preserves
that branch and calls MainLoop(ERR_HEAP_FREE_NULL), defined as83 in memory.h.
MainLoop's retail entry independently dispatches a nonzero error argument
to80019EF8. This is not the libc free(NULL) contract.

Native debug provenance differs explicitly: XENO_PC_PORT sets the captured
MIPS caller address to0, so the recorded source address is not retail-exact.
Do not claim complete heap/error-path parity from the matching branch alone.
No error-handler flag was forced, no allocator behavior changed, and no
allocation failure was injected into the live game.

Next implementation gate: compare constructor allocation sizes, complete
owner/anchor/vertex/face/constraint memory, SDK packet calls, and ordered
failure calls against the retail body. Exercise each allocation failure with
a clearly controlled returning heap boundary, and separately test the real
allocator's disabled-handler return and enabled-handler error dispatch.
Preserve signed halfword counts, declared segment total rather than a guessed
sum, and retail divide faults; do not add fallback geometry or clamped counts.

Live debugger678634 and game678654 were confirmed running during this trace.
Canonical executable SHA remains
314e2100931873407fae7de672e245a1e6f0f9ab20dfe0e6079de7c36702eff3.
This increment is source evidence only: no build or visible acceptance.

### Geometry constructor E1A14 implemented and differentially exercised

Added the complete native constructor body to field_object_overlay.c using
the preceding2276-byte retail pin. It creates anchors, the per-chain pointer
table,24-byte vertices including terminal entries,88-byte paired faces with
two40-byte GT3 packets, and optional zeroed16-byte constraints. Source
coordinates and lengths are unsigned halfwords; signed offsets and low-word
products precede truncating division. Owner counts retain their retail
halfword storage and signed consumers. Texture page origin uses signed
division toward zero, while UV stores wrap to bytes. Both front and back
colors are retained. Failure paths retain the documented null frees and
reload the table pointer after the first heap call.

New linked_geometry_constructor_retail_test.c and runner first reported
GEOMETRY CONSTRUCTOR FAIL missing native owner. With the implementation,
O0/O2/UBSan each pass1080 cases: chains2..4,12 data seeds, five scales
(4096,-4096,8192,0,INT_MAX), success plus each of five allocation-failure
positions. Unequal per-chain lengths, high-bit source values, negative
texture coordinates/dimensions, positive/zero/negative constraint counts,
and preserved allocation padding are included. Compare the full fixture
(owner,source and all allocation blocks), requested sizes, ordered frees,
and owner snapshots at heap calls. Heap calls are controlled and returning;
the test does not execute the allocator/error handler.

SDK GetTPage/GetClut/SetPolyGT3 execute retail instructions on both sides:
shared boundary, not independent native LIBGPU proof. Loaded SDK window
[80043A1C,80043D1C),768 bytes, SHA-256
d7e5c65695cfdae806e08f4a419ba9d4fc7e1503da10abb8d728fa7e71958ad0.
The runner pins this window as well as the overlay. Six deliberate mutations
are rejected: signed anchor input, substituted anchor free, signed length,
nonzero terminal length, wrong second-triangle V, and wrong back color.
Result log: pc_port/build_native/geometry-constructor-final.log; mutation
logs are in linked_geometry_constructor_retail_test/.

Destructor regression passes512 cases per mode and four negative controls
in geometry-constructor-cleanup-regression.log. Renderer regression passes
768 cases per mode, six mutations and the original signed-projection-shift
UBSan control in geometry-constructor-render-regression.log. No full port link, live
binary replacement, natural scene traversal, or visual acceptance occurred.
Remaining tests include chainCount1/zero-minimum divide faults, malformed
declared counts, allocation aliasing and real allocator error dispatch.
Constructor source existence does not close the actual parent constructor
or DCEC8 caller integration; those remain required for retail gameplay.

### Constructor fault coverage and parent integration trace

Extended the constructor fixture with chainCount1 and two chains whose
minimum segment count is0. Retail must stop at BREAK7 at801E1EF0 and
801E1F6C respectively; native child processes must terminate with SIGILL
(the explicit host trap adapter, not resumable PSX exception handling).
Core dumps are disabled only in these test children. Test compilation
initially exposed PSYQ/Linux uid_t and gid_t collisions; the fixture now
uses the established test-local psyq_test_* aliases around the included
production source. Production headers and runtime behavior are unchanged.

O0/O2/UBSan each pass1080 normal/failure cases plus both divide-fault
cases in pc_port/build_native/geometry-constructor-faults.log. Eight
mutations are rejected, including new early-return substitutions for each
divide fault. Scoped whitespace and shell syntax checks pass.

Read complete parent [801E742C,801E7D14),2280 bytes, SHA-256
a02d5125ca62ea1540800886a36327f5fad902e136622218ef8349dc40acdc75.
Its only direct JAL to E1A14 in the supplied overlay is801E7B34. Important
integration details, not yet implemented in the parent:

- Flags bit1 gates relocation of the fourth-argument texture archive and
  its+10 table; bit4 gates relocation of the third-argument model archive
  and its nested tables. The provisional native742C currently reverses
  these relocation gates. This is a source-confirmed discrepancy, not a
  demonstrated cause of a specific screenshot artifact.
- The texture+10 table's+4 pointer supplies metadata. Metadata byte12 sets
  parent10D, which controls allocation of count*36 bytes at parent114.
  Owners advance by0x24 (36 decimal), not24 decimal. The metadata cursor
  starts at+14 and is advanced through each descriptor and its constraints.
- Owner0 receives the first descriptor halfword. Signed descriptor fields
  supply bias, scale and XYZ offsets. Constraint count is preloaded from
  descriptor+22. Texture X/Y and CLUT X combine descriptor halfwords with
  caller placement offsets, then truncate to signed16. CLUT Y comes from
  the caller. Six color bytes occupy successive two-byte source slots.
- The geometry source pointer comes from texture+10 table+8; the table
  cursor advances4 for each geometry owner. After E1A14, each constraint
  consumes five source halfwords written in order to offsets6,E,0,2,4 of
  its16-byte destination record. Constructor-zeroed XYZ at8/A/C remain
  for the later update path.
- Parent initialization also calls E8510 for trails, allocates count*48
  effect records at118, initializes their1A/4/8/C fields, binds scripts
  through E3534/E35D0, and may clone/re-register the node block. The current
  blanket memset, fallback model blob and private pointer table are not
  substitutes for those retail owners and transitions.

Next integration work must close those dependencies and compare the full
parent body before replacing742C. No flags, object state, live scene or
running executable were changed by this trace.

### Parent trail initialization E8510 restored

Read and implemented complete [801E8510,801E8590),128 bytes, SHA-256
0764da3a5f31586321a67a62de035066a3c1694ce28e81ecec92308e09dc580d.
Nonzero parent byte10C requests count*112 bytes with flags0. Each record
gets halfword0=-1 and word8=0; other bytes remain untouched. Store the
allocation pointer at parent110 after initialization. Zero count returns
without modifying that pointer. Reload the count after allocation and in
the loop; no null-allocation recovery is present in the supplied body.

trail_init_retail_test/runner first failed with missing native body. It now
passes3072 cases in each of O0/O2/UBSan: every byte count0..255, four
nonzero memory patterns, and three allocator boundary behaviors (unchanged
count, count cleared, count decremented). The latter two are controlled
observations of reload semantics, not claimed normal allocator behavior.
Compare full parent/record memory, allocation size/flags/count and parent
snapshot at allocation. Five mutations are rejected: zero-count gate,
allocation size, leading sentinel, reset offset and cached pre-call count.
Log: pc_port/build_native/trail-init-final.log. Allocation failure and
allocator/record aliasing are not covered; this is not heap parity proof.
Geometry constructor regression passes1080 cases and two faults per mode,
plus eight mutations, in trail-init-constructor-regression.log. Scoped
production whitespace and new-runner shell syntax checks pass.

Also read complete E3534 [801E3534,801E35D0),156 bytes, SHA-256
dcca05b1af95b39ec18df4bb4a29aed5b33da46091e9bbd34f20c607882edc7b,
and E35D0 [801E35D0,801E36BC),236 bytes, SHA-256
7b59dc488b040e5b2b64a898276871a668e7a1795002fb826e140ac020ddfb9b.
E3534 takes four arguments, ignores a1, and stores a2/a3 as clip table/aux;
its state stores match the existing OvlyClipInit source on inspection,
not yet a new differential proof. E35D0 likewise takes four arguments:
a2 is passed to E39F0 and a3 is the clip index. Its bind branch uses signed
index<50, otherwise auxiliary table indexing, resets42/40/50/54/4C/23,
copies D801E863C into10A, and calls E39F0(obj,a2,-1,1,0). A new wrapper
must not silently substitute the current partial OvlyClipTick interpreter.

E8510 is available for the full parent replacement but not wired into the
provisional742C. Full parent ownership, full script interpreter, actual
renderer integration, port link and human-visible acceptance remain open.
The live game and executable were preserved.

### Retail script reset/bind entry points E3534/E35D0

Implemented both entry points with the preceding retail hashes and four
arguments. E3534 reuses the existing reset stores through OvlyClipInit;
E35D0 preserves null-object returns, queue-byte increment/saturation,
source-ID and clip-byte writes, signed clip-index table selection, state
resets and E39F0(obj,context,-1,1,0) dispatch. No clip-address heuristic,
index clamp or auxiliary-table fallback was added. Low clip addresses in
the fixture deliberately expose the old host adapter's rejection heuristic.

clip_bind_retail_test/runner first failed for absent native entry points.
O0/O2/UBSan each pass16 reset cases and16384 binds. Coverage: all256 queue
bytes, separate/aliased objects, four null-pointer combinations, and indices
-128,-1,0,1,79,80,81,255. Compare full fixture memory plus all five VM call
arguments and the destination's complete snapshot at the VM boundary.
E39F0 is controlled/recording in this test: no script opcode execution or
whole-VM fidelity is established. Six mutations are caught: wrong reset
auxiliary argument, queue limit, unsigned index comparison, auxiliary
table offset, phase reset and VM limit argument. Log: clip-bind-final.log
under pc_port/build_native; individual controls are in clip_bind_retail_test/.

Build inspection found that ordinary unresolved references can become
returning generated stubs. E39F0 is therefore an explicitly weak dependency
with a diagnostic abort if missing, so its absence does not request such a
stub during the trial link. This is a host missing-implementation boundary,
not retail error behavior or an interpreter implementation. A dedicated
test build omits the VM definition and checks SIGABRT plus the diagnostic.
Do not wire this bind entry into gameplay until the full interpreter exists.

Trail regression passes3072 cases per mode and five mutations in
clip-bind-trail-regression.log. Full port link, parent replacement, script
interpreter execution and visible scene acceptance remain NOT_RUN/incomplete.
No live executable was replaced.

### Full clip-VM dispatch inventory: replacement scope made reproducible

Added tools/scripts/audit_field_clip_vm.py, which reads only the pinned
retail sectors and emits address/hash metadata to stdout. Run:

```
python3 tools/scripts/audit_field_clip_vm.py disc/disc1.bin
python3 pc_port/tests/test_clip_vm_inventory.py
```

The VM body remains [801E39F0,801E59D4),8164 bytes with the prior SHA.
Its jump table is [801DC040,801DC204),452 bytes, SHA-256
af846cb60b1b8b9f6959042945ebabc40c84f0c189789b34948e3a28ee409e2c.
It has113 opcode slots and81 distinct entry addresses. Exactly29 opcodes
dispatch directly to801E5974, the continuation block. There are67 lexical
JAL sites targeting38 distinct functions and no JALR sites in this body.
These counts are not reachability, handler independence or dependency
closure proofs; the inventory explicitly says so. Four tests pass, covering
all slots, selected aliases/addresses, changed table bytes, changed VM bytes
and truncated input rejection. The first test run failed because the audit
tool did not yet exist. A manual commentary count of30 continuation opcodes
was corrected to29 after enumeration.

Re-read the entry/physics prefix, dispatch and epilogue alongside the
current OvlyClipTick. Its seven explicit opcode values are00,01,0C,10,13,
17,21; all other in-range values advance using s_clipExtra. That is a
partial host adapter, not a translation of the81 retail entry addresses.
Even those seven explicit cases are not certified by their presence.
Specific source-confirmed mismatches that constrain the full replacement:

- Opcodes02/03 set a local flag; after saving IP, retail calls800796F4 when
  that flag is set. Merely advancing IP omits the post-loop side effect.
- Opcode21 writes obj3C, then waits if caller argument2 is-1 or has bit0
  set; otherwise it continues. The current adapter always waits there.
- Opcode17 calls E8030 and follows retail exit flow; the adapter only
  rewinds the instruction pointer and stops.
- Pose opcode10 resolves data through E6910 and calls DEF10. The adapter
  substitutes its own lookup and pose application, so apparent pose output
  does not prove track setup or update semantics.
- Before dispatch, retail handles distance/height/timer continuation
  pointers at obj4C/54/50 and optional E63A8 targeting. Those are additional
  control-flow semantics beyond sequential instruction-length skipping.

The dispatch inventory is the checklist for the full native E39F0 rewrite,
not a substitute interpreter. Opcode bodies and their dependencies still
need semantic translation and differential execution. No game state,
partial-VM behavior, full binary, or live session was changed this turn.

### Clip-data lookup E6910 restored

Implemented complete [801E6910,801E6974),100 bytes, SHA-256
8419a785639aafafb67573a38a3fb70a9efbdee1ecb90d7cbc798c17011f8fd2.
Clear the caller's output word first. Index low byte FE/FF selects
obj2A&7F and returns obj2A&80 in that output (128, not boolean1). Other
indices use their low byte. Values below40 load obj14+index*4+4;
others load obj18+index*4-FC. Preserve 32-bit address arithmetic and store
ordering; no clip-address heuristic or null-table fallback was introduced.

clip_lookup_retail_test/runner first failed with missing native body.
O0/O2/UBSan each pass196608 cases: all256 index bytes, all256 object
selector bytes, and three output locations (separate, overlapping obj28
including selector2A, and primary-table entry1). Arguments carry high bits
A55A0000 to verify low-byte selection. Compare return value and entire
fixture memory against the complete retail instruction body; no callee
mocking is involved. Six mutations are rejected: omitted output clear,
FE default gate, booleanized flag, table boundary, primary offset and
secondary offset. Log: pc_port/build_native/clip-lookup-final.log.
Clip reset/bind regression passes all three modes, six mutations and the
missing-interpreter diagnostic check in clip-lookup-bind-regression.log.
Scoped whitespace and runner shell syntax checks pass.

Read the next pose dependency [801DEF10,801DF0B4),420 bytes, SHA-256
e12a4093ee078d91cb5fbfd8699e8ced3af26cd2f1fe848bacc52b1e57bef7d4.
Its pending translation must preserve changed-value comparisons, separate
rotation/translation limits, optional source-prefix skip, and track-pointer
gates at child70/74: a nonnull track whose byte3 isFF prevents that update.
Rotation changes set child4 and5; translation changes set child4 only.
Child count is (u16)(rootCount-1), so rootCount0 is not a harmless zero-child
case; do not copy the provisional adapter's count clamp. These are source
findings, not yet native pose-handler proof.

The exact lookup is available for E39F0, but the provisional host pose path
was not rewired. Full pose/interpreter/parent integration and visible game
acceptance remain unfinished. The running executable was not replaced.

### Direct pose application DEF10 restored

Implemented complete func_801DEF10 using the preceding420-byte retail pin.
The source prefix skip, separate rotation/translation counts, signed triple
loads and child stride7C follow the retail instructions. Only changed
values reach the corresponding track gate; nonnull track70/74 with byte3
equalFF suppresses that write but does not undo source consumption or the
component counter. Rotation writes set child4/5; translation sets child4
without overwriting child5. Child count retains the retail unsigned16
rootCount-1 expression, with no host node-count clamp or guessed null guard.

pose_apply_retail_test/runner first reported missing native body. O0/O2/UBSan
now each pass36864 full-memory comparisons against the complete retail
instruction body, without mocked callees. Coverage includes root counts
1,2,4,65; all four flag combinations; independent component limits0,1,2,70;
all nine null/open/FF track combinations; equal/changed component states;
both prefix modes; and constant/extreme or varying signed source triples.
The varying pattern ensures an incorrect prefix skip is observable instead
of accidentally matching a repeated triple. Eight mutations are rejected:
prefix,64-node clamp, swapped rotation/translation track pointers, rotation
dirty bit, extra translation dirty bit, omitted equality condition, and
unsigned translation loads. Log: pc_port/build_native/pose-apply-final.log.
Clip lookup regression passes196608 cases per mode and six mutations in
pose-apply-lookup-regression.log.

Root count0 wrap to65535, maximum component counts, source/destination
aliasing, invalid-memory fault behavior and hardware execution are not
covered by this fixture. Preserve these as unverified rather than imply
exhaustive input or retail hardware parity. Scoped production whitespace
and runner shell syntax checks pass. This new body is available for the
full interpreter; the provisional OvlyApplyPose caller is unchanged. No
full port link, live binary replacement or visible scene acceptance occurred.

### Node and tree track release DF52C/DFE8C restored

Read and implemented complete [801DF52C,801DF5F4),200 bytes, SHA-256
a0a446eac8e947dd26933b66a3a750963d7bfb05dcf32e07af3abbbdeafc3fad,
and [801DFE8C,801DFF78),236 bytes, SHA-256
c6a3090588554602e9bb56ccb10bfd96dcedb713542115a7eca6cc2adac7075a.
Both call the previously proven DF7A8 pool-release body, with no new heap
or ownership fallback. DF52C performs a signed index comparison against
the unsigned16 node count, uses wrapped32-bit index*124 addressing, and
releases nonnull70/74/78 slots selected by mask bits1/2/4. It does not
protect FF-marked tracks. DFE8C snapshots the root count and visits every
node including the root, preserving nonnull tracks whose byte3 isFF.
Both clear a released pointer only after the pool-release call.

node_track_release_retail_test/runner first failed for missing native
bodies. O0/O2/UBSan each pass52245 full-memory comparisons. The oracle
executes the retail release leaf and native executes its existing C body;
there are no mocked calls. Coverage: counts0,1,2,5,65; indices-1,0,1,4,
63,64,65,INT_MAX; mask0..15; all27 null/open/protected channel combinations;
free-index0,7,FFFF; and deliberately shared track entries across nodes.
The fixture reserves a preceding node so negative index-1 has a real
address rather than silently clamping it. Compare pool, every node and all
track memory. Six mutations fail: unsigned index test, shifted channel
mask, omitted single-node clear, wrong protected byte, skipped root and
omitted whole-tree clear. Log: pc_port/build_native/node-track-release-final.log.
Pose regression passes36864 cases per mode and eight mutations in
node-track-release-pose-regression.log.

Maximum count/index ranges, destructive pool/node aliasing and invalid
addresses remain untested. Scoped production whitespace and runner shell
syntax checks pass. These bodies close VM cleanup dependencies but do not
replace the full interpreter or parent teardown. No full port build or
visible gameplay acceptance occurred; canonical executable remains unchanged.

### Pose interpolation track binder DF0B4 restored

Read and implemented complete [801DF0B4,801DF52C),1144 bytes, SHA-256
eef6ffe9066cfd82b44ebd1d4048221527ef6d2ebf18019e30dc3f97141b7e5e.
Seven arguments: pool, root, pose, duration, absolute-mode, loop-bit, tag.
Only a fullword-zero duration becomes1; a nonzero value whose low16 is0
remains0 when stored. Rotation/translation source counts and prefix skip
follow the pose format. Changed channels reuse an unprotected track or
claim a free one with DF6F0. FF-marked tracks remain untouched. Skipped,
exhausted or equal channels release existing unprotected tracks with DF7A8.
Rotation deltas wrap to the signed12-bit shortest path, then optionally
add current angles for absolute mode; translation stores absolute targets
or low16 differences. Store mode3/4, loop bit, tag byte, start triples,
target/delta triples, zero elapsed and duration. Return the unsigned16
rootCount-1 child count. Scale is not handled by this routine.

pose_track_bind_retail_test/runner first reported missing native body.
O0/O2/UBSan each pass31104 full-memory/return comparisons. Both sides run
real pool claim and release implementations, not allocator stubs. Coverage:
root counts1,2,5; four flags and four component-limit pairs; all nine
null/open/protected track combinations; equal/varying signed source data;
duration0,65536,-1; mode/loop arguments with higher bits; free, exhausted
and initially occupied pool cursors; and both prefix modes. Six mutations
are rejected: halfword-only zero-duration test, wrong protected byte,
missing angular wrap, forced relative mode, omitted release and prefix skip.
Logs: pc_port/build_native/pose-track-bind-final.log and mutation directory
pose_track_bind_retail_test/. The runner also pins DF6F0..DF7A8,184 bytes,
SHA0d6bcac846b80f45722d187b804bc919c4e49af476b1a534a0eaea325e7980a1,
and the existing DF7A8 body.

Node/tree release regression passes52245 cases per mode and six mutations
in pose-track-bind-release-regression.log. Whitespace and shell syntax
checks pass. Root-count underflow/maxima, arbitrary source/track/node
aliasing, all duration/tag values and live animation behavior remain
unverified. The binder is not yet called by the incomplete full E39F0;
no full port link, live executable replacement or visual acceptance occurred.

### Stream track setup DF7F4 restored

Read and implemented complete [801DF7F4,801DFAC4),720 bytes, SHA-256
dc5578a95a99d8a88c5fcef27b75cb918c0bd1727e809aa3754ba08a28db0a3e.
Five arguments: pool,root,pose,loop,tag. Nonzero pose6 releases unprotected
tree tracks, applies the immediate pose and returns1. Otherwise use the
minimum of root count and rotationCount+1, compute the stream-data base
past six-byte descriptors and optional initial-value arrays, mask loop to
bit0 and subtract1 from the frame count only when not looping. Per-node
rotation/translation offsets other thanFFFF claim/reuse unprotected tracks
and install mode/tag, data pointers, elapsed0 and low16 frame count. A FFFF
offset leaves the root's existing track alone; children release unprotected
tracks. Streamed setup returns0. No fabricated pointer or exhausted-pool
fallback was added.

stream_track_bind_retail_test/runner first failed for the missing native
entry after a test-only numeric-token spacing typo was corrected. O0/O2/
UBSan each pass46656 full-memory/return comparisons, executing real native
and retail pose, tree-release, pool-claim and pool-release bodies. The runner
pins all five bodies. Coverage: root counts1,2,5; all pose flags; rotation/
translation limits0,1,5; nine null/open/protected track combinations; all
missing/present stream-channel combinations; frames0,1,FFFF; loop high-bit
masking; free/full/occupied pool cursors; and immediate/streamed branches.
Six mutations are rejected: immediate return, frame decrement, count limit,
descriptor-base calculation, root missing-stream handling and protected tag.
Log: pc_port/build_native/stream-track-bind-final.log.
Interpolation-binder regression passes31104 cases per mode and six mutations
in stream-track-bind-interpolation-regression.log.

Root count0/maxima, source/destination aliasing, all stream modes/tags and
subsequent stream decoding are not certified by this fixture. Pointer setup
is not playback proof. Full E39F0 and parent integration remain incomplete;
the adjacent DFAC4 initial-pose-plus-stream path is still not implemented.
Scoped whitespace and shell syntax checks pass. No port relink or live
executable replacement occurred; visible gameplay acceptance remains open.

### Initial pose plus stream setup DFAC4 restored

Read and implemented complete [801DFAC4,801DFE8C),968 bytes, SHA-256
91826341a3950ea20ff85e1ca4ea296f0d4056dab6df4a6b471daab2a3570f31.
The immediate-pose branch calls tree cleanup and DEF10, returning1. The
stream branch returns0 and shares the retail descriptor/base/count/frame
rules with DF7F4, but does not substitute that function: it consumes initial
pose triples for eligible child channels before installing streams. Missing
or protected streams still skip the corresponding triple; a failed pool
claim does not suppress an otherwise eligible initial-pose write. No root
initial-pose update is performed. Rotation writes halfwords and dirty4/5;
translation sign-extends halfwords into words and sets dirty4. Protected or
FFFF-offset tracks are preserved rather than released by this function.

initial_stream_bind_retail_test/runner first failed for missing native body.
O0/O2/UBSan each pass46656 full-memory/return comparisons with the real
pose, tree-release and pool functions on both sides. The fixture reuses
the streamed-binder case dimensions and adds varying initial-value data
to make cursor advancement observable. Seven mutations are rejected:
skipped cursor, suppressing initial pose on exhausted claim, root initial
pose, wrong protected tag, unsigned translation, frame decrement and dirty5.
The exhausted-claim mutation is scoped only to the pose-write condition,
not the preceding track lookup. Log: pc_port/build_native/initial-stream-bind-final.log.
Streamed-binder regression passes46656 cases per mode and six mutations in
initial-stream-bind-stream-regression.log. Scoped whitespace and shell
syntax checks pass.

This closes another VM dependency, not the complete interpreter. Maximum
counts, arbitrary aliasing, all stream modes and subsequent playback are
not proven. No full port link, live executable replacement or visible
scene acceptance occurred. The original partial VM remains in the live game.

### Object selector dependencies E67F8/E6830 restored

Complete retail spans [801E67F8,801E6830),56 bytes, SHA-256
3196805b21c24b52a7665109676897485a3738c80ddfa2c49076633956c1fef3,
and [801E6830,801E6910),224 bytes, SHA-256
15e3cad3fe23b5c63e7fb5311c088884c7e14cd8c6ca101e198b55108815ea48.
The first helper scans only eight bits and returns8 when none is set.
The second resolves retail selector aliases, retains byte truncation and
produces a low16 mask using the PSX five-bit variable-shift count. No native
slot clamp was introduced. Result arithmetic is not proof of a valid slot.

object_selector_retail_test passes all65536 mask values and393216 selector
cases in each of O0/O2/UBSan, comparing full memory and returns against actual
retail instructions. Coverage includes all selector/object-ID bytes, three
phase-mask modes, high argument bits, and output aliasing the object IDs.
Six mutations are rejected (scan limit, global width, self alias, derived
byte truncation, shift count, fixed slot). Log: object-selector-final.log.
Adjacent lookup regression passes196608 cases per mode and six mutations in
object-selector-lookup-regression.log. Logs are under pc_port/build_native.

### Child-node flag transfer E6668 restored

Read and implemented complete [801E6668,801E66BC),84 bytes, SHA-256
04a67648e51fed448553166db07002d74bc3dd0addbf007985a7eeb83c9c9e09.
Snapshot source-root count at+A; visit children1 through count-1 at124-byte
stride. A nonzero source flag+7 is cleared, then the target flag is set1.
Root flags are untouched. Identical and overlapping arrays retain retail
store ordering; there is no invented null/bounds guard or count clamp.

node_flag_transfer_retail_test first failed for the missing native body.
O0/O2/UBSan each pass15360 full-memory comparisons using the complete retail
body without mocked calls. Counts0,1,2,5,65; all256 flag bytes; constant,
alternating and varying flags; disjoint, identical, and both one-node-offset
overlaps are covered. Five mutations are rejected: root inclusion, inclusive
count, wrong stride, missing clear and non-normalized target value. Log:
pc_port/build_native/node-flag-transfer-final.log. Maximum count, arbitrary
byte-offset aliasing and invalid-pointer behavior are not certified.

Scoped whitespace and shell syntax checks pass. Live game678654 and its
debugger678634 were observed still running; canonical executable SHA remains
314e2100931873407fae7de672e245a1e6f0f9ab20dfe0e6079de7c36702eff3.
No full port relink or visible gameplay acceptance occurred. Full E39F0,
recursive transfer E6578, parent ownership and renderer integration remain
open; these helper proofs do not certify the existing partial interpreter.

### Recursive node transfer E6578 restored

Read complete retail [801E6578,801E6668),240 bytes, SHA-256
c2e23bce6a66f51de4c9e42c8338d43fe4f10a9f219d430ff78d6ac94f50d812.
The native implementation uses wrapped32 index*124 address arithmetic,
snapshots the root count, clears the selected source flag and sets target
flag1. It releases all three track slots unconditionally (including FF
tags), preserving the retail load/clear/call sequence, then recursively
visits children whose parent pointer matches the selected node. The child's
stored halfword ID selects the recursive node. No cycle guard, count clamp,
fabricated ownership or protected-track shortcut was added.

node_transfer_retail_test first failed for the absent native body. O0/O2/
UBSan each pass5184 full-memory comparisons against real retail recursive
and DF7A8 release instructions. Counts1..8, every selected node, star/chain/
binary tree shapes, all eight null/present channel masks, disjoint/identical
source and target, and three pool cursors are covered. Present tracks use
FF tags to detect inappropriate protection. Five mutations are rejected:
source clear, target flag, omitted releases, truncated child scan and wrong
parent comparison. Log: pc_port/build_native/node-transfer-final.log.
Child-flag regression passes15360 cases per mode and five mutations in
node-transfer-flag-regression.log. Maximum counts, cyclic or malformed
trees, noncanonical child IDs, negative selected indices, arbitrary pointer
aliasing and fault equivalence are not certified by these fixtures.

No full port relink or live executable replacement occurred. Full E39F0
interpreter and parent/render integration remain incomplete; helper-level
differential proof is not visible cutscene or whole-port acceptance.

### Interpreter orientation scalar E66BC restored

Re-ran the pinned VM call inventory: E66BC has three direct call sites.
Read full [801E66BC,801E67F8),316 bytes, SHA-256
37f897ed031858abadec3c7fe2bb4759629819f7e3756bc4c1c04ecd24433cbe.
The retail body calls OuterProduct12 (symbol8004A480), takes wrapped32 dot
product and squared length, calls SquareRoot0, adds1, then divides the
wrapped dot<<4 by that result and the wrapped quotient<<8 by argument4.
Return is sign-extended low16. Native code retains signed divide traps via
FieldTrackDivide; no floating-point normalization or denominator clamp.

orientation_scalar_retail_test first failed for the missing native body.
O0/O2/UBSan each pass57344 deterministic cases across full-word vector
patterns and seven positive/negative divisors. SDK boundaries are explicitly
controlled on both sides: the fixture records cross-product input vectors,
supplies deterministic full-word cross results, and records square-root
input. This tests the complete retail caller arithmetic/return/call counts,
NOT hardware cross-product or square-root parity. All five mutations are
rejected: dot axis, length increment, each shift, return narrowing.
Log: pc_port/build_native/orientation-scalar-final.log. Division fault cases,
all possible full-word inputs and SDK internals remain outside this fixture.
The existing OuterProduct12 implementation is in psyq_compat.c; this turn
did not certify or change it. Scoped whitespace and runner syntax pass.
No port relink, executable replacement or visible acceptance occurred.

### Script rotation binder E59D4 restored

Read complete [801E59D4,801E5B50),380 bytes, SHA-256
312c073a2f1d21558e69536b6472742d75a21ccb022cb0f48b69b63729aeba82.
Signed duration<2 writes low16 target XYZ at node54/56/58 and sets byte5
only. Otherwise full signed target arguments are compared against signed
node halfwords. A changed target reuses track70 or claims from DF6F0;
claim failure returns without invented fallback. Existing FF tags are not
protected. The track receives active1, loop0, mode3, tagFE, starting angles,
signed12 shortest-turn deltas, elapsed0 and low16 duration. The node's
track pointer is written last. No general pose-binder shortcut was used.

rotation_bind_retail_test first failed for absent native implementation.
O0/O2/UBSan each pass8064 complete-memory comparisons executing the real
retail DF6F0 pool claim. Seven durations include negative,0,1,2,65535,65536
and INT_MAX; twelve angle values include signed boundaries, full-word
extremes and the2048 shortest-turn boundary. Changed and narrowed-equal
targets, null/reused-FF tracks, full pool and occupied cursor are covered.
Five mutations are rejected: duration threshold, wrong dirty byte, tag,
shortest-turn boundary and elapsed initialization. Log:
pc_port/build_native/rotation-bind-final.log. Scoped whitespace and shell
syntax checks pass. Arbitrary aliasing, all input combinations and animation
playback remain unverified. Canonical executable SHA remains
314e2100931873407fae7de672e245a1e6f0f9ab20dfe0e6079de7c36702eff3;
no full relink or visible acceptance occurred. Full E39F0 remains incomplete.

### Distance-based track binder E5B50 restored

Read full [801E5B50,801E5C74),292 bytes, SHA-256
2ed8c6c81c3f3ad28f2ad4b20f6e72d4d34e78d5a93b1dab326ebce2a2cbfd21.
Nine arguments: pool,node,mode,first,second,duration,targetXYZ. Reuse node70
or claim DF6F0; failure leaves the node alone. Set active1, byte2=low8(mode+7),
loop0 and tagFE, including overwriting an existing FF track. Compute wrapped
full-word target-minus-node translation deltas and sum of squares; call
SquareRoot0 and store low16(result+1). Store first/second, targetXYZ, elapsed0
and duration as halfwords, then publish track70. No target clamp or fallback.

distance_track_bind_retail_test first failed for missing native body.
O0/O2/UBSan each pass32768 cases: all mode bytes with high argument bits,
32 deterministic full-word argument/position patterns, and free/reused-FF/
full/occupied-cursor pool states. Real retail/native pool claim executes;
SquareRoot0 is a shared controlled boundary, not SDK parity proof. The test
compares complete fixture memory, root input/call count and the complete
fixture snapshot at the SDK call, covering pre-call track-header ordering.
Five mutations are rejected: mode increment, tag, distance axis, root-result
increment and duration argument. Log: pc_port/build_native/distance-track-bind-final.log.
Rotation-binder regression passes8064 cases per mode and five mutations in
distance-track-bind-rotation-regression.log. Scoped whitespace/syntax pass.

Arbitrary aliasing, all full-word combinations, SDK internals and animation
playback remain unverified. No full relink or live executable replacement;
full interpreter/parent/renderer integration and visible acceptance remain
open. This helper proof does not establish whole-port completion.

### Timed-script setup E5C74 restored; optimized alias bug caught

Read complete [801E5C74,801E5CD8),100 bytes, SHA-256
aa7406e2a5c218eda80812514fa67b6fad6eb6f82f1325fcc91140decdf6fb09.
Zero data12 count writes only object98=FFFF. Otherwise clear98, store data2
at9A if any nonzero loop argument (elseFFFF), clear9C, reload data12 into9E,
and set A0/A4 to the wrapped sum of the data pointer and word data14. No
invented cleanup of inactive fields or host-address clamp was introduced.

timed_script_setup_retail_test first failed for missing native body, then
O0 passed but O2 failed at count1, overlapping-data layout2, loop0. The first
implementation's u32 load could be reordered across overlapping u16 stores
under C alias rules. Byte-copy word loads/stores now retain the retail order.
This was a native implementation bug caught by the differential fixture.
The original incompatible-type load is retained as a rejected mutation.

O0/O2/UBSan each pass589824 complete-memory comparisons: all65536 counts,
loop values0,1,INT_MIN, disjoint data and two overlapping layouts, varying
frame fields and wrapping stream offsets. Full retail body executes without
mocked calls. Six mutations are rejected: inactive sentinel, boolean gate,
count source, offset source, second pointer store and alias-unsafe load.
One initial pointer mutation became a no-op after the memcpy correction;
the runner rejected it as surviving, and it was updated to remove the
actual memcpy before the final successful run. Log:
pc_port/build_native/timed-script-setup-final.log. Arbitrary misalignment,
invalid addresses and downstream timed-command execution are not certified.

Adjacent E5CD8 [801E5CD8,801E5D44),108 bytes, was read, SHA-256
e1810ab5a08ecf8609402635dbd2c7a0c00f24ce0b4a11263a6685beff1ca198.
Selector0 reads pointer8005919C; selectors1/2 follow objectB0/B4 then+8;
each returns u16 at+14 shifted16. Other selector values return2. No native
implementation was added: ownership of the global pointer remains to be
resolved (no exact symbol found in the inspected symbol map/source search).
No fabricated global or default scale was installed. No full port relink
or visible acceptance occurred; full interpreter integration remains open.

### E5CD8 semantic correction: sound-bank lookup, not movement scale

The previous working description "scale-source" was incorrect. Traced the
pinned interpreter dispatch table: opcode3C enters801E4F00, obtains the
bank selector from the instruction high byte in s5, calls E5CD8 at4F18,
adds the operand low byte to its result, and calls8003A3B8 at4F28 with
pitch0 and the operand high byte as steps. The existing sound.c body names
8003A3B8 as the matching-channel pitch ramp. Independent retail battle
reader8008AA40 loads the same pointer8005919C, shifts its bank ID at+14,
ORs an effect ID and calls80039DB8; battle cleanup800B8774 passes the bank
to8003852C then HeapFree. These are sound-bank consumers, not geometry.

The VM inventory now records this pinned sound route and its unresolved
native global owner, along with the two package-pointer offsets and the
retail other-selector result2. A new regression first failed for missing
metadata, then all five inventory tests passed, including changed-body,
changed-table and truncated-input rejection. This metadata is a manually
traced data-flow record, not execution or dependency-closure proof.

Read-only searches found the main-exe reader8001FD00 and six battle reads.
Scans checked literal address/low-immediate references, GP+2C with configured
GP80059170, and a limited linear constant-propagation scan of main, field,
and battle images. No writer was identified. This does NOT prove no writer
exists: aliasing, other overlays, indirect stores and control-flow joins
remain unexamined. The corresponding main-image bytes are zero and lie in
the declared sbss range; they do not supply a valid initialized bank.
ObjectB0's retail writer is E742C at801E7C1C, loading the package pointer
from t0+8; B4's owner remains unresolved. E8030 also consumes B0+8 when
releasing sound data. Full parent ownership is therefore still necessary.

No fabricated bank, scale default, zero-return stub or guest/host pointer
alias was added. E5CD8 native implementation remains deferred pending
ownership resolution. The next evidence lane is parent package/bank
ownership or another unresolved VM dependency; the overall goal remains
active. No live process mutation, build replacement or gameplay acceptance.

### Object scale helper E8480 restored

Read full [801E8480,801E8510),144 bytes, SHA-256
e7efebddd8c4f81a23326459143c016db776a73c057c3e65cb2b5a29872defd3.
Read registry8670 at wrapped slot*4. Null slot returns0. Flag object4A bit8
selects signed object26/node4C; otherwise object28/node50. Reload the object,
multiply signed object1C by the chosen node component, wrap32 then arithmetic
shift12; multiply that result by the selected object factor, wrap32 and
shift12 again. No slot clamp or floating-point combined multiplication.
VM call801E5670 follows selector E6830 and adds the result to object8E as
a halfword. This is distinct from the sound-bank helper audited above.

object_scale_retail_test first failed for missing native body. O0/O2/UBSan
each pass393216 complete-body arithmetic cases plus10 null slots, without
mocked calls. Coverage: all65536 base-scale halfwords, varied signed factor/
component patterns, both flag branches (other bits set), slots0,1,9, and
all10 existing null slots. Fixture memory is checked unchanged. Five
mutations are rejected: flag bit, factor axis, component axis, signed load
and shifts. Log: pc_port/build_native/object-scale-final.log. Out-of-range
slots, arbitrary pointers and all independent factor combinations are not
certified. Scoped whitespace and shell syntax checks pass.

No full port relink, live executable replacement or visible acceptance.
Full E39F0 and parent/render integration remain incomplete; proven helpers
are not a substitute for the remaining interpreter or retail scene proof.

### Target resolver E63A8 restored

Read complete [801E63A8,801E6578),464 bytes, SHA-256
4ddc077241fbfd1dea89eb13ff3500e1f20af9b3c45c4f70c32fd4d239e3973d.
Resolve signed object58 selector: FF scans eight bits of object10A, FE
uses the full global halfword86B0, FD/FC use own/linked object ID bytes,
FA selects10,1..127 select index-1; other values retain slot0. Null or self
target returns without writes. Nonzero signed child index composes root+C
with root+child*124+2C into scratchpad; child0 uses root+C directly. Set
rotation/translation, execute MVMVA00480012 on object64 vector, and store
low16 results at88/8A/8C. Existing own-root track70 modes7/8 additionally
receive those results at A/C/E. No position override or registry clamp.

target_resolve_retail_test first failed for missing native body. O0/O2/
UBSan each pass7040 full-body comparisons of fixture memory, scratch,
GTE registers and SDK call records. Eleven selectors include negative,
zero, direct1/2/9/10, FC/FD/FE/FF and100; children-1..3, null/present
targets, null/mode7/mode8/mode9 tracks and sixteen data seeds are covered.
FF includes no-bit and single-bit cases. Composition is explicitly a shared
controlled boundary; GTE transfers/commands use the shared PsyCross backend.
This is not independent hardware or complete SDK matrix proof. Seven
mutations are rejected: scan bound, direct index, self gate, child matrix,
GTE command, mode8 exclusion and destination offset. Log:
pc_port/build_native/target-resolve-final.log. Shared-transform regression
passes16128 cases per mode and seven mutations in target-resolve-transform-regression.log.

FA slot10, direct selectors beyond native registry capacity, high FE IDs,
all masks, large child indices, arbitrary aliasing and fault behavior are
not covered by this bounded fixture. In particular, the native10-slot
registry is still smaller than the retail selector arithmetic permits;
this implementation does not hide that ownership problem with a clamp.
No full relink or live executable replacement. Full interpreter, parent
ownership and renderer integration remain incomplete; visible acceptance
is still open.

### Recursive node transform E7094 restored

Read complete [801E7094,801E7298),516 bytes, SHA-256
5c90ef3a247d87241cc96c3b48d765f87fc62fb943df332a9d546f39111b38fa.
Contrary to the initial working guess, this is a transform updater, not an
effect allocator. Low3 flag bits select rotation0, translation1, or scale
for every other value. Bit20 chooses addition instead of assignment.
Rotation/scale truncate to halfwords; translation sign-extends each input
halfword before wrapped32 addition or assignment. Dirty bytes4/5 are set.
Bit80 scans the object's root-node array and recursively updates children
whose parent pointer equals the selected node. Recursive arguments narrow
flags to byte and XYZ to signed halfwords. Root pointer/count are reloaded
after each visited child, matching the loop rather than snapshotting count.
No hierarchy-depth clamp, cycle suppression or allocation was added.

node_transform_retail_test first failed for missing native body. O0/O2/
UBSan each pass46080 complete-memory comparisons running actual recursive
retail calls, without mocked boundaries. All256 flag bytes with high bits,
counts1..5, every selected node, star/chain/binary shapes, and four varied
full-word argument/state patterns are covered. Six mutations are rejected:
kind mask, unsigned translation, additive bit, recursion bit, dirty5 and
parent match. Log: pc_port/build_native/node-transform-final.log. Recursive
transfer regression passes5184 cases per mode and five mutations in
node-transform-transfer-regression.log. Large/malformed/cyclic trees,
arbitrary aliasing, mutation of the root-count storage during traversal,
and all full-word combinations are not certified. No full relink, live
executable replacement or visible cutscene acceptance occurred.

### Recursive node flag E6D94 restored; sprite boundaries separated

Read [801E6D94,801E7094) and identified three distinct functions, not one
timed-transform routine. Complete E6D94 ends at801E6E48 (180 bytes), SHA-256
c568e5ac466edda111c10c930ff6a52fa982ea2621cb3f59640f6ef2f51cb0e1.
It sets node7 to flags bit0 and, when bit80 is set, recursively visits
children with a matching parent pointer. Root pointer/count are reloaded
after every visited child. Full flags pass into recursion, unlike E7094's
byte-narrowed flags. No invented count cap or hierarchy fallback.

node_visibility_retail_test first failed for missing native body. O0/O2/
UBSan each pass46080 complete-memory comparisons with actual recursive
retail calls. All256 flag bytes plus high bits, counts1..5, each selected
node, three tree shapes and four initial-data patterns are covered. Five
mutations are rejected: flag mask, recursive gate, parent test, destination
byte and child loop bound. Log: pc_port/build_native/node-visibility-final.log.
Recursive-transform regression passes46080 cases per mode and six mutations
in node-visibility-transform-regression.log. Large/cyclic/malformed trees,
root-pointer/count aliasing and arbitrary memory faults remain unverified.

The following E6E48 sprite constructor ends at E6F64; E6F64 is its attached
transform callback ending at E7094. Both were read but remain unimplemented
in this increment. The callback performs a saved indirect callback after
transforming sprite position; a fake or unconditionally skipped callback
would not close that dependency. Scoped whitespace and shell syntax pass.
No full relink, live process change or visible gameplay acceptance occurred.

### Sprite attachment dependency audit: missing constructor and false owner label

Pinned complete attachment constructor [801E6E48,801E6F64),284 bytes,
SHA-256 74668c6567f1addcc854107a218b19d830fa31e737d2fde330cd918e0927c733;
callback [801E6F64,801E7094),304 bytes,
SHA-256 641195ca393cf6c7f11befe4ad49bd40aa20a7b045b327f0b9a1a393af3f75b4.
Constructor calls80023FD8 with extra size18, configures sprite+38 through
21FE0/223B0/22000, uses signed wrapperBE to locate its trailing record,
and conditionally saves WorkListTaskGetTaskCallback (8001CD7C) before
TimerWorkListSetTaskCallback (8001CD6C) installs E6F64. The callback composes
the referenced node transform, applies its local vector, optionally replaces
Y with the parent object's signed60, writes shifted16 XYZ into wrapper38/3C/40,
then invokes the saved callback. Skipping this call is not equivalent.

Read full main-exe constructor [80023FD8,80024294),700 bytes, SHA-256
6291763a42ce40bf3073a031287166b8401aa0e6655b0cd4a47cfad702393c65.
It decodes animation type/mode, calls23A48, initializes inherited sprite
state, starts23538 and applies24730 type setup. Native source currently
has only INCLUDE_ASM for23FD8, and nm of the running canonical executable
shows no23FD8/E6E48/E6F64 symbol. This is an absent dependency, not a proven
linked return-zero stub. Native233A4 installs22DF4;24730 changes type7 to
22E8C and invokes BC158 for types10..13. Both timer callbacks exist natively.
No single-callback assumption or fabricated wrapper was introduced.

Found a pre-existing misleading label: game_overrides.c BC158 was described
as a field-overlay hook, but config/field.yaml places field BSS at800AF5F0;
the actual code is battle [800BC158,800BC2F0),408 bytes, SHA-256
1f42d3930dcfaf1335bb20084a37516c7e61589d4621cb9cd05108dc251f315f.
The retail body manages type10/11 ownership slots and may call an existing
task's free callback before replacement. The native body only logs and
returns. Updated its comment and diagnostic to identify the battle owner
and explicitly label the no-op as non-equivalent. No behavioral fix or
parity claim was made. Adjacent BA8F4 was not changed in this increment.

Scoped whitespace checks pass. No runtime/headless gameplay test, full
relink, live process change or visible acceptance occurred. Next work must
close the constructor and callback owners (including BC158 where reached),
or continue another missing interpreter dependency. Sound-bank ownership
and full interpreter integration remain open; this audit is not completion.

### Main sprite constructor 23FD8 restored and registered for native build

Implemented full [80023FD8,80024294),700 bytes, SHA-256
6291763a42ce40bf3073a031287166b8401aa0e6655b0cd4a47cfad702393c65,
in pc_port/src/sprite_constructor.c. The port's explicit PORT_SOURCES
registry now includes it; no silent auto-discovery assumption remains.
Constructor retains wrapped index/table addressing, decoded type/mode calls,
NULL owner to23A48, allocator-substituted package, wrapper flag20000000,
optional inherited sprite fields/bitfields, final cleared motion fields,
global scale low16, shifted16 position, script startup and type setup order.
Byte-copy field accesses avoid incompatible-type alias assumptions.

The optional parent pointer is read from the existing emulated-RAM slot
800C3E1C only when591AD is nonzero. A narrow pointer adapter accepts PSX
RAM aliases or pointers published by native adapters; it does not create
a new mirrored global, assign a parent, force591AD, or supply default data.
The writer/lifetime of that slot and inherited pointer consumers still
need integration proof. This restores the read semantics, not ownership.

sprite_constructor_retail_test compares the actual700-byte retail body
against native code with five explicitly controlled call boundaries:
23440,23468,23A48,23538,24730. O0/O2/UBSan each pass4096 full-memory,
return and complete-callee-snapshot comparisons. Sixteen type values,
null/native/self/guest-RAM parent sources, enabled/disabled inheritance,
eight state patterns and indices-1,0,1,7 are covered. The allocator returns
an alternate package, and startup/type callbacks mutate sentinel bytes to
expose call ordering. Seven mutations are rejected: table entry offset,
inheritance gate, inherited source field, cleared field, position shift,
type mask and global scale. Log: pc_port/build_native/sprite-constructor-final.log.
Strict standalone compilation with Wall/Wextra/Werror passes, as do scoped
whitespace and runner/build-script syntax checks.

This test was added after the implementation; no test-first RED claim is
made for this increment. It does not certify allocator/type/script callees,
all pointer aliases, invalid-address behavior, arbitrary overlapping ranges
or visible sprites. The known BC158 no-op remains open. No full port relink
or running executable replacement occurred; attachment E6E48/E6F64 and full
interpreter integration remain incomplete.

### Attachment transform callback E6F64 restored with shared callback adapter

Implemented complete [801E6F64,801E7094),304 bytes, SHA-256
641195ca393cf6c7f11befe4ad49bd40aa20a7b045b327f0b9a1a393af3f75b4.
Signed wrapperBE selects the attachment record. Resolve its parent and
signed child index; child0 uses root+C, otherwise compose with
root+child*124+2C in scratch. Set GTE matrices, execute00480012 on record10,
optionally replace Y with signed parent60 when recordE is nonzero, store
wrapped result<<16 at wrapper38/3C/40, then invoke saved record4 callback.

The interface-design skill guided an additive shared boundary, not a new
resolver: work_list_callback.h declares PcPort_WorkListInvokeSavedCallback
for unchanged32-bit task callback addresses and native task arguments.
Its implementation delegates to the existing WorkListInvokeCallback,
retaining native calls, active battle guest dispatch and abort on unresolved
guest addresses. Existing work-list consumers were not changed. No native
cast-and-call of a guest address or default callback was introduced.
Read BC158's tail too: it installs BC018, so restricting saved callbacks to
only22DF4/22E8C would have been incomplete. Guest battle callback execution
still depends on the real active battle runtime, not a claimed native decomp.

saved_callback_adapter_test first failed for absent exported adapter, then
native/guest routing passes in O0/O2/UBSan. Unresolved guest80123450 aborts
with its diagnostic (expected134; core limit0). Log: saved-callback-final.log.
sprite_attach_callback_retail_test first failed for missing body; all three
modes then pass3840 full-memory/scratch/GTE/call comparisons. Children-1..3,
Y override0/1/FFFF, negative/positive record offsets, native/guest saved
callbacks and64 data patterns are covered. Composition and saved callback
bodies are controlled boundaries; the actual shared work-list adapter runs.
Callback snapshots expose transformed positions before callback mutation.
Six mutations are rejected: child matrix, local vector, GTE command,
nonzero override gate, fixed-point shift and omitted callback. Log:
sprite-attach-callback-final.log. Logs are under pc_port/build_native.

Work-list pair regression passes12 checks; timer regression passes576 list
and30721 lookup cases per mode (saved-callback-work-list-regression.log and
saved-callback-timer-regression.log). Scoped whitespace checks pass.
Arbitrary aliasing, all signed indices, matrix hardware parity, saved
callback internals, parent lifetime and visible motion remain unverified.
No full port relink, live game change or visual acceptance. Constructor
E6E48, BC158 ownership behavior and full interpreter integration remain open.

### Attachment constructor E6E48 restored; packed callback getter added

Implemented full [801E6E48,801E6F64),284 bytes, SHA-256
74668c6567f1addcc854107a218b19d830fa31e737d2fde330cd918e0927c733.
Call23FD8 with index/package/position and extra18; configure sprite+38 via
21FE0/223B0 with signed angle and SpriteSetScale with signed scale. Locate
the attachment using signed wrapperBE, store parent8 and data5 as halfwordC.
Nonzero data13 saves the old task callback, installs E6F64, and copies
data6/8/A into record10/12/14 and byteC into halfwordE. Zero leaves those
optional fields untouched. No fabricated wrapper or default callback.

The first test compile exposed two misleading-indentation warnings, fixed
without suppressing checks. Linking then exposed the missing packed native
WorkListTaskGetTaskCallback (the original work_list.c is excluded from the
port). Added its retail32-bit read at task+8, returning the unchanged code
address. Fixture now executes actual retail getter/setter instructions in
main-exe [8001CD6C,8001CD88),28 bytes, SHA-256
566e2d6e0b2853ff969f9a46db2e32ede6b0e59c567b41e6ed566c347157cfc2,
against actual native packed accessors. E6F64's native signature now matches
the work-list void(void*) contract directly, removing its function-pointer
type cast without changing retail arguments.

O0/O2/UBSan each pass8192 memory/callee-snapshot comparisons: all256 flags,
negative/positive attachment offsets and16 varied state/angle/scale patterns.
23FD8 and three sprite setters are controlled boundaries, not complete
allocation/playback proof. The installed callback is checked as guest
801E6F64 versus its native function address, then only that field is
normalized for the byte comparison; saved callback and all data remain
unnormalized. Seven mutations are rejected: extra allocation size, angle,
child byte, optional gate, saved callback, vector component and getter slot.
Log: pc_port/build_native/sprite-attachment-final.log. Callback regression
passes3840 cases per mode and six mutations (sprite-attachment-callback-regression.log).
Timer regression passes576 list/30721 lookup cases per mode in
sprite-attachment-work-list-regression.log. Scoped whitespace/syntax pass.

Arbitrary aliasing, constructor/setter internals, null-allocation faults,
callback lifetime, BC158 ownership and visible attachment rendering remain
unverified. No full relink or running executable replacement occurred.
Full interpreter and parent/render integration are still incomplete.

### General recursive track binding: 801E6974

Restored the complete [801E6974,801E6D94) body in field_object_overlay.c.
The 1056 retail bytes hash to
`dce06cc781ab77e9c6d0fc5818e91b0ceb1e946bff67ad2e279b03d0cb2500e2`.
The runner verifies the complete overlay payload as well as that body slice.
This routine binds rotation, translation or scale tracks; it is not an
effect-spawn constructor. It uses the real restored DF6F0 allocator, overwrites
existing FF-tag tracks, snapshots optional current-value offsets, computes
mode-zero halfword deltas, and applies initial values only for byte modes 0/1.
Translation writeback sign-extends; there are no dirty-flag writes. Allocation
failure does not suppress child recursion. Child scanning reloads the root
count while retaining the advancing candidate pointer.

The test first failed with `TRACK BIND FAIL missing native body`. After the
implementation, O0/O2/UBSan each passed 49152 complete-fixture comparisons
against actual retail recursion and allocator instructions. Coverage includes
all 256 flag bytes with high input bits, byte modes 0/1/2/255, four value
patterns, three five-node tree shapes, and free/reused/exhausted/occupied
pools. Existing child tracks are retained in exhausted-pool fixtures so local
failure does not erase the observable recursive work. Six deliberately wrong
implementations are rejected: channel mask, delta mode, initial writeback
threshold, translation sign, recursion flag, and added dirty flag.
Log: `pc_port/build_native/general-track-bind-final.log`.

This is helper-level differential evidence, not visible cutscene or full
interpreter parity. Arbitrary pointer overlap, cycles, changing root ownership,
all tree sizes, and all independent parameter combinations remain unverified.
The running game and canonical executable were not replaced or rebuilt.

### Timed command 8: inherited stack value is behaviorally significant

Read the complete retail frame wrapper [801E36BC,801E37D0), 276 bytes,
SHA-256 `9277585ee654c29ae0768e3551fcab8ef33c94b168c7eedd832f38cbffc71fe3`.
It takes five arguments. For a nonzero object owner word, active objects call
E7298 and either DC848 or DC5C0, then execute DDBF8 followed by E5D44 for
each positive tick. DDBF8 return values are ORed and passed to E39F0 with
the original tick count and fifth argument. Hidden objects still reach E39F0.
The current native four-argument wrapper instead clamps ticks and invokes
the provisional clip path: it is not this retail sequence. Replacement is
still pending actual interpreter/timed-command closure, not approved by
helper tests. The owner-zero return also carries incoming s2 rather than a
locally initialized result; do not infer a guaranteed zero return.

The retail-only `timed_command_stack_retail_test.c` executes the complete
E5D44 body and its original dispatch table. Its command-8 fixture varies all
65536 incoming halfwords at entry SP-0x38 (callee SP+0x50). The instruction at
801E6014 reads this halfword without a local initialization. Zero selects
801E8330(slot, mask, animation); every nonzero value selects
801E8394(object, slot, mask, animation). The two target bodies are intercepted
only to record the observed boundary and return; their internals are not
certified. The test also checks stream advancement and timer/count updates.
O0/O2/UBSan each pass all 65536 cases. In-memory instruction mutations that
replace the load with zero or invert the branch are both rejected. Retail
disc bytes are never modified. Runner pins the whole overlay and the 1512-byte
E5D44 body; log: `pc_port/build_native/timed-command-stack-final.log`.

The immediately preceding DDBF8 uses a 0xB8 stack frame, placing the same
address at its SP+0x80. A lexical scan found no explicit 0x80(sp) operand
there, which does NOT exclude indirect/callee writes or establish the value's
origin. The natural caller/stack history remains unresolved. Synthetic stack
seeds prove dependence, not the value present during the opening cutscene.
Do not replace this dependency with a zero, a guessed mode, or an invented
native field. No native timed-command parity, full build, or visible runtime
acceptance is claimed by this experiment. Existing game processes remain live.

### Cross-object clip entry 801E8394

Implemented [801E8394,801E8430), 156 bytes, SHA-256
`52f01bdbf34869a3e7cf2cd2704d57bee4a84dfcd0376e1ee33ef71eff93ee1f`.
The routine takes a source object, destination slot, selection mask and clip
index. It truncates the slot to 16 bits, updates D86B0/D863C, clears destination
byte 35 before the retail null check, and calls the restored E35D0 only when
the destination differs from E67F8's first selected slot. No native slot
clamp or replacement registry object was introduced. Its E35D0 call uses the
original source object and D86A8 context. The existing provisional E8330
remains unchanged and is not certified by this test.

The new cross_clip_bind fixture first failed for a missing native entry.
Final O0/O2/UBSan each pass 136112 comparisons of complete object/table memory,
selection globals, and the interpreter call snapshot. It executes actual
retail E8394, E67F8 and E35D0, with only E39F0 controlled at the boundary.
Coverage includes all 65536 selection masks, self/distinct sources, plus a
cross product of ten valid native slots, nine single-bit/empty selections,
four queue states and seven signed/boundary clip indices. Context addresses
are normalized only after checking the retail D86A8 address. The source and
destination have distinct IDs and primary clip tables.

Six mutations are rejected: slot truncation to eight slots, mask suppression,
wrong clear byte value, reversed selection gate, wrong source object and
wrong animation argument. The first wrong-source mutation survived because
the initial fixture used equivalent source data; the fixture was corrected
and the full three-mode and mutation runs repeated. Log:
`pc_port/build_native/cross-clip-bind-final.log`. The existing clip binder
regression passes 16 resets/16384 binds per mode, six mutations and the
expected missing-interpreter SIGABRT; log:
`pc_port/build_native/cross-clip-binder-regression.log`.

This does not establish null-target fault equivalence, indices outside the
native registry, arbitrary aliasing, full E39F0 semantics, or opening-scene
acceptance. The unresolved inherited stack value in E5D44 is unchanged.
No full rebuild or replacement of the user's running executable occurred.

### Sound-bank selector E5CD8: source restored, ownership still unresolved

Implemented the complete [801E5CD8,801E5D44) lookup, 108 bytes, SHA-256
`e1810ab5a08ecf8609402635dbd2c7a0c00f24ce0b4a11263a6685beff1ca198`.
Selector 0 reads the existing RAM word at 8005919C. Selectors 1/2 follow
object B0/B4 to package word 8. Each route reads bank halfword 14 and shifts
it left 16. All other fullword selectors return 2; truncating this helper's
selector to a byte is incorrect even though its VM caller supplies a byte.
Packed pointers use existing g_PsxRam for KUSEG/KSEG0/KSEG1 RAM addresses
and retain already-native pointers otherwise. This adapter publishes no
bank pointer, installs no default bank, and changes no sound ownership.
Address zero is a hardware RAM alias, not a replacement-bank policy; its
boundary behavior is not covered by the tests below.

The new sound_bank_selector fixture first failed for the missing native
body. O0/O2/UBSan each pass 786432 direct retail comparisons: every 16-bit
bank ID, all three selector routes, and native/KUSEG/KSEG0/KSEG1 addresses
for bank and package references. Eight additional selectors, including
256/257/258 and signed/fullword extremes, check the return-2 path with an
unused NULL object. Six mutations are rejected: global slot, B0/B4 routing,
bank-ID offset, shift, default result and byte-truncated selector. Log:
`pc_port/build_native/sound-bank-selector-final.log`.
The byte-inventory regression passes all five tests; it still reports
`native_global_owner: UNRESOLVED`, correctly separating source lookup from
the missing owner/lifetime proof.

These are synthetic lookup inputs, not bank publication during the opening
scene or audible music acceptance. RAM-edge wrapping, arbitrary aliasing,
invalid references and complete sound-engine integration remain unverified.
The running binary was not rebuilt or replaced; VM integration remains open.

### Effect constructor E0A00: register carry traced through timed caller

Read the complete [801E0A00,801E1258) constructor, 2136 bytes, SHA-256
`4ede271950970cf43e61e2864660eaa9a27bbd4145f191c81330dab80d6ec32c`.
It is still missing natively. Types 4/5 with pattern-fill mode 2 choose a
component according to signed (outputY + row) remainder 3. For negative
remainders the switch assigns no new value to s1; the destination halfword
retains the incoming s1 value. This must not become an invented zero/default
color or an undefined uninitialized C local in a supposedly faithful port.

The retail-only effect_constructor_register test executes the entire body.
All 65536 low-halfword s1 values crossed with row offsets -3,-2,-1,0,1,2
produce the observed retained value or the selected color component: 393216
cases per O0/O2/UBSan. A RAM-only mutation that zeros s1 in the negative
remainder path is rejected. The first mutation targeted the wrong delay slot
and survived; moving it to the actually executed 801E1008 delay slot made
the negative control effective. No disc bytes were changed.

A lexical overlay scan finds one direct E0A00 call at E6258 in E5D44.
An additional 256 cases execute the real E5D44 command-9 handler, selector
E34BC and constructor together, varying the parent object's address. The
test does not seed s1: E5D44's real entry initializes s1 from its object
argument. Each negative-remainder fill writes the low halfword of that
parent address. HeapChangeCurrentUser and HeapAlloc are controlled boundaries;
the fixture verifies their arguments/counts and supplies a two-byte allocation.
Thus the carry source is resolved for this tested timed-command route, not
for every possible external/indirect caller. Ordinary nonnegative offsets
remain controls selecting explicit supplied components.

Log: `pc_port/build_native/effect-constructor-register-final.log`.
This is retail data-flow evidence, not a native constructor implementation,
all constructor modes, allocator fidelity, live reachability of negative
offsets, or visible effect acceptance. The complete native constructor and
its caller-context integration remain work to do. No running game state or
canonical executable was replaced.

### Complete E0A00 native body with explicit incoming-register contract

Added `PcPort_FieldEffectConstructWithS1` in field_object_overlay.c and its
contract in field_effect_constructor.h. It translates the complete pinned
2136-byte E0A00 body. The additional mandatory input is the actual incoming
retail s1, not a guessed zero, an uninitialized C local, or a new effect field.
There is deliberately no 19-argument E0A00 wrapper claiming to recover this
value automatically. The proven timed-command caller can supply its retail
parent address when integrated; other callers require their own provenance.
The API-design skill informed this explicit additive boundary.

Translation covers the existing-active rejection, owner change, ordered
header stores, type-dependent flag masks, zero-dimension defaults, signed
width rounding, conditional buffer allocations, both image-read/fill planes,
six-byte source row stride for types 4/5, packed-color fills for types 0/1,
and retained register color for negative pattern remainders. It preserves
retail's unsupported-type return after header initialization and does not
invent allocation-failure cleanup.

The new effect_constructor fixture first failed for the missing native body.
Final O0/O2/UBSan each pass 11088 complete-memory, return-value and call-snapshot
comparisons against the actual retail constructor. Heap ownership/allocation,
StoreImage and DrawSync are controlled boundaries, not certified SDK bodies.
Cases include type values 0..7/100/101/FFFF (hex for the latter three), high
argument bits, nine fill-mode pairs, all eight allocation flag combinations,
seven dimension/offset combinations (odd, default, negative and retained-carry
cases), and inactive/occupied records. Allocation requests are recorded even
when signed dimensions produce oversized requests; the test allocator returns
bounded fixture storage, so real large-allocation/failure behavior is not
claimed. Seven mutations are rejected: active gate, type narrowing, width
rounding, allocation flags, lost incoming carry, source stride and callback.
Log: `pc_port/build_native/effect-constructor-final.log`.

Effect-update regression passes 25920 cases per mode, the unknown-callback
trap, and eleven mutations; log:
`pc_port/build_native/effect-constructor-update-regression.log`.
Scoped whitespace and shell syntax checks pass. Arbitrary aliasing, every
dimension pair, allocation failures, actual GPU readback, complete caller
integration and visible effects remain unverified. This source addition does
not replace the running executable or complete the interpreter/parent pipeline.

### E8330 provisional animation entry replaced with retail binding

Replaced native func_801E8330 with [801E8330,801E8394), 100 bytes, SHA-256
`338db1f71d21b868721a7760e971ef414800b2eb601df4ccc0f34f8f28da8763`.
It loads the low16-selected registry entry before updating D86B0/D863C,
clears object byte 35 before the retail null check, reloads the entry, and
calls restored E35D0 with object as both destination/source and D86A8 context.
Removed its native slot clamp, premature object10A phase write, diagnostic
side path and call to the provisional OvlyClipBind implementation. Other
provisional interpreter users elsewhere in this file remain unresolved.

The fixture initially needed an assert-only ApplyMatrix boundary to link the
old provisional path; after that it reproduced the actual mismatch at phase0,
slot0, queue0, clip-128. That geometry guard is retained and never executes.
Final O0/O2/UBSan each pass 68056 complete-memory/global/interpreter-snapshot
comparisons against real E8330 and E35D0 with only E39F0 controlled. Includes
every selection mask plus ten valid slots, nine mask patterns, four queue
states and seven clip-index boundaries. Six mutations are rejected: slot
global, mask global, clear byte, premature phase write, animation and context.
Log: `pc_port/build_native/single-clip-bind-final.log`.

Crucially, invoking this native entry with no E39F0 body now terminates at
the binder's explicit missing-interpreter diagnostic (expected SIGABRT,
core limit zero), rather than falling back to guessed/skipped script behavior.
This is a source fidelity correction, NOT a playable-build acceptance gate:
**a fresh full build remains unsuitable for the user demo until E39F0 and
its caller dependencies are integrated.** The previously running executable
is preserved unchanged. Null-target faults, out-of-registry selectors,
arbitrary aliasing, full interpreter behavior and visible animation are not
certified by these tests.

### Trail/timed-script stop dependencies

Restored both complete 12-byte leaf routines: E0844 writes halfword FFFF
at the trail record base and returns -1 (ignoring the caller's second
argument); E632C writes halfword FFFF at object98 and returns -1. They do
not free buffers, clear timers or reset adjacent fields. Body hashes:
E0844 `4ff83a93b28850a90cc018298227c8bda54d9fb9734eb51f729dec1998e4e0f0`;
E632C `e72b17d79ecd6e56890a26389eca4dd0d82b0fff7f30845317c7871ce006ff9c`.

The script_stop fixture first failed for the missing entries. O0/O2/UBSan
each pass 131072 full-memory/return comparisons covering every previous
halfword value in both routines and varying neighboring data. Four mutations
are rejected: trail marker, trail return, script offset and script return.
Log: `pc_port/build_native/script-stop-final.log`. The runner pins both bodies
and the complete overlay. This closes these two leaf dependencies only; the
complete timed-command and clip interpreter paths remain incomplete. Invalid
pointer faults and visible playback behavior are not certified. No running
game or canonical executable was replaced.

### Complete timed-command translation with required CPU context

Added field_timed_commands.c/.h and explicitly registered the source in the
port build. `PcPort_FieldTimedCommandsWithContext` translates the complete
1512-byte E5D44 body, pinned to
`760f9f457085e59b63a6ae781297309641b8a4e429cc7ced348c1cda0f6c4c19`.
Its required context inputs are the inherited halfword at retail entry
SP-0x38 and the retail object address supplying constructor s1. Neither has
a default. The frame-loop caller is NOT wired to this function: the first
value's natural runtime origin remains unresolved.

The body includes all nine command slots and the retail unknown-command
behavior (increment count without advancing stream), signed time/count gates,
light record writes, trail stopping, node flags, both animation routes with
selection-global restoration, effect construction/cleanup, relative offsets,
and repeat reset. Constructor calls pass the proven retail object address
explicitly. Existing retail animation entries are used, not the provisional
binder. No command-count budget or invented unknown-op length was added.

The fixture was added after the initial implementation; no test-first RED
claim is made for this body. O0/O2/UBSan each pass 131072 complete-memory,
global and boundary-call comparisons against actual E5D44. Six callee
boundaries are controlled, including the constructor; this is not transitive
closure or visible playback proof. Inputs span all 256 command bytes, 256
fixture patterns, and zero/nonzero inherited stack values. Eight mutations
are rejected: timestamp gate, unknown length, trail stride, animation route,
global restoration, relative coordinate addition, effect flags and repeat
comparison. The first repeat mutant survived because equality was missing
from the fixture; the equality case was added and all modes/controls rerun.
Log: `pc_port/build_native/timed-commands-final.log`.

Standalone -Wall/-Wextra/-Werror syntax compilation and build/runner shell
syntax checks pass. Arbitrary aliases/callee mutations, all independent
operand/timer combinations, malformed indices, real inherited-stack ownership,
and full caller integration remain unverified. No full build or live game
replacement occurred. The missing E39F0 interpreter still prevents claiming
a newly built demo-ready port.

### Timed effect command integration: real constructor and cleanup

Added timed_effect_integration_retail_test and runner. This removes the
controlled constructor boundary for command 9: native and retail both run
the actual timed handler, E34BC callback selector, E0A00 constructor and
E165C cleanup. Only heap operations and GPU readback/upload/synchronization
remain controlled. The native caller passes the object's address as the
constructor carry; the retail E5D44 establishes s1 through its actual code.
Full fixture memory and ordered boundary snapshots are compared.

O0/O2/UBSan each pass 2304 cases covering eight type values, nine fill-mode
pairs, sixteen relative/offset/occupancy/repeat/linkage patterns and enabled
versus cleanup commands. Cases include negative pattern rows and default
widths. Three mutations are rejected: lost caller carry, missing allocation
flag and inverted cleanup gate. Log:
`pc_port/build_native/timed-effect-integration-final.log`.

The initial harness lacked LoadImage, then mistakenly hooked 8004495C rather
than the disassembly's 80044894. The resulting mismatch was corrected, and
the bridge now rejects every unrecognized target outside the pinned overlay
range immediately. It cannot silently run zero-filled main-executable memory
to a later mocked entry. Final runs use the corrected, fail-closed boundary.
The runner pins the full overlay plus constructor and timed-handler bodies.

This is command-9 integration evidence, not the complete frame loop, other
commands' transitive closure, real GPU/heap behavior, or visible effects.
Inherited-stack ownership for command 8 and E39F0 integration remain open.
No production logic changed in this increment, and the running executable
was not rebuilt or replaced.

### Clip interpreter entry phase: motion and redirect selection

Added field_clip_prelude.c/.h and registered the source in the port build.
It translates the observable entry phase [801E39F0,801E3D34), 836 bytes,
SHA-256 `ca040a2fd3119e90db71fdd0b4a12c4d7af5b560da6766ce1735fcfafa28a364`.
This is an internal phase, NOT a replacement E39F0 symbol or opcode loop.
It preserves the ticks-equal-zero/null-stream early exit, per-positive-tick
halfword acceleration and fixed-point motion, distance/height/timer redirect
priority and target refresh. Negative ticks skip motion but still process
redirects. It returns whether dispatch should proceed and the selected stream;
the eventual interpreter must still preserve its own local/stack state.

The fixture first failed for the missing native phase. O0/O2/UBSan each pass
6144 comparisons of complete object/node fixtures, boundary call snapshots
and selected stream against retail execution stopped immediately before
dispatch. Matrix application, distance, target refresh and heap owner change
are controlled boundaries; this does not certify their internals or GTE.
Coverage includes all redirect/target gate combinations, signed ticks
-2/-1/0/1/2/3, null/non-null streams and 32 motion/threshold patterns.
Seven mutations are rejected: negative-tick early exit, rotation shift,
acceleration source, distance comparison, height clamp, timer comparison and
target gate. The initial timer mutant survived a missing equality fixture;
an explicit equality case was added and all modes/controls rerun.
Log: `pc_port/build_native/clip-prelude-final.log`.

Native SVECTOR padding is initialized, not treated as a fourth component;
the controlled matrix boundary compares the three actual components only.
Arbitrary aliasing, all arithmetic inputs, SDK internals, retail temporary
stack/register equivalence and later handler dependencies remain unverified.
A lexical scan after the prefix found one explicit write to stack60 and no
explicit stack62/64 operand, but this is not proof against indirect consumers.
No opcode behavior, full interpreter parity or visible gameplay acceptance
is implied. Strict standalone compilation, shell syntax and scoped whitespace
checks pass. The running executable remains unchanged.

### Clip control instructions: waits, suspension and proven continuations

Added field_clip_control.c/.h and registered it for the native build.
It is one internal opcode group, not an E39F0 replacement. It handles 00
(hold), 01 (signed-halfword timed wait), 02/03 (postprocess flag), the 29
table-proven direct-continuation opcodes, and retail's hold behavior for
opcodes 71..FF. The remaining 80 valid opcodes return unhandled WITHOUT
modifying the working state or object; a caller must route them to real
handlers and must not treat unhandled as a successful skip. The only no-ops
in this group are supported directly by the pinned retail dispatch table.

Working state explicitly represents object/s3 and stack D0/D8/E8/F0/6C.
Successful waits clear elapsed time and consume ticks; incomplete waits and
limit=-1 rewind the stream and suspend. A test first failed for the missing
native group. Final O0/O2/UBSan each pass 393216 cases: 311296 direct retail
comparisons stopped at E5974, plus 81920 untouched-state checks for excluded
handlers. Inputs include all 65536 instruction words, signed wait boundaries,
all 65536 duration halfwords at completion and wrapped-tick boundaries.
Six mutations are rejected: limit sentinel, unsigned threshold, missing rewind,
lost postprocess flag, accepting unhandled instructions and a wrong no-op.
Mutations must emit the specific CLIP CONTROL FAIL diagnostic; arbitrary
assertions are not accepted as evidence. Core limits are zero for deliberate
assertion failures. Log: `pc_port/build_native/clip-control-final.log`.

Correction to earlier commentary: the current provisional OvlyClipTick does
already contain motion integration. Its discrepancies include tick clamping,
arithmetic/redirect/control behavior and skipped instruction payloads; saying
it omitted all motion was inaccurate. The verified entry/control components
do not yet replace that provisional loop or establish full interpreter parity.
Object/native pointer faults, all possible working-state aliases and later
postprocess/callee behavior remain unverified. Strict compilation and shell
syntax pass. No running game or canonical executable was replaced.

### Animation-state waits 20/21/22

Extended the control group with the exact handlers at E46AC, E46D4 and E4704.
Opcode20 holds for limit=-1 or status bit100. Opcode21 first stores the
instruction high byte to object3C, then holds for limit=-1 or bit1. Opcode22
does not read its threshold when limit=-1. Otherwise it uses bit400 for
parameter FF, or writes object3C and uses bit4 for other parameters. Matching
events increment the unsigned halfword at object42, compare it against a
signed-halfword threshold, and clear it on completion. Completion does not
consume ticks as opcode01 does. Incomplete waits rewind to the opcode.

The expanded test reproduced the missing handler at opcode0020 before the
implementation. O0/O2/UBSan each pass 786432 cases: 628736 direct retail
comparisons and 157696 untouched-state checks for the other 77 valid handlers.
It covers every instruction word, eight limit/status patterns, all 65536
duration values for ordinary and FF-parameter event waits, and counter/tick
wrap boundaries. Nine mutations are rejected, adding status-bit, event-bit
and signed-event-counter faults to the earlier six controls. Core limits
are now zero before positive/RED runs as well as deliberate mutation runs.
Log: `pc_port/build_native/clip-animation-waits-final.log`.

This closes these three handlers only, not all interpreter control flow,
remaining data-setting instructions, frame-loop integration or cutscene
acceptance. No guessed opcode lengths or default state were introduced.
The running executable remains untouched; a new demo build is still pending
the complete interpreter and its context dependencies.

### Clip object/data instructions: 22-handler closure

Added `field_clip_data.c` to the explicit port source list, implementing
opcodes 0C, 1E, 24, 2E, 30, 32, 33, 34, 36, 37, 3B, 48, 49, 50,
54, 55, 56, 5E, 5F, 63, 64 and 6D. Authority is the pinned E39F0 body
and dispatch table verified by `audit_field_clip_vm.inventory`; tests execute
the actual retail dispatch from 801E3D44 through 801E5974. This includes
motion-field reset, object flags, signed relative destinations, timer setup,
script-counter writes, signed XYZ stores, and wrapped Q12 distance arithmetic.
The three operand-consuming handlers are explicit retail table destinations,
not guessed lengths for missing implementations. Opcode63 leaves objectA4
unchanged; payload reads update the working operand only where retail does.
Packed loads/stores use memcpy to preserve read/write ordering under aliasing.

Test-first RED was `CLIP DATA FAIL missing native body`. Final O0/O2/UBSan
each pass 1724344 cases: 1484728 direct retail comparisons and 239616
untouched-state checks for excluded instructions. Coverage includes every
instruction word, every payload halfword for each included opcode, signed
boundary patterns, 20152 overlapping-script cases across aligned object/node
locations preserving the root pointer, and 256 null-object opcode24 cases.
Comparisons include the complete fixture, stream, ticks, running flag,
postprocess flag and operand; native object/limit must remain unchanged.
Nine deliberate faults are rejected with specific CLIP DATA FAIL diagnostics:
incomplete reset, unsigned relative displacement, incorrect script write,
lost operand, unsigned XYZ, wrong Q12 shift, writing timerA4 instead of A0,
wrong flag mask and accepting an unsupported instruction.
Log: `pc_port/build_native/clip-data-final.log`.
Control-handler regression also passes 786432 cases at O0/O2/UBSan and all
nine negative controls: `pc_port/build_native/clip-data-control-regression.log`.

These tests do not prove arbitrary pointer/fault behavior, every possible
memory alias or register/stack state, the remaining 55 valid opcode handlers,
the full E39F0 loop, frame-context dependencies or visible cutscene acceptance.
No successful skip, fallback interpreter, forced state or default data was
introduced. E39F0 remains missing and its binder remains fail-closed; this
source tree is not yet ready to replace the running demo binary. The running
game and canonical executable SHA 314e2100931873407fae7de672e245a1e6f0f9ab20dfe0e6079de7c36702eff3
were preserved. Shell syntax checks pass. Whole-worktree diff whitespace
checking reports existing unrelated PsyCross patch formatting; those files
were not changed in this increment.

### Clip motion, counted loops and indexed node data

Extended the data group with retail opcodes31, 4B/4C/4D/4E, 5C, 5D and6B.
Opcode31 reads the relative loop header, increments its following halfword,
then compares the signed wrapped count with the header's unsigned high byte.
The working operand is the incremented count; a taken branch selects the
loop header plus4. Motion setters/adders preserve halfword wrap and ordered
XYZ reads/writes. Opcode5C compares signed target halfwords against full-word
root coordinates before taking a signed relative branch. Indexed handlers
use signed-halfword indices, 124-byte stride and the retail halfword52 or
byte6 stores, without bounds clamps or fabricated nodes.

Expanded tests first failed on unimplemented opcode0031. Final O0/O2/UBSan
each pass 19585064 cases: 19353640 direct retail comparisons and 231424
excluded-opcode state-preservation checks. Coverage adds all 256 loop limits
against all 65536 counter values, all branch displacement halfwords with
eight XYZ equality/mismatch combinations, signed node indices -2..2, and
55080 overlapping-stream cases. This is not exhaustive arbitrary node-index
memory/fault coverage. The alias fixture initially overwrote the instruction
with its loop-counter setup; overlapping header ranges are now excluded from
that setup, while actual script/object aliases remain exercised. The old
global unsigned-relative mutation faulted on the new loop dereference;
it was narrowed to stored branch destinations, so rejection again requires
the intended CLIP DATA FAIL rather than an unrelated host fault.
All 14 mutations are rejected, including signed loop count, strict threshold,
motion addition, full-word position comparison and node stride. Log:
`pc_port/build_native/clip-data-motion-final.log`. The 30-handler group still
does not install a complete E39F0; 47 valid opcode handlers remain outside
the verified control/data groups.

### User-observed battle-entry abort and sprite dispatch direct returns

The user reports that the opening now looks correct and the Huff dialogue
appears, but gears remain invisible and entry into the battle-script scene
freezes. The preserved demo console records field2 dialogue52 at frame58553,
field teardown, battle entry at frame58926/field14, and retail battle adapter
entry80070F40, followed by SIGABRT from the explicit missing-dispatch assertion
in `src/slus_006.64/system/animation_scripts.c` (old binary line708).
Authority: `scratchpad/opening-retail-nodes.wjykKxsF/console.log`, original
demo SHA314e2100931873407fae7de672e245a1e6f0f9ab20dfe0e6079de7c36702eff3.
Process678654 is now zombie, not running gameplay. The existing gdb/PTY92506
remains open; read-only backtrace inspection cannot read the former stack,
and coredumpctl finds no core for that PID. The exact triggering opcode and
the reason memory became unavailable are UNRESOLVED. Do not claim that the
missing gears and this assertion share a root cause.

Retail inspection found 36 exact direct-return entries in jtbl800183D8;
native func8001FBE4 previously recognized only B2. Added an explicit predicate
and routed these 35 other proven no-op entries to return. All other missing
handlers retain the assertion. This is not blanket skipping of unsupported
commands, and it is NOT proven to fix the user's particular abort.
Earlier commentary counted 37 entries; corrected to36 after direct counting.
Retail table [800183D8,800185A4),460 bytes, SHA256
ee73be97683cbf2affa5db0d128e5083ac4b273da3bc5c2f0bfc12bfb75ee0f8;
body [8001FBE4,80021AD8),7924 bytes, SHA256
7431f354f172d1b450e6c231528af93546595e8aa6d750d10ca489d75e88442c.

The new sprite dispatch test was RED for the missing predicate. O0/O2/UBSan
each verify all 256 byte opcodes with four high-word patterns (1024 predicate
cases) and execute all 36 no-ops through both full native func8001FBE4 and
actual retail code (144 cases). Inputs are NULL; any dependency call hits
an explicit test-only abort guard, never a substitute production callee.
The classification excludes out-of-range opcodes, whose preexisting native
range-return remains separate. Missing-entry, invented-no-op, wrong-width and
disabled-native-routing mutations are rejected. Log:
`pc_port/build_native/sprite-dispatch-noop-final.log`.
The legacy source TU needs the same permissive compilation as the existing
port (warnings remain visible); the new test and CPU use strict warnings.
Initial standalone compilation exposed its preexisting implicit declarations
and missing injected stdint header; these were not suppressed in the test.

No canonical executable was rebuilt, no replacement game launched, and no
game state or assertion was bypassed. Full VM closure and a fresh visible
reproduction with the exact sprite opcode captured remain required.

### Sprite dispatcher: twelve retail data handlers and failure context

Restored opcodes8A, AD, AE, AF, B6, B7, B8, C9, CC, ED, EE and EF in
the decomp-owned `src/slus_006.64/system/animation_scripts.c`. Authority is
the same pinned 7924-byte retail func8001FBE4 and 460-byte dispatch table.
These paths make no callee calls. They implement the exact three-word
velocity reset (leaving +10 untouched), A8 packed flag mask, mirrored Z
rotation add/set, unmirrored X/Y rotation additions, byte decrement, gated
state halfword, signed relative address storage and fixed-point XYZ positions.
Successful rotation writes set bit28 of sprite3C; NULL transform paths do not.
OpcodeEE uses signed Q12 multiplication with truncation toward zero, then
the signed sprite84 base and wrapped shift16. Packed byte-based LE helpers
avoid signed-shift UB and effective-type alias assumptions. No unknown
handler skip, state clamp or synthetic callee was added.

The test initially reached the existing assertion on opcode8A before these
bodies were implemented. Final O0/O2/UBSan each compare 6815744 native/full
retail executions: all 65536 operand halfwords for each of12 handlers under
eight fixture patterns, plus every signed scale halfword for EE with eight
boundary operands. Patterns include both mirror/flag states, NULL transforms,
high opcode bits and script operands aliased into sprite8C. Whole fixture
memory is compared; any unexpected dependency call hits a test-only abort
guard. No production callee is replaced. This does not cover arbitrary
invalid pointers, every possible alias or every combination of all fields.
Native high-bit coverage uses the original opcode argument, not A1 after
retail's entry masking. Nine mutations are rejected: reset target, flag mask,
mirror bit, add/set route, rotation axis, signed rotation byte, signed relative
address, position shift and negative-product rounding. The first signed-byte
mutation changed only B8 signedness, which is equivalent after byte wrap;
it correctly survived and was replaced by the non-equivalent rotation-byte
mutation. Log: `pc_port/build_native/sprite-dispatch-data-final.log`.

Added host-only JSON failure telemetry immediately before the existing final
missing-handler assertion. Diagnostic questions: which full and narrowed
opcode failed, which dispatch index, and which sprite/operand addresses were
involved? The event records only those values, performs no input dereference,
and leaves the assertion and matching-build behavior unchanged. Process/run
correlation remains the existing per-session console log; no per-frame log,
new global counter or gameplay state is introduced. The observability skill
motivated retaining this context when the debugger stack cannot be recovered.
Before instrumentation, the explicit diagnostic test aborted without the
event. Final tests invoke missing opcode1234008C in a separate test process
at every optimization regime, parse the event, verify raw/narrowed/index
values, and require the original assertion text. These test aborts do not
launch headless gameplay. The user's actual triggering opcode remains unknown.

No canonical build or replacement game was launched. Gdb678634 remains open;
inferior678654 is still zombie, so it must not be described as live gameplay.
The missing gear rendering, complete sprite/clip interpreter closure, and
visible battle-script acceptance remain pending. This source-level repair
does not establish that the reported battle-entry abort is resolved.

One additional byte-oriented handler was restored from retail: opcode BF
stores the zero-extended operand byte as a halfword at sprite+36 (the
disassembly is `sh`, not `sb`). A first attempt misidentified opcode CF by
reading the neighboring FF00 body; the dispatch table maps CF to FF0C, so
that unverified change was removed. The BF implementation is tested separately
from the broad pointer-heavy fixture because the latter cannot safely represent
the same host and 32-bit guest pointer for aliased operand memory.

The focused guest-address test loads the complete retail dispatch table and
func8001FBE4 body (including the return at 80021AD8), then compares 256 BF
operands against native execution at O0/O2/UBSan. It passes:
`pc_port/build_native/sprite-byte-data-final.log`. The harness correction
confirmed that the earlier failure was first a truncated body, then a missing
dispatch table; both were test-only issues. This proves BF only, not the
neighboring CF body or arbitrary sprite pointers.

Follow-up corrected the neighboring simple entry: retail opcode A2
(target8001FF00) stores its operand byte at sprite+3D. The focused harness
now loads both dispatch table and complete body, compares BF's complete
halfword (including its untouched high byte) and A2's byte across 512 cases
at O0/O2/UBSan, and rejects A2-wrong-offset and BF-byte-store mutations.
The table still maps CF to 8001FF0C, so CF remains intentionally unimplemented.

Added the next non-return dispatcher body, opcode8C at retail8001FD50. It
loads the referenced sprite at +74, forms the two X/Z points from each
sprite's +2/+A signed halfwords, calls the existing retail angle helper
80023124, then applies the angle through the existing 80021FE0 and 800223B0
owners. A NULL reference returns before any writes, matching retail's branch.
The source compiles in the port's legacy compilation regime. A focused fixture
now supplies controlled angle/state-consumer probes and verifies point packing,
call order, returned-angle propagation, and the NULL-reference early return at
O0/O2/UBSan; it passes in
`pc_port/build_native/sprite-8c-final.log`. This proves the dispatcher body and
its retail helper wiring only; no visual or gameplay parity claim is made. The
currently running executable predates this source change and was not
overwritten.

The E39F0 boundary diagnostic was extended to record the object, context,
arguments, and current clip IP on the fail-closed path. The container rebuild
after that instrumentation completed successfully; log:
`pc_port/build_native/full-build-20260905-e39f0.log`. The rebuilt executable
hash is `b4b560dd5d66529084d18bd591a04db17b5a97ff2463bd5d301a92ff73efe628`.

### Container integration build and visible runtime

The host build was attempted first and failed closed because host SDL2
development files are absent; no executable changed in that attempt. The
documented `localhost/xenogears-dev-toolchain:current` container then completed
the full `pc_port/build_port.sh` path: all classified translation units built,
18 port-owned override symbols passed retirement metadata, trial link found
650 unresolved symbols, generated 79 function and 570 data stubs, and final
link succeeded with port-owned addresses verified. Log:
`pc_port/build_native/full-build-20260905-container.log`.

The rebuilt executable SHA is
`c95acf3cbc9219bec7a761eb59364db18f39baa94a016060712b6e3ca9af73f6`.
It was launched visibly (session14322, PID954108) after the prior inferior
had become a zombie. Runtime reached retail boot movie state, initialized
OpenAL/CD-XA/WDS, loaded map490 and field assets, and entered FieldMain's
main loop without the old immediate abort. This was a visible integration
run, not a proof of gameplay parity: the 650 unresolved symbols are still
stubbed, and no new battle/cutscene acceptance has been observed yet.

The visible run then advanced through the field-2 setup and emitted the battle
entry render path before stopping at
`[obj-ovly] retail E39F0 interpreter is not implemented`. This is the current
freeze boundary: `func_801E35D0` reaches the unresolved overlay interpreter,
which intentionally aborts rather than substituting the bounded host clip
tick. No input workaround or forced battle state was added; completing this
retail dependency remains the next required lane.

The next bounded slice is now present in the port: `func_801E39F0` invokes the
verified entry prelude and dispatches only the verified control/data handlers.
Any unsupported opcode or guard exhaustion reports its exact clip IP/opcode and
aborts; it never skips unknown payloads. This is intentionally not a claim of
full VM parity. The container build
`pc_port/build_native/full-build-20260905-vm-slice.log` links it with the same
650 unresolved-symbol census, and the prior prelude/control/data differential
fixtures remain passing.

The 90-second visible Lane-A opening harness reached four dialogs on map 4 and
reported no abort, SEGV, or primitive faults, but did not reach map 2 or E39F0
before timeout (`scratchpad/n5_opening_harness/lane-a-vm-slice`). This is
runtime progress only and does not establish battle or full-opening parity.

Retail opcode `0x0A` is now covered in the data dispatcher. Its pinned block
(`801E3E20`) passes the masked instruction parameter and fixed mask `7` to the
existing retail `801DF52C` track-release helper. The pose lookup path belongs
to opcode `0x10` (see below); the earlier local mapping was corrected before
production rebuild. The broad data suite was rerun with pool-dependent
opcodes isolated to focused fixtures and all negative controls passing.

Retail opcode `0x08` is now covered as well. Its `801E3E08` block passes the
E39F0 context argument as the animation-track pool to `801DFE8C`, then calls
the existing `801E632C` object sentinel/reset path. The VM state now carries
that context explicitly; a focused O0/O2/UBSan fixture verifies pool/root and
object propagation plus stream advance in
`pc_port/build_native/clip-release-opcode-final.log`. The broad data suite was
rerun with this pool-dependent opcode isolated to the focused fixture.
The full rebuild is recorded in
`pc_port/build_native/full-build-20260905-op08.log`.

Retail opcode `0x0B` is now covered from `801E3E3C`: it passes the E39F0
context pool to `801DFE8C`, then clears the child-node animation fields and
status bytes for nodes 1 through `root->count-1`. The focused release fixture
verifies pool/root propagation, exact child range, all six zeroed fields, and
both status-byte writes at O0/O2/UBSan (`pc_port/build_native/clip-release-
opcode-final.log`). The broad data suite and its mutation controls pass again;
the rebuild is `pc_port/build_native/full-build-20260905-op0b.log`.

Retail opcodes `0x0D` and `0x0E` are now covered from `801E3ED4` and
`801E3EF0`. Both pass the masked instruction parameter and root/context pool to
`801DF52C`; the fixed track masks are respectively `1` and `2`. The focused
release fixture verifies both calls and stream advancement at O0/O2/UBSan, and
the broad data differential plus mutation controls pass again. Rebuild:
`pc_port/build_native/full-build-20260905-op0de.log`.

Retail opcode `0x10` is covered from `801E3F0C`: it masks the parameter, calls
`801E6910` with the E39F0 object and stack flags slot, then calls `801DEF10`
with the object root and returned pose. The focused pose fixture now targets
the correct opcode and passes at O0/O2/UBSan (`pc_port/build_native/clip-pose-
opcode-final.log`).
The corrected mapping was rebuilt and linked in
`pc_port/build_native/full-build-20260905-op10-corrected.log`; no executable
was launched from the stale pre-correction image.

Retail opcode `0x11` is covered from `801E3F30`: it resolves the pose through
`801E6910`, consumes the following script word, invokes `801DF7F4` with the
context pool/root/pose and the retail loop/tag arguments, computes the signed
fixed-point magnitude into object `+0x8E`, then calls `801E5C74`. The focused
pose fixture covers both `0x10` and `0x11` call chains at O0/O2/UBSan
(`pc_port/build_native/clip-pose-opcode-final.log`); the broad data suite and
full rebuild `pc_port/build_native/full-build-20260905-op11.log` pass.

Retail opcodes `0x18` and `0x19` are now covered from `801E44A8` and
`801E44D4`. `0x18` performs the retail pose lookup, consumes a signed following
word, and calls `801E5C74`; `0x19` calls the object sentinel `801E632C`.
Focused pose/release fixtures and the broad data controls pass, with the
latest full rebuild recorded in
`pc_port/build_native/full-build-20260905-op1819.log`.

Retail opcode `0x1D` is now covered from `801E45A8`. It consumes the two
control words and seven signed coordinate words in retail order, selects the
`root + parameter*0x7C` node, and passes the exact flags/mode/tag/loop and
start/end/duration arguments to `801E6974`. A focused low-address fixture
verifies all arguments and stream advance at O0/O2/UBSan
(`pc_port/build_native/clip-track-opcode-final.log`); the broad data suite and
full rebuild `pc_port/build_native/full-build-20260905-op1d.log` pass.

Retail opcode `0x1F` is now covered from `801E4670`: it resolves the selector
through `801E6830`, masks the resulting slot, and switches the VM object to the
non-null entry in `D_801E8670`, otherwise preserving the current object. A
focused fixture verifies selector/slot routing and object replacement at
O0/O2/UBSan (`pc_port/build_native/clip-track-opcode-final.log`); the full
rebuild is `pc_port/build_native/full-build-20260905-op1f.log`.

Retail opcode `0x23` is now covered from `801E47D0`: it consumes a signed node
index, computes `root + index*0x7C`, and calls the existing recursive
`801E6D94` track routine with the instruction parameter as flags. A focused
fixture verifies signed index, node stride, flags, and stream advance at
O0/O2/UBSan (`pc_port/build_native/clip-track-opcode-final.log`); the broad
data suite and rebuild `pc_port/build_native/full-build-20260905-op23.log`
pass.

Fresh census of the pinned table versus source switch coverage finds 36 exact
direct-return entries and 16 non-return handler opcodes present in the source
(including the newly verified A2/BF). Sixty-one non-return opcode entries
remain outside the source's explicit handler bodies (some have aliases or
existing helper paths, which are not counted as parity). The machine-generated list is retained
in the command output for this turn; it is a planning census only, not a
parity certificate. In particular, table adjacency is not evidence that CF
shares A2's body.

The next retail clip handlers at `801E4B1C`/`801E4B94` are now wired as
opcodes `0x26` and `0x27`. `0x26` resolves the retail actor mask with
`801E6830` and clears the selected actors' attachment byte (`+0x5C`), while
`0x27` dispatches the existing source-faithful `801DC848` or `801DC5C0`
hierarchy path based on the object mode byte. Broad state-preservation and
negative-control suites pass, and the linked port is recorded in
`pc_port/build_native/full-build-20260905-op27.log`.

Battle-entry previously stopped at retail opcode `0x25` (`801E4810`). That
handler performs an eight-slot GTE/vector attachment calculation and calls
`801E66BC`; its alternate branch is intentionally not replaced with a guessed
transform. The remaining freeze, if that branch is selected, is therefore an
explicit unsupported-retail boundary, not a fabricated continuation.

Opcode `0x25` now has its retail common attach path (`801E4810`) wired: it
consumes the selector plus three orientation words, resolves the eight-slot
mask through `801E6830`, updates each selected actor's root attachment byte,
selector, mode, and active flag, and copies the three direct orientation words
for the non-GTE branch. The alternate bit-0 GTE/vector branch remains loudly
fail-closed until its `SetRotMatrix`/`SetTransMatrix`/`801E66BC` sequence is
ported; no host approximation is substituted. The port still links cleanly in
`pc_port/build_native/full-build-20260905-op25.log`.

The latest long opening trace reaches battle entry (`field=14`) and then stops
in the separate sprite-animation extended dispatcher (`func_8001FBE4`), before
any new field-clip assertion. Its failure telemetry is now unconditional so
the next retail run records the exact raw/dispatch opcode instead of only the
generic assertion line. Rebuild: `pc_port/build_native/full-build-20260905-battle-telemetry.log`.
