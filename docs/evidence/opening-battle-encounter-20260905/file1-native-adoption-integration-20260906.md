# File-1 native controller integration

The opening now executes the exact C reconstruction of the archive-20/file-1
controller through the production native battle bridge. This supersedes the
earlier guest-call prerequisite and disabled-adoption handoffs.

The loader must observe the ordinary archive selection and file load, then
verify the full 19,516-byte retail payload before initialization. The bridge
checks the immutable module prefix and generation before adopting controller
`801E6CE8`. A different/unproven module continues through the interpreter.
Packed globals are read afresh, pointer ranges are checked, and native
resident calls share the existing seven-to-eight-argument constructor bridge.
Five remaining guest helpers still execute through the guest-call service.
Selected-path failures propagate rather than retrying after partial writes.

Root verification:

- Exact C/controller and full-module check: PASS,
  `/tmp/xeno-file1-build-check-f9ph734c`. The declaration split preserves the
  original function body; the PSX controller remains 1,260 exact retail bytes.
- Original differential: 463 cases per O0/O2/Clang UBSan, all 315 instructions,
  both outcomes at 21 branches, all 11 controls rejected. Final pinned run:
  `/tmp/xeno-file1-controller-split-pinned-20260906`.
- Actual production adoption: 463 corpus cases plus 10 added cases, 4,319
  assertions per O0/O2/Clang UBSan mode, all 315 instructions/21 branches,
  all 10 compiled controls rejected. Root run:
  `/tmp/xeno-file1-adoption-root-final-20260906`.
- Existing guest-call and constructor ABI regressions also pass; their
  retained paths and exact scope are in the agent handoff beside this report.

The new binary `f59f7f87cb2b611d40441e54e973c7a295c4978f839920f31c0455b9b360c205`
was rebuilt and observed from ordinary New Game through the opening battle.
Read-only probes recorded 670 paired native controller entries/returns with
verified identity, stable generation 2, and the correct immutable prefix hash.
Root independently checked the pairs and inspected the capture showing Fei's
“Huff, huff... That's one down!?” dialogue. Ordinary input completed the battle
and reached `FieldMain` for field 14 at 06:27:13 UTC. The field-entry image is
still in its fade; map identity comes from the actual entry probe.

The owned run is
`pc_port/build_native/file1-adoption-opening-qqvyl5z1`. Its game, debugger and
Xvfb processes ended; the pre-existing application log was restored exactly.
The terminal SIGTERM was the intentional stop after field 14, not a crash.
Recording was disabled and audio used the null backend for this replay.

See the adjacent runtime report, root recheck, build pins, test pins and
control manifest. The later character-name menu correction was not in this
observed binary; its subsequent installed build is documented separately in
`docs/evidence/member-change-menu-20260906/`.

These gates prove this controller and bounded opening path. They do not prove
retail pixel identity, the complete helper graphs in C, later battles, the
forest route, or full-game decompilation. The full user goal remains active.
