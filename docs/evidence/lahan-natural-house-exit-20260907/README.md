# Normal-route Lahan house exit

Observed on current native build 87165a34833fd47912d4d9eb79621d028508169e42ded179481dcb531a18b49c.
Branch experiment/worldmap-open-gates-20260823, HEAD3a3e7aac; shared dirty work
preserved. No game-code change, stage, commit, or push in this observation.

## Route and authority

The same live normal-boot session completed the opening battle, returned to
field14, closed Fei's painting dialogue, climbed the actual stair corridor,
entered field13, played its exit conversation, and interacted with the outer
door to enter field1. No debug boot, checkpoint load, RAM/register/position,
party, story flag, or scene-state edit was used. Read-only GDB inspections
recorded positions and scripts; all movement was ordinary X11/SDL d-pad input.

Field14 actor17's routine at0750 checks trigger0 and executes opcode98 at0754:
field13, entrance1. Trigger X308..363, Z-52..0. Approaching the side of the stairs
was blocked; the tread corridor is reached from its lower-X end nearX197,Z-30,
then runs toward the trigger. This was navigation, not a demonstrated port bug.
Field13 actor17 interaction script contains opcode47 requesting field1,
entrance7. Walking close to its actual location and pressing Circle took it.

The scripts and triggers dumped from fields14/13 match the disc archive data
for their map-header-declared lengths. See map-retail-provenance.json for archive
entries, sectors, lengths, and hashes. Compressed stream headers declare extra
bytes beyond those map payload lengths; the comparison deliberately bounds the
decoder to the map-declared length and makes no claim about trailing bytes.
Disc bytes and compressed token bytes were not changed.

## Limits and live continuation

This is native llvmpipe runtime evidence, not a desktop-GPU acceptance test or
retail audiovisual comparison. Complete Lahan, optional interactions, later
battles, story departure, exact overlay reconstruction, and audio parity remain
open. No diagnostic navigation problem was converted into a speculative fix.

At recording time the game PID471575 and Xvfb471573 remain live on isolated
DISPLAY:2. The original observation-only deadline watcher PID471572 is suspended
(SIGSTOP); it does not control game input or advance game state. Execution
session36992 owns them. Do not terminate that parent session while retaining
its children: the execution service cleans up the whole session. Revalidate
processes before reuse. The live position is now the village, field1 entrance7;
raw navigation captures and read-only map dumps are in the scratchpad directory.

## Village movement follow-up

After the field1 transition, ordinary Up+Right input for two seconds moved Fei out from behind the foreground house. `village-walk1.png` shows him visible and the camera following. The earlier occlusion alone is not evidence of a rendering defect. Field1 script and walkmesh data were read without modification into the runtime scratch directory for subsequent route analysis (2652 triangles, 1412 referenced vertices, 12056 script bytes, 288 trigger bytes). These field1 dumps have not yet been compared against the disc. Village story progression and end-of-Lahan acceptance remain pending.
