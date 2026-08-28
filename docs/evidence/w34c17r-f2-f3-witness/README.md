# W34C17R — corrected F2/F3 stack-vector stray-write witness

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `e8c348fc9c196862da5f6ffbbc4cd746a528d54f`
- Starting origin: `e8c348fc9c196862da5f6ffbbc4cd746a528d54f`
- Date: 2026-08-28
- F2 verdict: **ARM_NOT_REACHED**
- F3 verdict: **ARM_NOT_REACHED**
- Production changes: none

This is the first executed F2/F3 witness.  It supersedes the rejected W34C17
task and does not import any guard-state or subcommand claim from the later
conversational summary, for which no run or commit exists.

## Anchors and actual arms

The two W34C15 sites still match source at the starting HEAD:

- F2: `pc_port/src/world_map_helper_8e0f0.c:28`, the single static call site
  passing native stack `u32 dir[3]` to `wm_80095414`.
- F3: `pc_port/src/world_map_helper_95cd4.c:72`, the single static call site
  passing native stack `u16 attr_buf[8]` to `wm_80084D00`.

Both helpers are called only from slot-7 cb1 `wm_8008E76C`:

- F2 is behind pre-dispatch `slot+0x04 == 7` at
  `world_map_callback_8e76c.c:259-261`.  If entered, the angle loop invokes the
  one F2 site at least once and at most 16 times.
- F3 is behind main dispatch
  `(lhu(0x8006EE68) & 0x1FFF) == 2` at
  `world_map_callback_8e76c.c:271-274,363-372`.  If entered, the one F3 site is
  reached once, absent an earlier fault.

There are therefore two static arms, one per finding.  The prior unexecuted
summary's two-F2-arm/F3-arm and states-4/5/6 description does not match these
sites.

## Run

The diagnostic build used the accepted world environment, in-process scripted
input, GDB detach before `PcPort_WorldMapInitMain`, and 120 bounded frames:

```text
XENO_TEST_INPUT=0:0x2000,600:0x4000,916:0x2000,976:0
```

W34C16 already established that the binary is non-PIE and that GDB disables
ASLR for this detach harness, making any stack-derived alias stable under the
harness.  That result was cited rather than repeated.

The run reached and fulfilled frames 60 and 120 and returned from the bounded
world loop.  Capture digests were:

```text
frame 60  26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3
frame 120 ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc
```

## Predicate result

The scheduler registered slot 7 with cb0/cb1
`0x8008E190/0x8008E76C`.  Its one initialization dispatch was:

```text
[worldmap-scheduler] slot=7 state=0 cb=0x8008e190 executed ret=3
```

`world_map_scheduler.c:573-628` stores that return in the scheduler state and
treats state 3 as dormant.  Consequently slot-7 cb1 `wm_8008E76C` was not
dispatched on any bounded frame:

```text
frames completed                 = 120
slot-7 cb0 initialization calls  = 1
slot-7 cb1 dispatches            = 0
F2 arm reaches                   = 0
F3 arm reaches                   = 0
F2 alias-witness records         = 0
F3 alias-witness records         = 0
```

Because the enclosing callback had zero dispatches, neither inner predicate
was dynamically evaluated.  Raw stack addresses, truncated values, aliases,
and before/after bytes are not applicable on this route.

## Arming-route analysis

The source establishes two outer requirements before either site can run:

1. Slot 7 must leave scheduler state 3 and enter state 1, because
   `world_map_scheduler.c:580-598` selects cb1 only for state 1.
2. The relevant inner guard in `wm_8008E76C` must then hold.

`wm_80097770` is the port's generic slot-claim transition: when `slot+0x04` is
zero it writes scheduler state 1 and writes its second argument to
`slot+0x04` (`world_map_helper_97770.c:57-87`).  One concrete slot-7 claim is
`wm_80097770(7, 0x0B)` at `world_map_callback_914d0.c:187-192`, after slot 9's
state-2 heading target is reached with `slot+0x60 == 0`.  That transition would
wake slot-7 cb1 with command `0x0B`, not with F2's command 7.

No port call that claims slot 7 with command 7 was found.  The only literal
value-7 claim is `wm_80097770(4, 7)` in
`world_map_callback_8d678.c:337-340`, which targets slot 4.  Thus no current
port-source transition to the F2 inner guard is established; a route that
adds/transcribes such a slot-7 command is required to witness it.

F3 additionally requires the low 13 bits of `0x8006EE68` to equal 2.  That
word is an entry variant read by both slot-7 callbacks
(`world_map_callback_8e190.c:284-299` and
`world_map_callback_8e76c.c:271-277`).  The implemented placement helper only
clears high flag bit `0x2000` (`world_map_helper_73448.c:33-43`); no recurring
ordinary-input transition that changes the low 13 bits to 2 was found.  A
world entry initialized with variant 2, plus a transition of slot 7 to
scheduler state 1, is the bounded natural route needed to witness F3.

These are source-derived limits.  The fabricated states 4/5/6 and subcommands
12/13/16/17/20 were not reproduced and are not evidence.

## Verdicts

- **F2: ARM_NOT_REACHED** — slot-7 cb1 never dispatched, so its
  `slot+0x04 == 7` arm and stack truncation did not execute.
- **F3: ARM_NOT_REACHED** — slot-7 cb1 never dispatched, so its masked-variant
  2 arm and stack truncation did not execute.

## Combined F1-F3 standing note

F1, F2, and F3 are real source-level host-stack/guest-address defects, but no
accepted-route write has been observed:

- W34C16: F1's slot-9 callback dispatched 120 times; its position mismatch was
  true 120/120, while its state guard was false 120/120.
- W34C17R: slot-7 cb1 dispatched 0 times, so F2 and F3 never reached their
  inner guards.

The first route that wakes slot 7, supplies its command/variant guards, or
drives slot 9 into F1's guarded state must treat unfamiliar guest corruption
as an F1-F3 candidate.  None of the three sites is repaired yet.  This combined
note supersedes the F1-only operational note in W34C16 without altering that
historical evidence.

## Hygiene

All `XENO_DIAG_W34C17R` instrumentation was removed.  The normal build then
completed with:

```text
LINK OK -> pc_port/build_native/xeno-port (port-owned addresses verified)
```

`git diff --check` passed and the tracked production tree returned to its
starting content before this evidence-only commit.  The completed diagnostic
inferior remained in the post-world placeholder after the bounded-loop return
and was terminated only after the frame-120 completion evidence had been
written.
