# Independent A9 implementation review — 2026-09-06

**ACCEPT for the bounded handler implementation. No production correctness blocker found.** This review covers the actual frozen production A9 case and durable fixture, not merely the earlier standalone candidate. Native outer-interpreter execution, a new gameplay replay, and C byte matching remain separate gates.

Reviewed production `src/slus_006.64/system/animation_scripts.c` SHA256: `8e5de18aa4c657cea9026626b4dd9a0d92f104788a4438024fa7a10c910bec5c`. A9 block at lines324–340 SHA256: `a90dfd10a78014bda2425e1d5e1cef37caf41ded43bf153ac2d513f6eea8c783`.

Durable evidence: `pc_port/build_native/sprite_opcode_a9_retail_test.aHN6LSau`. Fixture SHA256 `63a87ce09b0454f1adaeb3a3bb8fee51a0864f2012352b0df68dab0206a47747`; runner SHA256 `ff456fea5fd74fd555167f8d1ea298f015813c334df81e79734871596732c6e0`. Independent source-pin verification and execution results are in `/tmp/xeno-animation-a9-review-20260906/recheck.json`. All review writes stayed in the assigned report/scratch scope; no production/config/test/game-runtime files were changed by this lane.

## Retail authority and dispatch

I independently hashed the current 303104-byte `disc/SLUS_006.64` (`dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119`) and verified every range in the original audit's pins.json against that file. PS-X mapping is VMA minus0x8000F800. The retail dispatcher explicitly masks A1 with0xFF at8001FBF8, subtracts0x8A, bounds index below0x73, and dispatches through800183D8. Index31/A9's word at80018454 is80021698. The complete92-byte A9 range `[80021698,800216F4)` hashes to `9c729396bad3f859aa200f99af71826f25260c7f0fd8d8668dda58519b9caf83`.

The native dispatcher likewise narrows to u8, bounds the index and selects A9. The strengthened durable fixture now uses opcode high bits `0xFFFFFF00`, testing bits8–15 as well as16–31; the prior `0xA5000000` poison could not distinguish an erroneous u16 narrowing. This review suggestion was incorporated before Sol's final freeze.

`authority-helper-disasm.txt` and `authority-check.json` preserve the independent dispatch/helper byte check. The original audit Markdown line40 has a documentation-only SHA typo: the length-table hash has an extra trailing `1f`. The correct64-digit hash, already used by pins.json and the durable runner, is `f329de9c682e328cd9ca4718972691f3bb716046f2f57803c3a24e9b19326c0a`. This does not affect executable authority checks.

## Arithmetic, ordering and native helper ABI

The production body implements the retail sequence:

1. Read signed operand byte and signed sprite halfword+0x2C.
2. Multiply, add0xFFF only for a negative product, arithmetic-shift12 to truncate toward zero.
3. Call `func_80022CAC(sprite, value)` once, using the same pointer and signed32 scalar ABI as retail A0/A1→V0.
4. Reload flags+0xAC **after** the helper; test bit2, not bit3 or flags+0x3C.
5. Convert the helper return to u32 before shifting16; negate as unsigned when mirrored; add modulo2^32 to word+0x00. Return without a local animation-PC write.

The unsigned multiply, unsigned shift, unsigned negate and unsigned position addition avoid host signed-overflow/negative-left-shift UB while preserving R3000 low-word behavior. For actual A9 input ranges the first signed mathematical product is already bounded `[-4194176,4194304]`; its Q12 result is `[-1023,1024]`. C casts/right shifts rely on the supported GCC/Clang two's-complement arithmetic-shift implementation, which the compiled differential exercises.

The actual native helper owner is **pc_port/src/game_overrides.c:3295**, as declared by `pc_port/port_owned_overrides.txt:15`; build_port.sh weakens the duplicate matching definition in temp1.c so native callers bind to this strong override. Its body SHA256 is `40ac6e9930e7b98cdd1fbcdecd179b8d47c65accfe34fecf74430d59500c9c31`. The durable runner extracts and compiles this actual body, renaming only its symbol; its wrapper records the call before invoking it. It is not replaced by the earlier candidate helper formula or a success stub.

Retail helper `[80022CAC,80022CDC)` hash is `7edc9e76b551fe6d2b564c8ba92beb0785e2426f125dcc3695d928640f42f3c0`. Its +0x3A load is unsigned16; zero is passthrough; otherwise the signed input product is rounded toward zero by adding0x3FF when negative and shifting10. Although the existing native helper has signed multiplication, **there is no A9 overflow**: every possible factor0..65535 gives product between-67042305 and67107840, safely within s32, and the rounding addition is also safe. Generic helper inputs outside the A9 domain are not covered by that proof and are outside this repair.

The post-helper flags read and position read preserve retail order. Operand aliases into position, scale, helper factor or flags are safe for this body because the operand and scale are read before the sole write; the actual helper does not mutate the sprite. No unrelated byte, flag or PC store was added.

## PC advancement: precise gate

Retail shared tail `[80024EC8,80024F00)` calls the dispatcher, reads `D_8004FC40[opcode]`, adds it to sprite+0x64 and re-enters. A9's byte at8004FCE9 is2. I parsed the actual native length table: all256 bytes hash to the same retail table hash, and A9's native entry is2.

The fixture executes the **raw retail outer interpreter** on `[A9,0x21,0x86]`; it observes exactly one A9/helper call, PC advanced2, and the waiting86 state. This is real retail PC-execution evidence. The actual native temp1.c shared path at lines1374–1386 passes `pc+1`, calls the A9 handler and advances by the same table entry; the handler does not modify+0x64. This establishes the bounded source path statically.

The durable fixture does **not** link and execute native func800248D4. Its output/provenance now explicitly labels native PC advancement **NOT_RUN**. Therefore this review does not turn the retail stride test into a native outer-interpreter execution claim. No outer-interpreter source change is required by A9.

## Fixture quality and independent recheck

After Sol's ready signal, I checked every pinned current source against aHN6LSau, inspected the final body/helper extraction/mutations, and independently reran its retained O0, O2 and mixed-UBSan executables. All three return0 with `SPRITE A9 DIFFERENTIAL PASS cases=4608` and the retail-PC PASS/native-PC NOT_RUN markers. I also independently reran all five retained control executables: each exits exactly1 with an explicit differential or retail-sequence comparison failure. Review logs and executable hashes are retained in the review scratch; no build/game process was started.

Coverage is **256 operands ×18 deterministic variants**, not a full Cartesian product of every sprite field. The variants include negative and extremal signed scales; helper factor0,1,0x3FF,0x400,0x7FFF,0xFFFF; position wrap boundaries; mirror bit2 versus bit3; full opcode high bits; and nine operand locations including aliases into relevant sprite fields. Every run initializes nonzero deterministic bytes and compares the entire fixture: leading0x40 guard, sprite0xC0, trailing0x40 guard and four external operand bytes. This catches unrelated stores and preservation errors, including sprite+0x64.

The helper spy compares exactly one call, normalized sprite pointer and input value. It is consequential: the missing-Q12-rounding control at case20 leaves the entire byte fixture equal (`diff=324`, the fixture size) but supplies helper input-1 instead of0; the call-input check still rejects it. This is stronger than merely checking final position pixels.

Controls reject unsigned operand, missing negative Q12 adjustment, wrong mirror bit, wrong position word and wrong helper input. Wrong-position/helper-input controls can fail the full retail-PC-sequence state comparison while the printed PC word itself is still correct; the marker means the **whole sequence comparison** failed, not that those mutations specifically changed stride. The final runner requires exact exit1 and an explicit failure marker, so compile errors, aborts, missing symbols and oracle errors cannot count as successful controls. Original scratch run.sh's weaker any-nonzero test is superseded by this durable check.

## Sanitizer qualification and acceptance

O0/O2 compile the actual production body/helper with GCC and link the fixture with Clang. UBSan first attempts an all-Clang body. The existing surrounding translation unit fails on undeclared func8001F6B0/memcpy/ScaleMatrixL and the forward declaration conflict for SpriteComputeTransformMatrix; exact errors are retained in UBSan.clang-build.log. GCC-only sanitizer linking also encountered the unavailable system libubsan. The final labeled exception uses **GCC-instrumented production/helper objects plus Clang-instrumented fixture/interpreter and Clang sanitizer linking**. Both body/helper builds use `-fsanitize=undefined -fno-sanitize-recover=all`; this is a mixed-toolchain UBSan PASS, not an all-Clang PASS. No adjacent source repair or disabled sanitizer assertion was introduced to make A9 green.

Accepted: bounded production A9 arithmetic, helper boundary, dispatch masking and preservation behavior under the raw-retail differential. Retail stride+2 is observed. Native outer PC execution, full animation-interpreter parity, C byte-exactness and natural battle/rendering completion are not established by this review and remain separately labeled.
