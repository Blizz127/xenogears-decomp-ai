# W34N29 — seeded retail D7CC-one transition trace

Verdict: **SEEDED_TRANSITION_AUDIO_ORACLE_AVAILABLE**.

## Anchor and seeds

The rung started at `ea2e7d32` on
`experiment/worldmap-open-gates-20260823`, equal to origin.  It uses the
hashed W34N23 PCSX-Redux interpreter/disc/BIOS/savestate authority and adds a
`transition_exit` profile to the shared runner.

Two state writes are fully disclosed:

```text
PC 0x800719C0, frame 5: D554 1 -> 0
PC 0x8007109C, after driver return and before slot 2: D7CC 2 -> 1
```

The second seed is deliberately before slot 2 so retail teardown executes its
real D7CC-one snapshot branch.  No audio pointer, archive source, manager,
party tuple, or terminal output is seeded.  This is a structural transition
oracle, not evidence of a natural D554/D7CC writer or cadence.

Tracked artifacts are:

- `pc_port/tests/w34n29_retail_transition_exit.lua`;
- `pc_port/tests/run_w34n29_retail_transition_exit.sh`;
- the `transition_exit` profile in
  `pc_port/tests/run_w34n23_retail_world_trace.sh`.

## Repeated retail trace

Two fresh interpreter processes produced byte-identical normalized traces:

```text
W34N29_RETAIL DRIVER_EXIT D554=0 D7CC=2
W34N29_RETAIL SEED_D7CC before=2 after=1
W34N29_RETAIL SLOT2_CALL target=8007299c D7CC=1
W34N29_RETAIL SLOT2_RETURN D7CC=1 F954=8001 SNAPSHOT_HEAD=00000001
W34N29_RETAIL TERMINAL_DECISION D7CC=1 lane=transition
W34N29_RETAIL LOAD_OVERLAY call=1 a0=2
W34N29_RETAIL CHANGE_STATE call=1 a0=2
W34N29_RETAIL SOUND_CLEANUP call=1 BYTE594F8=00 EE70=0001 EE72=0001 EE74=0001
W34N29_RETAIL ALIGNED_SIZE call=1 a0=00000033 BCC8=00000033 C614=800faab8
W34N29_RETAIL ALIGNED_SIZE_RETURN size=7132
W34N29_RETAIL COPY call=1 dst=80062648 src=800faab8 size=7132
W34N29_RETAIL MANAGER_CREATE call=1 a0=80062648 OLD62528=80067210 SAVED4F2FC=80067210
W34N29_RETAIL MANAGER_CONFIGURE call=1 a0=80069320 a1=127 a2=0 NEW62528=80069320
W34N29_RETAIL COMMON_EPILOGUE BYTE591AE_BEFORE=01
W34N29_RETAIL SYNC_762FC call=1 BYTE591AE=00
W34N29_RETAIL MAIN_LOOP a0=0 frames=5
W34N29_RETAIL PASS frames=5 slot1=1 slot2=1 transition=1
```

This establishes that retail:

1. runs D7CC-one slot-2 snapshot/teardown before the terminal decision and
   preserves D7CC one;
2. ORs `0x8000` into F954 during snapshot teardown (`0x0001 -> 0x8001` here);
3. loads overlay 2, then changes to game state 2;
4. clears byte `0x800594F8` and expands F8E5's three bytes to EE70/72/74;
5. calls `0x80039CC4` before rebuilding the WDS/audio manager;
6. obtains aligned size 7132 from archive key `0x33`, copies that many bytes
   from `0x800FAAB8` to fixed buffer `0x80062648`;
7. saves old manager `0x80067210` to `0x8004F2FC`, creates manager
   `0x80069320`, publishes it to `0x80062528`, and configures `(127,0)`;
8. clears `0x800591AE`, calls `0x800762FC`, then calls `MainLoop(0)`.

## Bound and implementation target

All values above are live retail values under the disclosed D7CC seed.  The
trace does not establish natural transition timing, the producer of D7CC one,
or behavior for other archive/WDS contents.  Static instruction authority
remains `disc/world_map.bin [0x800711B0,0x80071264)` plus the common epilogue.

This is now a bounded code target: integrate the D7CC-one lane with the exact
native/guest authority decisions used by compiled port consumers, certify
snapshot-to-terminal ordering and all pointer publications, then retain the
existing bounded-route hashes as a dormant-path neutrality gate.

After adding the profile, the W34N23 frame-only, W34N24 session-repeat, and
W34N27 D7CC-zero profiles were each rerun in two fresh processes.  All three
still pass with byte-identical normalized traces.  `bash -n` and
`git diff --check` are clean.
