# W34C18 — F1–F3 guest-frame repair

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `d133d7fe272a1092919ca080beb410c076dde3b8`
- Starting origin: `d133d7fe272a1092919ca080beb410c076dde3b8`
- Date: 2026-08-28
- Verdict: **SITE_MISMATCH**
- Production changes: none

## Anchor result

W34C18 makes two committed witness records authoritative for the repair-site
list:

- `docs/evidence/w34c16-f1-witness/README.md` for F1;
- the combined F2/F3 witness README committed by W34C17.

The W34C16 record exists and anchors F1 to
`pc_port/src/world_map_callback_914d0.c:246`, where native stack storage
`delta_vec` is truncated and passed to `wm_80093484`.

No committed W34C17 combined F2/F3 witness README exists anywhere under
`docs/evidence/` at the starting HEAD.  Consequently the authoritative F2/F3
site list required by W34C18 cannot be cross-checked against the source.

For context only, the older W34C15 sweep and current source agree with the
reference names in the W34C18 task:

- `pc_port/src/world_map_helper_8e0f0.c:28` passes native stack `dir` to
  `wm_80095414`;
- `pc_port/src/world_map_helper_95cd4.c:72` passes native stack `attr_buf` to
  `wm_80084D00`.

That agreement is insufficient to substitute for the explicitly authoritative
W34C17 witness record.  Doing so would silently weaken the repair gate.

## Verdict

**SITE_MISMATCH** — the required committed W34C17 F2/F3 witness README is
absent.  The source was not changed.

Per the W34C18 ordering, anchoring precedes Step 0.  Therefore no pre-repair
baseline, repair build, route run, or neutrality comparison was performed.
Per the overnight stop condition, no later W34C15 queue rung was started.

## Standing-note status

The W34C16 F1 dormant-arm record remains unchanged.  There is no committed
combined W34C17 F1–F3 standing note to update, so W34C18 cannot truthfully mark
the three sites repaired or supersede that note.

## Hygiene

Only this evidence file was added.  No tracked production, test, harness, or
diagnostic file was changed.  Pre-existing untracked workspace files were
left untouched.
