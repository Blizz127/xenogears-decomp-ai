# W34C19 — F4 AudioManager split-brain witness

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `3eb08346`
- Date: 2026-08-28
- Verdict: **SPLIT_BRAIN_CONFIRMED_DORMANT**
- Production changes: none

## Anchor

W34C15's F4 source shape remains present:

- `world_map_init.c:2096-2181` creates/loads the AudioManager through native
  symbol `D_80062528`; the ready path assigns the newly created native pointer
  at line 2159.
- `world_map_callback_8e76c.c:427-432` reads the guest word at `0x80062528`
  only in slot-7 cb1's masked-variant-3 sound arm, then passes it to local
  no-op sound stubs.

## Natural-route witness

The accepted detached 120-frame route completed normally.  A temporary
diagnostic sampled both authorities and the complete outer/inner consumer
guards at frames 1, 60, and 120:

```text
frame  host D_80062528  guest 0x80062528  slot7 scheduler  slot+04  EE68   cb1  sound
1      0x887590         0x00000000        3                0        0000   no   no
60     0x887590         0x00000000        3                0        0000   no   no
120    0x887590         0x00000000        3                0        0000   no   no
```

The native manager is live and stable while the guest twin remains zero.  The
split-brain is therefore concrete on this route, not merely structural.

The bad consumer is dormant for two independently false predicates:

1. `world_map_scheduler.c:580-628` dispatches slot-7 cb1 only in scheduler
   state 1; the slot remains in dormant state 3.
2. Even after a future cb1 wake, `world_map_callback_8e76c.c:271-274,427-432`
   requires `(EE68 & 0x1FFF) == 3`; the accepted route holds `EE68 == 0`.

No sound-stub call consumed the stale guest twin during this run.

## Arming route

Slot 7 must first be claimed into scheduler state 1 through a transition such
as `wm_80097770` (`world_map_helper_97770.c:57-87`).  The sound arm then
requires a world-entry variant whose low 13 bits are 3.  Neither condition is
produced by the recurring directional input schedule.  This is entry/command
state, not ordinary per-frame traversal input.

The consumer itself is still a local no-op.  W34C15's queue decision remains
correct: resolve the `D_80062528` authority when that sound consumer is
retired, and re-witness both values on the live arm.  Mirroring or converting
the pointer now would be an unproven production choice.

## Verdict

**SPLIT_BRAIN_CONFIRMED_DORMANT** — native `D_80062528` is `0x887590` while
guest `0x80062528` is zero throughout the accepted route.  The only located
world callback consumer is unreachable because both its outer cb1-selection
and inner sound-state guards are false.

## Hygiene

The temporary diagnostic was removed.  The normal build completed with:

```text
LINK OK -> pc_port/build_native/xeno-port (port-owned addresses verified)
```

`git diff --check` passed and the tracked production tree returned to the
starting content before this evidence-only commit.
