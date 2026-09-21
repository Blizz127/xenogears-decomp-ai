# W34N5 retail world-session trace oracle

Verdict: **ORACLE_AVAILABLE (seeded retail transition)**.

This checkpoint creates the retail dynamic oracle requested by W34N4.  A
PCSX-Redux savestate at the retail `WorldMapMain` session head was acquired,
then loaded in a fresh emulator process and replayed through five displayed
frame back-edges.

This is a structural control-flow oracle.  It is not represented as a natural
playthrough state: the field-to-world transition is initiated by one disclosed
write to retail's existing field-exit flag at retail's final exit-test seam.
Retail performs the field teardown, game-state change, world overlay entry,
slot dispatch, session setup, and every observed frame after that write.

## Repository anchor

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `2445b5594a8e6fa139e50e54acb5d5a6eff1cb32`
- Local matched `origin/experiment/worldmap-open-gates-20260823` before the run.

## Runtime provenance

- PCSX-Redux public build 293, commit `3e10093a`
- AppImage SHA-256:
  `b27a564e6c32333453433c950ac26e652f40af32d5e236441f36e8123c9c651b`
- CPU: interpreter
- BIOS file: `disc/scph5500.bin`, SHA-256
  `11052b6499e466bbf0a709b1f9cb6834a9418e66680387912451e971cf8a1fef`
- PCSX identifies that file as SCPH-7003 (US), BIOS hash `8d8cb7e4`.
- Disc: `disc/disc1.bin`, SHA-256
  `39c547a9afc6da15d847ef81a2c6cea1a6516bdfa562cf13b0999b04e8598bda`
- Source field state: `scratchpad/retail_field_entry_3e10093a_scph5500_f180.state`,
  SHA-256
  `a4c1bf0346cc9beb60c8fdbf2a310725ec05f50f6c47cfc10be3535fd6d77d84`

The state must be loaded after PCSX reports `ExecutionFlow::ShellReached`.
Loading it before that event is not valid; earlier attempts either remained
stopped or executed from an invalid low PC.

## Acquisition method and disclosed seed

The acquisition script is retained as `acquire.lua`.  After loading the field
state, it waits for the retail instruction at `0x8007856C`, immediately before
the `D_800ADBE4` load in FieldMain's final exit-code-1 test.  It writes only:

```
D_800ADBE4 (0x800ADBE4) = 0
```

The earlier attempt to write this at field-frame entry was invalid because
later field processing could overwrite it.  At `0x8007856C`, the write is
consumed by retail's own guard.  The observed transition was:

```
SEED pc=8007856c frame=33 ctx=1 D_ADBE4=0
WORLD_ENTRY fieldFrame=33
SLOT0_SELECT mode=1 D7CC=2
SHARED_CALL target=80071cdc
SESSION_DECISION D7CC=2
SESSION_HEAD mode=1 D7CC=2
SLOT1_CALL target=80072238
```

The acquired world state is external (savestates are not repository assets):

- Path: `scratchpad/retail_world_session_3e10093a.state`
- Size: 21,159,295 bytes
- SHA-256:
  `6ad6c512a85b78bdde866240ababa43a8534ad218d546a979cbe3f37e9925781`

It is captured in the breakpoint callback at `0x80071034`, before the session
head instruction continues into slot 1.

## Acquisition event trace

After the state capture, retail produced:

```
SLOT1_CALL session=1 target=80072238
SLOT1_RETURN_SCHED session=1 D7CC=2
DRIVER_ENTRY session=1 D554=0 D7CC=2
FRAME_HEAD frame=1 session=1 D554=1 D7CC=2
FRAME_BRANCH frame=1 D554=1 taken=1
FRAME_HEAD frame=2 session=1 D554=1 D7CC=2
FRAME_BRANCH frame=2 D554=1 taken=1
FRAME_HEAD frame=3 session=1 D554=1 D7CC=2
FRAME_BRANCH frame=3 D554=1 taken=1
FRAME_HEAD frame=4 session=1 D554=1 D7CC=2
FRAME_BRANCH frame=4 D554=1 taken=1
FRAME_HEAD frame=5 session=1 D554=1 D7CC=2
FRAME_BRANCH frame=5 D554=1 taken=1
SUMMARY frames=5 sessions=1 stateSaved=1
```

The full external log is
`scratchpad/w34n5_retail_world_state_exit2.log`, SHA-256
`1e0b81dac460c77dffb1fcf970ffed0bd7c05eb7a6c7e523afdfe014638c54fd`.

## Fresh-process replay

`replay.lua` loaded the saved world state in a new PCSX process.  It resumed at
the slot-1 call (the session-head breakpoint had already saved the state before
continuing), observed the same target `0x80072238`, the same driver entry
`D554=0 / D7CC=2`, and five frame heads/back-edges with `D554=1`.

```
STATE_LOADED
SLOT1_CALL target=80072238
SLOT1_RETURN_SCHED D7CC=2
DRIVER_ENTRY D554=0 D7CC=2
FRAME_HEAD frame=1 D554=1 D7CC=2
FRAME_BRANCH frame=1 D554=1 taken=1
...
FRAME_HEAD frame=5 D554=1 D7CC=2
FRAME_BRANCH frame=5 D554=1 taken=1
PASS frames=5
```

The full external replay log is
`scratchpad/w34n5_retail_world_state_replay.log`, SHA-256
`3992068258ee511522180b67320b44883992ea8143257b2b2e805ea5fa4605f7`.

## Oracle boundary

Established dynamically:

- base-world slot 0 resolves to `0x80071CDC`;
- mode 1 enters the session with signed `D7CC=2`;
- base-world slot 1 resolves to `0x80072238`;
- slot 1 returns before the driver begins;
- driver entry observes `D554=0`;
- displayed-frame head observes `D554=1`;
- `0x800719C8` takes the `D554 != 0` back-edge to `0x8007130C` on
  five consecutive frames;
- the captured state is reusable across fresh emulator processes.

Not established by this five-frame oracle:

- natural playthrough provenance before FieldMain's exit guard;
- a `D554 == 0` natural world-session exit;
- slot 2 (`0x8007299C`) execution or teardown;
- modes other than the observed mode 1;
- long-run input/state equivalence with the native forced route.

Those limitations do not prevent using this state to compare the retail slot-1
ordering and recurring-frame event trace.  They do prevent treating it as a
complete lifecycle oracle until a separate natural-exit trace is acquired.
