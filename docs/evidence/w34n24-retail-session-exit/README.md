# W34N24 — seeded retail session-exit trace

Verdict: **SEEDED_SESSION_LIFECYCLE_ORACLE_AVAILABLE**.

## Anchor and purpose

The rung started at `e2fd9b87b9596b2cdf74173b3de4755d38bb2262` on
`experiment/worldmap-open-gates-20260823`, with local equal to origin.

W34N23 made the existing five-frame retail replay reproducible, but it stopped
on five consecutive `D554 != 0` back-edges.  It did not observe the driver
epilogue, retail slot 2, the signed `D7CC` decision, or the next session head.
W34N24 adds those structural events to the same hashed retail state and
self-checking runner.

Tracked artifacts:

- `pc_port/tests/w34n24_retail_session_exit.lua`
- `pc_port/tests/run_w34n24_retail_session_exit.sh`
- the `session_exit` profile in
  `pc_port/tests/run_w34n23_retail_world_trace.sh`

## Sole disclosed seed

The oracle writes one retail state word:

```text
at PC 0x800719C0, after frame 5:
    D_8009D554: 1 -> 0
```

The breakpoint is immediately before retail's own `lw D_8009D554` at
`0x800719C0`.  Therefore the load, branch at `0x800719C8`, driver epilogue,
mode-table slot-2 selection/call, slot-2 body, signed session decision, and
next-session branch all execute unchanged in retail.

This is a seeded structural oracle.  It is not evidence that retail naturally
clears `D554` on frame 5, and it does not identify the natural writer that
ends a session.

## Reproduced trace

Two fresh PCSX-Redux interpreter processes produced byte-identical normalized
traces:

```text
W34N24_RETAIL READY
W34N24_RETAIL STATE_LOADED
W34N24_RETAIL SLOT1_CALL target=80072238
W34N24_RETAIL FRAME_HEAD frame=1 D554=1 D7CC=2
W34N24_RETAIL FRAME_BRANCH frame=1 D554=1 taken=1
W34N24_RETAIL FRAME_HEAD frame=2 D554=1 D7CC=2
W34N24_RETAIL FRAME_BRANCH frame=2 D554=1 taken=1
W34N24_RETAIL FRAME_HEAD frame=3 D554=1 D7CC=2
W34N24_RETAIL FRAME_BRANCH frame=3 D554=1 taken=1
W34N24_RETAIL FRAME_HEAD frame=4 D554=1 D7CC=2
W34N24_RETAIL FRAME_BRANCH frame=4 D554=1 taken=1
W34N24_RETAIL FRAME_HEAD frame=5 D554=1 D7CC=2
W34N24_RETAIL SEED_D554 frame=5 before=1 after=0
W34N24_RETAIL FRAME_BRANCH frame=5 D554=0 taken=0
W34N24_RETAIL DRIVER_EXIT D554=0 D7CC=2
W34N24_RETAIL SLOT2_CALL target=8007299c D7CC=2
W34N24_RETAIL SLOT2_RETURN D7CC=2
W34N24_RETAIL SESSION_DECISION D7CC=2 repeat=1
W34N24_RETAIL NEXT_SESSION_HEAD D7CC=2
W34N24_RETAIL PASS frames=5 slot1=1 slot2=1 repeat=1
```

This establishes the exact base-mode slot-2 target (`0x8007299C`), proves
that the observed teardown leaves `D7CC=2`, and proves the signed decision
repeats the session at `0x80071034`.

## Verification and bound

The session-exit profile checks every value above internally, then the shell
runner checks the normalized output and requires two identical runs.  The
original W34N23 five-frame profile was rerun after the profile extension and
still passes twice with identical output.  `bash -n`, `git diff --check`, and
process cleanup are clean.

The oracle still does not establish:

- a natural `D554` writer or natural session-exit cadence;
- teardown behavior after naturally evolved input/state;
- a `D7CC == 0`, `D7CC == 1`, or default terminal lane;
- a complete second slot-1 execution after the repeated-session head.

Those are separate dynamic states.  This rung supplies a bounded retail
lifecycle ordering oracle for native slot-2/session-repeat work without
claiming natural-exit acceptance.
