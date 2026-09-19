# Opening through Blackmoon Forest — active route acceptance

Goal: retail-faithful normal play from the opening through Blackmoon Forest.
Status: **IN_PROGRESS**, not a parity certificate.

Fresh run starts at commit 7d50c62a using normal retail boot and title New Game.
The initial run used no field-map override, scenario override, checkpoint load,
forced battle result, teleport or game-state patch. A later process restart and
earned-checkpoint continuation are recorded below. HD-2D Fei remains enabled as requested;
its optional presentation changes are separate from retail simulation fidelity.

Local evidence is `scratchpad/blackmoon-route-20260919/`: run.json pins the
binary and launch environment; runtime.log records transitions; inputs.log
records ordinary keyboard inputs. route.xgqs is reserved for checkpoints earned
on this run, keeping user saves untouched. PIDs are historical hints; revalidate
process ownership before attaching or sending input.

Observed on this fresh run: title New Game -> field4 narration/city sequence
-> field2 opening Gear battle -> field14 painting room -> field13 downstairs.
The opening battle returned after 71,737,910 interpreted instructions. Field14
control unlocked at scenario6 and ordinary directional input reached its exit.
Field13 triggered Dan's conversation naturally. No runtime stub or unresolved
call was logged along this segment. This proves progression, not audiovisual
identity against a retail hardware capture.

Required route acceptance still outstanding:

- Finish downstairs dialogue, village, Alice interaction and mountain traversal.
- Citan's house, story events, night return, burning Lahan and Gear combat.
- Destruction, aftermath and normal departure to the world map.
- Natural entry into Blackmoon Forest, its required story events and battles,
  and traversal through the forest. Establish map identity from the actual
  route and retail assets; older notes inconsistently label map 16.
- Movement, collision, dialogue, menu return, battle victory/return and required
  transitions throughout; record rendering/audio discrepancies when observed.

A passed unit test, forced map inspection, old walkthrough or isolated screen
capture cannot complete this route. Newly discovered defects require retail-
backed diagnosis, relevant differential/byte checks and natural-path retesting.

## First mountain encounter checkpoint

The fresh route continued through Dan's dialogue (scenario7), field1 Lahan and
its mountain exit into field15. Navigation used ordinary keys; an older input
sequence stopped against a village rock and was corrected by walking around it.
This was navigation, not a detected collision defect. Alice's optional visit was
not exercised in this segment.

A random encounter started naturally on the mountain: Fei entered with 50/50 HP
and fought three enemies through the standard command/attack interface. The
victory rewards screen was observed, and the battle returned after 77,228,928
interpreted instructions. After return, Left input changed field position from
(660,-15,-840) to (555,-40,-737). F7 then wrote the route checkpoint normally.

Checkpoint files, deliberately local and not committed:
- mountain-entry.xgqs: first arrival in field15.
- first-victory.xgqs: earned progress after the first normal mountain victory.
- route.xgqs: latest route checkpoint (same as first-victory at this entry).

Continuation: owned game PID1767877 on Xvfb :95, window2097204; verify live state
before controlling it. Input scripts have finished and all keys are released.
The binary is still commit7d50c62a. Continue walking the mountain toward Citan,
handling encounters normally. Do not reload the older unrelated test saves or
use Escape's test-only battle bypass. The full goal remains active.

## Mountain continuation: five normal victories

The preserved live run won encounters 2 through 5 using normal battle controls.
Each victory returned to field15. The Items screen opened and cancelled normally;
a read-only debugger inspection found all 150 live item quantities zero. The
Abilities screen displayed Guided Shot and cancelled normally. Fei is level2,
11/52 HP, 10/11 EP; healing through normal gameplay is needed before further
encounters. No inventory or health was injected. A mistaken File selection
exercised the explicit unsupported message; dismissal returned to the menu.

Navigation at the central cliff was investigated with read-only walkmesh dumps.
A simple triangle adjacency route that ignored materials incorrectly proposed
climbing material2 cliff faces. Restricting that diagnostic to material1 revealed
the eastern climb, which was traversed normally to approximately (585,-475,1124),
but it ends at a cliff rather than the bridge route. No collision repair was
justified or applied. Layer1 geometry identifies the raised western trail;
continuation should use the western route rather than repeat the eastern detour.

Latest earned save: route.xgqs, with fifth-victory.xgqs preserved separately.
Player is at field15 approximately (584,-14,-505), facing after two camera
rotation experiments (do not assume the original screen/world direction map).
Game PID1767877 remains owned and live on :95; all input scripts are finished.
Health management is the next gameplay decision. Normal battle Escape via its
command menu is allowed; the keyboard Escape test bypass remains forbidden.
The goal is still incomplete: Citan, burning Lahan, aftermath/world departure,
and Blackmoon Forest have not been reached on this fresh run.

## Process interruption and earned-checkpoint continuation

After the fifth victory, ordinary walking returned through Lahan (field1),
Fei's main floor (field13), and his basement room (field14). The bed interaction
displayed "Took your hidden money." (`bed4.png`). Rest and restored HP were not
yet observed. The session was then interrupted; on the next live check neither
the game nor Xvfb existed. The log has no recorded fatal error at its end. Cause
of process termination is unverified; do not classify it as a reproduced game
crash. No save was made after leaving the fifth-victory checkpoint.

The original `runtime.log` and `run.json` are preserved. A new process was
launched with the identical binary SHA256 (c62d685ea6e834b284e1af93782a706d7f4a7bd8e10771fc845232be6537f4f8),
same isolated checkpoint path, normal boot, and HD-2D enabled. Its log is
`runtime-resume1.log`, with pins in `resume1.json`. Normal New Game was selected
because loading checkpoints requires free field control. The continuation is
intended to load only this run's earned `route.xgqs`; load acceptance and recovery
remain pending until observed. This is not an uninterrupted playthrough.

## Title checkpoint recovery fix

The first restart replayed the opening (using 5x for the already-covered segment,
restored to 1x before F8) and loaded the earned fifth-victory checkpoint. A random
encounter interrupted the return home. Fei entered with 11/52 HP and was defeated
after normal Escape attempts. The later checkpoint is preserved; continuation
uses a separate byte-identical copy of this run's first-victory checkpoint in
`recovery.xgqs` (SHA256 a9b6a8a559f4cc99e14d309aa2657753a49441080315d19b3eb3a5b9bf8922bd).
This explicitly rolls back the later four victories for the ongoing route.

Recovery exposed a native checkpoint gap: title field490 has no controllable
player and its nested menu loop prevents FieldMain polling. F8 stayed pending.
The fix permits loads in the initialized title field, rejects title saves, polls
in the port title-menu body and carries the prepared load through ordinary menu
cleanup to FieldMain's existing teardown. Other gameplay control gates remain.
The first live attempt with only the field gate fixed stayed pending, motivating
the title-loop regression and handoff fix. This supports native checkpoints;
it does not implement retail memory-card Continue or certify that interface.

Validation:
- Real checkpoint gate regression failed before the fix at title loading; O0,
  O2 and UBSan now pass. Tests cover locked gameplay, inactive title, missing and
  corrupt files, preserved save contents, and the two-stage menu/field handoff.
- Production-linked title-chain tests pass O0/O2/UBSan, including checkpoint
  exit/cleanup without selecting New Game, Continue or attract mode. Existing
  five negative-control mutants still fail as expected.
- Existing file, request, collision-restore and integration regressions pass.
- Full native build passes: 78 generated function stubs, 96 adopted leaves.
- Fresh before/after MIPS compilation of `src/menu/main/misc.c` gives identical
  whole `.text` bytes, SHA256
  `7ff8ccc25bf6ba12b25cd80e2e813799fb77cbf9c2a5271a32ced06230ee2504`.
  The title hook is exclusively in the XENO_PC_PORT branch. This proves no
  MIPS change from this patch, not whole-port byte identity.
- Live rebuilt game: title F7 rejected; F8 loaded field15 at (555,-40,-737).
  `title-load-pass2.png` shows LOADED and HD2D ON. The source checkpoint hash
  remained unchanged. A natural random encounter followed; Fei displayed49/50HP.

Current continuation is pinned by `resume3.json`, with log `runtime-resume3.log`.
The earlier PIDs above are historical. Full Blackmoon route acceptance remains
incomplete; no new retail movement/combat repair was justified in this segment.

The subsequent encounter was won normally and returned to field15. Directional
input then moved Fei from (578,-19,-610) to (380,-36,-851), verifying movement
after title load and battle return. Walking through Lahan, field13 and field14
reached the bed. The normal interaction awarded200G, showed the rest prompt,
and Yes restored Fei: `recovery-stats2.png` shows level1,50/50HP,10/10EP,300G.
The menu cancelled normally; F7 saved field14 at (232,0,-237).
`recovery.xgqs` now holds that earned rested state; `rested-at-home.xgqs` is a
preserved copy. The earlier first/fifth-victory and route checkpoints remain.
Live continuation at this entry: PID96416, Xvfb :95, window2097204,1x speed,
HD2D ON; all automation/input scripts have finished. Verify these live before
resuming. The map camera differs from the previous low-HP run: in the present
orientation Down approximately decreases x and z; Down+Right decreases z.
Next: return to mountain, follow the western ascent/running-jump route to Citan,
and continue required story progression. Bed recovery and checkpoint recovery
are observed; Blackmoon Forest and full retail fidelity are still unproven.
