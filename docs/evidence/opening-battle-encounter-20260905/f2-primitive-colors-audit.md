# F2 func_800B2AEC audit (2026-09-06)

Read-only production audit at `/var/home/blizz/Projects/xenogears-decomp-ai`, branch `experiment/worldmap-open-gates-20260823`, HEAD `3a3e7aac03a2f166fb924945a489e392d706f282`. Existing extensive dirty work preserved. No running xeno-port process observed in scoped process check. Parent owns animation_scripts.c changes.

## Recommendation

Implement this entire bounded leaf as native C, using explicit packed byte accesses and the existing `func_80021AD8`, in an already compiled source owner (animation_scripts.c or a narrowly scoped already-linked leaf owner). Do not simply replace INCLUDE_ASM in src/battle/main.c: that entire TU is reference-only and excluded by `pc_port/build_port.sh:126-129,172-173`. Do not turn the whole battle TU on. This is primitive RGB recoloring; it needs no GTE, rendering submission, state globals, allocator, or further undecompiled leaf.

Current native executable has `func_80021AD8` at `0x4cc928` and `PcPort_BattleMipsDispatchCallback` at `0x4f71b6`; `nm -g pc_port/build_native/xeno-port` has no func_800B2AEC. Source only has `INCLUDE_ASM` at src/battle/main.c:1279. Existing clamp in src/slus_006.64/system/animation_scripts.c:1098 adds two s32 values and saturates to [0,255]. F2 signed-halfword delta domain cannot overflow that addition.

## Retail provenance and contract

Authority: `asm/battle/nonmatchings/main/func_800B2AEC.s`, 0x800B2AEC..0x800B3348, length 0x85c. All 535 encoded instructions compare byte-for-byte equal with corresponding bytes of disc/battle.bin, load base 0x8006FAF0.

- battle.bin SHA256: `1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291`
- B2AEC byte-slice SHA256: `a16aa67b48a5c644f8abbe4d2ae75cc068122e2eccfea67bc21175408b91d03c`
- SLUS clamp 0x80021AD8..0x80021B04 SHA256: `20ee2dd52999178b6e1e6136ddcbe09ca0928b67c3b92c3a7d7d050121ccaf9a`

Effective signature: `void func_800B2AEC(void *header, void *buffer0, void *buffer1, s32 red_delta, s32 green_delta, s32 blue_delta)`. Deltas arrive as full ABI words; this function does not itself truncate them to halfwords. F2 supplies sign-extended halfwords.

0x800B2B24..38 snapshots descriptor = header + LE32(header+0x10), count = LE32(header+0x14). Count zero returns without descriptor/output dereference. For each descriptor `d`, key = `(d[3] & 0x1c) | (((d[2] ^ 1) & 1) << 8)` (0x800B2B3C..54). Only the following 12 of 16 possible keys write colors:

| key | source RGB triple bases in d | destination RGB triple bases in each buffer | entry label |
|---|---|---|---|
| 0x000 | 0x04 | 0x04 | 800B2C10 |
| 0x008 | 0x04 | 0x04 | 800B2E5C |
| 0x010 | 0x04,0x08,0x0c | 0x04,0x0c,0x14 | 800B2C3C |
| 0x018 | 0x04,0x08,0x0c,0x10 | 0x04,0x0c,0x14,0x1c | 800B2D34 |
| 0x004 | 0x10 | 0x04 | 800B2EB4 |
| 0x00c | 0x14 | 0x04 | 800B3090 |
| 0x014 | 0x10,0x14,0x18 | 0x04,0x10,0x1c | 800B2F6C |
| 0x01c | 0x14,0x18,0x1c,0x20 | 0x04,0x10,0x1c,0x28 | 800B319C |
| 0x104 | constant 0x80 each channel | 0x04 | 800B2E88 |
| 0x10c | constant 0x80 each channel | 0x04 | 800B3068 |
| 0x114 | constant 0x80 each channel, 3 triples | 0x04,0x10,0x1c | 800B2EE0 |
| 0x11c | constant 0x80 each channel, 4 triples | 0x04,0x10,0x1c,0x28 | 800B30E4 |

Keys 0x100,0x108,0x110,0x118 do no color writes but still advance. Source byte is unsigned. Each RGB component calls the sole direct callee func_80021AD8 with corresponding signed delta. Entire buffer0 RGB writes occur before buffer0 RGB bytes are re-read and copied individually to buffer1. No tag, opcode, UV or coordinate byte is changed.

Tail 0x800B32F0..3314 reads both d[0] and d[1] after color/copy writes. Both outputs advance by `(d[0]+1)*4`; source descriptor advances by `(d[1]+1)*4`. Counter loops until equal to initial count. Do not conflate the strides or cache d[0]/d[1] before writes. Do not use host GPU structs or extended pointer tag sizes.

For alias correctness, preserve each source load followed by its buffer0 component store; then copy buffer0 to buffer1 component-by-component in retail order. Do not replace the second phase with memcpy or cache all colors early. One subtle ordering detail: for the final channel, retail reads buffer0[4] after calling the clamp but before storing the last buffer0 channel, then writes that read byte to buffer1[4]. Distinct component offsets mean no ordinary observable difference from a post-store read, but retain ordering in a literal implementation if aiming at arbitrary alias/read-access trace parity. Keep descriptor tail reads after all writes.

## Can the existing bridge run it immediately?

The MIPS CPU core can interpret the whole leaf and clamp using pinned battle.bin and SLUS bytes in a standalone fixture; instructions are ordinary integer loads/stores, branches, shifts, JAL/JR. No GTE or external emulation boundary is needed if both ranges are mapped. Actual execution of this leaf was NOT_RUN in this audit; parent is building the retail fixture.

The production battle runtime can execute B2AEC as guest code while the actual battle overlay is loaded and active: target_is_guest_code accepts BATTLE_BASE..BATTLE_END (`battle_mips_runtime.c:373`), and the only external callee resolves to the existing native clamp (`battle_bridge_map.inc:84`; runtime fallback also exists). However, it is not an immediately usable F2-native-call solution: no six-argument entry wrapper exists. Public PcPort_BattleMipsDispatchCallback takes only one argument, initializes only a0, and requires non-NULL g_ActiveBattleRuntime (`battle_mips_runtime.c:815-846`). The active pointer is set only around func_80070F40 execution, then cleared (`:876-882`). It does not independently load battle.bin; main entry checks RAM at the battle entry (`:868-874`). Blindly invoking callback dispatch with B2AEC loses buffer and delta arguments, and invoking it during field opening is not evidence of an active battle runtime/loaded matching overlay. Thus: core capable; production wrapper unavailable as-is; full direct native leaf is the smaller reliable scope.

## Minimum useful acceptance

Use actual pinned retail B2AEC plus actual SLUS clamp with PcPortMipsRun, no replaced clamp callback; compare whole output arenas against native. Cover all 16 keys, irrelevant bits of d[2]/d[3], all color byte values, signed-halfword deltas at saturation transitions and extremes, zero/one/multiple counts, independent output/source strides including 0 and 255 byte fields, untouched sentinel bytes, both output buffers, and overlap cases. Negative controls should break key invert bit, each shape's offsets, constant-0x80 mode, signed deltas, second-buffer copy, and each stride independently. Separate leaf proof from F2 dispatcher operand/read-order proof and from fresh runtime opening progress.

## Scratch candidate review follow-up

Reviewed `/tmp/xeno-opening-ac-20260905/primitive_colors.candidate.inc` directly against all B2AEC branches. No semantic correction identified within the F2 signed-halfword delta and valid mapped-buffer contract. The proposed shared `src/battle/primitive_colors.inc`, included by battle/main.c and existing game_overrides.c, avoids excluded-TU/native availability trouble without duplicate bodies. `common.h` supplies u8/u32/s32 and types.h supplies uintptr_t on the matching target.

Constant-128 clarification: retail DOES call func_80021AD8 independently for every vertex/channel, not copy one adjusted triple across vertices. Key 0x114 performs 9 calls: RGB at output offsets 4,5,6; 0x10,0x11,0x12; 0x1c,0x1d,0x1e. Evidence: 0x800B2EE4,2EF4,2F04,2F14,2F24,2F34,2F44,2F54,2FF4. Key 0x11c performs 12 calls: same first 9 destinations plus 0x28,0x29,0x2a; call PCs 0x800B30E4,30F4,3104,3114,3124,3134,3144,3154,3164,3174,3184,3254. Keys 0x104/0x10c perform three calls each. The candidate's nested clamp loop is correct.

Exact sequencing (all handled classes): for every channel except final blue, obtain unsigned source byte or literal 128, call clamp, store result into out0. For final blue, obtain source, call clamp, read out0[4] into firstRed, store final blue into out0, then store firstRed into out1[4]. Afterward each remaining selected out0 byte is freshly read and immediately stored to corresponding out1 offset. This is essential when out1 overlaps out0 with displacement: a write may change a later source read. Candidate preserves it, including special firstRed snapshot.

Constant paths often place the previous result's SB in the delay slot of the NEXT JAL (for example 0x800B2EF4 JAL and 0x800B2EF8 SB). The delay-slot store executes before entry to the next clamp; it does not defer the store until after the next clamp returns. Candidate's call/store/call sequence is therefore equivalent, not an ordering defect.

Candidate also preserves late stride reads after all copies, descriptor classification before any writes, and separate source/output stride. Byte-field read ordering within the key expression is unspecified by C, but no intervening writes/calls occur and these are ordinary packed RAM bytes; this produces no buffer-alias difference. No volatile/MMIO contract is claimed. No runtime proof or ASM/BINARY-MATCH is inferred from this review.
