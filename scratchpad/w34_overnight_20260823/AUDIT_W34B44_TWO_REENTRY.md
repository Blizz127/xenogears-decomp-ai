# W34B44 — finite two-reentry diagnostic bound

## Scope

W34B43 showed the post-second-frame slot table was identical to the first and
all predicted cb1 callbacks remained covered. W34B44 extends the reviewed
gate from one additional frame to two additional frames. It remains a finite
diagnostic bound; it is not a claim that the retail session loop has been
implemented.

`XENO_WORLD_FRAME_REENTRY_TWICE=1` selects limit 2. The existing
`XENO_WORLD_FRAME_REENTRY_ONCE=1` behavior remains limit 1. Before each
additional call, the production predicate still requires a nonzero D554.
No should-not-run tripwire or guest-pointer boundary changed.

## Verification

- Build: `slice_17_build.log`, LINK OK.
- Predicate certificate: `slice_17_tests.log`, O0/O2/UBSan-O2 5/5 and
  three mutants detected.
- Natural: `slice_17_natural.log`, rc=0; re-entry counts 1 and 2 both
  observed; three frame-tail passes; three OT submissions; six packets;
  3075 OT-walk steps; all walks terminate at `0x8009CE6C`; zero adapter
  aborts; scheduler entry 4 with 53/53 callbacks and zero missing/invalid.

D554 remains 1 after the third frame. The mode-loop `0x80072238`,
post-loop/renderer entry `0x8007299C`, and separate world DrawOTag tripwire
remain zero-hit. The next safe task is a full third-tail slot census before
any further loop extension.
