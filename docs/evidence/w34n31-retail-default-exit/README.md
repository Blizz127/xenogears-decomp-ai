# W34N31 — seeded retail signed-default terminal trace

Verdict: **SEEDED_DEFAULT_EXIT_ORACLE_AVAILABLE**.

## Anchor and authority

The rung started at `d0a54d3e` on
`experiment/worldmap-open-gates-20260823`, equal to origin.  It extends the
hashed W34N23 PCSX-Redux interpreter/disc/BIOS/savestate harness with a
`default_exit` profile.  Static instruction authority is
`disc/world_map.bin [0x80071264,0x800712A0)` plus the common epilogue at
`[0x800712A0,0x800712B8)`.

Tracked artifacts are:

- `pc_port/tests/w34n31_retail_default_exit.lua`;
- `pc_port/tests/run_w34n31_retail_default_exit.sh`;
- the `default_exit` dispatch and assertions in the shared W34N23 runner.

## Disclosed seeds

The profile uses W34N24's established frame-driver exit seed and then a
signed-default state seed before slot 2:

```text
PC 0x800719C0, frame 5: D554 1 -> 0
PC 0x8007109C, after driver return and before slot 2: D7CC 2 -> -1
```

No terminal call argument, rectangle word, color, or common-epilogue state is
seeded.  This is a structural oracle, not evidence of a natural producer for
negative D7CC.

## Repeatable retail observation

Two fresh interpreter processes produced byte-identical traces.  The tracked
trace payload has SHA-256
`bcbba883613404947edd08a3974580429f72707413a7577ed0bd5645414124d5`:

```text
W34N31_RETAIL DRIVER_EXIT D554=0 D7CC=2
W34N31_RETAIL SEED_D7CC before=2 after=-1
W34N31_RETAIL SLOT2_CALL target=8007299c D7CC=-1
W34N31_RETAIL SLOT2_RETURN D7CC=-1
W34N31_RETAIL TERMINAL_DECISION D7CC=-1 lane=default
W34N31_RETAIL CHANGE_STATE call=1 a0=0
W34N31_RETAIL CLEAR_IMAGE call=1 rect=0,0,319,431 rgb=0,0,64
W34N31_RETAIL DRAW_SYNC call=1 a0=0
W34N31_RETAIL COMMON_EPILOGUE BYTE591AE_BEFORE=01
W34N31_RETAIL SYNC_762FC call=1 BYTE591AE=00
W34N31_RETAIL MAIN_LOOP a0=0 frames=5
W34N31_RETAIL PASS frames=5 slot1=1 slot2=1 default=1
```

The DrawSync record above is specifically the direct call at retail PC
`0x80071298`.  The common `wm_800762FC` helper also performs synchronization;
the profile deliberately does not miscount those helper-internal calls as
additional default-lane calls.

This establishes that retail slot 2 receives and preserves signed D7CC `-1`,
then the default lane performs, in order:

1. `ChangeGameState(0)`;
2. `ClearImage` over `RECT {0,0,319,431}` with RGB `{0,0,64}`;
3. direct `DrawSync(0)`;
4. clear byte `0x800591AE`, call `0x800762FC`, then `MainLoop(0)`.

The frame-only, session-repeat, D7CC-zero, and D7CC-one retail profiles were
all rerun after extending the shared runner.  Each still passed twice with an
identical trace.

## Bound and next target

Negative D7CC was injected, so natural reachability and its writer remain
unproven.  The executable observation nevertheless closes the last dynamic
ambiguity in the base WorldMapMain terminal dispatch.  The next bounded
code-producing rung can integrate `[0x80071264,0x800712A0)` and dispatch it
only for signed D7CC values below two other than zero and one, while retaining
the existing common epilogue.
