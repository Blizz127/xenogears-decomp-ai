# W34B23-R1 — Mechanical Replay onto Canonical 7575e26

Original candidate `ec779a3` (parent `7fbce38`) was replayed because
canonical advanced to `7575e26` (W34B24-A1 landed 952B0 + 95324 first).

- Replay: cherry-pick of `ec779a3` onto `7575e26`; sole conflict was the
  expected trivial `PORT_SOURCES` list overlap in `pc_port/build_port.sh`,
  resolved by preserving all accepted sources plus the three replayed ones
  (952b0, 95324, 93fe4, 94088, 951a8 all registered). Run script mode set
  to 100755 (matching the W34B24-A1 convention fix).
- No source-content changes relative to `ec779a3`.

## Re-certification on the replayed tree

- `run_w34b23_951a8_chain.sh`: PASS (O0/O2/UBSan identical, 14/14 mutants
  killed).
- `run_w34b24_i1_952b0_95324.sh` (cross-suite on the merged tree): PASS
  (14/14 mutants killed).
- `./pc_port/build_port.sh`: clean LINK OK + incremental LINK OK;
  stubs 241/524 unchanged.
- Symbols: exactly one strong `T` for each of wm_80093FE4, wm_80094088,
  wm_800951A8, wm_800952B0, wm_80095324; no stub shadows.

Awaiting independent acceptance; not pushed to canonical from this lane.
