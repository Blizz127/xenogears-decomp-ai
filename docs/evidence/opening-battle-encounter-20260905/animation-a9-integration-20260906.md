# Animation command A9: installed source and independent checks

The native sprite dispatcher now implements the relative-X command at retail
`80021698..800216F4`. This was the first unsupported command in the earlier
natural world-map-to-battle run `opening-window-fixed-1-bsbals5x`.

The implementation consumes a signed operand byte and signed Q12 scale,
rounds negative products toward zero, calls the existing retail-backed scale
helper, applies mirror bit 2, and adds the displacement to the packed X word
with 32-bit wrapping. The outer interpreter retains responsibility for the
two-byte script advance. Other commands and unsupported-command stops remain
as before.

## Independent production tests

Root ran `bash pc_port/tests/run_sprite_opcode_a9_retail_test.sh` successfully.
The retained output is
`pc_port/build_native/sprite_opcode_a9_retail_test.wkTIUnUV`; its provenance
manifest is copied beside this report. The full output is retained at
`/tmp/xeno-animation-a9-native-outer-root-test-20260906.log`.

- 4,608 cases per build mode compare the compiled dispatcher against the
  pinned retail instructions, covering all operand bytes and signed scaling,
  mirroring, wrapping, operand aliasing and opcode high-bit variants.
- The actual native `func_800248D4` also executes `[A9, 21, 86]`. Both retail
  and native execution advance the script by two bytes, stop at command 86
  with timer 1, and produce X `00210000`.
- The test links the actual scale-helper body from `game_overrides.c`.
  Unexpected outer-interpreter callees are guarded in the fixture.
- All five deliberately incorrect implementations compile and are rejected
  by semantic comparisons.
- O0 and O2 pass. UBSan passes with GCC-instrumented production/helper/outer
  objects and Clang's sanitizer runtime. Existing implicit declarations in
  the full production translation unit prevent an all-Clang build; this is
  a documented toolchain exception, not an all-Clang result.

Astra independently reviewed the arithmetic, ABI and preservation behavior.
Its earlier review's native-outer `NOT_RUN` entry is superseded by the root
test above. Natural gameplay and the actual later-battle caller remain
separate runtime gates.

## Build and runtime boundary

The native build after A9 and the text-table correction completed successfully:
`/tmp/xeno-exact-a9-texttable-native-build-20260906.log`.
Binary SHA-256:
`f4dcc43c404d453861429e80a017a5c4bda2ff355f85829d359276c274743ec6`.
It still contains generated stubs (79 function stubs and 566 data symbols);
linking does not establish complete decompilation or retail parity.

The preceding replay `later-battle-virtual-zf8ad3_8` used the pre-A9 binary
`59028f92...`. It completed the opening and returned to map 14, but did not
reach another battle before its owned 900-second observation limit. Its
debugger exit JSON records code 0; the enclosing tool session reported 143.
All three owned native/debugger/Xvfb processes were confirmed terminated.
This replay is not evidence that A9 executes successfully during gameplay.
A fresh copied-binary observation is tracked separately.

Retail hashes, reviewed source and integration pins are recorded in
`animation-a9-audit-20260906.md`, `animation-a9-review-20260906.md`,
`animation-a9-provenance-20260906.json` and
`exact-a9-texttable-build-pins-20260906.json`.
