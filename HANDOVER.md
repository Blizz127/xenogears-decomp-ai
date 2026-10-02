# Xenogears Codex takeover checkpoint — 2026-10-02

This checkpoint was authored by Codex after the owner transferred single-writer
ownership from Claude. It is **not a Claude-authored handover**. Read-only polling
from 18:44:27Z through 18:59:48Z found no `HANDOVER.md` in any registered project
worktree and no handover commit. Under the owner's 15-minute fallback, work
continued from the newest commit, `6e189169d3e88abe1c0ebb78a87341eaf0160b78`.

Active branch: `codex/native-port-20261002`, in
`~/dev/xenogears-decomp-codex-native`. The initial checkpoint remains on
`codex/battle-results-native-resume-20261002` in the battle-results worktree.
The coordinator then clarified that Claude's pending Dangerous rm prompt
concerns `infra-scratch/leaves3` cleanup. Leave that worktree's uncommitted
scratch untouched while the owner resolves it; all further Codex edits use
this isolated worktree. The prior Claude branch and all other worktrees are
preserved. Codex is the single writer. The permission prompt was not answered
or dismissed; existing authorization and credential holds remain in effect.

The unfinished next step recorded by `pc_port/README.md` and the private
`native-audit/battle-decomp/BATCHES_INDEX.md` was the batch-3 runner gate.
It has now executed once. See
[the validation record](docs/evidence/battle-results-batch3-20261002.md): all six
behavioral suites and the real source guard passed; the combined release gate
failed on 19 pre-existing binary retail-data findings. Nothing was published.

Next work remains the documented in-game results-state identity and native
execution route, plus the binary-data remediation required before packaging.
Do not adopt native caller drafts without the guest-frame/call-recorder proof.
Battle remains hybrid; the native milestone and coverage limits are tracked in
the validation record.

Route preparation review: the older worldmap2 probe job
`20260930-190001-xeno-worldmap2-verify-probes-1685321` did not prove a victory.
Its natural route never confirmed New Game, and both battle probes aborted;
the job's outer zero exit masked those sub-results. The onehit map-1 probe
reached RESULTS and returned with an empty party, so it is only a diagnostic
fixture. The existing `assists_test.py` also closes its log in `stop()` before
the caller writes its final marker. Before reusing that driver, fix its
shutdown ordering and isolate config, memory cards and a copied checkpoint.
Compare state at results entry and return, not guest-store log lines: native
helpers' writes are not present in that instruction-only log. These are
preparation findings, not new route acceptance or newly executed tests.

Existing HD-2D lane: approved detailed-v2 direction, art opt-in and OFF by
default, full Fei set incomplete. The first 32-state batch has 25 passing masks
and seven unresolved masks. Preserve the private art, contact sheets, approved
samples and effects mockups; do not promote the rejected V15 art or retire the
placeholder yet. Shared menu checkpoints and integration order remain recorded
in the existing lane evidence.

Long work belongs on the blizzpc runner under its existing caps, one heavy job
per lane, with PE/PlanetSide priority. Preserve private-channel release gates,
rollback, saves/settings, native/hybrid disclosure and the owner-device launch
restrictions. No public push or competing publisher.

The initially supplied report task ID `none` returned `Task not found`, so the
first progress reports used the coordinator's native inbox. The coordinator
subsequently assigned **`f5b072578f5f`** to the native-port takeover; use that
ledger task for further reports. Keep HD-2D task **`83b32c4c7ff6`** separate.
