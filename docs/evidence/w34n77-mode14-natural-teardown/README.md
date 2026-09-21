# W34N77 — mode-14 natural teardown acceptance

## Anchor and scope

- Starting HEAD: `117058dc79b53fb21776ce1df9b84db1a422d875`
- Branch: `experiment/worldmap-open-gates-20260823`
- Production changes: none
- Route: mode 14, accepted scripted bootstrap, detached before
  `PcPort_WorldMapInitMain`
- Watchdog: 700 displayed frames

W34N76 accepted coherent mode-14 output through frame 120, but that bound
preceded the complete retail state/timer sequence and therefore did not prove
state 11, `D554/D7CC` termination, or lifecycle slot 2.  This rung lets the
unmodified sequence finish naturally.

## Natural result

The mode-14 state table at `0x8009A450` contains:

```text
1,2,3,8,4,5,6,8,9,7,10,11
```

Its timer table at `0x8009A46C` sums to 615 ticks; intervening action states
make the complete displayed-frame count larger.  No value was shortened or
forced.

The detached product run reported:

```text
[worldmap-open-loop] frame=660/700
[worldmap-open-loop] natural state exit frames=667 D7CC=0
```

At frame 667, the scripted callback had naturally entered state 11 and
published `D554=0` and `D7CC=0`.  The main loop invokes the current mode's slot
2 callback before printing the natural-state exit, so this line is downstream
of the linked `wm_8007A8AC` teardown returning.

Across world execution:

- every registered callback address resolved; no missing or invalid scheduler
  callback was reported;
- upload pumps reported zero unknowns;
- no world-loop error or bounded-exit line occurred;
- capture requests at every 60-frame boundary through frame 660 were fulfilled
  by the same numbered presentation;
- the application proceeded into the next field state after world teardown.

## Visual evidence

- frame 600:
  `scratchpad/w34n77_mode14_teardown_capture/world-frame-000600.bmp`
  SHA-256 `3dbb18854caa86dc666ebede9483959ec0df482c8007391b88cdf8a74899798d`
- frame 660:
  `scratchpad/w34n77_mode14_teardown_capture/world-frame-000660.bmp`
  SHA-256 `30c1e51a26547acacefbf7cc91812d06ec0bee457a58d60b347b4da6e1f14c90`

Frame 600 retains the textured moving horizon/terrain scene.  Frame 660 is
near-black with only faint scene remnants, matching the terminal fade before
the frame-667 exit.  The distinct hashes and visual transition are coherent
with the scripted sequence; this is not a retail pixel-parity claim.

## Verdict

`NATURAL_TEARDOWN_PASS`

Mode 14 now has production-owned setup and teardown, a completely resolved
registered scheduler surface, coherent evolving output, same-frame capture
fulfillment, natural state-machine termination, and lifecycle resource-close
coverage.  No mode-14 callback or lifecycle gap remains in the current port
inventory.

The next target should be selected from the remaining global world-map
frontier rather than reopening mode 14.
