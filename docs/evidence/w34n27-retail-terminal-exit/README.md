# W34N27 — seeded retail D7CC-zero terminal trace

Verdict: **SEEDED_GUARD_SKIP_TERMINAL_ORACLE_AVAILABLE**.

## Anchor and authority

The rung started at `7ada0d13` on
`experiment/worldmap-open-gates-20260823`, equal to origin.  Its executable
authority is the same hashed PCSX-Redux interpreter, disc, BIOS, and savestate
enforced by `run_w34n23_retail_world_trace.sh`:

- PCSX-Redux AppImage:
  `b27a564e6c32333453433c950ac26e652f40af32d5e236441f36e8123c9c651b`;
- disc image:
  `39c547a9afc6da15d847ef81a2c6cea1a6516bdfa562cf13b0999b04e8598bda`;
- BIOS:
  `11052b6499e466bbf0a709b1f9cb6834a9418e66680387912451e971cf8a1fef`;
- world-session state:
  `6ad6c512a85b78bdde866240ababa43a8534ad218d546a979cbe3f37e9925781`.

Static instruction authority is `disc/world_map.bin` at base `0x8006FAF0`,
especially the terminal interval `[0x800710D0,0x800712D0)` recorded in
W34N26.  Tracked executable artifacts are:

- `pc_port/tests/w34n27_retail_terminal_exit.lua`;
- `pc_port/tests/run_w34n27_retail_terminal_exit.sh`;
- the `terminal_exit` profile in the shared W34N23 runner.

## Disclosed state seeds

The trace first uses W34N24's established session-exit seed:

```text
PC 0x800719C0, frame 5: D554 1 -> 0
```

That leaves retail's driver exit unchanged.  At `0x8007109C`, after the
driver returns and before retail selects or invokes slot 2, the terminal
profile then writes:

```text
D7CC: 2 -> 0
BBC4: 0 -> 1
```

Seeding D7CC before slot 2 matters: a natural terminal state presents its
exit value to teardown.  Seeding it after slot 2 would combine teardown from
the repeat-session lane with a zero-lane decision.

BBC4 is a second necessary state seed for this particular replay.  The replay
has no current transition-region record: D7D8 is `0xFFFFFFFF` before and after
slot 2.  With only D7CC changed, retail reaches the `BBC4==0` arm and attempts
to read `D7D8+0xE`; that is an invalid state combination, not a usable oracle.
The profile therefore seeds BBC4 nonzero and exercises retail's explicit
guard-skip path.  This is a seeded structural oracle, not evidence of a
natural D7CC-zero cadence or natural BBC4 writer.

## Reproduced retail trace

Two fresh interpreter processes produced byte-identical normalized traces:

```text
W34N27_RETAIL SLOT1_CALL target=80072238
W34N27_RETAIL FRAME_HEAD frame=1 D554=1 D7CC=2
...
W34N27_RETAIL FRAME_HEAD frame=5 D554=1 D7CC=2
W34N27_RETAIL SEED_D554 frame=5 before=1 after=0
W34N27_RETAIL FRAME_BRANCH frame=5 D554=0 taken=0
W34N27_RETAIL DRIVER_EXIT D554=0 D7CC=2
W34N27_RETAIL SEED_TERMINAL D7CC_BEFORE=2 D7CC_AFTER=0 D7D8=ffffffff BBC4_BEFORE=0 BBC4_AFTER=1
W34N27_RETAIL SLOT2_CALL target=8007299c D7CC=0
W34N27_RETAIL SLOT2_RETURN D7CC=0 D7D8=ffffffff
W34N27_RETAIL TERMINAL_DECISION D7CC=0 lane=zero
W34N27_RETAIL LOAD_OVERLAY call=1 a0=1
W34N27_RETAIL CHANGE_STATE call=1 a0=1
W34N27_RETAIL REGION_GUARD BBC4=1 D7D8=ffffffff TYPE=SKIPPED helper_expected=0
W34N27_RETAIL REGION_OUTPUTS writes=0 F950=0c00 F94E=0400 F954=0001
W34N27_RETAIL EF68 BD0C=00000000 expected=0400 actual=0400
W34N27_RETAIL COMMON_EPILOGUE BYTE591AE_BEFORE=01
W34N27_RETAIL SYNC_762FC call=1 BYTE591AE=00
W34N27_RETAIL MAIN_LOOP a0=0 BYTE591AE=00 frames=5 helper_calls=0
W34N27_RETAIL PASS frames=5 slot1=1 slot2=1 terminal=0 helper_calls=0
```

This proves, on unmodified retail execution after the disclosed seeds:

1. slot 2 runs before the signed terminal decision and receives D7CC zero;
2. D7CC remains zero after slot 2;
3. `LoadGameStateOverlay(1)` precedes `ChangeGameState(1)`;
4. BBC4 nonzero skips both the D7D8 dereference and the three region-output
   stores;
5. `EF68` receives the low half of `BD0C + 0x400` (`0x0400` here);
6. byte `0x800591AE` changes from 1 to 0 before `0x800762FC`;
7. `0x800762FC` executes once before `MainLoop(0)`.

## Verification and limitation

The profile checks those values internally and the shared runner requires two
identical traces.  The original W34N23 `frames` profile and W34N24
`session_exit` profile were both rerun twice after this extension and remain
byte-identical and passing.  `bash -n` and `git diff --check` pass.

This state does **not** dynamically witness the BBC4-zero region arm, its
optional `wm_80094364` call, or its F950/F94E/F954 stores.  W34N26 supplies a
retail-image-anchored focused certificate for the helper, while an eventual
terminal integration certificate must cover both BBC4 branches synthetically.
No claim of a natural terminal exit, natural current-region state, or natural
terminal helper reachability is made here.

The next code-producing rung can integrate the D7CC-zero lane using the static
retail decode for both branches, W34N26 for helper semantics, and this dynamic
trace for lifecycle ordering and the guard-skip/common-epilogue path.
