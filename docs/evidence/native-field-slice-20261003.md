# Interpreter-disabled retail field route, 2026-10-03

This is continuation of task f5b072578f5f. No useful playable slice is accepted
yet. Matching remains 2962/4464 at source c10983d7; that is not native coverage.

## Entry and acceptance contract

Use the existing private disc, normal cold boot and raw controller input to
select New Game, advance prologue dialogue (map 4), and reach Lahan (map 2)
with the real party. Do not use FIELD_TEST, warps, synthetic RAM/save edits or
the empty-party battle-results diagnostic. Settings, memory cards and quicksave
paths belong to an isolated private profile. Preserve owner saves.

Required evidence: ordered title/New Game/prologue/Lahan transitions; meaningful
input response and nonempty party; interpreter-disabled guard; explicit zero
interpreted instructions, refusals and unresolved bridges for the accepted
slice; repeatable state and visual/audio/timing/save behavior against reference.
An exit summary without a useful route is not acceptance. A killed process or
missing summary is not evidence of zero fallback. Script VM execution is allowed.

## Retained executable discovery

Private evidence root:
`/home/blizz/release-staging/xeno-work/native-goal-85/playable-slice-20261003/`.
The retained production executable is from source **3a73eecc**, not current HEAD:
SHA256 `d3717bde1c0b2a7ae2c1a3b3891012d76e539f2bf6c148f0594c64fd25e4f6c9`.
It verifies disc SHA1 `12db8ccb93516c391630f046a143762337cc21f4`.
The old runner job failed its binary retail-data gate; this is private diagnostic
reuse, not a publishable build. No new shared-runner job was submitted.

Local discovery uses one allowed CPU, nice 19, Xvfb/software GL and null audio.
Null audio supplies no audio-parity evidence. The initial unreadable config
attempt exited 78; a readable isolated default config corrected the harness.
The configured 60-second run reached real title map 490, but ended before the
scheduled New Game buttons. It failed route acceptance.

The next run's first Circle at raw Vsync 3700 opened the menu immediately.
Circle at 4500 selected Continue before Up at 4900. It logged stub
`func_801D9F98`, then stayed at frames=1355, fieldLoop=829 for over 120 seconds.
SIGINT and SIGTERM did not exit it; only this owned probe was killed. The failed
trace and escalation receipt remain under `disabled-route-360/`. Its reviewed
capture shows the actual title menu. This is a concrete Continue-path limitation,
not an interpreter trap or an accepted playable slice.

A subsequent bounded 240-second run uses existing schedule overrides: early
Circle 1000, menu Circle 3700, Up 3900, confirm 4100, dialogue pulses from 4500.
The changed timing follows the observed real menu, without changing game state.
It selected New Game and entered prologue map 4, then reached its observation
deadline normally (runtime 124, driver 1). Final heartbeat: t=230s, frames=3063,
fieldLoop=2395, map=4. The 1,135 recorded events retain party `[0,2,255]`, gold
100 and scenario 0. No stub or unconverted-target marker appeared. A reviewed
capture shows the actual prologue text. The historical smoke assertions require
an attract/movie/menu ordering that this route does not take, and Lahan was
not reached. Its FAIL result is preserved.

A 600-second repeat with wider dialogue-pulse spacing again entered Continue:
the two-Vsync Up pulse is unreliable. It stalled at frames=1341, fieldLoop=740,
logged the same stub, and was stopped as a failed owned diagnostic. This rules
out treating the one successful fixed-frame selection as repeatable evidence.

The next driver uses the existing `xeno_drive.py` XTest helper on its own Xvfb
display, with both console and developer menu explicitly disabled. It waits
for real title state, sends ordinary 0.2-second keyboard presses, and captures
the menu before/after Up. The reviewed after-Up image shows New Game selected;
the log confirms choice 2 and prologue map 4. The first attempt at this driver imported the helper before
setting its binary override; it exited because the default build was absent.
Moving environment setup before import corrected that harness-only error.

Older route logs under private `warps/route1` establish a real map 4 -> 2
transition with scenario 0 -> 3, party `[0,2,255]`, gold 100, then map 14 and
party `[0,255,255]`. They help define expected outcomes, but their companion
quicksaves are not admitted fixtures: the retained `f14save.out` contains a
failed `/proc/.../fd/0` save attempt, and creation provenance is incomplete.
No such save has been loaded into these probes.

## Real route terminal result and first native dependency

The keyboard run reached map 2 at **396.36 seconds**, through a recorded
scenario 0 -> 3 transition, party `[0,2,255]`, gold 100, entrance 0. The reviewed
map-2 capture shows the real opening scene. At 480.19 seconds, an ordinary F11
press was correctly refused: `field script event holds control`, `ADB68=0`,
`ADB64=0xFF`, `B21D0=1`. No save was fabricated or guard relaxed. The local
single-CPU software renderer advanced at roughly two frames per second there;
this is not timing parity.

At **577.53 seconds** the natural opening battle entered **0x80070f40** and
the disabled-interpreter guard refused it. The adapter reported **0 executed
instructions**; the runtime aborted (return code -6). The raw battle meter
incorrectly printed one interpreted instruction/call because it counted the
bridge miss before the adapter guard. Preserve that raw receipt and its
qualification; do not report it as one instruction actually executed.

The meter correction separates `disabled_dispatches` from interpreted
instructions/calls and target rankings. Its regression uses the production
adapter and production meter include together: enabled execution counts three
synthetic instructions, disabled entry counts zero with one refused dispatch,
and a native bridge followed by refusal retains its native count. Clang O0,
O2 and UBSan pass. These are mechanism tests, not game-fixture tests. The
production executable still predates the fix and requires a coordinated build.

Battle entry is still an `INCLUDE_ASM` retail body, 522 assembly-file lines,
with 54 direct call sites / 29 distinct callees. Native adoption of the entry
alone would not establish native battle execution; every reachable unconverted
callee would remain a refusal. The next smaller field-proof route is to capture
a legitimate post-opening-battle save with the unchanged **hybrid reference**
runtime, then replay that bounded field slice with the interpreter disabled.
A separate one-CPU, nice-19 reference run has a 1,800-second deadline, uses
ordinary keyboard input, and will save only after map 14 reports free player
control. Its CPU fallback is intentional reference behavior, never native proof.
No reference save is admitted until the real route and successful save receipt
are retained. This also preserves the larger battle-conversion milestone.

## Process-wide execution counter instrumentation

`XENO_CPU_EXECUTION_STATS=1` initializes process-wide adapter counters at startup
and registers a normal-exit report, including when the adapter is never entered.
It counts adapter entries, serviced native bridges, interpreted instruction
attempts, disabled-interpreter refusals and unresolved bridges. It does not
change dispatch or remove any guard. The execution counter increments beside
`cpu->steps` after a successful fetch, before instruction execution; unsupported
instruction attempts therefore count too. This measures adapter execution,
not coverage or the presence of native stubs elsewhere. The existing battle-only
fallback meter remains separate.

The retained executable predates this instrumentation and cannot emit these
new counters. Production build/runtime verification is pending coordinator
sequencing of a single cached incremental runner build. No production counter
claim may be made from the mechanism tests.

Validation: extended interpreter-disabled mechanism test passes Clang O0, O2
and UBSan. Each checks enabled execution, refusal before memory fetch, native
bridge then refusal, unresolved bridge, exact aggregate counts, a zero-call
exit report, single report registration and opt-out behavior. An initial test
placed aggregate assertions before the last call; that failed and was corrected,
with both logs retained. Source port/data/Psy-Q guards pass; `git diff --check`
passes. These PC-only changes do not modify the retail translation units;
no new ROM census is claimed.

Local audio capture dependencies `ffmpeg`, `pactl` and `parec` are absent. A
read-only check confirms all three exist on the established blizzpc runner.
The supported remedy is a coordinated run there using an owned capture sink,
not new dependencies or alteration of the owner's default audio devices.
The recorder's synthetic-tone regression is mechanism evidence only and must
not substitute for the real game's audio comparison.

## Next step

Finish and audit the separately labelled reference-fixture capture. Integrate
both counter changes into one production build when capacity is coordinated,
then replay a legitimate useful field slice with the interpreter disabled.
Runner readback: CPU quota 450%, memory cap 3G, free-memory floor 2G, disk-free
floor 30G, footprint cap 32 GiB. Filesystem free space was 31G; readable runner
footprint was 32,128,236 KiB (a lower bound because some retained cache/temp
directories denied reads). Two audit jobs were active and the queue was empty.
No cleanup, cap change or new submission was performed; the coordinator has
the exact build/capture request and narrow disk margin. Retain the binary retail-data
blocker (19 runs / 25,068 bytes), all guards and separate native/runtime/matching
evidence. No publication is authorized by this work.
