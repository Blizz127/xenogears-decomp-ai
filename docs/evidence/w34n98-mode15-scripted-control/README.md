# W34N98 — mode-15 scripted-control callback

## Scope and retail anchor

- Starting HEAD: `281d8a096b2c6aa36cb499b79dd472fd6e933119`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail image: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Disassembly: `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`
- initializer `[0x8007DE14,0x8007DE98)`, 132-byte SHA-256:
  `de90c2d19c4980b2e0f8804eb9a64f1bd34764050ae403cddd3174f2d88ea7fd`
- update `[0x8007DE98,0x8007E450)`, 1,464-byte SHA-256:
  `5d88d3981d5b63308ad174fbaee290844382ede697a9406ae118e6a6dce177c3`
- full pair `[0x8007DE14,0x8007E450)`, 1,596-byte SHA-256:
  `190f652180f4815f909b634f4fe3e874eda5b94f092c79ed6b7b9084e430072b`
- direct particle-release leaf `[0x80089514,0x80089580)`, 108-byte
  SHA-256:
  `6c1b35db616141b09065762f7dc2cc998494e0dfd03a336a30c7ad58a4553550`

## Production transcription

`wm_8007DE14` selects the command/timer stream pair at `0x8009A65C` using
the mode selector at `0x8009D3D4`, publishes both guest pointers into the
scheduler slot, seeds command and timer zero, and advances the stream index
to one.

`wm_8007DE98` implements the complete retail jump table for commands
`1-10`, `16-18`, `24-25`, and `61-64`. This includes signed timer expiry,
all ordered `wm_80097770` claims, fixed and selector-table sound triples,
`CCA4/D3CC` mode-state publication, and the command-64 `D554/D7CC` exit.
Unknown commands remain inert and the callback returns scheduler state 1.

The direct command-3 dependency `wm_80089514` walks all 256 particle
records at 0x4c-byte stride, matches one of eight member IDs in the requested
group, and clears the live halfword only when the retail secondary gate is
nonzero. The scheduler resolves both callback addresses symbolically.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n98_mode15_scripted_control.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- all four retail slice hashes: PASS
- initializer pointers, initial command/timer, and index: PASS
- signed timer expiry and next-entry fetch: PASS
- every implemented command family, ordered claims, sounds, globals, and
  exit state: PASS
- all-256-record particle release and unrelated-record preservation: PASS
- symbolic scheduler init/update resolution: PASS
- M1 wrong timer table: detected by `init.script_seed`
- M2 initial index left at zero: detected by `init.script_seed`
- M3 timer expires at zero: detected by `timer.zero_is_live`
- M4 wrong final command-2 claim: detected by `command2.sound_claims`
- M5 skipped particle clear: detected by `command3.particle_release`
- M6 wrong command-10 globals: detected by `command10.claims_state`
- M7 wrong selector sound stride: detected by `command61.sound_table`
- M8 skipped command-64 exit: detected by `command64.exit`

All M1-M8 were killed by named assertions. The W34N97 lifecycle regression
remains green, and the normal product build completed with `LINK OK`.

## Natural route

The identical detached entrance-15 run naturally executed `0x8007DE14` on
the first scheduler pass and `0x8007DE98` on each recurring pass. The first
pass improved from three to four executed records; the unresolved set is now:

```text
0x8007E450  slot 2, one hit per pass
0x8007ECA4  slot 3, one hit per pass
0x8007F8AC  slots 4-8, five hits per pass
```

The run reached frame 120, both capture requests were fulfilled in-frame,
the upload pumps remained at `unknowns=0`, and the bounded loop returned
normally. Frame 60 and 120 are bit-identical to W34N97, which is expected for
the naturally reached prefix: the first command holds for 90 ticks and the
next reached command is the command-62 sound triple, not a visual mutation.

- frame 60 SHA-256:
  `da5072d11def85a34c931bc34cd2a72039848f80bf772570bf101bd776097d89`
- frame 120 SHA-256:
  `96effaee2d5ea2ae45ae8439e972c1fe7424ae902a7a6609649c2f7c9d32e135`

## Verdict and next target

`SCRIPTED_CONTROL_RESTORED_NATURALLY_EXECUTED`

Next exact target: decode and restore the mode-15 camera/control pair
`0x8007E450 / 0x8007E4E4`, then rerun this same detached route.
