# W34N63 — retail mode-10 scaled-stream callback pair

## Scope and anchor

- Starting HEAD: `71400a97733041930e70afada784962b561e3c3a`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail source: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Retail unit: helper `0x8007A06C` plus callbacks
  `0x8007A144/0x8007A1B4`, ending at `0x8007A410`
- Slice SHA-256: `b6813757c0399adc9e19b760c1388de87eb4effa191fa69b9c28dfeb70f30707`

This is a bounded code-producing mode-10 slice. It does not integrate the
mode-10 lifecycle or alter any accepted route.

## Production behavior restored

`wm_8007A06C` initializes each 40-byte retail primitive record with the exact
code, RGB, texture-page, and CLUT values, then copies the complete formatted
stream from the owner source pointer to its destination pointer.

`wm_8007A144` clears the callback phase state and initializes both primitive
streams selected by the two context descriptors. `wm_8007A1B4`:

- copies the four-word live context position through both retail records;
- advances the primary phase by 192;
- advances and masks the secondary phase by 256 after the primary threshold;
- terminates at secondary phase `0x6000`, clearing the latch and both phases;
- copies the 32-byte retail base matrix into two scratch matrices;
- scales those matrices by `(primary,4096,primary)` and
  `(secondary,4096,secondary)` respectively; and
- publishes both complete 32-byte matrices to their context destinations.

The scheduler now resolves guest addresses `0x8007A144` and `0x8007A1B4`
directly to these bodies without a guest-function-pointer cast.

## Focused certificate

`pc_port/tests/run_w34n63_mode10_scaled_stream.sh`:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- helper arguments, every formatted field, stream copy direction/width,
  both descriptor streams, context copies, phase update/wrap/termination,
  scale-vector order, full matrix publications, and scheduler resolution:
  PASS
- M1 wrong primitive code: detected by `stream.primitive_code`
- M2 reversed stream copy: detected by `stream.copy_direction`
- M3 missing second stream: detected by `init.second_stream`
- M4 wrong primary phase step: detected by `update.primary_phase`
- M5 late terminal threshold: detected by `update.terminal`
- M6 swapped scale vectors: detected by `update.scale_order`
- M7 short matrix copy: detected by `update.matrix_copy`

The reverse-copy mutant initially exposed a weak equality-only test: copying
the wrong destination back over the source made both sides equally invalid.
The final certificate validates formatted destination bytes directly before
also checking byte equality.

## Regression and build gates

- W34N62 small mode-10 callback certificate, M1–M6: PASS
- W34N60 mode-9 callback certificate, M1–M5: PASS
- W34C1 scheduler cadence certificate, M1–M21: PASS
- canonical `./pc_port/build_port.sh`: LINK OK; port-owned addresses verified
- `git diff --check`: clean

## Remaining mode-10 callback frontier

Two mode-10-specific pairs remain unresolved:

- `0x80078E2C/0x80078EA4`
- `0x800795E4/0x80079778`

After those pairs land, the already-decoded `0x80078A60/0x80078D24`
setup/teardown lifecycle can be integrated and accepted naturally.
