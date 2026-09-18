# Native controller prerequisite: calls into remaining retail code

`PcPort_BattleMipsCallGuest` is installed as an internal service in
`pc_port/src/battle_mips_runtime.c`. It has no production callers yet and does
not enable native file-1 controller substitution.

The exact controller needs calls to retail routines with zero through seven
arguments. The existing callback interface supports one argument and retains
its original behavior. This new service reserves the controller's missing
`0x50`-byte guest stack frame, passes raw argument words in `a0..a3` and
`SP+10/14/18`, and returns the guest result only after successful completion.
The frame size is pinned to the retail controller's first instruction.

The service requires an initialized active battle and current bridge CPU.
It rejects invalid argument counts, missing arguments, invalid or misaligned
targets, and stack addresses outside its supported RAM range. It snapshots
arguments before writing the outgoing frame, inherits the guest register
environment including GP, and starts a fresh instruction/load-delay pipeline.
Nested calls restore the parent CPU/context. Performed guest writes are not
rolled back on failure; callers must propagate failure. The callee's own
stack usage and loaded-module identity remain separate responsibilities.

## Root review and checks

Root reviewed the implementation and fixture, including the GP-relative data
access requirement. Removing the new service block and header include exactly
recovers the prior runtime SHA-256
`e671118805b557588947557075f77870e1ea4357b4a99f6d9b97ab6308d42097`.
Existing callback and bridge implementations are unchanged.
Astra also reran the existing constructor ABI regression at O0/O2/UBSan
with all three semantic controls rejected; its preserved handoff is
`guest-call-service-agent-handoff-20260906.md`.

`bash pc_port/tests/run_battle_guest_call_test.sh` exited 0. Root output:
`/tmp/xeno-guest-call-test.FyVYo29F`; full log:
`/tmp/xeno-guest-call-root-test-20260906.log`.

- 202 assertions pass at O0, O2 and all-Clang UBSan.
- The actual production service executes synthetic MIPS callees covering
  0..7 raw arguments, stack slots and frame guards, GP-relative loads,
  inherited registers, discarded pending load state, argument/result aliasing,
  invalid input, guest failure, and nested native/guest callbacks.
- Six deliberately incorrect implementations compile and fail semantic
  assertions: wrong frame, wrong stack slot, missing GP, missing context
  restoration, argument alias corruption and invalid-RAM masking.
- The unchanged callback path is exercised with its existing initialization
  semantics. These ABI tests do not decompile or validate every retail callee.

The native rebuild completed successfully; log:
`/tmp/xeno-guest-call-native-build-20260906.log`.
Binary SHA-256:
`1738d43da2e7d901416cbc4376e2045298d0fcd26b0de418a0b75f764e992cd3`.
This binary is build-verified only. The simultaneous natural-play observation
uses its separately copied earlier `f4dcc43c...` binary. The matching build
inputs are unaffected by this port-only service.

## Remaining controller adoption gates

The exact C controller remains inert in the native build. Adoption still
requires validated archive/module identity and invalidation, checked packed
RAM/global bindings, a shared retail-seven/native-eight constructor adapter,
and a differential covering the actual selected native controller path.
Only after those checks should normal opening gameplay be tested with the
native controller. Whole-module C decompilation, forest progression and full
game parity remain incomplete.
