# W34C14 — `.sdata` pointer dereference watch (read-only)

Branch `experiment/worldmap-open-gates-20260823`, substrate HEAD
`f91be4c33ec022d59868d01204a778610f6c5360`. No production source was
changed and no port rebuild was performed. The existing native binary was
`pc_port/build_native/xeno-port`, sha256
`e3333bc66d423da7ab7b8a16740d645d08c55add0101c6fea3574b29e9b90485`.

This is the bounded runtime watch queued by W34C2 after main-exe rodata
`[0x80010000,0x80019524)` and sdata `[0x8004EA90,0x800576E4)` first became
resident in `g_PsxRam`. It looks only for observable regressions on the paths
below; it does not prove that every pointer-bearing static table is safe.

## Results

| route | result | wall time | runtime evidence |
|---|---:|---:|---|
| KernelMenu boot | PASS | 20.17 s | Static-data load reported both accepted ranges; archive and field common initialization completed; the decompiled main loop remained live until the intentional 20-second watchdog (`rc=124`). |
| maintained Map0/Map1 field smoke | PASS | 37.41 s | Both maps reached nonzero OT submission and actor drawing; each bounded child runtime ended by its expected watchdog (`rc=124`), while the suite returned 0. |
| Lahan fieldwalk | PASS | 20.21 s | Completed the deterministic walk through frame 335; captures were produced at frames 90/150/210/270/330. Camera target moved from `(405,55,-72)` at frame 20 to `(906,13,647)` at frame 220 and then settled. |
| Map014 visual review | PASS | 32.22 s | Completed through `DONE f=611`; six 640x480 captures were produced at frames 60/180/240/360/480/600, with changing camera state. |
| hardened `w18b_natural.gdb` | PASS | 46.39 s | Reached the world placeholder and held it for 120 Vsyncs. `stable=120 ft4=1 htr=1 rand=1`; all 13 required forbidden targets were `ZERO_VERIFIED`; all five positive controls hit; verdict exit 0. |

No run received SIGSEGV, SIGABRT, SIGILL or SIGBUS, hit an assertion, or
reported a guest/adapter abort or unfamiliar stop. The `catch signal` lines in
the GDB logs are breakpoint-registration messages, not caught signals, so no
fault backtrace exists to report. The recurring PipeWire/OpenAL warning and
the already-known unhandled CD command notices are environment/backend output,
not new stops.

## Output and timing comparison

- Hardened W18B is semantically identical to its banked
  `scratchpad/w18i_forbidden_instrumentation/natural.log`: the frame-814 and
  frame-916 tuples, arm tuple, state/GFX/FT4/heap-table-rand hits, placeholder
  preconditions, `W18I_DONE`, and final forbidden verdict all match. Only log
  line numbers differ.
- The last banked Map014 launcher output (Aug 3) stopped at frame 60 because
  its hard-coded capture directory did not exist. W34C14 created that output
  directory without changing the harness or game state; the route then ran to
  frame 611. This is a harness-output plumbing difference, not a changed guest
  result. There is no prior successful Map014 wall time to compare.
- No previous wall-time records were found for KernelMenu, the maintained
  Map0/Map1 smoke, Lahan fieldwalk, Map014, or W18B. Therefore this rung does
  **not** claim that elapsed time is unchanged; the measured values above are
  banked as future baselines. No suite-defined semantic output differed from
  an available successful baseline.

The Lahan and Map014 launchers delegate to old scratchpad GDB scripts. The
Lahan script leaves its `$dir` convenience variable unset and both scripts
contain stale absolute capture paths. To test the intended routes without
editing them, W34C14 invoked those same scripts and binary directly, supplying
Lahan's direction as `$dir=0x2000`, explicit field selections
(`map=1, entrance=6` and `map=14, entrance=0`), and pre-creating only the
capture directories. No injected game state, breakpoint behavior, or
production code was changed.

Raw logs and timings are under `scratchpad/w34c14_watch.Ic0msh/`. Captures
were written to the two absolute locations already embedded in the legacy GDB
scripts; every capture is 1,228,800 bytes. They remain diagnostic scratch
artifacts and are not committed.

## Additional paths and deliberate skips

The maintained `pc_port/tests/run_field_map0_smoke.sh` suite was added because
it reaches direct field initialization, actor update/draw, and OT submission
for Map0 and Map1 outside the forced world route.

Deliberately skipped:

- the forced W34C5–W34C11 world acceptance variants, because W18B covers the
  natural world transition/placeholder contract here and those variants have
  already run repeatedly since W34C2;
- focused production certificate/unit suites, because their isolated test
  memories do not broaden live guest `.sdata` consumer exposure;
- PCSX-Redux retail runs, because they execute the retail process rather than
  this port's populated `g_PsxRam`;
- battle, movie, menu submodes beyond KernelMenu, and the many field/overlay
  routes for which no maintained deterministic harness exists. Those remain
  unexercised, as do arbitrary consumers of pointer-bearing `.sdata` tables.

## Verdict

`WATCH_RETIRED` — no W34C2-populated pointer caused an observable regression
on the exercised KernelMenu, Map0/Map1 field, Lahan fieldwalk, Map014, or
hardened natural-world paths. This is explicitly bounded coverage. It does
**not** mean the port is globally unaffected or that unexercised battle,
movie, menu, field, overlay, or static-table consumers are clean.
