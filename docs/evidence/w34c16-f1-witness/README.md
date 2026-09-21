# W34C16 — F1 stack-vector stray-write witness

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `f38f7835d9899f0e12946d485c2f167e134a4282`
- Date: 2026-08-27
- Verdict: **ARM_NOT_REACHED**
- Production changes: none

## Question

Does the natural 120-frame forced-world route reach
`world_map_callback_914d0.c:246`, where native stack storage
`u32 delta_vec[3]` is truncated to `u32` and passed to
`wm_80093484`, a helper which rebases that value through `PSX_ADDR`?

The source defect established by W34C15 is unchanged.  This rung tests only
whether the guarded call executes on the accepted route; it does not force the
arm or repair it.

## Stack/ASLR determinism

The native binary is an ELF64 `ET_EXEC` executable, not PIE:

```text
Type: EXEC (Executable file)
Entry point: 0x404210
GNU_STACK: RW
```

Non-PIE fixes the executable mappings, not the stack by itself.  The host has
full ASLR enabled:

```text
/proc/sys/kernel/randomize_va_space = 2
```

The accepted detach harness starts the process under GDB.  GDB reports
`disable-randomization` as on, emitted no failure to apply that personality,
and two independent main-entry samples were identical:

```text
launch 1: RSP=0x7fffffffd0e0
launch 2: RSP=0x7fffffffd0e0
```

The inferior retains this no-randomization personality after GDB detaches at
`PcPort_WorldMapInitMain`.  Therefore a reached `delta_vec` alias should be
stable for this harness/build.  An ordinary native launch does not get that
protection: system ASLR remains enabled, so a stack-derived guest alias can
move between ordinary launches even though the executable is non-PIE.

## Run

The diagnostic build used the accepted world environment, detached before
`PcPort_WorldMapInitMain`, and ran exactly 120 bounded frames with:

```text
XENO_TEST_INPUT=0:0x2000,600:0x4000,916:0x2000,976:0
```

The temporary diagnostic recorded, once per `wm_800914D0` dispatch:

- capture frame and dispatch ordinal;
- slot-9 `+0x20` control state at callback entry;
- the post-main-dispatch control state used by the guarded block;
- the `(state == 3 || state == 0x10)` guard;
- the position mismatch predicate;
- whether the `wm_80093484` call was reached.

The bounded loop reached frames 60 and 120 and returned normally.

## Predicate result

```text
frames observed                    = 1..120, exactly once each
wm_800914D0 dispatches             = 120
entry control state 0              = 120/120
post-dispatch control state 0      = 120/120
state guard true                   = 0/120
position mismatch true             = 120/120
wm_80093484 guarded call reached   = 0/120
write-witness records              = 0
```

The first and last records were representative:

```text
frame=1   dispatch=1   entry_state=0 post_state=0 guard=0 mismatch=1 call=0
frame=120 dispatch=120 entry_state=0 post_state=0 guard=0 mismatch=1 call=0
```

The mismatch half of the predicate was continuously true.  The control state
was the sole blocker: it never left zero, so no native stack address was
truncated, no guest alias was formed at runtime, and `wm_80093484` performed no
guest read or write from this call site.

## Verdict

**ARM_NOT_REACHED** — F1 is a real source defect but is dormant on this exact
120-frame scripted route.  No prior evidence produced on the same route is
compromised by a stray write from this call site.

The source shows two bounded ways to reach the dangerous block:

1. A slot-9 subcommand value `15`, `16`, or `17` (`slot+0x04`, pre-dispatch
   cases 6/7/8) sets control state `0x10`; a position mismatch then reaches the
   call in the same callback.
2. Subcommand value `9` sets control state `2`; after its heading target is
   reached while `slot+0x60 == 0`, the callback transitions to state `3`, and a
   position mismatch reaches the call.

The current schedule issued neither state-changing subcommand and left the
slot in state zero for every observed dispatch.  A future route exercising the
slot-9 heading/transition commands is the proper natural witness.  This rung
did not inject those commands.

Because the call was never reached, the requested raw address, truncated
value, guest alias, before/after guest bytes, host-vector before/after words,
region classification, and within-run alias stability are all **not
applicable**, not missing evidence.

## Hygiene

The source diagnostic was removed.  A normal, non-diagnostic build was then
performed:

```text
LINK OK -> pc_port/build_native/xeno-port (port-owned addresses verified)
```

`git diff --check` passed and the tracked production tree returned to the
starting content before this evidence-only commit.
