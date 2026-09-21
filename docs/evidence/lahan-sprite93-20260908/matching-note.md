# Allocator exact-match check remains open

The C expression was adjusted to reproduce the retail v0 register sequence for the initial model load. The generated function is 84 bytes, and all instructions except one match after resolving its two call relocations to retail addresses.

At function offset 0x24, MASPSX emits `ori a0,zero,0x40` (`0x34040040`), while retail uses `addiu a0,zero,0x40` (`0x24040040`). The build currently uses MASPSX default immediate expansion. No global assembler option was changed. This is behaviorally equivalent but is **not** an exact compiled-byte match.

Run `python3 docs/evidence/lahan-sprite93-20260908/check-allocator-match.py` from the repository root to reproduce the failing byte gate. See allocator-match.json and matching-build.log. The updated regression run covers the new C expression and retains the allocator model-reload mutation control. The frozen live image predates this expression-only change.

A scratch-only assembly experiment with `--dont-expand-li` produces an84-byte function that matches retail exactly after the same two call relocations. See allocator-option-experiment.json and allocator-option-experiment.log. This identifies the remaining local difference as immediate-expansion policy. The repository build configuration remains unchanged: changing that policy across other functions requires a broader matching audit. The normal build gate still fails.
