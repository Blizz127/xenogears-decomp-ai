# Opening battle audio loader — 2026-09-04

Scope: `func_8001BBAC`, reached from the retail state-2 battle overlay.
Starting HEAD: `3a3e7aac03a2f166fb924945a489e392d706f282`, dirty worktree preserved.
Verdict: focused source repairs tested; complete battle rendering NOT PROVEN.

## Retail authority

The supplied `disc/SLUS_006.64` slice `[0x8001BBAC, 0x8001BD40)` has SHA-256
`2a5926e7e942988d6311c01c77e970dc1a133808f2f8b878b5f41ac797d9dca5`.
File offsets are virtual address minus `0x8000F800` (including EXE header).

| Instructions | Required behavior | Repair |
|---|---|---|
| `8001BBE4..8001BBF8` | Add `7FE1C000` modulo 32 bits to first allocation address | Translate host pointer back to guest before arithmetic |
| `8001BC90` | Store the first buffer pointer as a word | Replace erroneous truncated 16-bit declaration/store |
| `8001BC58` | Store the second allocation result at `8006F9C8` | Use `D_800595A8`, not first buffer |
| `8001BC34..8001BC98` | Queue files 2, 3, 4 and a zero terminator; file 4 targets `801E4000` | Represent the same records using native pointer-width queue entries for the archive adapter |
| `8001BCC8..8001BCF0` | Index initialized table at `8004F388` | Read loaded EXE data, not a zero-filled generated host symbol |
| `8001BD04..8001BD14` | LHU bank ID at buffer offset 20, shift 16, OR selected sound | Restore unsigned halfword read |

The six three-byte table rows `[8004F388,8004F39A)` hash to
`3a4a4bd0ca3c36141cf0e9f7a1c454083fa3cb10d826a3373402be694821582b`.
No replacement sounds, forced state transitions, or substitute content were added.

## Tests and limits

Run `bash pc_port/tests/run_battle_heap_reservation_test.sh`.
It links the production `temp3.c` function with controlled allocation/archive/audio
boundaries, checks three guest heap positions, all queue destinations and the
terminator, bank IDs `1234` and `FFFF`, the scene-4 skip, and `FF` table skips.
It deliberately leaves the obsolete host table symbol zero while populating the
emulated EXE table. O0, O2 and UBSan pass. The script pins the retail function slice.

Observed failing controls before the relevant repairs:

- Reservation `8041F054` instead of `0001BFF4` at guest `801FFFF4`.
- First archive queue destination assertion failed.
- Sound-ID assertion failed when only emulated retail table data was populated.

These are boundary tests, not audible or rendered acceptance.

## Runtime evidence

- `scratchpad/battle-mips-runtime-20260904-04/run.log`: reached battle entry
  `80070F40`; core PID 480630 shows `HeapAlloc(2153822600, 1)` from this loader.
- `scratchpad/battle-mips-runtime-20260904-05/run.log`: heap-only build progressed
  beyond that allocation; core PID 528042 shows sound lookup crashing with
  `packedId=0`, after `SoundHandleError`. This run does not include queue repairs.
- `scratchpad/battle-mips-runtime-20260904-06/run.log`: full repaired build replay;
  both earlier crashes cleared; reached retail effects setup then crashed at
  `ResolveArchiveEntryPointers(NULL)`. Core PID 553501 shows guest call return
  address `801E54D8`; its preceding LW reads `800595A8`. Native `D_800595A8`
  held `007D2284`, but the guest/native symbol binding was absent. Added an exact
  four-byte guest symbol entry backed by the retail store at `8001BC50` and that
  module load. The bridge-map regression failed before adding the entry and
  passes afterward. Further runtime verification remains required.
- `scratchpad/battle-mips-runtime-20260904-07/run.log`: the binding repair cleared
  the null-pointer failure. Asset decompression and setup progressed, then the
  interpreter stopped at unresolved `ReadGeomScreen` (`8004A0DC`) after 625989
  instructions. Added the retail CFC2 register-26 getter to the leaf adapters;
  the missing-symbol test failed first, then O0/O2/UBSan passed (24 checks).
  The three-instruction retail slice hashes to
  `4ecf2f5bbdb450de400e5eeafac260aaf27c3f5d36b3f67ef8ceb47855a2e800`.

The user requested visible testing after run 07. No further headless replay is
authorized. The rebuilt game was launched normally on DISPLAY `:0`, without
field selection or scripted inputs; log `scratchpad/battle-visible-20260904-01.log`.
It reached title field 490. Binary SHA-256:
`abda89a96a837a0f3fcb917faec42addfed639a72f4f9321fe27313ff550ac77`.
The getter repair and complete battle still need visible runtime verification.

## First visible battle failure and graphics ABI repair

Visible run 01 (PID 605529) reached battle through normal title/New Game input.
The user heard effects/music while the last field dialogue remained visible.
It stopped at guest `800B7204`, reading address `00000A04`, after repeated
`RotTransPers3`/`AddPrim` calls. The core shows scratchpad `sp=1F800370` and a
zero saved object pointer at `sp+50`. Retail call `800B7284` passes a 32-bit FLAG
destination at `sp+4C`; the native PsyCross wrapper's `long` sign-extension store
writes 8 bytes, destroying the next word. No first battle frame was proven.

The same trace has two `ReadTIM` calls followed by `LoadImage` with null pixel
addresses. Native `TIM_IMAGE` has four 64-bit pointers, unlike the retail five-word
layout. `ReadGeomOffset` also writes host longs into adjacent guest words.

`battle_mips_runtime.c` now calls these three native graphics boundaries using
host-local output objects, then serializes exactly the retail-width outputs
back to guest memory. Actual GTE projection and TIM parsing remain owned by the
existing implementations. No textures, projection results or flags are fabricated.

`bash pc_port/tests/run_battle_graphics_abi_test.sh` links production runtime code
and the real TIM parser. It covers CLUT/no-CLUT/failure, guest return pointers,
offset outputs and eight-argument projection marshaling, with adjacent-word guards.
Before fixes the guards at TIM+20, offset+8 and projection+4 failed. O0/O2/UBSan
pass after the fixes; retail parser, getter and projection slices are pinned.
The projection double verifies the ABI boundary, not GTE mathematical parity.

Rebuilt visible run 02: `scratchpad/battle-visible-20260904-02.log`, normal boot,
no scripted input, binary SHA-256
`9d7bf0dc8f57dd09687be3d22674c303fa02bc0623f7fb7ce53d1bc7859968c7`.
First rendered battle frame and full battle completion remain pending.

Visible run 02 instead entered title Continue (choice 1 / dispatcher entry 8).
It called generated stub `func_801D9F98(1,0)`, then crashed in load-menu cleanup:
core PID 637554, `HeapFree(0x1)` from `func_801E5B3C` through `func_801D9E3C`.
This did not exercise the new battle graphics adapters. Continue remains
incomplete; no fabricated cancel/success result or cleanup-skipping workaround
was added. Relaunched the unchanged binary visibly with normal inputs, log
`scratchpad/battle-visible-20260904-03.log`, and asked the user to select New Game.

The replay uses `run_w34d1_opening_a.sh`, selects field 4 after boot movie and
replays Circle presses. It is diagnostic, not natural title/New Game acceptance.
The interpreter executes supplied retail battle machine code; it is not proof
of completion of the native-C battle decompilation.

Run 06 binary SHA-256 (before the additional bridge-map repair):
`e857b44266fa8dae8aeb7acfa84510fe6d9a22d12876e60aa573249f6decc333`.
Build succeeded using `localhost/xenogears-dev-toolchain:current`, running
`bash pc_port/build_port.sh` with this repo bind-mounted at its original path.
The older `xenogears-dev-img:latest` lacked CMake. PsyCross patch replay passed:
26 applied, zero skipped, byte-identical to the live vendor tree.

Unresolved: battle presentation and completion, guest/native bridge closure,
movie flicker/full-run acceptance, village-zoom geometry, portraits, and audible
fire teardown. None of these is closed by the loader tests.

## Visible run 03: first drawing submission and guest OT stride

Normal New Game reached the first battle `DrawOTag`, then crashed (PID 649000).
The user observed a red flash, black screen, then the last "huff huff" dialogue;
this is not a rendered battle pass. Core frame 0 is
`ParsePrimitivesLinkedList`, following a zero link from `006CD044`.
The native draw environment at `006C8FD8` and display environment at `006C9034`
contain an alternating sequence of host OT pointers and zero/padding words.

Trace: battle calls `ClearOTagR(800C4A90,1000)` and later
`ClearOTagR(800C8B00,1000)`. Each retail table occupies `4000` bytes. PsyCross's
native `OT_TAG` is intentionally padded to **8 bytes** for compiled host
`u_long[]` arrays, so those calls wrote `8000` bytes and corrupted the next
battle render context and globals. Changing the vendor stride globally would
break native field tables. The repair is restricted to the battle bridge.

The new regression failed before repair at guest `800C8A90` (the first word
beyond a retail-sized table), receiving a native link instead of `A5A5A5A5`.
The bridge now clears four-byte entries in reverse order and retains the retail
entry-0 link `0005698C`. The supplied EXE sentinel is
`04FFFFFF 00000000 00000000 00000000 00000000`; no replacement data is created.
`ClearOTagR` returns the input table address, as at `80044B44`.

The matching drawing boundary follows validated guest links (and unambiguous
native RAM pointers returned by the existing heap adapter). It copies one DMA
payload at a time, changes only the temporary host linkage to a terminal link,
and feeds the existing renderer in linked-list mode. This preserves merged
packets, unlike `DrawPrim`, which consumes only their first command. Only all-zero
NOP payloads are omitted from rasterization; their links are still followed.
Guest tags and payloads remain unchanged. A single normal scene begin and final
flush preserve batching; invalid, unaligned, truncated or cyclic chains fail
instead of being treated as a successful frame. No vendor file was changed.

Focused O0/O2/UBSan checks cover 1 through 4096 table entries, neighbouring-word
guards, packet order and payload identity, merged packets, the EXE sentinel,
KSEG0/KSEG1/physical/native address forms, disabled GPU behaviour, malformed
packets and a cycle. Renderer-boundary doubles check marshaling, **not pixels**.
The earlier RTP1/RTP4 bridges are now tested alongside RTP3 for the same
32-bit-output/64-bit-native-long mismatch; retail `SWC2`/`SW` outputs and stack
arguments are pinned. The relevant retail SHA-256 slices are also checked by
`run_battle_graphics_abi_test.sh`.

Full build and visible runtime acceptance of these changes are separate gates.
First battle picture, complete battle and full retail parity remain unproven
until a new visible run demonstrates them.

Run 04 was launched normally on DISPLAY `:0`, without a field override or input
script: `scratchpad/battle-visible-20260904-04.log`, PID 700118. Binary SHA-256:
`562bf6ee5ae7d6f866f7c1bf25fd3a8c3130f7b0292212a2221c625770704a22`.
Build: 48 game TUs compiled, none skipped, final link OK. PsyCross replay:
26 patches applied, none skipped, byte-identical. Focused graphics, battle
heap/audio loader, MIPS adapter and retail leaf tests passed. Read-only debugger
sampling confirmed the normal title field 490 loop. The user subsequently chose
New Game; the result is recorded below. Repository-wide `git diff --check` reports pre-existing patch-file
whitespace; the four files changed in this OT repair have no trailing whitespace.

## Visible run 04: missing paired-asset loader

The normal New Game path reached field 4, field 2 and then battle. Two battle
`DrawOTag` calls completed without the previous linked-list crash. This is a
submission observation, **not proof of a visible gear frame**. Run 04 then
aborted after 700106 guest instructions with a `read16` at `00000100`, guest PC
`801E46E0`, inside the loaded archive-file-4 battle module.

Core PID 700118 shows `D_800D3364=0`. Retail battle `80071200..80071210` copies
this from `D_8005949C`, which is also zero in the native symbol. This is not a
missing binding: the traced `func_8001BB0C` calls generated stub `func_800379D8`
and never supplies the data. Its existing C caller also swaps a0 (the byte
resource index) with argument 5 (the third output-pointer address).

`src/slus_006.64/system/asset_loader.c` now implements retail
`800379D8..80037B88`, with its own split at EXE file offset `281D8`. It saves
the archive selection, selects `0xC/3`, changes heap user to 4, and compares the
signed index against `(s16)ArchiveDecodeSizeAbsolute(5)/2`. The valid branch
allocates and pins file `2*index+7+variant`, then file `2*index+6`, submits the
two-entry queue, sets the first output to the first file, clears the second,
and sets the third/shared output from queue entry 1 plus four bytes. The queue
entry is reloaded **after** the archive call, which can sort entries. The
invalid branch clears all three outputs and returns -1, without changing the
shared output. Both paths restore the archive index, not the heap user.

The port-only layout adapter retains the actual queue at guest `8005A1DC`:
three eight-byte entries, 16-bit indices, unchanged padding and 32-bit guest
addresses. The existing archive adapter accepts that representation and reads
the files synchronously. No alternate archive content or dummy pointer is used.
The native caller now uses typed output pointers in the retail argument order
and returns the loader's result. Four output/shared globals are explicitly
bounded as four-byte guest symbols in the bridge map.

`run_battle_asset_loader_test.sh`:

- Before the caller fix, the first case failed with `index=00703050 expected=00`
  instead of interpreting the resource byte. After the fix, all 256 byte values,
  output addresses and the forwarded return value pass.
- The production loader is compared against direct execution of the supplied
  retail instructions with the same deterministic archive/heap boundary doubles.
  953 cases per optimization regime cover byte indices/variants, invalid
  indices, signed-short count/truncation, wraparound arithmetic, queue sorting,
  padding/adjacent guards, call order, returned/shared pointers and return codes.
- O0, O2 and UBSan pass (2859 loader comparisons plus 768 caller cases).
  This is loader/ABI evidence, not proof of file contents, CD timing or pixels.

Pinned retail slices:

- `800379D8..80037B88`:
  `0ab73e7e3353da11925b9db4700117e358e0347fb55fb60a6c111ae6e446ab98`
- `8001BB0C..8001BB50`:
  `da393db3491f47b18db8a84fef891c641551670955975381b634b334d015a128`

## Projection flag union and retail instruction comparisons

The production PsyCross `RotAverageNclip4` wrapper overwrites the first three
vertices' RTPT flags with the fourth vertex's RTPS flags. Retail `8004A8C8`
ORs those flag words. Battle callers at `800B282C`, `800B292C` and `800B29E8`
consume this helper. The battle bridge now executes its actual retail GTE
command sequence through the existing GTE backend and stores only 32-bit
outputs; no projection result or flag is invented.

`run_battle_gte_retail_test.sh` compares RTP1/RTP3/RTP4/RotAverageNclip4 against
execution of the supplied retail instructions on the **same actual PsyCross
GTE backend**. Seven fixtures cover winding, zero area, initial depth flags,
zero/negative depth and fourth-vertex flags. Each compares output bytes/guards,
return values and final GTE register state. Before repair, the first-three-depth
fixture lost flags: native `00000000`, retail `80440000`. After repair all 84
comparisons pass across O0/O2/UBSan. This proves these wrapper cases, not the
GTE backend's equivalence to PS1 hardware. Pinned `8004A64C..8004A8EC` SHA-256:
`489349145f714efe93b292802e106bb31da567da46aa54413098f3c4a528a2c2`.

## Visible run 05

Rebuilt with both repairs: 49 game TUs compiled, none skipped, 89 remaining
generated function stubs, final link OK. The graphics ABI, heap/audio loader,
retail leaf, MIPS adapter, paired-asset loader and GTE comparison tests pass.
Binary SHA-256:
`602c94a5927cf8d5566d8bbead562ad5e711a73d75d3a04df1226d3a4ce064ad`.
Normal visible launch on DISPLAY `:0`, PID 775303, log
`scratchpad/battle-visible-20260904-05.log`; no field override or input script.
Read-only debugger sampling initially confirmed the title field 490 loop.
The user then selected New Game (choice 2); a later sample confirmed field 4.
It subsequently entered battle and stopped as recorded below. The native battle
is still a retail-machine-code adapter, not a claim that the C decompilation is
complete.

## Additional rendering dependency: SetMulMatrix (source/tests only)

While run 05 remained open, the native rendering dependency audit found
`SetMulMatrix` still generated as a zero-return stub. `system/rendering.c` calls
it when a sprite direction includes a transform. This is a definite missing
operation, but is **not identified as the cause of the reported village zoom
artifact or movie flicker**. The preserved movie contact sheets do not establish
the intermittent flash's cause, so neither visual issue was marked fixed.

`retail_leaf_adapters.c` now follows supplied EXE `8004987C..8004998C`: load m0
into the GTE rotation registers, execute three column MVMVA commands using m1,
pack the resulting IR values into the rotation registers, and return m0.
Neither input matrix nor its translations are written. The adapter retains
the retail LW/LH operand widths, including the high halfword passed to VZ0 by
the column-2 LW; the initial comparison caught a raw backend register mismatch
(`00001000` vs `A5A51000`) when that width was not retained.

The GTE retail-instruction suite now adds 136 matrix cases per build: identity,
zero, positive/negative extremes and 64 deterministic mixed-sign fixtures,
each with distinct or aliased inputs. It compares all input bytes and guards,
return pointers and final GTE data/control register state. The negative run
failed because the production owner was missing. After implementation,
O0/O2/UBSan pass all 408 matrix comparisons plus the prior 84 projection cases.
Existing leaf and graphics ABI tests also pass. Matrix slice SHA-256:
`a009644e1a602c9bf37cc61fba50d68c4d375f183ff6d8547055db15258e6b5c`.

This addition was initially kept out of the running game. PID 775303 and the
run-05 executable remained unchanged while the user completed that visible
test. It was included in run 06 after run 05 terminated. Visible behavior of
SetMulMatrix remains unverified; the tests share a GTE backend and are not an
independent PS1 hardware oracle.

## Run 05 result and battle UI packet initializer

User observation: the battle-entry effect played, but there were no gears and
the screen was black. The trace contains 86 `DrawOTag` submissions, passes the
paired-asset loader, then aborts after 6719914 guest instructions:
`refusing generated stub SetLineF2 at retail target 0x80043D78`.
Core PID 775303 has guest return address `801E5CBC`; `D_8005949C` is now nonzero
(`007E022C` host RAM pointer), unlike run 04. The loader repair removes that
earlier null-data fault but does not alone produce a battle scene.

`SetLineF2` is already decompiled in `src/slus_006.64/psyq/libgpu.c`, but that
SDK TU is intentionally excluded from the native port. A native leaf adapter
now performs the same two stores: length 3 at packet+3, opcode `40` at packet+7.
The retail five-instruction span `80043D78..80043D8C` hashes to
`3c2c2f96ba065c302ab697a498aaae48bf32c10c2b8f27bbe21cf3e5c15ebef6`.
The new negative regression failed on the missing production owner. O0/O2/UBSan
then pass all 29 leaf checks; four packet-fill cases confirm every other byte,
including coordinates, link, colour and adjacent guards, remains unchanged.
The graphics ABI regression suite also passes. No battle state is skipped.

## Visible run 06

Normal desktop launch on DISPLAY `:0`, PID 834178, log
`scratchpad/battle-visible-20260904-06.log`, no field override or input script.
Build: 49 game TUs, zero skipped, 87 remaining generated function stubs,
link OK. Both `SetLineF2` and `SetMulMatrix` resolve to actual native owners,
not generated stubs. Binary SHA-256:
`212334d6bb447cfcf0d78d567e401e9f87384f5c4d42293d930d6a02fae11b65`.
Full battle picture/completion and the other reported visual/audio issues
remain unverified.

## Run 06 result: system text pointer width and glyph consumption

Run 06 terminated with SIGSEGV inside native `GetStringEntry`, called by
`func_800338D8`; guest return PC `801E5EA8`, 6736324 instructions executed.
Core PID 834178 shows `g_SystemDataEntries = 007F0998` and adjacent entries
at offsets `48/4C` equal to `007F18C8/007F19E0`. Native code read both as one
eight-byte pointer (`007F19E0007F18C8`). Retail `800338EC` loads only the first
four bytes. The earlier `SetLineF2` boundary now executes successfully.

The fixed-offset and indexed string readers in `system.c`, plus the two glyph
table readers with the same defect, now retain the existing four-byte archive
slots. No pointers, strings or battle state are synthesized. The same audit
found `func_80033ABC` incremented its input cursor only for double-byte glyphs.
Retail `80033AFC` performs that increment in a branch delay slot on **both**
paths. It is now unconditional, so single-byte text consumes an input index.

`run_system_data_lookup_retail_test.sh` executes supplied EXE instructions
`80033728..80033CD0` against the production C readers on synthetic test tables.
The first negative run crashes at the exact live-failure lookup `800338D8`.
After the width repair alone it passes the lookup family but crashes in glyph
decoding; the branch-delay-slot repair removes that second failure. Final O0,
O2 and UBSan runs each pass 2194 comparisons (6582 total): 15 fixed-offset
readers, both indexed readers across 54 table indices, direct string lookup,
eight string indices including header reads -2/-1, 768 glyph-search pairs
covering all 324 table entries and misses, and single/double-byte name decoding
at five lengths including empty and 324 entries. Input tables remain unchanged;
output comparison includes the unused trailing buffer bytes. This is a text
reader/ABI regression gate, **not** proof of a visible Gear battle.

Pinned retail slice SHA-256:
`b4b0df02ca161eee5c67dded72a1615df824ee51074cf15f2ec8a5ecfbcff4f2`.
Local test logs: `pc_port/build_native/system-data-lookup-negative.log`,
`system-data-glyph-negative.log`, `system-data-lookup-results.log`.

## Visible run 07

Normal desktop launch on DISPLAY `:0`, PID 884263, log
`scratchpad/battle-visible-20260904-07.log`, no field override or input script.
Build link OK, 87 generated function stubs remaining. Binary SHA-256:
`327ee3ca0edb10cc3b159086e9021ce657ba4993603eb7e29eeccd047d644956`.
Includes the system-data reader repairs above. Gear rendering and complete
opening-battle playback remain unverified pending the natural visible run.

## Run 07 result: missing timer-task allocator

Run 07 passes the prior text lookup/rendering boundary, processes model data,
and stops after 6723035 guest instructions with unresolved target `8001CD08`
from guest call site `801E70B0` (core PID 884263). This target is
`TimerWorkListAllocateTask`, already decompiled in the matching work-list TU
but absent from the native packed-layout adapter. No Gear picture is claimed.

The adapter now includes retail `8001CD08..8001CD64`: allocate caller payload
size plus the 28-byte task header using `D_800591AF`, register with the existing
timer-list owner, install the delete callback and clear only the payload slot.
The matching delete operation (`8001CE44..8001CE74`) removes the same node then
frees it. The timer registration's task-ID increment now explicitly wraps at
32 bits like retail ADDIU; UBSan identified its old signed-overflow edge.

`run_timer_work_list_retail_test.sh` directly executes the supplied retail
Allocate/Add/Remove/Delete instructions with the same deterministic heap-call
boundary doubles. Before implementation it fails on missing native owners.
After allocation/delete restoration O0/O2 pass, while the task-ID edge fails
UBSan; after the wrap repair all three configurations pass 576 cases each
(1728 total). The comparison covers all 256 heap-flag bytes, original task
bytes and adjacent guards, null/non-null owner IDs, marked/unmarked tasks,
empty/populated lists, head/middle/tail/not-listed deletion, list cursor,
global counts, callback-domain-normalized addresses, returned pointer and
heap-call arguments. Size arithmetic includes zero, negative and signed
extremes. Existing paired-task allocation/callback/cleanup tests also pass.

Retail `8001CC18..8001CE74` SHA-256:
`73e431046377a0795e3627cc44af06c84574a20c3b25032ecd6df2015519697d`.
Logs: `pc_port/build_native/timer-work-list-negative.log`,
`timer-work-list-overflow-negative.log`, `timer-work-list-results.log`.
This proves the tested adapter behavior, not complete battle visuals or timing.

## Nested callback stack isolation (adapter regression)

The newly reachable module function `801E7098` installs guest callback
`801E6FEC` in its allocated timer task. Reviewing the callback bridge before
relaunch found that every nested guest invocation reset SP to `801FFF00`,
even while the outer battle's frames were still live. A synthetic MIPS
regression reproduces the overwrite: an outer saved value `1234` becomes the
callback's `5678` after just one nested invocation. This is a demonstrated
adapter defect, **not a claim that run 07 had already executed that callback**.

The native-call wrapper now tracks and restores its current guest CPU across
reentrancy. Guest callbacks start at that caller's SP and grow downward,
preserving the caller's live stack area; standalone callbacks still use the
normal stack top. No script, task or game-state transition is skipped.

`run_battle_graphics_abi_test.sh` now includes 45 stack cases per build: three
outer and three nested frame sizes, zero through four nested callbacks,
saved words, returned SP and guard bytes. O0/O2/UBSan pass all 135 cases and
the existing graphics ABI checks. The real-GTE retail comparison suite also
passes with the updated wrapper. These synthetic stack fixtures test host
adapter isolation, not scene content or full retail execution equivalence.
Logs: `pc_port/build_native/callback-stack-negative.log`,
`callback-stack-results.log`, `callback-stack-gte-results.log`.

## Visible run 08

Normal desktop launch on DISPLAY `:0`, PID 940437, log
`scratchpad/battle-visible-20260904-08.log`, no field override or input script.
Includes timer Allocate/Delete and nested callback stack isolation. Build
link OK; 87 generated function stubs remain. Both new task functions are
strong exported owners. Binary SHA-256:
`74b496cadc5ecca96c7bfd352b60192c7a524ba18347d301e674d0bc4bc894ad`.
Full opening battle rendering/completion remains pending visible observation.

## Run 08 result and requested testing-workflow change

Run 08 enters the timer callback and reaches battle sprite creation, then
terminates with SIGSEGV in `func_8001EE74`, called through native
`func_800242F4`/`func_8002435C` (core PID 940437). The wrapper currently omits
the first of eight retail arguments and substitutes the same pointer for
destination and package. The core shows destination `007187C8` incorrectly
used as package, instead of actual package `00713F58`. Retail `800242F4`
preserves both a0/a1, forwards five signed halfword arguments, and temporarily
stores the eighth argument in `D_800591B8`. This next repair is identified,
not yet implemented in run 08.

The user also confirmed that battle-only tests should resume just before the
Gear battle, avoiding repeated manual New Game/opening traversal. The current
Save/Load implementation stores only field/game state and player transform;
its safety gate intentionally excludes cutscenes and cannot reproduce this
boundary's active VM/audio/tasks. A real cutscene checkpoint or transparent
recorded-input replay needs an explicit implementation; no field override was
used or described as a savestate. No additional full-boot run was launched
after this workflow request.

User screenshot `/tmp/codex-clipboard-9kXZwC.png` reconfirms bright yellow/orange
triangles during the village zoom immediately before battle. This rendering
defect remains unresolved; none of the adapter regression passes above is
treated as a fix or visual acceptance for that artifact.

## Run 08 sprite-call repair (no additional game launch)

`func_800242F4` now has the actual eight-argument signature: destination,
package, five signed halfwords, and the temporary flags word. Both pointers
are forwarded independently to `func_8002435C`; the invented duplicate-pointer
substitution is removed from both build paths. This is the smallest repair to
the exact run-08 failure, not a claim that all sprite dependencies are closed.

`run_sprite_create_wrapper_retail_test.sh` mechanically extracts the current
production wrapper region and compares it with supplied EXE instructions
`800242F4..8002435C`. The callee is an observed boundary double on both sides;
the sprite initializer itself is not replaced in the game. The negative test
reports package `007187C8` instead of retail `00713F58`, using the core's
actual arguments. O0/O2/UBSan then each pass 65601 cases (196803 total): both
pointers, every 16-bit value in each of five halfword slots, dirty upper
argument bits, full-width flag bit patterns, temporary flag clearing, return
values, caller stack and adjacent memory guards. Retail slice SHA-256:
`f771a8dee7eb81c6a485bcd86185b4962dedbb97eb5b1f40d001871b251997bb`.
Logs: `pc_port/build_native/sprite-create-wrapper-negative.log` and
`sprite-create-wrapper-results.log`.

## Particle-bank ABI repair (source and regression evidence)

Reviewing the yellow-effect path found a separate concrete layout error:
`ParticleBank` occupied `0x80` bytes on the host instead of retail `0x78`.
Its native pointer moved from `+2C` to `+30`, also shifting the remaining
fields. Typed particle handlers used that expanded layout while raw handlers
such as `func_80088B68` and `func_80088C1C` still used retail offsets/stride.
The pre-repair executable allocated `0x400` bytes for the eight default banks;
retail `800A966C..800A967C` strides by `0x78` across `0x3C0` bytes, and
`800A99E0` allocates that same `0x3C0` for a live bank set.

The bank now stores its primitive pointer in a four-byte slot, like the
existing packed field actor types. Accesses explicitly recover the low-address
heap pointer; the standalone host table of bank pointers remains native-sized.
Compile-time assertions enforce bank/primitive sizes and critical offsets.
The allocation path also uses signed-halfword conversion before extracting
its blend mode, matching the retail shift without C signed-left-shift overflow.

`run_field_particle_bank_retail_test.sh` executes the supplied field
instructions against production default initialization, allocation, stop,
stop-banks and free. Heap calls and primitive initialization are observed
dependency doubles, not substitutes in the game. The first negative run fails
at the default-bank bytes with native size `0x80`; the layout repair alone
passes O0/O2 but fails UBSan on the flag shift. Final O0/O2/UBSan each pass
1281 cases (3843 total): 256 initial byte fills with signed actor-ID edges,
all 256 active-bank masks distributed across all 64 bank-table slots, all four blend modes,
allocation/stop/free effects, exhausted bank-table rejection, entire copied
bank/primitive regions, unmodified bytes and allocation guards. This does not
exercise full particle movement/rendering or prove the yellow screenshot fixed.
Field `800A9274..800A9B1C` SHA-256:
`1c5e78294b5415ce389b270d92aea46878e8a42474b2e53a935a0c21acad6da4`.
Logs: `field-particle-bank-negative.log`,
`field-particle-bank-layout-results.log`, `field-particle-bank-results.log`
under `pc_port/build_native/`.

## RotAverage4 output and guest-width repair

The projection helper used by field particles, actor quads and the zoom effect
also disagreed with retail `8004A7BC..8004A83C`. It overwrote the caller's IR0
output with OTZ and sampled/wrote flags in the wrong order when p and FLAG
alias (as they do in `FieldParticleRender`). Its host-long temporaries were
only partially initialized by four-byte GTE stores. The shared decomp body
and port owner now retain RTPT's FLAG, run RTPS, write IR0 then combined FLAG,
run AVSZ4, and return OTZ without another output store. Native p/FLAG outputs
are fully defined host longs; packed XY writes stay four bytes. The shim now
declares this function instead of relying on implicit-int calls.

The MIPS/native bridge separately narrows all six output stores to guest
words in retail order. A negative test after repairing the helper but before
adding this bridge detects an overwritten guest guard at output `+36`.
The earlier negative test detects native IR0 `2710` versus retail `0` in the
front-facing fixture. No depth, color, coordinates or scene state are invented.

The existing real-GTE comparison suite now includes direct native and bridged
RotAverage4 with distinct and aliased p/FLAG outputs, ten winding/depth/FLAG
and depth-cue fixtures, return values, packed-XY guards and full final GTE
register state. Depth cue is tested at nonzero, negative-clamped and
positive-clamped settings, not just the default zero IR0.
All O0/O2/UBSan configurations pass (60 RotAverage4 fixture comparisons, each
checking both direct and bridged execution), alongside the prior projection
and matrix tests. These tests share the PsyCross GTE backend: they establish
the tested wrapper/ABI behavior, not independent PS1 hardware parity or all
possible input/output-overlap patterns. The shared decomp body was repaired
from the same instruction sequence; a new byte-matching MIPS build was not run.
The pre-existing projection slice pin covers this function:
`489349145f714efe93b292802e106bb31da567da46aa54413098f3c4a528a2c2`.
Logs: `rotaverage4-negative.log`, `rotaverage4-bridge-negative.log`,
`rotaverage4-results.log` under `pc_port/build_native/`.

## Pre-battle testing workflow: still not implemented

The requested resume target is immediately before the opening Gear battle,
not Fei's painting room. No normal/full-boot game launch was made during the
three repairs above; unit tests are not being counted as visible acceptance.
The exact-entry hook would be before `func_80070F40` begins executing battle
instructions, after the natural main-loop overlay load and heap reset. Merely
setting a map, entrance, story flag or current game state does not recreate it.

A genuine checkpoint still needs an explicit snapshot/restore contract for
game RAM, external native game globals and pointer relocation, work lists,
sound sequencer/SPU state, graphics state, and any live CD/timer activity.
The current field-only file is insufficient. An arbitrary process-memory dump
is not a cross-build checkpoint. Recorded controller-input replay remains a
distinct possible convenience, not an instant resume or a retail savestate;
neither option has been presented as already implemented.

## Build and regression handoff after these repairs

Canonical container build completed with `compiled=49 skipped=0` and
`LINK OK`. There are still 87 generated function stubs; this count is not
treated as completed decompilation. The rebuilt default particle-bank symbol
is now `0x3C0` bytes. Executable SHA-256:
`29efde378ac3579409dc3d37feabbc0804d6763c83827c3fe8f4b89afa3786de`.
Build log: `pc_port/build_native/prebattle-repair-build.log`.
The run-08 executable was preserved separately as
`pc_port/build_native/battle-visible08-xeno-port` for its crash evidence.

Fresh regression passes also include battle graphics/callback-stack ABI,
field script core handlers and negative controls, primitive-offset handling
and negative controls, the 483-slot/481-handler script-table audit, VSync
presentation, and portrait packet ordering/gates. Logs are under
`pc_port/build_native/` with `prebattle-` or `particle-bank-` prefixes. These
are bounded automated checks, not a full script-semantics or visual audit.
No game is running at this handoff. Gear visibility/completion, the user's
yellow triangle artifact, movie flicker, audio continuity and portrait-color
acceptance remain unverified by a new visible run.

## Particle primitive initialization closure (no game launch)

The independent source audit continued while the checkpoint/replay decision
remained pending. It found three concrete faults in the particle initializer,
not a runtime proof that this is the only cause of the yellow/orange screenshot:

- `800A9058..800A9090` copies one `0x28`-byte FT4 from particle `+50` to `+78`.
  The native loop copied `0x48` bytes, overwriting all four vertices at `+A0`.
  It now uses the retail two 16-byte iterations and two trailing word stores.
- `800A8F80..800A9014` supplies all eight UV coordinates to
  `FieldClampPolyFT4UVs`. The native call omitted its final argument and
  substituted earlier coordinates for the second half of the table. The
  correct arguments and a shared nine-argument prototype are restored.
- Twelve labels beginning at `800AF27C` were separately allocated zero-data
  stubs instead of views into the 21-record, `0x1F8`-byte shape table. The
  port data owner now provides the supplied table with exact two-byte aliases,
  plus the following eight-byte direction table at `800AF474`. Both tables
  are compared byte-for-byte with `disc/field.bin`; no shapes were invented.

The negative coordinate differences also exposed C signed-left-shift UB.
Multiplication by 16 preserves the retail shift/subtract result for these
unsigned-halfword inputs without shifting a negative signed operand.

`bash pc_port/tests/run_field_particle_primitive_retail_test.sh` compiles the
production `misc5.c`, UV helper in `misc4.c`, and `data_field.c`. Four GPU leaf
bodies are mechanically extracted from the current PsyCross source so this
unit test needs no desktop/GL lifecycle. The comparison CPU executes the
supplied initializer, UV helper, and EXE GPU leaves with **no function bridges**.
The test's weak zero-data objects only reproduce missing production ownership;
they are rejected by the data gate and are not linked into the game.

Observed negative controls, under `pc_port/build_native/`:

- `particle-primitive-data-negative.log`: noncontiguous placeholder label 1.
- `particle-primitive-function-negative.log`: polygons and vertices differ;
  first UV difference at particle `+101`, native `00`, retail `40`.
- `particle-primitive-copy-negative.log`: after the UV repair, polygons match
  but vertices still differ (`+160`, native `30`, retail `00`).
- `particle-primitive-shift-negative.log`: copy repair passes O0/O2, but UBSan
  reports the negative `-64` shift before the arithmetic repair.

Final O0/O2/UBSan each pass 21,504 cases: all 21 supplied shapes, all four
blend modes and 256 byte-pattern fills. This is 64,512 comparisons of the
complete `0xC0` particle, preserved bytes and surrounding guards. The test also
checks the retail stack/callee-saved registers and all 512 source-table bytes.
It does not test invalid shape indices, particle motion, GTE transformation,
ordering-table submission, texture sampling or an actual rendered scene.

Pinned slices (the runner rejects changed authority):

| Source range | SHA-256 |
|---|---|
| field `800A8EAC..800A90B4` | `7fa217a406233868cd5c85d076009e48f522c4476a290ec87954a2bd63e5b203` |
| field `8007A44C..8007A5C4` | `c9ea0445847f175c48c6319e55528bdc6cb928747fcae4fd79e286fd74919c3c` |
| field `800AF27C..800AF47C` | `9b8952b872ecf0d3c52d255fe6bb72c4002efeb518fc03b94c212c1da2760937` |
| EXE `80043A1C..80043CC4` | `fb063ed80a14270d731dc1f70fe90a5d32e9d79dc9b5c93796ffcab9a72072d8` |

The canonical container build passes, `compiled=49 skipped=0`, `LINK OK`.
The actual executable has initialized shape-table aliases at `005F4900+2*i`
and the eight-byte direction data at `005F4AF8`, not the prior BSS placeholders.
Executable SHA-256:
`dddbeada75d3bbb6c952ab69d5a5749e9e772f282a7a8114cf3de60687e763b4`.
Logs: `particle-primitive-results.log`, `particle-primitive-build.log`.
Fresh adjacent regression logs use `particle-primitive-*-regression.log`:
particle-bank lifecycle, script primitive offsets and mutants, render-context
DRAWENV/DISPENV layout, battle GTE wrappers, battle graphics/callback ABI,
core script handlers and mutants, and the script-table census all pass.
There are still 87 generated function stubs; the 13 retired data stubs are
not evidence of a completed battle or complete decompilation.

Next independently identified rendering frontier: `FieldParticleRender`
inserts through typed `RenderContext.ot1` (native offset `D0`, stride 8),
whereas retail `800A9E90..800A9EEC` and the current raw field clear/submit
path use offset `CC`, stride 4. The current render-context regression tests
DRAWENV/DISPENV compatibility, not this mixed OT contract. This mismatch
is identified but **not repaired or covered by the initializer test**.
No headless or visible game run, state override, checkpoint implementation,
save modification, commit or push occurred during this increment.

## Particle ordering-table insertion repair

The next increment repairs the mixed OT contract identified above.
`FieldParticleRender` now inserts into packed `u32` slots at context `+CC`,
matching retail `800A9E90..800A9EEC` and the existing field clear/submit path.
It no longer uses native `RenderContext.ot1`, which starts at `+D0` and has an
eight-byte stride. Sort decisions, projection, color and scene state were not
changed. The other native RenderContext adapters remain unchanged; this is
not a claim that every typed/raw consumer has been reconciled.

New command: `bash pc_port/tests/run_field_particle_render_ot_retail_test.sh`.
It compiles the complete production particle TU and compares the render
wrapper with supplied field instructions `800A9B54..800A9F18`, pinned to
SHA-256 `d1272f8027d3f58fc692d437bdcd7e701f58d23fbfeeeeb41f46ae7016c1df38`.
Both sides use the same production native matrix helpers. A test-only
projection boundary supplies controlled XY, depth and FLAG outputs so the
ordering behavior can be exhaustively exercised at every valid bucket.
This does not prove those projection outputs correspond to a real scene,
or establish independent PS1 GTE/matrix hardware parity.

O0/O2/UBSan each pass 35,720 cases (107,160 total): depths `0..4112`, all
four sort modes and both buffers; eleven signed depth edges under all 32
legal shift counts; both matrix branches, with type flags distributed across
the cases. Entire native context/particle regions and surrounding guards are
compared, including untouched bytes and packet/tag header preservation.
The original failure is at context `+208`: native `A5`, retail `30` for
depth 0, top sort, buffer 0. Separate generated-copy mutants for wrong offset
and wrong stride both fail with an OT-byte mismatch after the repair.

A diagnostic also found `CompMatrix`'s unused SVECTOR high-half padding in
the shared backend's raw VZ backing word (`005A0004` versus `00000004`).
`INLINE_C.C::MFC2` sign-extends the low halfword of VZ on read; GTE arithmetic
also consumes its signed low halfword. The test therefore compares all 32
data-register readbacks through MFC2 and all control-register backing words,
not unused native padding. A readback check distinguishes a meaningful VZ
change (`4` to `5`). No production GTE behavior was changed to pass this test.

Logs under `pc_port/build_native/`: `particle-render-ot-negative.log`,
`particle-render-matrix-negative.log`, `particle-render-ot-results.log`;
the runner retains its `offset.log` and `stride.log` negative controls.
Fresh particle-bank and primitive-initializer regressions pass, logged as
`particle-render-bank-regression.log` and `particle-render-primitive-regression.log`.
Canonical build: `compiled=49 skipped=0`, `LINK OK`; still 87 function stubs
and 575 data stubs. Log: `particle-render-build.log`. Executable SHA-256:
`07a59ef228b7e3557a4395fca24680f0addb500dcd555e411d9386d40344d80b`.

The repaired binary has not been launched. The screenshot artifact, visible
Gear battle, audio continuity and full opening flow remain pending runtime
acceptance. The pre-battle checkpoint/replay choice is still unresolved;
neither workflow was silently implemented or substituted by these unit tests.

## Native mesh-normal dependency: VectorNormalSS

The subsequent user delegated the workflow choice; `SPEC-prebattle-resume.md`
now selects a genuine checkpoint. The active introduction-battle goal continues
independently of that unfinished feature. No fresh gameplay capture is claimed.

`func_800302D4` in `system/temp2.c` adjusts model normals then calls
`VectorNormalSS(pNorm, pNorm)`. That symbol was still a generated zero-return
stub. Its use in the live opening battle has not been observed, so this is a
confirmed model dependency repair, not a proven explanation of the black scene.

Added the native owner in `pc_port/src/psyq_compat.c` and its shim prototype.
It follows retail `80048DA8..80048E94`: signed halfword inputs, SQR0, summed
MACs, LZCS/LZCR normalization, table lookup, GPF0, arithmetic variable shifts,
and three halfword stores. It leaves the SVECTOR padding unchanged and supports
in-place calls. The existing `VectorNormalWork` was not reused or modified:
that host-arithmetic routine does not preserve the required GTE side effects.
The zero vector's actual lookup reads `80056B14 = 1C6C`; returning zero without
executing the GTE instructions would not preserve state.

Retail ADD overflow is not recoverably emulated by this adapter. It reports
the exception and aborts, rather than inventing output or indexing beyond the
table. This is an explicit unsupported exception boundary, not full emulation
of the PS1 exception handler. Ordinary nontrapping inputs follow retail code.

Differential tests extend `battle_gte_retail_test.c` and its runner. The test
first failed with `NORMAL SS FAIL: missing production owner`. At O0, O2 and
UBSan, each run now passes 524,288 cases: every signed-halfword value on each
axis plus mixed-component vectors, each with separate and aliased output.
Comparisons cover the complete 32-byte input/output/guard region, return word
and all GTE register backing state. Two additional near-overflow fixtures pass;
three child-process overflow cases explicitly terminate with SIGABRT without
core dumps. The CPU interpreter does not model signed ADD exceptions, so those
three are fail-closed adapter tests, not differential exception proofs.
Existing matrix and projection cases also pass. Both sides share PsyCross's
GTE backend; these are wrapper equivalence tests, not independent hardware proof.

Authority SHA-256 pins checked by the runner:

- `80048DA8..80048E94`:
  `2d59f42c6e5c2bf222028ae7d4c3abc908940a2ba5361e838a5acde0babc1df4`
- `80056B14..80056D14` lookup data, including zero-input predecessor:
  `7e7e21906ffb76fd4478baab9b5c10dafc148d2a7c99d22d92939262bb8db417`

Logs: `pc_port/build_native/normal-ss-retail-results.log` and
`normal-ss-build.log`. Canonical build: 49 game TUs, zero skipped, LINK OK,
86 function stubs and 575 data stubs. `VectorNormalSS` is a strong text symbol
at `0051ED1C`, no longer a generated fallback. Current executable SHA-256:
`b082c8cd98ed879c57e66a272cefa9670391510c9898eb15854406a9afb24c75`.
Focused edited-code whitespace check passed. No save/recording was altered,
no game was launched, and no commit/push was made. Gear visibility and normal
opening-battle completion remain unverified.

## Visible run Ps7dkSGz: native task callback round trip

Ran binary `b082c8cd98ed879c57e66a272cefa9670391510c9898eb15854406a9afb24c75`
on the real desktop, PID 1364400. No map/test-boot override or story-state
write. The existing field confirm hook supplied 2-on/12-off Circle pulses;
this is synthetic input, not a checkpoint or recorded replay. The normal
title/field 490 -> prologue/field 4 -> battle path executed. User reported
seeing fire move across the screen. Capture 005040 shows localized village
fires; that single image does not prove the entire zoom effect correct.
Captures 005760/006000 show red/black transition frames, not visible Gears.

Log and raw captures: `scratchpad/gear-visible-Ps7dkSGz/`. Capture filenames
end in `.png` but contain BMP bytes; `view-*.png` files are lossless format
conversions for inspection. The matching executable was preserved as
`pc_port/build_native/gear-visible-Ps7dkSGz-xeno-port` before rebuilding.
Orca's CLI worked, but its desktop process exited after startup, so no Orca
keyboard or screen action succeeded. Field captures come from the port's
existing screenshot hook. No headless game run was used.

The run passed the previously failing sprite-wrapper call many times, then
aborted on callback `800BB620`: at `800BB5E0`, retail `JALR v0` reads the free
callback from task+0C. The target was native `00522D51`, confirmed by the
matching ELF as `TimerWorkListDeleteTask`. `TimerWorkListAllocateTask` stores
that native pointer in its packed callback slot. The bridge map already
registers the routine at retail `8001CE44`, but the resolver previously only
looked up the incoming target as a retail address.

Repair: when retail-address lookup misses, match the full native pointer
against existing registered function entries. Reuse that entry through the
same ABI and generated-stub checks. Unknown executable addresses are not
accepted; wider host pointers are not truncated to find a match. Task state,
retail instructions and battle decisions were not altered.

Extended `battle_graphics_abi_test.c`: an exact registered-host callback first
failed before repair; it now passes at O0/O2/UBSan. Unknown and truncated
pointer negative controls remain rejected. Existing nested-stack, graphics
ABI and retail-slice checks pass. Logs: `registered-callback-negative.log`,
`registered-callback-results.log`, `registered-callback-build.log` under
`pc_port/build_native`. The callback test uses a controlled host callee and
proves dispatch/argument/return handling, not the task deletion algorithm;
the latter has separate existing retail work-list tests.

Canonical build succeeds with 86 function stubs and 575 data stubs. New binary:
`4801e0febf0872b53d85062a1f9a05b1683c78ee157e45cdbd6339531b8913e4`.
The exact observed failure is repaired in source and isolated tests; live
continuation past it and visible Gear battle completion still require a rerun.

## Visible run QL0aeha6: callback repair observed; atlas builder missing

The same visible diagnostic configuration ran binary
`4801e0febf0872b53d85062a1f9a05b1683c78ee157e45cdbd6339531b8913e4`,
PID 1391909. The title/prologue chain advanced without forced map or story
state. Log line 357828 confirms the previously failing indirect callback now
dispatches `TimerWorkListDeleteTask` through host address `00522DE9`. A later
direct retail-address call also completes. This is live evidence for that
repair, not whole-battle acceptance.

The run then aborts after 7,021,886 guest instructions on unresolved function
`80025FA8`, called by battle `8007680C`. The source still has INCLUDE_ASM for
that function, with no native owner. The matched executable is preserved as
`pc_port/build_native/gear-visible-QL0aeha6-xeno-port`; systemd core PID 1391909
was inspected before any further rebuild. Logs/captures/core inspections are
under `scratchpad/gear-visible-QL0aeha6/`.

Disassembly establishes a nine-argument atlas-to-POLY_FT4 builder:

1. Table pointer, entry index, output packet pairs, current buffer index.
2. Screen offset X/Y, scale X/Y, Z rotation (stack arguments, narrowed to
   signed halfwords where retail does so).
3. Copies the 32-byte identity matrix at `800188CC`, saves matrix/geometry
   state, scales/rotates it, then projects four corners per 28-byte atlas item.
4. Selects 40-byte packets within 80-byte double-buffer pairs; initializes
   primitive/tpage/CLUT, handles mirrored corners, and applies the retail
   rotation/projection-dependent UV adjustments.
5. Restores geometry offset/screen and matrix stack; returns signed item count.

The actual failed call from core is:
`{006DC7CC, 52, 006D5864, 0, DD, 22, 800, 800, 0}` (hex).
The selected entry is at `006DE5D4`, count 1. Its 28-byte item halfwords are:
`007C 00F7 0074 0008 FFC8 FFFB 0004 0000 0000 0080 01F1 03DF 00F7 0000`.
These are local diagnostic observations, not a replacement asset or a request
to hard-code the captured values. The next implementation must remain generic
over the supplied retail atlas, packet buffer and transform arguments.

Retail function bytes `80025FA8..80026338` SHA-256:
`a0f6f0de866dd0c2b8d4f7ae0733a3ba4b5586be3a4bb5ca2f49dd96b0fb5b78`.
Dependencies include PushMatrix, ScaleMatrixL, RotMatrixZ, ReadGeomOffset,
ReadGeomScreen, SetGeomOffset/Screen, SetRotMatrix/TransMatrix, SetPolyFT4,
SetSemiTrans, SetShadeTex, GetTPage, GetClut, RotTransPers4 and PopMatrix.
Next: implement this exact function and differential-test packet contents,
buffer selection, UV edges, geometry restoration and the captured call shape.
No new production edit was made during this diagnostic run. Game is stopped
after SIGABRT; Gear visibility and full introduction completion remain open.

## Atlas quad builder 80025FA8 implemented

Added the nine-argument native C body under `XENO_PC_PORT` in decomp-owned
`src/slus_006.64/system/temp1.c`. The matching MIPS lane retains INCLUDE_ASM;
no byte-match claim is made. The function consumes its supplied atlas and
uses the existing RAM adapter for retail identity/corner data at `800188CC`
and `8004FDC0`. It does not embed the captured entry, substitute artwork,
force battle state, or change the retail projection/UV decisions.
`pc_port/include_shim/psx_memory.h` exposes the existing adapter to game TUs;
the first full build caught that missing include path and was not allowlisted.

`atlas_quad_retail_test.c` executes the actual 0x390-byte retail routine and
the production C body mechanically extracted from its source. Retail GPU
leaf instructions execute directly; both sides use the current PsyCross GTE
SDK boundaries, including the existing packed-output projection bridge.
This proves the new routine against those shared hardware adapters, not PS1
hardware accuracy of the adapters themselves.

The initial test failed on the missing production owner. Review found that
the first fixture revision zeroed corner Z, so it was corrected to use the
actual retail corner block (Z=1000), then rerun. The final 1,536 fixtures per
O0/O2/UBSan cover counts 0..3, both buffers, both mirrors, six rotations,
positive/negative scales, index 0 and 52, and zero/nonzero UV origins. Index
52's one-item, unmirrored, rotation-zero, scale-800 case uses the captured
call shape and descriptor values, but the production body remains generic.
Comparisons cover packets/guards, unchanged atlas, shared corners, complete
GTE backing state and return value. Valid packet buffers and nonnegative
item counts are the tested domain; malformed atlas acceptance is not claimed.

Two generated-source negative controls fail as required: buffer stride 40->80
at fixture 5, and omission of the C00-angle UV decrement at fixture 97. The
runner pins the function, identity block, corner block and GPU leaf bytes.
Logs: `atlas-quad-results.log`, `atlas-quad-build.log`, and
`atlas_quad_retail_test/{buffer,uv}.log` under `pc_port/build_native`.

Canonical build: LINK OK, 86 generated function stubs, 575 data stubs.
`func_80025FA8` now exports a strong text symbol at `004E4F32`. The unchanged
stub count is expected: this previously unresolved entry had no linked stub.
Binary SHA-256:
`2413fde41d11d3d496c96a6d2eb1143bc02e0d482e2570fa70884f224158bed0`.
Visible rerun output is assigned `scratchpad/gear-visible-JC1by97l/`; its
outcome must be inspected separately. No full battle acceptance is claimed.

## User-prioritized startup movie selector audit and correction

The user reported the wrong startup FMV and requested that check before more
battle work. Stopped our diagnostic PID 1458492 (JC1by97l) with SIGTERM for
this change; that run does not prove the atlas repair live.

Retail EXE `800198C0..800198D4` compares ArchiveGetDiscNumber's result to 1,
selects 16 on equality and 7 otherwise, and stores that in `D_8004FE45`.
`D_8004FE44=1` is the stream type, not movie number. `MovieMain` copies FE45
to its MOVIE NUMBER variable `D_8007711C`. The port incorrectly assigned
ArchiveGetDiscNumber directly to FE45; comments in both boot and MovieMain
also mislabeled it. Corrected the mapping via `PcPort_SelectBootMovie`, the
production assignment, comments and boot diagnostic labels. No guessed movie
selection was substituted for the retail branch.

The focused boot-route test covers disc 1, disc 2, zero and -1. Its runner
checks the actual production call site and pins the 24-byte retail branch:
`29ee614fc352b8704ea1eb96ff8247947619ef368a95196073a03f2300c0422d`.
A generated header with the old disc-number mapping fails the disc-1 case.
Tests and full native build pass; build log is `boot-movie-selector-build.log`.

Binary `895523a9aea4126d17487f95eabcd3f5f1f53f2e421920b0f0c6721518fd7eba`
was launched visibly, without synthetic input, as PID 1480391. Evidence:
`scratchpad/boot-movie-check-nphB4gbh/`. Live log confirms type 1/movie 16 and
CD sector 106105, rather than the previous type 1/movie 1 at sector 825.
Movie frames 30 and 120 were captured; this is not verification of every
frame, timing, sound or the complete startup sequence.

The run subsequently reached title field 490 and menu, then terminated with
SIGSEGV after `[stub] func_801D9F98`. That separate title-menu failure remains
unresolved. No game process remains from this run; do not report it as still
playing or treat the selector fix as full startup/battle acceptance.

## Visible atlas progression and timer lookup dependency (8001D0A4)

Resumed visible normal boot/New Game as PID 1519509 with the same `895523...`
binary. Evidence: `scratchpad/gear-visible-Dtcy83Sp/`; matching executable
preserved as `pc_port/build_native/gear-visible-Dtcy83Sp-xeno-port`. This run
logged successful repeated `func_80025FA8` calls for atlas indices 0x52/0x53
and both buffers. The user saw Fei/Citan health bars, reported startup/New
Game usable and thought the orange artifacts were gone. Missing title
background remains reported. These observations do not establish full scene
fidelity or correct HUD visibility.

The run aborted after 7,297,371 guest instructions at unresolved `8001D0A4`,
called from `800BCE14`. Core inspection with the preserved executable gives
a0=0074F00C, a1=800BCFAC, a2=0000E214, a3=0, SP=801FFE28.
`core-backtrace.log` and `core-lookup-args.log` are in that run directory.

Retail `8001D0A4..8001D108` (0x68 bytes) is a read-only first-match timer-task
search: owner equality, equality of the lower 29 bits at node+14/owner+10,
then raw callback identity at node+8; next pointer is node+18. SHA-256:
`4d2e3d0559a80d25efb325eadf0c64edacc7fabd24e3147a3ade1cfd7c0f1090`.
The existing decompiled `work_list.c` routine uses host pointer dereferences;
the native build excludes that translation unit in favor of packed 0x1C
entries in `work_list_port.c`. Added the equivalent lookup to that adapter,
reusing its null-owner RAM+10 alias and 29-bit owner-id helper. The runtime
bridge now preserves this function's a1 as callback identity, rather than
translating it into a pointer to instruction bytes. No scene state or HUD
condition was changed.

Tests first failed on missing native owner and callback mistranslation
(`800BCBB4` received as host `004CCC54`). Extended the actual-retail execution
test to 30,721 lookup cases per O0/O2/UBSan: lists of length 0..3, individual
predicate misses, three low-bit IDs, all 8x8 upper-flag combinations, five
callback values including the observed `800BCFAC`, null/non-null owners and
duplicate first-match order. Compare results and unchanged node/owner area,
list heads and RAM+10. This is not an all-RAM mutation proof; static review
also verifies no stores. Existing 576 allocation/delete cases per build and
the full focused graphics ABI suite pass. A generated no-callback-comparison
mutant fails case 28,800. Logs: `timer-lookup-{red,results,negative}.log` and
`timer-lookup-abi-{red,results}.log` under `pc_port/build_native`.

Independent read-only review found no implementation contract violation;
its null-owner RAM+10 test-guard gap was addressed. Canonical build LINK OK,
86 generated function stubs/575 data stubs, strong native lookup at 005236A8.
Binary SHA-256:
`6979ae5aefc6f871fd196d77c8effd701b7cb7dd10d2b0f882a2f6dc371f88d0`.
Next visible run: `scratchpad/gear-visible-IcDIFWBe/`; inspect its outcome
before claiming the lookup passed live or the Gear scene works.

### Scripted sequence / HUD evidence boundary

The user's intended scene is the scripted Gear sequence, including Fei's
"Hiyaaaaa!" close-up, not a substitute normal playable encounter. The
reference video is https://www.youtube.com/watch?v=UO5f7K_dOEU&t=479 ; the user
specified 7:59 as the start marker. Initial fetches were throttled; the later
short URL returned page metadata but no video frames. No video comparison
is claimed. The user-provided screenshot is
visual reference, not evidence that this native run reached that phase.

A parallel read-only trace found field battle opcodes in `misc11.c` set
`D_80059508` from script data and explicitly set `D_800594F8=0`. Field exit 0
enters state 2; `func_8001B6C4` initializes and calls retail `80070F40`.
Retail branches on F8 at `80071034/80071050`; this run takes the zero path.
The entry copy source 800658FC, from the retail calculation
800658DC+(D_80059508<<5), proves selector 1. HUD primitives originated in
executed retail battle code. This does not prove their appearance is wrong
at that early phase; the later hide condition remains unresolved, and the
run aborted before reaching the reference phase. Do not suppress the HUD
or change the mode based solely on the screenshot.

Follow-up trace located a specific retail hide mechanism. The per-frame
`func_800BE790` calls `func_80076544` at 800BE90C; mode byte D_800C3E4C=1
selects `func_80076418`. At 80076418..80076424 that function returns early
when D_800C492A is nonzero, skipping the HUD submission suite. Otherwise,
`func_80073538`'s state-2 path reaches `func_80076710` and the observed atlas
calls at 8007680C/80076884. Dispatcher `func_800B3F04`, command 0x6A (table
jtbl_80070850 index 105, indexed by command-1), writes D_800C492A=1 at
800B3F68..800B3F6C. The normal mode itself is selected at 80071300..80071304
when D_800D2FC4=0. This establishes a script-controlled suppression path,
not that command 0x6A has been observed in this run, nor that its timing
corresponds to the user's particular reference frame. Script operands and
that phase's exact source-data/camera path remain unverified.

The preceding boot-check crash is separately localized to Continue/Load:
`func_801D9F98(1,0)` is still unimplemented, and cleanup in `func_801E5B3C`
attempted `HeapFree(1)`. It is not a failure of New Game. No cleanup bypass
or successful-load fabrication has been added.

## Lookup live pass; black geometry / script activation remain open

Run IcDIFWBe (PID 1676568) selected real New Game, entered field 4 and
successfully called `func_8001D0A4` for seven task owners with callback
800BCFAC. It continued into the battle frame loop instead of aborting there.
The user confirmed the menu is operable. Capture `field-frame-007920.png`
(BMP format despite suffix), converted losslessly to `view-007920.png`,
shows Fei 1800/fuel 1200, Citan 200/200 and Defend/Attack over black geometry.
This is explicitly a failed visual/cutscene acceptance, not scene completion.

Read-only live debugger observations in `live-battle-state.log` and
`live-battle-stage.log`: native D_8005947C=0, D_80059508=1, D_800594F8=0,
D_8005954C=5, D_800B2356=5; guest BSS D_800C492A=0, D_800C3E4C=1,
D_800D2FC4=0. The raw RAM dump of 8006F9DC is not asserted to equal effective
shared-symbol storage; that address is below the adapter's BATTLE_BASE.

Formation descriptor source D_800658DC is field-loaded BSS, not an EXE
table. FieldLoad decompresses the map's walkmesh section into base+10.
Selector 1 therefore refers to map 4's source-data record. Separately, field
initialization defaults D_800B2356=5; extended field opcode FE B8 can change
it (nonzero destination selector), and the battle opcode copies it to
D_8005954C. Retail `func_800B8098` receives that stage selector at
80071158..80071164. Value 5 uses `func_800B7870`; values 0..4 choose other
loaders. Whether map 4 intentionally retains 5 or a required opcode was
missed remains unresolved. No selector modification is justified yet.

Concrete black-render frontier: the live log repeatedly reports
`missing D_8004FE50 prim=1 variant=1; skip group` (first at line 362067).
Model calls are reaching `func_8002C700` with variant 1, but native descriptor
row 1 has no walker there. The corresponding retail row at 8004FE78 contains
8002F2E0 in slot 1. Implementing and differentially verifying that exact
walker is the next rendering lane; do not map it to another variant just to
produce geometry. Other missing groups may still be hidden by the first-8
log limit. This finding explains omitted geometry groups, not yet every
black pixel or missing cutscene action.

After preserving the verbose trace, changed only the live port diagnostic
`g_BattleRuntime.trace_calls` from 1 to 0 through GDB to stop rapid log growth;
see `disable-call-log.log`. No gameplay memory, mode, pose or script state
was set. The visible process was left running, not rebuilt underneath it.

Map-4 operand follow-up falsified the missed-FE-B8 hypothesis. Retail
`DiscArchive` map entry `FIELD_MAP_FIRST_ENTRY+4*2` payload SHA-256:
`bc36e949ec3e4c222ca886379ffe23f4827599a3245514836c94537e10c05ea0`.
The scripts offset is 0x9E4 (map+144), decompressed size 0x1D57, 14 scripts,
code start 0x404; ScriptsFile SHA-256:
`069e624f6528a3ad00e56b4d524e4f00b8c8781826e0c9d24b59b55840b7256e`.
The independent extractor's exhaustive byte search found no FE B8 sequence
anywhere in the decompressed scripts. Thus the default stage selector 5 is
consistent with the original map script, not a missing override to fix.
This does not yet validate every other script or battle descriptor consumer.

## Retail NCS triangle walker 8002F2E0

Implemented the native body of `func_8002F2E0` in decomp-owned `temp2.c` and
wired its retail descriptor row 1/slot 1 in `game_overrides.c`. The MIPS
matching branch retains INCLUDE_ASM; no objdiff match is claimed. The walker
performs RTPT/NCLIP/AVSZ3/NCS for lit 0x20-byte triangle packets, preserving
lookahead, final RTPT on count zero, partial XY writes for culled backfaces,
the normal load in the OTZ-zero delay slot, command byte during RGB update,
normal/output cursors and the actual y-bound return value. DMA-address
conversion stays in the existing `PcPort_LinkModelPrim` hardware adapter.

The new `model_prim_f2e0_retail_test` initially failed on missing production
owner. Seven explicitly checked fixtures cover counts 0..3, a lit/linked
face, a backface's partial writes, forced zero OTZ, screen rejection, packed
vertex-index upper bits, lookahead and shift 31. Each compares output/guards,
OT memory, relevant globals/cursors, source regions, return and complete GTE
backing state against execution of pinned retail bytes. Both sides share
PsyCross GTE; this is not an independent hardware oracle or exhaustive input
coverage. O0/O2/UBSan pass. Generated mutants for terminal RTPT, normal V0,
partial XY and packed-index masking are all rejected. The runner pins:

- 8002F2E0, 0x1D4 bytes:
  `648b7bb0e8ca0474b0cd2fd2f44fa006d965ac3475a4a8239e3625ec4032a540`
- Shared tail 8002E1F4, 0x38 bytes:
  `b7d98d849c2248e0da88f393f8e57a1f9b42df3c4d568ab3c194b01b91f5e2de`
- Descriptor row 8004FE78, 0x28 bytes:
  `f170e816fee8ecc43deafc02e7b3c7104bc63edb9ff325d79230cb3b209502c0`

Independent read-only instruction review found no contradiction. The first
full build failed because several original `gte_ldv*` macros emitted MIPS
assembly in the game TU (the isolated test uses PsyCross headers). Replaced
those loads with the existing MTC2 host register adapter; repeated focused
tests and full build pass. No allowlist or skipped TU was added. Logs:
`pc_port/build_native/model-prim-f2e0-{results,build}.log`; negative-control
logs are under `model_prim_f2e0_retail_test/`.

Canonical binary SHA-256:
`7dc69004c9dc3548742090b353087826c19c84d6d129107fc9f6a929a2d8a46f`.
LINK OK, 86 function stubs/575 data stubs. Previous process IcDIFWBe had
already exited when checked for shutdown; its matching executable was
preserved. New visible normal-boot run `scratchpad/gear-visible-ykMlGYy6/`
also uses GDB's `formation-watch.gdb` to capture field-4 source/output after
decompression and observe subsequent writes. No gameplay values are set.
Its actual visual and watchpoint outcomes remain to be inspected.

Latest run check: PID 1776970 remains alive under the diagnostic GDB session;
the log shows title field 490/menu choice 1, with no `title confirm` or
formation-watch capture yet. The automatic field confirm schedule does not
select New Game inside the separate title menu. User selection is pending;
do not claim the new renderer has been observed in the Gear scene.

Parallel formation provenance work found a pre-battle discrepancy. The raw
Map4 +148 section (offset 1394, decoded size 212 hex) has SHA-256
`57d6d043b399d1f6529d6be8f8dde76da6cd24d16d560faa407e188d799d71ed`.
Expected record-1 tail from source data is
`0b 00 1c 00 00 00 00 00 80 ff ff ff ff ff ff ff`; live IcDIFWBe host
source and copied guest destination both instead contain
`09 a0 12 01 00 00 00 00 80 82 82 82 82 ff ff ff`.
See `live-formation-bytes.log` in IcDIFWBe. No legitimate field-side writer
has been identified; do not call this an intentional rewrite. The fresh
run's post-decompression capture and first-write watchpoints are intended
to distinguish wrong compressed input from later corruption. This remains
an unverified cause of the ordinary command-menu behavior.

## Fresh-load trace resolves descriptor discrepancy; native GTE include repair

Run `gear-visible-ykMlGYy6` did subsequently select New Game (choice 2).
The captured `map4-live.bin` exactly equals retail entry 0xC0, including its
0x19E0 length. `formation-after-load.bin` exactly equals the retail decoded
0x212-byte section. All four first-write watchpoints fired in
`LZSSDecompress -> FieldLZSSDecompress -> FieldLoad -> func_800A5C40` during
the subsequent load of field 2. They did not identify an out-of-bounds writer.

The previously disputed bytes are exactly field 2's own formation data, not
a demonstrated mutation of field 4's formation. Retail field-2 entry 0xBC
has size 0x1E3A0 and SHA-256
`d81670fa78aeed510852153349cd8d4e4b5a6bc6469110e41fb6274aa4e4261f`;
its decoded formation section has SHA-256
`727bad94d380db9065d190fb7c28cc422549030dcaab9edee894b186fbbf85e9`.
Thus the earlier comparison used the wrong map's descriptor as its expected
value. This supersedes that suspected-corruption explanation; it does not
establish the correctness of the later battle script or HUD phase.

The same run then faulted at native `func_8002F2E0` address 004EAA0E.
Disassembly shows `gte_rtpt()` emitted PsyQ's `.word 0x000000bf` placeholder
into x86 code instead of calling the GTE adapter. x86 decoded it together
with following instructions, so the crash was not a bad vertex pointer.
The previous extracted-body tests pre-included PsyCross's correct header;
they never tested the production translation unit's own include path.

Strengthened `run_model_prim_f2e0_retail_test.sh` to compile the complete
production `temp2.c`, using its own includes, for O0/O2/UBSan. This reproduced
SIGSEGV in the first O0 run (exit 139). Corrected only the native include
selection in `temp2.c` to use `psx/inline_c.h`; the matching branch retains
the original PsyQ header. All seven retail differential fixtures now pass
through the complete TU at all three configurations, and all four existing
mutants remain rejected. Graphics ABI regressions pass. Logs:
`model-prim-f2e0-production-tu-{red,results}.log` and
`f2e0-header-graphics-regression.log` under `pc_port/build_native`.

Canonical rebuild: LINK OK, 86 function stubs/575 data stubs. Binary SHA-256
`2d6477fb8693633cec63240e8c60165ec08532d603e5e55ec15b892379cd0d82`.
Preserved the previous executable as `gear-visible-ykMlGYy6-xeno-port`.
New visible normal-boot run is `scratchpad/opening-field-battle-SSBmzyaE/`;
its debugger records field-2 dialogue, model submission counts, camera and
fade state, and battle entry without assigning gameplay state.

The user separately reports a black field segment with fire and Fei saying
"Huff... huff... Damn you" before battle entry. Field-2's retail dialogue
entry 52 (offset 0xC0C in its decoded dialogue section) contains that line.
Prior-run captures 016080/016200/016320/016440/016560 are black; 015600 and
015840 show the village prologue. Capture timing is not by itself proof of
the cause. Full field-scene and Gear-cutscene visual acceptance remain open.

## Field-2 black-image trace, 2026-09-05

`scratchpad/opening-field-battle-SSBmzyaE/run.log` establishes the live
ordering: field-2 dialogue index 52, owner/speaker 31, opens at field frame
3971; `func_8001B6C4` is reached later, at frame 4067. The map ID is already
14 at that latter boundary, but no field-14 asset load has occurred there.
The "Huff... huff..." segment therefore precedes battle entry; treating its
missing image solely as an unimplemented battle renderer would miss a
separate field-side symptom.

The field model loop is active during that segment. At sampled model-frame
counts 30/60/120/180/360/480 it considers 65 models and passes
16/18/20/27/16/13 to the renderer. `D_80059578` is respectively
344/301/301/439/469/401. `D_800ADC18` is zero at these samples; fade 0
clears while fade 1 changes color and transparency mode. These are CPU-side
observations, not proof that the final ordering table contains valid,
visible geometry. The first two samples still use the previous map's model
set after the pending map ID changes, so they are not field-2 asset evidence.

The corrected native F2E0 entry is reached in this run without the previous
immediate raw-opcode crash. Later console output reports missing primitive
variants 9/1 and 3/1 and invalid GPU packet lengths. That console output was
not included in GDB's `run.log`; it is an observed next investigation target,
not a durable passing runtime certificate or proof of its precise cause.

On resume, both prior processes were gone and the binary hash remained
`2d6477fb8693633cec63240e8c60165ec08532d603e5e55ec15b892379cd0d82`.
A fresh visible normal-boot run uses
`scratchpad/opening-packet-trace-YFoKQ2so/trace.gdb`. It records each field
ordering-table node's own header and payload immediately before `DrawOTag`,
with sampled scene/fade state and CPU VRAM dumps. It never assigns gameplay
state. Unlike the earlier GDB logging, `console.log` redirects both debugger
and inferior output. At the last inspection this run was still at the title,
with no New Game confirmation and no field-2 packet snapshots yet.

Desktop automation returned Orca `runtime_unavailable`; starting its helper
did not make the computer-use endpoint reachable. Manual New Game selection
is still needed for this run. The existing field confirm schedule does not
operate the title menu and is not a checkpoint or human acceptance test.

## Framebuffer staging attachment repair, 2026-09-05

The `opening-packet-trace-YFoKQ2so` run subsequently selected New Game.
Its complete `console.log` records field-2 dialogue 52 at frame 4738 and
battle entry at 4833. The packet snapshots at 4350/4440/4560/4680/4800 and
Huff+1/+15 have no broken links or cycles. Sample 4680 contains 570 FT4s,
243 FT3s, 51 raw FT4s, 146 semitransparent FT4s, 16 framebuffer-copy packets,
and one fade tile. The model textures and palettes are populated and model
packet colors are nonzero. In contrast, both CPU framebuffer pages and the
distortion's 64-by-240 texture-strip region at VRAM (960,0) contain only zero
at every sampled pre-draw boundary. Capture 017400 shows a black background
with the opening dialogue portrait appearing. These observations narrow the
field symptom to the draw/copy path; they do not independently prove every
submitted polygon or field-script operation correct.

Found shared OpenGL framebuffer attachment ownership:

- `GR_MaterializeFramebufferRect` attaches `g_vramTexture` to
  `g_glBlitFramebuffer` for reading a VRAM copy onto the display.
- `GR_StoreFrameBuffer` subsequently uses that same FBO as the staging
  destination without restoring `g_fbTexture` as its attachment.
- `GR_XenoReadBackbufferToVRAM`, despite its historical name/comments,
  actually calls `glGetTexImage` on `g_fbTexture`, so it reads stale pixels.

The new visible GL fixture executes the current production helper bodies,
extracted without behavioral rewrites. It creates a real SDL/OpenGL window
and uses explicit clear colors as test inputs, not substituted game content.
The initial capture passes. All six post-materialization captures fail in
the original code, including y=0/y=256 and a staging-size change; one sample
expects RGB555 0x7902 but reads the prior green 0x03E0 in all 71,680 pixels.
This reproduced on Mesa Intel(R) Graphics (ARL), without GL errors. It does
not support attributing this failure merely to Linux/llvmpipe readback.

Restored `g_fbTexture`'s attachment immediately after binding the capture
FBO. This is one additional GL operation, not a scene override. The tracked
patch is `pc_port/patches/psycross_framebuffer_staging_attachment.patch`,
applied after the existing mirror patch. Corrected misleading readback
comments in the final patched source. Scripts, fades, draw-list order and
battle flags are unchanged by this increment.

Verification:

- `run_framebuffer_staging_gl_test.sh`: visible O0/O2/UBSan tests pass;
  deleting the attachment restoration is rejected by the negative control.
  Each configuration checks all pixels in seven captures. This tests RGB
  freshness/ownership, not exhaustive PSX rasterization, mask-bit semantics,
  orientation, partial-copy placement, or GLES behavior.
- Test compilation runs in `localhost/xenogears-dev-toolchain:current`
  with `--build-only`; `--run-only` executes visibly on the host. UBSan is
  linked statically because the host lacks `libubsan.so.1`.
- Logs: `pc_port/build_native/framebuffer-staging-{red,green,results}.log`,
  with the negative-control log in `framebuffer_staging_gl_test/mutant.log`.
- Display-page, Vsync presentation, portrait rendering, and battle graphics
  ABI regressions pass, including their existing negative controls.
- Canonical build succeeds: 86 function stubs/575 data stubs. Binary SHA-256
  `d1bea03a1c240640484f5f3369646c64c26f37898261753aa82b265328fde516`.
  Previous binary preserved as `opening-packet-trace-YFoKQ2so-xeno-port`.

Fresh visible normal-boot run: `scratchpad/opening-staging-fixed.6jYerfxl/`.
The same read-only packet trace accepts `XENO_PACKET_TRACE_DIR` to keep its
new captures separate. In-game visual acceptance of the repair is pending.
The previous run durably captures the later missing 9/1 and 3/1 primitive
variants and invalid packet lengths; the Gear cutscene is not yet complete.

### FAE8 lit-quad dependency and visible follow-up, 2026-09-05

Implemented native `func_8002FAE8` in the decomp-owned `temp2.c` and wired
only row 9, variant 1 in `game_overrides.c`. Retail remains `INCLUDE_ASM`.
Authority: executable bytes `8002FAE8..8002FCFC` (exclusive), SHA-256
`0aad615182563b964aa23bbc2447cf969136f2083ed6b27fcaf3683c1d55bd5c`,
shared tail `8002E1F4` length `0x38`, and table row `8004FFB8` length
`0x28`. The runner pins all three slices before executing tests.

This is the retail lit FT4 routine, not an alias to another quad walker.
It preserves the initial full-width V0 index versus masked lookahead V0,
fourth-vertex RTPS, LZCR (not FLAG) reads, terminal RTPT, normal cursor,
rejection-side GTE operations, all eight packet-code bits, and the emitted
counter increment before OTZ-zero rejection. Native address translation
remains in `PcPort_LinkModelPrim`; no HUD or scene-state override added.

`run_model_prim_fae8_retail_test.sh` initially failed to compile due to
missing test headers; that was not a valid behavioral RED. After correcting
the test includes, RED was `missing production owner`. GREEN executes the
retail MIPS bytes and the actual native production translation unit on nine
fixtures at O0/O2/UBSan, comparing packets/guards, OT, cursors, globals,
return value and complete GTE state. Native input regions are checked for
unintended writes. Both paths share PsyCross GTE, so this is not an
independent hardware-accuracy oracle. Four deliberately incorrect versions
(terminal RTPT, lookahead V0 mask, fourth vertex, normal V0) are rejected.
The F2E0 regression also passes all configurations and four existing
negative controls. Logs are `pc_port/build_native/model-prim-fae8-`
`{red,green,results}.log` and `model-prim-f2e0-after-fae8.log`.

The live staging-fixed run subsequently selected New Game and reached
field 2: dialogue 52 at frame 5422; read-only snapshots at 5423/5437.
`field-frame-026400.png` (BMP despite suffix), viewed through the lossless
`view-026400.png` conversion, now contains village geometry but severe red
corruption and a split image. This is **not visual acceptance** of the
framebuffer fix or the scene. The renderer's remaining feedback-copy and
color behavior still need diagnosis. Later logs again show missing 9/1
and 3/1 handlers in this older running binary.

FAE8 has **not yet been rebuilt into the live game**; the executable remains
the staging-fixed SHA documented above. The active visible session was
left intact. F4B4 (row 3/1), full build/in-game FAE8 verification, and the
scripted Gear scene remain unresolved. No commit or push performed.

### F4B4 closure and packed-color reproduction, 2026-09-05

Implemented decomp-owned native `func_8002F4B4` and wired retail row 3/1.
The executable range `8002F4B4..8002F6B4` has SHA-256
`b5c1c8ba79f06d23330897a15f53351bcc505469014e82e82d45d5efd09d246a`;
the table row `8004FEC8`, length `0x28`, has SHA-256
`e14d03d8f5aa0b942fb31757dd4d2a4fb551501bc794014924fee8004fe6d024`.
It preserves indexed normals, the three scratchpad address writes at
`1F800000`, partial XY stores on rejection, NCT colors and code bytes,
terminal RTPT, and the OTZ-zero counter distinction from FAE8. Native
scratchpad uses the existing `g_PsxScratchpad` hardware adapter.

The differential harness is shared with FAE8 via the runner's `--f4b4`
option, but selects the actual F4B4 retail entry, production function,
normal layout and rejection expectations. RED was missing production
owner (the earlier shared diagnostic label still said FAE8; labels now
identify the selected routine). An initial production build exposed an
incorrect scratchpad accessor; this was corrected to the scratchpad array
before tests passed. O0/O2/UBSan now pass nine cases including complete
scratchpad state. Four mutants (terminal RTPT, partial XY, packed V1,
normal V0) are rejected. FAE8 and F2E0 regressions and their mutants pass.
Logs: `model-prim-f4b4-results.log`, `model-prim-fae8-after-f4b4.log`,
`model-prim-f2e0-after-f4b4.log` under `pc_port/build_native/`.

Full build succeeds (`lit-prim-closure-port-build.log`), executable SHA-256
`ff214f36dc1aeaacb4872bf92483273329308568143c197a614cd5d68d03a913`.
Both FAE8 and F4B4 are now in that binary. The owned previous debugger/game
were stopped only after preserving the matching executable as
`opening-staging-fixed-6jYerfxl-xeno-port`. New visible normal-boot run:
`scratchpad/opening-lit-closure.lNiFiQP3/`, awaiting New Game selection at
last check. Full scene acceptance remains pending.

Separately reproduced the materialization color defect on real OpenGL.
`GR_UpdateVRAM` uploads RGB555 bytes into GL_RG; materialization directly
blits those channels without decoding RGB555. The opt-in
`XENO_TEST_MATERIALIZE_COLOR=1` test now checks displayed pixels, not only
capture freshness. O0 RED (`materialize-color-red.log`) yields:

| Packed source | Expected RGB | Actual RGB |
| --- | --- | --- |
| `001F` | 255,0,0 | 31,0,0 |
| `03E0` | 0,255,0 | 224,3,0 |
| `7C00` | 0,0,255 | 0,124,0 |
| `7FFF` | 255,255,255 | 255,127,0 |

There are no GL errors; all seven existing freshness checks still pass.
This explains a concrete mechanism for the red/orange feedback corruption,
but the materialization repair and partial-rectangle placement remain to
be implemented and verified. The new color check is deliberately failing
and opt-in during diagnosis, not a completed production gate. No visual
retail-parity or complete Gear-cutscene claim is made.

### RGB555 materialization repair, 2026-09-05

Replaced the direct GL_RG byte-channel blit with explicit CPU-mirror
RGB555-to-RGBA decoding into a separate texture. The mirror is the actual
MoveImage output; no generated scene content or palette alteration is used.
The existing shared read FBO remains shared, so the earlier capture-attachment
restoration is still necessary. Patch:
`pc_port/patches/psycross_framebuffer_materialize_rgb555.patch`, applied by
`build_port.sh` after the staging attachment patch. Reverse patch check
passes. No other presentation path changed in this increment.

Color checks are now unconditional in the GL test. O0/O2/UBSan pass six
solid colors (including black with bit 15 set) and two nonuniform 320x224
RGB ramps at VRAM y=0 and y=256. The ramps check all 71,680 logical pixels
per page for component decoding and vertical orientation. Seven existing
capture-freshness cases still pass. Both attachment-removal and packed-byte
negative controls are rejected. Logs under `pc_port/build_native/`:
`materialize-color-{red,green,results,final}.log` and corresponding build
logs. The final gate no longer needs an opt-in environment variable.

This repair has not yet been built into the running game. Partial-copy
placement is still unchanged (the helper stretches the copied rectangle
over the backbuffer) and needs its own evidence/test before repair.

The user observed the burning village in the lit-closure run. Its log
records field-2 dialogue 52 at frame 3878 and battle entry at frame 3980.
At the subsequent check, neither previously missing 9/1 nor 3/1 handler
warning appears. However, `field-frame-006960.png`, viewed via lossless
`view-006960.png`, is black. Therefore neither Gear geometry nor complete
scripted-scene behavior is accepted. The broader full-decompile/PC-port
goal remains active; no percentage-complete claim is supported.

### RGB555 visible build handoff, 2026-09-05

Read-only debugger sampling of the lit-closure run showed the process alive
inside Vsync reached from the retail battle interpreter (`func_80070F40`),
with scene-map ID 14. This is not proof that the scene is advancing correctly.
Logs reached stubs `func_8001E9BC` and `func_800C11CC`; source identifies
E9BC as an additional sprite-render path and C11CC as the alternate
sprite-animation owner selected by `D_800591AD`. Their relation to the
missing Gear visuals still requires tracing; no stub substitution added.

Preserved that run's matching binary as
`pc_port/build_native/opening-lit-closure-lNiFiQP3-xeno-port`, then stopped
only its owned debugger/game. Built the RGB555 repair successfully:
`pc_port/build_native/materialize-color-port-build.log`, SHA-256
`392068803bbcc35caac7bf11c370311f36bbe40d5f2dc41f72245f1640aec60a`.
Display-page and portrait regressions also pass, with existing negative
controls (`display-after-rgb555.log`, `portrait-after-rgb555.log`).
Fresh visible normal-boot run: `scratchpad/opening-rgb555.iZEVq1Dy/`.
User New Game selection and visual color/Gear acceptance remain pending.

### Battle sprite caller ABI, 2026-09-05

Retail SLUS `800248D4..8002490C` calls `800C11CC` without replacing
the original sprite pointer in a0. The overlay begins by copying a0 to
s2 (`800C11D4`) and reads `sprite+0x9E` and `sprite+0x64`. The native
declaration/call incorrectly used `void(void)` and no argument. Corrected
both to pass `pData`; the C11CC implementation remains a stub, not an alias
to the field interpreter. This ABI repair alone does not restore animation.

Authority hashes: SLUS caller length `0x38`, SHA-256
`58cc815142f2979dd2c28ebae14e3d1be94790b9a92454d6b9d206b23350687f`;
battle entry at `800C11CC` length `0x34`, SHA-256
`470ab4b1325535f7e02b5edf4cff7481acc03ffe35ab811af718983c66dd2edc`.
Full C11CC range extends to `800C204C`; its opcode closure is not yet ported.

The walk-animation production test now includes a compile-time prototype
contract and directly checks mode-dispatch count and pointer identity.
The first attempt through AnimScriptTick did not dispatch because the
fixture's wait timer was zero; direct caller testing deliberately isolates
the ABI rather than claiming coverage of the scheduler or retail animation.
The prototype contract rejected the old declaration before repair.
O0/O2/UBSan pass in the existing toolchain container; a scratch-only NULL
argument mutant is rejected by the pointer assertion. Existing two walk
negative controls pass. `run_walk_anim.sh` now uses the production packed
packet layout, section GC for unrelated renderer dependencies, and grep
when the toolchain lacks rg. The host lacks its GCC UBSan shared library;
container execution is unit testing only, not a headless game launch.
Final log: `pc_port/build_native/battle-sprite-abi-final.log`.

No full game rebuild for this two-line ABI correction yet; the visible
RGB555 run and its executable remain unchanged. Native battle animation,
additional sprite rendering, and full scene acceptance remain open.

### Battle animation leaf decompilation, 2026-09-05

The C11CC dependency scan found four direct calls to `800B168C` and one
to `800B16A4`. Decompiled both in `src/battle/main.c`:

- `800B168C`: base + 12 + index * 28, with 32-bit wrapping arithmetic.
- `800B16A4`: walks the count at +0x14 from the relative offset at +0x10;
  sums `(byte0+1)*4` while advancing by `(byte1+1)*4`. These two length
  fields are deliberately independent.

Retail slices are pinned by the new `run_battle_animation_leaf_test.sh`:
`800B168C`, length `0x18`, SHA-256
`e0dedf5e7431d44f04bd206c92e077bba44d06d0fc1d4d759b0317db5d0f53ef`;
`800B16A4`, length `0x4C`, SHA-256
`407016beda83459888ae2a063ef1e73348d74f61f2b811a5917a74b70ace64c2`.
RED reported missing native owners. GREEN compares actual retail MIPS
execution against the native full translation unit: 28 index/wrap cases
and 65 variable-record lists, including empty input. Retail writes are
rejected by the test bus; native source bytes must remain unchanged.
O0/O2/UBSan pass, and incorrect stride, output-size field, and input-step
field mutants are rejected. Log:
`pc_port/build_native/battle-animation-leaf-final.log`.

These are behavioral differential results, not a claim of compiler-byte
matching. `src/battle/main.c` remains reference-only in the PC game build;
the active battle interpreter already executes these original retail leaf
bytes. The new C owners are tested decompilation dependencies for a future
native C11CC closure, not yet linked native replacements. No broad change
to the overlay ownership policy or undocumented stub behavior was made.
C11CC still has many direct dependencies plus indirect calls; these two
leaves do not close the interpreter or prove a visible animation repair.

### Partial framebuffer restore placement, 2026-09-05

Added a visible GL test for a 50x40 VRAM restore at draw-relative (30,20),
with a distinct untouched background. Before repair it mismatched 278,720
of 286,720 window pixels on each framebuffer page: the helper stretched
the small copied rectangle across the full window. This is independent of
the already repaired packed-color decoding. RED log:
`pc_port/build_native/materialize-rect-red.log`.

`GR_MaterializeFramebufferRect` now inverts the draw-clip-to-window mapping
used by `GR_StoreFrameBuffer`, places the copy in its destination rectangle,
and temporarily disables/restores polygon scissoring for the VRAM copy.
This changes the hardware adapter only. Patch:
`pc_port/patches/psycross_framebuffer_materialize_rect.patch`, applied after
the RGB555 patch; reverse apply check passes. No scene state is modified.

The placement test is now unconditional. O0/O2/UBSan pass both y=0/y=256
pages, checking every output-window pixel including the untouched region,
along with prior color-ramp/orientation and capture freshness cases. A
one-pixel polygon scissor cannot crop the copy, and the enabled state must
be restored. A full-screen-stretch mutant is rejected, as are the earlier
packed-byte and attachment-removal mutants. Final log:
`pc_port/build_native/materialize-rect-final.log`.

This covers the current draw-clip capture convention at integer 2x scale;
it does not prove every PS1 scanout mode, arbitrary display/draw offset,
fractional scaling, mask-bit interaction, or full scene fidelity. The new
placement repair is not yet in the visible running RGB555 executable.
That run remains alive at the title/idle cycle; no new in-game acceptance
has been claimed or forced.

### Native sprite callback into the retail battle overlay, 2026-09-05

Restored the missing native-to-guest C11CC boundary in
`pc_port/src/battle_mips_runtime.c`. Native `func_800248D4` now passes its
sprite argument (the prior ABI repair) into `func_800C11CC`, which calls
the already established retail callback adapter at `800C11CC`. That adapter
continues below an active guest caller's stack and retains the loaded
overlay/shared-data ownership. Outside an active battle overlay this
boundary aborts explicitly instead of silently succeeding.

This is an execution-adapter integration repair, **not** a native C
decompilation of the battle animation interpreter. It executes original
retail bytes and does not substitute the field interpreter, synthesized
commands, sprite poses or scene state. The earlier decompiled leaves
remain reference-only in the game build pending native dependency closure.

`run_battle_graphics_abi_test.sh` now pins all `0xE80` C11CC bytes to
SHA-256 `f7a1f233c785d48e5815a92abf5edc576fa02d7a0c4037d5a75067002649841d`.
The new test loads the original overlay and compares the wrapper's sprite
output with direct MIPS execution for an already-waiting sprite and two
duration commands (30/3F), including explicit timer/PC assertions. RED was
missing owner; a no-op wrapper mutant is rejected at case 1. O0/O2/UBSan
and the existing 45 nested-callback stack cases pass. Final log:
`pc_port/build_native/battle-sprite-reentry-final.log`.

This does not cover every animation opcode or certify the Gear scene.
The no-active-overlay abort is implemented but not separately exercised
by a subprocess test in this increment. The preceding live run's binary
was preserved as `opening-rgb555-iZEVq1Dy-xeno-port` before stopping only
the owned debugger/game to rebuild the integration.

Full build succeeded (`battle-sprite-reentry-port-build.log`): 85 function
stubs / 575 data stubs, executable SHA-256
`a772cd10e27ecfdf6bc0df7e898b9d38ca984262c453756f3a4e24eec26c4cc6`.
This build includes the partial-rectangle materialization fix, corrected
sprite-pointer call and C11CC re-entry adapter. New visible normal-boot
run: `scratchpad/opening-sprite-reentry.hwQNVyd6/`. It still requires
natural New Game progression and in-scene acceptance; the successful
link/stub-count decrease is not gameplay or full-decompile proof.
