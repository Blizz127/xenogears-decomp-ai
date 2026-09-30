# Milestones

> Public releases of the PC port happen only at milestones. Between milestones,
> test builds go to a private channel. The decompilation itself stays public and
> is updated continuously. Status here is honest: "verified" means a recorded,
> repeatable test against the retail game, not "it seemed to work once".

## Milestone 1: Black Moon Forest, 100%

**Target:** play from a new game to the exit of the Blackmoon Forest, exactly as
on the PlayStation, with no crashes, and with saving and loading through the
memory-card screen.

**Status (30 September 2026): about a quarter of the way there.**

What is verified today:

- Title screen, New Game, prologue, Lahan village and Fei's house; field menus.
- The world map on foot around Lahan is frame-exact against the retail game
  (position, heading, camera, wall sliding; 30 frames per second; fog and
  trigonometry fixed to retail).
- Entering Lahan and the Mountain Path from the world map by walking onto the
  triggers.
- One on-foot battle runs attacks, combos and damage without crashing.
- The title screen's Continue opens the retail memory-card screen (it used to
  freeze).

What is missing, in the order it will be worked:

1. **Memory-card Load, Save, Copy and Delete, and save points.** The port bodies
   exist and pass their unit tests; the end-to-end test (save at a save point,
   quit to the title, Continue, load) is running now.
2. **Battle to completion.** No battle has yet been driven to victory under
   test: experience, gold, drops, level-ups, escape and game over are unproven.
   Most of the battle module still runs through an instruction-level
   interpreter of the retail code with logging stubs at 18 boundary functions;
   each must be verified or replaced with its matched C.
3. **Field progression from Lahan to the forest exit.** Every scene from the
   wedding through the attack on Lahan, the exile and the forest (including its
   random encounters and the boss) must run and be compared with retail. No
   recorded run has reached the forest map yet. Two known crashes near the start
   (resting at a bed, the painting close-up) are on the list.
4. **Sound.** XA streaming and two partial sound routines are still open.

Next steps: the memory-card end-to-end test, then a progression probe (New Game
to wherever the scripts stop) and a battle-to-victory probe to turn the estimate
above into a measured one.

## Milestone 2: World map, 100%

**Target:** every world-map state as on the PlayStation: on foot, in a Gear, in
vehicles, with encounters, the world-map menu and saving, in every region.

**Status:** the on-foot path in the Lahan region is frame-exact against retail.
The Gear and vehicle paths are unverified (they still contain a legacy fallback
and a hand-written vehicle controller), most world-map functions run as hand
transcriptions rather than matched C, and later regions and vehicles cannot be
tested until the story reaches them. A rendering bug (terrain triangles
stretched across the screen at some camera angles, caused by the emulated GPU
drawing triangles the real one rejects) has a fix awaiting verification. This
milestone is gated on Milestone 1.

## How milestones are verified

- The port is built from the same source as the byte-matched decompilation and
  checked with `make rom-check` before every build.
- Behaviour is compared with the retail game running in PCSX-Redux from the
  owner's own disc; no retail data is committed.
- Every enhancement (HD-2D, text speed, assists) is off by default and has a
  test showing that "off" is identical to retail.
