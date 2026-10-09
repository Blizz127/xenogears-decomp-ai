# SDK release gate

The 2026-10-09 coordinator decision accepts the MIT PsyCross implementation,
requires its MIT notice in packages, and forbids shipping its retail-identical
sine/cosine, arctangent and square-root tables. The existing patches obtain
those values from the user's disc at runtime. The matching build's translated
Psy-Q sources remain excluded from the port build.

`pc_port/build_port.sh` keeps debug information and maps source paths relative
to the checkout. Its binary gate uses `--strict --require-empty-allowlist
--require-dwarf`. This build check does **not** scan retail tables; its receipt
says that the table scan was skipped.

`pc_port/tools/package_linux.sh` additionally requires the SHA-pinned retail
`disc/SLUS_006.64`. A missing or wrong executable fails the gate. It scans the
unstripped build binary with DWARF required, then the copied executable after
`strip --strip-debug`, before producing the archive. Both scans use strict
mode and require an empty allowlist. Strict mode has no PsyCross table
exemption. Failed binary inspection is an error, not a clean result.

The loaded ELF sections are checked for absolute build paths. Debug-only
paths do not count as shipped runtime strings. DWARF is retained in the
build artifact for source attribution and removed only from the package copy.

Run lightweight regressions without game data or a game build:

```sh
python3 tools/analysis/test_port_guards.py
python3 tools/analysis/test_psyq_release_guard.py
```

The second suite creates tiny synthetic ELF files using the host compiler and
binutils. It checks table exemptions, missing evidence, path mapping, stripped
copies, translated-SDK source attribution and invalid inspector results.

The guard detects classified source units, function identities, known table
windows and path strings. It is not a proof that arbitrary renamed code has
independent provenance. The 29 compatibility functions identified in the r5
audit still require the approved clean-room replacement workflow: public
specifications, black-box retail vectors, an isolated implementer, provenance
records and O0/O2/UBSan comparisons with rejected mutants.

No new public binary is authorized until the SDK gate and the separate native
execution requirement are both satisfied. Current CPU interpreter paths remain
disclosed; passing the SDK guard or function tests does not establish a useful
playable slice with zero fallback. Private builds retain their existing release,
identity, data, runner and publication requirements.
