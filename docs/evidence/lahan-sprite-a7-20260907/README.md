# Sprite A7 retail delay restoration

Shared dirty checkout HEAD3a3e7aac03a2f166fb924945a489e392d706f282. No commit or push. Scope: A7 in src/slus_006.64/system/animation_scripts.c, its new differential test/runner and one shared test-data symbol in the existing no-op fixture.

## Observed need and authority

The natural corrected-atlas mountain battle aborted on spriteA7. Prior evidence is ../lahan-atlas-work-buffer-20260907/README.md. Core-backed script bytes and retail battle800C1FC8..800C1FE0 prove that C8 deliberately resolved a variable inside SpriteData then requested A7. The operand pointer was not itself a defect.

Retail jtbl_800183D8[A7-8A] targets8001FDF0. Through8001FE60: a bit7-set operand replaces g_WorkListCurTimer with(low7+1). Otherwise the handler multiplies(operand+1) by((spriteAC>>7)&FFF), truncates division by256, substitutes1 for zero, and adds modulo16 bits to sprite9E. Both factors are nonnegative in this branch, so the unreachable negative-product rounding branch adds no alternative behavior. The C implementation follows these data accesses and preserves aliased operand reads before writes.

## Verification and limitations

The test executes the actual pinned retail dispatcher with no helper substitutions. Every operand byte and all4096 scale values are covered, followed by high opcode bits and operand aliases into sprite9D/9E/9F/AC/AD/AE/AF. Whole fixture bytes and the work-list timer are compared:1,075,456 cases per O0/O2/UBSan all pass. Initial counters are varied and wrapping is exercised; this does not claim exhaustive combinations of every possible input state. Pre-change RED reproduced the assertion. Mutants for zero-delay clamp omission, timer accumulation, narrowed scale mask and timer off-by-one are rejected. E6 and no-op tests plus their controls pass.

Tests ran on the host GCC/Clang setup used by the existing sprite runners. An initial container invocation lacked clang and did not run; the subsequent host RED and GREEN are the reported tests. Native and shared PSX builds succeed. Native executable SHA256 b8c372405638ce66f4b5eeb97d348e08477d87409da913e4e20b552d3ed8858b,74 function stubs. Full dispatcher compiled-byte matching is not established.

## Active continuation

A fresh normal-boot run uses this frozen executable at scratchpad/lahan-natural-20260907-a7/xeno-port. Execution session60981, game671427, Xvfb671423, display:1; process.json records the driver. It uses ordinary keyboard input and stops automatic input at the painting room. Revalidate processes before use. The mountain battle return, subsequent mountain/Lahan story, exact matching and audiovisual acceptance remain pending. Do not treat this run's launch as runtime validation.
