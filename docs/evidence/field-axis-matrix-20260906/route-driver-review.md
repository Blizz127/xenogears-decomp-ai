# Forest-route driver revision — prepared, not launched

Prepared: 2026-09-06 UTC  
Scope: `/tmp/xeno-forest-route-20260906` tooling only

## Pinned next build

- Native executable: `/var/home/blizz/Projects/xenogears-decomp-ai/pc_port/build_native/xeno-port`
- SHA-256: `cda8623b38a192018181a232875910308f213c9d8778e8798651c7f6c34461ae`
- Manifest: `docs/evidence/field-axis-matrix-20260906/final-build-pins.json`
- Manifest SHA-256: `4b5520df59b93643778e07360cc3141a5b8efcc833976d3601c659e48180a53b`
- Current A9 source-line address (`animation_scripts.c:325`): `0x4cb06a`

The driver checks both the source executable SHA and the manifest's `binary_sha256` before creating an X server or native process. The trace's A9 breakpoint was moved from the preceding build's `0x4caf2d` to `0x4cb06a`.

## Behavioral diff from terminal run

1. Process/window discovery is independent of the title marker. The driver validates `/proc/PID/exe`, validates the SDL window PID, and focuses that owned window as soon as it exists.
2. After field 490 reports that its main loop is active, the driver waits 0.8 seconds and sends focused ordinary `z`/Circle. If the title marker remains absent, it permits at most three further focused retries separated by at least two seconds. Every attempt is logged. This resolves the previous circular wait where the title marker required input but window discovery required the title marker without installing a frame-level input schedule.
3. After `title loop enter choice=1`, the driver waits for menu initialization and sends the observed `Up`, then `z` sequence. After the logged `choice=2` New Game confirmation, it sends one further focused `z` to leave the movie-backed transition for field 4. It does not install `XENO_FIELD_TEST_INPUT` or any controller schedule.
4. The initial deadline remains 900 seconds. A live `extend-seconds` file contains the total additional seconds (0..86400), and each accepted change is recorded. Changing it to `900` makes the current bound 1800 seconds from process launch; later changes replace, rather than cumulatively add to, that extension.
5. Periodic framebuffer capture was removed. Screenshots are explicit ImageMagick `import` captures and are checked/recorded as actual PNG files. This avoids the prior 237 MiB of BMP payloads named `.png`.
6. `XENO_QUICKSAVE_PATH` is always the unique run-local `quick.xgqs`. The control helper exposes F7 and toolbar-SAVE paths. Both reject field 490/4, require the first battle-return marker, require the latest probed field to equal the operator-supplied field, require a previously captured screenshot label, and certify the exact native `saved <run>/quick.xgqs` log marker plus file/hash. The native checkpoint safety predicate remains the final gate.
7. After the first battle returns, the driver wraps 5x back to 1x and writes `field-control-ready`; it sends no more automatic input. Navigation remains an explicit single-writer control-helper activity.
8. `xdotool search` status 1 with empty stdout and stderr is treated as the expected “native exists but SDL window is not created yet” poll result. Any diagnostic text or other nonzero status fails closed. This prevents the earlier discovery race from aborting boot without hiding display/tool errors.

## Static verification

```text
python3 -m py_compile run.py control.py                       PASS
current executable vs expected SHA                           PASS
manifest binary_sha256 vs expected SHA                       PASS
GDB info line animation_scripts.c:325 == 0x4cb06a             PASS
ended-run control invocation rejects absent owned PID         PASS (rc=1)
XENO_FIELD_TEST_INPUT / XENO_FIELD_CAPTURE in revised driver  ABSENT
```

Final tooling hashes at review freeze:

- `run.py`: `7ae630dd758c81b1f5b327201da4276e05cfbf83d916f170d04d7729004c06a5`
- `control.py`: `7b1d6efd43a0f42df19ee7bc70bab70b8d1b9f90c1509d5034ccc4e970a802f1`
- `trace.gdb`: `6c35297d96faf5f5e5ff31e1239b37540c1b2563ce2141a26a266aa9e077ada1`

No native process or Xvfb was launched while preparing this revision.

## Review and launch commands

Launch after explicit go-ahead and stable pin review:

```bash
cd /var/home/blizz/Projects/xenogears-decomp-ai
python3 /tmp/xeno-forest-route-20260906/run.py
```

The driver prints the run path and live handles to `actions.jsonl`/`run.json`. Examples for the separate operator shell:

```bash
python3 /tmp/xeno-forest-route-20260906/control.py extend 900
python3 /tmp/xeno-forest-route-20260906/control.py capture field14-free
python3 /tmp/xeno-forest-route-20260906/control.py save-f7 14 field14-free
# Equivalent visible-button path, use one save method only:
python3 /tmp/xeno-forest-route-20260906/control.py save-toolbar 14 field14-free
python3 /tmp/xeno-forest-route-20260906/control.py hold 0.8 Left Up
python3 /tmp/xeno-forest-route-20260906/control.py tap z 0.10
python3 /tmp/xeno-forest-route-20260906/control.py stop
```

Before either save command, the named capture must be visually inspected to show free field control and no active dialogue. A missing exact save marker causes a nonzero result and is not accepted as a checkpoint.

## Authorized live-run addendum

The reviewed driver was launched at `2026-09-06T07:08:27Z`. The canonical path is `/tmp/xeno-forest-route-20260906/run-y9dtjd02`, read directly from `current-run.txt` and corroborated by `run.json`, `/proc/528358/exe`, and GDB's argv. Handles: display `:1`, Xvfb `528340`, GDB `528348`, copied native `528358`, window `6291508`. An earlier message incorrectly named nonexistent `run-bdo9mdzu`; that string is retracted and is not evidence.
