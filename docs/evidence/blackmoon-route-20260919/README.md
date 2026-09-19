# Opening through Blackmoon Forest — active route acceptance

Goal: retail-faithful normal play from the opening through Blackmoon Forest.
Status: **IN_PROGRESS**, not a parity certificate.

Fresh run starts at commit 7d50c62a using normal retail boot and title New Game.
No field-map override, scenario override, checkpoint load, forced battle result,
teleport or game-state patch is used. HD-2D Fei remains enabled as requested;
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
