# Lahan sprite opcode C5

Natural run `scratchpad/lahan-natural-20260908-model-relocation` aborted during mountain battle 3 at 2026-09-08 09:36:11 CDT. Its core, screenshot, log and exact executable are preserved there. The native dispatcher rejected opcode 197 / index 59. Core operands begin 08 and D_80059198 is 0. Retail SLUS dispatch table maps C5 to 8001FCAC.

Implemented the missing 8001FCAC..8001FCF0 branch: sample unsigned operand byte and wrapped signed timing+1, divide, then call the existing motion integrator 80022CDC the quotient count. Negative quotient counting uses unsigned wrap; divide-by-zero preserves R3000 quotient -1 for a nonnegative numerator. These pathological billions-of-calls cases have not been exhaustively executed.

The boundary fixture executes the complete pinned retail dispatcher, bridging only 80022CDC to a mutation spy on both sides. It compares call count, argument identity, whole fixture/guards and global writes. All byte operands by timing 0..255, six signed denominator edges, separate/aliased operands: 134145 cases each at O0/O2/UBSan. Four semantic mutants rejected. Pre-fix unsupported-opcode failures captured at all three levels. An initial runner pin replacement typo and missing link guards were corrected before valid red results.

Native and full PSX builds pass. AB regression passes O0/O2/UBSan and six controls; BD regression passes its positive cases and controls. Initial AB invocation omitted its required GREEN mode; initial BD failed duplicate new guard linkage, corrected by retaining the shared guard only. Existing dispatcher fixtures receive a failing guard for the newly linked C5 dependency. No assertion bypass or successful no-op added.

Limits: this is dispatcher boundary equivalence, not proof of the motion integrator or complete animation dispatcher byte identity. The existing native 80022CDC owner in game_overrides.c follows the retail x/z/scaling/vertical order but retains signed arithmetic requiring its own boundary audit. Live C5 replay, night portraits, movie4 allocation repair and full start-to-end Lahan accuracy remain PENDING. No commit or push.

Follow-up: ../lahan-sprite-scale-20260908 records the native80022CAC low-word multiplication repair and917504oracle comparisons per optimization mode. Full80022CDC/80022B2C integration arithmetic remains unaudited. The CompMatrix replay later reachedaftermathfield3; see ../lahan-compmatrix-bridge-20260908/live-validation.json. Direct C5 execution coverage in that replay remains unproven.

Further follow-up: native80022CDC/80022B2C arithmetic has a bounded retail-instruction comparison and wrap repair in ../lahan-sprite-motion-20260908. Its floor callback is a fixture, so actual battle-floor integration and live C5 coverage remain unproven.
