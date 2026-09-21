# W34B49 — worktree and evidence hygiene

Read-only D4 detour. No source or banked proof tree was changed.

- `git worktree prune -v` removed no stale worktree metadata.
- `git worktree list --porcelain` reports 49 registered worktrees; all 49
  registered paths exist, so no stale directory was removed.
- The original checksum invocation used the wrong working directory for the
  W34B25 manifest and produced path-open failures. This was a path-context
  error, not a hash mismatch. Re-running each manifest from its recorded
  path context gives exit 0 for both:
  - `natural_scheduler_proof_c238e529/SHA256SUMS`: 6/6 `OK`.
  - `natural_scheduler_proof_c238e529_w34b25_f3d046a8/SHA256SUMS`: 8/8
    `OK`.

No banked tree was re-baselined, regenerated, or touched. The large set of
untracked scratchpad/artifact paths was left untouched.
