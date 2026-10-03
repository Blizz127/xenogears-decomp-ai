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
It has selected New Game and entered prologue map 4. Its terminal outcome must
be recorded separately; the historical smoke assertions also require an attract
movie branch that this direct menu route does not take.

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

## Next step

Finish the bounded retained route, preserve its exact outcome, and identify
the first real runtime blocker if it cannot reach the useful field slice.
Integrate the counter instrumentation into one production build when capacity
is coordinated, then repeat the legitimate route. Retain the binary retail-data
blocker (19 runs / 25,068 bytes), all guards and separate native/runtime/matching
evidence. No publication is authorized by this work.
