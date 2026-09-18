# Target display wrapper at800BC404

Status: native scalar-mask interface corrected; wrapper differential tested
with simulated downstream calls. Retail C now matches BC40420/20 and
BC2F0 66/66 with the configured BattleCdk toolchain. Full display
integration, runtime admission and natural rendering acceptance are unproven.

## Retail authority and interface

Battle SHA256:
`1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291`.
BC404 body is file offset4C914, size50 hex (20 instructions), documented in
asm/battle/mainc114.s. It saves the entire a0 value and forwards it unchanged
to BC460. In asm/battle/mainl115.s, BC460 saves that argument to s3, stores
it as a word at800C3678 (800BC4BC), and tests/shifts its bits from800BC4CC.
It is a scalar mask, not a pointer to dereference. The full32-bit word matters
even though the UI controller supplies only a low16-bit value.

Corrected only mainc114's BC404 definition and BC460 declaration from u8*
to u32, naming the value mask. The wrapper's logic is unchanged. A forced
test API header first failed compilation on both incompatible pointer types;
the corrected native body compiles against that same header. This prevents
the ABI regression from silently returning in the tested translation unit.

## Wrapper behavior

Read byte800C37C8 once. If zero, callBC2F0(1), thenBC460(original mask),
without rechecking the flag between calls. Whether skipped or executed,
reload halfword800C3CDC after the calls and copy it to halfword80059454.
Preserve s0/SP/RA; no wrapper return-value contract is inferred for this void
routine. Downstream calls may change the flag/source before the final load.

## Verification and limits

`run_battle_target_display_test.sh`, wTDrcH terminal0:786432 cases per
O0/O2/UBSan and expanded O2. All65536 low-mask values, four upper patterns
0/ABCD/FFFF/8000, and gate bytes0/1/255. These are representative upper-word
and nonzero-flag classes, not exhaustive32-bit masks or every flag byte.

Both actual wrapper bodies execute. BC2F0/BC460 are explicit simulations that
check call order/arguments and require the destination still unchanged on
entry. They set the gate nonzero and modify the source halfword. Guest writes
are limited to the two stack saves and one destination halfword; native
results and neighboring byte/halfword guards are compared against retail.
Native exact store counts are not inferred from final values.

Three native source mutations are rejected at relevant assertions: truncate
the forwarded mask, recheck the changed flag before BC460, or cache the source
before callbacks. Expanded baseline and no-change mutation guards pass.
Earlier TLefvI established the three positive builds before mutation coverage.

Historical wrong-toolchain result: `run_battle_target_display_match.sh`,
YW2pMM terminal1:FAIL16/20, exact50 size
and addressBC404. Differences are constant1 instruction form at20 and the
load/store register choices at30/34/38. Neighboring text is parked, so this
is an isolated gate, not a full-overlay link. No instruction patching or
compiler-flag change was used then. This result is superseded: the runner
incorrectly used Default rather than the established BattleCdk preset for
mainc114. Corrected evidence is recorded below.

The BC404 mask/pointer mismatch is corrected, but BC2F0/BC460 still need real
native implementation/integration evidence. The UI controller's simulated
DISPLAY callback does not automatically become a full-native display test.
No runtime dispatch or rendering path was activated.

The UI suite now also has an actual-BC404 integration mode on both retail and
native sides; see BATTLE_TARGET_UI_CONTRACT.md. Downstream functions remain
simulated. That mode now covers entry gates0,1,255 and36864 controller cases
per build; see the UI contract for evidence and observer details. This is an
additional composition proof, not full rendering or arbitrary mixed-gate
runtime evolution.

## Next downstream boundary: BC2F0

Read-only retail trace in asm/battle/mainc114.s (file4C800, addressBC2F0,
extent108 hex) identifies the following setup before any native replacement:
it stores the entire input mode to C3CC0 and initializes C3CBC to1. Mode4
changes C3CBC to5 and returns. Mode2 reads six signed halfwords from D30A0
at offsets0/2/4 and8/A/C and stores their low32-bit left-shift-by16 results
as two three-word vectors at6F99C and6F9AC. A native expression must avoid
left-shifting a negative signed integer; unsigned bit shifts preserve retail.

Other modes, including this wrapper's mode1, inspect task root C3680. If
nonzero, they load its callback word at task+0C, call it with the task address,
and then clear C3680 unconditionally. Next they reload root C3684, perform
the equivalent callback/clear sequence if nonzero, and return. Therefore a
first callback can replace the second root, and a callback-installed value
in its own root is overwritten by the post-call clear. Do not cache both
roots before calling or clear a root before its callback.

`run_battle_target_setup_contract_test.sh`, 3ES9Mu terminal0, now executes
the real BC2F0 bytes in O0/O2/UBSan: 65592 cases per build. All 65536
halfword patterns reach each of the six vector components (correlated inputs,
not all six-component combinations). Seven full-width modes cross four root
presence combinations and callback replacement enabled/disabled. The harness
checks callback entry arguments/order, replacement of the second root,
post-call clearing of callback-installed roots, exact guest store counts,
vector gap guards, and saved registers/SP/RA. Only task callbacks are
simulated. An isolated instruction-copy mutation removing the first root
clear fails the required assertion. Retail files are unchanged.

The existing native work-list implementation retains packed 0x1C tasks and
a 32-bit callback at +0C. `PcPort_WorkListInvokeSavedCallback` distinguishes
guest code from native code without widening that slot. This identifies an
existing callback mechanism, but does not prove whether BC2F0's two roots
contain native pointers or guest addresses at a future integration boundary.
Root producer/admission evidence remains required; do not cast a guest root
directly to a host pointer or assume either root is null in gameplay.
At that gate BC2F0 had no native implementation. The BC404 composition tests
still simulate it; this separate retail test is not native integration proof.

### Staged native BC2F0 body (not runtime dispatch)

`game_overrides.c` forwards BC158 to `PcPort_BattleMipsDispatchCallback`.
`run_guest_callback` passes its raw task argument into guest a0; BC158 stores
that value into these roots. The runtime resolver supports both KSEG aliases
and validated low native addresses, and resolves main-executable shared data
before guest RAM. Thus a guest-only root interpretation is insufficient.

`pc_port/src/battle_target_setup.c` now implements the BC2F0 state, vector,
and callback behavior through a supplied memory resolver and synchronous
callback invoker. It never masks task handles or casts callback words into
host function pointers. Vector loads/stores retain retail access order;
unsigned shifts avoid signed-shift UB. Errors stop without undoing previous
effects. The API excludes guest frame/register residue and forbids retrying
retail after a partially executed failure. No runtime binding was added;
At this stage `src/battle/mainc114.c` still used INCLUDE_ASM for BC2F0.
That historical native-port-only boundary is superseded by the retail C
match below; the PC branch still uses its separate resolver/frame API.

The setup suite now covers three resolver fixtures: KSEG0, KSEG1, and mixed
low-native/KSEG0 task handles. These are synthetic address-domain fixtures,
not the actual runtime resolver or work-list allocator. Each body executes
196776 cases/build; the combined retail/native run reports393552 executions.
Native runs omit the two guest stack stores and do not claim guest CPU-state
equivalence. A separate operation-tape fixture injects failure at every
operation in task/vector/mode4/empty-root paths:35 tapes including successful
baselines plus3 invalid API cases. It verifies stopping and accumulated
effects, including effects of a failing callback. Actual allocator/free
callbacks, guest frame aliasing, and natural gameplay remain unverified;
shared-data resolver composition is tested separately below. Native
mask-task, wrong-clear, and narrow-mode
mutations supplement the retail missing-clear control.

Final evidence: `scratchpad/battle-target-setup.J4ylaf`, terminal0, all
O0/O2/UBSan positive suites, expanded-by-sed unchanged native baseline, and
four required negative controls pass. Independent Hegel read-only review
found no semantic issue and identified the previously absent failure tests;
the operation-tape suite was added afterward. Reviewer did not execute tests.

### Actual runtime resolver and guest callback composition

`run_battle_target_setup_runtime_test.sh`, HRweiT terminal0, passes448 cases
per O0/O2/UBSan with seven modes, four root combinations, four vector seeds,
and KSEG0/KSEG1/low-native/mixed handle domains. The fixture includes the
actual runtime source, builds the generated symbol map from current symbol
files, and initializes real host-range/shared-data resolution. Native setup
uses actual runtime_read/runtime_write and PcPort_BattleMipsDispatchCallback.
Native D_8006F99C/D_8006F9AC are resolved through dlsym; raw guest backing is
poisoned separately. Both aliases and actual low host task storage are checked.

Both setup bodies execute against that resolver. The callback is real retail
BC3F8 (stores its raw argument at C367C), deliberately installed in test tasks
as a marker, not an allocator/free-callback replacement. A full guest RAM
comparison excludes only BC2F0's two saved-register words; the entire native
0x18-byte frame is independently required untouched. Both shared vectors,
host task storage, native invocation count/order/arguments, raw marker,
and unchanged parent CPU/bridge context are checked separately. Direct
seed-derived vector expectations and padding/raw-backing guards supplement
parity. The leaf marker does not prove nested stack inheritance. The pad-pump hook is inert;
unexpected graphics/GTE use aborts. These remain isolated tests, not gameplay.

Masking the callback argument and bypassing the shared-data resolver are
rejected by exact marker/RAM assertions. Source pins are checked after all
builds and controls. oRoJ9G was a warning-as-error build failure, fixed with an
unsigned expected count. NCE55X aborted in the fixture pad-pump stub; a
backtrace identified the runtime's unconditional hook, and the established
inert fixture hook was used. woX5nM passed336 cases before mixed-domain
coverage was added. 0ApUrM passed448 before review-driven stronger assertions.
Hilbert's read-only review identified overbroad stack exclusion, missing
per-call argument checks, a shared-oracle vector gap, and the untested nested
SP boundary. The first three were addressed; nested SP remains explicitly
unverified. Reviewer did not execute tests. No runtime source or production
dispatch was changed.

### Actual child cleanup callback and recursive setup

BC158 installs BBEE0 at task+0C via WorkListTaskSetOnFreeCallback
(BC2B4), replacing the generic AnimTask free callback. BBEE0 (battle file
4C3F0, size138 hex) selects C3680 for sprite type10 and C3684 otherwise.
When that root equals its task, it clears the root even with C37C8 nonzero;
only a zero gate restores three saved halfwords C3CCC/C3CD4 to D30A0/D30A8.
Sprite+AC bit5 optionally calls1CE74. Then it calls TimerWorkListRemoveTask,
WorkListRemoveTask(task+1C), and HeapFree(task), decrements byte C3CC4 with
wrap, and calls BC2F0(C367C) if the decremented byte is zero.

The re-entry can occur before the outer BC2F0 visits its second root. That
recursive call can clear the remaining root, or set mode/state/vector data
that the outer call retains on return. Native integration must not overwrite
callback-modified mode/state or silently bypass recursive retail execution.

`run_battle_target_cleanup_retail_test.sh` executes actual BC2F0 and BBEE0;
only1CE74, timer/work unlink and HeapFree bodies are simulated. It crosses
four root combinations, gate0/1/255, owner bit5 off/on, every byte counter,
and recursive modes0/1/2/4:24576 cases per build. Counter values inconsistent
with fixture root count are deliberate arithmetic/re-entry tests, not claims
about reachable gameplay. Callback entry requires the matching root; every
unlink/free observation requires that root already clear and verifies source
restoration/gating, exact argument/order, and the current guest stack depth.
Nested callbacks specifically require SP801FEFA0, versus801FEFD0 for shallow
callbacks;12 deep callbacks are required across the suite. Final mode/state,
counter wrap, roots, vector/gap data and saved registers/SP/RA are checked.
For recursive mode2, vector expectations snapshot source halfwords at the
actual recursive entry (after checked cleanup restoration), rather than
assuming both roots were already cleaned up. An instruction-copy mutation
removing BBEE0's recursive JAL must fail the expected stack-save count
(and would also violate the setup-entry count). Missing callback root clears
and early setup re-entry before HeapFree are separate rejected controls.

Final evidence H6kzSW terminal0:O0/O2/UBSan positives and all3 controls pass.
Stack writes are restricted to save slots, counted per executed frame, and
all unused bytes are guarded. Recursive entry requires a finished hook phase
and completed-free count equal to the initial counter, in addition to the
zero byte and correct SP. These assertions address Hooke's read-only review
findings; reviewer did not run tests and was closed. Earlier5CZhCQ/yiSWgw/
Wp6agA are intermediate runs with fewer controls or weaker stack assertions.

This establishes the retail nonleaf callback/re-entry contract, not native
BC2F0 frame adaptation, actual list unlink/heap effects, or natural gameplay.
The native dispatcher fixture above still uses the leaf marker callback.

### Staged native guest-frame adapter

`PcPortBattleTargetSetupFrame` now shares the setup implementation with the
frameless API. It subtracts0x18 from SP, writes mode/state, saves RA then s0
in retail order, supplies callback a0/s0/SP and actual return PCs BC3B8/BC3E0,
clears through callback-returned s0, resets s0 for the second root, and reloads
RA/s0 from guest memory before restoring SP. Root loads update a0 before
callback-pointer resolution, including failure paths. The CPU must not alias
bus memory; aligned SP>=0x18 is required, while memory access validation is
delegated to the supplied bus. Other caller-saved residue and the instruction
pipeline/count are excluded. Errors retain preceding memory/register effects.

The cleanup test can now replace both outer and recursively reached BC2F0
with this native variant while BBEE0 remains real retail. A test-owned callback
invoker runs a child CPU and propagates GPRs even on failure; this is not yet
the production runtime callback integration. Both setup variants start with
reset input globals, and all2MiB guest RAM is compared with no exclusions.
The existing stack/callback/final-state assertions remain. Each combined
build reports49152 executions (24576 retail/native pairs), including24 deep
callbacks across both paths. Actual main unlink/free operations remain mocks.

Failure coverage now has86 operation tapes across frameless/frame variants
and6 invalid-API checks. Frame save/reload accesses are injected individually;
callback-failure GPR effects and a0 after root loads are checked. Guest-memory
alias tests beyond these conventional frames remain pending, as does real
runtime activation. Two native source controls corrupt callback return PCs
or saved s0 and must fail RAM-parity or register assertions respectively.
The three retail cleanup controls remain required.

Final cleanup7h0ASg terminal0: O0/O2/UBSan paired RAM comparisons, unchanged
source baseline, and all5 cleanup controls pass. Setup regression dOGAG0
terminal0 retains196776 cases/body/build plus86 failure tapes/6 invalid APIs
and4 controls. Runtime marker regression VwdbC2 terminal0 retains448 cases
per build and2 controls. Kepler's read-only review found the late a0 update,
failure-time GPR propagation gap in the fixture, and unreset initial mode/
state globals; all were corrected before these final runs. Reviewer closed;
cross-model review skipped for autonomous continuation. No production
dispatch, native build registration, vendor files, commits or pushes changed.

Historical wrong-toolchain BC404 expression trial lNn2vh introduced an explicit destination pointer;
it still emitted16/20,size50 and was reverted. No volatile access, weakened
prototype, or instruction patch was retained to force matching.

## Retail C exact matches and corrected toolchain

`gears.toml` BattleCdk and the generated build.ninja mainc114 rules select
gcc-2.7.2-cdk-psx with --dont-expand-li, retaining O2/G8. The earlier isolated
BC404 runner incorrectly selected default gcc-2.7.2-psx and li expansion;
its16/20 result is not the current exactness limit. Both isolated runners
now use the established preset; no project preset or compiler flag was
changed to accommodate a candidate.

Retail BC2F0 is now C in mainc114: callback-prefix pointer type, signed-short
source vectors, signed-word destinations, and an inline three-component
expansion helper. Casts to u32 precede shifts, avoiding negative signed-shift
UB; conversion back to s32 follows the verified target compiler. Source is
an array so the second short vector is not an out-of-bounds scalar object.
The PC preprocessor branch keeps the explicit resolver/frame implementation
separate and does not expose retail pointers as native pointers.

Final setup gfWOuz terminal0: exact66/66 instructions, size108 and
address800BC2F0. Display AzoGjN terminal0: exact20/20, size50 and
address800BC404. Both gates reject in-memory content, size and symbol-address
mutations using their actual acceptance predicate. Full byte equality is
required; counts alone cannot pass. The display linker intentionally parks
neighbor code and resolves its setup call to the retail address; these are
isolated matches, not full-overlay or gameplay acceptance.

PC wrapper regression a5FBeh terminal0:786432 cases per O0/O2/UBSan plus
expanded baseline and all3 source controls pass. Boyle independently checked
the pin, exact bytes, source/PC separation and gate rejection cases; no blocking
findings. Reviewer closed. Initial setup AdpKT6 failed missing implementation;
zgp5Bw/aBH7t3 used the wrong default toolchain. Correct-preset E55SHu was4/66,
then SUwtU4/NaE3Zd/aHU9BY43/66 atsize108. The inline helper reached eTBR2L66/66;
gfWOuz includes final gate controls. No forced registers, instruction patches,
volatile accesses, runtime dispatch activation, commit or push were used.

Normal `ninja build/src/battle/mainc114.c.o` stops at missing
`mips-linux-gnu-cpp` (exit127), before source compilation. Manual full-TU
compile EX4DVu using host cpp with USE_INCLUDE_ASM and the configured CDK/
assembler succeeds: BC2F0 at objectoffsetAAC,size108; BC404 atBC0,size50.
Existing included-assembly missing-.end warnings remain. This verifies the
surrounding assembly can compile, not a clean normal Ninja build or a full
overlay link. No build prerequisite was installed or changed.

## Guest-frame alias coverage

`run_battle_target_setup_alias_test.sh` compares480 retail/native pairs per
O0/O2/UBSan. Nine aligned SP positions cross modes0/2/4 and16 representative
halfword seeds;48 additional cases simulate a callback overwriting saved s0,
saved RA, or both. Stack saves overlap mode/state globals, either short-vector
source, or vector destinations. Each execution resets the complete RAM image.
Parity covers all2MiB, saved registers/SP/RA, writes and callback count.
Independent expectations check mode/state, vector words, restored s0, and the
return address, including source halfwords overwritten by the prologue.

Some aliases corrupt RA (for example mode4 overwrites saved RA with5). The
retail interpreter stops at the independently computed return target before
fetching it; this proves epilogue state, not runnable continuation at that
address. Callback edits are simulations and do not represent naturally
observed gameplay. Only the enumerated aligned layouts are covered, not
arbitrary SP/task overlap, callback-private-frame aliases, or every resolver
domain. Host-caching original s0 or RA instead of using guest reload results
is rejected by separate native source controls.

Final UjTbah terminal0: all480 pairs/build and both cached-register controls
pass, including independent expected write totals. Earlier NfTiLC predates
that count assertion. Avicenna reviewed the fixture and exercised existing
binaries/controls, finding no blocking issue; reviewer did not rebuild the
runner and was closed. RAM parity/counts do not establish identical write
ordering. Callback-edit cases affect only the first callback on a conventional
stack, not both roots or edits combined with overlapping frames. No production
code or runtime dispatch changed in this alias-test turn.

## Next BC404 callee: BC460

BC460's pre-geometry integer selection/center stage is now isolated as a
staged native helper; see BATTLE_TARGET_BOUNDS_CONTRACT.md for the retail
arithmetic, mask/actor domains and test limits. BC404 tests still simulate
BC460 as a whole. The new helper does not establish full display composition,
matrix/projection behavior, or runtime rendering acceptance.
