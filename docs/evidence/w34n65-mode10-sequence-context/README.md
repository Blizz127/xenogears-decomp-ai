# W34N65 — retail mode-10 sequence/context callback pair

## Scope and anchor

- Starting HEAD: `81d210c718c2fb6eeff9e74b2544a032a8257efb`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail source: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Retail callback pair: `0x800795E4/0x80079778`, ending at `0x8007A06C`
- Slice SHA-256: `5c7badd535b7a5167f2ce758add4cfea97931bfd130cd1136e8e2d2e27affec7`

This is the final bounded mode-10 callback transcription. It does not yet
register the mode-10 lifecycle on a natural route.

## Production behavior restored

`wm_800795E4` restores retail's 13 context links, callback state, four-word
position copy, three phase/step pairs, world-position publication, context
angles `(-256, 0, -64)`, and packed sound-bank start.

`wm_80079778` restores the complete five-state sequence:

- state 0 publishes marker 11, applies the initial Y/Z movement, and enters
  state 1 at retail's signed Y threshold;
- state 1 publishes markers 11/12, expires marker 12's timer, advances the
  context angles, applies Y acceleration and Z movement, and enters state 2;
- state 2 publishes markers 11/12, lowers the angles, accelerates Z, clears
  marker 12, and claims scheduler slot 1 at the terminal Z threshold;
- state 3 continues the angle/Z motion until its eight-tick timer expires,
  then repeats the slot-1 claim and enters state 4; and
- state 4 consumes its latch, sets all fourteen context-record flags, and
  clears marker 11.

States 0 and 1 reuse the exact pre-motion scratch vector for their later
marker publication. This ordering is visible in retail and was preserved
explicitly instead of rebuilding the vector from moved coordinates.

The common tail wraps and publishes the four-word position, advances all
three 12-bit phases, builds four `RotMatrixYXZ` matrices, fans them out through
the complete retail context matrix chains, then builds and publishes the
context-angle view matrix. The scheduler resolves `0x800795E4/0x80079778`
directly to these bodies without a guest-function-pointer cast.

## Focused certificate

`pc_port/tests/run_w34n65_mode10_sequence_context.sh`:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- initializer/link order, all five states, signed thresholds, timer and
  acceleration behavior, marker/claim ordering, pre-motion vector reuse,
  wrap-before-publication, phase arithmetic, all matrix fan-out chains, final
  view matrix, and scheduler resolution: PASS
- M1 missing thirteenth context link: detected by `init.links`
- M2 wrong state-0 Y threshold: detected by `state0.transition`
- M3 wrong state-1 acceleration: detected by `state1.motion`
- M4 missing state-2 claim: detected by `state2.claim`
- M5 missing state-3 advance: detected by `state3.transition`
- M6 short state-4 flag fan-out: detected by `state4.flags`
- M7 skipped position wrap: detected by `tail.wrap_order`
- M8 wrong phase mask: detected by `tail.phases`
- M9 wrong matrix-D source: detected by `tail.matrix_d_chain`
- M10 missing final view-matrix copy: detected by `tail.view_matrix`
- M11 rebuilt second presence vector: detected by `state0.presence_reuse`

## Regression and build gates

- W34N64 camera/event callback certificate, M1–M9: PASS
- W34N63 scaled-stream callback certificate, M1–M7: PASS
- W34C1 scheduler cadence certificate, M1–M21: PASS
- canonical `./pc_port/build_port.sh`: LINK OK; port-owned addresses verified
- `git diff --check`: clean

The first canonical-build attempt overlapped another workspace build and its
shared object directory was rewritten during symbol retirement. The build was
rerun after that process exited and passed normally; no source workaround was
made.

## Next exact target

All mode-10-specific scheduler callback pairs are now port-owned. The next
slice is the already-decoded retail lifecycle at `0x80078A60/0x80078D24`:
integrate setup and teardown, register the eight callback pairs in retail
order, then run natural visual, deterministic-frame, and teardown acceptance.
