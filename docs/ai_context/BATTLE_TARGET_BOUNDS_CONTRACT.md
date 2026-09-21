# BC460 integer target center stage

Status: staged native math helper only. Full BC460 remains INCLUDE_ASM;
no runtime dispatch, guest-frame replacement, matrix call or rendering is
implemented by this helper.

The separate staged `battle_target_camera.c` now composes the complete BC460
algorithm and public writes through a passive memory bus; see the final
section. It is not installed in the native build or runtime dispatcher.

## Retail authority

Battle SHA256:
`1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291`.
BC460 starts at file4C970; body size644 hex. The tested nonempty path stops
at RotMatrixZYX address8004ABBC, after the JAL delay slot atBC6E0. The empty
path executes the real epilogue atBCA70 and returns. Only memset is simulated.

BC460 first reserves120 hex stack bytes and zeroes16 bytes atframe+10.
It stores the entire input mask atC3678, then scans11 slots. A slot contributes
only when its mask bit is set, byteC3EB0+slot*1C+7 is zero, and pointer
CCB3C+slot*4 is nonzero. Position words are at pointer+0/+4/+8. Bits above10
do not select another slot, though the stored mask retains those bits.

## Arithmetic preserved by the native helper

For each axis, add the arithmetic-right-shift-by-one position words using
32-bit wrapping. Interpret that sum as signed, divide by contributor count
with truncation toward zero, then double with32-bit wrapping. This value seeds
both minimum and maximum before a second scan expands those bounds using
signed comparisons. It is not interchangeable with initializing true extrema:
overflow can place the seeded value outside all actual contributing points.

Finally wrap minimum+maximum to32 bits, add its sign bit (0 or1) with wrapping,
then arithmetic-shift right17. These signed center components are the words
atframe+10/+14/+18 when the geometry call is entered. The helper implements
sign interpretation using int64_t and arithmetic shifts using unsigned bit
operations, avoiding signed overflow and implementation-defined right shifts.

`PcPortBattleComputeTargetBounds` accepts11 stable, already-resolved point
views, each containing a suppression byte and optional pointer to three
signed words. Output must not alias input. It returns count and center;
count/center are zero for no contributors. NULL top-level arguments return-1.
It does not store C3678, resolve native/guest addresses, or preserve guest
stack effects; those belong to eventual BC460 integration.

## Verification scope

`run_battle_target_bounds_test.sh` crosses2048 low masks, three upper patterns
(0/FFFFF800/80000800), three eligibility profiles (all present, selected slots
suppressed255, selected slots absent), and four coordinate datasets: small
signed odd/even values, INT32_MIN/MAX, fixed-point values around zero, and
all-minus-one. This is73728 cases/build, not exhaustive coordinate tuples or
every suppression byte. Native inputs are checked unchanged.

The retail test observes contributor count atBC550, full stored mask, stack
center/gap, memset arguments, and the geometry-entry SP/RA/arguments. Empty
cases additionally check restored s0-s7/fp/SP/RA. It does not execute any
geometry function. Native results are compared to these actual retail outputs.
Source controls replace arithmetic shifts with logical shifts, ignore the
suppression flag, visit only10 slots, or initialize true extrema. Each must
fail its designated count or center assertion. Matrix/projection/radius logic
afterBC6E0 and actual rendering remain unverified.

Final evidence scratchpad/battle-target-bounds.IKv9wl terminal0: all positive
O0/O2/UBSan runs, unchanged source baseline, two NULL API checks, and all4
native controls pass. HjgVJT was the earlier positive-only run. Ohm's
read-only review found no arithmetic/offset/oracle issue; reviewer did not
execute tests and was closed. The four datasets do not exhaust signed inputs,
and final quantization can hide intermediate differences beyond the tested
controls. No runtime source, native build registration or gameplay activation
was changed.

## Projected-point radius accumulator

`PcPortBattleAccumulateTargetRadius` now implements the BC7FC..BC858 integer
fragment for an already-projected packed screen point. Subtract(160,164),
multiply each displacement by4, retain its low16 bits and interpret those
bits as signed. Square and add with32-bit wrapping, then update the previous
maximum by signed comparison. No square root or projection is performed.
The equivalent second-point arithmetic atBC89C..BC8F8 is traced but is not
independently executed by this fragment test.

The fixture executes real bytes4CD0C..4CD6B (60 hex bytes), enteringBC7FC and
halting atBC85C after the branch delay-slot store. It supplies projected
halfwords and s5 as explicit arithmetic inputs, not planted gameplay state.
Every x halfword crosses y=0/FFFF/x+4 and four incoming maxima
(0,17,40000000,80000000):786432 cases per O0/O2/UBSan. This does not exhaust
all x/y pairs or all maxima. It checks resulting s5, SP, four scratch stores,
and low16-bit scaled coordinates. Direct witnesses cover origin, a one-pixel
offset, and both scaled offsets=-32768: their square sum80000000 is negative
under retail's signed comparison and must not replace maximum0.

Native controls change the maximum comparison to unsigned, remove signed
halfword interpretation, or change y origin to160. A retail instruction-copy
control changes SLT atBC84C toSLTU. All must fail the radius-parity assertion.
These controls do not change pinned retail files or execute geometry.

Remaining BC460 path: build rotation, read screen distance, transform a
short offset vector, callBB844, install rotation/translation, project each
eligible sprite and optionally its height-adjusted point when row+8 is
nonzero andC3688 is zero. SquareRoot0 follows accumulation. The radius<120
and scaled-distance branches then build another rotation/offset and publish
camera vectors. These geometry/library/state-write stages are not implemented
by either staged integer helper and are not validated by this fragment suite.

Final radius evidence3deoBQ terminal0:786432 cases each atO0/O2/UBSan and
unchanged source baseline, with all4 controls rejected. Bounds regression
8rSHaq terminal0 retains73728 cases/build and all4 bounds controls. James's
read-only review found no arithmetic/oracle/control defect; reviewer did not
run regressions and was closed. Neither staged helper is registered in the
native build or runtime dispatch. Full BC460 remains assembly.

## BB844 retail matrix builder recovered

`src/battle/mainc114.c` now implements BB844 in the retail branch only.
It normalizes target-minus-eye, crosses up with forward and normalizes the
right vector, then crosses forward with right and normalizes the vertical
vector. Matrix rows are right/vertical/forward. PushMatrix, ApplyMatrix on
the original eye pointer, wrapping negation of its three output words, and
PopMatrix follow. Matrix padding is untouched. Normal native builds retain
BB844 as ASM. The opt-in XENO_BATTLE_VIEW_MATRIX_STAGED build compiles the
same body for the native SDK comparison below; runtime admission is NOT_RUN.

`bash pc_port/tests/run_battle_view_matrix_match.sh` compiles the actual TU
with BattleCdk and literal-li maspsx, isolates the compiler-emitted function
without instruction edits, and requires all 400 bytes, size 0x190, address
0x800BB844 and global symbol to match pinned battle.bin offset 0x4BD54.
Evidence `scratchpad/battle-view-matrix-match.6MfW5I`: PASS 100/100.
Acceptance-predicate controls reject corrupted bytes, truncation and wrong
symbol address (using a retail-byte positive fixture). Initial absent-body
RED was ETkjg2; rsnAj7 exposed a test extraction bug that omitted `.globl`;
the runner now retains the compiler's declaration, not a fabricated symbol.

Neighbor exact gates CudP3o/GZNHAP retain BC2F0 66/66 and BC404 20/20.
Native BC404 regression YtmxUX passes 786432 cases per O0/O2/UBSan build
and unchanged baseline, with all three controls rejected; downstream calls
are still simulated. Full mixed-ASM TU manual compile rDwCED succeeds with
host cpp and configured CDK/maspsx, retaining existing missing-.end warnings.
This is not a normal Ninja/full overlay link or runtime validation. The TU
generator was not run or updated; regeneration must preserve this hand-edited
body and the existing setup recovery rather than overwrite them.

## BB844 native SDK composition gate

`bash pc_port/tests/run_battle_view_matrix_native_test.sh` builds the full
mainc114 TU with XENO_BATTLE_VIEW_MATRIX_STAGED, the existing production
func_80021B14 from animation_scripts.c, psyq_compat.c and actual PsyCross
LIBGTE/INLINE_C/GTE owners. No SDK calls are simulated. The other execution
runs retail BB844 and all its SDK callees from the pinned EXE through the
MIPS adapter, with no bridge. Both sides use the same PsyCross GTE backend:
this proves composition against retail instructions, not PS1 hardware accuracy.

Default builds check 77825 cases/build: 4096 correlated bounded vector fixtures
x three stack depths (0,1,19), plus all 65536 eye-X halfwords with fixed other
components, and an axis-aligned case. These use typed disjoint SDK objects.
Separate O2Alias/UBSanAlias builds explicitly use `-fno-strict-aliasing` for
all compiled owners and check 114689 cases/build, expanding the bounded
fixtures to four layouts (disjoint, matrix aliases eye, target, or up).
This alias evidence requires that compiler extension; it does not establish
overlapping SDK object validity under strict ISO C or production admission.
The suite compares the full guarded 128-byte input/output
region, GTE data/control registers, matrix-stack balance and all 640 saved
stack bytes. Retail callee-saved registers and SP/RA are checked. A separate
axis-aligned assertion requires identity rotation and translation (0,0,4096).
This is not exhaustive vector/alias/stack-depth coverage; stack overflow and
VectorNormal signed-overflow fault domains are not exercised here.

Evidence `scratchpad/battle-view-matrix-native.AdgtA0` terminal0 passes all
five builds and unchanged default baseline. Five source-copy controls (cross order,
wrong row, translation sign, missing PopMatrix, missing normalization) all
fail the designated comparison. Five independent observation controls corrupt
CP2D, CP2C, saved-stack bytes, a guard or input padding after native execution;
each must fail its specific assertion at case0. Initial link RED xiZpua proved no native
BB844 body before adding the explicit opt-in. The two unrelated legacy
TUs retain permissive compilation; new test/adapter/BB844 compile with
Wall/Wextra/Werror. No production source or vendor files are edited by the test.

Earlier pc7mZK/qodmyA/9zjUDe results are superseded: review caught typed
access through declared byte-array fixtures and insufficient isolated
observation controls. The typed-default/explicit-alias split and controls
above address these findings; UBSan alone does not prove effective-type safety.

Exact-match regressions fMQWz4/km0n52/OGJqxF retain BB844 100/100,
BC2F0 66/66, and BC404 20/20. No native build registration, dispatcher
changes, full BC460 composition, full overlay link or visible gameplay run.
Default-native BC404 regression PdjKUW terminal0 retains 786432 cases/build
at O0/O2/UBSan plus unchanged baseline and three rejected controls.

## BC460 rotation prerequisite: RotMatrixZYX repair

Tracing BC460 geometry exposed an unverified delegation in the already-native
`pc_port/src/retail_leaf_adapters.c`: RotMatrixZYX called PsyCross's sequential
RotMatrixX/Y/Z implementation. The new retail-execution gate reproduced a
real mismatch at angles (113,271,509): m[0][2] was1654 rather than1653 and
m[1][1] was3067 rather than3066 (RED `rotation-zyx-retail.puSedq`).

The native owner now follows scalar retail8004ABBC..8004AE48. It floors each
Q12 product separately, including sx*sy and sy*cx intermediates, then adds
or subtracts the separately rounded terms. The game trig shim's swapped
names are retained: rcos means sine and rsin means cosine. No GTE writes,
matrix translation/padding writes, or vendor modifications are introduced.
This changes an existing native leaf's arithmetic; it is not a new BC460
runtime dispatch registration. Natural gameplay acceptance remains NOT_RUN.

`bash pc_port/tests/run_rotation_zyx_retail_test.sh` compares original retail
instructions and trig-table reads against this production owner and actual
PsyCross trig functions. Evidence `scratchpad/rotation-zyx-retail.vilgzE`
terminal0:786433 cases/build O0/O2/UBSan plus unchanged baseline. Covers
every halfword angle on each axis with four fixed peer pairs, plus the
mixed-angle reproduction. Typed disjoint fixtures compare all80bytes,
return pointer, nine retail halfword stores, SP/RA/callee registers, and
unchanged GTE. Independent zero-angle identity is checked. This is not all
angle triples, input/output overlap, or full BC460 composition coverage.

Seven controls restore the old delegate, combine rounding, reverse a sine
sign, omit a store, write translation, alter GTE state or return the wrong
pointer. All fail their required observation. The legacy adapter test now
links real trig and checks identity instead of asserting delegation; its
O0/O2/UBSan suite passes35checks and retail slice pins. Halley independent
read-only review found no concrete issue in the disjoint-object contract;
reviewer did not rerun tests and was closed. Cross-model review skipped in
the autonomous continuation.

Broader `run_battle_gte_retail_test.sh` regression initially failed to link
because the fixture lacked the now-referenced PcPort_ServiceVblank symbol.
Added an explicitly inert fixture-only service boundary alongside its other
input stubs; production timing/input code was not changed. Final session75682
terminal0 passes O0/O2/UBSan SDK projection/matrix/normalization/square-root
checks and existing square/magnitude/normal controls. The shared GTE backend
and existing signed-overflow exception limitations remain in force.

## Complete staged BC460 computation and public writes

`PcPortBattleUpdateTargetCamera(memory, mask)` in
`pc_port/src/battle_target_camera.c/.h` uses the existing integer bounds and
radius helpers plus actual native RotMatrixZYX, ReadGeomScreen, ApplyMatrix,
BB844, SetRotMatrix/SetTransMatrix, RotTransPers and SquareRoot0 owners.
It publishes full mask C3678, distance C3CDC, and six halfwords at D30A0/A8.
Empty selections leave distance/camera outputs untouched. Height eligibility,
low-halfword wrapping, two small-branch screen reads, signed product wrapping
before the large-branch shift, and output-write widths/order follow retail.

All target handles stay in the caller's bus domain; none is masked or cast to
a host pointer. Inputs must be stable/passive during the call and must not
alias native locals or SDK backing storage. The two center scans use the
already-tested snapshot helper because no SDK call intervenes. This is not
an instruction-by-instruction memory-access tape or guest-frame replacement.
Bus failures return -1 immediately, retaining preceding public writes/GTE
effects; there is no fallback or replay. Native SDK overflow policy is unchanged.

`bash pc_port/tests/run_battle_target_camera_test.sh` runs complete retail
BC460, BB844, memset and all SDK instructions from pinned battle/EXE without
servicing any guest call (the bridge observes coverage only). Native executes
the real production SDK/helper owners. SquareRoot0 reads the original table
through the fixture's g_PsxRam. Both paths use the same PsyCross GTE backend;
this does not establish independent PS1 hardware accuracy or visible gameplay.

Final `scratchpad/battle-target-camera.NPktFF` terminal0 passes O0/O2/UBSan
and unchanged baseline:6183 cases/build, comprising all2048 low masks across
three geometry patterns, correlated suppression/null profiles and height gate,
30 screen-boundary cases, and nine near-threshold/halfword-height cases.
Census:2176 small-distance,3974 large-distance,33 empty;5793 second-point
projections. Nonzero initial GTE state and varying guest stack fill are used.
An independent coincident-point witness requires distance512, eye(0,0,-512)
and target(0,0,0). These inputs do not exhaust all vector tuples or SDK faults.

The test compares all2MiB RAM except the actual combined0x188-byte BC460/BB844
guest frame and the640-byte host-mapped SDK stack, which is compared separately.
It also checks stack balance and retail SP/RA/callee registers. CP2C and CP2D
are compared exactly except VZ0's unused upper half: retail loads unwritten
SVECTOR frame padding while native sets padding to zero. Initial Okd4x1/fJ1fLW
failed raw comparison for this reason. The final test records6099 raw-padding
differences and compares VZ0 through actual MFC2(1), retaining raw comparisons
for all other registers. INLINE_C.C sign-extends VZ0 on MFC2/SWC2; PsyX_GTE.cpp
VZ(n) and direct VZ0 consumers read only the signed low half. Raw backend-byte
identity is therefore NOT claimed, and no retail padding is planted in native
locals. A low-VZ0 corruption control must still fail its own assertion.

297 injected bus failures across empty/small/large baselines preserve the
exact preceding native public-write prefix, stop further bus access and retain
the GTE state at failure; nine invalid-API checks pass. This is native failure
behavior, not retail fault-instruction/frame equivalence. Seven source-copy
controls cover mask width, height gate/sign, eye Y sign, distance shift, camera
store width and radius threshold. Six isolated observation controls cover
camera state, SDK stack, CP2D, CP2C, low VZ0 and RAM below the excluded frame.
All fail designated assertions. Height-gate rejection is a GTE FIFO difference
even when final camera coordinates are unchanged.

Bounds regression UMajCi and radius regression RHPQ1O pass their O0/O2/UBSan,
baseline and controls. No existing production SDK/body or vendor file was
modified for this stage; new camera files and their explicit test build only.
Runtime resolver admission, guest-frame integration, full overlay/build and
natural gameplay verification remain pending; full BC460 retail TU stays ASM.

### BC460 real-resolver test boundary (2026-09-14)

`XBT_CAMERA_RUNTIME_RUN=1 bash pc_port/tests/run_battle_target_camera_test.sh`
now builds the same fixture with the actual `battle_mips_runtime.c` resolver.
Evidence `scratchpad/battle-target-camera.L5aHNm` exited0: 2217 cases per
O0/O2/UBSan and unchanged baseline, 297 native bus-failure prefixes and nine
invalid-API checks per build. Six target domains cover guest KSEG0, guest
KSEG1, validated low native pointers, shared g_GameState KSEG0/KSEG1 and mixed
handles. The mask census is sampled (step17), supplemented by the existing
39 screen/height boundary cases; it is not exhaustive across domains.

Host/shared input records are populated independently of the resolver and
checked unchanged after both executions. Raw RAM beneath shared data is
deliberately different. Exported battle-global host placeholders must remain
untouched; outputs resolve to guest RAM. Only the vector D_800D30A0 and distance
D_800C3CDC have map entries, so only those two test rejection of mapped host
bindings. D_800C3678 is absent from the map: its placeholder and SHADOW_WRITE
control detect accidental native writes, not mapped-binding rejection.
The initialized parent CPU and its
bridge pointer remain unchanged during the staged native call. This does not
invoke the production call bridge or prove guest-frame integration. Retail
still executes every SDK instruction without native-call bridging. The same
GTE backend and documented VZ0/guest-frame comparison limits apply.

All13 existing controls plus RAW_SHARED and SHADOW_WRITE are rejected at their
designated assertions. The flat-memory regression `gdF5Fn` also exited0 with
6183 cases/build and all13 controls. Source/ELF pins were rechecked unchanged.

Initial resolver run `y02hrT` reproduced a test-map defect: name-only inference
classified memchr at8003F918 as a0x150-byte data span, so instruction fetch at
memset8003FA08 read native libc bytes. The runner now requires the existing
`build/out/slus_006.64.elf` for function classification, as supported by the
production build's generator invocation; retail symbol maps remain address
authority, not ELF layout. No runtime resolver or generator code was changed.
Production's optional missing-ELF fallback is not validated by this test.

Arendt's independent read-only review identified the mask-binding coverage
overclaim above; the claim was narrowed rather than adding a synthetic symbol
to the real map. No other required findings were reported; review did not
rerun builds. Cross-model review was skipped in this autonomous continuation.

Runtime dispatch admission, guest-frame equivalence, full build/overlay and
natural gameplay remain pending. This is a resolver-backed staged computation
test, not approval to register BC460 in the production bridge.

### Full retail-C reconstruction experiment (2026-09-14)

Unadmitted candidate and reproducible scripts are in
`scratchpad/bc460-reconstruction.JG2WNF/`. Run `bash .../verify.sh` from the
repository root (replace `...` with that directory). This is legacy MIPS
compiler input, not portable native C: signed arithmetic and the SDK ABI are
judged by emitted machine code, not by a host C build.

The final identical source produces11/401 equal-address instruction words,
size674, with configured gcc-2.7.2-psx; CDK produces353/401, size640 versus
retail644. Both use literal-li assembly. This is evidence to investigate CDK
provenance, not permission to change BattleLiteral or generator admission.
The final candidate retains48 word mismatches and is explicitly NOT exact.
Remaining differences include min/max temporary allocation/store ordering,
radius sum register selection, small-branch scheduling, output-vector frame
locations and final target-pointer advancement. No assembly is injected into
the candidate to force the score.

Freshly rebuilt MIPS/GTE test dependencies execute both full machine-code
bodies against pinned retail EXE/SDK and BB844 without call bridging. The CDK
candidate and original-body identity baseline each pass6144 correlated mask/
geometry cases. A separately compiled wrong-mask-publication source mutant
fails case0's public-RAM comparison. These are O2 harness results, not an
O0/UBSan matrix or complete input coverage. Final source, binary, toolchain and
payload hashes are recorded in `final-pins.txt`; `verify.sh` exited0.

RAM comparison includes SDK stack and all public memory except the intentionally
replaced0x800-byte code window and the existing0x188-byte combined guest frame.
Architectural GTE comparison excludes only unused VZ0 high padding via actual
MFC2. No raw guest-frame or caller-saved-register equivalence is established.
The original `src/battle/mainl115.c` still uses INCLUDE_ASM for BC460; no build
preset, generator, runtime registration or production source changed here.

### BC460 reconstruction layout refinement and strict differential

Latest scratch-only candidate: `scratchpad/bc460-layout.krWs9l/candidate.c`.
Its `verify.sh` exited0 (session45805): CDK395/401 words with exact size644;
configured compiler11/401 with size674. The old JG2WNF baseline remains intact.
Six mismatches remain at BC848/84C/858 and BC8E8/8EC/8F8: the candidate uses
a2 for each radius sum where retail uses a1. No inline assembly/register
binding or post-assembly word replacement was used. No production toolchain
or generator admission change is justified yet.

The candidate now models observed frame reuse with a union at60: projected
SVECTOR at60 and screen at68 are subsequently replaced by the small-branch
VECTOR result. The large branch writes the enclosing result atA0. An unused
legacy local models D8–E7 reservation before the final SVECTOR atE8. This
models emitted layout, not proof of original source declarations. Jason's
read-only layout review found no required issues; no invalid live union reads
or out-of-frame accesses were identified.

Unlike the prior reconstruction differential, this compiled-MIPS comparison
now includes the entire guest frame, raw GTE state (including VZ0 high padding)
and all final GPR/HI/LO values. It still excludes the intentionally replaced
0x800-byte instruction window. Identity and candidate each pass6183 cases:
6144 correlated geometry/mask cases plus39 screen/height boundaries. The
wrong-mask source mutant and isolated frame-byte, VZ0-high and a1-register
corruptions all fail designated assertions. Source/payload/compiler pins were
rechecked unchanged. These are O2 harness results, not fault-instruction,
pipeline/cycle, independent hardware or whole-game equivalence.

This stronger frame result belongs ONLY to the compiled legacy-C candidate.
The separate staged native `PcPortBattleUpdateTargetCamera` retains its
documented frame and raw-VZ0 exclusions; its runtime admission is unchanged.
Exploratory sweep variants are not admitted or individually verified; use the
final candidate and its pins. Exact401/401 still required before replacement.

### BC460 exact under CDK; production replacement still gated (2026-09-14)

The401/401 requirement above is now met in scratch. The radius-shape sweep's
winning source shape is applied to `scratchpad/bc460-layout.krWs9l/candidate.c`
(sha256 `b42e05f2…`): each fused radius sum becomes a block-scoped running local
`{ int squared=vx*vx; squared+=vy*vy; if(maximum<squared) maximum=squared; }`,
which makes cc1 keep the sum in the a1 chain (retail `addu a1,a1,t0` /
`slt v0,s5,a1`). The equivalent `u32 squared` form emits identical bytes; the
signed form is retained. CDK is now 401/401, size644 exact; `candidate.bin`
sha256 `4549ebf7a618d484078505b6dd7d5d925e37c579ff7b529778bce077a6814996` is
byte-identical to `disc/battle.bin[0x4C970:0x4CDB4]`. Configured gcc-2.7.2-psx
remains11/401 size674, so this body is CDK-provenance only.

`verify.sh` exited0: identity and compiled CDK candidate each pass6183 cases
with the full guest frame, raw GTE and final GPR/HI/LO; the wrong-mask source
mutant and observation controls1/2/3 (public RAM / raw GTE / final registers,
including $a1) are rejected. `final-pins.txt` was regenerated; other pins are
unchanged.

Replacement is now INSTALLED (2026-09-14, user-authorized) as a hand-written
`BattleCdk` TU, not a generator regeneration: the vendored
`tools/scripts/gen_battle_tus.py` is stale and, run in a throwaway mirror,
rewrites 24 hand-edited files (`mainc114.c` port `#ifdef`s, `main14`…`main44`,
`main72/73/125`), so it was not used. `src/battle/mainc115.c` holds the body
(path `battle/mainc` → BattleCdk); `src/battle/mainl115.c` lost its BC460
`INCLUDE_ASM`; `config/battle.yaml` maps `[0x4C970, c, mainc115]` +
`[0x4CFB4, c, mainl115]`; `pc_port/build_port.sh` lists `mainc115.c` as
reference-only. Verified with the real preset: `build/src/battle/mainc115.c.o`
linked at the pinned retail symbol addresses is byte-identical to
`disc/battle.bin[0x4C970:0x4CFB4]` (`4549ebf7…`, `0x644` bytes), and
`mainl115.o` to `disc/battle.bin[0x4CFB4:0x4D00C]` (`80232ffa…`).

The end-to-end ROM gate is still blocked by a PRE-EXISTING, unrelated failure:
a from-clean `make build` fails at `build/out/battle.elf` with89 unresolved
local jump-table labels from `build/asm/battle/data/0.rodata.s.o`
(`.L80079F00…`), owned by landed C functions whose retained retail jump tables
reference `.L` labels the C body no longer defines (e.g.
`main14/func_80079ED8`). A baseline revert of this install reproduces the
identical89 references; BC460 has no jump table and adds none. Do not force the
link by deleting relocations or substituting addresses. `disc/battle.bin`
(retail) remains the unchanged authority, and the staged native
`PcPortBattleUpdateTargetCamera` admission is a separate, unchanged track.
