# Read-only pilot-window followup, 2026-09-06 UTC

## Current verdict after constructor followup

**The zero-row cause is now CONFIRMED at an unadapted guest/native constructor call.** Followup run `opening-result-after-5-a3gljb50` captures dynamic retail caller801E6FC0 supplying seven arguments with rowcount4. Generic bridging calls the native eight-argument ABI as mode4,height0. The returned window has rowcount0. The smallest justified correction is specific to the bridge target80032F54, preserving native callers' existing ABI. Read original guestSP+18 as a16-bit scalar, sign-extend its low16 bits for native height, put it in host argument8, and set dead host argument7 to0. Do not reuse a pointer-translated argument for this scalar.

Actual letter pixels/string identity remain **UNRESOLVED**. The raw retail module now establishes constructor->string selection->queue->C9 draw activation, but this report does not claim that the queued string has been decoded as the requested pilot wording or that fixed glyphs have rendered. See the final followup section for exact observed and retail-byte evidence.

## Earlier window-only result and boundary (superseded where noted below)

The natural battle calls the ordinary text-window routine `func_80034888` from retail call PC `80074FF4`. In six captured calls (frames350–355), the same window has **row count0** at+0x0C and flags0x000F at+0x10. Zero rows suppresses both text-row draw loops. Flag0x8 also suppresses further text decoding at the captured moment. These conclusions follow from the captured data and actual retail instructions; they do not depend on SDK FontDrawLetters.

A **retail/native constructor ABI difference** is verified: retail `func_80032F54` consumes seven arguments, whereas the native function consumes eight with a dead mode placeholder. Existing native field/world callers intentionally compensate for that difference. No direct constructor call or embedded constructor function pointer was found in retail battle.bin. Therefore this audit does **not** yet prove an unadapted call caused the zero row count, or that the captured window's string is the missing pilot line. Creation provenance and string identity remain UNRESOLVED.

Only this report was written. No production, probe, run, runtime state, or existing root scratch was changed. Root owns the concurrent return-state repair and future runtime observations.

## Evidence pin and completed-run limits

Source: `pc_port/build_native/opening-panel-owner-5-_rz0j8ci/census/census.jsonl`.

- Final read:3556 JSONL records, maxframe356.
- SHA-256:`e838cc9af9b56cc96a7e5b508cb7baf69dd7b362579710a291d94553dac5ed75`.
- Event counts:3072 link,356 frame_summary,99 upload,12 draw,7 window_gate,6 window,2 finish_out_of_scope,1 installed,1 battle_enter.
- Root reported the inferior exited normally, with native/GDB absent and no stop.json. Root did not request the exit; its cause is unobserved. This is not a completed frame405/500 diagnostic or opening acceptance.
- Heavy capture covers only350–355, with gate state at356. Link sampling was **exhausted in every heavy frame**:1036/1042/1046/1032/1029/1033 calls, of which524/530/534/520/517/521 were dropped. No link attributed to80074FF4 was retained, but that absence is not evidence the window emitted no primitives.
- No SystemRenderStringEntry event occurred in the captured interval. This API is not required by the active window: func34888 calls func33DF0 directly.

## Exact captured window

Guest global800D2DAC contains801AFAF0; bridge translates that to native007C2910. UI state pointer at800D2D28 is800DDCB8; bytes+C8/C9 are0101 in all seven gate observations. All six0x90-byte window dumps are identical.

| Window offset | Value | Meaning grounded in current native code / retail reads |
|---|---|---|
| +00 | 0016 | current horizontal cursor |
| +02 | 0000 | current row index |
| +04,+06 | 001C,0018 | x28,y24 |
| +08,+0A | 0064,0019 | pixel width100, width units25 |
| +0C | **0000** | text row count |
| +0E | 0100 | texture Y base |
| +10 | **000F** | active0x4, wait0x8, hide-background0x2, wrapped0x1 |
| +12,+14 | 001C,000E | buffer stride28, row spacing14 |
| +16,+18 | 0000,0001 | row base0, logical line count1 |
| +1C | 007C1F7F | current encoded-string pointer, bytes not captured |
| +20,+24 | 00000000 | saved/nested pointers unused here |
| +28 | 007C414C | row allocation pointer |
| +2C | 007C4154 | raster work pointer |
| +68,+69 | 01,01 | glyph rate |
| +6B,+6C | 01,01 | end-string/wait state as written by native decoder |
| +6E | FF | selected/highlight row sentinel |
| +82,+84,+86,+88 | all0 | queue count and timing counters |
| +8C | 00000000 | queue head |

The row pointer and raster pointer are only8 bytes apart. This is consistent with the constructor making a zero-sized rows allocation followed by a work allocation, but allocation history was not captured, so it is corroboration rather than proof of the writer.

The frozen state is also consistent with text reaching an end marker: native func80033DF0 code==0 sets flag0x8 and bytes+6B/+6C=1. That does not identify which string ended. No raw bytes around007C1F7F were included in this run's window record.

## Retail instruction authority for suppression

Read directly from `disc/SLUS_006.64` with Capstone MIPS32 little endian. File offset is address-80010000+800. Constructor/update bytes are retail authority, not native debug symbols.

- 80034A40: `lh v0,0x0C(s0)`;80034A48:`blez v0,80034B94` skips the first/right-half text-row loop when row count<=0.
- 80034BA4:`lh v0,0x0C(s0)`;80034BAC:`blez v0,80034D04` skips the second/left-half loop.
- 80034D3C loads flags;80034D44 masks0x58;80034D48 skips decode/upload if nonzero. Captured0x000F&0x58=0x8.
- The decoding call would be80034D50->80033DF0, followed by LoadImage at80034D74, if its gate passed.
- Row primitive insertion calls are80034B70 and80034CE0->80031798. The row width gates themselves are80034AE8 (width>=0x41 for the second half) and80034C48 (width!=0 for first half).
- The routine can still insert draw-mode primitives at window+3C/+30 with zero rows. Such insertions would not prove glyph submission.

Current `src/slus_006.64/system/system.c:891` uses the same relevant offsets and gates. No offset disagreement in func34888 itself was established for the observed zero-row state. Queue count0 does not suppress it immediately because flag0x4 is set; the early queue-return branch applies only when flag0x4 is clear.

## Constructor ABI difference: verified but causal path unproven

Retail80032F54 saves0x38 bytes of stack. It loads:

- 80032F94:`lhu a3,0x48(sp)` = originalSP+0x10 = argument5/y;
- 80032F98:`lhu v1,0x4C(sp)` = originalSP+0x14 = argument6/width;
- 80032F9C:`lhu a1,0x50(sp)` = originalSP+0x18 = **argument7/rows**;
- 80032FD8:`sh a1,0x0C(s2)` stores that row count;
- 80032FFC..80033010 multiply the stored row count by0x60 for HeapAlloc.

Current native signature at system.c:39 is `(window,tpageX,tpageY,x,y,width,mode,height)`, ignores mode, and stores eighth argument height at+0C. This cannot be called as an unmodified retail seven-argument ABI.

However, this difference is deliberately accommodated in existing native call sites:

- `src/field/dialogue/text_box_render.c:574` passes mode then height as arguments7/8.
- `pc_port/src/world_map_callback_92be4.c:79` passes0,1 as arguments7/8.
- `pc_port/src/world_map_callback_92df8.c:116` passes0,4 as arguments7/8. Lines23–25 explicitly document the accepted host dead-placeholder ABI.

Changing only the constructor's signature/slot would make those existing native calls use mode0 as their row count. No such change is authorized or justified by this audit.

An exhaustive raw JAL scan of battle.bin found no direct call into80032F54..8003342C and no direct queue/control call into800345E0..80034888. A byte scan found no embedded80032F54 or80034714 function pointer. The battle assembly contains the800D2DAC read in func74F70 and its BSS declaration, but no same-symbol writer. These scans do not exclude indirect calls, base-plus-offset stores, earlier/prebattle construction, or another resident wrapper. A natural constructor probe must start before battle entry to resolve this.

## Corrected panel-intersection callsite interpretation

Two retained packet contexts are **outer guest draw calls**, not established pilot or glyph owners:

- Guest call800A4AE4 is retail JAL8002C700, a model-render path. Census native stack runs through ModelPrimTriAverageVariant0/PcPort_LinkModelPrim to PcPort_AddPrimDomainAware, with guest_pc8002C700.
- Guest call800A47B0 is retail JAL800273C4. Census reaches native AddPrim through that helper, with guest_pc800273C4 and `direct_guest_addprim=false`.

Both can emit geometry overlapping the broad panel rectangle. Neither proves the visible pilot panel belongs to that packet without actual texture/frame attribution. Do not relabel800A47B0 as a direct retail AddPrim instruction.

## Bounded next root-owned observation

1. From full process start, observe native func80032F54 entry and return with all eight host arguments, guest CPU context when present, and native caller stack. Preserve actual seventh/eighth values and returned window+0C.
2. Track the window that ultimately becomes guest801AFAF0/native007C2910 (addresses may change next run) by creation/data identity, not a planted fixed address.
3. Read its encoded string at enqueue time (func80034714), then at decoder80033DF0 entry/end. Decode actual bytes using the resident text encoding. This establishes whether it is the pilot line.
4. Capture its row metadata and bitmap/upload rectangle, and prioritize/reserve link budget for native stack func80034888/func80031798. Current early model packets consumed the shared512-link budget before window links could establish attribution.
5. Only if an unadapted retail seven-argument call is naturally observed, or another writer demonstrably zeros+0C, identify the smallest adapter/callsite correction and required native-caller negative controls.

No forced font flags, row count, queue state, string pointer, or animation state should substitute for that observation. Pilot text owner remains **UNRESOLVED**, with zero-row suppression now **CONFIRMED** for the captured active window.


## Constructor followup: unadapted guest call confirmed

New root-owned run: `pc_port/build_native/opening-result-after-5-a3gljb50`.

- Binary SHA-256 from run.json:`82945416d614c27362542441a9a934e281390abde4ef709ba410b72c550cc64e` (after independent battle-return repair).
- Runtime source SHA-256 from run.json:`fd4fae9b90677538e8ec7b754e814690d2de6434c39165a512c32b7ca07a6d80`.
- Constructor capture file SHA-256 at review:`d3bbcd9499b7524a97ceb12a8c2a426a1534a9948692d16325645f00dbb356eb`.
- Constructor records1–8 are ordinary native field calls and successfully produce rows12,11,10,11,11,3,1,4. Record9 is battle_active=true and has native stack `func80032F54 <- runtime_bridge_call <- runtime_bridge <- PcPortMipsRun`.

Record9 exactly:

- guest_pc80032F54, guest_call_pc801E6FC0;
- guest register arguments `[801AFAF0, 00000380, 00000100, 0000001C]`;
- guest words at SP+10 onward:`00000018 00000018 00000004 00000000 800C0000`;
- host args:`window=007C2910,tpageX=896,tpageY=256,x=28,y=24,width=24,mode=4,height=0`;
- after constructor, window+0C=0000, rows=007C414C, work=007C4154 (8-byte gap), string pointer0 because enqueue is later.

This directly demonstrates the missing adaptation, rather than merely an interface difference. An actual guest seven-argument call supplied4 rows; the unadapted native routine instead used unrelated originalSP+1C as its eighth height argument, observed0. No planted state is needed to explain zero-row suppression.

## Exact dynamic retail caller and string-queue path

The caller is in the separately loaded module, not base battle.bin. Existing byte provenance identifies archive directory0x20,file1, Mode2/Form1 sector3EC7F, size4C3C, loaded at801E5000. This audit independently reread those disc sectors in memory (2048 data bytes at raw sector+24) and recomputed SHA-256 `64668d85bf48dea46cf6d5f04b38e38ca887877cfef53961f0c26cd2d363b670`, agreeing with `sprite-callback-retail-audit.md`. No module file was written.

Retail instructions:

- 801E6F60 sets UI state+C8=1.
- 801E6F74/78 set tpageX896,tpageY256.
- 801E6F80 loads `t0=*(800D3278)` (panel/control state).
- 801E6F88 loads `a0=*(800D2DAC)` (window).
- 801E6F90 writes y to SP+10.
- 801E6F94 reads state+7FA as unsigned16;801E6FA4/A8 multiply by3;801E6FAC writes width toSP+14.
- **801E6FB0 reads state+7FC with LHU;801E6FC0 calls80032F54;801E6FC4 writes that row count toSP+18 in the delay slot.**
- 801E6FF0 invokes80034614 on the window after setting its hide-background flag.
- **801E6FFC loads the string table pointer at800D3340;801E7000 loads the string index fromSP+20 as unsigned16;801E7004 invokes GetStringEntry.**
- **801E7010 loads window at800D2DAC;801E7014 invokes80034714 with the selected string in a1.**
- **801E7028 sets UI state+C9=1**, naturally activating base battle80074F70->80034888.

This resolves the earlier no-direct-base-battle-call result without weakening it: the creation/queue writer resides in the dynamic retail module. The next runtime string probe should use the801E7014 queue call and record its a1 contents/table index, rather than SDK FontPrintf or attack package0x17.

## Review of bridge-only correction

The proposed target-specific mapping is correct in scope: adapt only a resolved guest native-call target80032F54. Leave `src/slus_006.64/system/system.c` and compensated native field/world callers untouched. Raw guest stack argument7 becomes native argument8, and native dead argument7 becomes0.

**Refinement required for faithful scalar semantics:** do not implement the general mapping as merely `args[7]=args[6]` after generic translate_argument. That code interprets pointer-shaped scalar words as memory addresses. A guest argument word80000004 orA0000004 should still supply four rows, but generic translation can convert it to `g_PsxRam+4` before the copy.

Retail's exact row behavior:

1. LHU80032F9C reads only two bytes at originalSP+18, ignoring high16 bits of the argument word.
2. SH80032FD8 stores those bits into window+0C.
3. LH80032FFC sign-extends window+0C for rows*0x60 allocation.
4. LH80033390 likewise supplies the signed row limit for row initialization.

Therefore read width2 at guestSP+18 and convert the resulting low16 value to a signed16 numerical value for the host height parameter. An explicit arithmetic sign extension avoids relying on conversion of out-of-range unsigned16 to signed16:

```c
uint32_t raw_rows;
if (runtime_read(runtime, cpu->gpr[29] + 0x18u, 2, &raw_rows) != 0)
    return -1;
int32_t rows = (raw_rows & 0x8000u)
    ? (int32_t)raw_rows - 0x10000
    : (int32_t)raw_rows;
args[6] = 0;
args[7] = (uintptr_t)(intptr_t)rows;
```

This is a review sketch, not an installed patch. It can run before the final native call, provided it rereads raw guest bytes and overwrites the generic-translated slots. It must be guarded by the resolved address80032F54. A specialized branch before generic translation is also valid. Read failure must fail the bridge, not synthesize rowcount0. No change to guest registers, guest stack, guest flags, or the selected string is needed.

Negative/huge row counts are not plausible accepted production input; a bridge test can inspect signed mapping with a recording native callee rather than asking the real heap to allocate them. This followup reviews the row argument only; it does not assert that every scalar of the generic bridge has already been audited.

Meaningful bounded verification for the proposed correction:

- Real observed record: rows4, unrelated eighth0 -> native height4, dead mode0, window+0C4, allocation rows*0x60=0x180.
- Vary ignored high16 bits of argument7, including8000/A000/1F80-shaped words ending0004; all map to4.
- Mapping edge values0000,0001,7FFF,8000,FFFF; use a recording callee for unsafe allocation cases.
- Poison unrelated eighth argument independently; it must not affect the mapped height.
- Omission/mutant of the adaptation must reproduce observedheight0 or wrong8th dependence.
- Ordinary direct native eight-argument field/world calls still use eighth height; their mode placeholder remains dead.
- Other resolved guest targets receive unchanged arguments.
- An unreadable requiredSP+18 halfword fails the bridge.

The constructor capture establishes the ABI bug and four-row intended state. The window-render probe's128-record cap was consumed before battle, so this new run does not supply post-fix battle letters or a complete new window census. Final glyph/pilot acceptance remains pending an unforced rerun and string/packet/framebuffer attribution.

## Review of root's concrete candidate

Reviewed `/tmp/xeno-battle-result-probe-20260906/runtime.window-candidate.c`, verified SHA-256 `2b11d7e73cd1096869d465dfc76bc080cc0512a1e9c83124933484b69be42b09`. Its only diff against the inspected runtime is the target80032F54 branch after generic argument collection. It rereads originalSP+18 with width2, returns-1 on failure, sets args6=0, and sets args7 to `(uintptr_t)(intptr_t)(int16_t)height`.

**Review result: appropriate minimal bridge correction; no functional blocker found for the current supported native toolchain.** It does not copy a pointer-translated scalar, does not alter the selected guest window/string/stack, and leaves native compensated callers and other bridge targets untouched. The signed16 cast matches the target's two's-complement semantics; the explicit arithmetic alternative above is useful if implementation-independent C conversion semantics are required. Test the signed edge values with the actual target compiler/bridge as planned.

One comment wording clarification: the branch is physically after generic pointer translation, but it **rereads the original raw scalar**, so it bypasses the translated value correctly. “Read the guest scalar directly, bypassing the translated argument” would describe its operation more precisely than “before pointer translation.” This is not a correctness issue.

The worker's planned actual-bridge typed eight-argument spy, high-bit/eighth-poison/read-failure cases, and omission negative control cover this change's ABI contract. This review does not promote that spy into real constructor/glyph-runtime proof. Root retains the subsequent normal opening/framebuffer/return observation gate.

Retail main executable was freshly rehashed as `dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119`, confirming the constructor instruction source used above.
