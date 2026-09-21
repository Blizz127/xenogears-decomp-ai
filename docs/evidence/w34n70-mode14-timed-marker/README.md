# W34N70 — mode-14 timed-marker callback pair

## Scope and retail anchor

This is the first code-producing slice for the remaining mode-table frontier
(modes 12–18).  It transcribes only retail `[0x8007BA08,0x8007BB60)`, the
shortest unresolved scheduler pair registered by mode-14 setup
`0x8007A5DC`.  It does not implement the mode-14 lifecycle or make mode 14 a
natural product route.

Authority:

- `disc/world_map.bin`, mapped at `0x8006FAF0`;
- `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`;
- exact 344-byte slice SHA-256:
  `3337dc702544590b29ae15106982f4dc9821332e1773ff19a5553cc36bdbfa4e`.

Retail mode-14 setup registers this pair at `0x8007A830..0x8007A840`:

- cb0 `0x8007BA08`: return scheduler state 3 without touching the slot;
- cb1 `0x8007BA10`: latch the shared reset position when `slot+4 == 1`,
  run a 60-tick marker trajectory, publish marker 9 through
  `wm_80089160(9, 0x1F8000A0, 0x8009A488)`, then restore the four-word
  reset record and clear marker 9 through `wm_800894C8` on expiry.

The fixed-point per-tick deltas are the retail words `0xFFFF6A30` on X and
`0x0002F130` on Z.  Scratch coordinates are the signed arithmetic `>> 12`
results stored as three halfwords.

## Production change

- Added `world_map_callback_7ba08.c/.h` with the exact bounded pair.
- Added explicit weak-symbol resolver entries in `world_map_scheduler.c`.
  Guest addresses remain data until the bounded resolver maps them to linked
  native bodies; no raw guest address is called.
- Added the source to the normal port build manifest.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n70_mode14_timed_marker.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- M1 wrong cb0 return: detected by `leaf.return`
- M2 wrong timer seed: detected by `latch.timer_seed`
- M3 missing X trajectory step: detected by `active.velocity`
- M4 wrong marker-data pointer: detected by `active.marker_arguments`
- M5 missing reset heading: detected by `expiry.reset_record`

The certificate also proves the latch-only three-word copy, heading
preservation on latch, active scratch vector, expiry four-word restore,
marker clear, and scheduler resolution of both guest addresses without a
missing or invalid callback.

Regression certificates remain green for W34N56 mode-8/11 callbacks, W34N58
shared mode draw, W34N60 mode-9 callbacks, and W34N62 mode-10 small
callbacks.  The normal port build reports `LINK OK`.

## Runtime bound

The final binary completed the detached accepted base-mode route through
displayed frames 60 and 120 and returned from the bounded loop:

- log: `scratchpad/w34n70_base.8jUVNp/run.log`
- frame 60:
  `9abc0ef97f43b5774857e691c91a45a3626fa76a4eeb85af9ad178c2b2507730`
- frame 120:
  `d0c102b6a5c2ebd33d545ea343bcc352786132709178d73bd0af9698a3c922b6`
- upload unknowns: 0
- generated world-overlay callback stops: 0
- invalid scheduler callbacks: 0
- primitive-link/OT adapter aborts: 0 observed

These hashes are recorded as artifacts, not promoted to a historical oracle:
the shared worktree contained unrelated field/boot changes during this run.
Mode 14 is not registered on the base route, so this acceptance establishes a
bounded live-route smoke result rather than natural execution of the new
pair.

## Next exact target

Continue mode 14 with the next smallest unresolved registered pair, preserving
one retail callback pair per code-producing rung until the complete lifecycle
can be integrated and naturally accepted.
