# Harness regimes: O0 / O2 / UBSan — where they run, and what past claims mean

Date: 2026-09-21 UTC.

## Read this before trusting any "harness green at O0/O2/UBSan" claim

`PORT_GOAL_AND_PLAN.md` names the differential harness at **O0/O2/UBSan** as
the verification bar that justifies *not* byte-matching. It is the thing this
project trades byte parity away for.

**On this machine's bare host, the UBSan regime has never executed.** Many
evidence documents under `docs/evidence/` record "harness green at
O0/O2/UBSan". Where those runs were done on the host — which is where the
harness is normally invoked — **the UBSan third of that claim did not happen**.
Treat historical O0/O2/UBSan claims as O0/O2 unless the document says
explicitly that it ran in the container.

This is not a case of scripts lying. Every runner uses `set -e`, so a failed
UBSan link aborted the run with a nonzero exit; no runner ever printed a false
PASS. The failure was in **summarisation**: the link error arrived *late*, after
the O0 and O2 regimes had already printed their PASS lines, and read as a
build-environment annoyance rather than a missing third of the gate. Three
separate workers in one session recorded it that way.

## The facts

| environment | UBSan | detail |
|---|---|---|
| bare host (Fedora 44) | **does not work** | gcc 16.2.1 ships `/usr/lib/gcc/x86_64-redhat-linux/16/libubsan.so`, but that file is a linker script reading `INPUT ( /usr/lib64/libubsan.so.1.0.0 )` and that runtime is absent — the `libubsan` RPM is not installed. There is no static runtime either (`-static-libubsan` → *"have you installed the static version of the ubsan library ?"*). |
| podman toolchain container | **works** | Ubuntu 24.04, gcc 13.3.0, `libubsan1 14.2.0` present. `gcc -fsanitize=undefined -fno-sanitize-recover=all` compiles, links and runs. |

So UBSan was never actually unavailable to this project. **The harness was
being run in the wrong place.** Run it in the container.

To fix the host instead: `sudo dnf install libubsan`.

## How to run it

Inside the podman toolchain container, from the project root:

```
./pc_port/tests/run_ubsan_sweep.sh          # all suites
./pc_port/tests/run_ubsan_sweep.sh 120 60   # first 120 suites, 60s each
```

The sweep classifies every suite into `PASS` (UBSan genuinely ran), `NO-UBSAN`
(the suite has no UBSan regime at all), `UBSAN-NOT-RUN`, `FAIL` or `TIMEOUT`,
writes `pc_port/build_native/ubsweep/_report.tsv`, and exits nonzero if
anything could not run. Individual runners work too — they are unchanged apart
from the preflight.

## What the preflight does

`pc_port/tests/lib/sanitizer_preflight.sh` is sourced by all **519** runners
that have a UBSan regime. Before any regime executes, it probes that
`-fsanitize=undefined` compiles, links *and runs*; if not it prints

```
UBSAN NOT RUN -- THIS IS NOT A PASS -- suite=<name> cc=<cc> (details on stderr)
```

on stdout, a full banner on stderr naming the reason and this file, and exits
**97** — an exit code reserved for "a required sanitizer regime could not run",
so a sweep can tell it apart from a genuine test failure.

This does not weaken any check. The exit was already nonzero; the preflight
only makes the reason legible and moves it in front of the PASS lines that
used to precede it.

## Coverage, measured

- **566** runner scripts in `pc_port/tests/`.
- **519** have a UBSan regime and now preflight.
- **45** have **no sanitizer regime at all** — they are O0/O2-only suites. A
  suite in this group passing is not evidence about UBSan either way.
  `run_portrait_render_regression_test.sh` is one of them.

## First real UBSan results on this machine

Sweep of a 120-suite sample in the container, 2026-09-21:

```
ran=120  pass=39  fail=81  timeout=0  ubsan_notrun=0
of the 39 passing, 34 have a genuine UBSan regime
UBSan runtime errors: NONE
```

**34 suites executed a real UBSan regime for the first time here and found
zero undefined behaviour.** That is a clean negative result, not a null one —
but it covers only the suites that build in the container.

## Two container gaps that block a full sweep

Both are missing packages in `localhost/xenogears-dev-toolchain:krom-20260913`.
Neither can be fixed from inside this worktree; the image needs rebuilding.

1. **`clang` / `clang++` are absent.** 54 of the 81 sweep failures are nothing
   but `clang: command not found`. A large number of suites deliberately use
   clang for their UBSan regime (e.g.
   `run_battle_return_state_retail_test.sh` sets `cc=clang` for UBSan), so
   those regimes still have not run anywhere. **This is the single biggest
   remaining hole in the O0/O2/UBSan bar.**
2. **`ripgrep` is absent**, and the runners use `rg` as their assertion
   matcher. Worked around by `pc_port/tests/lib/compat/rg`, a documented
   grep shim the sweep puts on `PATH` only when real `rg` is missing. It maps
   the flags the harness actually uses (`-q -v -m -A -n -c -i -F -l`) onto
   `grep -E`. It is a stand-in, not ripgrep; prefer installing the real thing.

**What the user needs to do to close this:** add `clang` and `ripgrep` to the
toolchain image (Ubuntu 24.04: `apt-get install -y clang ripgrep`) and rebuild
it. With those two packages the sweep should be able to attempt every suite.

The remaining ~27 failures are compiler-version differences between the
container's gcc 13 and the host's gcc 16 — 7 suites pass `-fpermissive` with
`-Werror` (rejected for C by gcc 13), 9 hit undeclared identifiers, 2 want a
generated `battle_bridge_map.inc`, 2 trip `-Werror=unused-result`. These are
suites written against the host toolchain; they are not UBSan problems.

## Rule going forward

State the environment whenever you report harness results, and never write
"O0/O2/UBSan" unless all three actually executed. If UBSan did not run, write
**NOT RUN** — the preflight now says so for you.
