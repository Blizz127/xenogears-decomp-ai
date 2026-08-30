# W34N23 — executable retail world-session trace harness

Verdict: **RETAIL_TRACE_RUNNER_VERIFIED**.

## Anchor and correction

The rung started at `c0abe5900889511a82ef59648cae4c7a9fc5b0f7` on
`experiment/worldmap-open-gates-20260823`, with local equal to origin and no
tracked worktree changes.

Two premises inherited from an older handoff were no longer true at this
anchor:

- W34N5 (`2445b559`) is the dedicated field-to-world WDS/SPU ownership rung.
  It frees the field owner, loads the valid world WDS, and publishes the same
  non-null owner to native and guest authorities.
- W34N10 (`bda938b1`) makes retail slot 1 (`0x80072238`) own the accepted
  first session.  Current W34N22 acceptance logs still show the fresh cleanup
  and a non-null world WDS result on that path.

W34N9's closing first-session warning was accurate at its own commit but had
become a stale handoff.  Its README now carries a later-status note without
rewriting the historical result.

A seeded retail oracle also existed at W34N5 (`66f100ee`), but it consisted of
Lua files plus an external savestate and an unrecorded launch procedure.  It
was not an executable, self-checking repository harness.  This rung closes
that operational gap.

## Runner

Tracked artifacts:

- `pc_port/tests/run_w34n23_retail_world_trace.sh`
- `pc_port/tests/w34n23_retail_world_trace.lua`

The runner:

1. verifies the exact PCSX-Redux AppImage, disc, BIOS, and external world-state
   hashes before launching;
2. clones the user's PCSX configuration into a temporary directory, enables
   debugger breakpoints, selects interpreter execution, and disables the
   unnecessary GDB server in that clone;
3. uses unique temporary memory cards and Xvfb;
4. launches PCSX in its own process group with a bounded watchdog and cleanup;
5. asserts the slot-1 target, slot-1 return state, driver-entry state, and
   every frame-head/back-edge state;
6. runs the replay twice and requires the normalized event traces to be
   byte-identical.

The configuration detail is load-bearing.  A pristine PCSX configuration has
`emulator.Debug.Debug=false`; the savestate loads, but Lua execution
breakpoints never fire.  Interpreter mode alone does not enable the oracle.
The GDB server is not required.

## Reproduced retail trace

Both fresh emulator runs produced exactly:

```text
W34N23_RETAIL STATE_LOADED
W34N23_RETAIL SLOT1_CALL target=80072238
W34N23_RETAIL SLOT1_RETURN D7CC=2
W34N23_RETAIL DRIVER_ENTRY D554=0 D7CC=2
W34N23_RETAIL FRAME_HEAD frame=1 D554=1 D7CC=2
W34N23_RETAIL FRAME_BRANCH frame=1 D554=1 taken=1
W34N23_RETAIL FRAME_HEAD frame=2 D554=1 D7CC=2
W34N23_RETAIL FRAME_BRANCH frame=2 D554=1 taken=1
W34N23_RETAIL FRAME_HEAD frame=3 D554=1 D7CC=2
W34N23_RETAIL FRAME_BRANCH frame=3 D554=1 taken=1
W34N23_RETAIL FRAME_HEAD frame=4 D554=1 D7CC=2
W34N23_RETAIL FRAME_BRANCH frame=4 D554=1 taken=1
W34N23_RETAIL FRAME_HEAD frame=5 D554=1 D7CC=2
W34N23_RETAIL FRAME_BRANCH frame=5 D554=1 taken=1
W34N23_RETAIL PASS frames=5 slot1=1 driver=1 slot2=0
```

The runner reported `REPEAT_TRACE_IDENTICAL=YES` and
`W34N23 RETAIL WORLD TRACE PASS`.

Current-head regressions were executed rather than inferred from older
records:

- W34N5 WDS lifecycle: O0/O2/nonrecovering-UBSan PASS, M1-M3 detected;
- W34C1 cadence/natural slot-1 ownership: O0/O2/nonrecovering-UBSan PASS,
  M1-M21 detected.

Two preflight negative controls also pass: a zero frame count is rejected with
status 2 before launch, and a state with the wrong hash is rejected with
status 2 before launch.  `bash -n` and `git diff --check` are clean, and the
process-group cleanup left no PCSX-Redux process behind.

Runtime provenance is unchanged from W34N5:

- PCSX-Redux build 293 / `3e10093a`, SHA-256
  `b27a564e6c32333453433c950ac26e652f40af32d5e236441f36e8123c9c651b`;
- `disc/disc1.bin`, SHA-256
  `39c547a9afc6da15d847ef81a2c6cea1a6516bdfa562cf13b0999b04e8598bda`;
- `disc/scph5500.bin`, SHA-256
  `11052b6499e466bbf0a709b1f9cb6834a9418e66680387912451e971cf8a1fef`;
- external world state, SHA-256
  `6ad6c512a85b78bdde866240ababa43a8534ad218d546a979cbe3f37e9925781`.

## Bound

This is now a reproducible structural trace harness, but its oracle remains
bounded to the seeded retail transition, one slot-1 invocation, one driver
entry, and five `D554 != 0` frame back-edges.  The savestate is a local
external asset and is neither tracked nor provisioned by the repository.

It does not establish a natural `D554 == 0` exit, slot 2, the post-slot-2
`D7CC` decision, a repeated session, or natural-playthrough provenance.  Those
events require a new retail state/trace extension; this runner must not be
cited as a complete lifecycle oracle.
