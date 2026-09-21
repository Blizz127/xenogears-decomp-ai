# D4 worktree and evidence hygiene

## Banked proof tree

Validated from
`scratchpad/w34b5hs_natural_scheduler_verify/natural_scheduler_proof_c238e529_w34b25_f3d046a8/`:

```text
README.txt: OK
pre_scheduler_capture.log: OK
pre_scheduler_results.txt: OK
run_console.log: OK
scheduler_capture.log: OK
scheduler_capture_prologue_diag.log: OK
scheduler_prologue_diag_results.txt: OK
scheduler_results.txt: OK
```

All entries in the banked tree’s `SHA256SUMS` passed. The banked tree was
not modified or re-baselined.

## Worktrees

Ran `git worktree prune`. It removed no currently registered worktree
metadata. `git worktree list --porcelain` still reports the explicitly
registered worktrees, so no worktree directory was removed: their ownership
and contents were not safe to infer from this session.
