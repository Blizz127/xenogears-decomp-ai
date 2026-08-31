# W34N95 — natural mode-13 teardown

## Scope

- Starting HEAD: `cd439ab4d70a38f68c4124f91634665a6f4fb74d`
- Branch: `experiment/worldmap-open-gates-20260823`
- Production implementation under test: W34N93 lifecycle plus the W34N94
  shared mode-13 draw callback
- Route: entrance/mode 13, GDB detached before `PcPort_WorldMapInitMain`
- Product frame limit: 1000, used only as a watchdog beyond the predicted
  natural exit; no timer, command, or state was shortened or forced after
  bootstrap.

## Retail sequence prediction

The mode-13 command table is:

```text
[1, 2, 8, 3, 4, 5, 6, 7, 64]
```

with timer table:

```text
[135, 15, 135, 188, 2, 2, 120, 128, 0]
```

The initializer intentionally leaves the table index at zero, so command 1
and timer 135 are replayed once. The signed timer expires only after becoming
negative. Applying those exact rules predicts table loads at displayed frames
136, 272, 289, 426, 616, 620, 624, 746, and 876, followed by command 64 at
frame 877.

## Natural result

The detached product route progressed normally through frame 840. The
mode-13 slot-2 callback then executed exactly once at the predicted boundary:

```text
[w34n95-mode13-teardown] entry wds=0x638fc8 F94E=0 F954=0 BBC4=0
[w34n95-mode13-teardown] exit F94E=132 F950=736 F954=2 BBC4=1
[worldmap-open-loop] natural state exit frames=877 D7CC=0
```

This proves naturally that:

- the retail command/timer sequence reaches command 64 at frame 877;
- the slot-2 callback owns a live non-null WDS allocation on entry;
- audio/SEDS and owned world resources survive until teardown;
- teardown publishes `F94E=132`, `F954=2`, and `BBC4=1`;
- `F950=736` is copied from the live `BD3A` value;
- command 64 clears `D7CC=0`, selecting the field-state exit;
- control returns successfully into `FieldMain` rather than stopping at the
  artificial 1000-frame watchdog.

All six mode-13 scheduler records continued to resolve. Both upload pumps
reported `unknowns=0`; no missing/invalid scheduler boundary or adapter abort
was logged before natural exit. The host process was stopped only after the
successful return had entered the unbounded field loop.

## Restoration and regression gate

The teardown witness was compile-time diagnostic instrumentation only and was
removed after the run.

- `XENO_DIAG_W34N95_MODE13_TEARDOWN` residue: none
- W34N93 lifecycle certificate: PASS O0/O2/nonrecovering UBSan; M1-M8
  detected
- W34N94 shared-draw certificate: PASS O0/O2/nonrecovering UBSan; M1-M8
  detected
- normal isolated port build after diagnostic removal: `LINK OK`
- tracked production diff from this rung: none

Two attempted normal builds during this rung overlapped another agent's use of
the shared build directory and produced known transient retirement/link
signatures. The final result above is the isolated retry after the competing
builder exited, with no intervening production change.

## Verdict

`NATURAL_TEARDOWN_PASS`

Mode 13 now has a coherent evolving 120-frame visual acceptance and a separate
natural 877-frame lifecycle acceptance through retail setup, recurrence,
terminal command, slot-2 teardown, and field-state return.
