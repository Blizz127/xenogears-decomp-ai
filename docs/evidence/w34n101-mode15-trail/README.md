# W34N101 — mode-15 five-slot trail callbacks

## Scope and retail anchor

- Starting HEAD: `d0aeb0ea2c64801c1c3513e8101fae934204f87f`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail image: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Disassembly: `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`
- initializer `[0x8007F8AC,0x8007F968)`, 188-byte SHA-256:
  `7340d6c133833a2f36ac3ebeaf2971119152dc239628ce27d93a8dfc86e1b544`
- update `[0x8007F968,0x8007FC8C)`, 804-byte SHA-256:
  `633a11cbaf8d636c831212113ecc00e28b73361ac329df28087ade7b4459f80c`
- complete pair `[0x8007F8AC,0x8007FC8C)`, 992-byte SHA-256:
  `27ca49dbec1b55e2790f98d62a710a86b3c8ff2cb0a92dfc0d845ab0815fb5b8`

## Production transcription

`wm_8007F8AC` restores the five slot-specific primitive streams, their
retail 84-byte owner stride, slot-8 blend-mode exception, velocities,
initial state/timer, and table-selected scale.

`wm_8007F968` restores all five latch cases, the primary-object following
path, marker effects 35/36, the timed fade path, fixed orientation and
per-frame spin, position and scale publication, and the terminal owner
release. The scheduler resolves both initializer and update addresses.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n101_mode15_trail.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- all three retail slice hashes: PASS
- per-slot owner/descriptor/stream selection: PASS
- slot-8 blend-mode exception: PASS
- latch states, timers, and effect resets: PASS
- primary-position following and marker offsets: PASS
- fade decrement and terminal owner release: PASS
- fixed matrix, position, spin, and scale publication: PASS
- symbolic scheduler initializer/update resolution: PASS
- M1 wrong owner stride: detected by `init.stream_owner`
- M2 wrong slot-8 blend mode: detected by `init.slot8_abr`
- M3 wrong initial timer: detected by `init.timer`
- M4 skipped latch-2 state: detected by `latch2.state`
- M5 wrong marker offset: detected by `follow.marker_position`
- M6 wrong fade step: detected by `fade.scale_step`
- M7 wrong fixed matrix: detected by `render.fixed_matrix`
- M8 wrong Z scale: detected by `render.scale_vector`

All M1-M8 were killed by named assertions. The W34N100 focused regression
passes, and the normal product build completes with `LINK OK`.

## Natural route

The detached entrance-15 route executed `0x8007F8AC` once for each of slots
4-8 (five calls) and `0x8007F968` on all 120 recurring passes for all five
slots (600 calls). Neither address produced a stub hit. The run fulfilled
both capture requests in-frame and returned from the bounded loop, with both
upload pumps at `unknowns=0`.

Frame 60 now contains the leading edge of the companion geometry, and frame
120 contains the complete repeated trail/effect sequence following the
slot-3 object. This is a material visual change from W34N100.

- frame 60 SHA-256:
  `c5e2e9a5bfcc8730bb613451d9be6c061ffdba223c1a4269aada8c23d2ee1cea`
- frame 120 SHA-256:
  `626cbd7af77846625ac866be7305b0b0c6d0053893eecefc1385901cb0e034f3`

The complete entrance-15 scheduler route now has zero unresolved callback
stub hits. Slot 9 (`0x8007FC8C`) and slot 10 (`0x80078948`) resolve through
already accepted implementations.

## Verdict and next target

`MODE15_TRAIL_RESTORED_NATURALLY_EXECUTED`

Next target selection must move beyond the entrance-15 callback census: the
route has no unresolved scheduler callback remaining. The next bounded rung
should either compare this mode's object presentation against retail or run
the same natural callback census on the next unresolved world-map mode.
