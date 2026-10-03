# Claude resumption checkpoint — owner-directed idle

Matt's directive relayed by coordinator Claude 0aca0712: "swap all to claude default".
Codex stopped new work and left the lane idle. No local game/test job remains
live. No runner job was submitted. Sole writer is this checkout,
`/home/blizz/dev/xenogears-decomp-codex-native`, branch
`codex/native-port-20261002`. The archerfish checkout is not the writer.
Task: `f5b072578f5f`. Continue the existing goal, not a new assignment.

## Unverified allocator candidate preserved, not admitted

The exact four-file draft is `drafts/8008abb8-native-wrapper-unverified.patch`,
SHA256 `a19679b1c50aaaf5372029e0dfbd724c15a7f844f997d9fcd77eb4fc8910118f`,
based on `6d07c7cd`. It was reverse-applied only after saving and checking it.
The checkpoint's runtime/allowlist therefore retain their previously admitted
state. To resume the candidate, review the patch and use `git apply --check`
then apply it in the sole writer. Do not treat it as verified adoption.

Opening battle entry 80070F40 first calls 8008ABB8 for sizes A2B4,10C,2F8.
The decompiled wrapper calls HeapChangeCurrentUser(2,0), then HeapAlloc.
Naive allowlisting leaks its host RAM pointer into guest v0: existing generic
return normalization applies only to HeapAlloc at 80031BDC. Tiny-leaf tests
share a host-returning stub and do not test this production boundary. The old
heap-pointer runtime test interpreted the wrapper. Draft extends that test to
link real main44.c and require zero interpreted steps for direct native calls,
adds a typed scalar/guest-return bridge, and stages a new negative control.

Verification is incomplete. Baseline O0/O2 rejects 18 interpreted steps;
candidate GCC O0 passes 162 cases (including real retail reservation records,
NULL/edge pointers, allocation requests). O2 stops at GCC15 Werror=clobbered
in unchanged results_try_host/setjmp index. Clang UBSan reports an existing
generic host-function ABI mismatch. No O2/UBSan/negative-control pass is claimed.
Earlier build errors and all logs are retained. The candidate also zeroes a
meter local to address GCC15 maybe-uninitialized diagnostics. Before integration,
check pending-load semantics and scalar/pointer-collision cases, finish all
regimes/negative controls and admission/source guards. Do not relax guards.

Local test dependencies already exist read-only in archerfish's
`pc_port/extern/PsyCross`; draft runner accepts PSYCROSS_ROOT and CC. Writer
has no extern tree. Local tests used `taskset -c 0 nice -n 19`, no new dependencies.
Generated test artifacts remain in writer `pc_port/build_native/` (ignored).

## Counter build remains HOLD

Private evidence root:
`/home/blizz/release-staging/xeno-work/native-goal-85/playable-slice-20261003/`.
`incremental-plan/PLAN.txt`, `GUARD-INPUTS-ADDENDUM.txt`, manifests and immutable
counter-only patch define the pending build: retained base3a73eecc plus four-file
counter patch, rebuilding three objects. NOT full current HEAD or the allocator
draft. Existing cache/job222154 and retained binary identities are in the plan.
Only coordinator may sequence/submit. No duplicate submission, cleanup, cap
change, publication or permission bypass.

Physical free space was acceptable per coordinator's latest message, but total
runner footprint remains unbounded: all three retained jobs' out/home/.cache
and out/tmp/xeno-file1-adoption.* directories are unreadable. No authoritative
size receipt found in existing logs/manifests. Exact six paths and audit are in
`incremental-plan/footprint-receipt-audit.json`. Need coordinator-authorized
measurement or an existing complete receipt; do not infer <=0.86GiB or bypass
permissions with unshare/sudo/chmod. Last coordinator thread f837a54dd590b863.

## Real field proof and limits

`docs/evidence/native-field-slice-20261003.md` is the detailed ledger.
Legitimate fixture was earned by normal keyboard coldboot/NewGame/opening battle
in hybrid reference, then ordinary accepted F11 in field14 free control.
Fixture SHA256: 0fa6e67662ef361831f509fed95a4d9c9c2c60e44f647c6dfe4f27beeee8936d.
No synthetic save, warp, empty-party diagnostic or control bypass was admitted.
Retained production binary is source3a73eecc, SHA256
 d3717bde1c0b2a7ae2c1a3b3891012d76e539f2bf6c148f0594c64fd25e4f6c9.
Disabled and reference field14 movement/F11/move/F12 exact-restore sequences
passed functionally. Whole-process counters are absent from this executable;
null audio and visual/timing gaps mean FULL ACCEPTED NATIVE SLICES STILL ZERO.
Opening battle still refuses 80070F40 with interpreter disabled.

Settled entry captures differ by113pixels. Equal wall-clock or vblank holds do
not guarantee equal field updates: FieldPollControllers drains/ORs the queued
samples. Existing field-step input injection occurs after masks, so cannot be
substituted as strict guarded acceptance unchanged. Both *frames-cpu1 runs
actually used 0x6000(Right+Down), not their original move-right label, due to an
unscoped text replacement. Raw receipts retained with input-correction.json;
corrected pureRight driver passed source preflight but has NOT been rerun.
Recent nice-only probes initially lacked CPU affinity; later pinning exposed a
disk-cache worker widening its affinity. Exceptions/repinning receipts retained.
Do not describe these as unqualified one-CPU runs.

Harness shutdown fix ea29c765 drains the reader before closing its log;
regression passes with the original-close negative control failing. Completed
game runs predate that fix. Frame driver now retains its script/hash and final
exit/guard checks for future runs. No process/session should be redispatched
from the old continuation live fields; all handles are terminal.

Matching remains2962/4464; tested admitted battle-body inventory102 unchanged.
Neither is native runtime percentage. Retail binary-data blocker19runs/25068bytes
unchanged. Preserve all matching drafts, source/ROM/data guards and receipts.
`claude-handoff-receipt.json` pins the six allocator logs and shelved patch.
