Latest integration: [exact file-1 controller](file1-controller-exact-integration-20260906.md),
[A9 production and outer-interpreter checks](animation-a9-integration-20260906.md),
[text-table repair](text-table-integration-20260906.md), and
[global matching result](global-matching-after-table-fix-20260906.md).
The controller is byte-exact; native adoption remains disabled. The repaired
port links, with fresh later-battle gameplay observation separate. Global
matching now links all artifacts but still fails four retail checksum gates.
Earlier entries below are historical and retain their original proof limits.

The [guest-call prerequisite](guest-call-service-integration-20260906.md) now
passes independent ABI tests and a native rebuild, with no production callers.
The [remaining checksum inventory](global-checksum-mismatch-inventory-20260906.md)
and [member-menu compiler-order diagnosis](member-change-ordering-followup-20260906.md)
identify subsequent exact-match work; no checksum expectations were changed.

Newest verified result: `constructor-retail-repair-20260906.md` and
`constructor-after-runtime-20260906.json`. Installed constructor and glyph
regressions pass; fresh isolated native opening renders Fei dialogue and
returns directly to the painting room. Matching still fails on the existing
linker labels; full retail framebuffer parity is unresolved. The file1
controller draft/review remain distinct from native adoption and exact match.

Latest 2026-09-06 follow-up: native logging guards compile and link; matching
advances to unresolved jump-table labels (72 linker diagnostic lines), still no matching pass.
See `matching-telemetry-20260906.md`. Full constructor and dynamic-module
static audits are in `window-constructor-retail-audit-20260906.md` and
`dynamic-module-inventory-20260906.md`; scratch differential/candidate work
continues. Retail physical-confirm correction is documented in
`retail-input-correction-20260906.md`; no fresh Gear-panel capture yet.

# Opening scripted battle: encounter and pointer corrections, 2026-09-05

Runtime-end update (2026-09-06 UTC): the opening proof remains valid, but
continued play later entered another battle and aborted on unimplemented
sprite opcode0xA9/index31. The native/debugger session is now terminal; root
did not request its stop. Read the dated runtime JSON before resuming.

## Latest verified result — 2026-09-06 UTC

Fei's battle dialogue now renders, and the opening returns directly to the
painting room. See [window bridge repair](window-bridge-repair-20260906.md),
[battle return repair](battle-return-state-repair-20260906.md), and
[runtime evidence](window-and-return-runtime-20260906.json). Focused tests and
native build pass. Full matching retains the existing stderr errors; complete
retail pixel parity is not claimed. Current recorded replay was left live;
read ACTIVE_HANDOFF.md and check process ownership before UI actions.

Earlier milestone statements below are chronological evidence, superseded
where the current reports correct the text-owner and Kernel Menu findings.

The user requires the retail opening's scripted Gear battle, including its
setup, camera, sequence, and HUD behavior. Visible models or a normal command
battle alone do not satisfy that requirement. This continues the
[Gear-loader/trig investigation](../gear-model-list-resume-20260905/README.md).
HEAD remains `3a3e7aac03a2f166fb924945a489e392d706f282`; no commit or push.

Current milestone (2026-09-06): the native normal New Game replay returned
from the scripted Gear battle and rendered Fei at his painting-room easel
in field14. See [runtime evidence](type9-cleanup-runtime-result.json) and
[the captured room](painting-room-after-battle.png). Fei's battle lettering
and a complete retail visual comparison remain unverified. The remainder
of this file records earlier diagnoses and the subsequent repairs.

## Current-pointer correction (2026-09-06)

The replay did return from battle and render field14/the painting room, but
captures 15360 through 15600 show an unintended transient `XENOGEARS Kernel
MENU` during the transition. A direct retail transition is not proven: the
guest/native battle-result split identified by the read-only audit still
awaits a live probe. At the time this pointer was written, no prior preserved
PCSX Redux/native/GDB session was active. Root owns any newly launched
diagnostic runtime; it does not retroactively prove this milestone. The
previously preserved emulator PID is no longer live, and the reason for its
exit is unobserved.

The old presentation `0x17` attack atlas is not the pilot-text owner. The
actual lettering owner remains unresolved; this pointer does not reinterpret
the historical evidence below.

## Diagnosis

The previous native run entered battle with an incorrectly aligned encounter
record. A read-only watchpoint run,
`pc_port/build_native/opening-battle-party-position-icw0zbo9/`, followed normal
New Game through fields 490, 4 and 2. It observed retail auxiliary code writing
Citan's formation index `7F` at guest PC `801E422C`, then X `-7660` at
`801E46A8` and Z `11560` at `801E46CC`. The late battle camera followed that
off-scene position. The earlier trig correction did not resolve the missing
geometry/scripted sequence; its final capture still showed ordinary HUD bars
over black geometry.

Fresh disassembly of `disc/field.bin` located the upstream transcription error:
retail `80071054..8007106C` sets `a2=800658DC`, reads the compressed-source
offset at map-header `+148`, and adds `10` to the unused size argument `a0`.
Native `FieldLoad` instead added `10` to the destination. Its wrapper at
`8007008C` discards `a0` and forwards source/destination to `80032EB4`.

Map 2 comes from Disc 1 field archive relative entry 188, sector 121096,
123808 bytes. The header size at `+124` is 528; the compressed stream at offset
121448 advertises 530 bytes. Executing the actual retail decompressor confirms
530 bytes written, offsets 0 through 529. This is not a guessed overflow or a
reason to truncate the stream.

Retail battle `80071130..80071154` copies one 32-byte record from
`800658DC+(D_80059508<<5)` to `8006F9DC`. The opening's observed selector is 1.
The shifted native record supplied zero flags and asset index; the correct
retail record supplies flags `A0`, asset index `12`, and record byte 3 `01`.
The complete records are retained in ignored, pinned test/runtime evidence.

A second boundary then mattered: native `func_8001BB0C` read a separate
generated `D_8006F9DE` variable, while the retail interpreter wrote the active
record into emulated RAM. This had been masked by the erroneous zero index.
An independent test executed the retail 32-byte copy through the production
bridge and showed the native consumer reading a poisoned standalone value
`E7`, despite correct guest bytes including the expected `12`.

## Changes

- `src/field/main/misc3.c`: decompress the encounter section at `D_800658DC`,
  matching retail's destination. Correct the misleading walkmesh comment.
- `src/slus_006.64/system/temp3.c`: the PC build reads the active encounter's
  byte at guest `8006F9DE` through the existing PSX memory adapter. The matching
  build retains its original symbol-based expression.
- Strengthen the existing asset-loader fixture so all 256 index values come
  from the guest record, with a different value deliberately in the old stub.
- Add independent field-record and battle-interface regressions.
- `pc_port/src/battle_mips_runtime.c`: translate the native `HeapAlloc`
  return back to a guest RAM address before the retail CPU uses or stores it.
  This applies only to retail function `80031BDC`; other return values and
  the native allocator remain unchanged.

No battle mode, party, HUD, camera, pose, encounter selector, or script flag is
forced by these changes. The existing separate `D_80065ADC` native weights
storage is outside this fix: the field test models a contiguous retail buffer,
and does not claim native random-encounter weight aliasing is repaired.

## Verification

| Check | Result and evidence |
| --- | --- |
| Field call/wrapper/decompressor against actual retail instructions | RED `field_battle_record_retail_test.I4fn7z5q`; GREEN `field_battle_record_retail_test.SGYq9uD5`, eight cases each at O0/O2/UBSan |
| Complete field output and guards | All 530 bytes, all sixteen records, tail and surrounding guards; four initial fills and relocated compressed source |
| Field negative controls | Old `+10` destination, `+1` destination, one record-byte corruption and one tail-byte corruption all rejected |
| Actual retail copy through production bridge, native asset consumer | RED `battle_encounter_alias_test.4YO7tAe6`; GREEN `battle_encounter_alias_test.t9OVfen7`, O0/O2/UBSan, all 32 bytes and mixed-width KSEG0/KSEG1 reads |
| Consumer negative controls | Separate native stub and wrong `+1`/`+3` record bytes rejected at all three build modes |
| Heap pointer return through actual runtime and retail allocation fragment/helper | RED `battle_heap_pointer_retail_test.R4wXd1jO`; GREEN `battle_heap_pointer_retail_test.o5XaB7Nm`, 54 cases each at O0/O2/UBSan |
| Heap pointer negative controls | Raw host return, normalization of unrelated scalar returns, and missing pointer argument translation all rejected; includes NULL, edge addresses, complete guarded reservation records and subsequent `HeapFree` calls |
| Existing asset-loader/caller suite | 256 caller indices plus 953 loader cases each at O0/O2/UBSan; output order, archive queue, padding and returns preserved |
| Native build | PASS, `pc_port/build_native/battle-encounter-port-build-20260905.log`; 649 unresolved symbols, 79 function stubs, 569 data symbols; 18 owned overrides checked |
| Isolated matching `make check` | RED before and after on the same existing undeclared `stderr` errors in `temp1.c` and `animation_scripts.c`; no complete matching-binary result claimed |

Test directories above are under `pc_port/build_native/`. Before/after sources,
matching checkouts and logs are preserved at
`/tmp/xeno-gear-resume-20260905-l12d_42r/`. The non-PC `temp3.c.s` is identical
before/after: SHA-256
`d6bc0971aa80f3ff765a2981c65d8cab00152e468e233cb33cbd794302f4a29b`.
The field destination correction changes the non-PC generated assembly; full
byte matching remains unproven because the baseline check fails earlier.

## Provenance

| Input / artifact | SHA-256 |
| --- | --- |
| Retail Map 2 file | `d81670fa78aeed510852153349cd8d4e4b5a6bc6469110e41fb6274aa4e4261f` |
| Retail decoded 530-byte section | `727bad94d380db9065d190fb7c28cc422549030dcaab9edee894b186fbbf85e9` |
| Retail field section call `[8007104C,80071070)` | `ffb77b50eee3aa64a3d24a48eeaca360a5be34654dfd888ebc934519d9fe098b` |
| Retail wrapper `[8007008C,800700B0)` | `043363a6b87236971c393ce02b1fa3611823521f624c52a2fdb21c79d685b71c` |
| Retail LZSS `[80032EB4,80032F54)` | `603f626038f3ee725fe0b5a79988343ff0aca07e6c2a72a95791d3d565f926ed` |
| Corrected `misc3.c` | `9c1850f6892efcebd4f766fe58a10b8543050e762c4ad34e00453efcd3d2c9ed` |
| Corrected `temp3.c` | `c93a17cff33fb2c26b1976218a73d8bbafd184097b2e4fe124087a79c89f3ae6` |
| Encounter-correction executable | `c860d9140bb5990fb4e7e5cd98262438a5fc4e94b4b593fcffdbea8d306cc240` |
| Heap-corrected runtime source | `b46535084fe718fe7fd966d0ca8d27047891a8b56a81dfaa773c171f967e85d3` |
| Heap-corrected executable | `6cc1e829d952327cc751245f8fd159607cd38c505c630d6c06fe77d428b7b071` |

## Fresh normal-opening run

`pc_port/build_native/opening-battle-encounter-retail-li37dy5e/` records the
copied executable, source hashes, read-only debugger probes, input actions,
captures and log. The menu received normal Up/Circle input and confirmed New
Game at 20:32:28 UTC. A labeled field-only Circle schedule advances field
dialogue; it does not inject battle commands or write game state. Scripted
battle visual acceptance did not pass in this run. The live asset loader saw
the exact retail record and index 18. Citan's descriptor now receives formation
0 and X/Z `(3440,1480)`, rather than formation `7F` and the off-scene values.
Battle frame 2's atomic ordering-table snapshot contains 371 packets inside
Fei's Gear buffers; its camera eye/target are `(2495,-1643,-3025)` and
`(2495,-44,2245)`. This is packet/camera observation, not a visible Gear pass.

The run then stopped advancing before a subsequent capture. Interrupting the
owned inferior at 20:36:49 UTC showed the native out-of-memory loop:
`HeapAlloc(6480104,1)` -> `MainLoop(130)` -> `GameHandleError`. The request is
`0x62E0E8` bytes, larger than retail RAM. No allocation cap or success fallback
was added. The owned debugger terminated its inferior after recording the
backtrace. Both processes are gone.

The next run, `opening-battle-script-alloc-khl4t18s`, used the same pinned
executable and normal New Game path. A read-only breakpoint retained the guest
CPU registers and complete RAM at the oversized allocation. The retail
fragment `80070E48..80070E6C` allocates four bytes, then subtracts `801E5000`
from that returned pointer to reserve space for the special battle overlay.
The native heap returned host pointer `008032C8`, corresponding to guest
`801F34A8` with this run's RAM base `0060FE20`. The bridge passed the host
address directly into the retail CPU's `v0`. Retail's addition of `7FE1B000`
therefore produced `8061E2C8`; the generic argument bridge then misclassified
that size as a guest pointer and translated it to `0062E0E8` (6,480,104).
The correct guest-domain subtraction is `801F34A8 - 801E5000 = E4A8`
(58,536). This identifies a pointer-return ABI error rather than a need for
more RAM or an allocation cap. The stopped debugger and inferior are gone.
The independent fixture executes these exact retail instructions and the full
`8008ABB8` heap helper through the production bridge. It reproduces the exact
captured bad size in its original-code negative control and passes all 54
cases with the correction, including at UBSan. The fixture controls allocator
boundaries; it does not claim to prove allocator policy or rendered behavior.

The new native build passes (`battle-heap-pointer-port-build-20260905.log`),
as do the CPU, graphics ABI/nested callback and exhaustive trig suites.
`opening-battle-heap-guest-4c8zx1_0` is the fresh normal-opening run with the
heap correction. New Game was confirmed at 20:51:47 UTC; its probes also
retain guest registers and RAM on native errors or faults.

This run passes the allocation stall and presents visible Gears in the
burning arena. The user independently reported seeing Gears during this run.
Capture `captures/field-frame-005340.png` (BMP content despite its suffix),
also converted without visual edits to `current-view.png`, shows four Gear
models and no HUD bars at that moment. This is a visible progress observation,
not acceptance of the complete scripted sequence. Atomic battle snapshots
reach frames 60 and 120 with the special overlay enabled. The latter includes
364 Gear-buffer packets; its camera eye/target are `(2413,-1677,-3108)` and
`(2413,0,2420)`. The watched `800C492A` byte remains zero, so this capture does
not establish that the later script command which sets that byte has run.

The next blocker is explicit: native `func_8001FBE4` aborts on unimplemented
sprite command `A1`. The retained sprite is host `007C0BBC`, operand address
`007C0729` (operand zero), with scale `+82=8192`, flags `+A8=2001F800`,
speed word `+AC=00008020`, and velocity-Y `-126336`. Retail's jump table maps
`A1` to `800219AC..80021A44`, which sets vertical velocity using its state,
frame factor, signed operand and scale, then the packed speed divisor. Both
owned processes ended after recording the assertion and complete RAM.

The run also calls two existing native no-op hooks, `800BA8F4` and `800BC158`.
Fresh battle-image disassembly identifies ground-height updates and scripted
child-sprite ownership/callback setup respectively. Their behavior still
needs restoration before calling the scripted battle retail-accurate.

## Sprite motion and scripted ownership follow-up

The next correction implements `A1` using the retail shared-state override,
signed scaling/rounding, 32-bit arithmetic and signed division, including the
R3000 result for division by zero. It uses the existing packed byte-access
helpers. The two native hook bodies now call the existing active-battle MIPS
dispatcher with the original arguments and exact targets. The full retail
floor/ownership functions run against the active battle state. Missing active
runtime is an explicit error; neither body retains its former no-op fallback.

| Check | Result |
| --- | --- |
| A1 native versus full retail dispatcher/helper | RED `sprite_dispatch_a1_retail_test.PhqLYPSJ`; GREEN `sprite_dispatch_a1_retail_test.KpsGhXK6`: 366,593 cases per O0/O2/UBSan, including 71,680 valid alias cases; 14 mutants rejected |
| Native hook re-entry into actual retail bodies | RED `battle_sprite_hooks_retail_test.XSc5zHA9`; GREEN `battle_sprite_hooks_retail_test.rKeakGZk`: both floor-query paths and all 64 child slot/occupancy/old-frame/new-frame/mirror combinations; O0/O2/UBSan |
| Hook proof boundary | Leaf floor queries and callback helpers are controlled in this test only; production bodies are extracted verbatim, runtime/CPU/map inputs pinned, and wrapper/old-owner arguments, floor XYZ, memory guards and nested SP checked |
| Hook controls | No-op, wrong target and wrong argument rejected; both inactive-runtime calls abort. Clang's function-type sanitizer is excluded for the existing generic dlsym ABI; other undefined-behavior checks remain enabled |
| Neighboring animation suites | Data/B5, byte-data, no-op and 8C checks pass. The 8C runner's borrowed angle guard was namespaced to fix an existing duplicate test symbol; production angle code did not change |
| Native executable | `c20d3f2692b2abc232103020b87d50d147f4968146c13c3fdf5ec658a995d239`, build log `battle-script-motion-port-build-20260905.log` |

Fresh run `opening-battle-script-motion-7zrztlb6` confirmed normal New Game at
21:07:57 UTC. It passes the former A1 assertion, executes the restored hooks,
and changes to the scripted close-up camera and pilot display. The user
supplied a screenshot and reported "success! looks good" during this run;
the original image and hash are retained as `user-confirmation.png/json` in
the ignored run directory. No combat bars are visible. Atomic snapshots reach
battle frame 1200. At frame 300 the camera eye is `(3576,-433,947)` and target
`(2041,-2,2041)`, with 336 Gear-buffer packets. This is confirmed visible
scripted progress, not proof of the complete battle exit.

After that display, the run advances further and aborts on missing command
`B4`, at sprite `007DDC44`, operands `0079CDC5`, cursor `10`, operand `04`.
The same run requests a missing type-5 render callback. Both processes have
ended; the run's stop JSON/RAM and last capture retain the failure.
Retail `B4` at `80021480..80021494` calls the already implemented byte stack
push helper at `80021CA0`. Retail callback table entries 0/5/6/14 point to
`80025258`; the native table still left them NULL. The existing native body
also had incorrect packed-pointer width, depth predicates and omitted its
ordinary billboard draw path. Those two next repairs are under verification.

## Stack command and effect rendering

`B4` now captures its operand before calling the existing packed byte-stack
helper, preserving the retail decrement, signed cursor and overlapping-store
order. The production callback table binds types 0/5/6/14 to `80025258`.
That renderer now reads four-byte task pointers, translates guest aliases,
uses the active guest suppression byte, restores the retail depth predicates
and ordinary billboard route, and addresses ordering-table entries at a
four-byte stride. It reloads flags after the transform helper as retail does.

| Check | Result |
| --- | --- |
| B4 full dispatcher and stack helper | RED `sprite_dispatch_b4_retail_test.euAwStJA`; GREEN `sprite_dispatch_b4_retail_test.Fkr8FuqB`: 1,046,541 cases each at O0/O2/UBSan, including 784,396 valid aliases; nine mutants rejected |
| Billboard retail body and production table/dispatch | RED `battle_child_billboard_retail_test.NraVobGn`; GREEN `battle_child_billboard_retail_test.mAUwE7Js`: O0/O2/UBSan, packed pointers, KSEG aliases, suppression, flags 24/25/29, helper flag reload, depth boundaries, OT stride, arguments and complete fixture guards; seven mutants rejected |
| Billboard proof boundary | Actual retail instructions `80025258..8002541C`; verbatim native address helper/body/table/dispatch. Projection, matrix and final draw leaves are controlled; this is not GPU output or complete GTE proof. Parent verification and mutant outputs retained in the GREEN directory |
| Neighboring A1 suite after B4 | `sprite_dispatch_a1_retail_test.IgfuY7S8`, all three modes and 14 mutants pass |
| Existing walking/animation ABI check | O0/O2/UBSan and three controls pass in the project container; initial host attempt could not link its missing `libubsan.so.1.0.0`, not a test assertion failure |
| Native build | PASS `battle-script-billboard-port-build-20260905.log`; 648 unresolved symbols, 79 function stubs and 568 data symbols; 18 owned override addresses verified |
| Isolated matching check | Still RED on the same baseline undeclared `stderr` errors; `make-check.venv.after-billboard.log` retained with both prior matching inputs and final input hashes. No full binary-match claim |

The new executable SHA-256 is
`d5adf687a18fd75116411fe4075c525dcc758d0e70b09dd8f80e05fcaa98aea8`.
Fresh run `opening-battle-script-billboard-k7458uzi` began at 21:33:07 UTC,
and ordinary menu input entered New Game at 21:33:34 UTC. Its `run.json`
pins all seven production inputs and the same labeled field-only input
schedule. Full scripted battle completion remains unverified while this replay
is running.

## Later command boundaries and missing text

The billboard replay did pass B4 and render further effects. The user reported
the battle was working visually, with Fei's dialogue text missing. It later
stopped at capture 9000 on command BB. Retail BB adds a signed operand byte to
the halfword depth bias at sprite +30. Native now preserves the operand read
and halfword wrap. `sprite_dispatch_bb_retail_test.8dUxZNnO` passes 17,334,273
cases per O0/O2/UBSan (every halfword/byte pair, 557,056 aliases and the captured
input); nine controls fail. Original RED: `sprite_dispatch_bb_retail_test.xHHUsbIn`.

The next executable, `cb0f5ea1a4384ae358a40bb65f300dc3e2f71b9ec6f896d95ded78a4273961a1`,
also introduces the separately tracked [host speed control](../host-speed-20260905/README.md).
`opening-battle-host-speed-39dbja_6` passes BB and stops at battle frame 385
on E5, after registering type-5 child sprites. No callback types 8/9/15 are
requested in this occurrence. The read-only panel registration/first-draw
snapshots narrow the investigation but do not establish the cause of missing
glyphs. A preliminary external scratch diagnosis confused the executable
address mapping and was explicitly retracted; its ScaleMatrix interpretation
must not be used.

The actual SLUS E5 handler at `80020C20..80020C50` resolves a script destination
with `8001FBA4`, calls the retail RNG, then reads the unsigned range operand and
writes `((rand() & FF) * range) >> 8` as a byte. The native case now preserves
that ordering and uses the existing native retail RNG. Full native dispatcher,
pointer helper and RNG versus real retail instructions pass 5,293,569 cases
per O0/O2/UBSan in `sprite_dispatch_e5_retail_test.rIGgHf3I`; eleven mutants
fail. Coverage includes all random-byte/range pairs, selected full-width seed
boundaries, 968,192 aliases, operands overlapping the RNG seed and writes back
into the seed. It does not exhaust all 2^32 seeds. RED:
`sprite_dispatch_e5_retail_test.5thcaNQO`. Current animation source SHA-256:
`12cad9a7b0d5c5ec15a28b460a4b8e18f4666d0ad9dc1a7c00d3d8195fd1abbd`.

E5 is not yet included in a fresh built/runtime-verified executable at this
checkpoint. Missing dialogue text and complete scripted battle exit remain
open. Inspect the newest run metadata and host-speed report before continuing.

## NTSC speed correction, C1, and latest AC boundary

Normal runs `host-render-profile-1-cqdl6rsf` and
`host-render-profile-5-oe1ga92u` include E5 and advance to unsupported C1.
C1 at `800201F0..800202F4` scatters a sprite around its signed integer
position using three RNG calls, the existing radius/slow-scale helpers and
a zero-Z rotation. The first native attempt exposed real one-unit matrix
rounding differences in PsyCross `RotMatrix`; these were not tolerated.

The C1-only matrix helper now preserves retail `8003F738..8003F8AC` signed
Q12 multiplication and shift stages. In particular `8003F890` negates the
product before `8003F898` shifts it; negating after the shift is observably
wrong. The other renderer callers retain their existing matrix function.
See [the instruction-derived matrix contract](c1-zero-z-matrix.md).

- Unsupported-command RED: `sprite_dispatch_c1_retail_test.OAUJNsY5`.
- Matrix-precision RED: `sprite_dispatch_c1_retail_test.EJ7mry6h`.
- GREEN: `sprite_dispatch_c1_retail_test.QbzoMKZ9`, 1,048,577 cases per
  O0/O2/UBSan, including 163,840 aliases, all operand bytes, each 4096-angle
  axis cycle, three RNG calls/state, full guarded fixture, and all nine
  observable rotation halfwords. All 13 mutants reject, including the old
  host matrix and incorrect negation order. Both sides share the PsyCross
  GTE backend; hardware-GTE, unused padding, and visual parity are not claimed.
- A1/B4/BB/E5/DATA-B5/NOOP regressions remain green after adding fail-fast
  guards for the newly referenced, unrelated C1 leaves. The A1/B4/BB/E5
  fixture reruns used GCC 13.3 for both units; DATA/NOOP used GCC production
  objects and actual Clang 22.1.8 fixture compilation. No diagnostic or
  assertion suppressions were added to make these pass.

The final uninstrumented native build passes in
`pc_port/build_native/ntsc-speed-c1-port-build-20260905.log`, with 648 unresolved
symbols represented by 79 function stubs and 568 data symbols, and all 18
port-owned override addresses verified. Executable SHA-256:
`8827e71ceda0adad0802b3b96d81ec2c7cd2703b791000b534afda84c2f1f40f`.
Animation source SHA-256:
`951dbcda616d0fd3ceda15fc115c91dbec3b8e64a686523766d2e75b7992f7b0`.

Latest ordinary New Game replay `opening-ntsc-c1-5-jw5zz2qe` uses the corrected
NTSC mode and 5x host rate. It enters the retail battle and stops on **AC**
(raw 172, dispatch index 34), sprite `007DFEF8`, operands `0079EDDB`.
This RNG-dependent route does not establish live C1 entry or visual parity.
AC remains unimplemented; no skip or replacement state was installed. Its
retail handler `80021644..80021694` calls RNG before reading its unsigned
range, computes `(((rand & FF) * range) >> 8) - (range >> 1)`, scales the
signed offset by 16, adds angle halfword +32, then calls `80021FE0` with the
wrapped signed halfword. That helper invokes `80022974`; verify both native
helpers and their arithmetic before enabling AC. The run naturally aborted;
no runtime RAM snapshot was taken in this uninstrumented replay.

The separate [host-speed report](../host-speed-20260905/README.md) records the
verified 4.971x field progression. Missing Fei text and complete scripted
battle exit remain open. No commit or push.

After C1, the isolated matching `make check` still stops on the same
preexisting undeclared `stderr` in animation_scripts/temp1 as the baseline;
log `/tmp/xeno-gear-resume-20260905-l12d_42r/make-check.venv.after-c1.log`.
The matching workspace is a separate scratch copy with retail input mounted
read-only. Native build/testing success is not a full ASM/binary-match claim.

Interactive normal build left open at 1x: `interactive-ntsc-speed-1-k02ujy2z`,
PID `2315253` at launch, with no synthetic input. Verify the current
PID/executable ownership before interacting; it may have ended later.


## AC angle adjustment and shared velocity arithmetic (2026-09-05 23:20 UTC)

The AC command at retail `80021644..80021698` now calls the actual RNG
before reading its unsigned operand, computes the centered angle offset,
and uses the actual angle/velocity helpers. The shared `80022974` helper
now preserves the R3000's wrapping shifts and low-product arithmetic,
signed divide-by-zero result, and negation before its final arithmetic
shift. Trig indexes use the retail low 12 angle bits. These changes remove
host signed-overflow/division undefined behavior without adding a fallback.
See [the instruction-derived AC contract](ac-velocity-contract.md).

Unsupported-command RED is `sprite_dispatch_ac_retail_test.T1qNAcQe`.
Initial differential GREEN is `sprite_dispatch_ac_retail_test.Vpp9ymX0`:
4,616,193 cases each at O0/O2/UBSan, executing the full retail dispatcher,
RNG, angle setter, velocity helper and trig instructions. Complete fixture
and RNG state agree. Expanded angle coverage and negative controls are
still in progress; this initial result is not their final acceptance.

Neighbor regressions remain green with actual Clang fixture compilation:
C1 `sprite_dispatch_c1_retail_test.rmQZLq5v` (13 controls), E5
`sprite_dispatch_e5_retail_test.qjJ8907Y` (11 controls), and 8C
`sprite_dispatch_8c_retail_test` (point packing, call order, NULL gate).
No neighbor fixture edits were needed.

Native build `pc_port/build_native/opening-ac-port-build-20260905.log` is
LINK OK, preserving the existing 648 unresolved symbols / 79 function
stubs / 568 data symbols and all 18 port-owned address checks. Binary SHA:
`b373a556c304a9136986cb940b95386f140dca90be0534907f30310b755490b4`.
Animation source SHA:
`a6fe7059334e6ac2fc68829cc6ff8d4023de47822536551d01c47e9c0d7743b4`.
The isolated matching build still fails at the preexisting undeclared
`stderr` in animation_scripts and temp1, as recorded in
`/tmp/xeno-gear-resume-20260905-l12d_42r/make-check.venv.after-ac.log`.

Ordinary 5x replay **`opening-ac-5-khm2410x`** passes the previous AC stop
and reaches **C4** (raw 196, index 58), sprite `007DFEF8`, operands
`0079EDDD`. This is the next command in the same archive stream (file
`0x50` opcode offset `0x568`). It naturally aborts at the unsupported
handler; no state was planted, no skip installed. Capture frame 5640 was
viewed and shows both Gears against the burning background; complete
scripted battle exit and Fei text remain unverified. C4's retail body is
`800215B8..80021644`, including zero-X/Y rotation and `ApplyMatrixLV`.

The previously preserved interactive run `interactive-ntsc-speed-1-k02ujy2z`
also naturally ended at AC; PID 2315253 is no longer live. The new automated
run has ended too. Recheck ownership before any subsequent game interaction.

The [pilot-panel dialogue audit](pilot-panel-dialogue-audit.md) identifies
presentation ID17 and the exact archive50/51 bytecode/VRAM stream. The
active renderer is the special type5 animation path; the generic font
branch is unused at this observed beat. The missing-text root cause is
still unresolved. The [C4 matrix/GTE audit](c4-matrix-gte-audit.md) verifies
Z-only native RotMatrix and explains why the exact two-pass ApplyMatrixLV
semantics, including signed16 IR truncation and INT_MIN, must be retained.
C4's candidate is scratch-only pending its RED fixture at this point.

AC final suite: `sprite_dispatch_ac_retail_test.Vl30iViV` passes 4,616,193
cases at each O0/O2/UBSan and rejects all 12 scratch controls. The angle
cycle uses range0 to preserve and cover every masked angle. The final
negation-order control was further isolated to move only negation after
the arithmetic shift; `sprite_dispatch_ac_retail_test.0tV1Z1aS` rejects it
at case0/offset20. That targeted rerun does not rerun the three baseline
suites. See [machine-readable AC results](ac-test-results.json).


## C4 two-pass velocity transform (2026-09-05 23:32 UTC)

After semantic RED `sprite_dispatch_c4_retail_test.ZShvzpv6`, C4 now uses
its actual RNG and the existing native zero-X/Y RotMatrix, whose table
and specialization were independently verified. A command-local PC helper
preserves retail ApplyMatrixLV's two GTE operations, signed16 IR inputs,
MAC outputs, and wrapping INT_MIN split. Global PsyCross callers are
unchanged. The non-PC path calls the existing ApplyMatrixLV.

Final differential suite `sprite_dispatch_c4_retail_test.nEO2mwlU` passes
71,529 cases each at O0/O2/UBSan, with all six negative controls rejected:
narrowed input, unsplit input, swapped GTE stages, saturated output, signed
range, and reversed trig sign. Comparison covers the whole guarded fixture,
RNG state/count, five rotation-control words, IR1–3, MAC1–3 and FLAG.
Both sides share PsyCross's GTE backend; unrelated registers, exhaustive
32-bit inputs, hardware-GTE and full visual parity are not claimed.

Native build `pc_port/build_native/opening-c4-port-build-20260905.log` is
LINK OK with the same unresolved-symbol/stub inventory and override checks.
Binary SHA `e30c36c8dd99b8815b0360a62d0b45368a03ab9207dbed1877a135469e626e67`;
source SHA `e537139276d509ae7a53ffc637fcfe3a90ce136a8e84969c07781538b56db108`.
Isolated `make-check.venv.after-c4.log` again reproduces the existing
undeclared stderr failures in temp1 and animation_scripts.

Normal 5x replay **`opening-c4-5-0bdj7vdk`** passes C4 and B5, then naturally
stops at **BA** (raw186/index48), sprite `007DFEF8`, operands `0079EDE1`.
The file50 stream has BA at offset56C, operand02 at56D. Its handler
`80020F38..80020F4C` calls existing native `80023290` with the unsigned
operand. That helper sets blend/type flags, with separate type8/9 handling
and the normal render-update call otherwise. BA RED fixture is in progress;
no skip or state substitute has been installed. Text and full opening exit
remain unresolved.

After C4, compatibility checks pass for AC (`WOMhpPRr`), C1 (`XBVio0Y5`),
E5 (`UpoqCgW5`), A1 (`B1XAG8ws`), B4 (`GPetJOZL`), BB (`K1g7mIqc`),
and the fixed DATA/NOOP/8C artifact directories. These latest runs used
host GCC16.2.1 for production bodies and Homebrew Clang22.1.8 for fixtures
and linking, with statically linked UBSan runtime. Their existing negative
controls remain effective. New direct-GTE dependencies have fail-fast
linkage guards in unrelated fixtures; runner include paths now expose
the native GTE header. The A1 mutation extractor stops at the next case
instead of assuming AD immediately follows A1. No production change or
assertion suppression was needed for this compatibility work.

Continuation pointers: BA production is still unchanged. Scratch candidate
`/tmp/xeno-opening-ac-20260905/animation_scripts.ba-candidate.c` adds only
the handler call and declaration; wait for the independently owned BA
semantic RED before applying it. Agent `ba_regression` owns its new
fixture/runner and has been narrowed to that initial gate. No game process
from the latest automated replay remains live. Root retains production
ownership. Current HEAD is unchanged and staged diff is empty.


## BA blend/type dispatch and primitive refresh (2026-09-05 23:45 UTC)

BA was enabled only after semantic RED `sprite_dispatch_ba_retail_test.2gfhHgl1`.
The source change is the retail unsigned-operand call to existing 23290;
its helper and rendering code remain unchanged. Final suite
`sprite_dispatch_ba_retail_test.zI77rbK6` passes 94,208 cases per O0/O2/UBSan
and all seven controls. Both sides execute the helper and primitive-refresh
leaf, with transparent entry observations and whole guarded fixture comparison.
Coverage spans all256 operand bytes, all16 types, 23 operand aliases,
finite flag modes, and64-frame backing. Initial spy-only GREEN
`deoQ1TZE` is superseded by this actual-leaf evidence. Displayed rendering
and arbitrary primitive/sprite overlap are outside this fixture's claim.

Native build `opening-ba-port-build-20260905.log` is LINK OK; binary SHA
`44e4e128bb90a08c35d287127ce1e39afbeb131c465703d3f307a8fe674e1d98`,
source SHA `f330455910669b10d8bdc7542a9dad8c36c05c1245d7e576fa22da093a01563d`.
Isolated `make-check.venv.after-ba.log` retains baseline stderr failures.
Normal5x replay **opening-ba-5-yx4cte22** naturally passes BA then stops on
**91** (raw145/index7), sprite007DFEF8, operands0079EDE3. The latter points
to the next F1 command: 91 consumes no operand. Its handler80020DE8 jumps
to80020E04, clearing sprite+2B bit0 then calling the same primitive refresh.
The run has ended; no process was killed or state substituted. The 91
fixture is in progress and its candidate remains scratch-only pending RED.

Read-only leaf audit also records that retail1F6B0 reloads framecount+40
each loop while native captures it once; this differs if primitives overlap
that sprite byte. No such overlap was established in the opening or changed
in production. Disassembly is `/tmp/xeno-opening-ac-20260905/ba-render-leaf.asm`.


## 91 and F1 continuation (2026-09-06 UTC)

91 now clears sprite+2B bit0 before the actual primitive-refresh call and
consumes no operands. Final fixture `sprite_dispatch_91_retail_test.TkMEpEgx`
passes 65,536 cases per O0/O2/UBSan and five negative controls, including
NULL operands and actual leaf effects. The normal 91 replay reached the
blank pilot panel; one recorded normal Circle confirmation then advanced
it to unsupported F1. That run includes normal battle input as recorded
in its input-actions.jsonl; no game-state write was used.

F1 now preserves retail RGB byte-store ordering, sequential mode-2 operand
rereads and unsigned halfword stores, and the final mode reload before
primitive refresh. Final `sprite_dispatch_f1_retail_test.fi0jDt4b` passes
19,809 cases per O0/O2/UBSan and all nine isolated semantic controls.
The test executes the actual retail handler and actual retail leaf on its
oracle side. An intermediate fixture that substituted the native leaf was
rejected and restored before acceptance. Handler-only trace boundaries and
valid 64-frame primitive backing were repaired; tests cover each channel's
complete byte domain, finite RGB combinations, operand aliases and model
base aliases that change the final mode. Exhaustive RGB cross-products and
arbitrary primitive/sprite overlap are not claimed.

Native build `opening-f1-port-build-20260905.log` is LINK OK. Source SHA:
`2e9800535bca59bfff3f38a06d444277632aaa4d1c3e4740c3b03514e7d30e12`;
binary SHA:
`47c51cd99e2e45f76fd942e0af37063cb237811d5e403b706857d77eae64eabb`.
Isolated `/tmp/xeno-opening-ac-20260905/make-check.venv.after-f1.log`
retains the existing undeclared-stderr failures in animation_scripts/temp1.
There is no full ASM/BINARY-MATCH claim.

Replay `opening-f1-5-n16qbtw0` is no longer running. Its log confirms it
passed F1 and stopped at **F2** (raw242/index104), sprite007DFEF8,
operands0079EDED. Root recorded only the normal New Game menu input in
this run; the reason the pilot wait advanced is not established by the
input log. Do not invent another recorded Circle confirmation. The last
viewed pilot frame still had no visible letters. Fei's text and the full
opening exit remain unresolved. F2 is the next retail handler to implement;
its type15 path also calls the six-argument battle function800B2AEC and
must not be replaced by a no-op.


## F2 and native primitive colors; E7 scale (2026-09-06 UTC)

F2 is now native. It preserves signed byte deltas, RGB clamping, sequential
operand rereads for wrapping model-color halfwords, final-mode reload, and
the type15/header/bit1 gates for the six-argument B2AEC call. The existing
clamp and sprite primitive-refresh helper remain unchanged. Final handler
fixture `sprite_dispatch_f2_retail_test.B7w8w0ws` passes287,288 cases per
O0/O2/UBSan and five isolated controls. Its oracle executes actual retail
handler, clamp and refresh instructions. B2AEC is explicitly a six-argument
boundary observation in this handler fixture; its implementation is tested
separately below. The census includes complete single-byte RGB values, finite
delta/gate combinations, operand aliases and backed base-pointer aliases.
Invalid pointers accidentally created by fixture initialization were repaired;
no production fallback was added.

`src/battle/primitive_colors.inc` implements the complete B2AEC color routine,
shared by `src/battle/main.c` and the port's existing game_overrides owner.
The rest of the battle translation unit remains excluded from the native link;
no build-config or overlay-dispatch change was needed. See
[f2-primitive-colors-audit.md](f2-primitive-colors-audit.md) for the exact twelve
handled packet forms, four no-write forms, packed offsets, two independent
strides and store/copy ordering. Native source SHA:
`35596a9c1dc0ddaf6c590c7b4c305f0d394cc972c832114b649394dd20802db9`.

Initial leaf result GRzICq01 substituted the native clamp at the oracle
boundary and overstated source-overlap coverage. It is superseded by root's
corrected `battle_primitive_colors_retail_test.5ymTLD4m`:4,230 cases per
O0/O2/UBSan, all seven controls rejected with semantic mismatch exit1.
The corrected oracle executes actual battle.bin B2AEC and SLUS clamp bytes.
Its regular key/color census uses full-sized descriptors, and added cases
actually overlap descriptor records with output stores. Whole guarded arenas
are compared. This is finite memory-behavior proof, not rendered pixel proof
or an exhaustive descriptor/32-bit-delta universe. F2 supplies signed16
model deltas, keeping the native clamp addition within its defined range.

Native F2 build `opening-f2-port-build-20260906.log` is LINK OK; binary SHA
`d58adac3ef29d0e7e639cdc55de80d661fa615a53f1609ae28d0a80bcabe7126`.
Normal 5x replay `opening-f2-5-687dqhh4` passed F2 and stopped at **E7**
(raw231/index93), sprite007E0EF8, operands0079FDF1. Its last frame5640
shows both Gears in the burning scene; no pilot letters are established by
that frame. F1 regression `sprite_dispatch_f1_retail_test.4dccCWGA` passes
19,809 cases per mode and nine controls after F2. Legacy standalone
dispatch fixtures now have an aborting B2AEC linkage guard; their behavior
checks were not weakened.

E7 retail80021340..80021374 adds a doubled operand after narrowing to
signed16, then calls existing SpriteSetScale. Semantic RED was
`sprite_dispatch_e7_retail_test.WXnNkARa`. Candidate test
`sprite_dispatch_e7_retail_test.Jlf177YE` passes132,073 cases per
O0/O2/UBSan and six controls: missing doubling, wrong initial-scale field,
missing existing scale, swapped operand bytes, adjacent corruption, missing
helper. Complete16-bit operand and initial-scale domains are enumerated
independently, with finite edge pairs, null/overlapping transforms, operand
alias and high opcode bits. `e7-install-proof.json` verifies that the source
installed afterward is byte-identical to the tested candidate.

Native E7 build `opening-e7-port-build-20260906.log` is LINK OK; source SHA
`45385b9d8f54e2262f2fc0e8f6f8294df28ea494008559615cd1912436d42a6c`,
binary SHA `26a63852faa19fdd32b1f9fc233cfede5bd0249e2d2d9af5496e42c0e9069330`.
Isolated after-F2 and after-E7 make checks retain the baseline undeclared
stderr errors in animation_scripts/temp1. There is still no full
ASM/BINARY-MATCH, pilot-text, or complete opening-exit claim.

E7 runtime follow-up: normal5x `opening-e7-5-ycuq446v` passed E7 and
stopped at **AA** (raw170/index32), sprite007E1714, operands007CF699.
Frame5940 shows the Fei/Weltall pilot panel without visible letters.
AA800216F4..80021730 multiplies a signed operand by signed sprite scale,
rounds toward zero before shifting12, calls existing22CAC, and adds its
result as wrapping16.16 displacement at sprite+4. Its scalar domain keeps
the existing native helper's multiplication in range. AA RED is in progress;
root's candidate remains in `/tmp/xeno-opening-ac-20260905/`.
F2 after-E7 regression `sprite_dispatch_f2_retail_test.9TFsncPQ` passes
287,288 cases per mode plus all five controls.

The retail-emulator reference capture is a separate in-progress task. Initial
owned profile `/tmp/xeno-retail-opening-reference-20260906-8mpinaz6`
played the opening movie, but no pilot frame or naturally reached useful
savestate was captured. Sol now owns its isolated restart/input investigation;
root is not sending desktop input concurrently. Do not use the initial
native pilot images as retail pixel references.

### 2026-09-06 UTC — AA verified; callback ABI investigation

AA is installed with the retail signed-byte operand, signed scale, truncation
toward zero, existing 22CAC helper, and wrapping 16.16 position addition at
sprite+4.
RED `sprite_dispatch_aa_retail_test.0u819eSn` precedes GREEN
`sprite_dispatch_aa_retail_test.jxXDWimd`: 201,216 cases per O0/O2/UBSan
and six controls. The oracle executes the actual retail helper; its bridge
only observes arguments. Full independent operand, scale and timer domains
are combined with finite edge positions and four operand aliases; this is
not exhaustive coverage of their Cartesian product.

Native build `opening-aa-port-build-20260906.log` is LINK OK, executable SHA
`a2b0826171ab50f36ea4e3c0af6fedd951601a4975e654d96beab8d2232066ce`,
animation source SHA
`e423b31fa6b7cb881f677a141b8dfa7c4d4ebdafdc3dc177639cb4316241bff7`.
The isolated matching check retains the baseline undeclared-stderr failures.
Normal New Game run `opening-aa-5-0rmj97mi` passed AA and stopped with
`unresolved native call target=0x007fc208 guest-pc=0x800c1e6c` inside
callback800C11CC. Last frame6660 shows the Gear scene; no text/exit proof.

Retail800C1E5C loads sprite+68, then JALR at800C1E6C. Astra's read-only
audit finds the target maps exactly to guest801E93E8 in this executable's
g_PsxRam. Native setter80021BF8 stores argument1 verbatim there, but the
bridge omits that argument from callback classification and translates it
as data. Root's owned diagnostic normal replay is tracing the complete
setter chain before changing production. No widened target acceptance or
unknown-pointer fallback is proposed. Retail reference capture remains
separate and pending; Fei text and full opening exit remain unverified.

### 2026-09-06 UTC — Sprite callback argument classification repaired

The bridge now preserves argument1 of func_80021BF8 as a callback address;
argument0 retains normal sprite-data translation. The entire production
change is one classification entry. No accepted address ranges or fallback
rules changed. Runtime source SHA:
`fd4fae9b90677538e8ec7b754e814690d2de6434c39165a512c32b7ca07a6d80`.

Regression RED `battle_sprite_callback_retail_test.a2vAHke3` reproduced
guest callback translation into a host RAM alias in all three builds. Final
GREEN `battle_sprite_callback_retail_test.FL2rXqoy` passes O0/O2/UBSan
(with function-type checking excluded for the existing generic bridge ABI).
It includes the actual production runtime, executes the pinned retail setter
through the MIPS adapter, checks translated sprite data and preserved guest,
zero and native callback values, and dispatches the stored guest callback.
The callback-execution harness relocates the pinned simple retail setter into
a test address and proves its write by clearing a nonzero sentinel; this is
not a claim to execute the opening module's callback in this test. Removing
the new classification reproduces the semantic failure. Root verified source
pins and that the runtime differs by exactly one entry.

The diagnostic replay `opening-callback-trace-5-orqx88rm` followed normal
New Game and recorded normal pilot confirmations. It observed50 direct native
setters, then ended in HeapConsolidate SIGSEGV while opcodeE0 allocated a
child sprite. It did not observe the earlier guest setter-to-slot chain.
Both findings remain separate in `callback-diagnosis.json`.

`sprite-callback-retail-audit.md` proves callback801E93E8..801E9430 exactly
matches archive directory20/file1 loaded at801E5000: its72 bytes hash
`df0c3521a9f2dcfab542d62a8b63e03b627c4dacfdcc933e96868324311bcb5e`.
The saved RAM matches the module's entire code/table prefix through4C30.
Only its final three mutable data words differ. Three retail callsites
explicitly supply this callback to80021BF8. Thus the classification repair
is source-backed even though original live setter causality is unobserved.

Native build `opening-sprite-callback-port-build-20260906.log` is LINK OK,
executable SHA
`4fc351ed0c3de68780798c773a56ddaf5a297aae51c05259fcfec3673d2fb29a`.
Normal replay `opening-sprite-callback-5-b3kuz6dh` is in progress.
Fei text, full opening exit and the diagnostic heap failure remain unresolved.

Callback-repair runtime follow-up: `opening-sprite-callback-5-b3kuz6dh`
ended naturally at `unresolved native call target=0x0050118e
guest-pc=0x800afd74`, callback800AFC68. The pinned executable maps0050118E
exactly to native func_80022DF4 (AnimTask timer tick). Retail800AFD6C
loads the task callback from s0+4, then JALR at800AFD74. Native233A4
registers80022DF4 through TimerWorkListSetTaskCallback. The generated bridge
map lacks this symbol because the input retail symbol maps lack its explicit
assignment. Next bounded task: add/verify the retail-backed callback map
entry and regression without accepting arbitrary host pointers.

This replay used normal New Game plus the field-only input schedule, with no
extra pilot confirmations. Last frame6480 shows a small field actor beside
the purple Gear. This is a new observed stop, not proof of complete opening
parity or resolution of the separate heap crash. PID2942841 has ended.
Desktop input ownership returned to Sol's isolated retail-reference lane.

### 2026-09-06 UTC — Native animation task callbacks mapped; C0 reached

The retail symbol map now explicitly names80022DF4,80022E8C,80022EB8.
Retail233FC..2341C registers the tick/free callbacks;24878..24880 registers
the type7 tick variant. No runtime target ranges or fallback rules changed.
`battle_native_callback_map_test.YHMIfInO` is semantic RED before the entries;
`battle_native_callback_map_test.uRBPP6WZ` is GREEN at O0/O2/UBSan (the
existing generic function-type ABI is excluded from UBSan). The test uses
the actual runtime and production generator inputs, distinct boundary spies
for all three callbacks, translated guest data, unknown-host rejection and
generated-stub rejection. This is dispatch/ABI proof, not callback-body
retail parity. Source pins were verified after the tests.

Build `opening-native-callback-map-port-build-20260906.log` is LINK OK.
Executable SHA
`8fcf9a158838a5a9f2716b7f3b3762e1ee91228c8210af407ce3627dd995773f`.
The isolated matching check retains the baseline undeclared-stderr failure.
Normal replay `opening-native-callback-map-5-e2iikr3u` passed the callback
stop and ended at unsupportedC0 (192/index54), sprite007C97C0, operands
007C472D. Last frame7920 shows a character beside the Gear; no full-exit or
pilot-text claim. PID2983591 has ended.

`child-spawn-retail-audit.md` finds no child-constructor size/packed-pointer/
copy overrun mismatch in the bounded audited path. The prior heap crash
occurred before that child's initialization, and the first damaged heap
link remains unknown. It separately finds23538's missing recomputation after
default scaling: retail236EC falls through236F4/23708 while native uses
else-if. Root owns its repair; Luna is preparing a focused regression.
C0's retail jump table and handler20158..201F0 are identified; root candidate
is scratch-only while a second Luna worker prepares its actual-retail test.

23538 transform correction is installed: the enable-flag check for matrix
recomputation now follows the conditional default-scale block independently,
matching retail236EC ->236F4 ->23708. Initial RED ciphe7Hq and candidate
GREEN CAneQe0k cover the exclusive-branch defect. Final
`sprite_bind_transform_retail_test.SCA0V7Iq` passes32 cases per O0/O2/UBSan,
including Scale changing the enable flag to0 or2, header bits12/13, flag0/1,
and null/non-null backing. Both the old else-if and cached-condition controls
are rejected. Actual retail23538 instructions run with explicitly labeled
Scale/Compute boundary spies; this proves call order, arguments and controlled
effects, not the complete matrix algorithm. Source pins were verified.
C0 remains scratch-only pending its separate retail differential fixture.

### 2026-09-06 UTC — C0 installed and combined native replay

C0 retail20158..201F0 is implemented with two RNG calls, the byte-range
operand and signed scale, negative rounding, separately slow-scaled trig
components and wrapping X-add/Z-subtract. The existing retail symbol-map
convention names the cosine entry rsin and sine entry rcos; the candidate
was corrected before installation, preserving the wrong-trig variant as a
negative control. No GTE or rendering changes were made for C0.

`sprite_dispatch_c0_retail_test.QXXw9aHy` is semantic RED at O0/O2/UBSan.
Tested candidate `sprite_dispatch_c0_retail_test.pgG1PzwW` passes135,809
cases per mode and six controls: wrong RNG count, unsigned scale, signed
operand, missing rounding, wrong Z sign and swapped trig. Its oracle runs
actual retail dispatcher/RNG/trig/22CAC instructions. It covers independent
full operand/scale/timer axes plus finite seeds, position-wrap cases, operand
including RNG-seed aliases, whole fixture and final seed. This is not the
full Cartesian product or rendering proof. `c0-install-proof.json` pins the
installed source to the tested candidate.

Combined native build `opening-c0-transform-port-build-20260906.log` is LINK
OK, binary SHA
`6974e6da2c068ef3f64951499090c40088a171694d4300cee90b4dc9eacfd3d1`.
Animation source SHA
`e6423f2de9b3a3954125a0ec8cbd97defe70273c0f8a61e594f24e4b3f9a8179`,
temp1 source SHA
`494f6f78dc51aaf65e57d1bad5bc7a06e288059134de89c907648e709032d441`.
The isolated matching check retains baseline stderr errors in both files.
Normal replay `opening-c0-transform-5-0qxtsrd_` is in progress; its launch
manifest now also pins the explicit retail symbol map. No extra gameplay
state is injected. Fei text, full exit and the separate heap-crash first
writer remain unverified.

Combined replay result: `opening-c0-transform-5-0qxtsrd_` ended at
unsupportedA5 (165/index27), sprite007CA688, operands007CA725. Immediately
before the stop it logged missing child render table index9. Last frame7020
shows a character beside the purple Gear; no extra pilot confirmations were
used in this run. PID3059864 has ended.

A5's retail table entry points80021884; handler through218DC updates speed
at sprite+18 using signed operand and sprite+82, wrapping time multiplier,
then existing22974. Root scratch candidate is prepared; Luna owns its
actual-retail differential fixture. No A5 production edit yet.

Retail D_8004FD40[9] is80025544. The table currently binds NULL. Astra's
initial audit identifies this as a TILE+E1-mode renderer and finds material
projection, size, packed-pointer and OT-stride mismatches in the existing
unused draft. Do not wire that draft unchanged. The separate audit is
`/tmp/xeno-opening-type9-render-audit-20260906.md` (in progress). Its relation
to missing Fei text has not been established.

Retail reference input follow-up: keyboard Circle was unobserved. Sol added
only ordinary Circle API endpoints to its isolated Lua helper, preserved the
profile/title state/logs, and restarted the owned emulator. New PID3069504
was reported. Restoring the natural title state then API Cross->Circle
produced the distinct `I am Alpha...` prologue at+3 seconds; the confirm
input now has positive runtime evidence. Gear panel and exact text remain
pending natural progression. Desktop input ownership returned to Sol.


### A5 implementation and type-9 review (2026-09-06 continuation)

A5 is now installed, SHA `51cef2ac3afc4ffce35925b28701370d1ac32843951bf207ffec65c6d4da4b53`.
The candidate runs the signed-byte/time/scale low32 arithmetic and calls the
existing retail-verified22974 velocity helper. Initial production fixture
`A12zLNDl` is semantic RED. Root strengthened the test to compare exact
trig call counts, order and effective masked arguments, and corrected its
failure diagnostic formatting and alias scale selection. Native helper masks
before the call while retail masks within each leaf; raw argument equality
was an invalid test assumption and is not claimed. Final artifact
`sprite_dispatch_a5_retail_test.e8KVAtIB` passes74,602 cases each atO0/O2/UBSan
and seven negative controls, with actual retail dispatcher/A5/22974/trig
instructions. See `a5-install-proof.json`. These are finite independent
axes, overflow/zero-divisor edges and operand aliases, not a full Cartesian
census or runtime/rendering proof.

Type-9 scratch renderer and binding candidates passed Astra independent
read-only review against the exact retail leaf. Its regression is still
in progress; production type9 remains unchanged pending those checks.

Correction to the preceding retail input report: the Alpha/Omega movie
transition did not establish New Game. Sol subsequently verified the menu
cursor beside New Game, but an ordinary two-second Circle pulse left the
framebuffer unchanged. New Game, Gear panel and exact text remain
UNRESOLVED. Current isolated emulator PID3106417 is preserved; the worker
is tracing the ordinary controller press/release path. No game-state
planting or native-to-retail substitution is authorized or used.

A5 native build `opening-a5-port-build-20260906.log` is LINK OK, SHA
`232a29a79fd80710a241c569a1672dc012dbad24df02be14951f446c22565246`.
Isolated matching check `make-check.venv.after-a5-correct-cwd.log` retains
the baseline undeclared stderr failures in animation_scripts/temp1; the
first attempt mounted at /work and failed project discovery, then was
rerun from the required project basename. No byte-match claim.

Normal replay `opening-a5-5-kaiuio4z` entered New Game from ordinary Up/Circle
input, rendered multiple moving Gears and reached the type9 missing-render
warning. PID3167589 then ended without an opcode assertion or recorded
exit cause; last capture11880 shows the battlefield without actors.
Frame count does not prove A5 execution or full battle exit. See
`a5-runtime-result.json`; a separate read-only GDB stop/heap capture is
now running to identify the actual stop.

The separate debugger replay `opening-heap-stop-5-72kk118u` stopped with
SIGSEGV in HeapFree memory.c389, called by GfxLineScrollFree line_scroll.c73.
The native LineScroll struct has widened pointers: unk14 is accidentally
at host offset18 instead of retail14. The captured actual record at
guest800C3DB8 has NULL at14 and unrelated800E97DC at18, which native passed
to HeapFree. The heap walk from actual g_Heap reached an end marker; this
does not establish the cause of the older HeapConsolidate crash. See
`line-scroll-free-crash.json`. Luna owns a bounded actual-retail cleanup
regression and scratch candidate; production cleanup is not yet edited.

The retail-emulator lane is finalized UNRESOLVED in
`retail-input-capture-20260906.md`, SHA4045bbc79d167ee8158ac9467b7659bf3f27c04dce2c4e52868c91d8f022b039.
The fresh Circle press after a3.066-second settled NewGame selection also
left all frames unchanged. Mapping/edge conclusions were checked against
retail menu.bin and SLUS bytes; simple selection timing is ruled out.
PID3106417/profile/title state remain preserved; no further live retries.

### Type-9 renderer installed

The reviewed native80025544 projected-square renderer is installed and
bound only to D_8004FD40[9]. Exact source hashes and tested candidate paths
are in `type9-install-proof.json`; audit is `type9-render-retail-audit.md`.
It emits the retail square TILE and draw-mode packet with packed32 pointers,
v0/v1/v0 projection, signed projected dimension, floor centering, strict
buffer limits and mode->tile->old OT links. No dialogue relationship is
claimed.

Astra replaced the incomplete initial regression's native-SDK oracle
substitutions with a direct MIPS bus. Final artifact
`battle_child_tile_retail_test.GuFD7hpi` passes20 controlled cases at
O0/O2/UBSan and20 actual-SDK cases atO0/O2, plus14 negative controls and
the old draft. Retail matrix, RTP3 and AddPrim bodies execute as instructions;
native links the actual SDK and production guest_prim_link.c. Full visible
GTE register reads and guarded task/sprite/work/OT snapshots agree. Both
sides share PsyCross COP2, so this is not physicalPS1 or pixel proof.
Root independently confirmed matrix-upload offsets and reg4 sign extension.

The existing billboard regression now expects the real type9 binding and
passes its positive builds and seven controls; see
`/tmp/xeno-opening-ac-20260905/billboard-after-type9.log`. Combined native
build/replay awaits the separate LineScrollFree cleanup regression.

### Line-scroll cleanup installed and combined replay

Packed LineScrollFree is installed, SHA073e54815ee5b12c93e86272c9c0c0e4b512c17ebfd68f6370c8d10946d78073.
It reads the four-byte member at14, translates only actual2MiB KSEG0/KSEG1
RAM aliases for the HeapFree boundary, frees before clearing exactly four
bytes, and leaves the matching branch unchanged. Root restored the
misnamed scratch backup and preserved the tested full candidate separately
as `/tmp/xeno-opening-ac-20260905/line_scroll.full-free-candidate.c`.
Final root fixturejggq4lzB passes four bounded cases perO0/O2/UBSan and
four controls. The runner derives controls from the selected source rather
than a hardcoded scratch candidate; exact source/harness/runner pins are
checked at completion. See `line-scroll-free-install-proof.json`. Native
Initialize/Update and field scroll pointer arrays remain separate unresolved
layout work; this is cleanup-only parity with an explicit HeapFree spy.

Combined native build `opening-a5-type9-cleanup-port-build-20260906.log`
is LINK OK. Isolated matching check still has baseline undeclared stderr
errors in animation_scripts/temp1; line_scroll compiles. No byte-match
claim. The new normal replay records native return status in exit.json
through an owned monitor, and its source manifest now includes line_scroll.

Replay `opening-type9-cleanup-5-s6ef07mm`, PID3288739, binary SHA
`36b934f2899d5514093d98a66753a555211914f95dbf1aa9182901eeb4886af1`, is running. Runtime pixels, Fei text and full exit remain pending.

### Native battle return observed (2026-09-06 02:25 UTC)

Normal replay `opening-type9-cleanup-5-s6ef07mm` logged
`retail battle returned after 368858285 instructions`, then loaded field14
and rendered Fei standing at his painting-room easel. Capture16080 and
source/binary pins are recorded in `type9-cleanup-runtime-result.json`;
`painting-room-after-battle.png` is a format-converted copy. No unimplemented
sprite opcode or missing type9 callback warning occurred. This establishes
the native battle-to-room transition, not complete visual/lettering parity.

Root gracefully stopped only owned PID3288739 after this observation to
end unattended capture; its monitor recorded exit0 after the requested
SIGTERM. Both native process and monitor have ended. Retail emulator
PID3106417 remains preserved and its input/capture lane is UNRESOLVED.

Next work: inspect the missing Fei lettering and remaining retail visual
differences using the special animation/VRAM archive evidence, preserving
this successful transition as the regression baseline. Native LineScroll
Initialize/Update and field pointer arrays remain known adjacent layout
work; the earlier separate HeapConsolidate first writer remains unresolved.
No commit, staging, push or memory write was performed.
