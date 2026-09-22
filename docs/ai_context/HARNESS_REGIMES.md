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

## Three container packages were missing -- all three are installable

The stock image `localhost/xenogears-dev-toolchain:krom-20260913` is missing
three packages the harness needs. All three install cleanly from the Ubuntu
archive inside the container (network works; you need `--user 0`, because the
image runs as uid 1000 and apt needs root):

| package | why the harness needs it |
|---|---|
| `clang` | a large number of runners use **clang, not gcc**, for their UBSan regime (e.g. `run_battle_return_state_retail_test.sh` sets `cc=clang`). Without it the suite dies on `clang: command not found` -- 54 of 81 failures in the first sweep were exactly that. |
| `libclang-rt-18-dev` | clang's *own* UBSan runtime, `libclang_rt.ubsan_standalone-x86_64.a`. Installing `clang` alone is **not enough**: clang-based UBSan regimes then fail at link with `cannot find .../libclang_rt.ubsan_standalone-x86_64.a`. This one is easy to miss. |
| `ripgrep` | every runner uses `rg` as its assertion matcher. |

`Dockerfile` in this repo has been updated to install them. Note the running
image is **not** built from that Dockerfile (different WORKDIR, and it already
carries gcc and the venv), so the Dockerfile change alone will not fix the
image you are using.

To get a working image right now, build a thin derived one:

```
podman build -t localhost/xenogears-dev-toolchain:krom-20260913-harness - <<'EOF'
FROM localhost/xenogears-dev-toolchain:krom-20260913
USER root
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update -qq && apt-get install -y --no-install-recommends \
        clang ripgrep libubsan1 libclang-rt-18-dev \
    && rm -rf /var/lib/apt/lists/*
USER 1000
EOF
```

Then use that tag in the usual podman invocation. With it, the gcc *and*
clang UBSan regimes both run.

With the stock image the remaining failures are container gcc 13 vs host
gcc 16 differences -- 7 suites pass `-fpermissive` with `-Werror` (rejected
for C by gcc 13), 9 hit undeclared identifiers, 2 trip `-Werror=unused-result`
-- plus 2 suites wanting a generated `battle_bridge_map.inc` that the suite
itself never generates (it only adds `-Ipc_port/build_native` and hopes an
earlier build left the file there). Those are suites written against the host
toolchain, not UBSan problems.

## Three dead gates found in this run, and the pattern behind them

Separately from UBSan, three suites were reporting nothing while appearing to
work. They are worth reading together, because the shape repeats.

1. **`run_battle_overlay_leaf_bridge_test.sh` was not running at all.** It
   never linked `god_mode.c`, which `battle_mips_runtime.c` needs, so it died
   at link; and its negative-control injection anchor had drifted. This is the
   gate guarding the battle-leaf adoption mechanism, so the mechanism was
   unguarded while appearing guarded. Fixed by the battle worker in
   `cc98b9a4`. **Verified here**, with the full toolchain, for the first time
   end to end: three `BATTLE OVERLAY LEAF PASS checks=40` lines (O0, O2 and a
   genuine UBSan) followed by
   `BATTLE OVERLAY LEAF PASS (O0/O2/UBSan + always-interpret control)`,
   exit 0 -- so the negative control does reject.

2. **`run_battle_native_callback_map_test.sh`** had the same missing link.
   With it fixed the gate now fails **loudly** and correctly:
   `NATIVE CALLBACK FAIL generated stub was dispatched`, preceded by
   `unresolved native call target=0x0050f00d guest-pc=0xfffffff4`. That is a
   real pre-existing defect, deliberately left unfixed; it now has a voice.

3. **`run_battle_graphics_abi_test.sh` under-ran silently.** Its O0 binary
   printed two PASS lines and then returned 1 with **no message at all**, so
   `set -e` abandoned the O2 regime, the UBSan regime, and both mutant
   negative controls. Visible output was two PASS lines. Fixed here: `main()`
   was one long `||` chain returning a bare `1`, now a `REQUIRE()` macro that
   names the failing check, and the two silent `return 0` paths inside
   `check_registered_host_callback` now say which property failed.

   With that, the real defect is legible:

   ```
   REGISTERED HOST CALLBACK FAIL truncated-pointer was accepted
   BATTLE GRAPHICS ABI FAIL check=check_registered_host_callback()
   ```

   `runtime_bridge()` accepts a host pointer whose high 32 bits have been
   corrupted (`host | 1<<32`) instead of rejecting it. The test's own comment
   states the property: *"Unknown and truncated pointers must never become
   callable merely because the address lies in an executable host mapping."*
   Pre-existing, in `pc_port/src/battle_mips_runtime.c`, reported not fixed.

## The systemic count: 349 of 566 suites can under-run invisibly

Three dead gates in one session is a pattern, not three accidents, so the
whole of `pc_port/tests/` was audited for the shape rather than only the three
being fixed. `pc_port/tests/lib/underrun_audit.py` flags a suite when **both**:

* it emits PASS/CERTIFICATE text somewhere that is not its last statement --
  either the runner echoes it early, or a test binary it runs prints its own
  PASS; **and**
* there is more work afterwards: another regime, a mutant control, or any
  further statement.

```
runners audited      : 566
UNDER-RUN-RISK       : 349
no early PASS text   : 217
```

**349 of 566 suites can print PASS and still be abandoning later regimes or
controls.** That is not 349 dead gates -- it is 349 suites with the structure
that let three real ones hide. The audit correctly flags the known true
positive, `run_battle_graphics_abi_test.sh`.

Dynamically confirmed, not just static: a partial container sweep
(93 of 565 suites completed before it was stopped for time; logs in
`pc_port/build_native/fullsweep/`) found **4 suites that exited nonzero after
having printed PASS text** --
`run_anim_render_index15_retail_test`, `run_battle_effect_callbacks_retail_test`,
`run_battle_graphics_abi_test`, `run_battle_load_image_cache_test`. The full
sweep was not completed; at roughly 2.5 suites/minute it needs about four
hours, and the number above is therefore a **lower bound from a 16% sample**,
not a final figure. Re-run `pc_port/tests/run_ubsan_sweep.sh` to finish it.

Caveat on that sample: 62 of the 93 exited nonzero, but most of those are the
container gcc-13-vs-host-gcc-16 build differences described above, not test
failures. Do not read "62 failing suites" as 62 defects.

### What to do about it

The cheap, general mitigation is the one applied to
`run_battle_graphics_abi_test.sh`: make the *final* line of a suite the only
thing that says the suite passed, and make every intermediate failure name
itself. A suite that prints per-regime PASS lines is fine **provided** its last
statement is a single summary that cannot be reached early -- and provided a
reader checks for that line rather than for the word PASS.

## Rule going forward

State the environment whenever you report harness results, and never write
"O0/O2/UBSan" unless all three actually executed. If UBSan did not run, write
**NOT RUN** — the preflight now says so for you.
