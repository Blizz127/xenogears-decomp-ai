# Native-to-guest call prerequisite — frozen handoff

Accepted and unused by production dispatch. Added only `PcPort_BattleMipsCallGuest`, its internal contract header, and dedicated test/runner. Removing the new function and header include exactly recovers the prior runtime source; legacy public callbacks and all hard stops remain unchanged.

- Dedicated regression: `/tmp/xeno-guest-call-service-20260906/validated/` — O0/O2/all-Clang UBSan PASS,202 checks each; six compiled controls reject with exact exit1 plus assertion marker.
- Existing constructor ABI regression: `/tmp/xeno-guest-call-service-20260906/window-regression/` — O0/O2/UBSan PASS; all three existing semantic controls rejected.
- RED baseline: `/tmp/xeno-guest-call-service-20260906/red/O2.log` — explicit service-absent failure.
- Source pins: `frozen-source-pins.json`; isolated change: `runtime-service.diff`.
- Root independently accepted the suite and owns `docs/evidence/opening-battle-encounter-20260905/guest-call-service-integration-20260906.md` and native rebuild evidence. This lane did not build or launch the native game.

The internal contract rejects inactive/no bridge CPU, argc>7, missing argument arrays, invalid/unaligned/out-of-physical-RAM targets, and invalid/unaligned/underflowing initial stacks. It reserves the locally pinned retail0x50 frame; snapshots raw arguments before frame writes; inherits nonargument integer/GP/HI/LO/CP0 state with a fresh execution pipeline; preserves the parent CPU and restores bridge_cpu across success/failure/nested callbacks. Result storage occurs only on success; prior guest/frame writes are not rolled back. Existing callback zero-register initialization remains distinct. Bounds cover the initial service frame, not every callee frame.

The direct base/file1 helper assembly inspected does not itself reference GP, but this is not a transitive-call ABI guarantee. A GP-relative synthetic callee proves inheritance, and a compiled GP-drop control fails. Synthetic MIPS programs exercise the actual production runtime/interpreter; they are regression programs, not an adopted-controller or retail-helper equivalence claim.

Remaining adoption gates: loaded-module identity/generation; checked packed-global bindings and resident adapters; actual bridge-selected controller differential; matching/fallback preservation; then normal runtime evidence. No controller substitution, global target gate change, archive/build-script edit or module-identity implementation was added here.
