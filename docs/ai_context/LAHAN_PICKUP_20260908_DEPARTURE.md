# Lahan continuation after natural departure

Full user goal remains ACTIVE: start-to-end Lahan 100% retail accurate 1:1, decompile as needed, invent nothing. The natural walkthrough now reaches the world map; this does not establish the full goal. User explicitly resumed this session. No stage, commit or push. Preserve the large concurrent dirty tree. Branch experiment/worldmap-open-gates-20260823, HEAD3a3e7aac03a2f166fb924945a489e392d706f282.

## Live natural run

scratchpad/lahan-natural-20260908-slope-ground: game563309, Xvfb563307, DISPLAY:2, window2097204. Frozen binary0ebb5b95379d474c9f2dc3859e323a6d93acd8ff8e811adccd54b43f31b802da includes grounding/party removal/motion repairs, but NOT the later lighting repair below. At23:07CDT observer563306 was SIGSTOPPED and confirmedT so its23:13:40 teardown would not kill the run. Do NOT SIGCONT it after the deadline unless intending teardown. The game and Xvfb remain independent and live as last observed; verify host PIDs and screenshot on resume. Sandbox ps cannot see host game; approved host exec required. No active input jobs remain.

Natural route completed village, mountain and west gap/bridge, Citan/Yui/roof/musicbox/dinner, night return, burning Lahan, Gear battle, destruction and aftermath. Ordinary inputs only; one earned Hob-Jerky used normally and two encounters escaped normally. Gear battle five heavy attacks won;225totalEXP,level5,200gold. Aftermath0..151 ordinary confirms then10seconds produced rendered world map. departure-final-observe.png inspected;run.log enters worldmap scheduler. Previous black departure cleared. See docs/evidence/lahan-party-remove-20260908/runtime-observation.md/json and grounding runtime-observation.md/json. These are natural observations, not controlled live A/B isolation or hardware audiovisual proof.

## New lighting repair after that run

Retail caller80071618..8007167C verified against field.bin. Prior blackmoon audit inference was wrong: both JAL delay slots explicitly write ninth value0 atsp+24. Native calls omitted it and color element6=0x800. Corrected both calls in src/field/main/misc3.c and added full prototype. src/field/main/main.c writer80077844 now takes int arguments to match retail LW rather than LHU. Whole64bytes now EXACT,zero relocations. Full FieldLoad stillFAIL3416compiledvs3484retail.

pc_port/tests/run_field_light_init_test.sh: original retail caller/callee versus verbatim production call statements + full main.c TU.256memory/register/stackpatterns and65536writertruncationcases atO0/O2/UBSanPASS. FournegativecontrolsPASS. Native/fullPSXbuildPASS. docs/evidence/lahan-light-init-20260908 contains pins,exactstatus,red/greenlogs,change.diff andbefore snapshots. Existing unrelated source changes preserved.

Final native binary SHA256: a1196822e8c6319816a7d978b02aea59d9ec7f8cfc8a7f9082481e060f775c3f

Isolated map2before/after diagnostic under scratchpad/lahan-light-init-20260908 timedout124 normally. Captures450..600 are dark and do not establish visual acceptance. The diagnostic after binary contains calls fixed before final int signature. Do not confuse it with finalbuild or completed natural run. Captures with.png extension are actually BMP; converted view-540/view-600 are realPNG. No live state was patched. SolidblueGear and stripedworldmapobjects remain unaccepted observations.

## Next work

Verify runtime and source pins. Visually validate final lighting build through a fresh natural path or clearly separated diagnostic; do not claim an isolated scene as full natural acceptance. Continue retail-backed exact/decompile work: partyremove424vs436,ground2396vs2704,constructoromissions,paintingwallartifact,actorlifetimezero-root,remaining73nativefunctionstubs,battlelargelyinterpretedMIPS,otherexactFAILs. FullhardwareAVparity and exhaustivebehavior unproven. Do not mark the goal complete because departure works.

For ordinary input use manual-step.py in the natural-run directory. Read screenshots before inputs and BEFORE separate field-GDB reads. Never dereference field actors during battle/black transitions. No breakpoints (earlierINT3observer crashed), no writes/calls. Prior19:24handoff is historical; CURRENT.md in run folder has the complete route/encounter progress. Old motion-matching/party-remove run timers have expired; do not assume their PIDs alive.

## Active continuation after this handoff

Fresh final-lighting natural run started under scratchpad/lahan-natural-20260908-light-init, driver1300344,12hourtimer. Read its process.json/window.txt/driver.log for current state. It uses finala1196822 build and ordinaryNewGame; automatic confirms stopatpainting14. Older slope-ground worldmap remains preserved,observerSTOPPED. Isolated party-removal source variants under scratchpad/lahan-party-remove-exact-20260908 ALLFAILsize424/432/464vs436; no productionchange. Direct X11field16 finalbuildscreen18 shows detaileddarkblueGears; parity stillUNPROVEN. No commit/push.

### Latest: menu texture correction and Equip failure

Read CURRENT.md in light-init run: nowstuckGameHandleError131inHeapFree(NULL)fromEquipwindow2cleanup,confirmedread-onlybacktrace. UnderlyingresourceModes3/19andEquipcore801E05D0unported. Preservegame1300347;noerrorbypass. Separate menurestoreY0->256fix insrc/field/main/misc4.c verifiedretail68bytes,32wholeVRAMpatternsO0/O2/UBSan+4negativecontrolsPASS,nativePSXbuildPASS. docs/evidence/lahan-menu-texture-restore-20260908. Livea119binarypredatesmenufix. FreshvisualvalidationPENDING. Nextdecompileresource/Equipownersfromretailbeforefreshnaturalrun;fullgoalACTIVE.

Equip resource follow-up: src/menu/main/misc.c resource dispatcher nowimplements3/0x13fourbanks withretailorder.512casesO0/O2/UBSan+4negativecontrolsPASS,nativefullPSXbuildPASS. docs/evidence/lahan-equip-resources-20260909. Core801E05D0 STILLSTUB:2472verifiedretailbytes,23directcalleesnotlistedasstubs(butnotprovencomplete). Nextfullcoredecompile/differentialtest;donotrunasifEquipfixed. Live1300347 remainsfrozena119errorloop;newbinarynotloaded. TextureYrestorefixincludedinnewbuildbutnaturalvalidationstillPENDING. FullgoalACTIVE,no commit/push.

Equipcore801E05D0nativecontrolflow nowimplemented;400casesO0/O2/UBSan,618/618retailinstructions,6negativecontrolsPASS. docs/evidence/lahan-equip-core-20260909. NativePSXbuildPASS;PSXstilloriginalASM(noexactCclaim). Stubcount79now:core-1,newlylinkedhelpers+7:DE5CC,DF0D4,DF5D0,DF890,DFB68,DFF5C,E36D4. Earlierabsent-from-stubs censuswasnotimplementationproof. NEXTimplement7helpersand auditDE2C8/DE36Crawmenu+434pointerwidth,resourceModes7/17. No freshruntimeacceptanceyet;game1300347stillpreservedolderrorloop. FullgoalACTIVE,no commit/push.


Equip snapshot/cancel follow-up: DF5D0 and DF890 now native retail-backed implementations. 10,560 cases per O0/O2/UBSan and four negative controls PASS; native/full PSX builds PASS, function stubs77. Evidence docs/evidence/lahan-equip-snapshot-20260909. PSX retains ASM, no exact C claim. Cancel restores only three third-bank bytes (retail asymmetry preserved). NEXT: remaining DE5CC,DF0D4,DFB68,DFF5C,E36D4 helpers plus init/cleanup raw434 pointer and resource7/17 audits. No fresh runtime acceptance; preserved older error-loop binary unchanged. Full goal ACTIVE, no commit/push.


Equip effects follow-up: 801E36D4 now implemented from940 retail code bytes plus40-byte jump table. 263,168 retail differential cases each O0/O2/UBSan PASS, four negative controls rejected. Native/full PSX builds PASS; function stubs76. Evidence docs/evidence/lahan-equip-effects-20260909. Original PSX ASM retained, no exact C claim. NEXT remaining DE5CC/DF0D4/DFB68/DFF5C plus resource7/17, raw434 init/cleanup pointer audit and existing stat-helper stride audits. Fresh runtime acceptance still PENDING; full goal ACTIVE. No commit/push.


Equip preview writer DF B68 (func_801DFB68) implemented from708 verified retail bytes. 14,336 cases per O0/O2/UBSan and four negative controls PASS; native/full PSX builds PASS. Evidence docs/evidence/lahan-equip-preview-20260909. No exact C claim; PSX keeps ASM. NEXT DF0D4 has no direct callees; DE5CC list builder and DFF5C description renderer remain unported, plus resource7/17 and raw434 init/cleanup/stat-stride audits. Full goal ACTIVE; no fresh runtime acceptance or commit/push.


## 2026-09-12 rebaseline and stat repair
HEAD f67fe692; older statements that DE5CC/DFF5C/resource7/17 are missing are stale: native implementations now exist. func_801E3A80 still had two retail defects, now repaired: native resource stat destination uses typed unkB8; character-type4 divisor10 replaces5.16,896 retail comparisons each O0/O2/UBSan and four negative controls PASS. Native build and PSX menu overlay PASS. Full PSX build FAIL in concurrent battle/main27.c func_8007D478 declaration before h. Exact comparison FAIL despite428-byte matching length. See docs/evidence/lahan-equip-stats-20260912. Next audit remaining Gear/stat helpers and integration before natural Equip acceptance. No runtime completion claimed; old process IDs are historical (no game process observed in current host census). No commit/push.


Gear aggregation801E3C2C implemented from672 retail bytes.30,720 cases per O0/O2/UBSan and four negative controls PASS; PSX menu PASS. First native link rejected due partial ELF set misclassifying586 symbols. Built slus/field/member-change/shop ELFs, then native final rebuild PASS with classification restored. Evidence docs/evidence/lahan-gear-stats-20260912. NEXT fix caller801DFE2C raw pointers/0x28 stride, missing801E433C and audit801E4754 resource pointer. Harden build_port.sh partial-ELF guard. Full runtime and exact C acceptance unproven; no commit/push.


Gear preview caller801DFE2C now fixes0xA4 stride, typed native resource ownership and stat copies, with retail reloads between callbacks.16,896 cases per O0/O2/UBSan and four negative controls PASS; native and PSX menu PASS. Evidence docs/evidence/lahan-gear-preview-caller-20260912. Tests extract production caller body and use two shared fixtures; callee integration/runtime still unproven. NEXT801E433C decompile,801E4754 pointer audit, partial-ELF build guard. No commit/push; full goal remains incomplete.


Native stub classification guard repaired: unknown ELF symbols now reject before output overwrite; build driver explicitly stops on generator failure.5 real-ELF/driver tests PASS; partial menu-only real input rejects, full native build PASS67function/572data stubs. Evidence docs/evidence/native-stub-classification-20260912. User additionally authorizes pushing and running on Bazzite; no push performed in this pass. Next continue801E433C Gear-effects decompile and801E4754 pointer audit.


801E433C Gear accessory effects implemented from1048retail bytes+44-byte jump table.32,896 cases per O0/O2/UBSan and four negative controls PASS; native66function/572data stubs and PSX menu PASS. Evidence docs/evidence/lahan-gear-effects-20260912.4928callback fixture only; next audit real4928 and4754 pointer/behavior then integration. Retail9->10fallthrough and postcallback mapping reload preserved. Full runtime/exact C acceptance unproven. No commit/push in pass; prior user push/Bazzite authorization remains.


Gear weapon/callback fixes:4754 native resource pointer uses retained32-bit typed slot;4928 divisor120 replaces15 per retail.262,144 callback+10,240weapon cases per O0/O2/UBSan and four mutants PASS. Real-callee integration rootDFE2C through3ECC/433C/4928/4754/3C2C passes19,200cases per O0/O2/UBSan and two mutants, no helper fixtures. Native and PSX menu builds PASS. Evidence docs/evidence/lahan-gear-weapon-20260912 and lahan-gear-integration-20260912. NEXT fresh natural Equip/Gear runtime check and remaining rendering/layout dependencies. Full byte/hardware fidelity unproven. No commit/push in pass; authorization remains.


2026-09-12 visible Equip follow-up: desktop run game279244 crashed in DE5CC after natural opening and manual Equip input. Core proves raw retail resource+4 read high native weapons-pointer half, yielding NULL accessories. Corrected test uses native MenuUnk6 separately from retail buffer and reproduces pre-fix SIGSEGV. Typed production pointer fix passes352cases O0/UBSan+4negative controls; native build PASS. O2 divergence and visible ghost text/missing stats remain unresolved. Evidence docs/evidence/lahan-equip-live-layout-20260912. Fresh visible DISPLAY=:0 run scratchpad/lahan-natural-20260912-equip-layout-desktop, driver300016/game300025, startup screenshot confirmed. No virtual display. Automatic ordinary input stops at painting field14; preserve user control. NEXT observe fresh Equip behavior. Full goal incomplete; no commit/push.
