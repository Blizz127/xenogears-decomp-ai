# Lahan fidelity continuation: sprite opcode E6

Scope: one missing resident sprite-script handler; not end-to-end Lahan acceptance.
Checkout: experiment/worldmap-open-gates-20260823 at
3a3e7aac03a2f166fb924945a489e392d706f282, with substantial pre-existing dirty work.
No active xeno-port, GDB, Claude, or port-build process was found at preflight.
No staging, commit, push, save edit, or forced gameplay transition performed.

## Retail authority and change

Retail SLUS_006.64 SHA256:
`dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119`.
Jump table 800183D8 index 5C points to 80020DCC. Instructions through
80020DE4 call 8001FBA4 to resolve a variable slot, load operand byte 1,
clear slot byte 1, and write the loaded value to slot byte 0 in the return
jump delay slot. The C transcription preserves operand-aliasing order.
The test runner rejects a different retail image before executing the oracle.

Only the 11-line E6 handler was added to the already-modified animation_scripts.c.
Added a dedicated retail-interpreter fixture and runner in pc_port/tests.

## Verification

- Before: focused fixture reached the unimplemented dispatcher assertion.
- After: 41,472 cases per optimization level at O0/O2/UBSan match the complete
  retail dispatcher and variable resolver, comparing the entire fixture.
- Includes all operand value bytes, high opcode bits, register and stack slots,
  and operands overlapping either destination byte.
- Three deliberate mutants rejected: missing clear, wrong high byte, and reading
  the operand after the clear. The latter fails specifically in the alias cases.
- Adjacent DF regression: 800 cases per level, pass.
- No-op regression and its four negative controls: pass.
- Full native rebuild in the existing toolchain image: LINK OK, 74 stubs.

## Limits and next work

This is bounded instruction-behavior evidence, not compiled PSX byte equality.
Fresh natural Lahan playthrough, E6 reachability in that playthrough, hardware-GPU
battle rendering, final Lahan story exit, and retail visual/audio comparisons
are NOT_RUN in this increment. Existing handlers still have unresolved paths,
including BC player/party ownership and its stale-register branch; no defaults
were invented for them. The earlier A9 runtime failure already has a current
source implementation, so it must be retested rather than blindly repaired.
The overall 100% goal remains active and incomplete.

Linked native SHA256: `0dbeef9709d75a675996e34723fd1fc7db886bbc9343bf20c2882e07c799d5b2`.
