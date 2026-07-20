# Open Issues

Rules:

- If it's in this file, it's OPEN. Closing means DELETING the entry, not annotating it.
- Every item carries a reproducer command, not just a description.
- Every item carries `Last verified @ <commit>`. Stale verification = suspect claim.
- Every claim is tagged by evidence class:
  proven | build-verified only | observed | inferred
- For PSX symbol-width findings, classify the retail storage pattern before
  proposing a fix: direct data array, packed 32-bit pointer slot loaded/stored
  with `lw`/`sw`, or embedded array addressed with `lui`/`addiu`. Commit
  `1229ea1` is the reference implementation for all three forms.

---

## Lahan-playthrough worklist (scoped 0a32159; story-ordered, read-only, no implementation)

The path to "fully playable Lahan," ordered as a player hits each wall.
Evidence: normal-boot walk, 21-map Lahan-block boot survey (maps 0-20),
frame captures (maps 0/2/5/16), fail-loud assert census, stub census.
Last verified @ 0a32159.

(A) MAP SET (Lahan block 0-20): 19/21 boot to PLAYS+SOUND.
  0 = dark interior story scene (27 actors); 1 = Lahan village day (walk/
  dialog/well-camera all previously validated); 2 = Lahan night w/ fog
  (fixed 0a32159); 5 = house interior (Alice's; verified frame); 6,8,9,
  11,12,13 = interiors; 14 = Fei's-room intro scene (fire-painting zoom);
  15 = mountain path -- ASSERT AT BOOT; 16 = Blackmoon Forest --
  RENDER-SUSPECT (frame mostly black at f150; loader stub fires);
  17-20 = forest/outskirts, play. Failures: MAP3 (SEGV, unported sprite-
  loader func_800A1364) and MAP15 (func_8008399C interaction assert).

(B/C) ORDERED WALL-LIST (playthrough order; BLOCKER vs COSMETIC; size):
  1. Intro FMV skipped -- movie decoder unported. COSMETIC (skippable).
     Large (a decoder). Defer.
  2. New Game flow hardcodes map1/ent6 (port_main.c:1587), bypassing the
     retail intro scene (MAP14 zoom + intro windows). FLOW-COSMETIC --
     the scene itself plays when booted directly. Small (routing).
     WALL-10 SCOPE (NG-flow, read-only): the port's NG boot is a DELIBERATE
     SHORTCUT -- port_main.c:1587 hardcodes D_8006F94E=1 -> drops straight
     into playable Lahan (Fei+Citan, control immediate; wall 8). It runs NO
     pre-Lahan scene. The "scene circling Citan's lab" observed in testing
     was a DIRECT-BOOT to MAP14 (Fei's-room FIRE-PAINTING scene, scripted
     mode-1 camera panning the painting -- captured, confirmed), NOT the NG
     boot; "Citan's lab" was a mis-ID of Fei's room.
     (B) Retail NG: func_8001BB50 -> func_8001B970 (template, wall 7) + state
     bytes -> FMV -> opening scene chain (fire painting MAP14 -> dream ->
     Fei wakes -> Lahan), each scene advancing on a scripted trigger.
     func_8001BB50 sets no map -> the first field is the opening scene via
     the transition system, not map1 directly.
     (C) MATCH: PARTIAL / DELIBERATE-SKIP -- not mis-routed (map1 is the
     correct DESTINATION), the port just omits the whole cinematic opening.
     (D) OPENING SCENES BOOTABLE: MAP14 renders correctly (the painting +
     scripted camera; only benign libarchive/sound stubs, NO asserts/stalls,
     NOT a cutscene-opcode wall). MAP0 = a dark 2-char interior scene, boots,
     sprites render. Scenes hold on scripted-timer/trigger advance in
     headless (MAP14 did not auto-transition or confirm-advance in a ~480f
     window -- timed/event-gated, not stalled).
     (E) FMV BOUNDARY: SKIPPABLE. The opening FIELD scenes are field maps
     (field pipeline, no movie decoder -- MAP14 renders with none involved),
     SEPARATE from the FMV/STR decoder. NG-routing scene->scene does NOT
     need the deferred decoder.
     (F) PLAN: plausibly-bounded ROUTING pass, NOT a decomp: route NG entry
     to MAP14 (not map1), let the scenes chain via their scripted
     transitions, supply the per-scene advance (timer/confirm). Scenes boot,
     FMV is skippable, map1 is the right end. RISK/UNKNOWNS (needs live
     verification): the full chain MAP14->dream->house->Lahan transition
     triggers are untraced (MAP14's advance mechanism unidentified); the
     untested scenes (dream, Fei's-house-proper) may surface per-scene
     cutscene-opcode walls when run live. RECOMMENDATION: the current
     shortcut already serves "playable Lahan first"; NG-routing adds the
     cinematic opening (fidelity) as a medium routing pass with
     live-verification risk -- lower priority than the object-overlay arc
     (which unblocks MAP3+MAP16 gameplay). Defer unless the opening
     sequence is the explicit goal.
     WALL-8 DIAGNOSIS (the "intro input-lock", read-only pass): NO IN-FIELD
     LOCK EXISTS -- the wall-7 "NG movement is input-locked" reading was a
     HEADLESS-CAPTURE ARTIFACT (no keyboard focus in the GL capture -> the
     button mask D_800AFE9C reads 0). Live trace under real NG boot: the
     player-control opcode func_8009F5F4 fires EVERY frame, player IP stable
     at the control loop (0xa2), g_FieldCameraMode=0 (follow) from frame 1,
     input mask D_800ADB00=0xFFFF (fully unlocked); the port wires
     keyboard->pad (arrows->d-pad, X/V/Z/C->buttons, PsyX_main.cpp:257) so a
     REAL user already has control. PAYOFF PROVEN: real NG boot (real roster
     [Fei,Citan], real Citan bind, NO party-state scaffold) + injected pad
     (the keypress stand-in) -> Fei walks (405->666->906) and Citan FOLLOWS
     with the correct slot-1 spacing (dist 9 = the 0xA gate), keeping the
     0x1000000 follower flag -- the wall-5/6 walk-follow now REAL under real
     boot, not scaffolded. The "scripted intro on the windmill" the framing
     expected is the PRE-Lahan sequence (FMV + Fei's-house scene) the port's
     boot SKIPS -- i.e. items #1 (movie decoder) + this #2 (NG->intro->Lahan
     routing), NOT an in-field lock. No code change; nothing to fix here.
  3. MAP14 intro ripple banding -- COSMETIC, already filed below.
  4. Village interaction walls -- func_8008399C "button/special
     interaction" RESOLVED: implemented from asm 80083A98-80083BFC
     (0x100 whole-map button targets: confirm-edge talk 2/3 facing the
     player, else passive arm 3/4 + the D_800ADF64/player-sprite+0x10
     latch for 0x8000000 actors; 0x80-only clears the latch). MAP15 now
     PLAYS+SOUND (19/24; probe: its 0x880-flag actor exercises the
     clear-latch path each poll, latch stays 0 -- retail-faithful).
     func_80084158's 5 targeting asserts RESOLVED: the 3 machines behind
     them implemented from asm (select-target ride latch .L80084520,
     standing-on-top momentum transfer .L80084570 with the 0xE3 ramp
     counter, post-loop fresh-commit record snapshot .L80084718 +
     func_800825AC pairing). Also fixed a pre-existing A40 transcription
     bug the machines feed: func_80084A40 dropped retail's FIFTH argument
     (targetState, 0x110(sp)) and conditioned its sprite+0x84 ground
     writes on y instead -- with y=0x7FFFFFFF (199/200 calls, probe) the
     old code wrote +0x84=-1 to every actor every frame. Now threaded +
     conditioned per retail; boot fade pacing shifts (converges to the
     identical lit scene, brightness 70.0 vs 71.2) -- retail-correct.
     The deep branches (ride/momentum) await platforming beats to fire;
     transcription is asm-verified, no spurious firing at boot.
  5. MAP3 SEGV -- SCOPE CORRECTED (the "1 loader, 1 pass" estimate was 1
     of at least 4 walls; probed end-to-end this pass):
     LANDED: (a) opcode 0x107 func_8008D604 implemented -- as a no-op stub
     it never advanced the IP, so the VM executed the FE-prefix's
     extension byte as a BASE opcode, desyncing every script stream that
     used FE07 (MAPs 3/160/200; 200 still plays, now with a CORRECT
     stream). (b) func_80077884's heap-size host-pointer fix (retail
     sizes by the PSX gap [0x801DC008, D_800ADB30); port sizes by
     ArchiveDecodeAlignedSize(0x6B9), the misc4.c precedent) -- was a
     ~5MB bogus alloc -> GameHandleError(130) spin. (c) the
     member_change_menu overlay boundary guard in func_80077AB4.
     STAGED OFF (XENO_FIELD_OBJECT_OVERLAY): func_800A1364 itself --
     asm-verified, binds the sprite (probe: actor 4 bound) -- but
     activating it arms the object pipeline: D_800B2264 != 0 ->
     func_80077884/AB4 stream per-slot model archives (0x6BA/0x6BB+id)
     and instantiate through the member_change_menu overlay
     (func_801E738C/func_801E742C at 0x801E7xxx, UNPORTED and
     UNEXTRACTED -- no asm split exists), and the now-synced streams
     reach func_800821F4's battle-animation assert on maps 16/80 (which
     play today only via the FE07 desync).
     (d) 2026-07-19 -- the cross-actor NULL on MAP3 (func_8009EB78 SEGV,
     garbage actor index 128, ~99 frames) is FIXED (1c48ed4). Same FE07
     IP-desync class: func_80088198 (IP += 1, snapshots 20 GameState
     per-slot entries) and func_8008B180 (IP += 5, gated obj-anim drive
     via func_801E8330) were no-op stubs that never advanced the IP,
     desyncing MAP3's script until a garbage index reached func_8009EB78.
     Ported both to plain C (func_8008D604/FE07 convention). MAP3 now
     boots 200 frames + renders 16.7% nonblack. Dispatch probe confirms
     the tripwire maps (000/001/014/047/334) hit neither handler (0/0 vs
     MAP3's 1/1) -> change provably inert there, watchdogs EXACT. Only
     func_800230A8 of the 3 stubs remains unaudited (not on MAP3's boot
     path -- MAP3 boots without hitting it). MAP3's full flip still needs
     the overlay extraction + model-instantiation port and the
     func_800821F4 branch -- a sub-project, not a pass. Flip the define
     when it lands.
     (e) 2026-07-19 diagnostic (current-blocker pass, no code): re-booted MAP3
     on the current binary to find its CURRENT blocker (post-FE07-fix). RESULT:
     MAP3 has NO crash blocker left -- it boots 220 frames, NO SEGV, NO assert,
     PAST func_8009EB78. Its remaining gap is RENDER COMPLETENESS: renders only
     16.7% nonblack (a dark-red region on black) vs ~74% for a playing map
     (MAP7). Characterized (asm-confirmed, ruling classes out): NOT a crash/
     assert/stall (not the FE07/prim-table/opcode bounded classes); NOT the
     object-overlay DRAW (func_801E742C/738C = 0 calls/frame -- not the gap);
     the model-build path func_800748E8/func_8002C700 is a one-time build, 0
     per-frame calls on ALL maps incl. playing ones (the per-frame field render
     is OT/DrawOTag-based, not func_8002C700). So MAP3's gap is a RENDER-PATH
     completeness issue (the object/model-heavy scene doesn't fully draw),
     shared with the object-overlay convergence + the menu render -- the DEEP
     piece, NOT a bounded flip. MAP3 has advanced boot-SEGV -> un-SEGV'd ->
     func_8009EB78 crash -> FIXED -> now boots clean, render-partial; the
     remaining gap is the render/object-overlay sub-project. Bank at 21/24.
  6. Cutscene opcode walls: func_8001FBE4's 0xBC family LARGELY RESOLVED --
     the 3 0xBC-internal asserts became a full implementation of the
     position opcode: 21 of 27 sub-commands live (target/parent/own/anchor-
     table/height-offset/screen-center/track-table sources, the anchor
     machine with its +0xAC-bit-2 mirror, the camera-relative tail with the
     lhu translation adds, the bit7-clear parent-anchor path with its
     DISTINCT +0x3C-bit-3 mirror), ReadGeomOffset added to psyq_compat.
     Synthetic gdb probe verified both store destinations exact. STILL
     ASSERTED (named): subs 1-4/0x19-0x23 (need the animation system's
     player global D_800C3E1C + party list D_800D363C, whose writer lives
     in temp1's unported sprite-spawn region -- THE wall under the player-
     relative cutscene family), sub 5 (retail divides zeroed accumulators
     by a stale register -- indeterminate), sub>=0x27 (uninit-stack UB).
     0xBC fires in NO map's boot window (probed 14/1/0) -- story-triggered
     only. Also remaining in this class: func_8001FBE4's dispatch-path
     catch-all (all OTHER unimplemented opcodes -- surfaces per-opcode as
     cutscenes run), func_80022660 bytecode path, func_800248D4 opcode
     paths (2 asserts; the post-menu anim landmine). NOTE: slus baseline
     hash moved to 7a3e773b (animation_scripts.c compiles into matching
     slus; single-file-revert isolation proved the diff is confined).
  7. Party-change walls RESOLVED: func_800815F0's D_800B234E==0 branch is
     the follow-the-leader machine, implemented from asm (.L800816F8-
     80081C04): followers consume the player's movement-history ring
     (0x48-stride entries the ported func_80081C54 records; head
     D_800B2360[0] decrements, per-slot cursors at (&D_800B2360)[partyId]),
     with idle-settle, airborne step-gates (0xA slot 1 / 0x14 else),
     caught-up hold, and the D_800B21CF==1 snap that SKIPS the caught-up
     check and consumes with the snapped cursor while keeping the old
     entry's 0x800 flag. func_800A24C4's initialized branch implemented
     (func_800A22AC(2) + func_800AD898 -- the 60-line 3D334.s leaf, now
     ported in game_overrides: per-party-slot gamestate+0x22B1 gate ->
     flags 0x200 set / 0x500 clear -- object-slot refresh loop via the
     overlay entry func_801E8330 (auto-stub, fail-visible, EMPTY while
     D_800B2264==0), player -8 Y nudge while interacting, and the global
     scroll-drift application). Probe: sync branch is the LIVE path at boot
     (B234E=0, ring recording, head walking), zero followers on solo maps
     so it scans-and-skips exactly as retail; the consume path awaits a
     party-of-2+ story state (the Citan join). A24C4 stays gated until
     g_GamePartySkinsInitialized sets (the join init).
  7b. Citan-join beat (wall 6, diagnostic pass): the ENTIRE add-member
     opcode chain was unported and is NOW IMPLEMENTED (5 fns, ~540 asm
     lines, all callees ported): func_8008BC80/BDD8 (add-member opcodes,
     arg/immediate variants; busy-retry IP-1+yield, free-slot -> roster
     write + skin kickoff, already-member -> +0x1D30 roster bit),
     func_8008A7DC (skin-archive staging, field charId+5 / gear
     GetGearID+0x10+5, D_800ADBC4=1), func_8008B894 (wait opcode: sync ->
     LZSS into g_PartyDataBuffers[slot] -> activate -> 0xFF re-arm),
     func_8008B978 (activation: roster bit, find the actor declaring the
     char via script-0 `16 <id>`, re-init + place at player + run script 0
     + settle, gear-map variant, full VM-context save/restore).
     FOLLOWER SYSTEM PROVEN LIVE (scaffolded diagnostic on MAP1: real
     party-of-2 preconditions reproduced, Fei walked via pad injection):
     the follower TRAILS FEI'S PATH with the slot-1 spacing (dist 9-10 =
     the 0xA gate), walk anim while moving, idle-settle at rest -- the
     wall-5 machine works.
     WALL (a) NEW-GAME GAMESTATE INIT: RESOLVED (wall 7). func_8001B970
     implemented (temp3.c): loads the new-game save TEMPLATE from archive
     0x10 file 3 (0x2358 bytes) into g_GameState, decodes the 31 character-
     name records via func_80033B34 (36-line leaf, now in system.c),
     resets the sound-volume block (controller half faithful; the 0x20
     bytes of UNNAMED BSS below it are host-unaddressable and only matter
     for a return-to-title flow -- documented divergence), sets the two
     mode bytes. Wired into the NG menu path ONLY (game_overrides
     PcPort_BootMain; field-test launchers keep the zero-state harness).
     THE TEMPLATE REVEALED: retail starts with [Fei, Citan, empty] --
     roster [0,2,FF], rosterBits 0x0005; Citan is in the party FROM NEW
     GAME (the join opcodes serve later party changes). The already-ported
     GamePartyCharactersInitializeSkins syncs gamestate->runtime at field
     entry. Also fixed en route: func_800A08B8's partyId!=0 assert was
     retail's shared follower-bind tail (bnez .L800A0978 skips only the
     player-index writes) -- restructured; Citan's field actor (2) now
     binds with the REAL follower flag 0x1000000 under NG boot, Lahan
     renders, Fei is player actor 1. NG-boot movement is input-locked by
     the scripted intro (retail behavior) so the live follow ran under the
     wall-6 scaffold; A790 proves free-slot(newChar->slot 2)/already-
     member(Citan) with the real roster.
     REMAINING WALL: (b) story-flag progression to later join scripts (no
     scenario harness; canonically post-attack) + the intro input-lock
     release path for interactive NG play (confirm-edge injection).
  8. MAP16 Blackmoon Forest "mostly black" -- DIAGNOSED (wall 9,
     read-only): NOT a BG-layer / lighting / palette bug. The forest
     ENVIRONMENT IS OBJECT-BASED and the object loader is the SAME staged
     wall-3 subsystem (func_800A1364). Evidence: (a) partially black --
     one foliage clump draws correctly-textured/lit in the corner, rest
     black; (b) CullCam: seen=56 emit=1 overlap=54 (vs working MAP17
     seen=1598 emit=286) -- geometry almost entirely absent, not culled-
     dark; (c) model builds = 3 (6 groups) vs MAP17's 90 (151) / MAP5's
     23 -- the terrain/forest models are not building; (d) func_800A1364
     (object-sprite loader, wall-3 staged -> port auto-stub) is called
     ~68x/frame (4100 in 60 frames; MAP17 calls it 0x) and D_800B2264
     object count stays 0; (e) actor 3's script is STUCK spinning on the
     stubbed opcode -- ip=7fff fixed across all 4100 calls (the no-op stub
     never advances the IP), so the forest objects never load. The 3
     models that do build are the base terrain fragment (the drawn clump).
     CAUSE: the object-overlay sub-project (already scoped at #5/wall-3) --
     MAP16 black and MAP3 SEGV share this root (func_800A1364 + the
     member_change_menu overlay func_801E742C/738C + the func_800821F4
     battle-anim assert). NOT bounded; fixing MAP16 = the object-overlay
     pass. Diagnostic: all gdb + the existing XENO_CULL_CAM_LOG feature;
     no code changed. Blackmoon Forest is dark-themed but this is a
     genuine missing-geometry BUG, not authored-dark.
  9. THE LAHAN-ATTACK SCRIPTED BATTLE -- the battle system is entirely
     unported (field->battle boundary is the no-op func_80281204;
     func_800821F4 "battle animation branch" assert; menu/battle overlay
     arc unstarted). THE ARC-ENDING WALL: "playable Lahan" ends at the
     attack trigger without it. Multi-pass subproject; needs its own
     scoping.
  Boot-stub census (benign at boot, unknown in-story): func_80028B14
  (libarchive, 231L, fires broadly), func_80098430 (misc7, 51L, map1),
  func_80091BBC (misc11, 149L, maps 7/11/12/13/19/60/143), func_8003A450
  (sound, 48L), func_80089FD0 (50L)/func_8008F90C (80L, map16).
  STALE known-state note: func_8009635C (item-give) is IMPLEMENTED.

(D) BLOCKERS: #4, #5, #6, #7, #9 (+#8 if real). COSMETIC: #1, #2, #3.

(E) MENU DEPENDENCY: the Lahan story chain forces NO menu (talk/cutscene/
  fetch beats only; save points optional). Menu overlay stays DEFERRABLE
  for story-playability -- but a user-opened menu returns into the
  func_800248D4 post-menu anim assert (landmine, part of #6).

(F) CRITICAL PATH (ordered): #4 interaction branches -> #5 MAP3 loader ->
  #6 cutscene opcodes as-hit -> #7 party-join walls -> #8 forest triage
  -> #2 NG-flow routing -> #9 battle system. Estimate: #4-#8 ~5-7
  bounded passes (each far-color/loader-sized) = story-walkable Lahan up
  to the attack; #9 is a separate multi-pass arc gating "fully playable."
  RECOMMENDED FIRST: #4 func_8008399C interaction branch -- the only
  blocker runtime-proven TODAY (MAP15 boot), bounded, and it unblocks
  both the mountain-path map and village interactions.

## Gameplay-completeness worklist (scoped 7774804; ranked, measured, no implementation)

Measurement pass turning "the game isn't complete" into a ranked target list.
Read-only: 24-map boot survey, opcode/TU enumeration (decomp_status.py),
dialog-path liveness, menu-overlay re-trace.

STATE OF PLAY (24-map survey, entrance 0, boot-to-~25s):
- 15/24 (62.5%) PLAY+SOUND clean (7,10,20,30,40,60,80,100,120,143,200 + the
  five watchdogs). The field engine is broadly working: field overlay is
  80.4% done (878 fns, 172 unported), dialog is COMPLETE.
- 9/24 fail in exactly FOUR signatures (a small root set, not scattered):
  1. SEGV cluster -- maps 2,3,160,400 (4/24). CORRECTED ROOT (the initial
     scoping's "one func_80021BCC root / stubs are red herrings" was WRONG --
     deeper trace overturned it): the crash is a CLASS of NULL-sprite writes,
     and the [stub] lines are NOT red herrings -- a STUBBED per-actor
     sprite-LOAD opcode leaves the actor's pSpriteData NULL, so its later
     boot-script sprite-write opcodes deref NULL. Retail's loader binds the
     sprite (calls func_80076AC0, writes actor+0x4), so retail's writes hit a
     real sprite; the port's stubbed loader -> NULL -> host fault. A null
     guard at the write site MASKS the missing sprite (NPC renders invisibly)
     -- rejected. Each map uses a DIFFERENT stubbed loader + crashes at a
     DIFFERENT sprite-write opcode:
       - MAP2: loader func_800A06E8 (op 0x121) -> crash func_8009E574 (setpos)
       - MAP3: loader func_800A1364 (calls binder) -> crash func_8009EB78
       - MAP160: crash FieldScriptVMHandlerEnableActorVM (loader TBD)
       - MAP400: crash func_80098A7C (misc7.c:253) (loader TBD)
     PARTIAL FIX (this pass): func_800A06E8 decompiled (coexistence, d88f13c
     pattern -- port C body binds the sprite, INCLUDE_ASM kept for matching;
     only ~24% fuzzy so not {}). MAP2 actor 3's sprite now binds
     (verified pSpriteData 0x6500a0); its SEGV is GONE -- but MAP2 then hits
     signature 2 (the prim-table gap, `missing D_8004FE50 prim=5`), so it is
     not yet PLAYS+SOUND. The SEGV cluster is thus a MULTI-LOADER sub-project
     LAYERED behind the prim-table gap, not a one-function/4-map flip.
     Remaining: decomp func_800A1364 (MAP3) + MAP160/400 loaders (identify),
     then the prim-table gap behind them. slus untouched; matching byte-exact;
     working maps + tripwires unregressed. Repro: boot map 2/3/160/400.
  2. Model prim-table gap -- RESOLVED for maps 4,50 (FLIPPED to PLAYS+SOUND);
     MAP2 cleared this signature and advanced to a NEW wall. Corrected scope
     (the "wire prim-0's buildProc" estimate was 1 of 6 pieces): the gap was
     (a) the whole [0x00] row absent -- its buildProc func_8002CDCC was
     UNPORTED (decomped: byte-identical retail clone of prim 8's
     func_8002CF58; coexistence in temp2.c, matching keeps INCLUDE_ASM), and
     (b) the depth-cued (DPCS fog) walker family for variants 4/5 entirely
     unported: retail 0x8002EF0C (tri AVSZ3+DPCS), func_8002F0E4 (tri
     min-SZ+DPCS), func_8002FCFC (FT4 AVSZ4+DPCS), func_8002FF0C (FT4
     min-SZ+DPCS) -- all four implemented in game_overrides.c (RGBC from
     D_80059598/99/9A, per-prim DPCS, packet color = code<<24 | RGB2) and
     wired at [0x05].proc[4]/[5] and [0x0D].proc[4]/[5] (+ retail-identical
     alias [0x05].proc[1]). Unported walkers left NULL (loud abort):
     [0x00].proc[1]=0x8002ED20, proc[3]=0x8002E8DC; [0x05].proc[3]=
     0x8002E8F0; [0x0D].proc[3]=0x8002EAF4.
     MAP2's far-color wall: RESOLVED -- MAP2 now PLAYS+SOUND with working
     distance fog (18 of 24 survey maps play). The branch was the per-actor
     SPRITE fog tint (retail 0x800760AC: RGBC <- model base color, DPCS by
     the actor's RotTransPers IR0, SpriteSetColor with the fogged RGB2 --
     the fog-on counterpart of func_80075B08). The whole fog-state chain was
     verified already-live before implementing: func_800748E8 refreshes
     base color + SetFarColor (RFC ctc2 21-23) + func_80048AB0 ->
     SetFogNearFar (DQA/DQB ctc2 27/28, real PsyX impl) each frame; PsyX
     RTPS/RTPT compute IR0 = DQB + DQA*h/sz3. Port-side impl only (matching
     keeps the assert text). Verified visually: MAP2 night scene, near
     geometry textured/clear, distant structures blending to the authored
     far color (60,60,80). func_80075B44's OTHER asserts (special actor,
     double-render, rotated actor) remain fail-loud -- untriggered on MAP2.
  3. Model-walker assert -- map 25 (1/24): `modelData+0x12 != 1` (temp2 model
     walker hits variant 1, unhandled). Map 15: `func_8008399C button/special
     interaction branch not migrated` (filed separately). One decomp gap each.
  4. Hang -- map 250 (1/24, SIGKILL/timeout): a live stub cluster
     (func_80088508/8861C/888A4...) spins. Larger (needs those fns ported).

## Non-playing map re-survey (2026-07-19, read-only, HEAD 3c0ad64, ranked)
Re-booted the non-playing candidates on the current binary to find the CHEAPEST
flip (19->20). MAP15 has FLIPPED to playing since the 7774804 roster: it boots
220 frames, renders 99.9% nonblack, and func_8008399C (its "not migrated"
interaction gap) is now a real ported C fn (misc8.c:1616) -- roster stale. So
the ~5 non-playing maps are now 3, 25, 160, 250, 400. Blockers + rank:
  #1 CHEAPEST -- MAP25: stops at frame 99 on assert(*(s16*)(modelData+0x12)!=1)
     in func_800748E8 (misc2.c:1586). modelData+0x12 is the MODEL PRIM-TYPE that
     flows to func_8002C700(...,type) at misc2.c:1686 -- i.e. this is the
     PRIM-TABLE family (signature 2), type 1 unhandled. BOUNDED + KNOWN pattern:
     the roster already extended the prim-table for types 0/5/D + the variant 4/5
     DPCS walkers (game_overrides.c). Fix = wire the type-1 buildProc/walker +
     drop the fail-loud assert. Localized, no known layered wall behind it.
  #2 MEDIUM -- MAP160 + MAP400: the NULL-sprite-loader class (signature 1).
     MAP160 SIGSEGVs in FieldScriptVMHandlerEnableActorVM (misc6.c:80) -- VALID
     actor index but g_FieldActors[idx].pActorData == NULL. MAP400 SIGSEGVs in
     func_80098A7C (misc7.c:253, pSpriteData->position NULL). Both: the actor
     exists but a stubbed per-actor sprite-LOADER left its data NULL. Fix =
     identify + decomp the stubbed loader per map (bounded, ~1 fn each, the
     MAP2 func_800A06E8 pattern) BUT a layered prim-table gap likely behind
     (MAP2 needed both). Two maps, same class.
  #3 LARGER -- MAP250: hang, the func_80088508/8861C/888A4 stub cluster spins
     (reaches 40 frames but never progresses). Needs the cluster ported.
  #4 DEEP -- MAP3: boots 220 frames + renders PARTIAL, but its full gameplay
     flip needs the object-overlay convergence (stubs func_801E738C/742C/8330 --
     the menu.bin/forest sub-project, SHARED with the menu 3D-render core). Not
     a bounded flip.
  RECOMMENDATION: MAP25 is the nearest bounded win -- a single model prim-type-1
     gap, a known bounded prim-table pattern (localized to misc2.c:1586 + the
     func_8002C700 prim-table). Runner-up: MAP160/400 (bounded loader-decomp,
     but layered). MAP3/250 are the deep/larger tail.
  CORRECTION (2026-07-19, on attempting MAP25): the survey MIS-characterized
     MAP25 as a cheap "prim-table entry." Deeper read of the retail asm
     (func_800748E8.s, matched) shows modelData+0x12==1 triggers a TYPE-1
     LIGHTING BRANCH the port's hand-written (nonmatching) func_800748E8 stubbed
     with the misc2.c:1586 assert: after the normal matrix build it copies a
     matrix into s3 (sp+0x78), ScaleMatrix(s3, scale), func_80030B14(s3) (ported
     -> light matrix = D_80059F64 x s3, SetLightMatrix), then func_80030C40(
     D_800AFB04/06/08) -- the light-COLOR GTE regs, and func_80030C40 is UNPORTED
     (17-instr ctc2). AND func_8002C700 with a3=1 hard-aborts if any prim's
     proc[1] is NULL ([0x00].proc[1]=0x8002ED20 etc. are unported). So MAP25 =
     a delicate GTE lighting-branch port (mapping retail sp-local matrix buffers
     to the C's restructured vars -- error-prone, could render garbage) + the
     unported func_80030C40 + prim-table proc[1] walkers. This is the
     UNDER-ESTIMATE pattern, NOT a cheap flip. Re-ranked: MAP160/400 (the
     sprite-loader class, the PROVEN bounded MAP2 func_800A06E8 decomp pattern)
     is likely the cheaper actual flip; MAP25's GTE lighting branch is delicate.
  OUTCOME (2026-07-19, a923c1c): pivoted to MAP160/400 and MAP160 FLIPPED. It
     was NOT the sprite-loader class after all -- it was the FE07-class IP-desync
     (garbage actor index 128, same as MAP3's boot-SEGV): func_8008F1C8, a
     field-script opcode handler that copies two operand triples into the actor's
     color fields + advances the IP by 8, was a no-op stub -> desync -> garbage
     0x80 to FieldScriptVMHandlerEnableActorVM. Ported (func_80088198 pattern) ->
     MAP160 boots 220 frames + renders 75.4%, tripwire 0-hit inert. 21/24 play.
     MAP400 RE-CHARACTERIZED: genuinely the sprite-loader class (NOT a desync) --
     g_FieldActors[D_800AFD1C].pSpriteData is NULL (valid actor, IP=680 sane), so
     actor D_800AFD1C's sprite-loader is stubbed. func_80023290 (misc.c:1804) is
     a red-herring CONSUMER of pSpriteData, not the loader. MAP400's real fix is
     tracing + decomping the loader that should bind D_800AFD1C's sprite (the
     multi-step MAP2 pattern) -- more involved than MAP160's clean desync fix.
     Remaining non-playing: 3 (object-overlay, deep), 25 (GTE lighting, delicate),
     250 (hang cluster), 400 (sprite-loader). Next cheapest is likely MAP400's
     loader (proven pattern) or MAP250's hang cluster.
  MAP400 RE-CHARACTERIZED AGAIN (2026-07-19, survey-signature-is-hypothesis
     fired a THIRD time -- no code): confirmed NOT a desync (D_800AFD1C=19 valid,
     < g_FieldNumActors=62), so genuinely the sprite-loader class -- but NOT the
     clean MAP2 stubbed-opcode pattern the survey implied. The real mechanism: in
     func_800A28D4 (the field-script boot fn, hand-ported) with
     g_GamePartySkinsInitialized=0 (the correct first-load PATH 2 -- path 1
     early-binds + returns, gated on a party-skin-init flow not met on a cold
     boot), the boot loop (D_800ADBFC=20 actors) runs each actor's boot script
     THEN late-binds its sprite (func_80076AC0, line 658). Actor 19 (the LAST
     boot actor) runs setpos func_80098A7C (0xFE-prefixed, its script's 1st op at
     IP=680) which writes pSpriteData->position -- BEFORE its late bind, and
     retail's func_80098A7C has NO NULL guard, so retail's actor 19 genuinely has
     a sprite by then. HOW retail binds actor 19 before its setpos on path 2 is
     UNRESOLVED (no stubbed load opcode in actor 19's script; candidates:
     FieldScriptVMRun yield handling runs too far in the port, the boot-loop
     structure dropped a pre-bind, or the script offset differs). This needs
     deep func_800A28D4/FieldScriptVMRun flow analysis -- NOT a bounded flip.
  META (cheap map-flips exhausted): MAP15 (already flipped) + MAP160 (FE07
     one-function fix) were the cheap ones. The remaining 4 non-playing are ALL
     genuinely involved: 400 (deep VM boot-flow, above), 250 (hang cluster --
     port func_80088508/8861C/888A4), 25 (delicate GTE lighting branch), 3
     (object-overlay convergence, shared with the menu render). So 21/24 is the
     natural bank point for BOUNDED flips; further gameplay progress is the
     deeper sub-projects (VM boot-flow, object-overlay, or the menu render).
  MAP400 ROOT CAUSE (2026-07-19, pushed deeper -- no code, not a bounded flip):
     traced actor 19's full boot script. Each boot actor (D_800ADBFC=20, so 0-19)
     runs a 4-opcode boot script [first],42,27,0 then gets late-bound (658).
     CONFIRMED it is actor 19's OWN script (curActor==g_FieldActors[19].pActorData)
     and its first opcode is 0xFE->func_80098A7C (setpos), which writes actor 19's
     pSpriteData->position -- BEFORE its late bind. Actors 15/16/18 also start
     0xFE but a DIFFERENT extended opcode (non-sprite), so only actor 19 crashes.
     The FE dispatch is CORRECT (FieldScriptVM2Run u8 cast, a past bug already
     fixed). KEY: actor 19's skinId(0x126)=0x0 -- NORMAL, identical to actor 0
     (which binds fine) -- so actor 19 is NOT a special-sprite/stubbed-loader
     actor. It is a pure ORDERING/CONTEXT issue: actor 19's setpos-first script
     assumes its sprite is ALREADY bound, which only happens via the PATH-1
     early-bind (func_800A28D4's g_GamePartySkinsInitialized!=0 branch). The
     harness cold-boots (path 2, party skins NOT initialized) so the early bind
     never runs. MAP400 is a late-game map whose scripts assume the party-skin-
     initialized context; the cold harness boot is unrepresentative. => NOT a
     stubbed loader, NOT a bounded code fix -- most likely a HARNESS/CONTEXT
     limitation (MAP400 would plausibly play when reached in real gameplay with
     party skins initialized -- UNTESTED). The survey "sprite-loader/MAP2 pattern"
     label was wrong (signature-is-hypothesis, 4th time). Bank at 21/24.

DIALOG: COMPLETE. Both TUs (field/dialogue/text_box.c, text_box_render.c)
are 100% matched {} -- ZERO INCLUDE_ASM. The render path executes live
(frame captured on MAP001). Memory [[dialog-dismiss-edge-gated-confirm]]
confirms text boxes advance on edge-gated confirm. NOT a wall. (The filed
Map143 dialogue SIGSEGV @ e3f4b49 predates the render-fix arc -- MAP143 now
boots PLAYS+SOUND; that crash needs re-verification under the interaction
repro, likely stale.)

OBJECT-OVERLAY ARC == MENU.BIN ARC (wall-11 scope, read-only): KEY FINDING
-- the object-instantiation code that blocks MAP3 (wall 3 SEGV) and MAP16
(wall 9 black forest) is IN menu.bin. menu.bin spans VRAM 0x801C5000..
0x801EA908 (0x25908 bytes); the menu render tree lives at 0x801C5xxx and
the FIELD-OBJECT instantiation functions at 0x801E7xxx -- both in the same
overlay. The field jal's directly into it (func_80077AB4: jal func_801E738C
/ func_801E742C), so menu.bin is resident during field play and its high
region serves field object rendering. => bringing up menu.bin's infra is
the SHARED prerequisite for the menu system AND the MAP3/MAP16 gameplay
unblock. (Wall 3's "member_change_menu overlay" attribution was wrong: MCM
is 0x801c5000..0x801cb800, too small to reach 0x801E7xxx.)

PHASED PLAN (object-overlay / menu.bin convergence):
  Phase 1 -- menu.bin BUILD-INFRA: DONE (commit below). config/menu.yaml +
    config/symbol_addrs.menu.txt + gears.toml overlay entry + generated
    src/menu/main/misc.c (314 INCLUDE_ASM). menu.bin (VRAM 0x801C5000,
    0x25908) now splits (319 functions), builds, and is disassembled.
    Baseline: the MENU RENDER TREE matches 100% (+0 for the 310 tree fns);
    overall 93.83% -- the residual is an ISOLATED +8-byte shift in the
    object region from splat's handling of 2 FALL-THROUGH object entries
    (func_801E7378/7D14 begin mid-flow after a load-delay nop), which is
    MOOT (Phase 2 rewrites those as C). MAIN SLUS INTACT (a55929a1); both
    overlays (member_change/shop) unchanged; port build LINK OK, MAP1/14
    PLAYS+SOUND. THE KEY DELIVERABLE: the object subtree is now visible.
  Phase 2 -- OBJECT INSTANTIATION: SCOPE CORRECTED (analysis pass, the
    Phase-1 "9 clean functions" was an artifact of the seeded mid-function
    labels). The 8 field object jal-targets are ALL MID-FUNCTION ENTRY
    POINTS -- the field jals into the MIDDLE of 5 larger menu functions at
    hand-optimized shared-code fragments (they use loop regs like $s0
    before saving and share the enclosing function's frame/epilogue;
    verified: func_801E742C's split .s ends with func_801E733C's -0x30
    epilogue). MAP of target -> enclosing function:
      0x801E72CC -> func_801E71B4 (98 instrs, +0x118 in)
      0x801E7378/738C/742C -> func_801E733C (236 instrs; THREE entries)
      0x801E7D14 -> func_801E7C50 (134 instrs, +0xC4 in)
      0x801E7FD4 -> func_801E7E68 (108 instrs, +0x16C in)
      0x801E8030 -> func_801E8018 (11 instrs)
      0x801E8330 -> a leaf/no-frame fn
    REAL SCOPE: 5 enclosing functions, ~587 instructions, with 7 of 8
    entries mid-function. This is a MEDIUM port with a MULTI-ENTRY twist
    (C can't express jal-into-mid-function): port each enclosing function
    as C, exposing the field's entry points as separate C funcs that
    replicate the code path from the entry to the shared return. The
    PSYQ-compiler tail-merging/cross-jumping means matching those blocks
    as {} is likely impossible -> coexistence (INCLUDE_ASM keeps the
    whole enclosing function; port C provides the functional entries).
    Callees ARE all ported PsyQ prims (subtree bottoms out), so no deeper
    unported deps -- the ~587 instrs are the whole job.
    STEP 1 (un-seed) DONE (commit b200bbf): removed the mid-function seeds;
    the 5/6 real functions are whole; menu.bin 93.83% -> 99.87% (the +8
    shift GONE, size matches exactly), OBJECT REGION 0x801E7xxx 100% CLEAN
    (the 204 residual bytes all in the menu tree 0x0-0x84b8, Phase-2b's
    concern). Required `gears clean` + DELETING src/menu/main/misc.c so
    splat regenerated it fresh (splat preserves an existing .c, so a
    partial regen left stale func_801E742C refs -- delete-then-split).
    STEP 2 (the port) SCOPE CONFIRMED: 6 SELF-CONTAINED menu functions --
    func_801E71B4, func_801E733C (236i 3-entry core: 738C setup / 742C
    per-object instantiate), func_801E7C50, func_801E7E68, func_801E8018,
    func_801E927C (SetPolyFT4 helper). ALL 13 external callees ALREADY
    PORTED (DrawSync/GetClut/GetStringEntry/GetTPage/HeapAlloc/Free/
    LoadImage/SetPolyFT4/SemiTrans/ShadeTex/SystemRenderStringEntry/bzero/
    func_80033B34) -- no deeper deps. func_801E742C writes object render
    data to the g_Menu object-slot array at +0x2784.. (per-object position/
    texture params); the model-return D_801E8670 is populated MENU-SIDE
    (func_801E8474 via func_801C58EC/D2D38, NOT the field render path), so
    the field render consumes the g_Menu slots. PORT AS COEXISTENCE:
    INCLUDE_ASM keeps the whole enclosing functions (object region now
    100%), port C provides the functional field entries.
    STEP 2A DONE (analysis): the g_Menu object-slot at +0x2784 is a
    POLY_FT4 SPRITE ARRAY (0x28 stride, standard PSX textured-quad prim:
    xy0-3 / uv0-3 / clut / tpage / rgb-code -- the sh,sh,sb,sb write
    pattern at 0x2784/86/88/89, 0x278C.., 0x2794.., 0x279C.. is 4 verts).
    Addressed as g_Menu[0x34C] + 0x2784 + (i + g_Menu[0x308]) * 0x28.
    func_801E742C writes one POLY_FT4 per object (position from x/y/vec
    args, texture from GetTPage/GetClut on the loaded buffers).
    ARCHITECTURE (Step-2A discoveries -- they reshape the phasing):
    (1) the POLY_FT4 array is DRAWN by the MENU RENDER (func_801C7BF4
    DrawOTag), NOT the field -- the menu objects' VISUAL rendering is
    entangled with Phase 2b. func_801E7C50/7E68 are also called by ~10
    menu-internal fns (shared menu helpers).
    (2) func_80077AB4 EARLY-RETURNS if D_800B2264 == 0; on MAP16 the loader
    stub keeps D_800B2264 = 0, so func_801E742C is NEVER reached until
    func_800A1364 (Phase 3) activates and registers objects. So the Phase-2
    port is required to SAFELY activate func_800A1364 (Phase 3) -- without
    it, activate -> registers objects -> func_801E742C stub -> wall-3
    regression.
    (3) MAP16's FOREST is FIELD MODELS, blocked by func_800A1364 SPINNING
    (ip=7fff, script stuck), NOT by func_801E742C. Activating func_800A1364
    (Phase 3) un-spins the script -> the forest models build. The menu
    objects (POLY_FT4s) are a SEPARATE menu-side render (Phase 2b), likely
    NOT needed for MAP16's forest.
    NET: Phase 2 makes the object pipeline SAFE; Phase 3 activates the
    loader -> MAP16 forest builds (field models) + MAP3 boots; the menu
    objects' own render is Phase 2b.
    STEP 2B (code pass -- proved the port target is the WRONG code, so no
    faithful/functional C was written; this is a substantiated STOP, not
    analysis drift):
    (A) 7 of the 8 field-imported object entries (func_801E72CC / 7378 /
        738C / 742C / 7D14 / 7FD4 / 8030 / 8330) are MID-FUNCTION entries.
        Their first instructions consume menu-CONTEXT register/global state
        the field's plain-argument calls never establish. DISASM PROOF:
        - func_801E738C: `addu $a0,$a0,$v0` -- consumes uninitialised $v0.
        - func_801E742C: `addu $v1,$s0,$v1` -- consumes menu-loop $s0
          (=iter*2) + $v1 (=g_Menu[0x308], loaded by the SKIPPED prior
          instr). The field's caller (func_80077AB4) leaves $s0 =
          &D_8005A450[i] (a ~0x8005xxxx pointer) -> garbage slot address.
        - func_801E7D14: `addu $v0,$s3,$v0` -- consumes menu-loop $s3.
        - func_801E8330: `lw $v1,0x308($a1)` while the field caller
          (func_800A24C4) passes $a1 = 0 -> reads absolute addr 0x308.
        Only func_801E927C is a CLEAN standalone fn (SetPolyFT4 + SemiTrans
        + ShadeTex + rgb=0x80); the field does not import it directly.
    (B) The menu.bin code at 0x801E742C is func_801E733C's 16-iteration
        MENU-sprite loop: it writes FIXED atlas UVs (D_801EA04C/D_801EA050)
        and FIXED GetTPage(0,0,0,0x140)/GetClut(0,0x1C0) -- it does NOT use
        any per-object texture/position. So the code's ACTUAL behaviour (a
        menu's fixed 16-sprite atlas) != the field's NEEDED behaviour
        (per-object textured sprite from bufTex at x/y). Same bytes cannot
        do both. Step 2A's "func_801E742C writes one POLY_FT4 per object
        from the x/y/vec args" described the MENU loop and MIS-ATTRIBUTED it
        to the field function; the field's object-draw routine is not
        coherently present at these addresses. A faithful port cannot
        satisfy the field ABI; a "functional" port would be reconstructing
        behaviour the asm does not reveal (== guessing, forbidden).
    (C) THE REDIRECT (what actually unblocks MAP3/MAP16): func_800A1364 (the
        object-REGISTER opcode, staged) does MORE than register -- it (1)
        loads the object's sprite via func_80076AC0(g_FieldSpriteData), (2)
        advances the script IP by +3 (UN-SPINS the stuck script), THEN (3)
        registers the object (D_800B21DC[D_800B2264]=spriteId; D_800B2264++).
        Effects (1)+(2) are the real MAP3/MAP16 unblock (sprite loaded +
        script un-spun -> field models build); effect (3) merely populates
        the object COUNT that later drives the (incoherent) draw path.
    NEW PLAN (supersedes "port the 6 fns"): the object-DRAW port is the
    wrong/blocked target. Do NOT port func_801E742C faithfully (impossible)
    or functionally (guessing). Instead Phase 3 implements func_800A1364's
    load + IP-advance; func_801E742C/738C/7D14/8330 become SAFE no-ops
    (field calls resolve to non-corrupting stubs -- objects simply don't
    draw, which is a Phase-2b concern and NOT needed for the forest/boot).
    OPEN EMPIRICAL Q (needs runtime, gated on Phase 3 un-stage): once the
    script un-spins, does MAP16's forest appear from FIELD MODELS with the
    object-draw as a no-op? If yes, the func_801E742C port is never needed;
    if the objects are load-bearing, capture the LIVE g_Menu writes under
    GDB to recover the real draw behaviour (the only non-guessing source).
  Phase 3 -- ACTIVATE (now the FIRST real code step, per the Step-2B
    redirect: it no longer waits on a func_801E742C faithful port):
    (1) implement func_800A1364's body -- FieldScriptVMGetArgument -> sprite
        id, func_80076AC0 load, func_800A0C94, actor flag bits, IP += 3
        (un-spin), and the object-register tail (D_800B21DC[cnt]=id;
        D_800B2264++). This is a self-contained field-script opcode with NO
        801E dependency in its own body.
    (2) provide SAFE no-ops for the field's object entries
        func_801E742C/738C/7D14/8330 (+72CC/7378/7FD4/8030) so the
        post-register draw loops (func_80077AB4 / func_8007520C /
        func_800A24C4) don't jal into incoherent menu code -- objects don't
        draw (Phase 2b / TBD), but the pipeline is non-corrupting.
    (3) unstage func_800A1364 (XENO_FIELD_OBJECT_OVERLAY on); implement
        func_800821F4's battle-anim branch (asm 800822D8-80082360, ~30 lines,
        calls func_801E8330 x2); port MAP3's 3 stubs (func_800230A8 34L /
        func_80088198 23L bounded leaves, NO overlay dep -> can be done
        ANYTIME; func_8008B180 39L).
    VALIDATE (empirical, the decisive test): MAP3 boots (no SEGV); MAP16
    forest builds from FIELD MODELS once the script un-spins (seen>>56).
    If the forest appears with object-draw as a no-op, the func_801E742C
    port is confirmed UNNEEDED. If not, GDB-capture the live g_Menu object
    writes to recover the real draw (only non-guessing path).
    ===== PHASE 3 DONE (empirical results, build validated) =====
    IMPLEMENTED: (1) func_800A1364 activated for the port via
    -DXENO_FIELD_OBJECT_OVERLAY in pc_port/build_port.sh (its C body already
    existed, asm-verified; the flag flips it from a stub to the live un-spin).
    (2) the draw entries func_801E742C/738C/7D14/7FD4/8330 are the port's
    auto-stubs -- confirmed SAFE no-ops (xeno_port_stub logs once + returns,
    never aborts). (3) func_800821F4's battle-anim branch WRITTEN (misc8.c,
    replaces the assert; asm .L800822D8-.L80082360: object-slot anim-state
    writes into &D_800B2346-0x162 + no-op'd func_801E8330 draw). (4) a NEW
    blocker surfaced + fixed: func_80075B44 (misc2.c) asserted on the
    object-actor render branch (actorFlags4 & 0x2000, the flag func_800A1364
    stamps) -- replaced with `continue` (skip the object-actor draw, Phase
    2b; field-model actors still render).
    THE DECISIVE ANSWER -- func_801E742C is NOT needed for MAP16's forest:
    with the object-draw fully no-op'd, MAP16 boots 200 frames (was blocked),
    D_800B2264=4 (4 objects registered by the un-spin), and a FOREST TREE
    RENDERS (green foliage model, top-right, GL glReadPixels ground truth) --
    a FIELD MODEL, drawn by the field's own system once the script un-spun.
    The object POLY_FT4 draw is proven off the forest's critical path.
    (The rest of the MAP16 frame is sparse/black at the spawn camera -- a
    separate camera/actor-population question, NOT the object overlay.)
    MAP3: the un-spin WORKS (99 frames, was a boot-SEGV), but a downstream
    NON-object blocker remains: func_8009EB78 (misc6.c:446) NULL-derefs
    g_FieldActors[actorIndex].pActorData where the un-spun script passes
    actorIndex=0x80 (out of range for the ~52-slot actor array; the C is
    asm-faithful, no dropped mask). This is a MAP3 script/actor-load
    semantics issue (why does the script reference an unloaded actor 0x80?),
    separate from the object overlay -- the next MAP3 blocker to investigate.
    TRIPWIRE/BASELINE INTACT: all five maps 000/001/014/047/334 stay at
    D_800B2264=0 (they never invoke func_800A1364), so the object code paths
    provably never execute -> render fingerprints unchanged, no crashes.
    slus sha256 a55929a1e1ea5563 UNCHANGED (misc2/misc8 are FIELD TUs, not
    slus); matching build 468/468 OK; port LINK OK.
  Phase 2b (SEPARATE, menu-system track, NOT needed for MAP3/MAP16): get
    the matched menu tree to RENDER in-game. SCOPED (diagnostic pass, HEAD
    c083879) across the 3 layers -- invocation / execution / draw:
    (A) INVOCATION -- WIRED + PORTED (not the blocker). func_800799D4 (field
        menu-opener, DEFINED in the port) is called from the field main loop
        (main.c:621) when D_800ADB64 is set (menu button) -> MenuMain ->
        MenuExecute (menu.c:129) -> switch(D_80059460): case 0 = func_801C62A8
        (MAIN menu), case 1 = MemberChangeMenuMain, etc. Menus CAN be invoked;
        the path runs. (g_MenuDebugEnabled=0 on the field path skips the
        overlay DATA-load -- irrelevant for the port's compiled C.)
    (B) EXECUTION -- THE BLOCKER for the main menu. func_801C62A8 (case 0) is
        a NO-OP STUB in the port (menu.bin's misc.c is INCLUDE_ASM -> nothing
        defined). Its transitive tree = 96 menu.bin functions, all unported.
        So the main menu is invoked but returns immediately (nothing draws).
        NB the old "~6 fns" estimate was WRONG -- it counted only the top
        dispatcher, not the 96-fn transitive closure.
    (B') PROOF-OF-CONCEPT ALREADY EXISTS: member_change_menu (case 1,
        MemberChangeMenuMain) is a PORTED menu overlay -- 68-fn tree, ~90%
        ported, only 12 stubs. So a ported menu overlay is nearly executable;
        the render architecture is not fundamentally broken.
    (C) DRAW DEPS -- mostly satisfied. Of 316 external callees in the
        func_801C62A8 tree, only 10 are STUBBED. GPU prims (DrawOTag/AddPrim/
        ClearOTagR) are PORTED. The 10 stubs (SHARED with member_change):
        SystemRenderStringEntry (the TEXT renderer, 59 instrs -- blocks text
        in ALL menus), func_80026338/263E4/2675C/2DD20/36410/3852C (43-222
        instrs, bounded), 2 sound (SoundMute/EnableAllSpuChannels).
    (D) MENU-SIDE OBJECT DRAWS -- part of the tree, COHERENT menu-side, NO
        Phase-3 conflict. The main-menu tree includes ~18 object fns
        (func_801E733C the 16-sprite POLY_FT4 loop, func_801C7BF4 DrawOTag,
        func_801E8xxx). Menu-side these are WHOLE self-contained functions
        (the menu provides the loop registers) -- and they are DIFFERENT
        SYMBOLS from the field's no-op'd mid-entry stubs (func_801E742C/738C).
        In the static port they coexist: porting func_801E733C (menu, whole)
        does NOT un-no-op func_801E742C (field, mid-entry). So the Phase-3
        field no-ops stay; the menu ports the whole functions. (These 18 are
        within the 96-fn count; likely DEFERRABLE for an initial window+text
        render -- they draw character/item sprites, not the window frame.)
    (E) SCOPE: main-menu render = BIG (~96 menu.bin fns + the 10 shared draw
        stubs), a MULTI-PASS port (an initial window+text render is a ~20-40
        fn subset; full functionality incl. sub-menus/input is the 96).
    ===== PROGRESS (main-menu arc) =====
    Phase A (94e24bc): 208 menu.bin data symbols migrated as contiguous C
    .data (blob-per-section + .set aliases). Phase B1a (07fa23c): dispatcher
    func_801C62A8 + init slice func_801C5F10 + 9 alloc toggles ported
    (native-struct convention -- struct FIELDS, not raw PSX offsets, since the
    port's SystemMenu uses 8-byte pointers). B1b step 1 (2026-07-19): B1a
    runtime milestone CONFIRMED via code-side force XENO_KERNEL_SEL=4 (the
    KernelMenu "Menu" option) -> MenuExecute case 0 -> func_801C62A8 ->
    func_801C5F10 (init) -> func_801D2D38/55A0 render entry, no crash 240
    frames, backtrace-proven. (gdb symbol/struct reads of g_Menu/D_80059460
    are unreliable here -- a native-layout view artifact; trust control flow.
    This is WHY the earlier gdb symbol-WRITE force failed.) B1b step 2a
    (dd56cab): content-build coordinator func_801C7B0C + zero-dep helpers
    6D4C (renderContext=0) / 6D5C (zero unk4CC scratch) ported; coordinator
    sets pSelectionMenu->unk1180 (RECT 320x224 @ 704,256) + unk348->unk15B and
    drives the builder sequence, reaches the render entry, no crash; matching
    compiles; tripwire inert (0 dispatch hits). REMAINING (B1b step 2b): the
    POLY-setup builders func_801C6E68 (4x func_80026338 border-texture unpack),
    func_801C6E0C (needs func_801C6D90 + overlay func_801E7E68), then the big
    ones 6400/6AA0/6F70/65F4 (134-321 instrs, read Phase-A data) -- these build
    the actual window/text POLYs. Then B2 (render subtree func_801C8694, 86
    fns) wires AddPrim so content reaches the screen.
    B1b step 2b BLOCKER (2026-07-19, investigation -- no code landed): the
    "cheap frame-first" plan is invalidated by a resource-load dependency,
    and the current harness cannot verify textured content. Chain:
      func_801C6E68 (frame, a verbatim twin of MemberChangeMenuInitialize-
      WindowBorders) reads g_Menu->unk2DC (the texture-UV atlas) ->
      unk2DC is populated ONLY by func_801C65F4 (the 321-instr resource-load,
      the twin of MemberChangeMenuLoadResources -- the HEAVIEST builder, not a
      cheap one) -> func_801C65F4 reads pResources = D_8005945C ->
      D_8005945C is set ONLY by (a) the field menu-opener func_800799D4
      (misc4.c:528, D_8005945C = menuLoadCommands[0].pData) or (b) the debug
      path (menu.c:213, gated on g_MenuDebugEnabled).
    PROBED at the coordinator in the XENO_KERNEL_SEL=4 run: g_MenuDebugEnabled
    = 0 AND D_8005945C = NULL. So the KERNEL_SEL=4 force drives dispatch/init/
    coordinator CONTROL FLOW only -- it never loads the menu resources. With
    unk2DC NULL, porting func_801C6E68 would turn a safe no-op stub into a
    NULL deref in func_80026338 (a regression), and there is no resourced
    harness to see a frame against. func_801C65F4 itself is BOUNDED (all
    callees available: LZSSHeapDecompress/OpenTIM/ReadTIM/func_8002DD20/
    LoadImage/ResolveArchiveEntryPointers/...) and closely mirrors the ported
    MemberChangeMenuLoadResources -- but it needs D_8005945C != NULL to run
    (else ResolveArchiveEntryPointers(NULL) crashes).
    => The REAL next unit is a RESOURCED headless harness (code-side force of
    the field menu path func_800799D4, or g_MenuDebugEnabled=1 with the port's
    overlay/resource load verified), NOT more builder porting. member_change's
    border was validated via the map005 field/normal-menu repro (interactive
    xdotool key injection) -- that IS the resourced main-menu path (FE55
    opcode func_80093740 -> func_800799D4 -> D_8005945C), but it is not
    headless. Once a resourced harness exists: port func_801C65F4 (resource-
    load, mirror MemberChangeMenuLoadResources) + func_801C6AA0 (its caller) +
    func_801C6E68 (frame), and the window frame becomes verifiable on-screen.
    RESOURCED HARNESS BUILT (41d4c9f, 2026-07-19): PcPort_ForcedFieldMenu
    (psyq_compat.c Vsync hook, XENO_MENU_FORCE=1) forces the FIELD menu-opener
    path -- once the field is up it sets D_800ADB64=0x80 (main menu), the field
    loop calls func_800799D4, which streams the menu resources and runs
    MenuMain. Verified on map005: func_800799D4 fires, func_801C62A8 (main
    menu) dispatches, D_8005945C=0x68e930 (NON-NULL, resources loaded), no
    crash 260 frames. Inert unless the env is set (tripwire clean). Harness:
      XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 XENO_FIELD_MAP=5 XENO_MENU_FORCE=1
    This UNBLOCKS the resource-load port. Next unit (~500 instrs): func_801C6AA0
    (184, a thin wrapper: calls func_801C865C + func_801C65F4) + func_801C65F4
    (321, the resource-load -- core mirrors MemberChangeMenuLoadResources:
    ResolveArchiveEntryPointers -> icon TIM (pResources[1]) -> func_8002DD20
    (pResources[2]) -> unk2DC = LZSSHeapDecompress(pResources[3]) -> unk2E0
    (pResources[4]); ADDITIONS: two BASLUS save-name strings at MenuUnk2 0x4FCE/
    0x501C, 4x stack-local func_80026338, a 3-iter character-portrait loop over
    pResources[5] w/ 0xB20 stride + LoadImage). Verifiable milestone: unk2DC
    populated (non-NULL) in the harness.
    IMPORTANT (frame visibility): porting the resource-load + func_801C6E68
    does NOT put the window frame on-screen -- func_801C6E68 only unpacks border
    texture COORDS (texPage/clut/UV) into g_Menu fields; the border POLY_FT4s
    are built (func_8002675C) and DRAWN by the render path (func_801C55A0 / the
    B2 subtree func_801C8694), still stubbed. So the pipeline to a VISIBLE frame
    is: resource-load (unk2DC) -> builders (border coords) -> B2 render (draw).
    B1b gets the first two; B2 draws. Don't expect pixels until B2.
    RESOURCE-LOAD PORTED + VERIFIED (d748fee, 2026-07-19): func_801C865C
    (party bit-select) + func_801C6AA0 (party/character setup: availableCharacters
    from the party flag mask, the 3 currentCharacterIDs slots + gear flags, first
    active slot) + func_801C65F4 (the resource-load -- icon TIM, menu textures,
    unk2DC=pResources[3] atlas, unk2E0, 3 party-portrait TIMs to VRAM, 2 save-file
    names). Native-struct convention; explicit void* proto for LZSSHeapDecompress
    (port -w would truncate the 64-bit ptr). VERIFIED in the resourced harness:
    func_801C6AA0->65F4 run to completion no crash; unk2DC = 0x5a9818 (NON-NULL --
    atlas loaded), read via func_80026338's arg (indices 0xE0/0x14B/0x14C/0x14D as
    transcribed). slus untouched; matching clean; tripwire 0-hit inert.
    FRAME BUILDERS PORTED (d86f409, 2026-07-19): func_801C6E68 (window-border
    coords -- 4x func_80026338 unpacking top/bottom/left/right from unk2DC into
    g_Menu's border texPage/clut fields; verbatim twin of member_change) +
    func_801C6E0C (palette upload + string work-buffer + content stub). Both
    verified in the harness (border calls fire 0xFE/0x103/0x100/0x101 as
    transcribed, texPage fields written at correct native offsets, no NULL-crash).
    CONFIRMED: SystemMenu is INFLATED in the port (sizeof 0x2098 vs PSX 0x1E98;
    unk4E0 at C-offset 0x610, texPage0 at 0x59C) -- field-name access is
    mandatory (member_change's raw offsets work only by self-consistency at a
    wrong-but-valid location). The window frame's DATA path (coords + palette) is
    complete.
    REMAINING BUILDERS (substantial -- the "thin wrapper" lesson): func_801C6400
    (134 instrs -- memory-card save-file archive loader, reads saves via the
    BASLUS filenames + MenuUnk2 0x501A/0x501B) and func_801C6F70 (214 instrs --
    GPU POLY builder: SetPolyF4/SetLineF3/SetDrawMode; deps func_801C8164 +
    func_801D22C4 already ported). Both build OTHER content (save UI, menu-item
    POLYs), not the window frame -- deferred as their own units.
    B2 RENDER-CORE WALL (2026-07-19, investigation -- no code landed): the
    "pixels gate" is a MONOLITH, not a small subtree. The main-menu render entry
    (func_801C62A8 case 0 -> func_801D2D38 + func_801C55A0, run once via
    MenuMain -> MenuExecute) has a transitive closure of 272 menu funcs, 269 of
    them UNPORTED. The ~86 estimate was for func_801C8694 (MENU 2's render), NOT
    the main menu. There is NO clean frame-only entry: the render setup
    (func_801D2D38 allocs + inits via 0x801E functions), the OT/GfxEnv/context,
    and the object-overlay sprite draws (func_801C55A0 calls func_801E8044/8070/
    8978 -- the 0x801E object-overlay region, same as the MAP3/MAP16 convergence)
    are all shared/entangled with the frame draw. member_change renders because
    ITS render subtree is small + mostly ported (simple AddPrim loops, e.g.
    func_801C7DA8/E38/EC8); the main menu's render is genuinely a larger, unported
    monolith. So the first main-menu pixel is NOT a single pass -- it needs either
    a multi-pass render-core port (269 fns) or a careful minimal-frame extraction
    (smallest OT-setup + border-build func_8002675C-caller + border-AddPrim +
    DrawOTag func_801C7BF4, stubbing the string/cursor/item/sprite branches -- a
    dedicated analysis, risky because the render setup is shared). The frame's
    DATA path (atlas -> coords -> palette) is complete + verified and READY; the
    DRAW path is the monolith. NB the render entangles the 0x801E object-overlay
    region -- so the menu render and the MAP3/MAP16 object-overlay convergence
    share this subtree.
    MINIMAL-FRAME EXTRACTION MAPPED (2026-07-19, analysis -- no code): the
    smallest window-frame draw path was traced. Draw chain (the loop draws
    each iteration via func_801C7BF4):
      func_801C55A0 (142, the input LOOP; input branches -> stubs, harness has
        no input) -> func_801C7BF4 (102, per-frame draw: ClearOTagR + 3D setup
        + sub-draws + DrawOTag + Vsync/PutDrawEnv/PutDispEnv present)
        -> func_801D1CA0 (47) -> func_801D1B20 (52) -> func_801D0C78 (76)
        -> func_801D09F0 (167, the window AddPrim) -> func_801D0954 (leaf prim).
    KEY CONSTRAINT: the window is drawn via 3D PROJECTION -- func_801D09F0/0954
    call RotTransPers4 to project the window vertices, so the 3D matrix setup
    (func_801C7F34 99i + func_801D1D40 88i, RotMatrix/TransMatrix/SetRotMatrix)
    CANNOT be stubbed -- the window won't position without a valid MATRIX. Plus
    the window-BUILD (one of 12 func_8002675C callers reachable from the
    func_801D2D38 106i setup) + func_801D2D38 itself. So the minimal frame is
    ~10-12 funcs (~800-1000 instrs) INCLUDING the 3D projection pipeline
    (matrices + RotTransPers4 + OT mgmt) -- a real focused render port, not a
    2D border blit. Feasible + bounded + mapped; the RotTransPers4 dependency is
    what makes it non-trivial (the window frame isn't 2D screen-space).
    REMAINING to start the port: identify which of the 12 func_8002675C setup
    callers builds the MAIN window frame (the one at the frame rect 320x224).
    RENDER-CONVERGENCE SCOPING (2026-07-19, read-only -- the KEY verdict):
    the claim was "MAP3 render gap + menu B2 render + MAP16 share a root -> one
    arc, three payoffs." VERDICT: FALSE -- it is TWO INDEPENDENT ARCS, not one.
    (C) The menu B2 chain (func_801C55A0/C7BF4/D1CA0/D1B20/D0C78/D09F0/D0954)
    lives in the menu.bin OVERLAY (0x801C5000+), which is NOT loaded during a
    field map's render (menu closed) -- so MAP3's field render CANNOT use it
    (different overlay). MAP3's field render is func_800xxxxx (func_800748E8 /
    FieldAddPrimitives in misc2.c) -- ALREADY PORTED C, a distinct path. They
    share ONLY the base GTE/GPU core, and (E) that core -- RotTransPers4,
    AddPrim, DrawOTag, SetRotMatrix -- is ALREADY PORTED (PsyCross/PsyX), so it
    is not even a shared bottleneck to fix. Porting the menu chain does NOT flip
    MAP3, and vice versa. => the "highest-leverage, three payoffs" framing was
    OPTIMISTIC; the two arcs are independent.
    (A) ARC A = MENU B2 (the mapped ~10-12-fn / ~800-1000-instr chain above):
    SINGLE payoff = the menu window. Bounded, mapped, ready. The stubbed menu-
    overlay draw functions route through the already-ported RotTransPers4/AddPrim.
    (B/D) ARC B = the FIELD render-completeness gap (MAP3 16.7%, MAP16 2% vs ~74%
    playing): a SEPARATE, un-pinned gap. NB the port's per-frame field render
    does NOT go through game-level DrawOTag/AddPrim/func_8002C700/FieldAdd-
    Primitives (all ~0/frame on MAP3 AND playing MAP7 -- PsyX intercepts the
    submission). So MAP3/MAP16's low render is in HOW their object/model scenes'
    primitives get built/submitted through the ported field render + PsyX layer
    -- needs its OWN diagnostic pass (this scoping did not pin the exact
    mechanism; it is confirmed NOT the menu chain). MAP16 is likely the same
    field-render arc as MAP3 (possibly + a spawn/population question on top).
    (F) PHASED PLAN (two arcs, independent):
      Arc A (menu window): Phase A1 = pick the window-frame func_8002675C caller
      + port func_801D2D38 setup slice; Phase A2 = the draw chain func_801C55A0
      -> func_801D09F0 + the 3D matrix setup (func_801C7F34/1D40); Phase A3 =
      wire func_801C7BF4 present. Payoff: the first visible main-menu window.
      ~1000 instrs, mapped, SINGLE payoff.
      Arc B (MAP3/MAP16 flip): Phase B0 = a DIAGNOSTIC pass to pin the field-
      render-completeness gap (what primitive build/submit MAP3's scene needs
      that a playing map has) -- NOT yet scoped into a port. Payoff: MAP3 (+maybe
      MAP16) flip. Depends on B0's finding.
    RECOMMENDATION: the arcs are INDEPENDENT -- the menu B2 (Arc A) is the
    DEFINED, bounded, ready slice (single payoff: the menu window). MAP3's flip
    (Arc B) is a separate field-render diagnosis, not a byproduct of the menu
    port. Pick Arc A for a mapped port with a visible payoff, or Arc B0 to
    diagnose MAP3's field gap first -- but they are two efforts, not one.
    ===== ARC A EXECUTION (chosen; menu window render) =====
    CONFIRMED chain (2026-07-19): de-risked by member_change func_801C7EC8
    (RotTransPers4 project + AddPrim, renders on-screen). func_801D1B20 is a
    LINEAR sequence of 22 void sub-renderers -- the window is ONE branch
    (func_801D0C78); the other 21 stay no-op stubs, so only the window draws.
    Port order (bottom-up, ~10 fns):
      1. func_801D0954 (41i) -- DONE + compiles (project 1 window quad + AddPrim
         to g_Menu->pGfxEnv->ot[otIdx]).
      2. func_801D09F0 (167i) -- loops func_801D0954 x11.
      3. func_801D0C78 (76i) -- Push/Rot/TransMatrix + SetRot/Trans (window 3D
         matrix) then func_801D09F0 x2; reads windows[] (0x364).
      4. func_801D1B20 (52i) -- 22 void calls (21 stubs + func_801D0C78).
      5. func_801D1CA0 (47i) -- func_801D1B20 + 3 stubbable.
      6. func_801C7BF4 (102i) -- ClearOTagR + func_801C7F34/1D40 + func_801D1CA0
         + DrawOTag + Vsync/PutDrawEnv/PutDispEnv present.
      7. func_801C55A0 (142i) -- the input loop (drives func_801C7BF4/frame).
      8. func_801C7F34 (99i)+func_801D1D40 (88i) -- global view matrix (verify if
         needed vs func_801D0C78's local matrix).
      9. func_801D2D38 (106i) setup + the window-BUILD (a func_8002675C caller)
         -- builds the border POLY_FT4s the chain AddPrims, else nothing to draw.
    Shared GTE/GPU core (RotTransPers4/AddPrim/DrawOTag/matrices) all PORTED
    (PsyX). glReadPixels VERIFY only once the whole chain lands. Harness:
    XENO_FIELD_MAP=5 XENO_MENU_FORCE=1. Multi-turn port; leaf done + compiles.
    ===== func_801D2D38 FAN-OUT SCOPED (2026-07-19, read-only; the pixels gate)
    The draw pipeline is PROVEN running (891a448: dispatch->loop->draw->window
    matrix every frame, no crash); func_801D0C78 skips the AddPrim because (1)
    windows[] is empty and (2) the draw guard pManager->shouldRenderWindow[i]
    (MenuManager+0x20) is never set. func_801D2D38 decoded in full:
      for i in 0..1 (main menu only): windows[i]=HeapAlloc(0x720)+bzero;
        windowParameters[i]=HeapAlloc(0x18)+bzero; func_801E53CC(i)  <- FRAME
      func_801C8574(0x5E)                                            <- sound
      per party slot (x3): func_801E8DA8(charId, slot*2) + gear      <- portraits
      func_801E8474(8, D_801EA19C)                                   <- content
      func_801D29A8(1, 0)                     <- OPEN-ANIMATION + THE FLAG SET
      func_801D28FC()                                                <- post
    ROLES/SIZES: func_801E53CC (203i) = THE window-frame builder -- SetPolyFT4
    x4 borders + SetPolyG4 background + GetTPage/GetClut/SetSemiTrans/
    SetShadeTex; SELF-CONTAINED (only PsyX-ported GPU setters, zero other
    calls); a WHOLE menu-coherent function (real arg-taking entry -- distinct
    from the field's no-op'd 801E mid-entries). Its output is exactly what the
    ported func_801D09F0 draws (border corners/top/bottom/left/right pairs +
    background G4) -- the build and draw halves match.
    func_801E8DA8 (68i) = portrait build (name-string render + LoadImage) --
    CONTENT, separable. func_801E8474 (160i) = selection-menu content build --
    CONTENT, separable. func_801D29A8 (243i) = the open ANIMATION; its tail
    sets shouldRenderWindow[1]=1 / [0]=0 (sb 0x21/0x20) -- NOT separable (it is
    the flag setter). Its fan-out: func_801C81E0 (89, leaf) + func_801C8324
    (144, leaf) + func_801D5A50 (90 -> 6 geometry fns totaling 747:
    func_801D4F2C/50EC/51EC/53D0/55B4/5794) + func_801E8018 (13) +
    func_801E8044 (15) + func_801D28A8 (23 -> func_801D397C 102). func_801D28FC
    (29 -> func_801D397C + func_801D5CF8 124) = post-setup.
    THE MINIMAL-FRAME SLICE (recommended): func_801D2D38 (106, content/portrait
    calls stay stubs) + func_801E53CC (203, self-contained) + func_801D29A8
    (243, its animation callees func_801C81E0/C8324/D5A50 initially stubbed --
    the anim loops degrade to instant-complete, the tail still sets the flag)
    ~= 550 instrs / 3 functions -> windows[] built + frame geometry + flag set
    -> the PROVEN chain draws it -> glReadPixels. RISK: if func_801E53CC builds
    start-of-animation geometry and the func_801D5A50 family computes the FINAL
    geometry, the frame may draw degenerate -- fallback: port func_801D5A50 +
    its 6 geometry fns (+837). FULL build (frame+content+portraits+animation+
    post) ~= 2260 instrs / ~19 fns. All fan-out fns currently INCLUDE_ASM
    stubs; none mid-entry.
    ===== PIXELS SLICE LANDED (5d866b8) -- window draw FIRES; verts are the
    LAST gap. Ported: func_801E53CC (frame-primitive init) + func_801C81E0/
    func_801C8324 (anim slot init/stepper -- REAL, not stubbed: the wait loop
    exits via the stepper's done flags; two hazards found + fixed: (1) stubbed
    steppers = infinite loop; (2) the cold harness has an EMPTY party (mask 0)
    so nothing steps the anim -- retail-unreachable state; fixed with an
    env-gated party seed (Fei) in PcPort_ForcedFieldMenu) + func_801D29A8
    (open/close animation, tail arms shouldRenderWindow[1]) + func_801D2D38
    (setup/allocs). RUNTIME: the full chain runs -- open animation (~40 real
    interpolated frames) -> flag armed -> func_801D09F0 fires EVERY frame (116
    draws/1392 quad AddPrims probed) -> DrawOTag, no crash. PIXELS: black
    (0.0%) -- the degenerate risk landed: the window VERTS are never written
    (windows[] bzero'd -> zero-size quads). THE LAST SLICE (fully mapped, no
    unknowns): func_801D28FC(29) -> func_801D397C(102, writes windowParameters
    x/y/w/h; window 1 = 0xCC/0xC6/0x50/0xD0-ish args) -> func_801D4D1C(100,
    dispatcher) -> 7 per-piece verts writers (func_801D3C4C 91, func_801D3DB0
    149, func_801D3FF8 212, func_801D433C 214, func_801D4688 213, func_801D49D0
    214, func_801C851C 24) + func_801D5CF8(124, border UVs via 5x func_8002675C)
    = 1472 instrs / 11 fns. Port that family -> the verts/UVs become real ->
    the ALREADY-FIRING draw rasterizes the frame -> pixels.
    ===== ARC A PAYOFF (b340077, 2026-07-19): THE WINDOW FRAME RENDERS =====
    The verts family is PORTED (all 11, coexistence). glReadPixels flipped
    0.0% -> 4.2%: a REAL Xenogears menu window at the window-1 rect (screen
    ~[392..583]x[407..475]) -- grey semi-trans 0x68 G4 background, blue border
    lines, rounded atlas corner pieces, gold outer glow. Capture archived:
    captures/render_diag/menu_window_frame_20260719.png. This is the first
    visible main-menu content -- the payoff of the whole arc (data migration ->
    dispatch -> coordinator -> resource-load -> builders -> draw pipeline ->
    animation -> geometry). Recipe: XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0
    XENO_FIELD_MAP=5 XENO_MENU_FORCE=1 (the hook seeds a 1-member party on the
    cold boot -- retail always has one when a menu opens).
    FOLLOW-ONS (the menu arc continues, all now verifiable on-screen):
    (a) the MAIN window (window 0? -- window 1 is the small bottom bar; the
        big selection window presumably comes from func_801E8474's content
        path or another func_801D397C caller -- trace which builds window 0);
    (b) CONTENT: labels/cursors (func_801E8474 + the D_801EA19C string
        descriptors), portraits (func_801E8DA8), the icon strip's DRAW (its
        sub-renderer in the func_801D1B20 22-list is stubbed);
    (c) the portrait-frame geometry (func_801D5A50 family, +837i) for the
        open animation's sliding frames.
    CONTENT PASS 1 (f9cfd41, 2026-07-20): func_801E8DA8 PORTED -- it is the
    party NAME-PLATE renderer (SystemRenderStringEntry x2 -> 0x28x0xD VRAM
    upload at D_801EA578/D_801EA5C4 slot coords), NOT the portrait-quad
    builder (the scoping label was wrong). Runtime-verified: 6 calls fire
    (3 slots x char+gear), uploads complete; glReadPixels unchanged 4.2% as
    expected (texture-side write; blank names on cold state).
    THE PORTRAIT-QUAD SLICE (fully traced, the next content unit, ~913i):
      BUILD: func_801D5A50 (90i) -> func_801D4F2C 116 / func_801D50EC 68 /
      func_801D51EC 127 / func_801D53D0 127 / func_801D55B4 126 /
      func_801D5794 183 -- write the portrait-frame geometry into the B1a
      unk39C buffers (3 x 0x127C), positioned by the open-anim slots.
      DRAW: func_801CE540 (76i, the portrait sub-renderer in func_801D1B20's
      22-list, currently stubbed) -- reads unk39C + RotTransPers + AddPrim.
      TEXTURES: complete (faces = resource-load; plates = f9cfd41).
      Port build+draw -> Fei's portrait frame renders (the anim slides it in).
    PORTRAITS RENDER (c77df61, 2026-07-20): the 12-fn slice PORTED. glReadPixels
    4.2% -> 38.9%: FEI'S ACTUAL PORTRAIT ART on-screen (x3 slots, cold party)
    with live LV/HP/EP/Next-LV stat panels + digit rendering (zeros, faithful).
    Capture: captures/render_diag/menu_portraits_20260720.png. Transcription
    catches: charId as $a1 register residue (func_801D5A50); maxHP/maxMP digit
    loops position by BUILD count (delay-slot index bump). KNOWN ARTIFACT:
    slide-in trails -- PsyX DRAWENV isbg background-clear UNIMPLEMENTED
    (LIBGPU.C TODO at ~436/458); the menu relies on isbg to clear each frame.
    Fix = implement isbg clear in PsyX (the build_port.sh _xeno patch
    mechanism) -- the next polish item. Remaining content: labels/cursors
    (func_801E8474), the big selection window (window 0 trace), icon-strip
    draw sub-renderer.
    OPTIONS RENDER (477d789, 2026-07-20): func_801E8474 CONFIRMED (asm-first)
    as the option-label reveal (staged: polysCursors highlight set +
    polysTexts normal set from D_801EA19C pairs into pSelectionMenu, 2 drawn
    frames/stage) + its draw func_801CEC40 (the selection sub-renderer:
    shouldRenderSelectionMenu gate, dim/undim restyle on unk1192!=unk1193,
    batch-AddPrims both sets to ot[4]). glReadPixels 38.9% -> 40.6%: the REAL
    option list renders -- Status/Equip/Items/Abilities/Gear/File/Exit in the
    diagonal cascade with sphere bullets. Capture:
    captures/render_diag/menu_options_20260720.png. PSX idiom #3: g_Menu via
    $a0 register residue at func_801CEC40 entry.
    THE MENU NOW SHOWS: window frame + name-plates(VRAM) + portraits + stat
    panels + digits + THE OPTION LIST. Remaining: the isbg clear (PsyX polish,
    kills the trails), the big selection window (window 0), the icon-strip
    draw, nav/input (cursor movement -- needs input, the member_change
    harness-nav question).
    ISBG IMPLEMENTED + TRAILS RE-DIAGNOSED (ac3b676, 2026-07-20): the PsyX
    DRAWENV isbg TODO is closed (_xeno_isbg build_port.sh patch: PutDrawEnv
    fills the clip via ClearImage on env-apply -- NOT DrawPrim, which would
    over-clear immediate-mode callers). FIELD verified unregressed (the field
    sets isbg=1 unconditionally -- MAP7 renders byte-flat 74.3% == baseline,
    5 tripwires boot). BUT the menu trails PERSIST -- root cause corrected:
    the retail menu sets isbg ONLY in the debug path (menu.c 285-288,
    g_MenuDebugEnabled); normal-path menu envs have isbg=0. The menu's real
    per-frame clear is the MoveImage BACKDROP RESTORE in func_801C7BF4
    (VRAM (704,256) 320x224 -> the draw fb half, then DrawOTag on top).
    PsyX does NOT composite VRAM-blit content into the GL backbuffer as a
    base layer (see the _xeno_read_materialize note in LIBGPU.C DrawSync), so
    the menu's GL quads accumulate across frames = the trails. THE TRAILS FIX
    (own unit): materialize MoveImage/GR_CopyVRAM writes whose DEST overlaps
    the active draw framebuffer into the GL backbuffer before the frame's
    prims (the _xeno_drmove patch family territory) -- OR equivalently blit
    the VRAM fb rect as the frame's base layer when it has been dirtied.
    Survey-signature note: the "isbg gap" label for the trails was itself a
    mislabel -- the isbg TODO was real but was NOT the menu's clear mechanism.
    TRAILS FIXED (6cc102d, 2026-07-20): the MoveImage->GL materialize landed
    (_xeno_fb_materialize patch pair: GR_MaterializeFramebufferRect in
    PsyX_render.cpp -- GR_UpdateVRAM + glBlitFramebuffer of the restored vram
    rect over the backbuffer as the frame's base layer; MoveImage in LIBGPU.C
    triggers it only when the dest overlaps the active draw env clip). The
    menu renders CRISP: 40.6% -> 15.1% (the trails were ~25% of the screen);
    three portraits with legible LV/HP/EP/Next-LV panels, sharp option list,
    clean window. Capture: captures/render_diag/menu_clean_20260720.png.
    FIELD verified: MAP7 75.4% vs 74.3% baseline, eyeballed PRISTINE (the
    +1.1% = a field MoveImage now correctly composited); 5 tripwires boot.
    Known harness-only residue: stale KernelMenu debug text in the backdrop
    VRAM (XENO_FIELD_TEST boot path) now faithfully composites -- absent on a
    normal boot. THE MENU IS VISUALLY COMPLETE for its current content: frame
    + portraits + stats + options, clean. Remaining: the big selection window
    (window 0), the icon-strip draw, nav/input (cursor movement).
    GOLD + TIME WINDOWS RENDER (a9653f3, 2026-07-20): window 0 traced asm-
    first -- NOT "the big window" (survey-signature again): it is the GOLD
    window (96x16 at 0xD4,0xB2), built by func_801D28A8 (the open-settle) via
    the proven func_801D397C path + func_801D5BA4 (gold digits from
    g_GameState.gold + the "G" glyph into the B1a unk340[0] buffer). Their
    drawer func_801CE464 was a two-for-one: its unk5[1] branch draws the
    unk340[4] buffer, revealing func_801D5CF8's true role -- the PLAY-TIME
    readout (the "icons" are time digits, the 0xEE "separators" are colons):
    window 1 = the TIME window, now showing 000:00:00. glReadPixels 15.1% ->
    18.5%; capture menu_gold_time_20260720.png. BOTH allocated windows render;
    there is no unrendered "big window" (the sub-menu screens 801DB/DD/DE/E0/
    E1xxx create windows 2+ on demand via func_801D397C when an option is
    selected -- nav-gated, future arc). ARC A RENDER CONTENT IS COMPLETE for
    the cold-state main menu: portraits + stats + options + gold + time, all
    clean. Remaining: nav/input (cursor movement + option select -> the
    sub-menu screens), byte-match refinement of the coexistence fns.

    ================================================================
    ===== ARC A RENDER CHAPTER: COMPLETE (capstone, 2026-07-20) =====
    ================================================================
    The cold-state Xenogears main menu renders fully and cleanly in the port:
    Fei's portraits + LV/HP/EP/Next-LV stat panels + the option cascade
    (Status/Equip/Items/Abilities/Gear/File/Exit) + the gold window ("0 G") +
    the time window ("000:00:00") -- no trails, 18.5% glReadPixels. Captures:
    menu_clean_20260720.png, menu_gold_time_20260720.png (render ground truth,
    real GL-backbuffer glReadPixels).

    (1) THE COMPLETE RENDER CHAIN (all in src/menu/main/misc.c unless noted):
      dispatch func_801C62A8 -> init func_801C5F10 (B1a allocs: pManager,
      pSelectionMenu, unk32C/330/348/354, unk340[0]=0x328 gold buf,
      unk340[4]=0x374 time buf, unk39C[0..2]=3x0x127C portrait bufs)
      -> coordinator func_801C7B0C (selection rect + builder sequence)
      -> resource-load func_801C6AA0 (party setup) + func_801C65F4 (icon TIM,
         menu textures, unk2DC ATLAS, unk2E0, portrait TIMs -> VRAM)
      -> frame builders func_801C6E68 (border tpage/clut from atlas) +
         func_801C6E0C (palette)
      -> the draw pipeline: func_801C55A0 (input loop) -> func_801C7BF4
         (per-frame draw: gfx-env flip, ClearOTagR, render, DrawOTag,
         present, MoveImage backdrop restore) -> func_801D1CA0 ->
         func_801D1B20 (the 22 sub-renderers) -> func_801D0C78 (window
         matrix, shouldRenderWindow gate) -> func_801D09F0 (window AddPrim
         loop) -> func_801D0954 (RotTransPers4+AddPrim leaf)
      -> the window build: func_801D2D38 (windows[0..1] allocs + portraits +
         content + animation) -> func_801E53CC (frame primitives) ->
         func_801D29A8 (open animation; func_801C81E0/func_801C8324 anim
         slots; tail arms window 1) -> func_801D28FC (window 1 = TIME window
         geometry via func_801D397C -> func_801D4D1C -> the piece writers
         func_801D3DB0/3FF8/433C/4688/49D0 + func_801C851C verts leaf) ->
         func_801D28A8 (open-settle: window 0 = GOLD window + func_801D5BA4
         gold digits) -> func_801D5CF8 (time digits)
      -> content: func_801E8DA8 (name-plates -> VRAM) + func_801D5A50 family
         (portrait panels: func_801D4F2C face/frame, func_801D50EC icons,
         func_801D51EC HP, func_801D53D0 MP, func_801D55B4 EXP,
         func_801D5794 level; func_801C80B8 digit parser) + func_801E8474
         (option-label reveal)
      -> content draws (in the 22-list): func_801CE540 (portraits),
         func_801CEC40 (options/selection), func_801CE464 (gold + time).

    (2) PsyX-LAYER FIXES (2 upstream gaps closed, tracked in build_port.sh):
      (a) _xeno_isbg: DRAWENV.isbg background-clear implemented in PutDrawEnv
          (real libgpu semantics; NOT DrawPrim -- immediate-mode would over-
          clear). The FIELD sets isbg=1 unconditionally; verified byte-flat
          (MAP7 74.3% == baseline at the time).
      (b) _xeno_fb_materialize: MoveImage dests overlapping the active draw
          env clip blit the restored vram rect over the GL backbuffer as the
          frame's base layer (GR_MaterializeFramebufferRect). THE trails fix
          (the menu's per-frame clear is its MoveImage backdrop restore).
          Also closed a latent field compositing gap (MAP7 74.3->75.4%,
          eyeballed pristine = added fidelity).

    (3) CORRECTED ROLE LABELS (survey-signature-is-hypothesis fired
        REPEATEDLY -- these are the TRUE roles, do not re-mislabel):
      - func_801E8DA8 = party NAME-PLATE renderer (NOT "portrait builder")
      - func_801D5CF8 = play-TIME readout (NOT "icon strip")
      - window 0 = the GOLD window; window 1 = the TIME window (there IS no
        "big window" -- submenu windows 2+ are created on demand)
      - the isbg TODO was real but NOT the trails cause (a root cause is
        only proven when the fix removes the symptom)
      - func_801E8474 = option-label reveal (the one label that was right)

    (4) BYTE-MATCH STATE / REFINEMENT CANDIDATES (menu/main/misc.c):
      BYTE-MATCHED (asm retired): func_801D0954, func_801D09F0,
        func_801D0C78. (func_801D1B20/func_801D1CA0 have C bodies but their
        nonmatching asm remains -- functional, not yet matched.)
      COEXISTENCE (matching keeps retail asm; port C is behavioral --
        the byte-match refinement pool, ~40 fns): the render chain +
        content fns listed in (1) carrying #ifndef XENO_PC_PORT blocks
        (func_801C55A0/5F10/62A8/65F4/6AA0/6D5C/7B0C/7BF4/80B8/81E0/851C,
        func_801CE2B4/E464/E540/EC40, func_801D28A8/28FC/29A8/2D38/397C/
        3DB0/3FF8/433C/4688/49D0/4D1C/4F2C/50EC/51EC/53D0/55B4/5794/5A50/
        5BA4/5CF8, func_801E53CC/8474/8DA8/91C4/920C/927C).
      PLAIN-C in both builds (functional, objdiff-counted): the B1a/B1b
        alloc toggles + frame builders (func_801C5B54..5E74, 6D4C, 6E0C,
        6E68, 8324, 865C).

    (5) THE NEXT ARC -- NAV/SUBMENUS (scoped, NOT started): cursor movement +
      option select -> the submenu screens (the 801DB/DD/DE/E0/E1xxx
      families) which create windows 2+ on demand via func_801D397C.
      NAV-GATED: needs input reaching the menu loop (func_801C55A0 reads
      g_Menu->input). CAVEAT: the cold harness cannot drive nav (the same
      ceiling member_change's labels hit -- harness-context-artifact).
      The nav arc needs a SCOPING pass first: how does g_Menu->input get
      fed (ControllerPoll -> ?), can the harness inject edges (the
      dialog-confirm precedent: edge-gated, memory
      dialog-dismiss-edge-gated-confirm), and what do the confirm branches
      pull in (func_801C531C, the 0x801E nav callees, the submenu families).

    (6) THE HARNESS (the working recipe): XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0
      XENO_FIELD_MAP=5 XENO_FIELD_ENTRANCE=0 XENO_MENU_FORCE=1
      [XENO_MENU_FORCE_DELAY=60..240]. The hook (psyq_compat.c
      PcPort_ForcedFieldMenu) presses the menu button once the field idles
      and seeds a 1-member party (Fei) iff the party mask is empty.
      KNOWN RESIDUE (harness-only): stale KernelMenu debug text composites
      from the backdrop VRAM (the XENO_FIELD_TEST boot drew it); absent on
      a normal boot.

    (7) LESSONS (banked to memory where durable):
      - survey-signature-is-hypothesis: role labels are leads; open the asm.
      - A ROOT CAUSE IS ALSO A HYPOTHESIS until the fix removes the symptom
        (the isbg episode).
      - PSX idioms to watch: register-residue args ($a0/$a1 at entry --
        func_801D5A50's charId, func_801CEC40's g_Menu), delay-slot loop
        counters (the maxHP/maxMP build-count positioning).
      - Native-struct FIELD NAMES throughout (SystemMenu inflated 0x2098 vs
        0x1E98); pure-data structs (GameState, MenuWindow, MenuSelectionMenu)
        are safe at PSX offsets.
      - The B1a buffers (unk340[0]/[4], unk39C[0..2]) are load-bearing
        across the whole content chain -- allocations from the FIRST menu
        pass feed the LAST content renderers.

    ===== NAV/INPUT ARC -- SCOPED (2026-07-20, read-only) =====
    (A) INPUT PATH: g_Menu->input (0x325) is written by func_801C7D78 (the
      main-menu per-frame reader, called inside func_801C7BF4) and the general
      MenuProcessControllerInput -- both read g_C1ButtonStatePressedOnce /
      g_C1ButtonStateReleased (the edge state) and map buttons -> the
      MENU_INPUT_* code (RIGHT0/DOWN1/LEFT2/UP3/CONFIRM4/IDLE8). func_801C55A0
      then reads 0x325 and acts.
    (B) HARNESS VERDICT: **YES -- the cold harness CAN drive menu nav.** The
      real input pipeline is ALREADY wired in the Vsync hook (psyq_compat.c):
      PsyX_UpdateInput (SDL keyboard/gamepad) -> ControllerPoll (recompute
      g_C1ButtonState* edges) -> ControllerPushState (the queue func_801C7D78
      drains via ControllerPopState). So keyboard/pad -> the exact edge state
      the menu reads. The ONLY gap: func_801C7D78 (the menu input reader) is a
      STUBbed INCLUDE_ASM (no-op'd in the render port because "no input in the
      render harness") -> g_Menu->input stays 0. Port it -> LIVE keyboard nav
      works, no harness change. For AUTOMATED glReadPixels proof: add an
      env-gated synthetic nav-edge inject -- the PROVEN PcPort_ForcedKernelSelect
      pattern (it already ORs g_C1ButtonStateReleased |= 0x20 Circle) -- OR the
      DOWN bit into g_C1ButtonStatePressedOnce. This is NOT the member_change
      ceiling: there the nav STATE MACHINE was bypassed; here the input is fully
      wired and only the reader is stubbed. (harness-context-artifact does NOT
      apply.)
    (C) NAV CONSUMER: func_801C55A0's cursor move (menu1Choice++/-- on UP/DOWN,
      wrap 0..6) is ALREADY PORTED. On a menu1Choice change it fires
      func_801E8978 (123i) + func_801E8070 (285i) -- the cursor/highlight
      rebuild -- both currently STUBS. CONFIRM (input 4) fires func_801C531C
      (195i, the submenu dispatcher, stub).
    (D) FIRST MILESTONE -- CURSOR MOVE (~827i, fully traced, separable):
      func_801C7D78 (130, reader -> g_Menu->input; deps Controller*/Sound*/
      func_80036410 ported, func_801C8574 22i stub) + func_801E8978 (123 ->
      func_801D1EE0 252i cursor-sprite build via func_8002675C) + func_801E8070
      (285 -> func_801C851C[ported] + func_801E8044 15i) + a synthetic DOWN-edge
      inject. Result: UP/DOWN moves the cursor highlight between options.
      Verify: inject DOWN -> glReadPixels shows the highlight/pointer on the
      next option. Separable from confirm/submenus.
    (E) SUBMENU SCOPE (context, later phases): CONFIRM -> func_801C531C
      dispatches by menu1Choice to ~11 submenu-screen entries (func_801D3674/
      D9808/D9F98/DBE54/DE29C/E0F78/E23CC/E2BE4/E3088/E8018 ...). Each is a
      FULL screen (Status/Equip/Items/Abilities/Gear/File) = its own big
      multi-function slice (each creates windows 2+ via func_801D397C). The
      first submenu (Status) is a dedicated later arc.
    (F) PHASED NAV PLAN:
      N1 CURSOR-MOVE (~827i, first slice): the (D) functions + the nav-edge
         inject -> the cursor moves. On-screen-verifiable, the first INTERACTIVE
         milestone (static render -> responds to input).
      N2 CONFIRM/CANCEL PLUMBING: func_801C531C dispatcher structure + cancel
         (input 5 -> exit) with the submenu entries STUBBED -> the menu responds
         to Circle (dispatches; the selected submenu is blank until N3+).
      N3+ SUBMENU SCREENS (one at a time, big): Status first, then the rest --
         each a full screen slice.
    RECOMMENDATION: N1 (cursor-move). Verdict is YES (harness drives it); the
    slice is ~827i fully-traced, separable, on-screen-verifiable. It turns the
    menu from a static render into one that responds to input -- the first
    interactive milestone of the nav arc.
    ===== THE SYSTEMIC BLOCKER (found before porting stubs -- the guard
    fired EARLY) + THE FIX (delivered) =====
    A menu overlay's PORTED FUNCTIONS are NOT sufficient to render: each
    overlay's .rodata/.data must ALSO be MIGRATED as C (the data_field.c
    pattern), because the port auto-generates overlay data symbols as ZEROED
    stubs and the runtime overlay-load writes g_PsxRam (PSX_ADDR) -- a
    SEPARATE buffer from the ported functions' x86 data globals. member_change
    is ~90% ported in CODE but its LAYOUT DATA (D_801CB180 window rect
    {0,0,180,276}, D_801CB190 {0,0,200,200}, cursor/char slot positions,
    texcoords -- 16 symbols, 1052 bytes) was ZEROED -> a 0x0 window with
    everything at position 0 -> renders NOTHING.  Empirically confirmed: the
    real values live in member_change_menu.bin .data (file 0x6180+) and
    match the source-comment tables in misc.c. So porting the 10 draw stubs
    ALONE would NOT have rendered member_change -- the data is the systemic
    gate (the brief's "deeper blocker", found cheaply before the stubs).
    DELIVERED: pc_port/src/data_member_change_menu.c migrates all 16 symbols
    verbatim from the binary (correct-by-construction; matches misc.c
    comments); registered in build_port.sh PORT_SOURCES; port LINK OK; the
    symbols are now real .data (nm: D_801CB180 type D), removed from the
    zeroed stub set. Port-only -- slus a55929a1 + tripwire (map014 D_800B2264=0,
    clean) untouched.
    ===== RENDER HARNESS BUILT + THE WINDOW RENDERS (architecture +
    data-sufficiency for LAYOUT proven empirically) =====
    HARNESS (captures/render_diag/member_change_menu_harness.gdb, reusable):
    boot a field map, force D_800ADB64=1 at a stable frame -> the field
    menu-open trigger (main.c:618) fires func_800799D4 -> loads the overlay
    -> MenuMain -> MenuExecute case 1 -> MemberChangeMenuMain; glReadPixels
    capture. VERIFIED on map005: MemberChangeMenuMain ENTERED (D_80059460=1),
    the menu RUNS.
    RESULT (glReadPixels ground truth): the member_change WINDOW RENDERS --
    a large window rect at a REAL position/size (frame ~95 after open, 51%
    non-black), NOT the 0x0 collapse the zeroed data would give. So the data
    migration is proven SUFFICIENT FOR THE WINDOW LAYOUT, and the menu render
    architecture works end-to-end (invoke -> overlay-load -> MenuMain ->
    dispatch -> MemberChangeMenuMain -> window draw).
    THE EMPTY CONTENT IS DRAW STUBS, NOT MORE DATA (sufficiency answered):
    the window is empty (no border/text/sprites) because member_change's
    content-draw path is stubbed -- func_801C59E0 (108i) + func_801C95A0
    (63i) [member_change draw helpers, INCLUDE_ASM, they call
    SystemRenderStringEntry + LoadImage + GetStringEntry] and the shared
    func_8002xxxx (func_80026338 43i leaf, func_8002675C 172i POLY_FT4,
    func_8002DD20 49i TIM loader). This is the STUB layer, not a data gap.
    THE TEXT-RENDER PORT IS INTRICATE (next-pass scope, not a quick stub):
    SystemRenderStringEntry (59i) sets up a CONTIGUOUS ~0xF0-byte descriptor
    struct based at D_80059FD8 (fields at +0x00/02/08/0A/0C/10/12/1C/28/2C/
    68/69/.., verified against the writes) and calls func_80033DF0(&D_80059FD8);
    func_80033DF0 (ALREADY ported, system.c:350) reads it via arg0+OFFSET, so
    the port must make D_80059FD8 one contiguous buffer (NOT 18 separate
    zeroed symbols -- the same alias trap as the overlay data). Port
    SystemRenderStringEntry writing via offsets into that buffer + the
    func_801C59E0/95A0 callers, then the harness proves TEXT.
    ===== TEXT PATH PORTED (renders content, not-yet-clean glyphs) =====
    DONE: (a) SystemRenderStringEntry (system.c, coexistence) -- the
    CONTIGUOUS descriptor handled correctly: ONE static 0x100-byte buffer
    (s_StringRenderDescriptor), fields written by offset (+0x00..0xEA), +0x28
    holds a host pointer to the row buffer at +0x90; func_80033DF0 (ported)
    reads it via arg+offset. The alias trap the scope flagged -- solved by the
    single buffer, not 18 symbols. (b) func_801C59E0 (member_change misc.c) --
    the content-label drawer (INCLUDE_ASM -> C). (c) func_8002DD20 (temp2.c,
    coexistence) -- the TIM texture/CLUT loader.
    A REAL BUG FIXED en route: func_801C59E0 read g_Menu[0x558] as *(void**)
    (8 bytes) but the asm uses lw (4 bytes, a 32-bit PSX pointer); the 8-byte
    read pulled in the adjacent +0x55C field (0x30000000) -> a garbage pointer
    -> SIGSEGV in LoadImage. Fixed to a 4-byte read (port heap < 4GB so the low
    word is the whole host pointer). LESSON: raw g_Menu pointer reads must match
    the asm's lw width (4 bytes), never *(void**).
    RESULT (glReadPixels): render advanced 51.3% -> 55.4% -- the window now has
    CONTENT at the text/border positions, but it renders as vertical COLOUR
    BANDS, NOT clean glyphs. Palette is NOT the cause (SystemTransferPaletteTo-
    VRAM is ported). The remaining gap is the window-BORDER / content draws
    func_80026338 + func_8002675C (POLY_FT4 setup) -- STILL STUBBED. So the
    text PIPELINE runs (no crash, positions correct, TIM loaded) but clean
    glyphs need those border-draw stubs. HONEST: partial -- pipeline ported +
    running, clean text NOT yet proven.
    slus a55929a1 UNCHANGED (system.c/temp2.c coexistence); tripwire maps 1/14
    boot clean (200 frames) -- SystemRenderStringEntry going live didn't
    regress the field.
    ===== BORDER PORTED -- BANDS RESOLVED (they were the border) + WINDOW
    FRAME RENDERS CLEAN =====
    The bands ambiguity is RESOLVED on BOTH forks:
    (1) GLYPHS RASTERISE (proven directly, not guessed): dumped the text work
    buffer (g_Menu[0x558]) mid-member_change under GDB -- 88% nonzero, and
    rendered as a 4bpp image shows CLEAN GLYPH SHAPES. func_80034FFC (the glyph
    blitter, already ported) works. So the bands were NEVER a rasterisation
    failure.
    (2) THE BANDS WERE THE BORDER: func_8002675C (the window-border POLY_FT4
    builder) was stubbed, so the border poly slots held GARBAGE and drew as the
    colour bands once func_801C59E0 triggered the full menu render. Porting the
    border pair CLEARED them.
    DONE: func_80026338 (temp1.c, coexistence) -- atlas-entry -> tpage/CLUT/
    texcoord unpacker; func_8002675C (temp1.c, coexistence) -- the 172-instr
    POLY_FT4 border builder (per-item scale>>12, H/V flip via +0x1A/0x1B,
    4-vertex + UV emit, returns item count).
    RESULT (glReadPixels): 55.4% -> 58.8% -- member_change now renders a CLEAN
    WINDOW FRAME (light border + corner ornaments + cursor). A recognisable
    menu window. The glyphs rasterise (work-buffer proof). HONEST: the window
    FRAME is the clear proven win; the in-window text-LABEL display isn't
    clearly visible in the harness capture (empty interior -- likely no party
    data in the forced-open harness state, or a draw-order detail) -- the text
    PIPELINE is proven at the work-buffer level but its on-screen label render
    in this state is not yet visually confirmed.
    slus a55929a1 UNCHANGED (system.c/temp1.c/temp2.c coexistence); matching
    468/468; tripwire map14 boots clean (200 frames).
    ===== IN-WINDOW LABEL DISPLAY: DIAGNOSED AS A HARNESS-STATE LIMIT (not
    party data, not a render bug, not a port gap) =====
    Traced the empty interior end-to-end under GDB. The label PIPELINE is
    complete and correct; the labels don't show because the forced-open
    harness never reproduces the menu's navigation/layout STATE:
    - func_801C59E0 IS called (count=4, valid work buffer) and builds the
      FIXED labels (string idx 9-12 from D_801CB400) -- so it is NOT a
      "no party data" issue (those labels are party-independent). Brief's
      hypothesis corrected.
    - func_801C57A0 (faithfully ported -- its asm sets UV/tpage but NOT
      screen XY, confirmed) leaves the poly screen position to a separate
      transform/layout pass.
    - func_801C7DA8 AddPrim's the label polys to ot[4] ONLY if
      g_Menu->pManager->unk34[i] != 0 -- and in the forced-open harness those
      flags are ALL ZERO, and the poly XY is 0 (unpositioned).
    - Forcing unk34[i]=1 AND setting the poly XY by hand STILL did not show
      the labels (58.8% unchanged) -> there is a further per-frame dependency
      (a transform pass overwriting the forced XY, a text-VRAM/UV mismatch, or
      a draw-order/OT detail) that manual state-forcing did not reproduce.
    CONCLUSION: the in-window LABELS cannot be visually proven from the
    D_800ADB64=1 forced-open harness -- they need the real menu-navigation
    state (item visibility flags + the layout/transform pass), which only the
    proper menu flow sets up. That flow is reached by ENTERING member_change
    from the main menu -> so the on-screen label proof is gated on the
    main-menu port (func_801C62A8), the next arc. Porting func_801C95A0
    (portraits) was NOT done: it is party-dependent and gated by the same
    state, so it would be equally invisible in this harness.
    HONEST NET for member_change: PROVEN = window frame (border/corners/cursor)
    + glyph rasterisation (work-buffer) + a complete label build/AddPrim
    pipeline. NOT PROVEN = the labels/portraits ON-SCREEN (needs the real menu
    flow). No code this pass (func_801C57A0 is faithful, no port gap; the gap
    is harness state). HEAD stays fc9b18d.
    ===== UPDATE: tried to DRIVE member_change; the content display is a
    LAYOUT-STATE-MACHINE limit, deeper than "send input" =====
    Ported func_801C95A0 (portraits/char-name pre-render) -- member_change is
    now down to 1 remaining INCLUDE_ASM (nearly code-complete). Then traced
    the content-display gate at frame 380 (loop running):
    - The menu RUNS (windowParams[2]->unk11=1 open; shouldRenderWindow[2]=1;
      unk46 "render characters"=1; shouldRenderCursors=1 -> window + cursor
      render). But the CONTENT polys (labels unk4E0, characters) sit at screen
      XY (0,0) -- unpositioned.
    - The content is positioned by RotTransPers4 (ported) projecting each
      MenuString's vertices[4] (0x50) -> poly XY. But those vertices[] are 0,
      so the projection is (0,0). The vertices are set by an item-LAYOUT pass
      that only runs in the real menu-navigation state.
    - Forcing pManager->unk34[i]=1 EARLY (before the transform) STILL left the
      poly XY (0,0) -- proving unk34 is not the gate; the missing piece is the
      vertices[]/layout pass, not a flag. Porting func_801C95A0 (the char
      content) also didn't display -- SAME layout gap.
    CONCLUSION: member_change's on-screen CONTENT (labels + characters) is
    gated by the menu's item-LAYOUT state machine (sets each item's vertices[]
    + render flags), which the forced-open (D_800ADB64=1) harness does NOT
    reproduce. This is significantly MORE than "send input" -- it needs the
    real menu-navigation flow. And per (E) that flow is NOT the main-menu port
    (separate overlays) -- it is member_change's OWN entry+nav, which needs the
    specific field/game state to enter member_change normally.
    HONEST NET for member_change: CODE ~complete (1 INCLUDE_ASM left); PROVEN
    on-screen = window frame + cursor; PROVEN buffer-level = glyph raster;
    NOT PROVEN on-screen = labels/characters (gated on the real menu-nav/layout
    flow, a non-trivial state reproduction -- NOT cheaply reachable via the
    forced-open harness, contra the earlier "small task" hope).
    RECOMMENDATION: stop chasing member_change's on-screen content via the
    harness (dead end without the layout state). member_change is a validated
    proof-of-concept (recipe proven; code ~complete; frame on-screen). Decide
    the next arc on its own merits: the MAIN MENU (func_801C62A8, 301 fns + 208
    data -- big, the menu-system payoff, and it self-drives its own layout so
    ITS content would display), or other tracks (MAP3 func_8009EB78, battle,
    general decomp). The main menu does NOT resolve member_change (decoupled).
    VALIDATED RECIPE: overlay port = CODE + DATA migration + draw-stub port
    (text pipeline + border proven); on-screen CONTENT additionally needs the
    menu's navigation-state flow.

TOTAL SCOPE: Phase 1 (infra, 1 big mechanical pass) + Phase 2 (~15-25 fns,
multi-pass, size firms up after Phase 1) + Phase 3 (~4-5 bounded fns).
DEPENDENCY (REVISED after Step 2B): Phase 2's object-DRAW port is NOT on
the MAP3/MAP16 critical path -- Step 2B proved its target (func_801E742C et
al.) is incoherent menu-context code, and func_800A1364's real unblock is
sprite-load + IP-advance (un-spin), with the draw handled by SAFE no-ops.
So the order is now Phase 1 -> Phase 3 (activate + no-op the draw); Phase 2
(recovering the real object-draw) is DEFERRED and only pursued if the MAP16
empirical test shows the objects are load-bearing (needs runtime capture).
The 2 bounded MAP3 stubs (230A8/88198) and Phase 2b remain order-independent.
Wall 3's "activating func_800A1364 regresses maps" is now understood: the
regression was the incoherent draw path, which the no-op prevents. FIRST BOUNDED PHASE: Phase 1 (menu.bin infra) -- a
well-defined mechanical splat bring-up (config + split + baseline), the
concrete non-blind starting point; optionally warm up with the 2 bounded
MAP3 stub leaves first. RECOMMENDATION: this is the single highest-leverage
remaining arc (MAP3 + MAP16 gameplay + the entire menu system on one infra
bring-up), but it is a multi-pass sub-project gated on Phase 1's overlay
extraction -- fund it as a dedicated arc, start at Phase 1.

MAIN-MENU PORT (func_801C62A8) -- PHASED PLAN (scoped read-only; the
member_change recipe is validated, but the SCALE is ~5x and the render is
MONOLITHIC, not incrementally sliceable):
(A) TREE = 301 menu.bin functions / 37,192 instrs -- NOT 96 (the earlier
    estimate undercounted; the under-estimate pattern again). func_801C62A8
    (86i dispatcher) -> init (func_801C5F10 10fns + func_801C7B0C 16fns = 26
    fns, but init does NOT render -- reaches neither the border draw nor
    DrawOTag) -> a MONOLITHIC render/loop core (func_801C55A0 268 / func_-
    801C58EC 269 / func_801C57A4 271 fns -- heavily overlapping, ALL reach
    DrawOTag + border + text). Smallest subtree that reaches the render:
    func_801C8694 (86 fns) / func_801D2D38 (96 fns). So there is NO small
    "frame-only" slice -- the render infra is shared across everything.
(B) DATA MIGRATION = 208 symbols (15 .rodata @0x801C50xx + 193 .data
    @0x801E96A4-0x801EA904, ~0x1260 bytes) -- 13x member_change's 16. Large
    but MECHANICAL (the data_field.c / data_member_change pattern: scripted
    verbatim extraction from menu.bin). Required for any render (the port
    auto-zeros overlay data). Self-contained, no code dependency.
(C) SHARED DEPS mostly DONE (member_change paid this off): of 111 external
    callees, only 6 are STUBBED -- SoundEnable/MuteAllSpuChannels (2 sound),
    func_800263E4 / func_8002A498 / func_80036410 / func_8003852C (4 draw/
    system). The text pipeline (SystemRenderStringEntry + descriptor), border
    (func_80026338/8002675C), TIM (func_8002DD20), glyph blitter (func_-
    80034FFC) are all ALREADY ported.
(D) FIRST BOUNDED SLICE: no small testable render exists (the render is
    monolithic). Two viable first steps: (i) the DATA migration (208 symbols,
    mechanical, required, but INVISIBLE alone -- 0% code ported here, unlike
    member_change which was 90% pre-ported), or (ii) dispatcher + init +
    smallest render subtree (func_801C8694, 86 fns) + stub the rest + the
    data -> the first TESTABLE render (~110 fns). Testable via the harness
    (force D_800ADB64 -> D_80059460=0 = the main-menu case, glReadPixels).
(E) member_change NAV RESOLUTION -- CORRECTED: the main-menu port does NOT
    resolve member_change's on-screen labels. The main menu and member_change
    are MUTUALLY-EXCLUSIVE overlays (both VRAM 0x801C5000); the main menu
    never calls member_change (no D_80059460=1 setter in menu.bin -- it is
    entered via the field's MenuMain dispatch, separate overlay load).
    member_change's label-visibility (pManager->unk34) + poly positioning are
    set by member_change's OWN input-driven navigation, which the forced-open
    harness bypasses. So member_change's labels are resolved by DRIVING
    member_change (harness input / navigation), NOT by this port.
(F) PHASED PLAN: Phase A = DATA migration [DONE -- pc_port/src/data_main_menu.c,
    all 208 symbols VERBATIM from menu.bin, verified in the linked binary
    (D_801C5028="BASLUS-00664", D_801E96A8={1,2,4,8,16,32}); nm: all type D,
    0 remaining in the zeroed stub set; CONTIGUITY guaranteed via a per-section
    blob + .set aliases at exact offsets (relative layout matches PSX exactly
    -- the alias-trap is handled for any struct read by arg+offset); slus
    a55929a1 untouched, tripwire map14 clean].
    Phase B1a [CODE DONE, runtime-proof PENDING] -- dispatcher + init
    allocation slice (11 fns: func_801C62A8 + func_801C5F10 + the 9 alloc
    toggles) ported to C in src/menu/main/misc.c. KEY FINDING that reshapes
    the port: the port's SystemMenu/MenuManager/etc are NATIVE layout (8-byte
    pointers inflate offsets -- unk2E0 is at C-offset 0x428, not PSX 0x2E0),
    so the port is NOT a raw-offset transcription of the asm -- it MUST use
    struct FIELDS + sizeof(native type) (member_change's convention, which
    de-risks the type mapping). Byte-array fields that hold PSX 4-byte pointers
    (unk340[0]/[4], unk39C[i*4]) need 4-byte truncated storage (native 8-byte
    pointers overflow the u8[]). VERIFIED: port LINK OK, functions correctly
    placed (nm/breakpoints -> misc.c), matching build 468/468 + slus a55929a1
    UNCHANGED (the C compiles under gcc-2.7.2 too), tripwire map14 clean,
    member_change harness un-regressed. NOT verified: the runtime milestone
    (force main-menu case -> dispatcher runs -> reaches render entry) -- the
    gdb probe to force D_80059460=0 / D_800ADB64=0x80 hit a persistent batch
    "Invalid cast" quirk (the symbol writes work INTERACTIVELY but not inside a
    gdb command-block), so the run never reached the main-menu path. This is a
    PROBE limitation, NOT a known crash. NEXT (B1b): (i) confirm the runtime
    milestone via a working force (a code-side env override, or a fixed gdb
    harness), (ii) port func_801C7B0C (needs nested MenuSelectionMenu field
    mapping) + the 4 big window/POLY setup fns (func_801C6400 125i / 65F4 302i
    / 6AA0 171i / 6F70 211i, which READ the migrated Phase-A data) + the
    remaining ~10 init fns + the 3 sprite helpers.
    Phase B2 = smallest render subtree
    (func_801C8694, 86 fns) + stub the option branches -> first TESTABLE
    main-menu window render. Phase C = fill in the render core (55A0/58EC/
    57A4) + the option/nav/submenu branches (~180 remaining fns), multi-pass.
    NB the native-struct finding means EVERY main-menu fn needs struct-field
    mapping (member_change provides the types/patterns), so it is more intricate
    than "transcribe the asm" -- but tractable and de-risked.
    RECOMMENDATION: Phase A (data migration) first -- bounded, mechanical,
    required, de-risks everything. But BE HONEST about scale: this is ~301
    fns + 208 data symbols, a LARGE multi-pass arc (~5x member_change), and
    unlike member_change (90% pre-ported + self-contained) it is 0% ported +
    monolithic-render, so the first VISIBLE render needs ~110 fns, not a small
    slice. The recipe is de-risked; the effort is not small.
Prior overlay state: member_change_menu (95.5% matched) and shop_menu (84.7%)
ARE disassembled; member_change is the ported proof-of-concept.

DRIVE ORDER (what unblocks the most): the FIELD path is the high-yield
incremental track -- the SEGV cluster (1 root -> 4 maps) then the buildProc
gap (1 wire -> 2 maps) reclaim 6/9 failing maps with two small bounded
fixes, and the field-script/animation gaps they expose are the same class
of "wall" that blocks deeper play on the 15 already-booting maps. The menu
overlay is orthogonal and heavy -- park it as a separate arc. Dialog needs
nothing.

RANKED WORKLIST (bounded targets, incremental-field track):
1. SEGV cluster root (func_80021BCC / func_8009E094 NULL) -- ~1 fn trace +
   fix; payoff 4 maps; SIZE: one pass. RECOMMENDED FIRST.
2. Model buildProc prim-0 gap -- 1 table wire; payoff 2 maps; one pass
   (may share the decomp of the prim-0 build proc).
3. Map25 modelData-variant-1 + Map15 func_8008399C asserts -- 1 decomp each;
   1 map each; small.
4. Map250 hang stub-cluster -- port the spinning fns; medium.
-- separate heavy arc --
5. Main menu.bin overlay: build-infra bring-up + mode-0 render tree
   (~6 overlay fns + draw callees). Multi-pass sub-project; unblocks all
   menus (main/load/member-change/shop). Do as a dedicated arc.

Field opcode/TU backlog (the steady drip behind deeper play): field 172
unported -- misc.c 64 (FE-extended script handlers), misc11 25, misc7 19,
misc5 15, misc9 12, misc6 10, misc8 9. func_800248D4 anim set: 7 opcodes
unimplemented (0x85/8E/98/C8/D4/E2/FA) but PROVEN unreachable in all 730
maps' per-map anim packages (global/battle packages an open coverage gap).
Recommendation: drive the field track (target 1 first); it is the play-
forward path and each fix is bounded + measurable (a map flips to
PLAYS+SOUND). Menu overlay is a separate funded arc when menus are the goal.
Repro (survey): scratchpad/map_survey.sh (24 maps, verdict+stub-cluster
per map). Evidence: proven (boot survey + SEGV backtraces + decomp_status
counts + dialog liveness capture + menu-overlay asm re-read)
Last verified @ 7774804

---

## Sound cold-init is blocked below the decomp by hollow PsyCross SDK primitives

`src/slus_006.64/system/sound.c` is linked and all ten audited sound layouts are
retail-correct, but the port's field-test boot bypasses retail's
`SoundInitialize(0)` call. Consequently `D_800595D8`, the packed 32-bit current
audio-manager address, remains zero while field code calls the sound API.
Replacing the live oracle stubs with their exact-matched bodies therefore faults
on the first manager-element access. Retail has no null guard; adding one would
hide the missing initialization rather than restore it.

The current gate is below `SoundInitialize`: several PsyCross SDK symbols on
the successful cold-init path link cleanly but are implemented only as
`PSYX_UNIMPLEMENTED()` no-ops. **Symbol-resolved does not mean implemented.**
In particular, the earlier dependency classification called
`SpuSetReverbModeType` "real" because it was not in the generated stub manifest;
that was wrong. Its PsyCross body is a hollow implementation.

Cold-init reaches these hollow SDK primitives without taking an error path:

- `OpenEvent` (`pc_port/extern/PsyCross/src/psx/LIBAPI.C:140`) returns zero and
  retains no callback. Retail uses it at `SoundInitialize` `0x80037C84` to
  register `func_8003C020`.
- `EnableEvent` and `DisableEvent` (`LIBAPI.C:152-161`) are no-ops used by sound
  heap allocation and audio-manager list insertion.
- `SpuSetIRQCallback` (`LIBSPU.C:356`) and `SpuSetIRQ` (`LIBSPU.C:344`) are
  unconditional at `0x80037CCC` and `0x80037CD4`.
- `SpuSetCommonAttr` (`LIBSPU.C:191`) is reached synchronously through the
  cold-init `SoundSetCdAttr` call and later by the timer callback.
- `SpuSetReverbModeDepth` (`LIBSPU.C:236`) is unconditional in
  `func_800386C4`, and `SpuSetReverbModeType` (`LIBSPU.C:230`) is reached by
  the normal first reverb-allocation branch. Both are no-ops.

There is also a missing infrastructure layer rather than a single stub:
PsyCross's `SetRCnt`/`StartRCnt` record counter state, but there is no event pump
that invokes the counter-2 callback. Fixing `OpenEvent` alone would therefore
allow registration while `func_8003C020` still never fires.

This also blocks the three already exact-matched PsyQ leaves
`SpuGetReverbModeType`, `SpuSetReverbModeDelayTime`, and
`SpuSetReverbModeFeedback`: their retail behavior depends on reverb state that
the hollow type/depth functions never maintain. Exposing those three symbols
without the state layer would create a symbol-complete but behaviorally partial
initialization path—the same partial-init trap in a quieter form.

Retail calls `SoundInitialize(0)` from `func_80019578` at
`0x80019668-0x8001966C`. `SoundInitialize` spans
`0x80037B88-0x80037DBC` and stores `func_8003B148(0x10)` into
`D_800595D8` at `0x80037D50-0x80037D68`. Its complete known dependency
tree has four legs:

- **Manager allocation:** `func_8003B148` calls `func_8003B32C` at
  `0x8003B194`; that calls real `SoundInitializeAudioManager` at
  `0x8003B358`; the lower chain `SoundHeapFree`, `func_8003B930`, and
  `func_8003B32C` is now exact-matched. `func_8003B148` remains unported. Its
  allocation-failure exit also calls
  stubbed `SoundHandleError` at `0x8003B184`. That handler is not a leaf: when
  control flags `0x88` are clear, retail `0x8003F6E0-0x8003F724` reaches
  stubbed `SoundLoadWdsFile` and `func_80039E60` in addition to real
  `SoundSpuMemoryFreeBlock`, `SoundAddSedsEntry`, and `func_8003BDFC`.
- **CD mix:** `SoundInitialize` calls unported `func_800386C4` at
  `0x80037D04`; that path reaches unported `SoundSetupCdMix`.
- **Reverb:** real `SoundSetReverbModeWithAllocation` still reaches generated
  stubs for `SpuGetReverbModeType`, `SpuSetReverbModeDelayTime`, and
  `SpuSetReverbModeFeedback`; its symbol-resolved `SpuSetReverbModeType` and
  `SpuSetReverbModeDepth` dependencies are PsyCross no-ops, not real backends.
- **Timer tick:** `SoundInitialize` registers unported `func_8003C020` as the
  recurring callback at `0x80037C7C-0x80037C84`. Its sound-tick graph reaches
  at least `func_8003E900`, `func_8003AE84`, `func_8003A838`,
  `func_8003EBF0`, and `func_8003EB5C`, all still unported.

`SoundSpuMemoryAllocateBlockAtAddress` (`0x800395B8-0x800396DC`) is an
independently portable leaf: its only call is the real
`SoundSpuMemoryGetFreeBlock` at `0x80039678`.

The corrected sequencing is:

1. implement and behaviorally validate the PsyCross SDK integration layer
   (coherent reverb state, `SpuSetCommonAttr`, IRQ state/callbacks, and actual
   event/timer delivery);
2. exact-match the ten cold-init decomp functions;
3. route `SoundInitialize(0)` only as a diagnostic proof, verifying manager
   allocation, timer delivery, reverb-transfer completion, and zero stub hits;
4. then restore and extend the live playback path.

The SDK work has no objdiff oracle; validate it as host integration, like
`game_overrides.c` and the host walkers, using state/behavioral probes. Do not
resume the ten-function decomp or route boot on top of hollow primitives.
Earlier routing would construct a partially initialized manager and register a
non-delivered tick callback, turning a loud null dereference into quiet wrong
state. Four live playback helpers
(`func_8003A20C`, `func_8003A344`, `func_8003A450`, and `func_8003A55C`)
already have objdiff-`{}` C transcriptions, but they must remain oracle-stubbed
until initialization is complete.

Repro: `sed -n '140,245p' pc_port/extern/PsyCross/src/psx/LIBSPU.C` and
`sed -n '135,165p' pc_port/extern/PsyCross/src/psx/LIBAPI.C` show the
`PSYX_UNIMPLEMENTED()` happy-path bodies. `rg 'func_8003C020' pc_port/extern/PsyCross/src`
finds no counter/event delivery path. The original runtime repro remains:
locally replace `func_8003A20C` with its exact-matched C body, rebuild, and run
`timeout 20s ./scratchpad/run_map001.sh`; GDB faults with `D_800595D8 == 0`.
Restore the oracle stub afterward.
Evidence: proven
Last verified @ 609c426

### Scoping pass (design + pricing, no implementation) — dd0dbfe

Read-only design pass extending the map above. No showstopper found; the
project is large and multi-layer. Key facts established this pass:

**Host backend is REAL and can produce audio (not a showstopper).** The SPU
backend is `pc_port/extern/PsyCross/src/audio/PsyX_SPUAL.cpp` — an OpenAL SPU
emulation with the full playback path: voice attributes, key on/off, SPU-RAM
`Write`/`Read`, and reverb via OpenAL EFX effect slots
(`alAuxiliaryEffectSloti`, `g_nAlReverbEffect`). "No output path exists" is
false.  BUT it is entirely dormant: `g_spuInit` is 0 because `SpuInit()` /
`PsyX_SPUAL_InitSound()` is never called in the port boot, and every backend
function early-returns on `if (!g_spuInit)`. So even the *wired* PSYQ
primitives (`SpuSetReverb`, `SpuSetVoiceAttr`, `SpuSetKey`) currently no-op.

**Five distinct blockers, do not conflate:**
- **B0 backend device dormant:** `SpuInit`/`PsyX_SPUAL_InitSound` absent from
  the port boot (`grep` in `pc_port/src/*.c` finds no call). Prereq for
  everything below.
- **B1 cold-init not routed:** retail reaches `SoundInitialize(0)` at
  `func_80019578:0x80019668` (verified in the asm), followed by 4×
  `SoundLoadWdsFile`. `port_main.c`'s `main()` re-implements the other
  `func_80019578` boot duties (HeapInit, state reset, disc) but NOT
  `SoundInitialize`. **Menu-thread trap confirmed live: a perfect layer behind
  an uncalled init is silence.** Insertion point is known and bounded (the
  same `port_main.c` shim), but it is REQUIRED, first-class work.
- **B2 event pump missing (the core architectural gap):** `counters[3]`
  (LIBAPI.C) store value/target/cycle but nothing advances them or dispatches
  on target; `OpenEvent`/`EnableEvent`/`DisableEvent` are hollow (retain no
  callback). The sound tick `func_8003C020` is registered as a **counter-2**
  event and never fires. Leverage: `intrThreadMain` (PsyX_main.cpp) already
  fires `vsync_callback` at the NTSC/PAL timestep on a dedicated SDL thread —
  the pump is an *extension* (event-table registry in OpenEvent + counter-2
  advance/dispatch in that thread), not a from-scratch build.
- **B3 hollow primitives (7):** `SpuSetCommonAttr` (master/mix state),
  `SpuSetReverbModeType` + `SpuSetReverbModeDepth` + `SpuSetReverbModeParam` +
  `SpuSetReverbDepth` (reverb), `SpuSetIRQ` + `SpuSetIRQAddr` (SPU IRQ).
  Reverb is the state-coherence trap the entry above warns of: on/off is wired
  but type/depth are not, and `SpuGetReverbModeType`/`DelayTime`/`Feedback`
  read state the no-ops never set. Design: a shared `SpuReverbAttr`-backed
  state struct + a 10-entry PSX-mode → OpenAL-EFX-param table
  (`SPU_REV_MODE_OFF..PIPE` → AL_REVERB decay/density/gain). Attr structs are
  flat scalars / SPU-RAM offsets (not host pointers) → **low LP64 surface** for
  the primitive layer; the LP64 work already lives in sound.c's 10 audited
  layouts (done).
- **B4 cold-init decomp unported:** `SoundInitialize` and its legs
  (`func_8003B148`, `func_800386C4`, `func_8003C020`, `SoundSetupCdMix`,
  `func_80039E60`, `func_8003E900/AE84/A838/EBF0/EB5C`, `SoundHandleError`) are
  all `INCLUDE_ASM` (sound.c has 132 unported functions total). Unlike the SDK
  layer, these ARE MIPS asm with an objdiff `{}` oracle — normal matched-decomp
  work, not host design.
- **B5 sample data:** `SoundLoadWdsFile` is stubbed — no sound banks load, so
  audible output is gated behind the playback path even after B0-B4.

**Behavioral probe suite (no objdiff oracle for B0-B3; "links != works"):**
1. `func_8003C020` tick actually FIRES — counter/log in the tick; expect
   ~counter-2-rate hits, not zero.
2. Reverb state round-trips — set type/depth then `SpuGetReverbModeType`/etc.
   return the set values (coherence, not defaults).
3. Zero stub hits on the `SoundInitialize` happy path — stub-hit log during
   init must be empty.
4. `D_800595D8` becomes non-zero AND the manager stays coherent (not the
   hollow-init trap: re-read manager fields after init).
5. `g_spuInit == 1` and an actual sample reaches OpenAL — probe backend voice
   activity / AL source state.

**Pricing (phased):**
- Phase 0 — route `SpuInit` at PsyX startup, set `g_spuInit`. Small (~1 call +
  device-init verification).
- Phase 1 — event pump: OpenEvent/Enable/Disable registry + counter-2 dispatch
  in `intrThreadMain`. Medium; bounded by existing infra. No oracle → probe 1.
- Phase 2 — primitive layer: wire 7 hollow primitives + reverb state/EFX table.
  Medium (~7 funcs + 10-entry table). No oracle → probes 2, 5.
- Phase 3 — cold-init decomp: `SoundInitialize` + ~12 named legs + transitive
  deps (est. 20-40 funcs). Large, BUT objdiff `{}` oracle exists.
- Phase 4 — route `SoundInitialize(0)` into `port_main.c` boot. Small.
- Phase 5 — behavioral validation (probes 1-5). Small-medium.
- Phase 6 (separate follow-on) — playback data: `SoundLoadWdsFile` + WDS load +
  the ~132-function playback engine. Large.

Milestone framing: "cold-init proven coherent, timer fires, zero stub hits"
(the entry's phase-3 diagnostic goal) = Phases 0-2 + 4-5 + enough of Phase 3 to
run `SoundInitialize`. "Audible in-game music" = all phases incl. B5/Phase 6.

**Recommendation: FUND, scoped to the SDK-integration milestone (Phases 0-2,
4-5), not the full engine.** Rationale: (1) no showstopper — backend is real,
init is routable; (2) B2 (event pump) is the single highest-leverage bounded
piece — it's reusable infrastructure and the one true "architecture, not a
stub" gap; (3) the SDK layer (B0-B3) is the no-oracle work that everything
above it needs and that only this design pass has de-risked; (4) defer B4
(has an oracle — normal decomp, can proceed independently) and B5/Phase 6
(playback) as follow-ons. Set expectations: audible sound is several phases
out; the fundable near-term win is a *behaviorally-proven initialized sound
subsystem* (manager coherent, timer delivering, reverb state live, zero stubs),
which converts "silent and uninitialized" into "initialized and ticking" — the
prerequisite state for all later audio work.
Evidence: proven (backend/boot/counter code read; init-absence grep-confirmed;
retail SoundInitialize call site verified in func_80019578 asm)
Last verified @ dd0dbfe

### Phase 0 + Phase 1 landed (backend awake + counter-2 event pump)

Implemented Phase 0 (B0) and Phase 1 (B2) of the plan above; the SPU subsystem
is now awake and the event pump ticks. Behaviorally validated (no objdiff
oracle for this layer) via a synthetic-callback probe, deliberately independent
of SoundInitialize (still B1/B4, not touched).

- **Phase 0 (B0):** `port_main.c` now calls `SpuInit()` after `PsyX_Initialise`
  (idempotent, before pad init). Confirmed `g_spuInit == 1` (GDB read of the
  real static + the new `PsyX_SPUAL_IsInit()` accessor), OpenAL device opens
  ("found sound device: OpenAL Soft", "PSX SPU effects ... initialized"). The
  backend early-return gate is now open; wired primitives no longer no-op.
- **Phase 1 rate:** derived + cited, NOT assumed. Retail `SoundInitialize`
  (0x80037C84) does `OpenEvent(RCntCNT2, EvSpINT, EvMdINTR, func_8003C020)` then
  `SetRCnt(RCntCNT2, 0x44E8, EvMdINTR)` + `StartRCnt`. Mode `0x1000` has no
  `RCntMdSC`/0x1 bit, so RCnt2 uses its default clock = system clock / 8 =
  33.8688MHz/8 = 4.2336MHz (the port's own `SetRCnt` confirms via the
  `spec==2 && !(mode&1)` branch). Target 0x44E8 (17640): 4233600/17640 = **240.0
  Hz exactly** — 4x the 60Hz vblank, so it runs on its own cadence.
- **Phase 1 registry + dispatch:** `OpenEvent`/`EnableEvent`/`DisableEvent`/
  `CloseEvent` in `LIBAPI.C` are now a real lock-free event table (was
  `PSYX_UNIMPLEMENTED` no-ops). `intrThreadMain` (PsyX_main.cpp) gained a second
  HPC timer that calls `PsyX_Sys_DispatchCounter2()` at 240Hz, dispatching
  enabled RCntCNT2/EvSpINT events — extending the existing vblank tick, not a
  new loop.
- **Synthetic probe (the pass/fail):** `XENO_SOUND_PUMP_PROBE=1` registers a
  counting callback via the real `OpenEvent` on the counter-2 event, enables it,
  and measures dispatch. Result: **PASS** — 120 ticks / 500ms = 240 Hz when
  enabled, +0 ticks over 200ms after `DisableEvent`, handle > 0. This proves the
  pump architecture (registry + enable-gating + 240Hz dispatch + disable)
  independent of SoundInitialize. When Phase 3/4 land the real func_8003C020
  registration, it flows through this identical, proven path.
- **NOT this pass (unchanged):** SoundInitialize routing/decomp (B1/B4), the 7
  hollow primitives (B3), WDS/playback (B5). The real `func_8003C020` does NOT
  run yet (nothing registers it until Phase 3/4) — the "func_8003C020 fires"
  probe belongs to that later milestone, not here.
- **Deferred concern:** cross-thread safety of a callback *body* against the
  main/field thread (retail used EnterCriticalSection to disable IRQs; the
  port's is a no-op). The synthetic probe touches no shared state; this must be
  addressed before the real sound tick runs concurrently (Phase 2+).

Persistence: PsyCross is a gitignored vendored tree, so the LIBAPI.C /
PsyX_main.* / PsyX_SPUAL.* changes live in `pc_port/patches/psycross_sound_pump.patch`
(marker `_xeno_sound_pump`), applied idempotently by `build_port.sh`.
Map014 tripwire boots clean (52 actors, reaches field main loop) with the pump
active on the interrupt thread.
Repro: `env XENO_SOUND_PUMP_PROBE=1 XENO_FIELD_TEST=1 XENO_FIELD_MAP=5 ...
pc_port/build_native/xeno-port` -> `[sound-probe] ... RESULT: ... PASS`.
Evidence: proven (synthetic probe PASS; g_spuInit confirmed; rate derived from
retail SetRCnt; tripwire clean)
Last verified @ 28f12e4

### Phase 3 call-tree measurement (diagnostic, no decomp) — f79d131

Measured the real transitive call tree under `SoundInitialize` by BFS over the
split asm (`asm/slus_006.64/nonmatchings/system/sound/*.s`) PLUS the C-body
calls of already-decomped functions (so undone functions hidden *below* done
ones are not missed), tagging every node oracle-decomp / already-done / SDK-real
/ SDK-hollow. Method + counts: `scratchpad/soundtree/`.

**Headline: NOT a menu-overlay-style surprise. The 20-40 name estimate is
accurate for the FULL init+tick scope (~23), but it hid a cheap init-proof
sub-milestone (~3-4 functions).** Most of the sound engine is already
decompiled (sound.c has ~94 real C bodies; 40 of the ~76 nodes in the
SoundInitialize tree are DONE), so the remaining work is small and splits
cleanly:

- **INIT happy-path decomp: 3 functions** (+ `SoundInitialize` itself) —
  `func_8003B148` (manager alloc, 50 asm lines; its lower chain
  SoundHeapFree/func_8003B930/func_8003B32C/SoundInitializeAudioManager is
  already {}), `SoundSpuMemoryAllocateBlockAtAddress` (83 lines, a leaf — only
  calls the real SoundSpuMemoryGetFreeBlock), `SoundSetupCdMix`. All small,
  all with an objdiff {} oracle. This is the *near-term* Phase-3 number, far
  below "20-40".
- **INIT full decomp (incl error + CD-mix legs): 13** — adds the error leg
  (`SoundHandleError → SoundLoadWdsFile, func_80039E60, func_8003A65C,
  func_8003B644, func_8003BDFC, func_8003E5BC, func_80039024`), which fires
  only on allocation failure (stub-first, deferrable), and CD-mix (`func_800386C4`,
  gated on control bit 0x4000 — optional at cold init).
- **TIMER-TICK leg decomp: 20** — `func_8003C020` (the 240Hz callback, run by
  the Phase-1 pump) reaches `func_8003E900/AE84/A838/EBF0/EB5C/EEA0/EFE4/
  C4C4/C6E8/CC84` + shared spu-mem/error funcs. This is the real bulk — needed
  for *ongoing audible* sound, but SEPARABLE: the tick body can stay stubbed
  while the init proof is validated (the pump already fires the callback).
- **Union (init-full + tick): 23 oracle-decomp functions** — within the 20-40
  estimate. So the estimate was right for "fully working sound," just not
  broken out by milestone.
- **SDK-HOLLOW (Phase 2, no oracle): 6 touch init** — `SpuSetCommonAttr`,
  `SpuSetReverbModeType`, `SpuSetReverbModeDepth`, `SpuSetIRQ`,
  `SpuSetIRQCallback`, `SpuReadDecodedData`. These are Phase-2 primitive-wiring,
  NOT Phase-3 decomp; do not price them into the decomp count.
- **Unclassified/minor:** `SoundTransferCallbackStore` (no .s, no C body, no
  macro — one node, likely a small helper); `FILE_SIGNATURE` is a macro (noise).
- **No sprawl beyond sound.c** — the tree stays inside the sound engine + the
  SDK primitives; zero genuine other-TU-undone game functions (the earlier
  "OTHER-TU" bucket was done C bodies + libc/macro false positives).

**Re-priced recommendation / sequencing:** the near-term *init-proof* milestone
("SoundInitialize completes, manager coherent, timer fires, reverb state live,
zero stubs on the init happy path") costs only **~3-4 small decomp functions +
6 Phase-2 primitives** — much cheaper than the "20-40" implied. Do Phase 2
(wire the 6 primitives) and the 3-4 init decomp together to reach it; stub the
error leg and defer the 20-function tick behind it. The tick leg is the real
bulk and is where the "20-40" mostly lives — deferrable, incremental, oracle'd.
Net: fund the init-proof milestone next (small, bounded); the full audible-sound
project is ~23 decomp + 6 primitives + WDS/playback (B5), sequenced after.
Evidence: proven (BFS over split asm + C-body edges; sizes from asm line counts;
already-done set cross-checked against sound.c C bodies)
Last verified @ f79d131

### Phase 2 landed (SDK primitives wired + behaviorally validated) + measurement corrections

Wired the init-reached hollow SDK primitives against the now-awake backend and
validated them behaviorally (no objdiff oracle for this layer). Shipped as
`pc_port/patches/psycross_sound_prims.patch` (marker `_xeno_sound_prims`,
applied idempotently by `build_port.sh`; reverse-and-rebuild verified). Two
measurement corrections from the Phase-3 BFS fall out of this pass.

- **Correction 1 — init-reached hollow primitives are 5, not 6.**
  `SpuReadDecodedData` is NOT on the init happy path: its only callers are the
  SPU-command handler at `sound.c:1403-1414` (`pCmd->pSpuData`, the tick/command
  leg), not `SoundInitialize`'s tree. It stays hollow, deferred to the tick leg.
  The 5 that DO touch init (verified by caller-trace): `SpuSetIRQ` +
  `SpuSetIRQCallback` (direct `SoundInitialize` jals), `SpuSetCommonAttr` (via
  `SoundSetCdAttr`, unconditional), `SpuSetReverbModeType` (via
  `SoundSetReverbModeWithAllocation`), `SpuSetReverbModeDepth` (via unconditional
  `func_800386C4` and `SoundSetReverbModeWithAllocation`).
- **Correction 2 — `func_800386C4` is UNCONDITIONAL, so the init-proof decomp
  count is 4, not 3.** The Phase-3 note bucketed `func_800386C4` as the optional
  CD-mix leg, but `SoundInitialize.s` calls it with a plain `jal` (no guard); it
  is the volume/reverb-apply leg. The *thing it optionally calls* is
  `SoundSetupCdMix` (gated on control bit `0x4000`, skipped at cold init).
  `SoundHandleError` is the allocation-failure error leg (skipped on success).
  So the init happy-path decomp set is exactly **4**: `SoundInitialize`,
  `func_8003B148`, `SoundSpuMemoryAllocateBlockAtAddress`, `func_800386C4`.

**What was wired (behavioral, NOT objdiff-matched — claimed as probe results):**
- Shared `SpuReverbAttr`/`SpuCommonAttr` state in `LIBSPU.C` so `Set` and `Get`
  round-trip. `SpuSetReverbModeType`/`Depth`/`ModeParam` write it;
  `SpuGetReverbModeParam` reads it back. A 10-entry PSX-mode → OpenAL-EFX preset
  table (`SPU_REV_MODE_OFF..PIPE` → gain/decay/gainHF) drives the real reverb
  effect via a new backend hook `PsyX_SPUAL_ApplyReverbParams` (gated on
  `g_spuInit` + `g_ALEffectsSupported`, re-commits `g_nAlReverbEffect` into the
  active aux slot).
- `SpuSetCommonAttr` merges per-mask into the shared common-attr state and, when
  `MVOLL/R` is set, drives master output via new backend hook
  `PsyX_SPUAL_SetMasterVolume` (PSX 14-bit master vol → OpenAL listener gain).
- `SpuSetIRQ`/`SpuSetIRQCallback` maintain enable state + registered callback and
  return PsyQ-correct values (IRQ returns arg; Callback returns previous). **No
  SPU IRQ source exists in the port backend**, so the callback is registered but
  never fired. **This is the pre-tick-leg gate** (boundary flagged): SPU-IRQ /
  streaming sync belongs with the tick leg, not init, and cross-thread callback-
  body safety (retail's EnterCriticalSection IRQ-disable is a port no-op) must be
  resolved before any real tick body runs concurrently.

**Behavioral probe — the pass/fail (`XENO_SOUND_PRIM_PROBE=1`, `port_main.c`):**
Exercises the primitives in isolation against the awake backend (as the Phase-1
pump probe validates the pump, independent of `SoundInitialize`). Result:
**PASS** — reverb round-trip `mode=HALL, depthL=0x4000, depthR=0x5000` read back
exactly via `SpuGetReverbModeParam`; `SpuSetCommonAttr` master vol `0x2000` →
listener gain `0.500`; `SpuSetIRQ(ON)` returns ON and the callback register
round-trips (`old1==NULL, old2==cb1`). Backend confirmed EFX-capable at run
("PSX SPU effects are supported and initialized"), so reverb/master calls drove
real OpenAL, not just state. The Phase-0+1 pump probe still PASSes on the same
binary (240Hz, +0 after disable) — no regression.

**NOT this pass (the remaining init-proof work, now precisely scoped):**
- **Phase 3 (4 decomps):** `SoundInitialize`, `func_8003B148`,
  `SoundSpuMemoryAllocateBlockAtAddress`, `func_800386C4` to objdiff `{}` (or the
  d88f13c coexistence pattern if codegen resists — behavior first, `{}` as the
  quality gate). Their lower chains are already `{}`. `func_800386C4` is a
  mode-switch flag machine + the `D_80059518`-gated `func_80038824` leg;
  `SoundSpuMemoryAllocateBlockAtAddress` is a block-search allocator loop.
- **Phase 4:** route `SoundInitialize(0)` into `port_main.c` boot (same shim
  family as the other `func_80019578` duties), keeping `func_8003C020`'s tick
  body stubbed.
- **Phase 5:** init-proof validation — init completes with zero stub hits on the
  happy path, `D_800595D8` non-zero AND manager fields read back coherent (not
  the hollow-init trap), real `func_8003C020` registered and firing at 240Hz
  through the Phase-1 pump (body stubbed).

Persistence: `pc_port/patches/psycross_sound_prims.patch` (LIBSPU.C +
PsyX_SPUAL.cpp/.h), wired into `build_port.sh` after the pump patch. Idempotent
(marker-gated apply; verified: applies from a clean baseline, skips on re-run,
build stays 223 stubs / LINK OK).
Evidence: proven (behavioral probe PASS on the patch-built binary; all 7 wired
symbols resolve to real `T` defs, zero stubbed; caller-trace for the 5-vs-6 and
unconditional-`func_800386C4` corrections from `SoundInitialize.s` +
`func_800386C4.s` + `sound.c`)
Last verified @ 74fe427

### Phase 3-5 landed: cold-init decomped, routed, and behaviorally coherent

The sound subsystem is now **initialized and coherent** in the port: the game's
own `SoundInitialize(0)` is decompiled, routed into boot, and validated to build
a coherent audio manager with the 240Hz tick firing (body still stubbed). This
resolves the top-of-file "sound cold-init blocked" issue for the init milestone
(playback/WDS remain, below).

**The 4 init happy-path decomps** (corrected count from the Phase-2 note):
- `func_8003B148` (audio-manager alloc + per-element voice assignment loop) --
  **objdiff `{}`**.
- `SoundInitialize` (the ~40-store/call top-level init sequence) -- **objdiff
  `{}`** (packed manager pointer via `SOUND_PTR_TO_PSX`; 6 previously-undeclared
  globals + 3 callbacks now declared in sound.h).
- `SoundSpuMemoryAllocateBlockAtAddress` (block-search allocator) --
  **coexistence** (d88f13c). Logic traced 1:1; residual is a gcc-2.7.2
  register-coloring inversion (`addr`->s0 vs retail's s1, driven by `addr` being
  live across `SoundSpuMemoryGetFreeBlock`) + base-pointer caching. Not `{}`.
- `func_800386C4` (reverb-mode flag machine) -- **coexistence**. Logic traced
  1:1; residual is the switch `expand_case` decision tree (retail linear-from-low
  + tail-merged store vs gcc range-split); no switch/if-else variant reproduces
  it. Not `{}`. Its `D_80059518` echo-controller leg is dead at cold init.

`SoundSetupCdMix` (0x4000-gated) and the `SoundHandleError` error leg stay
stubbed/deferred as designed. Matching build green; the 2 `{}` decomps match and
the 2 coexistence functions stay byte-exact via `INCLUDE_ASM`. Pre-existing
`func_8003B32C` mismatch (struct `unk_0x18` width) is unrelated and untouched.

**Port integration (the func_80019578 duty port_main did not replicate):**
`SoundInitialize(0)` routed in `port_main.c` after `SpuInit`, before pad init.
Data-symbol sizing added to `symbol_addrs` for the new .bss the init path now
touches: the sound heap `D_80065B0C` (0x6300) and `g_SoundSpuMemoryTableStart`
(0x24). **Fixed an LP64 landmine:** `SoundClearVoiceDataPointers` strides by
`sizeof(SoundVoiceData*)` (8 on the port vs 4 retail) to offset 184, but
`g_SoundChannels` was stub-sized at the 32-byte default -> the clear overflowed
into the adjacent `g_SoundHeapHead` stub and zeroed it, crashing the second
`SoundHeapAllocate`. Sized `g_SoundChannels` to 0xC0 (24 x 8-byte pointers).

**Reverb round-trip completed:** the init path reaches `SpuGetReverbModeType` /
`SpuSetReverbModeDelayTime` / `SpuSetReverbModeFeedback` via
`SoundSetReverbModeWithAllocation` -- these were absent from PsyCross (stubbed).
Wired them onto the shared `s_reverbAttr` state (extends the prims patch), so the
init happy path hits **zero stubs**.

**Init-proof validation (`XENO_SOUND_INIT_PROBE`, all PASS):**
- Init completes; **zero stub hits on the synchronous happy path** (the
  `func_8003Axxx`/`func_80098430` stubs seen in a GDB window have no sound-tree
  callers -- they are async interrupt/field-thread work, not init).
- Manager coherent (fields read back, not just non-null): `D_800595D8`=0x7e0eb0,
  `elementCount`=0x10, `unk_Flags`=2, `unk_0x18`=0x7F, `unk_0x32`=1, `unk_0x38`=4.
- Real `func_8003C020` **registered** (`g_unk_SoundEvent`=1, callback resolved)
  and **firing at 240Hz** (post-init shadow counter: 120 ticks/500ms), body still
  stubbed -- the graduated tick probe.
- Reverb round-trip + pump + prim probes still PASS; Map014/47/334 tripwires
  boot clean (52/36/40 actors, reach field main loop, no crash) with init routed.

**Gate flagged (not forced):** SPU IRQ has no source in the port and
`EnterCriticalSection` is a no-op; safe while the tick body is stubbed (dispatched
stub touches no shared state). Real tick body + cross-thread/IRQ gating are the
gate before the tick leg. WDS/playback (`SoundLoadWdsFile`) NOT routed.
Evidence: proven (2 objdiff `{}` + 2 coexistence logic-verified; init-proof probe
suite PASS on the built port; tripwires clean; matching build green)
Last verified @ cfd96fb

### Tick-leg scoping (trace + gating design, NO implementation)

Read-only depth-trace of `func_8003C020`'s subtree + the cross-thread gating
design that must land before any real tick body runs. Method: BFS over the split
asm + C-body edges + the `g_SoundScriptHandlers` jalr table.

**(A) The ~20 estimate is a ~3.75x undercount: 75 unported functions.** BFS from
`func_8003C020` (following the `func_8003C6E8 -> g_SoundScriptHandlers[opcode]`
jalr table) reaches **75 unported** functions, not ~20. The estimate captured the
control layer but missed the entire data-dispatched sequence-command layer. Split:
- **Tick-core control: 12 unported** -- the empty-queue tick. Every-tick:
  `func_8003E900`, `func_8003EB5C`. Manager-active-conditional: `func_8003C4C4`,
  `func_8003C6E8`, `func_8003EFE4`, `func_8003EBF0`. Lifecycle-conditional:
  `func_8003AE84`, `func_8003A838`. Plus transitive `func_8003CC84` (<-C6E8),
  `func_8003EEA0` (<-EBF0), `func_8003E5BC`, and `func_8003C020` itself.
- **Sequence-command handlers: 51 unported** (of 97 distinct in the 128-slot
  `g_SoundScriptHandlers` table; 46 already `{}`, 31 slots are
  `SoundScriptDefaultHandler`). Statically ENUMERABLE from the table (not
  "untraceable") but only *invoked* when the tick processes real sequence data
  (needs WDS/B5) -- with an empty queue the tick hits Default/Nop handlers only.
- Handler-subtree helpers: ~4 more. Error/WDS leg (`SoundHandleError`,
  `SoundLoadWdsFile`, `func_80039024/39E60/3A65C/3B644/3BDFC`,
  `SoundSpuMemoryAllocateBlock`): 8, deferrable (folds into B5).
- **Runtime-trace-required (NOT statically enumerable):** `func_8003EFE4`'s
  second jalr is `obj->field0(obj)` -- a per-voice method pointer set at runtime
  (an ADSR/envelope state handler). Its handler set must be runtime-traced; it
  may add to the 75 if those handlers aren't otherwise reached.

**(B) Cross-thread gating design (the prerequisite; SPU-IRQ resolved).**
- *Shared state* the tick body touches: `g_SoundAudioManagerListHead` (manager
  linked list), `g_SoundVolumeController`, `g_SoundControlFlags`,
  `g_SoundCdFadeFramesRemaining`, `D_80059504/40/5C/C4`, and via the subtree the
  voice tables (`g_SoundChannels`), manager fields, and SPU-RAM (SpuSetVoiceAttr/
  SpuSetKey in handlers). The field/main thread touches the same via the sound
  API (SoundReset, heap alloc, manager add/remove, volume/reverb, playback ctl).
- *Retail's primitive:* `func_8003C020` does NOT gate itself -- it runs as the
  counter-2 IRQ handler (atomic on HW). The MAIN thread brackets every non-atomic
  shared-state access with `DisableEvent/EnableEvent(g_unk_SoundEvent)`
  (SoundHeapAllocate, SoundAddAudioManagerToList, etc.).
- *Port design:* map that discipline onto a **recursive** `g_SoundTickMutex`.
  `DisableEvent`/`EnableEvent` on the counter-2/EvSpINT event acquire/release it;
  the pump (`intrThreadMain`) holds it around the `func_8003C020` dispatch (today
  it dispatches under `g_intrMutex` but the main thread only flips the `enabled`
  flag -- no exclusion vs an in-flight tick). **Must be recursive**:
  `func_8003AE84` (on the tick path) re-enters `DisableEvent/EnableEvent`
  (SoundHeap*/EnterCriticalSection are NOT reached from the tick, so that is the
  only re-entry vector). SDL_CreateMutex is reentrant -- verify at impl. No
  deadlock (short critical sections, recursive lock); no stall (tick + sections
  are us-scale). NOT a showstopper -- the pump is already a dedicated thread and
  retail already brackets the accesses; no restructuring needed, tick body
  unchanged.
- *SPU-IRQ question RESOLVED:* the tick is the RCnt2 240Hz **timer** (already
  firing via the pump). `SoundSpuIRQHandler` is a SEPARATE SPU-IRQ path
  (streaming/transfer-completion for WDS/XA). **The tick leg does NOT need an SPU
  IRQ source -- that gap defers to B5, not here.**
- *Concurrency validation (the third regime):* (1) TSan build (-fsanitize=thread)
  over the sound TUs + PsyCross, boot + stress -> reports races directly;
  (2) stress probe: main thread hammers the sound API while the real tick fires
  at 240Hz, with invariant assertions (manager list acyclic/terminated, voice
  indices in [0,24), no manager use-after-free); (3) any un-bracketed multi-word
  access TSan flags is a pre-existing retail race to review.

**(C) Sequence (gate-first, leaf-up) + pricing.** NOT one pass.
1. **Gating pass FIRST** (its own bounded pass): recursive `g_SoundTickMutex` +
   wire DisableEvent/EnableEvent + pump dispatch; validate TSan + stress with the
   tick still STUBBED. Behavioral + concurrency, no oracle. ~1 pass.
2. **Tick-core (12)**, leaf-up: `func_8003E900/EB5C` -> conditional legs
   (`func_8003C4C4/C6E8/EFE4/EBF0`) -> `func_8003AE84/A838` -> `func_8003C020`
   last. objdiff `{}`. ~1-2 passes. Milestone: empty-queue tick runs safely.
3. **Script handlers (51)** batched, objdiff `{}`; enables sequence processing.
   ~2-4 passes. Audible needs B5 (samples) too.
4. **Error/WDS (8) + B5** (SoundLoadWdsFile, SPU streaming, sample banks): the
   large separate "audible" project.
Total tick decomp ~= **63 functions** (12 + 51) to `{}` + the gate-first pass,
vs the ~20 estimate. Recommend: fund gate-first, then tick-core, then handlers.
Evidence: proven (BFS over split asm + C-body + g_SoundScriptHandlers table
edges; gating traced from func_8003C020.s + retail DisableEvent discipline;
SPU-IRQ role read from SoundSpuIRQHandler)
Last verified @ 2ffd372

### Sound tick gate LANDED (tick-leg step 1; tick body still stubbed)

The gating pass above is implemented: `psycross_sound_gate.patch` adds
`g_SoundTickMutex` (SDL recursive mutex, created before the interrupt thread
spawns). `DisableEvent` on the counter-2/EvSpINT tick event acquires AND HOLDS
it (per-thread `__thread` depth); `EnableEvent` releases one level; unpaired
enables (func_80037F44 boot toggle) just flip the flag; `CloseEvent` dissolves
the closing thread's held levels (both port probes Disable -> Close without an
Enable). The 240Hz pump TRY-locks around dispatch: a held bracket DROPS the
tick (retail's disabled-event semantics), and TryLock makes pump stall and
lock-order deadlock structurally impossible. All registry mutations run under
the mutex. func_8003C020's body REMAINS STUBBED -- the mechanism is validated
before any real body can race.

Validation (the concurrency regime, XENO_TSAN=1 + XENO_SOUND_GATE_STRESS=1):
- Gate-stress probe PASS (normal + TSan builds): gated pump 120 ticks/500ms
  (240Hz exact); held bracket = +0 ticks over 200ms while vblank advances
  (pump thread alive); 1.82M main-thread bracket pairs vs 464 ticks with ZERO
  torn multi-word reads on either side; 632 func_8003AE84-style re-entrant
  brackets on the tick path, no self-deadlock; post-CloseEvent dispatch
  resumes (no leaked hold).
- TSan (probe suite + Map001 field boot): ZERO data races on sound shared
  state. Remaining instrumented-code reports are pre-existing PsyX infra
  races to review later: g_psxSysCounters vblank counter (PsyX_main.cpp:190
  vs :165), LIBETC.C:27 ResetCallback registration, PsyX_Shutdown teardown.
- Regression: pump/prim/init probes PASS; manager coherent; five-map
  watchdogs (0/1/14/47/334) boot to field main loop, field-diag tripwire
  lines addr-normalized EXACT vs prior baselines.
- Patch idempotent: reverse-apply clean, build re-applies, rerun no-op.
Honest state: GATING VALIDATED, tick body stubbed, no real tick body yet.
Next: tick-core decomp (12 fns, leaf-up, func_8003C020 last) under the gate.
Evidence: proven (gate-stress + TSan runs; five-map smokes; idempotency cycle)
Last verified @ HEAD of this commit

### Tick-core REAL (tick-leg step 2; dispatching at 240Hz into the proven gate)

The 12 tick-core control-layer functions are decompiled and live, leaf-up,
func_8003C020 last. Matching split: **6 objdiff {}** (func_8003EEA0/A838/
EB5C/E900/AE84/CC84 -- E900 via the volatile SPU register-pair cursor + merged
live-range idioms) and **6 coexistence** (d88f13c pattern: func_8003E5BC/C4C4/
EFE4/EBF0/C6E8/C020). The coexistence residual is ONE uniform class: this
pipeline's cc1 strength-reduces the element-cursor giv family onto a different
anchor register and fills load-latency slack the retail object leaves
unfilled -- same operations at the same absolute element offsets (verified
per-diff); INCLUDE_ASM keeps the matching build byte-exact. Matching build
proven byte-identical: built slus_006.64 hashes EQUAL with and without the
whole change set (A/B rebuild).

Port integration:
- g_pSoundSpuRegisters (retail .sdata -> 0x1F801C00) was a NULL auto-stub --
  safe only while the tick was stubbed. Now backed by a static SpuUnion page
  in sound.c (port-only); the real tick's register writes (key on/off, ADSR,
  pitch) land in real memory, faithfully maintained but not yet wired to the
  OpenAL backend (that is the B5/WDS leg).
- LANDMINE FOUND + FIXED (the SoundClearVoiceDataPointers class): FieldLoad's
  walkmesh LZSS decompress writes ~0x220 bytes into D_800658DC, a default
  32-byte data stub -- every field load silently trampled the neighbouring
  stubs including the sound heap D_80065B0C. Harmless while nothing read the
  heap; the real tick crashed on the corrupted manager list (caught by
  hardware watchpoint). Fixed by sizing D_800658DC to its retail 0x230 in
  symbol_addrs (stub generator honours size:).
- func_8003EFE4's per-voice envelope method pointer stays SoundPsxAddress
  (LP64-safe); its jalr is unreached until the envelope setters land (step 3
  runtime-trace set). g_SoundScriptHandlers dispatch is compiled but
  unreached in-port (no sequence data loaded -- elements stay inactive), so
  the handler table stays a stub until step 3 provides host routing.

Validation (dual regime):
- Matching: 12/12 snddiff EXACT (6 real, 6 via INCLUDE_ASM); whole-object
  instruction diff clean modulo reloc-vs-addend encodings that link
  identically; slus binary A/B hash-identical.
- Live tick: real func_8003C020 dispatching at 240Hz (120 ticks/500ms),
  manager coherent after ticks; pump/prim/init/gate-stress probes all PASS
  with the real body (1.81M bracket pairs vs 462 ticks, zero torn reads,
  630 re-entrant brackets).
- TSan (real bodies): probes + Map001 field boot -- ZERO races in sound.c;
  only the two catalogued pre-existing PsyX infra races (vblank counter,
  shutdown) remain.
- Five-map watchdogs (0/1/14/47/334) boot clean with the real tick running
  during field play; field-diag tripwire lines addr-normalized EXACT.
Honest state: tick core real, dispatching at 240Hz into the proven gate,
TSan-clean; script handlers + envelope setters + WDS still stubbed -- the
tick processes an empty queue, NOT YET AUDIBLE. Next: step 3 (51 script
handlers + host handler-table routing), then error/WDS + B5 for audible.
Evidence: proven (snddiff x12 + A/B binary hash; live-tick probes; TSan;
five-map tripwires; watchpoint root-cause on the stub overrun)
Last verified @ HEAD of this commit

### Handler layer batch 1 + host dispatch table (tick-leg step 3, partial)

The host g_SoundScriptHandlers table is LIVE: 128 entries in retail slot
order rebuilt as host-width function pointers (port-only; matching keeps the
.sdata original). Batch 1 of the 51 unported handlers landed: **12 objdiff {}
(oracle-confirmed fuzzy=100)** -- func_8003CE9C/CD54/DEB4/D370/D3A4/D884/
E4BC/D034/DAB0/DE18/DF3C/D17C -- and **8 coexistence** (d88f13c; scheduling
residual): func_8003CD08/D0E8/D110/DB2C/CE68/D7C8/D13C/D60C. **31 handlers
remain unported** (mid/large: D8B8, D9A4, DD24, DC50, DF78, E04C, ... --
scratchpad/handlers_sized.txt) for the next pass; their table slots dispatch
to stubs until then (unreached without those opcodes in a stream).

BUGS FOUND BY THIS PASS (the live-exercise payoff):
- func_8003C6E8 (fa91aeb coexistence body) had FOUR active_flag-vs-status
  transcription bugs (loop exit, post-loop gate, tie set/clear, gate word all
  read `lhu 0($s2)` = active_flag in retail; the C read status). Matching
  build was never affected (INCLUDE_ASM); the port's empty-queue tick never
  reached them. Fixed + exercised.
- 791f21b's D_800658DC size:0x230 swallowed D_80065ADC (a real symbol at
  +0x200 referenced by field misc4) and broke the matching FIELD link --
  latent because report-mode ninja has no link edges. Corrected: 0x200 +
  D_80065ADC sized 0x30.

Validation: synthetic sequence probe (XENO_SOUND_SEQ_PROBE) feeds a hand-
built command stream to the live 240Hz interpreter -- five opcodes route to
five distinct handlers with distinct observable effects; IP advances exactly
to stream end (routing + operand-length proof); fermata counts down (the
sequencer steps). SYNTHETIC data, labeled: full live exercise awaits WDS/B5.
Matching binary byte-identical (d004692f...) with all 20 bodies; make build
green (slus + field). TSan (probes + seq probe): zero sound races, only the
two catalogued PsyX infra races. Five-map watchdogs boot clean, tripwire
lines EXACT. Gotcha recorded: `make report` leaves build.ninja in report-only
mode (no link edges); run `make build` after to restore the matching
pipeline (gears matching vs gears report modes).

Honest state: dispatch table live and routing-proven via synthetic stream;
20/51 handlers real (12 {} oracle-confirmed + 8 coexistence); 31 handlers
remain; NOT YET AUDIBLE (WDS/B5).

### Handler layer batch 2 (step 3b, partial): 32/51 real

Group A of the remaining 31 landed: **9 objdiff {} oracle-confirmed
(fuzzy=100)** -- func_8003DB58/E40C/CFF0/CFA4/DB98/E308/D7FC/DEE4/E4F0 --
and **3 coexistence** (audited 1:1 + exercised; residual = load-reload
elision / register-copy / arg-setup scheduling): func_8003CEF0/D1BC/D4E4.
Oracle 1227/2292 (+9), code 36.05%, fuzzy 51.62%. Matching binary
byte-exact (d004692f); make build green; TSan clean (pre-existing PsyX
vblank only); five-map tripwires EXACT.

Synthetic stream extended to 10 opcodes: adds vib-accumulator nudge (0xE1),
channel fade (0xA7, interp70 counter/target verified), pitch-slide arm
(0xD4), and a pan FADE (0xEA) that runs to completion through C4C4's fade
path (retail sets the scaled delta on the final step -- expectation model
corrected, handlers verified asm-faithful). Routing + operand-length +
effects PASS; IP advances to stream end.

**19 handlers remain for 3c** (group B: CD8C, CF38, D070, D21C, D3D8, D438,
DBE4, D53C, DEE4-done, E1F8, E180, E360, E44C, E54C; group C large: DC50,
DF78, D8B8, DD24, E04C, D9A4 -- see scratchpad/handlers_3b.txt minus group
A). 0x99 (CF38, loop-continue) will enable a loop-stack test in the stream.
Honest state: 32/51 handlers real, oracle-confirmed, exercised via
synthetic stream; audible awaits WDS/B5.

### Handler layer batch 3 (step 3c, partial): 45/51 real

Group B (13 mid handlers): **3 objdiff {} oracle-confirmed (fuzzy=100)** --
func_8003CF38 (0x99 loop-continue), CD8C (0x90 dal-segno/track-end), E54C
(0xFF release-if-silent) -- and **10 coexistence** (audited 1:1 + residuals
all non-semantic micro-shapes: commutative operand order, delay-slot copy
placement, register reuse): func_8003D070/D21C/D3D8/D438/DBE4/D53C/E180/
E1F8/E360/E44C. Oracle 1230/2292, code 36.12%, fuzzy 51.69%. Binary
byte-exact; TSan clean; five maps EXACT.

FINDINGS:
- **Envelope method table RESOLVED**: func_8003E180 (0xF0) binds
  env->pfnHandler from D_800508A4 (sdata word table) -- the "runtime-trace"
  EFE4 jalr set is statically enumerable after all. The PORT needs host
  routing for that table (g_SoundScriptHandlers pattern) before envelopes
  can arm -- lands with the envelope/B5 pass. Until then streams must not
  arm envelopes (0xF0/0xF6).
- **Loop-stack live test deferred to 3d**: placing 0x98/0xA1/0x99/0x9A
  mid-stream parked the IP at the loop body -- C6E8's post-pass TIE-SCAN
  walks forward from the stored IP and interacts with loop constructs (and
  reads past stream end after a trailing rest -- synthetic streams need a
  scan-safe epilogue). Handlers are asm-faithful (objdiff/audit); this is
  stream-design + interpreter-model work, instrumented in 3d. Stream
  reverted to the 3b-proven form (PASS).

**6 handlers remain (group C large, 53-67 insns)**: func_8003DC50, DF78,
D8B8, DD24, E04C, D9A4 -- unhurried in 3d, plus the loop-test redo.
Honest state: 45/51 handlers real; not yet audible (WDS/B5).
Evidence: proven (oracle fuzzy=100 x3; probes PASS; A/B hash; TSan; 5/5
tripwires)
Last verified @ HEAD of this commit

### HANDLER LAYER COMPLETE (step 3d): 51/51 real

The final 6 group-C handlers landed as **coexistence** (audited 1:1;
residual = register-pressure frame shape, one extra callee-saved reg vs
retail; {}-upgrade candidates for a fresh matching session):
func_8003DC50/DF78/D8B8/DD24/E04C/D9A4 -- the envelope-priming family
(0xD8/0xD9/0xE4/0xE5/0xEC/0xED). They bind env->pfnHandler either directly
to func_8003F240/F2A0 (host fn addresses survive the u32 round-trip under
no-pie) or from D_800508A4 (retail addresses -- host routing deferred to
the envelope/B5 pass; streams must not arm table-bound envelopes).

**LOOP-TEST REDO PASSED**: scan-safe stream (loop-first + guard byte +
0xFD tempo restore after the in-loop 0xA1 zeroes the tempo product).
vol58=0x5B0000 proves the 0x98/0x99 loop ran exactly 2 live iterations --
CEF0 push + CF38 continue/pop LOOP-EXERCISED. Two stream-design findings
(probe-model class, handlers asm-faithful): (1) **0x9A is an early-exit-
INSIDE-loop construct, not a loop terminator** -- placing it after an
exhausted 0x99 pops an empty stack -> NULL ip (crash reproduced + root-
caused); (2) a drained rest makes the interpreter consume the scan guard
as a note (retail-faithful) -- rests must outlast the probe window.

Validation: oracle 1230/2292 (unchanged -- all 6 coexistence), binary
byte-exact (d004692f), make build green, TSan zero non-catalogued races,
probes PASS, five-map tripwires 5/5 EXACT.

**THE TICK ENGINE IS COMPLETE**: gate + tick core + all 51 sequence-command
handlers real and dispatching at 240Hz. Remaining before AUDIBLE: WDS/B5
(sample banks + SPU streaming) + the envelope-method host routing
(D_800508A4 + func_8003F240/F2A0 decomp) + {}-upgrades for the 19
coexistence bodies as desired.
Evidence: proven (loop-test live run; A/B hash; TSan; 5/5 tripwires)
Last verified @ HEAD of this commit

### B5 audible-leg scoping (trace + design, NO implementation)

Read-only trace of the last mile between the complete tick engine and sound
output. B5 is SMALLER than feared -- most of the chain is already real:

(A) WDS LOAD: SoundLoadWdsFile (62 insns, unported) + func_80039024 (72,
unported) = **2 oracle decomps**; every other callee is already-real C
(SoundQueueSpuWriteCommand, SoundSpuMemoryAllocateWDS, SoundHeapSetBlock/
ClearBlockMemory, gate brackets) or the stubbed error leg. Disc access is
REACHABLE: the caller (field misc8 func_80085F30, real C in the port) reads
the WDS via the WORKING archive path (ArchiveDataSync + heap buffer) --
sound reuses the field loader's disc infrastructure, no new I/O needed.

(B) SPU-RAM UPLOAD: **already wired end-to-end** -- SoundQueueSpuWriteCommand
-> SoundProcessTransferCommand -> SpuSetTransferStartAddr + SpuWrite (real in
PsyX LIBSPU.C, writes the backend SPU-RAM image) -> SoundOnTransferCallback
(real, drains the queue). Behavioral verification needed, no construction.

(C) PLAYBACK TRIGGER -- **THE one real gap**: the tick's voice-register
flush (func_8003E900/EB5C) writes key-on/off, pitch, volume, ADSR, start
address into the static SpuUnion backing page (state-faithful, unwired).
The backend is driven via SpuSetKey/SpuSetVoiceAttr (both real; 4x
alSourcePlay sites in PsyX_SPUAL). Wiring = a register->backend translator
(port-side: either #ifdef branches in E900/EB5C calling the Spu* API, or a
post-flush shim reading the backing page). ONE bounded host-wiring pass.

(D) SPU-IRQ -- **DEFINITIVE: NOT needed for audible**. SoundSpuIRQHandler
(real C) is a thin dispatcher to g_SoundSpuIrqCallbackFn -- clients are the
XA/CD STREAMING paths. WDS playback = one-shot SPU-RAM upload + key-on; the
upload uses the transfer-complete callback, not the address-IRQ. The IRQ
source defers AGAIN, to the streaming/XA leg (design it there).

(E) ENVELOPE ROUTING: func_8003F240 (24) + F2A0 (26) decomps + a 16-entry
host table for D_800508A4 (dispatch-table pattern). NOT audible-critical --
hardware ADSR shaping flows through the voice regs in (C); envelope objects
are modulation on top. Quality layer, defer past first-audible.

MINIMAL FIRST-AUDIBLE (B5.1, ~2 passes): decomp the 2 WDS fns (oracle) ->
behavioral-verify upload (SPU-RAM image gets sample bytes) -> wire the
register->backend translator -> drive a real WDS bank + note-on (field map
or synthetic stream with a real bank). B5.2 (~1 pass): envelope routing +
F240/F2A0. B5.3 (separate leg): XA/CD streaming + the SPU-IRQ source.

AUDIO-OUTPUT VERIFICATION REGIME (proving audible, not "ran"): (1) backend
state probe -- AL_SOURCE_STATE==AL_PLAYING + buffers queued (PsyX accessor,
alGetSourcei already used internally); (2) captured-output proof:
ALSOFT_DRIVERS=wave renders to a WAV, assert non-silence (RMS threshold) --
headless/CI-grade evidence; (3) the human ear for the milestone.

No showstoppers: disc access reachable, backend functional, IRQ defers.
Recommendation: fund B5.1 (first audible) next -- 2 decomps + 1 wiring pass.
Evidence: proven (subtree trace over split asm + C-body; transfer/IRQ roles
read from real C; PsyX_SPUAL API confirmed)
Last verified @ a7390b0

### B5.1 pass 1 LANDED: WDS load real, samples verified in SPU-RAM

SoundLoadWdsFile + func_80039024 decomped to **objdiff {} (oracle
fuzzy=100)**; SoundSpuMemoryAllocateWDS's void->u32 signature restored
retail's v0-passthrough (bare `return;` in a u32 fn -- codegen-identical,
still {}). Oracle 1232/2292, code 36.21%, fuzzy 51.78%. Binary byte-exact.
func_80039024 retail quirk (faithful): the heap-full failure path returns
WITHOUT re-enabling the tick event (bracket leaks; unhit in practice).

BEHAVIORAL PROOF (XENO_SOUND_WDS_PROBE): the REAL WDS bank (archive dir
0x1C file 3, 155,120 bytes) reads through the working archive path, parses
(dataOff 0x100, dataSize 0x25CF0), allocates SPU addr 0x12000, links into
g_SoundWdsLinkedList, transfer queue drains, and **SpuRead-back of the
backend SPU-RAM image MATCHES the source** at the first non-silent window
(probe-model fix: ADPCM banks open with silent blocks -- verify at the
first nonzero 16-byte window). Samples are IN SPU-RAM. NOT AUDIBLE -- pass
2 wires the register->backend key-on translator.

Validation: all 5 probes PASS (pump/init/gate/seq/wds); tripwire maps
3/3 EXACT; make build green. TSan: runs blocked by the recurring
PipeWire/OpenAL boot flake (documented since tick-core); the partial log
shows only pre-existing races (LIBETC ResetCallback); full TSan re-verify
deferred to a stable-audio session -- the pass's delta runs entirely under
the proven gate brackets.
Evidence: proven (oracle fuzzy=100 x3; SPU-RAM readback match; A/B hash)
Last verified @ HEAD of this commit
Evidence: proven (oracle fuzzy=100 x9; extended seq-probe incl. fade
convergence; A/B binary hash; TSan; five-map tripwires)
Last verified @ HEAD of this commit
Evidence: proven (oracle fuzzy=100 x12; seq-probe routing; A/B binary hash;
TSan; five-map tripwires)
Last verified @ HEAD of this commit

### B5.1 pass 2 LANDED: FIRST AUDIBLE -- register->backend translator, three-tier proof

The port makes sound. `PcPort_SpuRegFlushTick` (port_main.c) translates the
SpuUnion backing page into PsyX backend calls: registered as a second
counter-2 OpenEvent slot after SoundInitialize, it runs at 240Hz right after
func_8003C020 under the same g_SoundTickMutex bracket. Per pending KON bit it
builds a SpuVoiceAttr {VOLL|VOLR|PITCH|WDSA} from the voice regs (addr =
reg<<3), calls SpuSetVoiceAttr + SpuSetKey (-> alSourcePlay), then clears the
page's KON/KOFF words (write-trigger semantics). No envelopes (B5.2), no
streaming/IRQ (B5.3).

PLAY PROBE (XENO_SOUND_PLAY_PROBE, needs XENO_SOUND_WDS_PROBE): arms element
0 with a synthetic stream -- 0xFC bank+instrument 0, 0xE0 volume (el+0x78
accumulator; 0xA0 is NOT volume), note 0x30, long rest. Probe-model findings
that were engine truths, not bugs: (1) key-on requires arming from the REST
state (active=0x401) -- status bit 0x1 is only set on a rest->note edge;
(2) the voll chain is elVol(el+0x78 hi16) x expression(el+0x76) x manager
level(interp70 hi16) x pan law -- el+0x76 and interp70 are armed by the
UNPORTED song-start path, so the probe stands in for it. Result: real WDS
instrument 0 at SPU 0x12000, pitch 0x1530 from the extracted sdata tables,
voll/volr 12603/8571 through the full retail chain.

THREE-TIER AUDIBLE PROOF: (1) AL source state: SpuGetKeyStatus polled during
the window -> AL_PLAYING observed. (2) Wave-capture RMS: ALSOFT wave backend
(drivers=wave) -> 39.6s float32 capture, max 100ms-window RMS 0.0726
(FS=1.0), peak 0.31; CONTROL run (WDS load, no play probe) is digitally
silent (RMS 0.000000, peak 0.0000) -- the energy is the note. (3) Ear:
capture saved at scratchpad/first_audible_capture.wav (untracked) -- listen.

TSAN RAN CLEAN this pass (flake absent under wave backend) and CAUGHT A REAL
BUG: SoundSpuMemoryAllocateWDS's retail v0-passthrough (bare `return;` in a
u32 fn) is UB on host; native x86 happened to keep the allocator's result in
the return register, TSan's exit instrumentation clobbered it (spuAddr came
back garbage). Fixed via the d88f13c coexistence split: port body has
explicit returns, matching build keeps the retail shape -- binary still
byte-exact (d004692f...). 0/38 TSan reports touch the sound path (rest are
pre-existing Mesa/gallium/SDL boot noise). WDS probe also gained a
drain-poll + bounds guard (fixed-order readback raced the pump under TSan's
~15x slowdown).

Sound sdata tables (D_80050B78/BF0/A94/9B0/824) extracted from
3F290.sdata.s into guarded port-side C (auto-stubs were zeros -> pitch 0).
Validation: all 6 probes PASS (pump/prim/init/seq/wds/play); tripwire maps
3/3 clean (env-noise-only diffs); make build byte-exact. Known limits: notes
are envelope-less (raw ADSR unsupported by PsyX backend; hard-stop on
key-off), one synthetic note != real music, song-start arming still probe-side.
Evidence: proven (three-tier audible incl. silent control; TSan clean run;
A/B binary hash; six probes; three tripwire maps)
Last verified @ HEAD of this commit

### Song-start path SCOPED (read-only trace; the map the B5.1 probe stands in for)

TWO start paths, one driver. Sequenced sound starts EITHER as an SFX pair or
as a music manager; both feed the already-ported tick/handler/translator
engine (the tick's func_8003C020 port body already walks
g_SoundAudioManagerListHead -- multi-manager is free).

SFX pairs (menu/field cues): game APIs func_80039DB8(packed sedId<<16|entry)
/ func_80039F9C(packed, slot, vol, pan) [+E18/EC4 (ported C)/E60/F18] ->
**func_8003B644** (200 insns, the ONLY 0x409 element-armer): find SED in
g_SoundSedsLinkedList, bind WDS bank, arm TWO elements (each SED entry = 2
script-offset halfwords at +0x20, one script per channel), set el+0x14 IP,
active=0x409/0x40B, el+0x78=0x7F<<24, **el+0x76=(reqVol x entry volume
byte at sed[+0x18 table])>>7**, el+0x74=pan, mgr+0x10|=0x8000 -- everything
the B5.1 play probe hand-armed. Handler func_8003CFF0 calls func_80039F18
(script-spawned starts; currently a silent stub under real sequences).

Music managers (field BGM): retail field func_80085C90 (per-frame poller,
port SHIMMED; retail asm in matchings/) buffer-reads the SONG FILE --
ArchiveSetIndex(0x1C,0) + ArchiveReadFileToBuffer(songId*2+0x14, D_80062648,
0, 0x80), the SAME working path the WDS probe used -- then
**func_80039850**(file) create-manager (elementCount=file[0x14], binds file
at mgr+8; B0AC extra block if file[0x15]) -> **func_8003B22C** (copy header
0x10-0x1D: wdsId+0x16, tempo bytes, reverb depths -> mgr,
SoundSetReverbModeWithAllocation + ported SoundInitializeAudioManager) ->
**func_8003B424** (136 insns; music twin of B644's loop: per-element script
IPs from file payload, 0x7F<<24, SoundFindWdsEntry bank bind, E5BC prime,
AssignVoiceAndStop) -> **func_8003A89C**(mgr, level, fade) = the
manager-level (interp70) setter/fader = FE 0E's target (func_8008C84C,
ported, mgr ptr D_80062528). Variants: func_80039910 rebind-same-manager,
func_80039A80 restart, func_80039B68 WDS-rebind+level resume, func_800399D4
free, func_8003AA30 resume-from-mute. interp70 DEFAULT 0x7F<<24 is already
ported (SoundInitializeAudioManager line ~1895) -- the probe's zeros were
el+0x76/el+0x78, both B644-armed.

Data: song files = dir 0x1C file songId*2+0x14 (buffer-read, reachable NOW);
music WDS banks = dir 0x1C file D_800ADFCC[musicIdx*2]*2+0x13 (retail
STREAMS via func_80085560; port's ArchiveReadFile bails on CdlModeStream --
substitute buffered read, host-wiring); common SED = dir 4 file 0xA8
(SoundAddSedsEntry, ported). Field boot already routes: main.c
func_80085B20(map's D_800B2290) at map load; cue opcodes at misc.c
1552-1592 -> func_80085634/func_800855C8 (855C8 = no-op shim).

Port traps identified: D_80062648 (song buffer, unmapped BSS at 0x80062648,
~12.9KB gap to D_800658DC) will auto-stub tiny -- needs size: annotation
(the D_800658DC LZSS lesson, applied proactively). func_80039F18 stub is
callable from ported handler CFF0. No showstoppers found.

Phased price (decomp insns via split asm, all oracle-checkable):
M1 music-side decomp: 10 fns ~574 insns (39850/39910/399D4/39A80/39B68/
B0AC/B22C/B424/A89C/AA30) -- one tick-core-sized pass. M2 first REAL
sequence audible: host probe ~80 lines (buffer-read song+bank, 39850+39A80+
A89C(0x7F)), three-tier audio proof -- rides M1. S1 SFX chain: 11 fns ~606
insns (B644/A65C/F9C/DB8/E60/F18/A20C/A344/A55C/FF8/A094) -- menu+field
cues, un-stubs CFF0 spawns. M3 field un-shim: real func_80085C90 body +
func_800855C8/85678 + CdlModeStream buffered substitute + D_80062648
sizing -- in-game music in MAP000 smoke. B5.2 envelopes (as scoped): host
D_800508A4 routing + F240/F2A0 -- note shaping quality.
Recommended order: M1 -> M2 (proof-of-life) -> S1 -> M3 -> B5.2.
Evidence: traced (asm-level, both paths end-to-end; no implementation)
Last verified @ 4dbf1a0

### Song-start M1 LANDED: music arming layer decomped, 9/10 {} + 1 audited coexistence

The 10 music-side functions from the scoping map are real. **{} (oracle
fuzzy=100 x9)**: func_80039850 (create manager from song file; merged
live-range idiom -- the parameter register is reused for the manager),
func_80039910 (rebind + 0x4000 not-heap-owned), func_800399D4 (destroy;
0x4000-guarded free), func_80039A80 (restart: re-init + re-arm + level under
the bracket), func_80039B68 (WDS-rebind resume; single-cursor loop anchored
on voice_data.flags -- the flags store sits LAST so the induction family
anchors there), func_8003A89C (manager level/fade = interp70; FE 0E's
target), func_8003AA30 (resume: reverb reapply + all-dirty + running),
func_8003B0AC (5-byte override records -> manager tail block),
func_8003B22C (song header -> manager + reverb + common init).
**Coexistence (audited)**: func_8003B424 (the element-arm loop) -- the same
cc1 anchor-rebase residual as C6E8/EBF0 (retail keeps the field cluster on
the voice_data cursor; this cc1 rebases the family onto last-use/derived
anchors, register-permuting the loop; 12 variants tried). Field-by-field
offset map annotated inline; retail-faithful quirk kept: voice numbers lag
the element index (element 0 = conductor, voice 0xFF, bounds-checked away).

New matching idioms recorded: dbr fills a branch delay slot with a preceding
independent copy (place `pF = pFile` BEFORE the error branch); the
alias-variable (manager=(cast)pFile) blocks that placement; st[k]/named-field
choice decides induction-family anchoring (byte-casts off a typed cursor
split families).

D_80062648 (field song buffer) size-annotated 0x3200 (0x62648..0x65848,
flush to the next referenced symbol) BEFORE anything reads into it -- the
D_800658DC lesson applied proactively. Validated under make build.

TRIPWIRE CATCH (MAP014 segfault, fixed): Map014's field script issues FE 0E;
with func_8003A89C now real, the func_80085C90 shim's "song loaded" report
fed it D_80062528 == NULL (retail writes low kernel RAM silently -- no null
guard in the asm). Port-boundary guards (XENO_PC_PORT-only, documented,
remove at M3) added to func_8003A89C and func_800399D4 (misc4.c reaches it
the same way). Matching binary unaffected.

Validation: oracle 1232 -> 1241/2292 (+9 = the {} count), fuzzy 52.10%;
slus + field.bin byte-exact (d004692f... / c4200fdb...); port + TSan builds
green; all 6 probes PASS at 240Hz; TSan 0/38 reports touch sound.c or the
M1 functions (rest = pre-existing gallium/SDL/PsyX families); five-map
tripwires clean (MAP000/001/014 exact; 047/334 diffs are CD-path cosmetics
from the baseline's capture directory).

HONEST STATE: the arming layer is decomped and byte-verified, but NOT yet
exercised against real data -- M2 (next) probe-drives a real sequence
through func_80039850 + func_80039A80 + func_8003A89C and proves audibility
via the three-tier regime. No music-audible claim here.
Evidence: proven (oracle fuzzy=100 x9; coexistence audit; A/B binary hash;
six probes; five tripwire maps; TSan build)
Last verified @ HEAD of this commit

### Song-start M2 LANDED: A REAL XENOGEARS SEQUENCE PLAYS

First real music through the whole stack: archive dir 0x1C pair (WDS bank
file 0x13, 'wds ' magic, id 0x21, self-addressed to SPU 0x38000 -- coexists
with the common bank at 0x12000; song file 0x14, 'smds' magic, 21 elements,
wdsId 0x21 == the bank id) buffer-read via the proven path and driven
through the M1 chain exactly as retail field code does: func_80039850
(create) -> func_80039A80 (start, level 0x7F). The ported tick interprets
the real sequence at 240Hz; the translator keys voices.

Scan first (XENO_SOUND_SONG_SCAN): dir 0x1C = common bank (3) + repeated
(bank, song) pairs from 0x13 -- songs are the 'smds' files at even indices
(elemCnt at +0x14, wdsId at +0x16), banks 'wds ' at the odd ones; several
slots duplicate the same pair (CD streaming locality).

THREE-TIER PROOF (XENO_SOUND_SONG_PROBE): (1) AL: per-voice
SpuGetKeyStatus sampling -- 6-8 voices PLAYING in 16/16 half-second
samples, 22 key-ons / 38 key-offs over 8s of playback (polyphonic,
sustained, evolving = a SEQUENCE, not a note). (2) Wave-capture RMS
profile: 8 consecutive non-silent seconds (1s-window RMS 0.048-0.065,
VARYING), silence lands exactly at the probe's teardown; CONTROL run
(manager created, never started) is digitally silent (RMS 0.0000, peak
0.0000, keyons=0). (3) Ear: scratchpad/first_real_sequence_capture.wav
(untracked) -- which theme file 0x14 is (map->musicIdx mapping lives in
field data D_800ADFCC) is verifiable by listening.

Boundaries measured clean: func_80039F18 (the flagged CFF0/S1 trap) NOT
hit; M1 NULL guards never fired (real manager end-to-end -- create ->
start -> stop -> destroy, teardown verified by post-stop silence);
playback-caused stubs are EXACTLY the three envelope helpers
(func_8003E290/E3E0/F2A0 = B5.2), isolated by play-vs-control stub diff
(the SFX-chain stubs seen in both runs are boot-path, S1).

TSAN CAUGHT ANOTHER PASSTHROUGH BUG (pre-M1 code, first real exercise):
func_80039C4C called SoundReleaseAllVoices() with NO argument -- retail
rides the manager in $a0 (the asm has no move before the jal); native x86
reproduced the luck, TSan clobbered it (SEGV in teardown). Fixed with the
explicit argument -- gcc emits zero extra instructions ($a0 already holds
it): func_80039C4C stays {} and the binary stays byte-exact. The
passthrough family now has two members (v0-return + a0-argument); grep
argless calls of parameterized functions when TSan crashes what natively
works.

Validation: slus byte-exact (d004692f...); all 7 probes PASS (incl. the
new song probe) at 240Hz; TSan rerun clean -- PASS with identical playback
stats, 0/37 reports touch the song path; five-map tripwires clean.

HONEST STATE: a real sequence plays PROBE-DRIVEN and envelope-less (notes
key at full computed volume, no ADSR shaping -- B5.2), with sequence-set
tempo/volume via the 51 real handlers. In-game triggering (field shims,
CdlModeStream substitute) is M3; SFX chain is S1.
Evidence: proven (three-tier incl. silent control + varying-RMS profile;
stub-set isolation; TSan clean rerun; A/B binary hash; five tripwires)
Last verified @ HEAD of this commit

### Song-start M3 LANDED: IN-GAME MUSIC -- maps play their themes on boot, no probe

The field music trigger is real end-to-end. func_80085C90 (per-frame music
poller, called from func_80078B5C while D_8004F308==-1) is un-shimmed: full
retail structure from the matchings asm -- bank-swap completion leg,
common-bank lazy-load leg (func_80085FB8 kick + func_80085F30 complete, dir
0x1C file 3), song-file read (songId*2+0x14 -> D_80062648, the M1-sized
buffer), then the M1 chain (func_80039850 create -> func_80039A80 start
0x7F, plus the muted-start+FE-0E-fade and func_80039B68 bank-rebind resume
branches, D_8004F340/348 state machine as retail).

STREAM SUBSTITUTION (documented, XENO_PC_PORT block in C90's swap leg):
retail streams the per-map music bank in 8-sector CD windows chunk-pumped to
SPU by func_800859DC (func_800380D0 + SoundTransferWdsPart, unported;
func_80028B14 chunk-poll is a stub) -- never resident in main RAM. NOT
substitutable in archive_port.c as scoped (the stream buffer is an 8-sector
window; the chunk consumer is unported); and NOT substitutable by a
HeapAlloc'd whole-file read either -- the field heap cannot fit ~190KB
mid-map-load (caught live: MAP001 requests music 0 -> HeapAlloc(195040)
fails -> GameHandleError(130) spin). The port stages the file in HOST malloc
memory and lands it via SoundLoadWdsFileHostStaged (new port-only sibling of
SoundLoadWdsFile in sound.c): identical SPU-alloc/header-copy/list-append
flow, but the payload goes through synchronous SpuWrite -- the transfer
queue narrows source pointers to PSX addresses, which host memory doesn't
have. Same never-in-PSX-RAM property as retail's stream.

M1 NULL GUARDS REMOVED (the tracked exit criterion): func_8003A89C and
func_800399D4 are guard-free. The real C90 raises D_8004F36C (FE 0E's gate)
only after the manager exists, and misc4's teardown pair is gated on
D_8004F304 which nothing ported sets yet -- NULL exposure closed
structurally; MAP014 (the original guard trigger) now plays music through
FE 0E on a real manager.

Field-test harness stand-ins (XENO_PC_PORT + XENO_FIELD_TEST, misc3 init):
(1) direct map entry skips the exit-transition flow that requests music --
the harness runs FieldMain's own request body (func_8001B66C, D_8004F308=-1,
D_8004F324=D_800B2290, func_80085B20) with the field default id 0x1D;
(2) retail boots D_8004F364=1 ("common bank resident", loaded by the
unported new-game flow) -- the harness clears it so C90's retail leg
lazy-loads the bank (the lie surfaced as a host SEGV in func_8003E5BC when
the sequence's instrument-change ran bankless; retail reads PSX low RAM
harmlessly there).

TSAN CAUGHT A REAL RACE, FIXED AT THE ROOT: SoundQueueTransferCommand's
queue-index/flag writes (main thread, via the now-live func_80085F30 ->
SoundLoadWdsFile) raced the tick's g_SoundControlFlags reads. Retail
protects the section with EnterCriticalSection (interrupt mask); the port's
Enter/ExitCriticalSection were no-ops. Now mapped onto the tick gate
(PsyX_Sys_SoundGateEnterCritical/Exit in LIBAPI.C, recursive hold like a
DisableEvent bracket; psycross_sound_gate.patch regenerated, reverse-check
verified). TSan rerun: 0/38 reports touch the music path.

IN-GAME PROOF (existing smoke launchers, NO probe): MAP000 -- 4s boot
silence (pre-trigger control, RMS 0.0000) then 55/59 non-silent seconds
(1s RMS 0.035-0.074 varying, peak 0.41); key-ons across voices 9-18 with
per-voice stereo volumes, distinct pitches, six instrument addresses; ZERO
sound-path stubs (no func_80039F18, no envelope stubs -- complete playback).
MAP001 (music 0 + bank 0x13 swap): music from t~4s, 85/89 non-silent.
MAP014 (FE 0E): music from t~17s, 72/89. WAVs: scratchpad/
map000_ingame_music.wav (music id 0x1D = song file 0x4E -- identify by ear).

Validation: slus byte-exact (d004692f...); all 7 probes PASS at 240Hz;
five-map tripwires ALL EXACT on the final build (line-buffered captures;
the old runner's "LOG CLOSED" trailer filtered). Known nit: a map with music
running may ignore SIGTERM (MAP334 exits via SIGKILL under timeout -k;
game output exact) -- teardown signal handling, not a field regression.
Remaining boundaries: menu/normal-boot music start (needs the new-game flow
or a KernelMenu-path stand-in), SFX cues (S1), envelopes (B5.2), real CD
streaming (would retire the host-staged substitute).
Evidence: proven (in-game three-tier on three maps incl. pre-trigger
control; TSan root-fix + clean rerun; A/B binary hash; five exact tripwires)
Last verified @ HEAD of this commit

### B5.2 LANDED: envelope shaping -- vibrato/tremolo modulation live, comparative-proven

The envelope layer is real: 10 new {} (oracle fuzzy=100; 1241 -> 1251/2292)
-- func_8003E290 (target packing; switch/expand_case shape), func_8003E3E0
(arm/reset), and the full method family func_8003F1A4/F1EC/F240/F2A0/F308/
F354/F3C0 (gate, alternating set, triangle, ping-pong, sawtooth, random
level, random bipolar) + func_8003F43C (xorshift RNG). F2A0 matched via the
merged counter/reload live range + unmasked decrement + flags-var-reuse
idioms. D_800508A4 (16-entry method table) is wired port-side: runtime-
filled at SoundInitialize with host addresses narrowed through the no-pie
u32 round-trip (a truncating cast is not a valid static initializer);
func_8003EFE4's already-ported dispatch consumes them. The translator
gained a continuous change-detected voll/volr/pitch flush (envelopes
modulate the register page every tick; key-ons alone are edge-triggered).
Bonus fix: func_8003E1F8's coexistence body dropped E290's rate argument
(latent while E290 was a stub).

COMPARATIVE PROOF (same song, M3-baseline binary from a worktree vs B5.2):
(1) VIBRATO, register domain -- song 0x14 arms pitch envelopes (state 0,
0xD8/ping-pong): baseline pitch-mod accumulator flat 0 for 7646 ticks;
shaped run has 133 modulated ticks swinging +/-14 with the SPU pitch
register tracking (0x1071..0x108C around 0x1080). (2) TREMOLO, audio
domain -- song 0x1C arms volume envelopes (state 1, alternating): 567/892
aligned 50ms windows differ >5% with oscillating deltas (+2%..+120%),
identical note timing (18 keyons both) -- the modulation is the only
difference. (3) THE TABLE IS LOAD-BEARING -- song 0x20 (method-6 random
envelope via the table) SEGVs the baseline at pfnHandler=0 inside
func_8003EFE4's dispatch (exact bt) and plays fully on B5.2. Playback
stubs are now ZERO (E290/E3E0/F2A0 were the last). Pan-envelope audio
isolation (song 0x20, state 2) was inconclusive in the full mix -- not
claimed; the dispatch/accumulator path is shared with the proven cases.
IN-GAME: MAP001's music is song 0x14 -> its vibrato now shapes in-game
(85/89 nonsilent seconds on the smoke boot); MAP000's song arms no
envelopes (engine truth, boot unchanged/exact).

M3 CORRECTION -- the critical-section fix was DEAD CODE as shipped:
PsyCross compiles LIBAPI.C as C++, so the gate functions got mangled
linkage and the C callers bound to the auto-stub no-ops (nm shows the
_Z31... twin). Fixed by declaring them in PsyX_main.h's extern-C block
(patch regenerated, reverse-check verified). The blanket
EnterCriticalSection->gate mapping then proved TOO WIDE under TSan
(couples every subsystem's critical sections to the audio lock; TSan
MAP000 regressed) -- narrowed to the actual load-bearing site: the SPU
transfer-queue bracket in SoundQueueTransferCommand takes the gate via
port-only calls; global Enter/ExitCriticalSection stay no-ops. TSan
MAP000 back to M3-parity (RUN=124, full boot, 0 sound-path reports).
Related robustness: the C90 bank staging now records the bank file at
stream START (func_80085B20) instead of deriving from the CURRENT music
index at completion (wrong file if the request changes mid-stream), with
size guard + sector-rounded staging alloc.

KNOWN ISSUE (TSan-only, filed): MAP001 under TSan crashes late in the C90
bank-staging read (memcpy with a garbage-huge count in ArchiveReadFile's
bounce path; the same staging passes on TSan MAP000 and everywhere
native). Suspected archive-global/TSan interaction in the M3-era leg, NOT
a data race (0 sound-path race reports across every B5.2 TSan run) and
NOT reproducible natively (all five tripwires exact). Root-cause deferred.

Validation: slus byte-exact (d004692f...); all 7 probes PASS at 240Hz
(native + song probe under TSan); five-map tripwires ALL EXACT; oracle
1251/2292 (fuzzy 52.34%).

HONEST STATE: notes are envelope-MODULATED (vibrato/tremolo/pan program
from the songs' own scripts). Hardware ADSR attack/release per-note remains
approximated (PsyX has no raw ADSR; key-off is a hard stop) -- the
envelope OBJECTS are done, ADSR emulation is a backend follow-up. SFX (S1),
menu/boot music, CD streaming remain.
Evidence: proven (register + audio comparative A/B against an M3 worktree
binary; negative-space table proof; oracle fuzzy=100 x10; five tripwires)
Last verified @ HEAD of this commit

### S1 LANDED: the SFX chain -- sound effects play through the real pipeline

The SFX twin of the music path is real: 12 functions. **{} (oracle
fuzzy=100 x4; 1251 -> 1255/2292)**: func_80039DB8 (fixed-top-slot API),
func_80039E60 (allocated-slot API), func_80039F18 (the CFF0-spawn/API entry
-- the flagged stub, now REAL), func_80039F9C (field-cue API).
**Coexistence (audited, d88f13c pattern)**: func_8003B644 (the SFX
element-arm core -- the EXPECTED B424-twin cc1 anchor-rebase residual;
every store audited against the split asm offsets), func_8003A65C (slot
allocator: dedupe + downward window scan + oldest-steal; regalloc
permutation residual), func_80039FF8 (stop-all; load-scheduling),
func_8003A094/func_8003A14C (stop-by-SED/stop-by-id; cursor-role
permutation), func_8003A20C/A344/A55C (pair stop/volume/pan; a
two-instruction scheduler placement of the slot masking).

PASSTHROUGH-UB FAMILY, 4th MEMBER: func_8003A65C's steal-scan leaves its
`best` slot variable UNINITIALIZED when no candidate has priority <= 0x20
(retail reads stale $s6 -- PSX wraps harmlessly; the host would index
wild). Port-guarded default (scan start) under XENO_PC_PORT; matching
keeps the retail shape. The linkage trap did not apply (no PsyCross-side
additions this pass).

SFX AUDIO PROVEN (XENO_SOUND_SFX_PROBE): the probe loads the field's
common SED (dir 4 file 0xA8, real SoundAddSedsEntry -- id 0, 468 entries)
plus the common WDS bank, then fires entry 1 through the REAL chain
(func_80039F18 -> A65C slot alloc -> B644 pair-arm -> tick interprets the
SED scripts). Three tiers: (1) AL -- 1 key-on event, 2 voices PLAYING
(the SED pair), 3/20 100ms samples (a ~350ms one-shot); (2) wave-capture
-- a 0.35s attack/decay burst (RMS 0.039->0.078->decay, peak 0.38) with
the CONTROL (SED loaded, nothing fired) digitally silent through that
window; (3) WAV: scratchpad/first_sfx_capture.wav. Probe-model catches en
route: effect scripts bind WDS banks (opcode 0xFC), so the common bank
must be resident (the probe loads it; the field boot's FB8/F30 leg does
in-game) -- bankless was a NULL-instrument crash, engine truth. And a
PROBE-ARTIFACT fix: the B5.1 play probe's teardown zeroed the manager
TEMPO (mgr+0x54), freezing the sequencer for any later sound work in the
same run -- it now restores the saved value (caught because the combined
suite failed only after the play probe).

CFF0-spawn: no measured song uses the CFF0 opcode (M2/M3 test songs
don't), so the in-sequence spawn path is exercised via the probe's direct
func_80039F18 call -- the identical entry the handler invokes.

Validation: slus byte-exact (d004692f...); ALL 8 probes PASS in a single
combined run at 240Hz; TSan SFX run PASS with identical stats, 0/34
reports touch S1 code (the run's exit is the filed TSan-only late
normal-boot staging crash, post-probe); five-map tripwires ALL EXACT.

HONEST STATE: sound effects play, probe-proven, on the same pipeline as
the shaped in-game music. Remaining: in-game SFX triggering (the
func_800855C8/85634 field-cue un-shim -- the M3-equivalent for effects),
menu/boot music (new-game flow), hardware-ADSR per-note polish, CD
streaming, the coexistence {}-upgrade backlog.
Evidence: proven (three-tier incl. window-silent control; TSan clean on
the SFX path; A/B binary hash; five exact tripwires; oracle fuzzy=100 x4)
Last verified @ HEAD of this commit

### IN-GAME SFX LANDED: field cues fire real effects -- M3's twin for the effects domain

The field SFX cue path is un-shimmed. func_800855C8 is the real retail body
(stop the channel pair via func_8003A20C, fire through func_80039F9C -> the
S1 chain); func_80085634 now passes its retail FOURTH argument (the channel,
a1 & 7 -- visible in the matchings asm's $a3 flow, dropped in the old
transcription and latent while the shim no-op'd everything). Both compile
into the field build; no slus changes (oracle unchanged at 1255/2292, slus
byte-exact d004692f...).

REAL IN-GAME CUES FOUND: MAP001 (Lahan) fires villager/scripted cues
(common-SED entries 0x6/0x7/0x37/0x86) -- but actor-AI-nondeterministically
(observed 12 cues in one run, zero in six others; gdb instrumentation
overhead also perturbs script pacing). MAP014 fires a DETERMINISTIC
boot-script cue: id 0x36 at scripted volume 32, chan 3 -- reproduced in
every run. MAP334 fires one. The cue path: field script -> misc.c cue
opcode -> 855C8 -> A20C+F9C -> B644 pair-arm (elements 14/15 -> voices
22/23, unclaimed by anything else) -> tick -> translator.

RESIDENT-PATH FIX (the no-guards rule): firing MAP014's cue crashed at the
effect script's bank bind (func_8003E44C -> E5BC, NULL WDS list) -- the cue
fires BEFORE the per-frame C90 leg lazy-loads the common bank; retail never
had this window (its new-game flow preloads the bank, the D_8004F364=1 boot
state). The harness stand-in now loads the common bank EAGERLY at field
init (func_80085FB8 kick + func_80085F30 completion, bounded retry) --
restoring retail's residency invariant instead of guarding.

IN-GAME PROOF (MAP014 boot, no probe): (1) register/AL tier -- one cue ->
KONs on exactly v22+v23 (the pair) at tick-t 4.3s, both from the common
bank (addr 0x12000), distinct per-channel pitches 0xC7/0x128; (2) audio
tier -- the capture's first sound starts at 4.25s: a sustained quiet
ambient (~0.014 RMS, scripted vol 32) filling the 13-second pre-music
window where the M3-era control capture is DIGITALLY SILENT (0.0000 until
its music at 17s; both captures then converge on the identical music
profile); (3) WAV: scratchpad/map014_ingame_sfx.wav. Diagnostics added:
XENO_SOUND_KON_TRACE (translator key-on trace with tick timestamps) and an
XENO_FIELD_DIAG cue print.

The AMBIENT-SFX chain was scoped but stays gated: func_80085788 (per-map
ambient SED loader, dir 0x1C file ambientId+0x115 -> SoundAddSedsEntry +
the D_800AE060 schedule) and func_80085678 (per-frame scheduled-cue pump ->
func_80039EC4) are decompilable, but their CALLERS (func_800A7C58 map-load,
func_800A732C per-frame) are unported field-load machinery -- ambient
scheduled SFX land when that ports (or with a harness pump stand-in).

Validation: slus byte-exact; all 8 probes PASS; TSan MAP014 full boot with
the cue path, 0/35 reports in cue/SFX code; five-map tripwires ALL EXACT
(the new diagnostics are env-gated, default-off).

HONEST STATE: sound effects fire IN-GAME from real field-script cues
(MAP014 deterministic, MAP001/334 observed). Remaining: ambient scheduled
SFX (caller porting), menu/boot music, hardware-ADSR polish, CD streaming,
the coexistence {}-upgrade backlog.
Evidence: proven (in-game three-tier vs the M3-era silent-window control;
deterministic cue; TSan clean; five exact tripwires; A/B binary hash)
Last verified @ HEAD of this commit

## Audio fidelity: hardware-ADSR scoping map (backend pass, priced, no implementation)

SCOPING PASS (read-only; the audio-fidelity arc's first leg). The user-audible
gap -- "plays right but sounds like a .wav, not a PS1" -- decomposes with
hardware ADSR as the biggest lever: today a note keys on at its computed
volume with NO attack ramp, and key-off is alSourceStop -- a hard cut with NO
release tail. Every note is a rectangle. The SPU's per-note envelope
(attack/decay/sustain/release) is what this pass scoped.

(A) GROUND TRUTH -- the SPU ADSR algorithm (psx-spx, "SPU Volume and ADSR
Generator", https://psx-spx.consoledev.net/soundprocessingunitspu/; the
chapter is hardware-test-derived). ADSR1 (voice reg +0x8): bit15 attack mode
(0=linear/1=exp), 14-10 attack shift, 9-8 attack step, 7-4 decay shift, 3-0
sustain level. ADSR2 (+0xA): bit15 sustain mode, 14 sustain direction, 12-8
sustain shift, 7-6 sustain step, 5 release mode, 4-0 release shift. Envelope
level 0..0x7FFF advanced by a 44.1kHz counter machine: AdsrStep =
(7-step) << max(0,11-shift) (negated for decrease), CounterIncrement =
0x8000 >> max(0,shift-11), a step applies when the counter carries bit15.
Exponential increase is fake (step/4 above level 0x6000, shift-dependent);
exponential decrease scales the step by level/0x8000. Decay is always
exp-decrease, ends at (SL+1)*0x800; attack always increase, ends at 0x7FFF;
release always decrease, ends at 0. KON resets the level to ZERO and starts
attack; KOFF switches to release from any phase. ENVX (+0xC) exposes the
live level.

(B) PLUMBING VERDICT: the params reach the REGISTER PAGE, not the backend.
Retail computes everything -- the instrument load (func_8003E5BC leg)
unpacks per-program ADSR fields from the WDS bank, seq cmds (0xC1 raw ADSR
family) override them, and func_8003E900's dirty-flag flush (bits
0x10/0x20/0x40/0x80/0x100) assembles real ADSR1/ADSR2 words into the
SpuUnion page at +0x8/+0xA per voice, before committing KONs (ordering is
already attr-before-key). The translator (PcPort_SpuRegFlushTick) forwards
only VOLL/VOLR/PITCH/WDSA; the backend ignores ADSR mask bits anyway
(PsyX_SPUAL_SetVoiceAttr's "TODO: ADSR" -- though SPUALVoice already embeds
the full SpuVoiceAttr with ar/dr/sr/rr/sl/adsr1/adsr2 fields, so storage
exists unused). PREREQUISITE (cheap, ~30 lines): change-detected raw-word
forward from the page via the existing SPU_VOICE_ADSR_ADSR1|ADSR2 mask bits.

LATENT BUG FOUND (fixed free by this pass): SpuGetVoiceEnvelopeAttr is an
auto-stub that never writes its out-params -- seq cmd 0xFF (func_8003E54C,
"release the voice once the envelope decays") branches on UNINITIALIZED
stack. The real envelope generator backs this API with true ENVX and makes
the engine's own voice-release logic correct.

(C) APPLICATION POINT: per-tick AL_GAIN at the existing 240Hz translator
tick (counter-2 event, g_SoundTickMutex-serialized -- the advance clock
already fires in the right bracket). No software mix stage exists (PsyX
decodes ADPCM at key-on into one static AL buffer per voice) and none is
needed for phase 1. Composition rule: the envelope MULTIPLIES the
volume-derived gain (effective = baseGain * level/0x7FFF); today
SetVoiceAttr writes AL_GAIN directly from VOLL/VOLR, so the volume path must
store baseGain and let the envelope tick own the final AL_GAIN write (two
writers would fight). GRANULARITY: advance the spec's counter machine at
44.1kHz in a per-tick batch (~184 cycles/voice/tick, ~1.1M iter/s for 24
voices -- trivial), quantize the APPLICATION to 240Hz; OpenAL-soft smooths
gain changes across its mix period, and sub-4ms attacks collapse to
effectively-instant (audibly identical). The risky band is ~5-50ms ramps;
phase 1's capture proof doubles as the stepping measurement. If stepping is
audible, phase 2 is AL_SOFT_callback_buffer streaming (per-sample envelope
in the mixer callback) -- the only leg that touches architecture, deferred
until measured, NOT assumed needed.

(D) STATE MACHINE (per SPUALVoice): {phase Off/Attack/Decay/Sustain/Release,
level, counter}. KON -> level=0, Attack (do not re-latch: rates are read
LIVE from the last-flushed adsr1/adsr2 each advance -- hardware reads the
registers continuously, mid-note changes apply). Attack->Decay at 0x7FFF,
Decay->Sustain at (SL+1)*0x800, KOFF -> Release from any phase; in Release
the source KEEPS PLAYING (no alSourceStop) until level 0, then stops.
Key-status semantics follow: GetKeyStatus stays AL_PLAYING-based, which now
correctly reports SPU_OFF_ENV_ON during tails; mute/pause paths unchanged.

(E) FIDELITY PROOF (curve-match, not "has an envelope"): (1) UNIT -- the
envelope generator as a pure function, golden-diffed cycle-exact against an
independent offline Python implementation of the psx-spx pseudocode over an
AR/DR/SR/RR/SL x mode matrix (phase durations + level trajectories). (2)
CAPTURE -- prim-probe a single note (constant-amplitude synthetic sample,
chosen ADSR params), ALSOFT wave capture, extract gain(t) by windowed RMS
divided by the sample's flat amplitude; assert attack-knee time and release
slope within +/-1 tick (4.2ms), sustain plateau at (SL+1)*0x800/0x8000 of
peak, exponential phases compared in the log domain. (3) IN-GAME A/B --
MAP000 before/after: release tails visible in the waveform where notes now
decay instead of cutting rectangular; by-ear confirmation. HONEST
LIMITATION: the proof standard is "matches the psx-spx-documented algorithm"
(itself derived from hardware tests); a real-console or
DuckStation-reference A/B is an optional stretch, not the milestone gate.

PRICING: BOUNDED backend pass, M2-sized; ZERO decomp (retail's side is
complete and already proven byte-exact). Split: (i) translator plumb ~30
lines (port_main.c); (ii) envelope generator + state machine + gain
composition + key-off/status rework ~150-200 lines in PsyX_SPUAL.cpp --
PATCH-MANAGED (extern/ is gitignored; new psycross_sound_adsr.patch beside
the six existing patches; the C++ linkage trap applies to any new export --
extern-C decl + nm/stubs.c check); (iii) proof tooling ~100 lines (Python
SPU-curve model, envelope extractor, probe ADSR-param hook). Phases: 1 =
plumb + machine + per-tick gain + curve-match proof (THE milestone,
first-audible-fidelity); 2 = per-sample callback path ONLY if phase 1
measures audible stepping; 3 = the other fidelity causes (ADPCM decode,
reverb-vs-EFX, resampling) as separate later passes. SHOWSTOPPERS: none --
both candidates checked and cleared (no mix-stage needed for phase 1;
params unplumbed but cheap to plumb). Risk register: OpenAL-soft's
gain-smoothing behavior is assumed, and phase 1's capture measures it.
Evidence: scoped (read-only trace, spec + backend + translator + retail
flush all read; no implementation)
Last verified @ HEAD of this commit

### ADSR fidelity phase 1 LANDED: hardware ADSR envelope live, release tails in-game

The scoping map above is implemented (psycross_sound_adsr.patch + the
translator plumb). The backend now runs the psx-spx counter machine
per voice: {phase, level, counter} advanced at the FULL 44100Hz envelope
rate in per-tick batches (240Hz translator tick, fractional accumulator --
long-run rate exact), rates read LIVE from the last-flushed ADSR1/ADSR2
words (mid-note changes apply). KON = level 0 + Attack (the source starts
at composed gain 0 and ramps); KOFF = Release with the source KEPT PLAYING
until level 0, then stopped -- the release tail, replacing the legacy hard
alSourceStop rectangle. Composed gain has ONE writer
(ApplyVoiceComposedGain = baseGain * level/7FFFh; the volume path stores
baseGain and routes through it; last-value suppressed). Raw ADSR words are
plumbed at KON + change-detected in the continuous flush.
SpuGetVoiceEnvelopeAttr/SpuGetVoiceEnvelope are REAL (backed by the
generator) -- fixing the latent bug where seq cmd 0xFF (free-voice-on-
envelope-decay) branched on uninitialized stack through the auto-stub.
ALC_REFRESH=240 requested so property updates track the tick cadence.

THREE-LEVEL CURVE-MATCH PROOF:
(1) UNIT -- the production generator (PsyX_SPUAL_AdsrDebugCycle drives the
exact runtime code) vs an INDEPENDENT Python transcription of the psx-spx
pseudocode: 8-entry rate/mode matrix covering linear/exp attack (all three
>6000h slowdown legs), decay, sustain hold/increase/exp-decrease,
never-step rates, small-increment and clamp-to-1 counter legs, lin/exp
release -- ~1.3M cycles, 14,892 sampled points, diff EMPTY (cycle-exact).
(2) CAPTURE -- one note on a constant-|amplitude| synthetic ADPCM square
with known params (lin attack s10 = 53.1ms, decay s7 to SL7 = plateau
0x4000, lin/exp release s12): attack knee 50.6ms vs model 50.4ms (delta
0.1ms); attack+decay+sustain residual vs the 240Hz-quantized model 1.84% of
peak; release SHAPE residual vs the model trajectory 0.42% (linear) / 1.25%
(exp) of plateau; envx state samples matched the closed form exactly
(30870 @ 50ms = 14 steps x 2205 cycles).
(3) IN-GAME -- MAP000 60s A/B vs the 76a4108 baseline: hard-cut cliffs
(>=12dB drop per 5ms window at >5% peak) 54.2/min -> 6.0/min (-89%; the
remainder are legitimately fast-release instruments); max drop 167dB
(digital cut) -> 16.8dB. Tails are audible; captures saved (session
scratchpad adsr_before/after_map000.wav).

STEPPING MEASUREMENT (the phase-2 trigger): at tick-resolution rendering
the max attack-ramp step is 2.78dB (2ms windows, >10% peak) and OpenAL-soft
fades linearly within each period -- piecewise-linear envelope, NO
discontinuities. At DEFAULT backend periods (~20ms) the 240Hz updates
coalesce (measured: knee 34ms vs 50.4, crest -14%) -- the ALC_REFRESH=240
hint recovers most of it (knee 45.3ms, release ramp restored). VERDICT:
phase 2 (per-sample mix stage) NOT warranted by measurement -- remaining
granularity is a playback-device period property, not an envelope defect.

Documented fidelity decisions: exp-decrease scales the step via
arithmetic >>15 (floor) -- psx-spx writes /8000h, but truncation would
stall exp release above zero while floor keeps negative steps <= -1 and
terminates, matching hardware-verified emulator cores; shifts >26 follow
the documented formula (psx-spx notes real hardware degrades oddly there;
games do not use them); the decomposed Psy-Q rate attrs (SPU_VOICE_ADSR_AR
family) stay unimplemented -- the game programs raw register words only,
and a half-faithful repack would be silent wrongness; the captured
plateau/peak reads ~0.53 vs the ideal 0.498 because the 6ms 7FFFh crest
spans ~1 mix period and under-renders ~5% -- a rendering artifact, the
envelope state is exact.

Validation: slus byte-exact (d004692f... unchanged; zero src/ changes);
all 8 sound probes PASS post-change; five-map tripwires EXACT
(addr-normalized, session before/after logs); TSan MAP000 clean on the
envelope path (advance runs under the same gate bracket + backend mutex as
the pre-existing attr writes). Linkage verified (all five new exports
unmangled T, zero in stubs.c). Remaining fidelity causes: ADPCM decode,
reverb-vs-EFX, resampling (phase 3, separate passes); per-note ADSR is
DONE.
Evidence: proven (3-level curve-match: cycle-exact unit diff + capture
knee/plateau/release-shape + in-game cliff A/B)
Last verified @ HEAD of this commit

## Audio fidelity: ADPCM-decode + Gaussian-resampling scoping map (cause #2, priced, no implementation)

SCOPING PASS (read-only; fidelity cause #2 after ADSR landed). Verdict up
front: the ADPCM RECONSTRUCTION is structurally correct and is the SMALL
part; the audible gap in this cause is (1) the SPU's GAUSSIAN INTERPOLATION
being absent -- pitch resampling is delegated to OpenAL-soft's cubic
resampler -- and (2) loop-point defects. And the honest scope-reshaper: the
Gaussian fix REQUIRES the per-voice streaming pipeline (the mix-stage
architecture ADSR phase 2 deferred) because mid-note pitch changes rule out
any key-on-time pre-render.

(A) GROUND TRUTH (psx-spx SPU chapter, "SPU ADPCM Samples"/"SPU ADPCM
Pitch"): 16-byte blocks -- header byte = shift(0-3)/filter(4-6), flag byte
bit0 LoopEnd (set ENDX + jump to repeat address), bit1 LoopRepeat (0 WITH
bit0 = End+Mute: jump + force Release + env 0), bit2 LoopStart (latch
current addr as repeat addr); codes: 0/2 continue, 1 End+Mute, 3
End+Repeat. Decode: s = clamp16((nibble<<12 >> shift) + (f0*prev +
f1*prev2 + 32)>>6), filters {(0,0),(60,0),(115,-52),(98,-55),(122,-60)}/64
(the "same as CD-XA" cross-reference; the backend's own K0/K1 floats are
EXACTLY these fractions -- 0.9375=60/64, 1.796875=115/64, 1.53125=98/64,
1.90625=122/64, -0.8125=-52/64, -0.859375=-55/64, -0.9375=-60/64); shift
13-15 behaves as shift 9 (XA-note edge, encoders emit 0-12). PITCH: 16-bit
counter step = VxPitch (1000h = 44100Hz, step clamped to 4000h = 176.4kHz);
counter bits 12+ select the sample within the block, bits 4-11 are the
8-bit GAUSSIAN INDEX. Interpolation (the "SPU sound"): out =
(gauss[FFh-i]*oldest + gauss[1FFh-i]*older + gauss[100h+i]*old +
gauss[i]*new) each SAR 15, over the 512-entry table (fully transcribed in
the spec; captured to the session scratchpad psxspx_spu.md).

(B) BACKEND CLASSIFICATION (PsyX_SPUAL.cpp @ e96f4cb):
- ADPCM reconstruction (vagToPcm, 583-598): APPROXIMATED-CORRECT. Exact
  coefficient VALUES as floats; structure right (nibble sign-extend, two
  prev taps). Diverges in numerics only: float accumulation with round()
  instead of the integer (+32)>>6 path, pow(2,12-shift) with NO shift 13-15
  clamp, prev-state carried in float. LSB-level -- inaudible alone, but
  blocks sample-exact proof.
- Gaussian interpolation: ABSENT -- DELEGATED. Decode-at-keyon renders the
  whole chain to PCM tagged 44100 (748), AL_PITCH = pitch/4096 does the
  resampling through OpenAL-soft's explicitly-selected CUBIC resampler
  (434: AL_SOURCE_RESAMPLER_SOFT=2). Cubic is brighter with different
  image rejection than the SPU's soft 4-tap Gaussian -- THE character
  difference, and it touches every pitched note (i.e. essentially all).
- Loop flags (decodeSound, 635-684 + 752): WRONG-in-general. LoopStart
  latches at k+26 ("FIXME: is that correct?" in-source) = the END of the
  flagged block; hardware latches the block START -- loop start lands one
  block (28 samples) late. The loop_addr adjustment (752: loopStart +=
  loop_addr - addr) adds a BYTE delta to a SAMPLE index (should be
  bytes/16*28) -- benign only in the common loop_addr==addr case.
  End+Mute (code 1) is simplified to play-to-end/no-loop; hardware jumps +
  forces Release + ENDX (matters more now that ADSR release is real).
- Pitch-step clamp (4000h): absent (AL_PITCH unclamped). Rare; note only.
IMPACT ORDER within this cause: Gaussian >> loop defects > decode numerics
> pitch clamp.

(C)/(D) FIX SHAPE -- two phases, and phase B is the mix-stage question
answered FOR REAL:
- Phase A (bounded, no architecture change): rewrite the decoder
  integer-exact (hardware semantics incl. shift-clamp + (+32)>>6 +
  int-state + clamp16), fix loop-block semantics (latch at block start,
  sample-unit loop math), keep OpenAL cubic. ~80-120 backend lines. Gets
  sample-exact PCM + click-free correct loops; does NOT get the Gaussian
  character.
- Phase B (the lever; scope-reshaper): per-voice STREAMING synthesis via
  AL_SOFT_callback_buffer -- the mixer-thread callback walks the SPU pitch
  counter (step = live voice pitch, clamped 4000h; bits 4-11 Gaussian
  index), decodes ADPCM blocks on demand with hardware loop/ENDX/End+Mute
  semantics, applies the 4-tap Gaussian, outputs fixed-rate 44100 PCM;
  AL_PITCH pinned 1.0. WHY streaming is REQUIRED: sequence pitch changes
  mid-note (vibrato/portamento modulate pitch per tick), so any
  pre-rendered resample at key-on is wrong the moment pitch moves --
  per-sample generation is the only faithful shape. Composition with ADSR:
  clean -- the per-tick composed AL_GAIN (envelope x volume) rides on top
  of whatever the source plays; pan/reverb sends unchanged; KON resets the
  stream cursor where UpdateVoiceSample sits today. Mid-note pitch actually
  IMPROVES (read live per callback chunk vs AL_PITCH quantized at device
  periods). Once the callback pipeline exists, per-sample ADSR (the
  deferred phase 2) becomes a cheap optional upgrade inside the same loop.
  Threading: the callback runs on the mixer thread -- per-voice state
  handoff needs short g_SpuMutex sections or a per-voice seqlock (design
  gate, not a stopper). Emscripten lacks the extension -- keep the legacy
  decode-at-keyon path as a compiled fallback.

(E) PROOF (same bar as ADSR -- matches the spec, sample-exact where
possible): (1) unit decode: hand-built ADPCM vectors (all 5 filters, all
shifts incl. 13-15, clamp edges) + a real WDS instrument (bytes already
readback-proven in SPU-RAM) through the production decoder via a debug
export vs an independent Python integer decoder -- diff EMPTY,
sample-exact. (2) unit Gaussian: the 512-entry table + interpolation at
swept counter positions, C vs Python sample-exact; end-to-end fixed-pitch
resample of known PCM, sample-exact. (3) capture: single-note at
pitch != 1000h, FFT -- alias/image line positions and levels must match the
Python-predicted Gaussian spectrum (current cubic capture is the A/B
baseline); loop-click test on a sustained looped instrument (before:
loop-rate click harmonics from the off-by-a-block start; after: clean).
(4) in-game MAP000 spectral tilt A/B (Gaussian rolls off highs vs cubic)
+ ear. Real-hardware/emulator reference optional, not the gate.

PRICING: ZERO decomp (pure spec->backend, ADSR-shaped). Phase A: ~half an
M2 (decoder rewrite + unit proof + loop capture). Phase B: M3-sized
(~300-450 backend lines: callback pipeline, Gaussian table+counter,
threading, fallback; + probe/analysis extensions) -- bigger than ADSR
phase 1, bounded, not open-ended. RECOMMENDATION: run as one combined pass
(A then B -- B's proof harness subsumes A's); B is where the audible
character lives, A alone won't move the user's complaint much.
SHOWSTOPPERS: none hard. The scope-reshaper is explicit: faithful Gaussian
NEEDS the streaming mix-stage (AL_SOFT_callback_buffer -- available in
OpenAL-soft >= 1.22 natively); the one design-risk gate is mixer-thread
state handoff discipline. After this cause: reverb-vs-EFX and any residual
resampling polish remain (causes #3/#4, separate passes).
Evidence: scoped (read-only trace @ e96f4cb, spec captured + backend
classified line-by-line; no implementation)
Last verified @ HEAD of this commit

### ADPCM/Gaussian LANDED: SPU-faithful streaming synthesis -- the timbre pass

The combined A+B pass is implemented (psycross_sound_adpcm.patch):
per-voice AL_SOFT_callback_buffer streaming (OpenAL-soft 1.25.1) --
integer-exact ADPCM block decode (five filters, (+32)>>6 rounding, clamp16,
shift 13-15 -> 9), hardware loop semantics AT THE BLOCK MACHINE (LoopStart
latches at decode, LoopEnd jumps after the block, End+Mute crosses to the
tick via an atomic flag and forces Release + envelope 0 -- the old
one-block-late latch and byte-vs-sample loop-addr bugs are structurally
gone), and the SPU resampler: 16-bit pitch counter (step = live VxPitch,
clamped 4000h), counter bits 4-11 indexing the 512-entry Gaussian table
(transcribed from psx-spx; parse validated by the 4-tap unity-gain
invariant, all 256 sums in [32639,32641]), 4-tap interpolation with
per-product SAR 15.  AL_PITCH pinned 1.0; mid-note pitch reads live per
fill (better than the old per-period AL_PITCH quantization).  ADSR rides
on top unchanged (per-tick composed AL_GAIN).  Legacy cubic path kept:
XENO_SOUND_LEGACY_RESAMPLER=1, emscripten, or no-extension fallback.

THE CONCURRENCY GATE (the pass's load-bearing design, met): all stream
state + SPU-RAM uploads under a dedicated s_StreamMutex; LOCK ORDER
g_SpuMutex -> s_StreamMutex, NO OpenAL call ever made while holding it (no
inversion against alsoft's mixer lock); KON restart addresses staged
tick-side so the callback never reads voice attrs except the
mutex-mirrored pitch; g_spuInit atomic; shutdown quiesces the callbacks
before teardown.  TSan on the FINAL build: MAP000 100s music (per-tick
vibrato pitch writes racing mixer fills) -- ZERO new-class port races
(residuals: the pre-existing VBlank/exit-teardown class + external-lib
noise); the probe run (full pipeline incl. shutdown) -- ZERO port racing
frames.  Two probe-harness races found and fixed along the way (main-thread
SpuWrite now gate-bracketed in both capture probes; ShutdownSound ordering).

PROOF: (1) unit -- production decoder + interpolator vs the independent
Python model (16 crafted blocks x all filters/shifts/clamps + 156 Gaussian
points): diff EMPTY, sample-exact.  (2) capture FFT (period-set conf per
the alsoft trap): all 25 reference alias peaks matched at 0.00dB mean/max
error, log-mag correlation 0.9933 vs the model (cubic A/B: 0.8638), and
the audible signature quantified -- CUBIC IS +10.4dB HOT in 8-20kHz vs
the SPU reference while the Gaussian tracks it within 0.1dB; loop floor
delta -0.4dB (no clicks).  (3) in-game MAP000 A/B: 40s band averages are
mix-dominated (no clear tilt -- band means sit on content bins, not alias
floors; honest null), ADSR release tails intact (cliff metric 1.0/min),
all 8 sound probes PASS, five-map watchdogs byte-exact except the new
one-line streaming banner, slus d004692f intact.  EAR: scratchpad/
gauss_before_map000.wav vs gauss_after_map000.wav.

Remaining fidelity causes: reverb-vs-EFX, residual resampling polish
(separate passes); per-sample ADSR is now a cheap optional upgrade inside
the callback loop.
Evidence: proven (sample-exact units + FFT alias-line curve-match + TSan
gate on the final build)
Last verified @ HEAD of this commit

### FIXED: End+Mute consume now does the full off-lining (was: streams feeding at stale gain)

Fix applied (see the commit): the endMute consume performs the same
off-lining as the normal release-complete path -- phase=OFF, level=0,
ApplyVoiceComposedGain (the zero reaches AL; NOT under the stream mutex,
per the gate invariant), stream.active=0 under s_StreamMutex. The
hardware-faithful End+Mute JUMP in the callback is unchanged.
Validation: the diagnosis's own census flipped 1-4 stuck voices at every
instant -> stuckOffActive=0 at every instant; background floor -13%
(the stuck-loop mush gone) while onset density +29% (percussion hits
PRESENT and more distinct -- not over-silenced); 8/8 sound probes PASS;
five-map watchdogs byte-exact; TSan zero new-class port frames on the
changed handoff; slus d004692f intact; psycross_sound_adpcm.patch
regenerated CUMULATIVELY (742 lines; note for the recipe: when AMENDING
an existing patch, the index seed must capture the PRE-SERIES state --
reverse the old patch first -- or the regen clobbers it to fix-only).

Original diagnosis (for the record):

SYSTEMIC BUG (diagnosed): End+Mute consume left the stream feeding at stale gain

The USER's refined report ("every song has an element that repeats") is a
REAL systemic 7d9b969 regression, DISTINCT from the ambient finding below
(which stands for the MAP014/Lahan ambients specifically). Mechanism,
proven code + runtime:

CODE SITE (PsyX_SPUAL.cpp, EnvelopeTick's End+Mute consume): when the
mixer callback hits an ADPCM code-1 block (LoopEnd without Repeat =
hardware End+Mute: jump + force Release + envelope 0), it sets the
endMutePending atomic; the tick consumes it and sets phase=ADSR_OFF,
level=0 -- and then `continue`s, SKIPPING the two lines that actually
silence a voice in the streaming path: ApplyVoiceComposedGain (the AL
gain stays at the STALE SUSTAIN VALUE -- level=0 never reaches OpenAL)
and stream.active=0 (the callback KEEPS FEEDING the looped-from-
repeatAddr sample forever). Net: every one-shot/End+Mute-terminated note
(the percussion family -- claps, slaps, drums, in EVERY song) keeps
looping audibly at its sustain gain until something re-keys the voice
(the "stops when the song changes section" the user observed).

RUNTIME PROOF (MAP000, census every 400 ticks): at EVERY instant, 1-4
voices in exactly the predicted stuck state -- phase=OFF active=1
lastGain 0.16-0.24 (audible), rep=start+0x10, cur wandering the sample --
individual stuck notes persisting 800+ ticks (3.3s+: V17 identical
lastGain 0.194899 at f400 and f1200; V19 across f800-f1600), a rotating
population as re-keys reclaim voices. The earlier MAP001 lifetime census
ALREADY showed the signature (V14/15/16 "phase=0 lvl=0" yet active at
f900) and it was misread as a one-tick transition window.

OLD-PATH CONTRAST (why it regressed): the cubic path gave code-1 samples
loopLen=0 -> AL_LOOPING FALSE -> the source played out and stopped
naturally. The streaming rewrite implements the hardware jump (correct)
but drops the MUTE half on the tick side.

FIX (next pass, surgical): in the endMute consume, do the full
off-lining the normal release-complete path does -- phase=OFF, level=0,
ApplyVoiceComposedGain (writes gain 0), and stream.active=0 under
s_StreamMutex. One small block; no streaming-architecture change.
Evidence: code-path read + stuck-state census (phase-OFF/active/stale-
gain, multi-second persistence, rotating population)
Last verified @ d53b347

### PARTIALLY-RETRACTED (the ambient half stands; the "no defect" conclusion was wrong -- see the systemic entry above): the "boot-zombie clap" is the scripted ambient, rendered hardware-faithfully

The fix pass DISPROVED the entry below (kept for the record). The full
corrected chain, each link evidenced:
- The "zombie" KON is the REAL MAP014 boot-script ambient cue (id 0x36):
  watchpoint on element 14's active_flag caught the armer red-handed --
  FieldScriptVMRun -> func_8008F558 (cue opcode) -> func_800855C8(0x36,
  32, 64, ch3) -> func_80039F9C -> func_8003B644 arm -> func_8003E5BC
  instrument bind. Script-OWNED, not ownerless. (The diagnosis pass's
  "frame 1" timing was a gdb-pacing artifact -- real-time onset is 4.40s,
  the known cue time; its "8s stuck at sustain" is an AMBIENT sustaining,
  as ambients do, until the script changes it.)
- addr 0x12000 / pitch 0x203 / voll 1785 are the AUTHORED instrument-0
  bind of the effect's bank (start = offset 0 + bank base), not defaults.
- func_80085634's id-0 stop gate is present and correct in the port (the
  suspected dropped branch does not exist -- retail asm compared).
- The loop point is the AUTHORED in-data LoopStart latch (0x12890) --
  identical on hardware: retail's own func_8003E5BC asm (read this pass)
  computes loopAddress WITHOUT the bank base (addu of raw offsets ->
  +0x50), so the repeat REGISTER is bank-relative-garbage on hardware too
  and the in-data latch supersedes it there exactly as in the port. The
  filed "dormant bank-base bug" is RETAIL-FAITHFUL BEHAVIOR, not a bug;
  do not "fix" it. LSAX forwarding stays unforwarded-and-documented: the
  register is vestigial in this engine (in-data latches always win), and
  forwarding it would only matter in the no-in-data-LoopStart case where
  hardware jumps to the same garbage value anyway.
- The audible change vs pre-7d9b969 is the RENDERING, and it is the
  hardware-faithful one: at pitch 0x203 (0.126x) the old cubic resampler
  smeared the crackle's transients into the faint ~0.014-RMS wash the
  earlier passes measured; the SPU's Gaussian renders them as the crisp
  pops the console produces. The hardware-spec Python model, fed the
  ACTUAL dumped sample at the actual pitch, produces the same dense
  crackle texture (scratchpad/amb_model_render.wav vs amb_c_capture.wav);
  captured ambient loudness matches the authored gain math (~427 vs
  predicted ~606 envelope units). MAP001's "clapping" is the same class:
  Lahan's scripted ambient cues (chopping/knocking), inaudible-by-smear
  under cubic, now rendered as authored -- and they stop at section
  changes because the SCRIPT stops them.
VERDICT: no fix applied; the streaming path and the arming path are both
correct. Remaining honest uncertainty: whether the authored ambients
sound THIS prominent on real hardware -- needs a console/emulator
reference of the intro windows (same standing limitation as the ripple
entry). If a reference shows retail quieter, the delta hunt starts at the
effect-script modulation (C6E8) fidelity, not the resampler.
Evidence: watchpoint callchain + retail asm (85634 gate, E5BC loop math)
+ hardware-spec model render vs capture
Last verified @ 7d9b969

### SUPERSEDED by the entry above (original diagnosis, premise disproven): boot-zombie SFX voice becomes audible under streaming

User-heard after 7d9b969 (live MAP001): a percussive "clapping" loops under
the music until a song-section change, then stops. ROOT-CAUSED (gdb-only,
zero code changes), deterministic reproducer on MAP014:

VOICES 22/23 (the S1 SFX pair) are KEYED ON AT FRAME 1 with default page
state -- addr = 0x12000 (page start-reg default 0x2400 <<3 = the bank
BASE), pitch 0x203 (0.126x), voll/volr 1785 (audible; baseGain 0.109),
full-sustain ADSR -- and NOTHING ever terminates them: no sequencer note
owns them (keyed before any cue), no KOFF ever arrives, and the bytes at
the bank base happen to carry an in-data LoopStart (0x12890) with a
Repeat-flagged end, so the stream loops ~87ms of bank-opening data at
0.126x = ~1.4 repeats/sec -- the "clapping" -- at env sustain 32767 for
8+ seconds (probe: konf=1, ageTicks 1999+) until something re-keys the
pair (MAP014's 4.3s cue; on MAP001, whenever the music/section re-keys).

WHY IT'S NEW WITH 7d9b969 (the exact regression semantics): the zombie
KON predates the pass, but the OLD path decoded the sample ONCE AT KEY-ON
-- at frame 1 SPU RAM was still EMPTY, so it snapshotted and looped
SILENCE (inaudible zombie). The NEW path streams LIVE SPU RAM (hardware-
faithful -- a real SPU reads live memory too), so when the bank loads
seconds later the zombie starts SOUNDING. The streaming pass didn't create
the stuck voice; it made a pre-existing silent zombie audible.

EXONERATED by the probes: the loop machinery (in-data LoopStart latch +
LoopEnd jumps correct on MAP000/001 censuses; sustained instruments
release THROUGH loops -- v0 observed fading phase=4 across loop jumps);
the End+Mute atomic (unexercised -- Repeat is set); the KOFF path (1652
KOFFs flowed; released voices die normally); the mixer gate (not a race).

FIX-PASS TARGET: trace who keys 22/23 at frame 1 (the S1 element-arming
path -- B148 pre-assignment / B644 arm with an empty spec at init, or the
field-test harness's eager-load ordering) and stop the ownerless KON.
NOT a streaming-path change. Next probe: break the page key_on commit
(func_8003E900 tail) at boot, walk the caller chain for bits 22/23.

ALSO FOUND (dormant, file-for-later): the translator never forwards the
engine-programmed loop register (page +0xE; no LSAX in the KON mask or
the continuous flush -- attr.loop_addr is 0 in-game), and func_8003E5BC's
port body computes loopAddress WITHOUT the bank SPU base
(startAddress = base + *(pBank+0x28) but loopAddress = base + loopOff<<3
-- page values run 0x12000 short; verify against retail asm). Harmless
today: every observed music instrument carries in-data LoopStart flags
that override the register (psx-spx's register-redirect use case --
one-shot redirected to a silent loop -- is where these would bite).
Evidence: root-caused (deterministic MAP014 reproducer; per-voice census,
loop-jump traces, gain/pitch dump); fix not yet implemented
Last verified @ 7d9b969

## Map014 intro scene renders the wrong geometry (back of Fei's head, not the fire painting)

User-confirmed live at f2d8778: MAP014's intro scene -- the camera zoom-in
on the fire painting, with Fei -- renders the WRONG content. The shot shows
the back of Fei's head where the fire painting the camera is supposed to
zoom into should be. Audio on the same scene is correct (the map's music and
its deterministic boot-script SFX cue were both proven this session); the
defect is visual only.

DISTINCT from the fea685a fix: that commit resolved the Map014 intro
"overdraw smear" (func_8002E688 used RotTransPers4's return value instead of
retail's min(SZ0..SZ3) >> D_80050100 for OT-bucket assignment -- a
depth/ordering defect). The present bug is wrong CONTENT/geometry, not
ordering: a different class -- candidate causes include a separate
render-path defect, wrong model/object selection, or a camera/transform
issue in the zoom sequence. Needs its own investigation; do not assume the
fea685a mechanism.

Repro: `./scratchpad/run_map014.sh` (or XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0
XENO_FIELD_MAP=14 XENO_FIELD_ENTRANCE=0) and watch the intro zoom sequence.

ROOT CAUSE (diagnosed, gdb-only, no code changes; fix is a separate pass):
the intro closeup is a DEDICATED SCREEN-FILLING PAINTING OBJECT -- actor 24,
the "fiery painting" static model -- positioned directly in front of the
scripted closeup camera. Its quads project EXACTLY where retail wants them
(a grid spanning the full screen; per-quad probe at f60: xy from (-64,-116)
to (415,247)) but nearly every screen-filling quad carries GTE FLAG
0x80021000 (bit17 perspective-divide overflow -> bit31) from RTPT on its
near-plane vertices, and ModelPrimQuadFT4Variant0's `rtptFlag < 0` gate
(game_overrides.c:1079) rejects them. Only small edge slivers survive --
the stray fire patches visible at the frame edges early in the scene. With
the painting gone, the raw 3D scene shows through: the script camera
(eye=(141,-75,-431) at=(23,-50,-548), held frames 0-211, script-armed and
faithfully applied) sits 35 units behind the script-placed player at
(115,-1,-455), whose head therefore fills the shot.

PROOF (runtime experiment): clearing the RTPT/RTPS FLAG verdicts for actor
24's quads only (gdb, no code change) recovers the retail shot -- the fire
painting fills the frame through the closeup and the pull-back still works.
Evidence frames: scratchpad/m14_bug_f120_head.png (defect),
m14_rootcause_forced_f120/f180_painting.png (forced-accept recovery),
m14_hidefei_current_f120_wall.png (hide-player intermediate: without the
billboard the camera sees only a wall corner -- the painting content CANNOT
come from the room BG/easel from this camera).

ELIMINATED with evidence: harness-entry placement (spawn records are
(165,-25)/(178,313); the player's (115,-1,-455) is script-placed); camera
mis-decode (arm values logged at FieldScriptStartCameraMovement match the
held camera); BG-angle (cur==target mod 0x1000); actor-visibility gate
(func_800AAA74 passes, dispatch confirmed at f30/f120); stubbed opcode
handlers (zero field-script stubs fire in the current build);
XENO_E688_IGNORE_FLAG (no effect -- it bypasses func_8002E688's gate, but
actor 24 draws through ModelPrimQuadFT4Variant0, a different walker with an
unbypassed FLAG gate).

FIX-PASS QUESTION (the one fork left): retail hardware demonstrably shows
this scene, so either (a) real-GTE RTPT does not overflow the divide here
(port-side SZ3 smaller than hardware's -- a transform/precision divergence
upstream), or (b) PsyX's GTE divide-overflow semantics set bit17 where
hardware wouldn't (UNR divide emulation), or (c) retail's variant-0 FT4
walker asm does not reject on this FLAG pattern (compare the original asm
behind D_8004FE50's proc entry). Next probe: dump SZ0..SZ3 for the rejected
quads vs h=0x80 threshold (overflow iff h/2 >= SZ3 per psx-spx), and read
retail's walker asm gate. The scene's script choreography (extended opcodes
0x26/0x27 = FieldDistortionInitialize/control -- the painting ripple)
composes ON TOP of the billboard via framebuffer feedback and needs no
separate fix.

Related history: the same intro scene previously exposed fea685a (OT-bucket
min-SZ), the BG-angle sign-extension fix (misc2.c:1207), and the
"floating geometry" era (m14_cu_* captures: scattered BG fragments --
resolved by the intervening matrix/CLUT fixes; today's room renders
coherently). This scene is the port's render torture test: one shot
exercises the scripted camera, the BG re-projection, actor models, the
closeup billboard, AND the distortion feedback.
FIXED: the fork resolved one level deeper than any of the three candidates
as stated. The SZ dump (f60: SZ 227..307 vs h=512 threshold 256, flag
firing EXACTLY when a pushed SZ < 256) proved the port's transform chain
EXACT (billboard world-delta ~85 units x worldScale 3.0 = SZ 255 on the
nose) and PsyX's overflow arithmetic spec-correct -- the scene is AUTHORED
with the billboard straddling h/2 (the script itself sets scrZ=0x200:
opcode bytes `a0 05 80 19 80 00 82` at IP 17). Hardware overflows these
divides too. The resolution is in the retail walker asm: its "FLAG check"
is `mfc2 $t0, $31` -- MFC2 reads DATA reg 31 (LZCR, a leading-zero COUNT,
always 1..32), not the FLAG control reg (which needs CFC2) -- so retail's
bltz reject NEVER TAKES on hardware. The overflowed divide SATURATES
(quotient clamped 0x1FFFF; PsyX's Lm_E identical; screen coords clamp
+/-0x400) and the quad draws with mild stretch. The port's walker had
translated the apparent intent (gte_stflg = the real FLAG) instead of the
actual shipped behavior, creating a rejection gate retail never had.
Fix: removed both FLAG gates from ModelPrimQuadFT4Variant0
(pc_port/src/game_overrides.c; port-only, slus untouched). Validation:
fixed f120 is PIXEL-IDENTICAL (100.0%, max diff 0) to the forced-accept
target; f60 OT tripwire EXACT post-fix (actor 38 walls {73,74,87,89},
actor 31 bucket 67 unchanged; actor 24 now emits 13 quads into buckets
14-17; no other actor uses this walker at f60 -- blast radius structurally
confined); five-map watchdogs exact (see commit).
The distortion ripple (opcodes 0x26/0x27) composes over the billboard via
framebuffer feedback and was NOT part of this defect; its own fidelity is
untested-but-unchanged here.
Evidence: fixed + proven (pixel-exact vs the proven target; tripwires)
Last verified @ HEAD of this commit

## Map014 intro ripple (opcodes 0x26/0x27): horizontal banding/doubling while active — fidelity untested

User-observed in live play after the cb57e9d fire-painting fix: during the
intro closeup the painting shows horizontal "doubling"/banding — content
appears displaced across two seams (upper third + lower). TRIAGED
(read-only): the banding exists in the STATIC mid-ripple frames (f60/f120
row-discontinuity spikes of magnitude ~42-45 at capture y=68 and y=376)
and is GONE at f180 (profile halves and moves) — it tracks the distortion
effect's ACTIVE WINDOW (script arms it f0, distA=1 until ~f145) exactly.
The seam positions map onto the EFFECT'S OWN packet geometry, not the
billboard's quad rows (110/370 do not spike): y=68 = the warp grid's
pinned top edge (PSX 0x20, row i==0 fixed while row 1+ wave), y=376 =
inside the rows-14-16 relocated strip band (PSX 176-224, re-composited
from the D_800AEB24 capture strips). NOT the cb57e9d fix's doing: the fix
is bit-identical to the forced-accept target and the target frame ITSELF
contains the banding (it is a mid-ripple frame).

OPEN QUESTION (why this stays filed): some seam structure is AUTHORED
(retail's packet layout pins the top edge and recomposites the bottom
strips; hardware shows displaced copies mid-wave too), but whether retail
looks this HARD-EDGED is unverified — needs an emulator/console reference
of the intro. Candidate port-side mechanism if unfaithful: the ripple's
framebuffer CAPTURE (DR_MOVE packets threaded mid-OT at bucket 1) vs the
GL pipeline's draw ordering — the prior FT4-feedback arc verified texel
SAMPLING exact but not capture ORDERING. Repro:
./scratchpad/run_map014.sh, watch the closeup's first ~5 seconds; compare
scratchpad/m14_fixed_f120_painting.png (banded, mid-ripple) vs the f180
equivalent (clean, post-ripple).
Evidence: triaged (seam profile tracks the effect's window + geometry);
fidelity vs hardware unverified; no fix attempted
Last verified @ cb57e9d

## Inert LZCR "flag gates" across the model-prim walker family (SWEPT for all ported walkers)

Found during the Map014 fire-painting fix. EVERY "GTE FLAG check" in the
retail model-prim walker blob (asm/slus_006.64/system/temp2.s) is
`mfc2 $tN, $31` -- MFC2 reads DATA register 31 (LZCR, a leading-zero count,
always 1..32, never negative), NOT the FLAG control register (CFC2). The
paired bltz rejects are therefore DEAD CODE on hardware: overflowed
perspective divides saturate (0x1FFFF) and the primitive still draws. The
single real `cfc2 $31` in the file is elsewhere (0x80030D20, a different
subsystem).

FULL ENUMERATION (the original filing's list was grep-truncated at 14):
24 sites -- E150, E364, E3A0, E58C, E788, E7C0, E9F8, EBF4, EC2C, EE20,
F01C, F1F8, F3E0, F5BC, F7C8, F9D0, FA0C, FBE8, FC2C, FE04, FE40, 30018,
30054, 30858. The walkers are shared common bodies with multiple entry
stubs, so the PORTED walkers cover exactly SIX of these: E150 (shared tri
body), E58C (tri min-SZ body), E364+E3A0 (shared quad body -- F4 and FT4
entries), E788+E7C0 (quad min-SZ body -- F4 variant2 and func_8002E688
entries).

SWEPT (this commit; the fire-painting fix cb57e9d was the template): all
seven port-side gates removed -- ModelPrimTriSmallAverageVariant0,
ModelPrimTriSmallMinimumVariant2, ModelPrimQuadF4Variant0 (both gates),
ModelPrimQuadF4Variant2, ModelPrimTriAverageVariant0,
ModelPrimTriMinimumVariant2 (game_overrides.c) and func_8002E688's
(temp2.c port branch; matching build untouched -- the body is inside the
#else of the #ifndef XENO_PC_PORT guard). Real gates kept everywhere:
NCLIP, screen-overlap, zero-depth. XENO_E688_IGNORE_FLAG DELETED (it
modeled a gate that does not exist). ModelPrimQuadVariant0's gate remains
but the function is UNWIRED dead code (declared/defined, in no table).
Validation: MAP014 f60 OT buckets identical (walls {73,74,87,89}, actor 31
at 67, A24 13 quads at 14-17); MAP014 and MAP000 8-frame ladders both
PIXEL-IDENTICAL pre/post (nothing was being flag-culled on those frames,
and nothing over-draws -- the fea685a class watched, absent); five-map
watchdogs (see commit). CullCam telemetry: flag values still read and fed
to CullCamSeen; the CC_FLAG drop counters are now structurally zero.

REMAINING (not a port defect): 18 sites live in retail walker variants the
port has NOT implemented (variant-1/3/5 functions -- NULL proc slots, plus
later bodies F7C8..30858). They matter only as PORTING GUIDANCE: when any
of those walkers is ported, do NOT translate its mfc2-$31 bltz into a real
FLAG gate -- port it as inert (no rejection), per this entry.
Evidence: retail asm read (all 24 sites); all 6 ported-walker sites swept +
validated (frame-identical captures, tripwires exact)
Last verified @ HEAD of this commit

## Map143 dialogue path crashes in the shared tile/sprite renderer

Map143 has legal entrances `{0, 1}`. From entrance 0, real d-pad input can move
the player to shop actor 14 and a real Circle/talk input selects that actor's
authored talk routine. The routine opens dialogue, then remains at
`WAIT_DIALOG` through at least frame 500 even after authentic close inputs. The
unmodified renderer SIGSEGVs at frame 358, before the script reaches its shop
opcode and before any menu entry point or menu oracle stub executes.

The fault stack is:

```text
GR_UpdateVRAM
AddSplit
BeginTexturedSplit
ProcessTileAndSprt
ParsePrimitive
DrawOTag
func_8007554C
FieldMain
```

The frame-340 pre-fault capture at `/tmp/menu143_prefault.png` is a coherent
field scene with the dialogue box still open; it is not a menu frame. This is
therefore an independent field renderer/dialogue-path defect, not a menu
failure. `GR_UpdateVRAM` and the tile/sprite processing path are shared, so the
same mechanism may affect other maps and needs a separate diagnostic pass.

Repro: build the normal port, then run
`env XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 XENO_FIELD_MAP=143
XENO_FIELD_ENTRANCE=0 SDL_VIDEODRIVER=x11 DISPLAY=:0
pc_port/build_native/xeno-port`; use the d-pad to move from the entrance to
actor 14 and press Z (the port's Circle/talk mapping). The authored interaction
opens the dialogue and reaches `WAIT_DIALOG`; capture under GDB to obtain the
stack above. Entrance 1 is structurally legal as well, but entrance 0 is the
verified reproducer.
Evidence: observed (authentic input and authored actor script; fault occurs
before menu dispatch)
Last verified @ e3f4b49

## Normal menu is cold-boot reachable but render is blocked by the stubbed func_800799D4 (live repro)

The scan's "normal menu is cold-boot reachable" is now confirmed at runtime, and
the render block is isolated to a single stub. Live repro (deterministic,
captured via the nested-X harness below):

- map005, entrance 0. Walk the player to actor 15 (the save actor) and press
  Circle. Its authentic talk routine (script 15, routine 2) runs
  `FE 99 00 | FE 55` at script PC `0x68D`: set-menu-open-arg then
  OP_OPEN_NORMAL_MENU, which sets the menu request `D_800ADB64 = 0x00` (mode 0)
  and bumps the wait counter `D_8004F350` to 1.
- `src/field/main/main.c:595` then calls `func_800799D4` — an oracle stub
  (`pc_port/build_native/stubs.c:610`). It returns without loading the menu
  overlay, without running `MenuMain`, and without clearing `D_8004F350`.
- The next opcode `FE 87` (OP_WAIT_MENU, `func_800936E4`) at script PC `0x68F`
  spins: it advances only when `D_8004F350 == 0`, else decrements the IP by 1.
  Because the stub never zeroes the counter, actor-15 script slot 0 parks at
  `currentIP = 0x68F` forever and the player freezes.

Measured at the freeze: `D_800ADB64 = 0xFF` (main.c:599 reset it after the stub
returned), `D_8004F350 = 1`, `D_80059460 = 0` (mode 0), `g_Menu = NULL`, actor-15
slot 0 = `(IP 0x68F, scriptId 2)`. Successive framebuffer captures are
byte-identical — the field is frozen, no menu overlay is drawn. `func_800799D4`
is hit exactly once. This is the first menu render target with a live
reproduction: porting `func_800799D4` (retail body at asm `0x800799D4-0x8007A448`)
is what loads the overlay, runs `MenuMain`, and clears `D_8004F350` so WAIT_MENU
advances.

UPDATE (validated in working tree, pending commit): func_800799D4 is now
decompiled (near-match, 653/671, single register-spill residual) with a
port-side sizing fix, and the stub gate above is CLEARED end-to-end on the
same repro. Two findings supersede the paragraph above once that lands:

1. Retail sizes the overlay buffer as the fixed-map gap
   `(D_800ADB30 & 0xFFFFFF) - 0x1C5008` (overlay region 0x801C5000, next
   allocation above it). Against the port's host-pointer `D_800ADB30` that
   yields a ~5MB bogus size -> `HeapAlloc` fails -> `GameHandleError(130)`.
   The host-correct size is what the buffer actually receives — the decoded
   size of menu overlay file `(menu id + 5)` — the same derivation retail's
   own `D_8004F370` branch and `func_80077884` (main.c:171) use.  Fixed under
   `XENO_PC_PORT` at both fixed-map-idiom sites in the function (overlay
   alloc, and the latent `D_800ADB20` restore at the tail).  Matching form
   verified byte-identical before/after (the ifdef does not leak into mwcc).
2. With the sizing fixed, one authentic talk completes the full round trip in
   ~7s: alloc OK, `MenuMain` RUNS (`g_Menu` non-null, measured `0x5813b0`),
   `MenuExecute` mode 0 hits stub `func_801C62A8` (no menu visuals — the real
   render gate), post-menu tail runs (`func_800798BC`/`func_800A2488` stubs
   log), `D_800ADB64 -> 0xFF`, `D_8004F350 -> 0`, and actor-15's script
   advances past `0x68F` to its STOP.  The mode-0 body `func_801C62A8` is the
   next menu porting target, same repro.

SCOPE CORRECTION (func_801C62A8 is NOT a one-function port): reading the retail
asm from `disc/menu.bin` at offset 0x12A8 shows `func_801C62A8` is an
84-instruction **dispatcher**, not a self-contained render.  It calls setup
`func_801C5F10`/`func_801C7B0C`, sets `g_Menu->shouldDrawMenu` (+0x327) and
`unk32A` (+0x32A), then switches on the menu-mode byte `D_80059460` and
dispatches to overlay-internal sub-functions, always finishing at
`func_801C5FE4`.  The mode-0 path (`D_80059460 == 0`, the map005 repro) calls
`func_801C5F10 -> func_801C7B0C -> func_801D2D38 -> func_801C55A0 ->
func_801C5FE4` — **all five are unported overlay code**, and the actual window
/ content drawing lives in them, not in `func_801C62A8`.  So porting
`func_801C62A8` alone renders nothing and exposes those five as the next gates.

Two premises the prior report carried were wrong, corrected by reading the code:
1. `func_801C62A8` is the ENTRY of the mode-0 render call tree, not "the last
   gate before pixels."
2. **The normal-menu overlay has no decomp infrastructure at all.**  It lives
   in `disc/menu.bin` (archive dir 0x10 / file 5, VRAM base 0x801C5000, 153864
   bytes, stored uncompressed — byte-identical to `disc/menu.bin`).  Unlike
   `member_change_menu`/`shop_menu`, `menu.bin` is NOT in `gears.toml`'s overlay
   list, has no `config/menu.yaml`, no `asm/menu/` split, no `src/menu/`, and no
   matching target.  There is therefore nothing to `objdiff {}` against and no
   `asm/menu/func_801C62A8.s` to `INCLUDE_ASM` — neither the matched-decomp path
   nor the d88f13c coexistence pattern is available until the overlay is brought
   up.

Rendering the normal menu is thus a two-part project, not a single-function
pass: (a) bring up the `menu.bin` overlay TU (add to `gears.toml`; write
`config/menu.yaml` mirroring `config/member_change_menu.yaml` — VRAM 0x801C5000,
gp 0x80059170; seed symbols; splat-split; linker + gears integration; matching
baseline), then (b) decompile `func_801C62A8` + its mode-0 callee tree
(`func_801C5F10`, `func_801C7B0C`, `func_801D2D38`, `func_801C55A0`,
`func_801C5FE4`, and their transitive callees — the draw code).  The map005
nested-X repro validates each layer (does the window/contents draw yet?).
Evidence: proven (retail asm at menu.bin:0x12A8 read in full; overlay identity
byte-verified against disc/menu.bin; build-config absence confirmed).

New downstream defect, deterministic on the same single-tap repro: after the
menu round trip, field re-entry asserts in `func_800248D4`
(`src/slus_006.64/system/temp1.c:968`) — the port's sprite-animation VM
implements only the >=0x80 opcode dispatch, and post-menu re-entry drives an
actor animation into the sub-0x80 frame/delay fallthrough.  Observation
consistent with the stubbed `func_800A2488` party-sprite reload: the
after-cycle frame restores NPC sprites but the player sprite is missing
(`scratchpad/map005_aftercycle.png`).  Hypothesis only — trace before porting.

Reusable nested-X harness (no Wayland focus fight): `Xvfb :99 -screen 0
1024x768x24`, launch the port with `DISPLAY=:99`, then drive authentic input
with `xdotool windowfocus --sync <win>; xdotool keydown/keyup <key>` (plain
XTEST to the focused window updates SDL's `SDL_GetKeyboardState`; `--window`
synthetic events do NOT and are silently dropped). `scratchpad/run_map005.sh`,
`scratchpad/map005_drive.sh` (empirical d-pad calibration + greedy walk to
actor 15), and `scratchpad/map005_attach_probe.gdb` reproduce the capture.

Repro: `Xvfb :99 -screen 0 1024x768x24 &`; `XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0
XENO_FIELD_MAP=5 XENO_FIELD_ENTRANCE=0 SDL_VIDEODRIVER=x11 DISPLAY=:99
pc_port/build_native/xeno-port`; run `scratchpad/map005_drive.sh` to walk+talk;
attach `scratchpad/map005_attach_probe.gdb` — actor-15 slot 0 sits at IP 0x68F,
`D_8004F350 == 1`, `[stub] func_800799D4` appears once in the port's stderr.
Evidence: proven (authentic input, measured freeze state, frozen framebuffer)
Last verified @ c08aaa1

## Member-change/shop menus are state-gated on all 730 maps; normal/load menus are cold-boot reachable

The whole-archive field-script reachability scan proposed by the earlier
surveyed-shop-branches entry is done and authoritative (730/730 maps decoded,
102,382/105,830 routines walked, 3.26% abort rate, all degenerate-scan guards
passed). It corrects two earlier records:

**Correction 1 — there are EIGHT menu-request opcodes, not two.** Prior work
tracked only `FE 56`/`FE 58`. The full set in `g_FieldScriptVMHandlers2`
(dispatch chain proven from this repo: `g_FieldScriptVMHandlers[0xFE]` →
`FieldScriptVM2Run` at `src/field/main/misc8.c:2758` →
`g_FieldScriptVMHandlers2` → handlers at `src/field/main/misc11.c:286-335` set
request `D_800ADB64` → `src/field/main/main.c:595` → `func_800799D4` (retail
asm `80079E40`: `D_80059460 = D_800ADB64 & 0x7F`, `jal MenuMain`) →
`MenuExecute` at `src/slus_006.64/system/menu.c:236`):

- `FE 55` (2B) mode 0 normal menu; `FE 56 aaaa` (4B) mode 1 member-change;
  `FE 57` (2B) mode 2 load-game; `FE 58 aaaa` (4B) mode 3 shop;
  `FE 59 aaaa` (4B) mode 4 normal+`ChangeGameState(1)`; `FE 5A aaaa` (4B)
  mode 5 overlay; `FE CF aaaa bbbb` (6B) mode 1 member-change + map jump;
  `FE DA` (2B) mode 6.
- Noah's `fieldScriptOpcodes_EX` table does NOT register 0x56/0x58/0xCF/0xDA;
  this repo's asm is the only authority for those. The normal menu also opens
  by button press (`src/field/main/main.c:601`, pad bit 0x10 → request 0x80),
  engine-level, gated only by `D_800B21D0` — independent of scripts.

**Correction 2 — "no menu is cold-boot reachable" was too broad.** The split:

- Member-change (`FE 56`/`FE CF`) and shop (`FE 58`): STATE-GATED on all 730
  maps, no exceptions. Every site is behind scenario-var-0/map-flag/party
  checks, a state-gated scheduling chain, or a state-conditional actor-disable
  in an init/manager routine (map205's member-change NPC and map290's
  party-select scene both fall to that last class). The earlier 7-map hand
  audit generalizes. Reaching these authentically requires story state
  (scenario var 0, map flags, and party composition restored through the
  `g_pGameState+0x1930` script-memory block) or an explicitly labeled
  menu-test scaffold — not an ad hoc flag poke.
- Normal menu (modes 0/4/5) and load-game menu (mode 2): UNGATED at 93 sites.
  91 are talk routines (walk up, press Circle); 88 of those have no soft
  condition beyond the talk. Cleanest live target: map005 actor 15 talk
  routine, byte-verified `FE 99 00 | FE 55 | FE 87 | 00` (set save-arg, open
  normal menu, wait menu, stop). Alternative: map490 script 0 routine 1 opens
  the load-game menu from an auto routine behind a pad-input check.

The 12 raw `FE 5x` byte pairs in code no modeled entry reaches (e.g. map247's
three copy-pasted dialog-menu blocks with only the first referenced) are
accounted as dead leftover variants; none is a shop opcode, and each is listed
with hex context in the scan JSON.

Scan artifacts (untracked, session convention): `scratchpad/scan_field_script_menus.py`
(scanner, evidence citations inline), `scratchpad/build_length_model.py` +
`scratchpad/length_model.json` (per-opcode length/control-flow model,
this-repo-first with Noah cross-check), `scratchpad/full_scan.json` +
`scratchpad/full_scan_stdout.txt` (full site table and health).

Repro: `python3 scratchpad/scan_field_script_menus.py --json /tmp/rescan.json`
(3s over `disc/disc1.bin`); health line must show 730 maps, abort ≤5%, and the
site table splits UNGATED 93 / STATE-GATED 342 / PRESENCE-GATED 20 /
GATED-SCHEDULING 5 / INIT-DISABLED 1.
Evidence: proven (static scan; dispatch chain and both member-change survivors
hand-verified at byte level against this repo's asm)
Last verified @ e402f79

## Unported member-change and shop menu overlays

Both menu `misc.c` TUs now compile after the retail-proven `POLY_FT4` window
border correction and the `ShopMenuBuyMenu` declaration fix. Their compile
errors had been masking 15 link-reachable `INCLUDE_ASM` dependencies: three in
the member-change overlay and twelve in the shop overlay. Matching ELFs now
exist and there is no port-symbol collision. Both TUs now compile and link
normally with generated oracle stubs. Port-only routing reaches the real menu
entry bodies; live coverage of the 15 inner stubs now awaits an authentic shop
state as tracked above.

Repro: compile both menu TUs with the port GFLAGS/INC and compare their undefined
symbols against their `INCLUDE_ASM` declarations, or remove the holds locally
and verify that the typed stub manifest regenerates and links.
Evidence: proven
Last verified @ 5b68551

## Work-list runtime routing: decomp source vs. host-safe port override

`src/slus_006.64/system/work_list.c` now compiles, but it cannot be linked
alongside `pc_port/src/work_list_port.c`: nine definitions overlap. The port
file is a deliberate, partial host-layout override, not a full replacement.
It uses PSX-layout 0x1C entries with 32-bit stored pointers because the original
decomp layout is unsafe for the 64-bit host; it was added to restore the
runtime-critical work-list paths that were previously stubbed.

The runtime-layout decision is now made: keep `work_list.c` excluded as
matching/reference source and make `work_list_port.c` the canonical native
owner. Exclusion alone is not completion; remaining exports must be ported into
the packed-u32 implementation as live paths require them. See
[`Port-Coexistence-Architecture`](docs/wiki/Port-Coexistence-Architecture.md).

Repro: compile `src/slus_006.64/system/work_list.c` with the port GFLAGS, then
compare its defined symbols against `pc_port/src/work_list_port.c`; the shared
definitions produce duplicate-definition link errors if both objects are linked.
Evidence: proven (source comments, symbol comparison, and runtime-recovery
history)
Last verified @ fdd86e7

## Port-only source compilation is not fail-closed

The game-TU compile loop aborts on unexpected failures, but the port-only loop
only prints `FAILED` and continues. A failed `game_overrides.c`,
`archive_port.c`, `work_list_port.c`, data source, or other port source is
omitted from `GAME_OBJS`; the trial link can then satisfy its missing symbols
with generated stubs. `port_main.c` likewise reports a compile failure without
aborting or deleting a prior object, allowing a stale entry object to be linked.

Repro: inspect the port-source loop and `port_main.c` compile command in
`pc_port/build_port.sh`, or induce a temporary compile error in an isolated
worktree. The driver reaches the trial link instead of exiting at the failed
compile.
Evidence: proven (control-flow inspection)
Last verified @ fdd86e7

## Unimplemented sprite-animation opcodes in func_800248D4

`func_800248D4` is the **sprite animation-script dispatcher**, not the field
script VM. Its dedicated unimplemented set is now `0x85, 0x8E, 0x98, 0xC8,
0xD4, 0xE2, 0xFA`. Opcode `0xBE` was implemented from the shared retail
`0x80` handler at `0x80024A84-0x80024B9C`.

The strict static scanner in
`tools/scripts/psx/scan_field_anim_opcodes.py` decoded all 730 field maps,
3,234 per-map sprite packages, and 16,382 animation entries with zero aborts.
Only the now-implemented `0xBE` is reachable from a per-map animation entry:
seven distinct sites in Map047 package 0 animations 0/1/2, Map048 package 0
animation 2, and Map334 package 0 animations 0/1/2.

The other seven are **not reachable in any per-map animation package**. Do not
call them unused: global party, battle, and special-animation packages were not
part of this field-map scan and remain a real coverage gap.

Repro: `python3 tools/scripts/psx/scan_field_anim_opcodes.py --json
scratchpad/field_anim_opcode_scan_all.json`; expect 730 maps, 3,234 packages,
16,382 fully-decoded entries, zero aborts, and seven reachable `0xBE` sites.
Evidence: proven for per-map packages; global animation-package coverage open
Last verified @ 4d7cb57

## CompMatrix PsyQ decomp match

Audit is DONE (semantics verified vs retail 0x8004931C-0x80049478).
The MATCH is outstanding — handwritten GTE sequence, codegen-focused work.
Evidence: proven (audit); match not attempted
Last verified @ ed61298

## Map015 entrance 0 — func_8008399C assertion

Repro: launch Map015 entrance 0; assertion fires, map not runnable
Evidence: observed (surfaced during ABR scene sweep)
Last verified @ ed61298

## LIBGTE.C invalid UTF-8 byte

Blocks patch-editor modification of the file. Maintenance item.
Repro: iconv -f utf-8 -t utf-8 < pc_port/extern/PsyCross/src/psx/LIBGTE.C > /dev/null
Evidence: proven
Last verified @ ed61298
