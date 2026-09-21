# GitHub Wiki staging area

`docs/wiki/` is the **source staging area** for GitHub Wiki pages for this project.
It does not replace the canonical repo docs:

- [`docs/ai_context/ACTIVE_HANDOFF.md`](../ai_context/ACTIVE_HANDOFF.md) — deep per-session journal and verified state
- [`docs/ai_context/REFERENCE_SOURCES.md`](../ai_context/REFERENCE_SOURCES.md) — approved external references
- [`pc_port/README.md`](../../pc_port/README.md) — PC port architecture and build notes

The actual GitHub Wiki is a **separate git repository** from the main code repo.

## Manual sync workflow

1. Clone the wiki repo (once):

   ```bash
   git clone git@github.com:Blizz127/xenogears-decomp-ai.wiki.git
   ```

   If your fork uses a different repo name, the wiki URL is:

   ```text
   git@github.com:Blizz127/<repo-name>.wiki.git
   ```

2. Preview what would be copied (does not write):

   ```bash
   ./scripts/preview_wiki_sync.sh ~/Projects/xenogears-decomp-ai.wiki
   ```

3. Copy staged pages into the wiki checkout, review, commit, and push manually:

   ```bash
   rsync -av \
     ~/Projects/xenogears-decomp/docs/wiki/*.md \
     ~/Projects/xenogears-decomp-ai.wiki/

   cd ~/Projects/xenogears-decomp-ai.wiki
   git status
   git add *.md
   git commit -m "Update Xenogears project wiki"
   git push
   ```

Do **not** treat wiki pages as authoritative unless they match current committed handoff notes.
When in doubt, trust `ACTIVE_HANDOFF.md` and recent commit messages.
