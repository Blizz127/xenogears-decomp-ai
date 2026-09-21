# W34N10 — natural first-session slot-1 integration

Verdict: **NATURAL_FIRST_SESSION_OWNER_VERIFIED**.

The accepted bounded world route no longer executes retail slot-1 setup as an
environment-implied ladder before entering the session loop.  It now performs
the pre-session slot-0 callback once, enters the session loop, dispatches the
mode table's real `0x80072238` slot-1 owner on the first session, runs the
session-entry scheduler, and then enters the displayed-frame recurrence.

## Anchor and authority

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `ba48526415bf29d99de59b2a4a75b6aab8f5e61c`
- Local matched origin before the change.
- Retail loop authority: `docs/evidence/w34n4-recurring-loop/PHASE1.md`.
- Slot-1 owner authority: `docs/evidence/w34n9-base-slot1-owner/README.md`.
- CD loader authority: `docs/evidence/w34c11-cd-loader/README.md`.

Retail order at the converged seam is:

```text
pre-session slot 0
  -> session slot 1 (0x80072238)
  -> session-entry scheduler (0x80097800)
  -> DrawSync / Vsync / controller reset
  -> frame/session driver (0x800712D0)
```

## Production change

`PcPort_WorldMapInitMain` now recognizes the bounded recurring-loop route
immediately after the common WorldMapMain pre-session initialization.  On that
path it calls the existing slot-0 dispatcher and transfers ownership to
`wm_80071034`; it does not enter the historical nested gate ladder.

`wm_80071034` now dispatches mode-table slot 1 and calls the outer scheduler
for every session, including session 1.  A nonzero result from the bounded
native owner fails the route loudly instead of continuing into frames with
partially initialized state.  Bounded exit still intentionally occurs before
the natural slot-2 teardown.

The old individual gate bodies and implication functions remain available as
diagnostic archaeology paths, but none select or execute the accepted route.
Thus W34N3's 24 early setup gates and 14 post-archive/scheduler gates are
**retired from the accepted route**, not deleted from source.

## Integration fault found and corrected

The first reduced-environment run reached the slot-1 owner but stopped in its
CD-work drain after logging `CD44 1->2`.  W34N9 had bound the owner to
`wm_800967E4_dispatch_cd_work`, the older partial asynchronous implementation
in `world_map_init.c`.  That implementation cannot complete state 2 on this
native path.

W34C11 had already supplied the authoritative production seam:
`wm_800967E4` in `world_map_helper_96130.c`, backed by the certified
synchronous native `wm_8009699C` loader.  The owner now calls that symbol at
the same retail sequence point.  The W34N9 certificate was updated to bind and
count this actual seam.  No loop threshold, artificial queue mutation, delay,
or gate was added.

The W34C11 runner had also accumulated an unprovided dependency on
`wm_80096668_circular_distance` through its linked production object.  Its
test fixture now supplies the exact modulo-16 head/tail calculation, allowing
the existing O0/O2/UBSan and M1-M9 certificate to run again; production code
was not changed for that compatibility repair.

## Structural certificates

The cadence certificate now seeds the real base-mode slot-1/slot-2 table
entries and calls `wm_80071034` without a manually injected outer scheduler.
It proves:

```text
session slot 1 calls:                 1
session-entry scheduler calls:        1
per-frame scheduler calls:          120
displayed frames:                   120
effective presentations:            120
bounded-exit slot-2 calls:             0
capture request/fulfillment:       60=60, 120=120
```

O0, O2, and nonrecovering UBSan pass with focused warnings clean.  Existing
M1-M8 remain detected, and new M9 removes the slot-1 dispatch and is detected
by `ASSERTION session.slot1.exactly_once`.

The final regression set also passed:

- W34N9 slot-1 owner: O0/O2/UBSan, strict warnings, M1-M6;
- W34C11 CD loader/wiring: O0/O2/UBSan, M1-M9;
- W34N8 loading transition: O0/O2/UBSan, M1-M6;
- W34N7 slot-2 teardown: O0/O2/UBSan, M1-M6.

Normal `./pc_port/build_port.sh` completed with:

```text
LINK OK -> pc_port/build_native/xeno-port (port-owned addresses verified)
```

## Reduced-environment native acceptance

The detached route contained only these world variables:

```text
XENO_WORLD_INIT=1
XENO_WORLD_OPEN_LOOP=1
XENO_WORLD_FRAME_LIMIT=120
```

All former setup, archive, framebuffer, terrain, convergence, common-tail,
and scheduler gates were absent.  The usual field bootstrap variables and
scripted input schedule remained unchanged.

The final run established:

- slot 0 executed once;
- `wm_80072238` began with `wm_80072BB0` and completed through common-tail P5;
- the world WDS loader returned non-null (`0x88c070`) and published it;
- ready-buffer, mode-audio, convergence P1/P2, and P0-P5 all executed;
- frame 60 and frame 120 capture requests fulfilled on the same frame;
- bounded exit reached `frames=120 limit=120`;
- no slot-1 failure, unsupported-restore diagnostic, or production error.

The resulting captures remain byte-identical to the standing images, which is
the expected equivalence result for replacing the same stage sequence with
its retail owner:

- frame 60:
  `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`
- frame 120:
  `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`

## Remaining boundaries

This closes first-session setup ownership.  It does not yet make the entire
world selector ungated: `XENO_WORLD_INIT` still selects the port-owned world
entry and `XENO_WORLD_OPEN_LOOP` still selects the bounded recurring harness.
The legacy per-stage gates remain source-visible for old focused harnesses.

The highest-value next code work is no longer slot-1 setup.  It is the active
frame driver's explicit local-stub list identified by W34N4, starting with
the already-transcribed helpers shadowed at `0x80071478`, `0x80071480`,
`0x8007197C`, `0x80071984`, and `0x8007198C`, followed by bounded
transcription of the genuinely absent conditional helpers.  Natural lifecycle
acceptance also still requires a route which drives `D554` to zero and proves
slot 2 plus the signed `D7CC` decision; later sessions additionally require
the three restore-entry helpers recorded by W34N9.
