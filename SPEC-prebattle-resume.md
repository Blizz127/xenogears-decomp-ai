# Spec: resume immediately before the opening Gear battle

Status: SELECTED — the user delegated the workflow choice and implementation
on 2026-09-04. Proceed with the genuine checkpoint, not controller replay.
Implementation and state-closure verification remain incomplete.
This is not an implemented checkpoint, a passing runtime gate, or a claim that
the opening battle already renders correctly.

## Objective

Let the user capture the natural New Game transition into the opening Gear
battle once, then resume that point without manually repeating the movies,
title menu and prologue text. Preserve the captured game state and sound; run
the actual battle entry and its normal return path. Keep ordinary boot and
the existing field-save file behavior unchanged.

The assumed useful scope includes compatible code-repair builds, not only
reloading the exact same executable. Unsupported layout changes must reject
the checkpoint before changing the running game. Requiring another opening
capture after every ordinary code-only repair would not solve the testing
workflow the user described.

## Current evidence and boundary

Repository: `experiment/worldmap-open-gates-20260823`, HEAD
`3a3e7aac03a2f166fb924945a489e392d706f282`, shared dirty worktree.
Executable SHA-256 verified when recording the workflow decision:
`07a59ef228b7e3557a4395fca24680f0addb500dcd555e411d9386d40344d80b`.

- `pc_port/src/quick_checkpoint_file.h` stores `0x2358` game-state bytes,
  map/entrance and the player transform. `quick_checkpoint.c` deliberately
  queues cutscene saves until free field control. It is not a cutscene snapshot.
- `src/slus_006.64/main/main_loop.c` performs the natural overlay load, graphics
  synchronization, heap reset and controller reset before dispatching the
  state owner. A restore must not manufacture this state by setting a map.
- State 2 dispatches `func_8001B6C4` in `system/temp3.c`. The proposed capture
  boundary is after its `func_8001B844()` setup, immediately before
  `func_80070F40()`. The wrapper's local `state` is assigned only after that
  call returns. An explicit continuation can therefore preserve the retail
  post-battle decisions without serializing the earlier native C stack.
  This is a proposed boundary, not a proven complete state-closure contract.
- The target is **not** an arbitrary point inside a field VM script. Previous
  field teardown and the exact live-state closure must be established at this
  boundary; do not expand this task into a universal mid-instruction savestate.
- `battle.bin` entry `80070F40` reads pre-existing main-executable state. In
  particular `80070FC8..80071000` copies the existing sound handle from
  `80062528` into battle state. Later sound-start/fade behavior is conditional.
  There is no established unconditional audio reset that permits dropping
  the captured sequencer/voice state.
- Live game state is not contained solely in `g_PsxRam`: an earlier inspected ELF places
  `g_GameState` at `009ADEA0`, the RNG seed at `00998FF8`, and task-list globals
  outside RAM as well. The RAM buffer itself moved from `005FD4E0` in run 08
  to `005FE4E0` in that build. These are historical observations, not current
  symbol pins. Copying old host-pointer words verbatim is not a
  cross-build restore strategy.
- `audio/PsyX_SPUAL.cpp` holds sample RAM, write position, 24 voices, envelope
  counters, ADPCM history, interpolation history and playback positions.
  `psx/LIBSPU.C` separately owns transfer and common/reverb state. No checkpoint
  export/import interface exists in these current sources.
- Audio uses a tick mutex, an SPU mutex and a stream mutex; the documented
  order is SPU mutex outside stream mutex, with no OpenAL calls while holding
  the stream mutex. Capturing only the game thread is not a consistent save.
- OpenAL owns reverb processing. Saving effect parameters does not establish
  that its in-flight tail is recoverable. This remains a feasibility gate;
  do not silently reset the tail or call parameter replay an exact snapshot.
- `archive_port.c` has host-only stream bookkeeping. CD synchronization at
  the proposed boundary must be observed, not assumed from the function name.

## Required behavior

1. Capture only when the real opening reaches the boundary. Identify the
   opening from observed retail state, not a chosen battle ID or story flag.
2. Capture the full live dependency closure consistently: game RAM and heap
   state, necessary external native game globals, RNG, live work lists,
   sound/sequencer/SPU state, graphics/GTE state, and live archive/event state.
   Exclusions need read-before-write or lifetime evidence, not a guessed list.
3. Encode relocatable references by explicit ownership/type. Do not scan all
   integers for values that merely resemble pointers. Do not save SDL/OpenAL
   resource handles or mutex objects as restorable game state.
4. Validate format, payload bounds/checksums, disc identity and state-layout
   compatibility before committing any restore. Reject incomplete, stale,
   corrupt or incompatible files without partially changing the game.
5. Resume through the actual battle entry and retail post-battle decision
   path. Keep the saved music/effect positions and script state; preserve
   normal input once execution resumes.
6. Keep this format separate from `quicksaves/quick.xgqs`. Never reinterpret
   an old field save as this checkpoint. Report unsupported save/load states
   clearly instead of displaying success.

## Project structure and style

- Game behavior remains under `src/`; native checkpoint ownership belongs
  under `pc_port/src/` with explicit hardware-boundary interfaces.
- Hardware changes follow the repository's tracked PsyCross patch workflow;
  no untracked-only vendor implementation or new external dependency.
- Focused C/C++ tests belong in `pc_port/tests/`; runtime logs and captures
  remain ignored local evidence. Record verification in
  `docs/evidence/battle-audio-loader-20260904/README.md` or a dedicated evidence
  directory. Do not place disc bytes or saved game payloads in Git.
- Follow the current fixed-width, explicit-endian save-code style, e.g. the
  existing encoder in `quick_checkpoint_file.c`:

  ```c
  static void put_u16(uint8_t* dst, uint16_t value)
  {
      dst[0] = (uint8_t)value;
      dst[1] = (uint8_t)(value >> 8);
  }
  ```

## Commands and verification

Existing canonical build command, after confirming no game is running:

```bash
podman run --rm --userns=keep-id --security-opt label=disable \
  -v /var/home/blizz/Projects/xenogears-decomp-ai:/var/home/blizz/Projects/xenogears-decomp-ai \
  -w /var/home/blizz/Projects/xenogears-decomp-ai \
  localhost/xenogears-dev-toolchain:current bash pc_port/build_port.sh
```

Existing regression commands (not proof that this new feature exists):

```bash
bash pc_port/tests/run_quick_checkpoint_file_test.sh
bash pc_port/tests/run_quick_checkpoint_request_test.sh
bash pc_port/tests/quick_checkpoint_runtime_regression_test.sh
bash pc_port/tests/run_battle_mips_adapter_test.sh
bash pc_port/tests/run_battle_graphics_abi_test.sh
bash pc_port/tests/run_battle_gte_retail_test.sh
```

New checkpoint-specific test commands must be specified with the approved
implementation plan; no such tests exist yet. Required coverage includes
codec corruption/truncation, missing state domains, unsupported pointer kinds,
compatibility rejection before mutation, audio continuation, capture/restore
atomicity, and repeated restores without leaked or duplicated tasks/resources.
Run focused native tests at O0/O2/UBSan where appropriate. Use actual captured
state for integration verification; synthetic fixtures prove only bounded
codec/adapter behavior.

Visible acceptance requires a natural capture, a fresh-process load, a load
after a compatible code-only repair build, and repeated returns through the
opening battle into normal gameplay. Record both video and sound. No headless
game run, field override, invented checkpoint, forced scene completion, or
automated pose setting can satisfy this acceptance gate.

## Boundaries

- Always preserve the shared dirty worktree, existing save/recording files,
  active user sessions, and the requirement for source-backed game behavior.
- Review this specification and the implementation plan before expanding
  Save/Load or the hardware-state interfaces. Ask before substituting replay
  or undertaking a new audio-backend architecture to solve the reverb gate.
- Never claim a partial state dump, field reload, or ordinary input schedule
  is a genuine pre-battle checkpoint. Never weaken the existing cutscene safety
  gate to make Save appear to work. No commits or pushes are authorized.

## Selected workflow and next dependency

The real checkpoint is selected. Audio preservation and cross-build relocation
are engineering gates, not an unanswered user choice. Do not ask the user to
select checkpoint versus replay again.

The audio inspection additionally found that the current reverb adapter is
not a retail implementation: `psycross_sound_prims.patch` defines floating-point
OpenAL presets, reduces two signed depth channels to one clamped scalar, and
does not consume the stored delay/feedback in `PsyX_ApplyReverbState`.
Existing decomp-owned routines under `src/slus_006.64/psyq/libspu/` provide the
reference for register writes; preserving an OpenAL approximation is not proof
of retail audio fidelity. See the audio dependency audit linked below.

Next: establish the retail register/preset authority and explicit hardware
state ownership before implementing audio serialization. Keep the existing
audible backend unchanged until a tested replacement exists; neither muting
reverb nor restarting its tail satisfies this contract.

Evidence: `docs/evidence/battle-audio-loader-20260904/checkpoint-audio-audit.md`.

A separate temporary option is **fully visible recorded-controller replay**:
one normal user-driven capture, then automatic repetition of those inputs
through ordinary boot/New Game until control is handed back before battle.
This avoids repeated manual input but still plays the opening, is not instant,
requires divergence detection, and is not a savestate or a new visual parity
proof. The existing synthetic `XENO_TEST_INPUT` schedules are not a recording.
This alternative was not selected and must not silently replace the checkpoint.
