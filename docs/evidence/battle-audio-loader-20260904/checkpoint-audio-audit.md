# Pre-battle checkpoint: audio dependency audit

## Scope and decision

2026-09-04: user delegated the retail-accurate implementation choice. A genuine
pre-battle checkpoint is selected. No save format or runtime state was changed
by this audit. No new visible game run was performed.

Branch: `experiment/worldmap-open-gates-20260823`.
HEAD: `3a3e7aac03a2f166fb924945a489e392d706f282`, shared dirty worktree.

## Confirmed source differences

The tracked `pc_port/patches/psycross_sound_prims.patch` supplies the current
`PsyCross/src/psx/LIBSPU.C` reverb implementation:

- `s_reverbPresets` contains OpenAL gain/decay/high-frequency presets, not the
  retail SPU coefficient and address register table.
- `PsyX_ApplyReverbState` takes the larger of left/right signed depth values,
  divides by 32767, clamps to 0..1 and sends one gain. Channel separation and
  negative depth values cannot survive that mapping.
- Delay and feedback setters store values and call that apply function, but
  it does not read either value. Round-trip getters do not prove audible effect.
- `PsyX_SPUAL_ApplyReverbParams` forwards those three floats to OpenAL EFX.
  OpenAL owns the running effect. No export/import of its processing history
  was found in these adapter sources. This is not proof that every possible
  external backend offers no such API; it is a missing capability here.

Existing decomp-owned references:

- `SpuSetReverbModeDepth.c` writes left/right signed halfwords separately to
  the SPU register image and retained attributes.
- `SpuSetReverbModeDelayTime.c` handles ECHO/DELAY only and updates six address
  registers from the selected preset and scaled delay.
- `SpuSetReverbModeFeedback.c` handles ECHO/DELAY only and writes the vWALL
  coefficient through `_spu_setReverbAttr`.
- `SpuSetReverbModeType.c` handles preset selection, work-area allocation,
  depth reset, register writes and optional work-area clear.
- Symbol map authority candidates: `_spu_rev_startaddr` at `80058E70`,
  `_spu_rev_param` at `80058EC0`, mode setter at `8004DD1C`, depth setter at
  `8004E574`. These labels were located, not independently byte-matched by
  this audit. Verify the disassembly/data before using them for a new backend.

This establishes a concrete approximation in the adapter. It does **not**
establish that the sequencer is fabricated, nor identify the cause of every
reported missing-music, lingering-fire or movie-audio symptom.

## Checkpoint consequence

Do not serialize reverb parameters and describe them as a complete sound
snapshot. Do not discard voice decoder/interpolation, ADSR, sequencer/task,
pending transfer or already-buffered output state. A replacement must first
have an explicit, testable hardware state model and sample-continuation tests.
No blind host-pointer relocation or field-save reload is authorized as a
substitute. Keep current playback available while the replacement is developed.

## Verification in this audit

- Particle OT tests: 35,720 cases each at O0/O2/UBSan; both wrong-offset and
  wrong-stride negative controls rejected. Shared matrix backend and controlled
  projection remain the test boundary, not hardware or visible parity proof.
- The sound primitive runner failed to compile with the host compiler's
  incompatible-pointer diagnostics in `sound.c`. No diagnostics were suppressed
  and no production sound code was edited to make this pass. The unchanged
  runner subsequently passed O0/O2/UBSan, native ownership and retail-byte
  checks inside `localhost/xenogears-dev-toolchain:current`. These tests cover
  allocation/transfer/CD mix, not the reverb implementation discussed above.

Source SHA-256 pins at inspection:

```
LIBSPU.C       790a947d147756153e463713dea261fb779e4f128cadfce1df9ef1b7a984d123
PsyX_SPUAL.cpp 3f79dddd73f1babfa64f6765db529f226845a61d49999c6bf3de055387ff0ef8
xeno-port     07a59ef228b7e3557a4395fca24680f0addb500dcd555e411d9386d40344d80b
```

Checkpoint implementation: NOT_IMPLEMENTED. Audio fidelity: NOT_PROVEN.
Village zoom and Gear battle visible acceptance: NOT_OBSERVED on this binary.
