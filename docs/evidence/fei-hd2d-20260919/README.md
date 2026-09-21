# Fei HD-2D experiment verification

Baseline b64d38a6, branch experiment/worldmap-open-gates-20260823.

- Native build passes, including the 92-leaf adopted-symbol link gate.
- Production Fei packet code passes O0, O2 and UBSan: default on / environment
  off, runtime toggle, actual atlas loading, raw-color versus shaded-color
  semantics, projected height, bind/quad/reset order, mixed-OT fallback,
  other-character exclusion, special-animation exclusion, field lifetime,
  exhausted buffer and unavailable texture. Existing toolbar tests pass.
- Fresh GCC 2.7.2 / maspsx MIPS builds before and after the guarded rendering.c
  edit have byte-identical whole `.text` sections. Hashes are recorded here.
  This verifies preservation of the baseline MIPS compilation; it does not
  claim the native executable is byte-identical to a PlayStation executable.
- Live field-15 comparison: toolbar OFF renders original Fei; ON renders the
  authored RGBA character over the same terrain, preserving the floor shadow.
  Movement exposed reversed facing in the initial experiment; the final map
  uses camera-relative 0=right, 0x400=front, 0x800=left, 0xC00=back. The final
  desktop capture shows the corrected side-facing walk pose.
- The direct-field harness initially used for the desktop preview did not
  initialize a normal party. The user reported immediate game over when a
  battle began there. This launch was stopped. The earned post-victory
  checkpoint from the preceding normal New Game combat test was copied to a
  task-owned file and loaded through the game's quick-load action. Read-only
  observation then confirmed Fei 50/50 HP, 10/10 MP and party [0,255,255].
  Walking from that checkpoint naturally entered battle and reached the player
  command menu at 50/50 HP with the retail combat sprite. No HP edits or forced
  encounters were used. A new full battle victory was not required or claimed.
- Final visible desktop process uses that checkpoint, HD2D ON, and verified
  50/50 HP, 10/10 MP. User's original quicksave was not overwritten.

Runtime logs/screenshots and the task checkpoint remain local in
`scratchpad/astra-fei-hd2d-20260919/`. This is a four-direction/two-pose field
art experiment. Combat, special poses and the environments remain retail art.
See pc_port/assets/fei_hd2d/README.md for controls, scope and asset provenance.
