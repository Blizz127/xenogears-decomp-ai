> **PROJECT GOAL:** a native PC port in the mould of Ship of Harkinian /
> the Silent Hill decomp ports, ultimately re-rendered (HD-2D).
> **Read `docs/ai_context/PORT_GOAL_AND_PLAN.md` before planning any work.**
> C coverage of executed code paths is the critical path; byte parity is a
> quality gate, not the deliverable. An `INCLUDE_ASM` body is worth nothing to
> the port. Anything still running through `battle_mips_runtime.c` can never be
> restyled.

Latest completed checkpoint (2026-09-06 UTC): MAP16 (OPEN_ISSUES item 8, the
"Blackmoon Forest renders mostly black" blocker) is RESOLVED, and its recorded
cause was wrong. MAP16 is not a broken forest: it is a scripted cutscene — fixed
camera behind two foliage fragments, four archive-6B9 objects (a Gear flight)
lerped in one after another, then a warp to field 15; between passes the scene is
authored-dark. Objects were loading all along (65k draw packets); what was broken
was the object DRAW path. The port's approximate func_801E7D14 omitted
SetColorMatrix(D_800B223C) and per-node SetLightMatrix, composed w2s x node.local
instead of rootView x node.world, ignored scale and used walker variant 0 instead
of 1 — and this map's colour matrix is all zeros, so only the (30,30,30) back
colour survived. Two further bugs: misc2.c FieldUpdateObjectActor read the scale
table at retail 0x800B21DC (the spriteId<<1 table; the blob starts at
0x800B2078), scaling objects 7.0x; and main.c declared D_801E8670 as void*[]
(8-byte host stride) so the scale table was never written for slots 1..3.

Fixed by retail transcription (six-pass func_801E7D14 704B, new func_801DCEC8
3376B / func_801E0398 768B / five-arg func_801E36BC 276B, func_801E5D44, plus a
new pc_port/src/model_prim_ed20.c carrying the variant-1 lit small-tri walker
func_8002ED20 wired as D_8004FE50[0].proc[1]). OBSERVED, verified by the
coordinator independently of the implementing agent: map16 now shows three lit,
textured, correctly-scaled Gears passing the camera, peak frame 372 at 35.2%
non-black / 18.3% bright (was 0.0-12.2% with a single fragment). LINK OK, 74
stubs. Regression maps 17/1/2/15 rc=124, zero faults, zero missing-D_8004FE50.
Test run_object_draw_retail_test.sh: 1536 cases O0/O2/UBSan, 7 mutants rejected.
Evidence: docs/evidence/blackmoon-map16-render-20260906/.

OPEN_ISSUES item 8 should be REWRITTEN (its loader-stub cause is obsolete) or
closed; MAP3 still needs its own run before the shared-root claim is retired.

Also landed this session, all with regression certificates and no commit:
D_8004FE50 rows 0x02/0x06/0x07/0x0E filled from retail .sdata, taking the table
from 7 absent (zero-filled, NULL buildProc — the Map 2 OT-corruption class) to 3;
the encounter weight table D_80065ADC re-aliased onto D_800658DC+0x200, which had
never been written, so NO random encounter could fire anywhere in the port;
func_800A1364 byte-exact; four field functions byte-exact plus a real port bug
(func_8007234C was declared/called with zero arguments and only worked by
accident of register liveness). The flagged slus/field hash drift is PROVEN to be
a splat/spimdisasm VERSION artifact, not source: pinned 0.33.2/1.33.0 reproduces
slus 5c674b3f exactly. Every recorded slus/field hash is generator-dependent;
state the generator version alongside any future baseline.

BUILD/RUN ENVIRONMENT CORRECTION: earlier checkpoints claiming the port cannot be
built or run here are WRONG. The environment is a podman IMAGE, not a distrobox
container (distrobox list is empty, which is what misled us):
  podman run --rm --security-opt label=disable --userns=keep-id \
    -v /var/home/blizz/Projects/xenogears-decomp-ai:/home/blizz/Projects/xenogears-decomp \
    -w /home/blizz/Projects/xenogears-decomp \
    localhost/xenogears-dev-toolchain:current bash -lc './pc_port/build_port.sh'
--userns=keep-id is required; the mount path drops the -ai suffix, which also
satisfies tools/gears' hard-coded directory-name check so make check runs against
the live tree with no copy. The image ships real mips-linux-gnu-cpp, so the old
cpp wrapper is unnecessary; add pinned splat in a venv for the matching build.
Run headless with SDL_VIDEODRIVER=offscreen. Set TMPDIR=/var/tmp: /tmp is a
12.3G-quota tmpfs and under exhaustion cc1 silently emits objects with NO
function bodies while exiting 0.

HARNESS TRAPS that produced two wrong conclusions this session: XENO_FIELD_MAP=N
does NOT pin the map (map16 warps to 15 near the end) — split logs on
"FieldLoad begin field=" before attributing any metric or frame, and cross-check
the drained archive against (mapNum<<1)+0xB9. "model-build count=N" is a RUNNING
total. Capture files named field-frame-*.png are actually BMP.

Boot chain verified by capture: FMV decodes (SquareSoft logo), Map 490 title logo
renders 93.3% non-black (an earlier note had it at 1.9%), Map 4 prologue
narration types correctly. The opening path shows ZERO obj-ovly activity, so the
Gear draw fix does not apply there.

Latest completed checkpoint (2026-09-06 UTC): func_800A1364 (field object-register
opcode, OPEN_ISSUES item 8's root) is now BYTE-EXACT and INSTALLED in
src/field/main/misc6.c for both the matching build and the port (the
XENO_FIELD_OBJECT_OVERLAY #ifdef/INCLUDE_ASM staging is gone; build_port.sh's -D
is now vestigial). Actual retail cpp/cc1/maspsx pipeline: all 99 instructions /
396 bytes / 0x30 frame, opcodes+relocs identical to the INCLUDE_ASM control with
and without the old flag; relinked with its 10 dependencies pinned it is
byte-identical to disc/field.bin[0x31874:0x31A00], SHA-256
abf8efc25897d54dc9dd9b7fa2d38ed2e464836d332eb3cbb9b506e598b7be98. The previous
port-only body was 89 instructions / 0x28 frame and had never been compiled by
the matching build. Load-bearing shapes (30 variants): pointer-arithmetic slot
stores, one volatile counter read for the stamp, IP/flags via the global
pointer with g_FieldActors[D_800AFD1C] direct, unused s32 pad[2], byte-neutral
(u32) on the id shift. Root differential test
pc_port/tests/run_field_object_register_retail_test.sh: O0/O2/UBSan 1440 cases /
57702 checks / all 99 slots each, 7 controls rejected; spies only, not the VM,
binder or pose reset. Whole field: the function moves to its retail-relative
slot, field.bin f49b4fd02316ddd6a1bb568440ff7dff99082d5a13c04947c267ada0bbf2d854
at 242622 (was ececa463; masked per-function compare of 1054 functions: 0
differing, 60 moved), slus 3192a514 / member 3b9e2b89 OK / shop 770921de / menu
9fc9b811 unchanged, gate still red on slus/field/shop.

The flagged slus/field drift is PROVEN and REPRODUCED as a generator-version
effect, not source: in the podman image localhost/xenogears-dev-toolchain:current
(/.venv = splat 0.33.2 / spimdisasm 1.33.0, the requirements.txt pins) the 12:15
sources give slus 5c674b3f / field f72b2245 (the 02:37 shop-resources record);
the host's splat 0.41.1 / spimdisasm 1.42.2 gives 3192a514 / ececa463. First
moved symbols: slus g_CurGameStateOverlayID (.main_bss start 0x80058d10 ->
0x80058d04; retail is buffer+4, so the newer generator is right there), field
D_800AF5F0 (.field_bss start 0x800aaeb0 -> 0x800aaeac; retail is data-end+4, so
the older generator is right there). Every recorded slus/field hash depends on
the generator; state it in future baselines.

BUILD-VERIFIED: ./pc_port/build_port.sh in the podman image with the final
source -> LINK OK, 74 function stubs, xeno-port
522e901c0789888bb8b7e11a931b47bd30e2d265dc59917a04065d64baca4e5e (4805016 bytes),
NOT_RUN. The host also links it with Homebrew SDL2/OpenAL/GL
(PKG_CONFIG_PATH=/home/linuxbrew/.linuxbrew/lib/pkgconfig
CMAKE_PREFIX_PATH=/home/linuxbrew/.linuxbrew). Runtime: the coordinator's MAP16
observation (docs/evidence/blackmoon-map16-runtime-20260906) is on the
preceding binary 591ce91e, not on this one; no render claim from this pass.
OPEN_ISSUES.md untouched (item 8's loader-stub cause is closed by their
observation plus this proof; MAP3 needs its own run). field_object_overlay.c
owners and func_800821F4's object branch remain build-verified only.

ENVIRONMENT CORRECTIONS: the build container is a podman IMAGE
(localhost/xenogears-dev-toolchain:current; distrobox list is empty), so earlier
"cannot build/run here" notes were wrong. /tmp is a 16G tmpfs with a 12610M
per-user quota that a concurrent agent's tree filled (EDQUOT killed every shell
for a while); this pass's scratch is /var/tmp/xeno-blackmoon-verify-mT7beL and
compiler temporaries go to TMPDIR=/var/home/blizz/.cache/xeno-tmp. tools/gears
still needs a "xenogears-decomp" path component (scratch copy or container
mount). Evidence: docs/evidence/blackmoon-object-overlay-verification-20260906/.
No commit, stage or push.

Latest completed checkpoint (2026-09-06 UTC): shop string UV/tpage rebuild
func_801C5A7C is now INSTALLED, replacing an INCLUDE_ASM placeholder. The actual
production shop TU compiled with the authoritative GCC2.6 preset emits all576
retailbytes /144instructions for801C5A7C..801C5CBC, SHA-256
d71c52a1f841c5be08425bba883a28d3f2ac47acccc4ee1062c84316f1bd3e09. Shop function
ordering is still unresolved, so the overlay places this body at801C9738; the
production object was relinked onto the retail address with only its6 real
dependencies pinned. Body claim, not overlay layout.

Two source-shape facts were load-bearing and are documented: blend is u16 (an
s32 accumulator emits `or v0,v0,s1` instead of retail `or v0,s1,v0`, and source
operand order cannot fix it because GCC evaluates the GetTPage call first and
swaps commutative operands itself); and the u2 corner is fed by a mid-block
`u = (half & 1) * 128;` assignment while the other three corners spell the
expression out, which is what leaves retail's `move v0,s3` loop-invariant copy.
Neither changes behaviour. This supersedes the13 non-exact candidates in
/tmp/xeno-shop-texture-pair-c-20260906, whose best was4bytes short.

Full make build completes472tasks with0failures; the unchanged retail checksum
gate still fails the same3 (SLUS, field, shop). A control build with the
original INCLUDE_ASM restored is byte-identical for slus_006.64, field.bin,
member_change_menu.bin and menu.bin, and its shop hash06700c87 reproduces the
preceding checkpoint's recorded shop baseline, so the environment is faithful.
Only shop_menu.bin changes,55296bytes,770921ded3bb1232d63df2589bd74bf264f3ac055581d78747ce0df2b58f4641.
Shop inline assembly bodies drop15->14. Evidence:
docs/evidence/shop-string-uv-20260906/README.md.

FLAGGED, not addressed: slus_006.64 and field.bin hash differently from
docs/evidence/shop-resources-20260906/matching-after.json at identical sizes.
They are byte-identical across the control and this change, so the drift comes
from other uncommitted field/slus edits already in the tree.

BUILD ENVIRONMENT (this checkout): tools/gears hard-codes the project directory
name "xenogears-decomp" (tools/gears/src/file_system.rs find_base_path), so
`make build` at this repo root fails with "Could not locate project base path"
because the checkout is named xenogears-decomp-ai. Verified builds used a copy
at /tmp/xeno-shop-uv-build/xenogears-decomp. There is also no mips-linux-gnu-cpp
on PATH; a wrapper over the system cpp that drops the pre-GCC3 -lang-c spelling
reproduced the recorded member-change-menu and shop baselines exactly. Docker is
absent (podman only), and pc_port/build_port.sh needs the xenogears-dev
container, which does not exist here.

NOT_RUN at this checkpoint: native pc_port link, any shop UI rendering, any
runtime observation, and any focused native differential regression test for
this body. pc_port/build_native/xeno-port (7739b2b7...) predates this change.

Live route run-y9dtjd02 is TERMINAL: it stopped at07:53:27UTC on its own
2700-second bound, exit.json records gdb_returncode0, first_battle_returned
true, a9_return_observed false, and the final SIGTERM stack is in FieldMain via
Vsync. It reached the map15 mountain area after the village exit. All3 owned
PIDs are absent and no owned live process remains. Do not treat earlier
"live run" text below as current.

No commit, stage or push. The full boot-to-forest and whole-game decomp goal
stays active.

Latest completed checkpoint (2026-09-06 UTC): ShopMenuLoadResources C is now
INSTALLED, replacing a native stub. Actual fullTU GCC2.6 body matches1088retail
bytes; all16 D801C5000 data bytes preserved, including actualglobalprefix.
Root128cases/11944checks/all272instructions passO0/O2/all-ClangUBSan plus7
rejectedcontrols. Helpers are spies, not actualshopUI/decompression/GPU/audio.
Private global472tasks still3checksumfailures; shop55296bytes, otherchecked
modulehashes unchanged. 15shopassemblybodies remain. Native LINK OK77function
stubs; binary7739b2b794a430eee899aade8be196e0c8b2d804cbce5d3bb451887b82872da6,
NOT_RUN. Includes priorlookup and archivepointer repairs. Evidence:
docs/evidence/shop-resources-20260906/README.md.

Live naturalroute at07:38UTC remains run-y9dtjd02, copiedcda8623b,
Xvfb528340/GDB528348/native528358, display:1. Additional1800seconds accepted
07:34:29 (total2700 from07:08:27). Sol sole controlowner; no restart.
Dan/Timothy dialogue completed naturally;13->14stairs->13 transitions work.
Natural village exit achieved07:39:31UTC with ordinary z in narrow doorway
atobservedfield13x181,z4. Root inspected nav64 PNG and actualFieldLoad1log769.
Earlierx116triggerzone was approach geometry, not interactionpoint; noexitbug.
SAVE WAIT/noquick.xgqs; read-only failedgate D80059179=1 remains. Sol continues
ordinaryvillage/story route. Evidence field-axis-matrix-20260906/live-route-village.json.
No forest/laterbattlecompletion claim.

Latest completed dependency checkpoint (2026-09-06 UTC): ArchiveReadFileToBuffer
now accepts void* destination rather than s32. Actual oldsource reproduced
high-host-pointer truncation; corrected fullTU passes7focusedcases in O0/O2/
mixedUBSan and recreatedoldtype control fails. Entire GCC2.6 TU assembly and
all4checkedglobalmodulebytes unchanged. Native LINK OK binary:
1b82f340a37889e691983cf3c9f61ee43eaeb524102c3882bbcd2bfadbfe435c.
Build-only; live run-y9dtjd02 still uses its original copiedcda8623b binary.
Evidence docs/evidence/archive-buffer-pointer-20260906/README.md.
Shop resource loader remains scratch-only pending final root integration;
Astra proved authoritative GCC2.6 exact1088bytes and native128cases/all272slots.

Latest completed source checkpoint (2026-09-06 UTC): shop byte lookup
func801CE8D8 now matches all68 retail bytes from the full production TU.
Root independently passed65cases/297checks/all17instructions in O0/O2/UBSan
and rejected4controls. Signed end comparison uses native-width intptr_t;
matching-only types fallback added. Private make check completes472tasks,
still3checksumfailures; shop now55296bytes, othercheckedmodulehashes unchanged.
Native build53c04117da90ed9860d2f6fd5f0a9f76d3898732ba8edebcdd916c0341dab49d
links but is NOT_RUN. Evidence: docs/evidence/shop-byte-lookup-20260906/README.md.
Shop resource loader has an exact1088-byte scratch candidate under Astra;
full native fixture/integration remains pending, no shared loader edits yet.

Live route run-y9dtjd02 retains copiedcda8623b binary. Opening battle completed
07:13:34UTC, returnedfield14, then ordinaryinput reachedfield13 (FieldLoad log415).
Sol owns runtime/navigation; Luna is inspecting retail field13 exit scripts.
SAVE WAIT remains pending, not an accepted checkpoint. Do not restart live run.

Live natural traversal (2026-09-06 07:10UTC): canonical run is
/tmp/xeno-forest-route-20260906/run-y9dtjd02, NOT the erroneous bdo9mdzu message.
Root verified /proc/argv and run.json agree: Xvfb528340, GDB528348, native528358,
display:1, window6291508. Copied binarycda8623b matches finalbuildmanifest.
Focused ordinary Circle reachedtitle; Up/Circle choseNewGame2; log nowreaches
field4. Earlier body fixes are tested/built; noforestallbattleclaim.
Sol owns ongoingcontrol/observation; sourcewriters finished. Live extension
file is run-y9dtjd02/extend-seconds (totaladditionalseconds frominitial900).
Currentlive driver's movie-skip condition still waits for a mainloop marker
that occurs after initialization; manualfocusedCircle correctly advanced the
sameprocess. Sol is correctingfuturetooling; do notrestartthisliverun.
Use current-run.txt and process/exit state asauthority. Root inspected genuine
field490moviePNG; remainingroute screenshots/entry events must be observed.

Latest completed source checkpoint (2026-09-06 UTC): missing field axis matrix
routine func800759E4 is now INSTALLED, superseding the scratch-only note below.
ActualfullTU C body matches292retailbytes, native seed matches16retailbytes;
root200cases/21078checks/all73instructions passO0/O2/all-ClangUBSan plus4controls.
SDK math helpers are spies in this test; realcaller/rotatedactor runtime is
NOT_OBSERVED. Correction: the rotatedactor assert is matching-only; native
already uses FieldRenderActorSpriteTail. No directfieldcaller of759E4 found;
similar matrixwork exists inline in764B4. Standalonebody is a decompcompletion
repair, not a newly enablednativeactorpath. Wholefield gains292
bytes but remainsnonmatching (242622bytes); othermodulehashes unchanged from
menu-frame check. Global472tasks/3checksumfailures. Nativebuild SHA-256:
cda8623b38a192018181a232875910308f213c9d8778e8798651c7f6c34461ae.
Evidence: docs/evidence/field-axis-matrix-20260906/README.md.

Previous naturalroute run-rgg6p30b TERMINAL at07:01:14UTC, all3ownedPIDs absent,
applicationlog restored. Reachedfield4only; nofirstbattle/forest/A9 observed.
Its focuswait was circular; driver now discovers/focusesSDLbeforewaitingtitle,
uses normalCircleinput, supports extend-seconds, disablesperiodicBMPcapture,
and isolatesquicksavepath. Sol owns corrected driver and nextnaturaltraversal
under /tmp/xeno-forest-route-20260906; consult current-run.txt, run.json and
actualprocess/exit state beforeinterfering. Do not restart a confirmedlive run.
Astra field and Luna shop/review agents completed; no other sourcewriters.
Fullgoal remainsactive and incomplete; no commits/stage/push.

Latest continuation (2026-09-06 UTC): system menu frame repaired with exact C.
This supersedes the final-build pin below; prior runtime reports stay tied to
their recorded binaries. Current build SHA-256:
4ab179081703f3808e21a4414ce49dab23944d6b54808e6ab328c2daa22bbcb6.

func_8001C074 uses typed host graphics environments, alternates both buffers,
and preserves retail debug/pointer reload order. All308 retail bytes match
from the actual compiled production TU. Native300cases/6020checks/77instructions
pass O0/O2/mixedUBSan; four semantic controls rejected, actual oldsource red.
The ordinary debug-index RAM qualifier is scoped correctly: volatile accesses
remain only in input. Full system/member/shop emitted instructions preserve
prior scheduling; member fullmodule remains exact and field/shop hashes stay
unchanged after472-task private makecheck (three checksum failures remain).
Evidence: docs/evidence/system-menu-frame-20260906/README.md.
Actual execution of this repaired menu frame is NOT_OBSERVED.

Work in flight at this checkpoint: Sol owns natural route run
/tmp/xeno-forest-route-20260906/run-rgg6p30b (copied preceding1c9e2c73 binary,
Xvfb402539/GDB402618/native402637, display:2; original900s bound).
Verify actual process/exit handles before resuming; do not infer terminal state
from the bound. No later-battle or forest completion claim. Astra owns scratch
/tmp/xeno-field-matching-audit-20260906: missing func800759E4 has an exact292-byte
candidate; full-TU/native validation is in progress, NOT installed. Field text
shortfall is spread across translated functions; do not patch jump-table data
to disguise relocated targets. Luna's shop audit is /tmp/xeno-shop-matching-audit-20260906:
16 assembly bodies are hoisted before C; a small68-byte function remains
nonmatching. Prefer actual decompilation to hiding this behind layout changes.

No commits/staging/push; full boot-to-forest/game decomp goal stays active.
Revalidate live agents and source hashes before further edits.

Latest verified continuation (2026-09-06 UTC): native file-1 controller adoption
and complete character-change menu matching are installed. This checkpoint
supersedes older pending-adoption and four-checksum statements below.

The ordinary opening executed 670 paired native controller calls with verified
module identity and immutable-code hashes, displayed Fei dialogue, and returned
to FieldMain/map14. Observed binary: f59f7f87cb2b611d40441e54e973c7a295c4978f839920f31c0455b9b360c205.
Actual controller adoption regression passes 473 cases per O0/O2/Clang UBSan
mode, 4,319 assertions and all 10 compiled controls. Remaining helper bodies
still execute through the guest/resident bridges; this is not full C decomp.
Evidence: docs/evidence/opening-battle-encounter-20260905/
file1-native-adoption-integration-20260906.md.

All 26,624 bytes of member_change_menu.bin now match retail, SHA-256
3b9e2b890c27ae0de97fe343ac75cd35c05fe7f9605ae387d2166da0be78109c.
Four C repairs restore the swap function, label drawer/initializer, and name
renderer. Isolated make check builds all 472 tasks; only SLUS, field and shop
menu checksum gates still fail. Actual native swap, label and name functions
have focused retail-instruction differential tests and negative controls.
Evidence: docs/evidence/member-change-menu-20260906/README.md.

Final native rebuild links, SHA-256
1c9e2c73427c0ed20177b5db49cdd62ec488cc03b717269c68f57ac3b3e5b215.
This final binary includes the later name-renderer repair; its separate build
pins do not claim a repeat gameplay observation. The opening was observed on
the preceding f59f7f87 build. The RECORD/F9 audio fix remains included; its
paired real-game capture evidence is in docs/evidence/recording-audio-20260906.
Owned opening game/debugger/Xvfb processes ended and the application log was
restored. Recheck source writers and live processes before starting new work.

The full goal remains active: boot through the requested forest, later-battle
A9 observation, remaining systems and full retail byte parity are incomplete.
No flags/positions/story state were forced during the opening observation.
Use Luna for routine work, Sol for moderate integration/runtime tasks, and
Astra for difficult decompilation or uncertain retail semantics.
No commit, stage or push was performed. Earlier checkpoints follow.

Latest user priority resolved (2026-09-06 UTC): RECORD/F9 audio silence was
caused by replacing Pulse sample timestamps with packet-read wall-clock time.
Only the audio override was removed; raw video keeps wall-clock timestamps.
Native build bd9b23362e894ba56a4852671235f9bf016437bd9473e8ce2ca0e9968732e5ac
links. Fresh real-game recording matches direct-monitor audio activity
77/152 bins exactly, RMS ratio0.969. Root functional tests pass30/150Hz
presentation with38/38 and39/39 active bins; old source fails3/38 with a
valid continuous reference. Source/fixture/runner pins and owned cleanup
are checked. All owned native/encoder/reference/Xvfb processes ended.
Evidence: docs/evidence/recording-audio-20260906/README.md.

The prior A9 gameplay observation also ended: opening returned and ordinary
play reached field13 with dialogue progression; field1/world/laterbattle/A9
were not observed before its900s stop. See the dated A9 observation report.
Full decomp goal remains active. Next controller adoption gates are module
identity/invalidation, packed RAM bindings/resident ABI, and actual adopted-
path differential; the guest-call prerequisite is installed and tested.
No active source writers remain at this checkpoint; recheck before edits.
Use Luna for routine work, Sol for moderate integration/runtime tasks, and
Astra for difficult decompilation or uncertain retail semantics, escalating
only when needed and returning routine follow-up to cheaper agents.

Latest verified continuation (2026-09-06 UTC): the file-1 controller C now
matches all 1,260 retail bytes. The full 19,516-byte module containing this C
function and remaining assembly also matches exactly. Root's 463-case
O0/O2/UBSan regression and all 11 controls pass. The PSX target uses the C
body; native controller adoption remains disabled pending module identity,
RAM bindings and guest reentry. See the dated file1-controller exact report.

The later-battle A9 sprite command is implemented. Root verified 4,608 cases
per mode, all five controls, the actual native outer interpreter's two-byte
advance and the actual scale helper. The repaired native build links, SHA
f4dcc43c404d453861429e80a017a5c4bda2ff355f85829d359276c274743ec6.
Gameplay after A9 is being observed separately; focused tests are not a
later-battle completion claim.

Matching-only jump-label visibility and the text interpreter's invented
three-byte offset table are corrected. Isolated make check now completes
all 472 build tasks, then fails the unchanged retail checksums for SLUS,
field, member-change menu and shop menu. Full byte parity remains incomplete.
Text-table O0/O2 and mixed GCC/Clang-runtime UBSan checks pass; the standard
GCC sanitizer link lacks its installed runtime and exits 1.

Evidence: docs/evidence/opening-battle-encounter-20260905/
  file1-controller-exact-integration-20260906.md
  animation-a9-integration-20260906.md
  text-table-integration-20260906.md
  global-matching-after-table-fix-20260906.md

The guest-call service prerequisite is now installed and independently
verified: 202 assertions per O0/O2/all-Clang UBSan mode and six semantic
controls pass. Rebuilt binary1738d43d... links. There are no production
service callers and no controller substitution yet. See the dated
guest-call-service-integration report. Sol owns natural-play observation
on copied prior binaryf4dcc43c...; Luna diagnoses matching layout separately.
Recheck live process/agent ownership before acting. The older reports below
are historical; their 302/315 controller and linker-blocker status is superseded.
The route through the forest and full-game decompilation remain open.

# Active Handoff

Latest constructor continuation (2026-09-06 UTC): retail constructor repair
is installed in system.c (SHA b75fb09b...). Both durable constructor and glyph
retail-instruction regressions pass O0/O2/ClangUBSan with all controls rejected.
The native build ed9987ed... links. Virtual-display replay
pc_port/build_native/opening-constructor-virtual-jwlhtzig visibly renders Fei
battle dialogue and returns directly to the painting room, with result1/map14,
computed state1 and FieldMain entry at04:05:48UTC. The owned monitor stopped
native/debugger/Xvfb after return captures. Source/build/runtime/limits:
docs/evidence/opening-battle-encounter-20260905/constructor-retail-repair-20260906.md
and constructor-after-runtime-20260906.json. Full retail pixel parity and
complete matching/decomp remain open.

The reviewed file1 controller C draft is preserved in
scratchpad/decomp-candidates/battle-command-file1-20260906. Structural/helper
boundary tests pass458 cases, independent review463 cases with all315
instructions and both outcomes of21 branches covered. Corrected baseline exact text match FAIL
(1248 vs1260 bytes at801E6CE8), native adoption NOT_RUN. The old1256-byte
comparison included leading alignment NOPs. A durable463-case regression
now lives in pc_port/tests; see the dated file1-controller review correction.
Next source-ownership work must keep this module distinct from other payloads
sharing801E5000 and the resident constructor's native8arg/retail7arg interfaces.

Latest verified continuation (2026-09-06 UTC):

The normal opening now displays Fei's battle dialogue and returns directly
to the painting room. The two current repairs are the battle-result dispatcher
and the seven-to-eight-argument dialogue constructor bridge. A sparse runtime
trace observes guest result1/map14, next state1, and actual FieldMain entry;
readable battle and painting-room dialogue were captured. Full retail pixel
parity and complete matching remain open.

Read `docs/evidence/opening-battle-encounter-20260905/README.md`, especially
`window-bridge-repair-20260906.md`, `battle-return-state-repair-20260906.md`,
and `window-and-return-runtime-20260906.json`. Earlier0x17 pilot-text ownership
was disproven: that atlas contains attack/action assets. The real text caller
is dynamically loaded archive(0x20,0)/file1 at801E6FC0.

Focused installed-source tests pass:384 retail return-state cases per mode
and nine controls; the constructor bridge ABI matrix and three controls.
The native build links. Host-only logging now has XENO_PC_PORT guards in
animation_scripts.c and temp1.c, preserving all hard stops. The isolated
matching check gets past the stderr errors but still fails at resident linking
on unresolved jump-table labels (72 linker diagnostic lines). See matching-telemetry-20260906.md.

Earlier desktop replay: `pc_port/build_native/opening-window-fixed-1-bsbals5x`.
The verified opening completed. Continued play reached the world map and
another battle, then aborted on sprite command0xA9/index31, sprite00731F04,
operands0071E4AE. Native3572318/debugger are no longer live. Root did not
request this stop. The opcode is a later-play decomp blocker, not a failure
of the already captured opening transition. Recording had been active;
preserved file is recordings/xenogears-20260905-221812.partial.mp4.
ffprobe reads video/audio and387.467s; full playback is not verified.
Prior retail PCSX PID3106417 is also no longer live; its exit cause was not
observed. Recheck ownership before any launch or UI action.

Retail input follow-up: physical Cross confirms after retail ControllerInit
swaps face-button mappings; Circle cancels. See retail-input-correction-20260906.md.
Fresh owned PCSX3677470 reached the opening movie from selected New Game,
then exited cleanly at03:42:35UTC, cause unobserved. An operator-coordination
question is pending; this checkpoint leaves the emulator closed to avoid
reopening a possibly operator-closed window. No fresh retail Gear-panel screenshot exists. Constructor/glyph regression and constructor repair are now installed;
file1-controller C remains a reviewed draft, with no native module adoption.

The user requested automatic model routing: use Luna for bounded fixes/tests,
Sol for moderate tasks when useful, and Astra for difficult diagnosis,
detailed coding and retail-correctness review. Default to the least expensive
model suited to the bounded task; escalate for uncertain retail semantics,
repeated failures, or a failed independent review, then return routine follow-up
to Luna. Avoid duplicate investigations and unnecessary agents. Keep explicit
file ownership. This controls task-agent selection; the main chat model is
selected by the user in the model picker.

The host-speed correction now passes normal-opening timing verification:
29.990 field frames/s at 1x and 149.091 at 5x (4.971x). The native bootstrap
was leaving PsyCross in its PAL fallback; it now publishes the retail NTSC
mode before starting the clock thread. Read
`docs/evidence/host-speed-20260905/README.md` for source authority and evidence.
Temporary framebuffer-cache, compiler and timing experiments are removed.
Inspect current run/process ownership before acting; older named replays
have ended.

The canonical project workbench is:

`docs/ai_context/ACTIVE_HANDOFF.md`

Use that file for the current PC-port state, checkpoints, verified runtime
evidence, user-confirmed visual status, and next safe step.
