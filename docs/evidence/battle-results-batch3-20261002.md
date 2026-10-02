# Battle results batch 3: runner validation, 2026-10-02

Source: `6e189169d3e88abe1c0ebb78a87341eaf0160b78`.
Runner: `20261002-140340-xeno-battle-results-batch3-6e189169-4116556`
on blizzpc, existing clang image, two CPUs inside the runner limits.
The source archive passed transfer SHA-256 readback:
`0ac5ab325f5feb26cdc9531dfb51561fad3d3852f08d77c02c8334b97de5c171`.

**Behavioral suites PASS; combined release gate FAIL.** The binary guard found
the same 19 retail-identical runs (25,068 bytes) as the preceding gate-7 build.
This is an unresolved release blocker, not a waived check. No package was
published and no existing owner build was changed.

| Check | Executed result |
|---|---|
| Matching prebuild, rom-check, host build | PASS |
| Store-return scan | PASS |
| Results differential | O0/O2/UBSan PASS: 1,530 cases, 516 controls, 15 leaves per mode |
| Results negative controls | Wrong body, raw guest mapping and wrong identity each rejected |
| CPU-fallback meter self-check | PASS in all three modes; `adopted_helpers=3`; no output while disabled |
| Overlay differential | PASS in all three modes: 3,811 checks, 888 inconclusive, 20 hand-only, 169 leaves; both mutation controls rejected |
| Leaf bridge | PASS: 40 checks per mode and always-interpret control |
| Guest calls | PASS: 204 checks per mode |
| File-1 adoption | PASS: 463 cases per mode |
| HD-2D OFF identity | PASS: both runs reach frame 3,000; 432 paired keys, zero mismatches, 16 ambiguous keys |
| Source retail-data guard | Real PASS: nonempty snapshot, 2,824 files scanned, 95 allowlisted runs |
| Strict Psy-Q guard | PASS |
| Retail binary guard | FAIL: 19 runs / 25,068 bytes; normalized reference locations and lengths identical to gate 7 |

The overall runner exit was **1**, with `port-guards rc=2`. Expected mutation
failures within passing test suites are preserved in the raw logs. The wrapper
also logged permission failures while removing its generated source tree after
the commands finished; those messages do not replace the recorded gate results.

Built executable SHA-256:
`38689c2cae53e058ebc4002a1efa9ec46ece5be56b1cdb399b437acc3693633a`.

## Evidence locations

Full private artifacts remain on blizzpc under
`~/devbox-runner/work/20261002-140340-xeno-battle-results-batch3-6e189169-4116556/out/`.
Thirteen small receipts were copied to the devbox with SHA-256 readback checks:
`~/release-staging/xeno-work/codex-handover-20261002/evidence/`.
`sha256-readback.json` records each receipt's digest; `gates.txt` records each exit
status. `batch3-gate-commands.sh` and `xeno_runner_handover.sh` in the parent
directory preserve the invocation. Scratch, config and state used runner disk.

The fixed snapshot guard required a nonempty index; this run indexed 2,824
files. This run does not rehabilitate older results produced by the broken
snapshot harness.

## Native-conversion milestone and limits

All 15 adopted results routines now have differential evidence. Approximately
48 other results routines, the intro module and battle code outside the native
allowlists still execute through the CPU interpreter. The production runtime is
**hybrid**. The force-interpret path is also used as the test oracle; the passing
leaf tests do not demonstrate interpreter-free gameplay.

The results prover compares exit registers, HI/LO, guest RAM, the mapped native
game-state copy and scratchpad. It does not compare access order or timing, and
kinds 101–111 remain excluded from its generated cases. Overlay inconclusive and
hand-only cases remain explicit coverage gaps.

The meter fixture observes 1,521 interpreted instructions and 10 interpreted
calls in the retail run; with adopted helpers it observes 256 interpreted
instructions, one interpreted call and nine native results calls. This fixture
does not cover nonzero main-exe/overlay/file-1 classifications, nested callback
accounting, table overflow, or meter-on/off full-state identity. Its call share
is not a time or instruction percentage; a faulting fetch can count one extra
instruction under the current bridge-verdict counting contract.

Still required: in-game results-window state identity and a route proving native
results execution, then a useful playable slice with the CPU interpreter
disabled/absent, zero fallback and semantic, visual, audio, timing and save
evidence. Broader native coverage follows that slice. Native callers must wait
for the documented guest-call/guest-frame contract and call-sequence recorder;
the existing unverified drafts are not adoption evidence.
