# W34N88 — natural mode-12 teardown

## Scope

- Starting HEAD: `71a30cb7ab7cefe08e98898f4a411162a13f7099`
- Branch: `experiment/worldmap-open-gates-20260823`
- Production implementation under test: the W34N87 retail mode-12 lifecycle
- Route: entrance/mode 12, GDB detached before `PcPort_WorldMapInitMain`
- Product frame limit: 1600, used only as a watchdog beyond the predicted retail
  exit; no timer, command, or mode state was shortened or forced after bootstrap.

W34N87 established the retail command table as
`[2,16,17,18,19,20,22,64]` with timers
`[600,60,8,255,180,270,75,0]`. That sequence predicts the terminal command at
displayed frame 1463. This rung tests the natural slot-2 lifecycle rather than
the earlier bounded 120-frame epilogue.

## Natural result

The detached product run progressed normally through frame 1440. The mode-12
teardown callback then executed exactly once:

```text
[w34n88-mode12-teardown] entry wds=0x639a20 F94E=0 F954=0 BBC4=0
[w34n88-mode12-teardown] exit F94E=273 F950=291 F954=2 BBC4=1
[worldmap-open-loop] natural state exit frames=1463 D7CC=0
```

This proves on the natural route that:

- the retail command/timer sequence reaches command 64 at exactly frame 1463;
- the slot-2 callback owns a live, non-null WDS allocation on entry;
- audio/SEDS and owned world resources survive until natural teardown;
- terminal state is published as `F94E=273`, `F954=2`, and `BBC4=1`;
- `F950=291` is copied from the live `BD3A` value;
- `D7CC=0` selects the field-state exit;
- control returns into `FieldMain` successfully rather than stopping at the
  artificial world-frame bound.

Both upload pumps continued to report `unknowns=0`, and no missing or invalid
scheduler callback boundary occurred before exit. The host process was stopped
only after the successful return had entered the unbounded field loop.

## Restoration and regression gate

The teardown witness was compile-time diagnostic instrumentation only. It was
removed after the run.

- `XENO_DIAG_W34N88_MODE12_TEARDOWN` residue: none
- W34N87 focused certificate: PASS at O0/O2/nonrecovering UBSan; M1-M8 detected
- normal port build after diagnostic removal: `LINK OK`
- tracked production diff from this rung: none

## Verdict

`NATURAL_TEARDOWN_PASS`

Mode 12 now has both a coherent, evolving 120-frame visual acceptance and a
separate natural 1463-frame lifecycle acceptance through retail setup,
recurrence, terminal command, slot-2 teardown, and field-state return.
