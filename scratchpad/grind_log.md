# Grind Log

Append one entry per completed task. Do not edit past entries.
Newest entries at the bottom.

---

## Template (copy this for each entry)

### [YYYY-MM-DD HH:MM] <one-line task title>
- **Hypothesis:**
- **Scope:** <files/functions touched, or "investigation only">
- **Change made:** <smallest bounded change, or "none">
- **Build result:**
- **Runtime result:**
- **Proven:** <what evidence actually shows>
- **Not proven / still open:**
- **Committed:** <commit hash, or "no — validation didn't pass" / "no — investigation only">
- **Stop reason (if stopped early):**

---

### [2026-07-12 00:05] Seed standing rules + grind log
- **Hypothesis:** Persistent alwaysApply rule + grind log will keep decomp constraints across sessions.
- **Scope:** `.cursor/rules/xenogears-decomp.mdc`, `scratchpad/grind_log.md`
- **Change made:** Created both files from user-provided templates (no game code).
- **Build result:** N/A
- **Runtime result:** N/A
- **Proven:** Files present on disk with `alwaysApply: true` and append-only log template.
- **Not proven / still open:** User still needs Cursor UI settings (approval for destructive cmds, Max mode, spend limits).
- **Committed:** no — investigation/setup only (untracked `.cursor/` + `scratchpad/`)
- **Stop reason (if stopped early):**

---

### [2026-07-12 00:10] Commit + push standing rules for cloud mode
- **Hypothesis:** Cloud agents need the rules/log on a remote they can clone.
- **Scope:** `.cursor/rules/xenogears-decomp.mdc`, `scratchpad/grind_log.md` only
- **Change made:** Committed those two files; pushed to `fork` (origin denied).
- **Build result:** N/A
- **Runtime result:** N/A
- **Proven:** `d554474` on `fork/ai-private-main` (`Blizz127/xenogears-decomp-ai`).
- **Not proven / still open:** Not on `ladysilverberg/xenogears-decomp` (403). Branch now tracks `fork/ai-private-main`.
- **Committed:** `d554474` (this log entry itself remains local/uncommitted)
- **Stop reason (if stopped early):**

---

### [2026-07-12 00:40] A14 script 3: no camera-eye-override opcode near FE54
- **Hypothesis:** bit14 clamp-skip exists to protect a scripted eye-override near A14 FE54; if that opcode is missing/stubbed/not firing, raw follow-cam eye2.y=1491 leaks through.
- **Scope:** investigation only — A14 script 3 bytecode (OFF 585–~773), VM handler table, camera_movement / misc11 handlers. No code edits.
- **Change made:** none
- **Build result:** N/A
- **Runtime result:** N/A (prior capture: A14 executes FE54 then sticks @IP592 on `09 01 65` waiting A1 r5)
- **Proven:** Full A14 r3 disassembly from `captures/render_diag/map1_scriptfile_20260708.bin` @sig `fe542501…`: reachable CAM-related ops are only FE54@585 (`func_80093B10`, unk48|=0xC000) and FE53@649/705/771 (`func_80093AC8`, clear). No opcode in this script writes `g_CameraEye2` / eye movement dest / `g_FieldCameraMode`. Apparent 0x65/0x66/0x9A bytes are operands (routine id / warp arg / jmp target 0x029A), not insn starts. Near FE54 path is `FE54 → HideById(1) → Sleep → 09 actor1 routine5` only.
- **Not proven / still open:** Whether A1 r5 (or another actor) supplies a deliberate high eye under bit14; 0xE6 already ruled out for eye position. Redirect search to Actor 1 / routine 5.
- **Committed:** no — investigation only
- **Stop reason (if stopped early):**


### [2026-07-12 00:47] A1 routine 5: no camera-eye/mode-writing opcode
- **Hypothesis:** A1 r5 (cutscene body A14 waits on) contains a deliberate eye/mode write that bit14 is meant to protect; if missing/stubbed, raw follow-cam eye2.y=1491 leaks.
- **Scope:** investigation only — A1 routine 5 bytecode OFF 189–248 (routine table r5=189,r6=249), handler sizes from matched C/asm. No edits, builds, or runs.
- **Change made:** none
- **Build result:** N/A
- **Runtime result:** N/A (prior logs: eyeY=1491+C000 at f≈57 as A1r5 starts @189; sticks historically @211 op=57)
- **Proven:** Full length/BFS decode of r5 (60 bytes). **Zero** opcodes write `g_CameraEye2`, eye-movement buffers, or `g_FieldCameraMode`. Byte `0xE6` inside `57 80 1c 00 e6 fe … f0` is a **0x57 operand**, not opcode `func_80091A08` (corrects prior handoff). `0xAA` / later dialog are **r7+** (table r6=249, r7=252), outside r5. Onset window @189–211 is only warp/FE64/rotate/show/anim/sleep then 0x57 — no eye write.
- **Not proven / still open:** Whether retail mid-cinematic high eye is intentionally the unclamped follow-cam value; whether any *other* actor/system writes eye during the well interaction. Hypothesis that a scripted eye-override exists in A14 r3 or A1 r5 is **not supported** by bytecode.
- **Committed:** no — investigation only
- **Stop reason (if stopped early):**


### [2026-07-12 00:54] Retail well stills vs port black-void — visual sanity check
- **Hypothesis:** Existing retail CRT stills may show the same high/void camera as the port symptom (not a bug), or normal grounded framing (real divergence).
- **Scope:** read-only view of `scratchpad/retail_well_ref.png`, `retail_well_highcam.png`, and contrast `port_well_ref_pose.png`. No code/builds/runs.
- **Change made:** none
- **Build result:** N/A
- **Runtime result:** N/A
- **Proven:** Both retail PNGs are byte-identical and show a normal Lahan well shot: elevated oblique field cam, full grass/paths/well/NPCs/compass, no void. Port still shows black background with floating well/building fragments — does **not** match retail framing. Retail is not the mid-cinematic black-void state.
- **Not proven / still open:** Why port enters/stays in unclamped follow-cam+bit14 void while retail surface photo does not; next is other live actors / non-script camera paths or cinematic-completion timing, not “retail also voids.”
- **Committed:** no — investigation only
- **Stop reason (if stopped early):**


### [2026-07-12 01:03] g_FieldCameraMode + 0x99/0x9A/0xAC — stubbed but not on well path
- **Hypothesis:** Non-script camera mode / opcodes 0x99/0x9A/0xAC are the missing eye-write mechanism for the well void.
- **Scope:** read-only — `camera_movement.c` INCLUDE_ASM sites, `misc2.c` mode dispatch, retail asm for those three ops, map1 scriptfile scan (IPs via base 0x1084). No edits/builds/runs.
- **Change made:** none
- **Build result:** N/A
- **Runtime result:** N/A
- **Proven:** (1) 0x99=`func_8008FB98`, 0x9A=`func_8008FC4C`, 0xAC=`FieldScriptStartCameraMovement` all **INCLUDE_ASM stubs** (`camera_movement.c:58,60,166`). Asm: 0x99 sets `g_FieldCameraMode=1`; 0x9A transitions 0↔1↔2; 0xAC arms at/eye movement flags+deltas (IP+=4). (2) Matched C `func_80073230` (`misc2.c:552–666`): mode **0/2** = follow-cam `func_80072A38` (+clamp unless bit14); mode **1** = write `g_CameraEye2` from `g_CamEyeMovementCurrent`. (3) Well symptom eye is follow-cam → mode 0/2 path. Map1 decode: **no** 0x99/9A/AC in A14 or A1; only A0 r4 has 0x99 and A69 r30 has 0xAC. Stubs are a real gap elsewhere, **not** evidenced as firing in the well window.
- **Not proven / still open:** Full live-actor census at void onset (logs only prove A1+A14 non-idle). Whether void is simply bit14 stuck (cinematic incomplete) vs another actor’s eye write.
- **Committed:** no — investigation only
- **Stop reason (if stopped early):**


### [2026-07-12 09:15] Full live script-slot census during bit14 void window
- **Hypothesis:** Actors other than 1 and 14 may hold live named scripts during the unk48 bit14 / eye2y≈1491 window; prior A1/A14-only logs left that unproven.
- **Scope:** `src/field/main/misc2.c` (`func_8007554C`) — one TEMP-DIAG dump gated by `XENO_FIELD_TEST=1` + bit14, every 10 frames. Scratchpad repro helpers only. No camera/opcode changes.
- **Change made:** Temporary `[TEMP-DIAG slots]` printf enumerating all actors' non-0xFFFF script slots (actor, slot, scriptId, IP).
- **Build result:** LINK OK (`pc_port/build_native/xeno-port`)
- **Runtime result:** Capture `scratchpad/well_all_slots.log` (well teleport, f60–f400, unk48=0xC000, eye2y≈1491). Named live scripts (`scriptId!=255`) in f56–f338: **A1 script 5**, **A14 script 3**, **A19 scripts 6 and 7**. Many other actors retain idle leftover IPs with `id=255` (not named live).
- **Proven:** Full census of named live scripts in the void window is {1,14,19} only — not just 1+14. A19 is new vs prior ruled-out set. Quick bytecode scan of A19 r6 (`fe 13 10 80 7f 80 04`) and r7 (`26 05 80 fe 13 0f 80 7f 80 04`): **no** 0x99/0x9A/0xAC/FE54/FE53; FE13 + 0x26 + yield only — not an eye/mode writer on first look.
- **Not proven / still open:** Whether A19 FE13 has any indirect camera side effect (full decode next if needed); why bit14 stays set after A14 advances past 592 (this run reached A14 IP 748–768); non-script eye paths.
- **Committed:** no — diagnostic left dirty for review (user request)
- **Stop reason (if stopped early):**


### [2026-07-12 09:53] Extended well_all_slots capture: A14 re-entry loops forever
- **Hypothesis:** Prior f~401 cutoff ended mid Sleep on second-pass A1; extending past f2000 with per-frame sampling will show whether the loop terminates or repeats.
- **Scope:** RUN+CAPTURE only — existing misc2.c TEMP-DIAG (every-frame A1+A14); well_all_slots.gdb quit at f>2500; no game-logic edits.
- **Change made:** none to game logic; capture duration/cadence only (already present). Invoked via gdb.minimal (full `gdb` absent from PATH).
- **Build result:** pre-existing binary with TEMP-DIAG string present (build-verified earlier this session).
- **Runtime result:** DONE f=2501 unk48=0xc000; 9 A1 r5 passes; FE53@771 only once (f=332); bit14 never clears in dump; after SetBit path, ladder skips 748 forever.
- **Proven:** (1) A1 leaves Sleep@199 after exactly 90f every pass incl. pass2 f=339..428→429. (2) Auto-confirm is every-wait not once-only (misc2.c:1339-1354 / 5f1e799); passes 2–9 complete past dialog region. (3) A1 reaches End each pass (id=255,ip=247). (4) A14 advances 592→ladder every pass. (5) First ladder →748 = mem[0x2C8]&0x40 CLEAR (ConditionalJmp AND false→targ); later →619/666/722 = SET (fallthrough). (6) bit14 never permanently clear. Verdict: loops forever.
- **Not proven / still open:** exact flags_0x12 value at 0x09 release (dump lacks that field; End writes 0xFF); whether FE53 clears bit14 for 1–2 undumped frames between 771 and FE54 re-entry.
- **Committed:** no — capture/investigation only; user forbade commits.
- **Stop reason (if stopped early):**


### [2026-07-12 09:58] Anti-retrigger candidates A/B/C — read-only decode
- **Hypothesis:** Retail avoids A14 script-3 re-entry via (A) moving Fei out of AABB, (B) clearing flags0 0x2000, or (C) a stubbed opcode on the short-circuit path.
- **Scope:** investigation only — well_all_slots.log + map1 bytecode + src/asm. No edits/builds/runs.
- **Change made:** none
- **Build result:** N/A
- **Runtime result:** N/A (no new run). Prior log has no Fei XYZ fields.
- **Proven:** (A) 0x57@A1:211 lands (28,-282) INSIDE AABB x[-206..66] z[-350..-78] (enc_a14_aabb.log); same op every pass. (B) map1 has F8 SET 0x2000 at A14 r0 IP572 only; zero F8 clears of 0x2000; no `&= ~0x2000` in src/field. (C) stub `func_8009635C` via 0x8C on paths 722/666/748 only — stable 619 has no 0x8C and still loops. Short-circuit bytecode JMPs to FE53 (sampling may miss IP771).
- **Not proven / still open:** What retail uses instead (edge-trigger vs while-inside, other map systems); Fei XYZ not in 2500f slot log.
- **Committed:** no — investigation only
- **Stop reason (if stopped early):**


### [2026-07-12 10:00] func_8008237C force-gate — matched geometry, not latch
- **Hypothesis:** Infinite re-trigger is because func_8008237C is a stub returning 0, making force AABB fire every frame.
- **Scope:** investigation only — misc8.c C + asm/field/matchings/main/misc8/func_8008237C.s + force path in func_8008399C.
- **Change made:** none
- **Build result:** N/A
- **Runtime result:** N/A
- **Proven:** MATCHED (misc8.c:680-714; matchings asm). Returns -1 outside expanded AABB, 0 inside. Arg 0x10 = extraRadius. No sticky/hit-count state. Force-path call+bnez matches retail 80083C58-80083C60. Stub-returning-0 hypothesis FALSE.
- **Not proven / still open:** What retail uses to stop re-fire after script End while still inside (chain still open; 8237C is not it).
- **Committed:** no — investigation only
- **Stop reason (if stopped early):**


### [2026-07-12 10:08] scriptFlags bit23 (0x800000) — retail writer exists; not A14/well root cause
- **Hypothesis:** Port missing a writer for scriptFlags bit23 leaves `(flags0 & 0x00A20000)==0` forever on A14, causing infinite script-3 re-trigger vs retail.
- **Scope:** investigation only — retail asm (misc6.s DA70/DA98, whole-asm lui 0x800000+or+sw+0), handler table 3DF78.data.s, map1_scriptfile reachability, misc6/7 bit17/21 writers, misc.c F8.
- **Change made:** none (read-only; user accepted verdict)
- **Build result:** N/A
- **Runtime result:** N/A
- **Proven:** (1) Retail setter IS `func_8009DA70` opcode **0xCD** (asm misc6.s:37-47; clearer 0xCE / DA98). Port matched at misc6.c:47-54 (`scriptFlags_0x16`). Only field OR of 0x800000 into actor+0x00. (2) Prior "zero writers" was a false negative (bitfield write, not `|= 0x800000`). (3) Map1 reachable bytecode: **zero** 0xCD/0xCE; A14 none. A14 F8@572 only `|=0x2000`. No F8 case1 that sets bits 17/21/23. (4) Bit17/21 known writers do not run on A14 post-cutscene (no 0x2A/0x16 on A14; A14 0x10 walks are not 99AC0 case-1). (5) Therefore `0xA20000` gate stays open for A14 in retail too — mask vestigial for this scene; anti-retrigger elsewhere.
- **Not proven / still open:** What actually stops retail A14 script-3 re-entry while Fei remains inside the force AABB.
- **Committed:** no — investigation only
- **Stop reason (if stopped early):**


### [2026-07-12 10:15] Post-0xA20000: force gate census + +0x74 C/asm mismatch
- **Hypothesis:** After ruling out bit23/0xA20000, retail anti-retrigger of A14 script-3 is some other gate in func_8008399C / call chain, or a +0x74 interact latch.
- **Scope:** investigation only — misc8.c func_8008399C/84158, asm 80083A68–80084154, map1 A14 r3 + A1 r5 bytecode, enc_a14_*.log flags.
- **Change made:** none
- **Build result:** N/A
- **Runtime result:** N/A (prior logs only)
- **Proven:** (1) Force script-3 gates are only: flags0&1, player+0x74==i, Y band, AABB 8237C, !confirm→0xA20000, already-running scriptId==3. Call gate: encounters==0 && D_800B21CC==0. (2) Port **clears** +0x74 on match (misc8.c:1426-1428); retail **only skips** (80083A78-80083A84 → 84090 with scriptId=FF, no sb). (3) A14 flags4=0x4000a00 lacks 0x80/0x180 — pure flags0 0x2000 force; 8399C never writes +0x74. So +0x74 latch is not armed for A14 by the force path itself. (4) A1 r5 is linear (189…214 End): every 0x09 re-call re-SHOW + 0x57 to inside AABB — no second-pass exit. (5) Short-circuit 619 still FE54→F5→9C→…→FE53→00 — no disable/clear 0x2000.
- **Not proven / still open:** What stops retail re-entry. Next: runtime log Fei+0x74 / A14 flags0 / slot id across End→next fire; or whether incomplete 84158 (hasTarget always 0 → clears +0x74 every frame) matters for any well actor; or retail also re-enters and void is elsewhere.
- **Committed:** no — investigation only
- **Stop reason (if stopped early):**


### [2026-07-12 10:20] Runtime latch probe: Fei+0x74 / a14f0 / B21CC across well re-entry
- **Hypothesis:** If Fei+0x74 ever equals 14 (or B21CC/a14f0 mask bits change) across End→refire, that latch is the missing anti-retrigger; if all stay inert, those gates are dead for A14 on the port.
- **Scope:** TEMP-DIAG extend in misc2.c (XENO_FIELD_TEST only); scratchpad/well_latch_probe.{gdb,sh,log}. No gameplay logic change.
- **Change made:** TEMP-DIAG prints fei74/a14f0/B21CC each bit14 frame; probe run to f900.
- **Build result:** LINK OK (run_build_port.sh)
- **Runtime result:** DONE f=901 unk48=0xc000 eye2y=1491. diag lines=844. fei74_not_FF=**0** (always 255). a14f0 unique=**{0x21b0}** only. B21CC unique=**{0}**. Re-entry visible: A14 id=3 ip 771→589 within ~2f (f332→334, f617→619, f893→895) without a dumped id=255 gap.
- **Proven:** On the port well void path, interact latch +0x74 never arms for A14; flags0 never gains 0xA20000 bits; D_800B21CC never blocks 8399C. Anti-retrigger is not these latches (at least not as implemented/armed here).
- **Not proven / still open:** Whether retail arms a latch the port never writes; or retail also re-enters and the void is a re-entry-body/camera divergence. Next: retail-vs-port comparison of post-FE53 camera/script, or find who (if anyone) should clear A14 0x2000 / move Fei after first completion.
- **Committed:** no — diagnostic only (TEMP-DIAG must be removed/gated; not a fix)
- **Stop reason (if stopped early):**

### [2026-07-12 10:30] step.y / 0x86 landing probe — STOPPED (dirty tree)
- **Hypothesis:** N/A — task not started.
- **Scope:** intended: static scan Fei jump anim + TEMP-DIAG step.y/gravity/PC/0x57; forbidden: opcode impl.
- **Change made:** none
- **Build result:** not run
- **Runtime result:** not run
- **Proven:** Tree is not clean at claimed c184902. HEAD=`58a57cc`. Dirty: `src/field/main/misc2.c` (leftover latch TEMP-DIAG +63), `scratchpad/grind_log.md`. SHA `c184902` not in this clone. Stop condition fired before probe.
- **Not proven / still open:** Which of (a)-(f) keeps step.y negative / 0x86 waiting. Needs user call: revert latch diag, replace in place, or locate c184902.
- **Committed:** no — stopped early
- **Stop reason (if stopped early):** Unexpected dirty files + HEAD mismatch vs brief (c184902).

### [2026-07-12 10:40] Room/BG draw stub census — prediction falsified for floor/walls
- **Hypothesis:** Missing floor/walls/skybox at well void = stubbed room/BG draw or stubbed world transform.
- **Scope:** read-only — stubs.c grep, asm/C draw chain, ACTIVE_HANDOFF cullcam captures. No edits/builds/runs.
- **Change made:** none
- **Build result:** N/A
- **Runtime result:** N/A (reused prior cullcam_camz_stock.log / handoff)
- **Proven:** (1) Main per-frame room path is NOT stubbed: func_8007554C → func_80074108 (BG quads) + func_800748E8 (model actors) + FieldRenderQuad / func_8002C700 — all decompiled/matched C. (2) Static world transform IS applied: FieldComputeSceneMatrices (misc2.c:52-75) + func_800748E8 TR-add (misc2.c:1511-1521; asm 80074EDC-80074F18). (3) At eye2.y=+1491 well pose, model walkers emit≈66/1486 (~4.4%) with nclip_backface=967 + flag=343 (captures/render_diag/cullcam_camz_stock.log f=1+); fragments = surviving emits. (4) Normal Map1 free-play renders full town (handoff Jul 9) — draw path works at clamped cam. (5) Stubbed on secondary path only: func_800273C4 (horizon/line-scroll, gated D_800B00B2; setter opcode func_8008A148 also stubbed); func_801E7D14 (overlay when D_800B2264≠0). Matrix stubs RotMatrixZYX/SetMulMatrix not on floor path.
- **Not proven / still open:** Whether retail mid-cinematic at +1491 also sparsifies (no retail capture at that pose); whether Map1 arms D_800B00B2; which single walker guard drops ground quads vs buildings at the live well cinematic.
- **Committed:** no — investigation only
- **Stop reason (if stopped early):** Tree still dirty (misc2.c latch TEMP-DIAG + grind_log) at HEAD 58a57cc ≠ claimed c184902; proceeded read-only only.

### [2026-07-12 14:43] Restore retail trig entry-point semantics for the Lahan well camera
- **Hypothesis:** The port's conventional PsyCross `rsin`/`rcos` linkage does not preserve the retail executable's reversed entry-point semantics, making the FE54-unclamped well camera use sine where retail uses cosine and placing the eye below the floor.
- **Scope:** `pc_port/include_shim/psyq/libgte.h`; temporary runtime/capture probes and this grind-log entry. Pre-existing `src/field/main/misc2.c` TEMP-DIAG was used but not modified.
- **Change made:** Added game-side inline remaps so decomp `rsin` calls PsyCross `rcos`, and decomp `rcos` calls PsyCross `rsin`; PsyCross itself remains conventional.
- **Build result:** `distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp && ./pc_port/build_port.sh'` → LINK OK (`pc_port/build_native/xeno-port`).
- **Runtime result:** Deterministic Map1/entrance6 well trigger at Fei `(57,-1,-110)` now reports `unk48=0xC000`, `eye2.y=-914` (before: `+1491`). `scratchpad/well_trig_fix.png` is a full bright 640x480 field frame; mean RGB rose from 0.0364 in the before capture to 0.4461. Four 240-frame held-direction Map1 smokes all reached DONE with movement and `unk48=0` outside the trigger.
- **Proven:** Retail `rcos` at `asm/slus_006.64/2FF38.s:108-116` reads `D_800523F0`, whose stream begins `0,6,13...` (sine); retail `rsin` at lines 118-126 reads `D_800523F2`, whose stream begins `0x1000,0x1000...` (cosine), per `asm/slus_006.64/data/3F290.sdata.s:7389-7404`. The well camera calls those exact named entries at `asm/field/matchings/main/misc2/func_80072A38.s:94,122`. Preserving that retail ABI removes the below-floor/black-frame symptom without changing FE54 or the terrain-clamp gate.
- **Not proven / still open:** No whole-game regression sweep was run beyond the exact well repro and four Map1 movement smokes. Existing unrelated/in-progress diagnostics remain dirty; inventory/audio stubs in the well reward sequence are separate gaps.
- **Committed:** no — validated locally; commit not requested and the pre-existing worktree contains in-progress diagnostics.
- **Stop reason (if stopped early):**

### [2026-07-12 14:54] Restore retail PsyQ rand sequence in the native port
- **Hypothesis:** Because `build_port.sh` excludes `src/slus_006.64/psyq`, native field scripts bind `rand()` to glibc instead of Xenogears' 32-bit PsyQ LCG, changing the well reward branch range and sequence.
- **Scope:** `pc_port/src/psyq_compat.c::rand` and its 32-bit seed storage only.
- **Change made:** Added the retail `seed = seed * 0x41C64E6D + 0x3039` update with 32-bit wrap and return `(seed >> 16) & 0x7FFF` at the existing PsyQ compatibility boundary.
- **Build result:** Native `./pc_port/build_port.sh` in `xenogears-dev` → LINK OK.
- **Runtime result:** With seed zero, live native calls returned `0, 21468, 9988` and stored seeds `0x00003039, 0xD3DC167E, 0xA70427DF`, exactly matching retail. The Map1 well capture still completed at frame90 with `unk48=0xC000` and `eye2.y=-914`.
- **Proven:** Retail asm `asm/slus_006.64/matchings/psyq/libc/rand.s:1-13` and matched C `src/slus_006.64/psyq/libc.c:36-42` use the implemented LCG. Before this change `nm -D pc_port/build_native/xeno-port` showed `rand@GLIBC_2.2.5`; after it, the executable owns the retail-compatible symbol and deterministic sequence.
- **Not proven / still open:** The well's opcode 0x8C inventory-add path still reaches stubbed inventory helpers, so correct random selection does not yet guarantee the selected reward is stored.
- **Committed:** no — validated locally; broader cleanup/reward work remains in progress.
- **Stop reason (if stopped early):**

### [2026-07-12 15:17] Restore the retail well-reward inventory add cluster
- **Hypothesis:** The well dialogue/scenario flags advance, but opcode `0x8C` cannot store its three rewards because `func_8009635C` and its four category/slot helpers are native stubs; additionally, category-0 duplicate lookup incorrectly returns slot zero instead of the matched slot.
- **Scope:** `src/field/main/misc10.c::func_8009635C`; `src/field/main/misc11.c::{func_80094E8C,func_8009501C,func_800950A0,func_80095124,func_800951B8}`; their `0x1D0..0x22B` jump-table ownership in `config/field.yaml`; temporary GDB validation only.
- **Change made:** Transcribed all five missing retail functions, corrected `func_80094E8C` to `return i`, and extended `misc11`'s generated rodata span through `0x22C` so its four compiler-emitted jump tables replace the former raw copies.
- **Build result:** Native port build → LINK OK. `make check` now compiles and links all four matching targets, including `field.elf`; the final repository-wide SHA check still reports the pre-existing SLUS and field mismatches (member-change and shop pass), so a global matching claim is not made. All six changed functions retain the exact retail sizes (`0x50`, `0x84`, `0x84`, `0x94`, `0x94`, `0xB0`). The five `misc11` functions and four jump tables match the retail instruction/data layout; the idiomatic `func_8009635C` C preserves the retail prologue, calls, saved-register assignment, block order, behavior, and `0xB0` size, but its equivalent compiler-scheduled shared-store tail differs in 11 linked bytes.
- **Runtime result:** Live helper test proved all five category ID/quantity offsets; fresh item/accessory adds; duplicate category-0 item in slot 7 increments `98→99` and remains capped; a full 150-slot item inventory returns `-1` and neither endpoint is overwritten. In the real Map1 actor-14 sequence with field-test auto-confirm, rewards `0x003D` and `0x0002` fired at frames 302/587, occupied item slots 0/1 at quantity 1, and set scenario `0x02C8` bits to `0x00C0`. With only those prior-reward bits preseeded (RNG unchanged), the next favorable retail roll called `0x025A` at frame 469, stored accessory ID `0x5A`/quantity 1, and advanced the bits to `0x01C0`. Camera remained `eye2.y=-914` throughout.
- **Proven:** The three real grants are A14 routine-3 IP `0x02F7→0x003D/bit6`, `0x02DD→0x0002/bit7`, and `0x02A5→0x025A/bit8`. Opcode `0x8C` now mutates the correct inventory category, honors quantity 99, preserves full inventories, and only then allows the authored scenario-bit progression in the observed sequence.
- **Not proven / still open:** Repository-wide checksums remain nonmatching outside this bounded cluster. The well still immediately re-enters in the automated port harness after control should return; retail footage proves reactivation is manual, so that latch/overlap issue is separate and still open. Reward/audio playback remains behind the port's larger sound backend gap.
- **Committed:** no — build/runtime validated; commit not requested and unrelated user scratch artifacts remain.
- **Stop reason (if stopped early):**

### [2026-07-12 15:20] Retail-video oracle for well camera motion and reactivation
- **Hypothesis:** The slight zoom visible during Fei's well jump is a scripted projection/FOV change.
- **Scope:** Read-only frame sampling of `https://youtu.be/PWtGUF2cx3U?t=1190` plus a temporary native camera/actor trace; no gameplay edit.
- **Change made:** none.
- **Build result:** N/A for the read-only comparison.
- **Runtime result:** Port trace through the jump: Fei Y `-1→-94→+14→-1`; camera at/eye follow the arc while `sceneScrZ=768` and `sceneScale=4696` remain constant. Retail at ~19:50.6–19:51.7 shifts the 720p scene about +145 px right/-160 px up with parallax and only ~2% rim growth; it remains bright/stable, then follows Fei back out at ~19:56.4–19:57.3.
- **Proven:** The retail visual cue is target/rig follow with fixed-looking projection, not a uniform zoom. Retail restores control/compass around 20:13 with Fei southeast/right of the well, and the walkthrough manually activates the next attempt; an unconditional post-cutscene loop is not retail behavior.
- **Not proven / still open:** The exact missing port-side suppression mechanism after Fei lands at `(28,-1,-282)` remains under audit (`ActorData+0x74`/overlap handling is the leading bounded area). Pixel-exact framing still needs synchronized retail/port frame pairs.
- **Committed:** no — investigation only.
- **Stop reason (if stopped early):**

### [2026-07-12 15:22] Final Lahan regression matrix + well re-trigger boundary
- **Hypothesis:** The apparent repeat loop was caused by the multi-frame teleport probe holding Fei inside the well, and the final stub regeneration might still shadow the newly decompiled inventory functions.
- **Scope:** final native binary symbol audit; one-shot well trigger; existing well, talk, movement, alternate-entrance, Map0, and Map1→Map15 probes. Read-only audit of `func_8008399C`/`func_80084158` against retail asm.
- **Change made:** No additional gameplay change. Removed all three temporary inventory/camera GDB probes after recording their results.
- **Build result:** Fresh `./pc_port/build_port.sh` → LINK OK, 683 undefined symbols/236 function stubs. `func_8009501C`, `func_800950A0`, `func_80095124`, `func_800951B8`, and `func_8009635C` are strong native text symbols and absent from `stubs.c`; disassembly shows the real call/mutation body.
- **Runtime result:** Fresh final-binary reward recheck: `FINAL_REWARD_CALL f=302 arg=0x3D`, item slot0=`(0x3D,1)`, scenario=`0x40`, `eye2.y=-914`, with no inventory-stub hit. Well visual probe: `eye2=(1299,-914,-983)`, GL shot frame150. Talk: `TALK-FIRE` and DONE. Four-direction run: frames 60–560, four shots, camera target followed Fei, no signal/reload. Alternate entrances ent8/ent0 and Map0: timeout-only RC124, zero crash marks. Reload: Map1→Map15 `FIELDLOAD #2 frame=168`, textbox/fade initialized, three spawns, `POST_RELOAD_OK frame=200`.
- **Proven:** The immediate second well start is real even with a one-shot teleport: FE53 ends at Fei `(28,-1,-282)`, relative A14 `(+98,-68)`, latch `255`; FE54 restarts immediately at the same state. That point lies inside the retail 120×120 actor box and the +16 force expansion. The known C-only clear of player `+0x74` in `func_8008399C` is a real decomp mismatch but not this cause: A14's retail `flags0=0x21B0` includes `0x80`, so both retail and C reject it before `func_80084158`'s general latch writer.
- **Not proven / still open:** The retail-only suppression state around FE53 is still unidentified. An A14-specific edge latch would be speculative; next evidence must come from retail runtime state (global interaction gate, script-slot lifecycle, or another control-state writer). The exact one-line `+0x74` C/asm cleanup is safe but deliberately not bundled here because it cannot fix this well behavior and needs its own regression pass.
- **Committed:** no — no commit requested; changes remain separated by file/scope for review.
- **Stop reason (if stopped early):**

### [2026-07-12 15:35] Commit split + make check baseline + field.yaml isolation + camera mode-1 audit
- **Hypothesis:** (a) the dirty tree divides into four independent logical changes; (b) the make check failure is pre-existing at HEAD; (c) the field.yaml rodata move does not shift unrelated layout; (d) nothing reachable from the well cutscene arms camera mode 1.
- **Scope:** repo hygiene (baseline, isolated worktree builds, commit split) + read-only multi-agent camera-mode audit. No new gameplay edits of my own.
- **Change made:** committed the session's validated work as five commits (field.yaml 621d70f, misc10/11 inventory 7b37def, rsin/rcos shim 83ca8e8, rand LCG 23220ce, working rules 59ec60c) + this log entry.
- **Build result:** `make check` on clean 58a57cc: slus_006.64 FAILED (`229aa9d63118870802f34fc8afd2895948edbc6735d6fce720ebdbba18bc4a17`), field.bin FAILED (`f431a75a25d6623cb3ef08af6273c67d20c2cf90ed2ea71f969229f288f1852b`), member_change_menu OK (`3b9e2b890c27ae0de97fe343ac75cd35c05fe7f9605ae387d2166da0be78109c`), shop_menu OK (`7890e14bcabddcf85368de10783ecff8daa166dc5ac254e06db5e27959cab4cf`) — **pre-existing failure now proven** with all tracked changes stashed. A clean isolated build of field.yaml-only commit 621d70f produced a byte-identical baseline field.bin (`f431a75...`), proving no layout shift. Final `make check` has the same target statuses and unchanged SLUS/member/shop hashes; field.bin is `9600115766a1366e6cf5f2658b63a243023fe7b98b85cabe2cd895537a46c9c4`, exactly 21 individual bytes different from baseline, confined to `func_80094E8C` (the retail `return i` correction) and `func_8009635C` (equivalent tail scheduling). The other four helpers and all four jump tables are byte-identical, and the following raw `0x22C..0x267` span is unchanged (`48579c8180055e332a3cace4a7f54ebffe32bb44ba740d3493cecf2399db6234`). Old ownership `0x18+0x98` and new ownership `0x74+0x3C` both total `0xB0`, so nothing at or after `0x268` moves. Also characterized the pre-existing field.bin mismatch: built .field PROGBITS ends at the BSS boundary (24,904 bytes shorter than the on-disc file) and code sits ~0x3D48 below retail VMAs from early on — wholesale layout deficit, unrelated to today's changes.
- **Runtime result:** Fresh final source: native port LINK OK with all five inventory functions and `rand` as strong symbols. One-shot well run called reward `0x3D` at frame 302 and, on frame 303, item slot 0 was `(0x3D,1)` with `eye2.y=-914`. Independent camera capture reported `at2=(221,-33,96)`, `eye2=(1299,-914,-983)`, and a rendered frame at 150. Earlier regression matrix results from 14:43/14:54/15:17/15:22 remain applicable.
- **Proven (camera mode-1 audit; 4-agent workflow, 32/32 adversarially verified claims CONFIRMED):** (1) FE54=`func_80093B10` ORs 0xC000 unconditionally — ori before beqz, sw in the delay slot (`asm/field/matchings/main/misc11/func_80093B10.s:15-17`). (2) bit14 = disable follow-cam roof clamp; sole reader `func_80073230` (misc2.c:641, asm 800733B8); also set/cleared standalone by ops 0xB7/0xB8. (3) bit15 = "script owns camera": sole retail readers are two pad-input gates in `func_800726E8` (asm 80072874/80072918) that block ±45° cam-rotate; those branches are **omitted from the decompiled C** (misc2.c:231-295) — port fidelity gap, bit15 currently unread on the port. (4) The ONLY writer of g_FieldCameraMode=1 in the whole tree is opcode 0x99 `func_8008FB98` (also sets bit15, snapshots yaw/pitch/dist); 0x9A returns 1→0/1→2; 0xAC arms movement flags/deltas and initializes g_Cam{At,Eye}MovementCurrent=From — inert unless mode 1. All three INCLUDE_ASM (camera_movement.c:58,60,166) → auto-stubs on the port (stub returns 0 without advancing IP → script would spin, no crash). (5) Full map1 walk (source-verified opcode sizes — the prior walker had 30+ wrong entries; 283 routines, 0 unknown ops, raw-byte reconciliation complete): reachable 0x99/0x9A/0xAC exist ONLY in actor0 routine4 (map-entry camera routine, also callable via `07 00 64` from actor24 r2) and actor24 routine2 (talk cutscene). Well chain (A14r3→A1r5; A19 r6/7 leaf music cues) reaches NEITHER. Prior "0xAC in A69 r30" claim REFUTED (dead-code decode artifact). (6) Therefore retail runs the well cutscene in **mode 0** — the port's mode was never wrong; the eye divergence was the rsin/rcos entry-point swap (fixed 14:43, commit 83ca8e8). FE54's 0xC000 is the encounter-zone bundle, not scripted-camera protection.
- **Not proven / still open:** retail well re-entry suppression (separate line of work); the func_800726E8 bit15 input-gate omission (safe bounded future fix); pre-existing slus_006.64/field.bin layout mismatch.
- **Committed:** yes — 621d70f, 7b37def, 83ca8e8, 23220ce, 59ec60c, + this entry.
- **Stop reason (if stopped early):**

### [2026-07-12 17:02] Reclassify retail Lahan start and advance Map014 camera/animation frontier
- **Hypothesis:** The user's 10:19 retail timestamp is not an exterior Map001 spawn; it is the authored Map014 Fei-painting sequence, and the port skips or stalls it because extended field/camera handlers are still native stubs.
- **Scope:** Retail-video frame comparison; Map014 runtime tracing; exact/semantic decomp of FE66, FE4A, FE4B, FE27, FE19, camera rotation, and camera-movement start; two port-only PSX-layout guards. No default-boot target change and no guessed painting effect.
- **Change made:** Added exact `func_8008F558` (FE66 sound channel), `func_8008ACE8`/`func_8008A9AC` (FE4A/FE4B special-animation load/install), `func_8008B328` (FE27 distortion control), and `func_80091008` (camera vector rotation); added the full semantic FE19 party-removal handler and four-mode `FieldScriptStartCameraMovement`; preserved the retail sprite special-animation pointer at raw +0x4C and the complete 0xFC `FieldEffects`/0x4C distortion layout under LP64.
- **Build result:** Fresh native build LINK OK with 231 function stubs. Final `make check` status remains SLUS FAILED (`229aa9d63118870802f34fc8afd2895948edbc6735d6fce720ebdbba18bc4a17`), field FAILED (`542d8cccfad878f94ee1ca83e9028c4c714e324fa068ff9ad57bb0b03ce3121a`), member OK (`3b9e2b890c27ae0de97fe343ac75cd35c05fe7f9605ae387d2166da0be78109c`), shop OK (`7890e14bcabddcf85368de10783ecff8daa166dc5ac254e06db5e27959cab4cf`). SLUS/member/shop hashes are unchanged; the field hash changes because FE19 and camera-start are semantic/size matches but not byte-exact. FE66, FE4A, FE4B, FE27, and camera rotation match retail instruction layout exactly.
- **Runtime result:** Map014 cold boot no longer crashes in FE66/FE4A and the FE4A→FE4B archive transaction completes (`unk120=0x5c50c0`, archive index 1974). FE19 now advances past the absent-character-2 removal instead of freezing actor0. At frames 30/90/180 the room renders stably with camera `eye2=(771,-466,-1112)`, `at2=(115,-33,-455)`; the old camera helper changed the framing from the earlier incorrect static pose. The master script now reaches the next real frontier, stubbed opcode 0x99 `func_8008FB98`.
- **Proven:** Retail footage at 10:19 starts inside Fei's room, then pulls back/orbits before the `Whew! That about does it...` dialogue; normal port boot to Map001/entrance6 skips this sequence. Map014 owns the room shot. Distortion init/draw is a paired subsystem: enabling init alone would assert on frame 1, and it requires an unmigrated `D_800AEB24` table. FE19 at Map014 IP59 removes character 2; with party `(0,0,0)`, retail only DrawSyncs and advances, so it was a script freeze but not Fei's renderer.
- **Not proven / still open:** Fei is still absent in the frame-180 capture; opcode 0x99 camera-mode initialization is still stubbed; the distortion initializer/drawer and table are not migrated; the synthetic new-game state still supplies party `(0,0,0)`. Do not switch normal boot to Map014 until these are safe.
- **Committed:** yes — 6c45f9a, 7da4d38, a2734be, 7f788d8, 694f4fd, c90e8fb, 72e7d7b, 9d03825. Log entry pending its own evidence commit.
- **Stop reason (if stopped early):**

### [2026-07-12 17:14] Restore Map014's retail close-up, pull-back, orbit, and wide camera chain
- **Hypothesis:** Once opcode 0x99 enters script-owned camera mode, the restored 0xAC scheduler can execute Map014's authored camera chain; Fei's apparent absence is either occlusion in the easel framing or a real actor-load failure.
- **Scope:** Exact 0x99 camera-mode entry, semantic 0x9A exit/hand-off, exact distortion rectangle data migration, party-slot type-width correction, and late-frame Map014 GL/runtime capture. Distortion init/draw remains intentionally disabled as a pair.
- **Change made:** Decompiled `func_8008FB98` (exact 45-instruction retail sequence) and `func_8008FC4C` (retail-correct mode 0/1/2 behavior, close but not byte-exact); migrated the exact 60-byte `D_800AEB24` capture-rectangle table; corrected `D_8005A444`/`D_8006F990` declarations from u16 to retail s32 width.
- **Build result:** Fresh native LINK OK, 229 function stubs. Final `make check`: SLUS FAILED (`229aa9d63118870802f34fc8afd2895948edbc6735d6fce720ebdbba18bc4a17`), field FAILED (`1b76be1317328561400721b0dddc1f6381035dfaf7406a602f4c44189b23c7d4`), member OK (`3b9e2b890c27ae0de97fe343ac75cd35c05fe7f9605ae387d2166da0be78109c`), shop OK (`7890e14bcabddcf85368de10783ecff8daa166dc5ac254e06db5e27959cab4cf`). Status and the other three hashes retain baseline; field hash changes only with the new semantic decomp bodies.
- **Runtime result:** Cold Map014 now initializes camera mode 1 and runs multiple authored movements. Frame 180 close shot: `eye2=(372,-137,-202)`, `at2=(25,-50,-546)`, durations 31/31. Frame 240 pull-back settles at `eye2=(455,-140,-118)`, durations 0/0. Frame 360 begins the side orbit at `eye2=(651,-178,-490)`, durations 14/14. Frame 480 settles wide at `eye2=(-392,-294,-958)`, `at2=(111,-50,-459)`, durations 0/0. Captures visibly follow close painting/Fei → pull-back → side swing → high/wide easel view, matching the retail timestamp's staging. Final Map001/entrance6 regression smoke reached the field main loop and survived the full 20-second timeout (`RC=124`) without a crash.
- **Proven:** Fei is present, not missing: player actor 1/character 0 is visible at `(115,-1,-455)`, special animation index 1974 is installed/current, frame primitives are submitted, and his script waits normally in opcode 0xEF for both camera movements. The easel occludes him in the wide pose, as in retail. FE19 removes character 2 (Elly), not Fei. The s32 slot fix initializes both party-slot arrays as three distinct `(255,255,255)` words instead of `(255,255,0)`.
- **Not proven / still open:** The initial painting/distortion composite is absent because `FieldDistortionInitialize` and the active `FieldDistortionDraw` path remain unmigrated and must land together. Synthetic new-game party `(0,0,0)` is still upstream of Map014 and should be fixed in the missing new-game state rather than patched in the field. Normal boot remains Map001 until the complete sequence is safe.
- **Committed:** yes — 0b1b8ed, 18ff7fa, a181bf2, b171bed. This and the preceding evidence entry pending one log commit.
- **Stop reason (if stopped early):**

### [2026-07-12 18:35] Lahan opcode sweep and Map014 camera/cull boundary
- **Hypothesis:** Small, live Map001 script stubs around actor movement and camera state may still affect Lahan sequencing; the Map014 opening's sparse/garbled room might instead be a camera, matrix, or GTE-culling mismatch.
- **Scope:** Exact field VM handlers in `misc6.c`, `misc7.c`, `misc8.c`, `camera_movement.c`, `text_box.c`; read-only Map014 script/matrix/GTE audit. The in-progress distortion/build-script experiment was not changed.
- **Change made:** Replaced nine bounded stubs across seven commits with retail-matched C: dialogue readiness; Map001 actor arguments; well camera state; scripted movement clipping; camera yaw/pitch/distance state; movement-to-actor setup; and timed movement origin/interpolation (`func_800976A8`).
- **Build result:** Targeted matching-object build and exact objdiff for `func_800976A8` report 100%; final native `./pc_port/build_port.sh` completed with `LINK OK`.
- **Runtime result:** The previously committed movement-to-actor opcode was reached in Map001; the new timed sibling has no direct hit in the bounded entrance-6 trace, so no behavioral improvement is claimed from it alone. A static closure proves camera helper `func_800910C0` (opcode 0xEB) is not reached by the well or early Map001 flow.
- **Proven:** Map014's raw camera script, retail/Noah matrix order, and PsyCross RTPT values agree with direct fixed-point checks. Representative forward geometry projects exactly; FLAG-bit31 drops are genuine near-plane/behind-camera or winding culls. Do not relax camera/GTE culling. The remaining Map014 visual issue is downstream of emitted packets (texture/raster handling), not a camera/matrix/GTE defect.
- **Not proven / still open:** No retail-accurate Map014 composite has been restored, and the well's remaining re-entry boundary is separate. Experimental `src/field/effects/distortion.c` and `pc_port/build_port.sh` changes remain uncommitted and were intentionally excluded.
- **Committed:** `5ecb5d5`, `6ec8301`, `c9c6e1f`, `a6411f7`, `bc6e18e`, `d6e5104`, `1838561`; this log entry pending its own commit.
- **Stop reason (if stopped early):**

### [2026-07-12 19:50] Map014 framebuffer-feedback ordering + exact FT4 variant-0 dispatch
- **Hypothesis:** The Map014 painting composite is garbled because PsyCross defers polygons across `DR_MOVE`, captures a stale CPU/GL VRAM state, and then rotates its double-buffered VRAM texture after a split has already saved the old texture ID; a separate `0x0D` model-dispatch mismatch may also give room FT4s the wrong depth/cull path.
- **Scope:** Native-port renderer ordering in `pc_port/build_port.sh`; port-only model descriptor/walker in `pc_port/src/game_overrides.c`; read-only comparison of `FieldDistortionDraw` packet order to retail; isolated Map014/Map001 runtime captures. The in-progress matching-tree `src/field/effects/distortion.c` migration was inspected but not expanded in this cycle.
- **Change made:** (1) Moved the VRAM upload from `DrawAllSplits` to immediately before a textured `AddSplit` snapshots `g_vramTexture`, while retaining the DR_MOVE batch flush and immediate framebuffer materialization. (2) Restored retail `D_8004FE50[0x0D]` routing: variants 0/1 use `0x8002E268` semantics (AVSZ4 FT4), variant 2 remains `func_8002E688`. (3) Matched E268's actual gates: its `NCLIP` instruction occupies GTE latency and its result is never read; retail has neither an NCLIP cull nor an oversize cull in this walker, so both port-only gates were removed.
- **Build result:** Fresh `./pc_port/build_port.sh` → `LINK OK`. `make check` completed its build and retains the known target status: `slus_006.64` FAILED (`229aa9d63118870802f34fc8afd2895948edbc6735d6fce720ebdbba18bc4a17`), `field.bin` FAILED (`a87c5d5699d4866b64fc43b0306b45ee7f56b4f41c8a458f2309bea6bd1c58ba`), member-change OK (`3b9e2b890c27ae0de97fe343ac75cd35c05fe7f9605ae387d2166da0be78109c`), shop OK (`7890e14bcabddcf85368de10783ecff8daa166dc5ac254e06db5e27959cab4cf`).
- **Runtime result:** At Map014 f60, the two feedback batches previously bound stale IDs (`GR_SetTexture arg=5/global=6`, later `arg=6/global=5`); after the ordering change, both bind their captured/current ID and `stale_vram_tex_calls=0`. Isolated f180/240/300/360 and later f420--600 captures show the transitional early view resolving into a coherent distorted Lahan room (walls, ceiling, easel, paintings, furniture) rather than a camera-under-floor blackout. A distortion-bypassed f360 remains sparse, proving the feedback composite now contributes the scene. A 20-second Map001/entrance6 smoke under isolated Xvfb timed out normally (`RC=124`) after entering the field main loop, with no crash.
- **Proven:** `asm/slus_006.64/system/temp2.s:8002E364--8002E3AC` samples FLAG before issuing `NCLIP`, branches on that sampled FLAG, then never reads MAC0; it otherwise applies FLAG, SZ3, screen, and OTZ gates before packet insertion. The stale-texture trace and the late-frame visual captures independently validate the renderer ordering fix.
- **Not proven / still open:** Map014 now has recognizably correct assets and camera staging, but its darkness, warp timing, and early-frame framing are not yet pixel/phase-aligned with the retail video. Do not claim a fully retail-identical composite yet. The well re-entry gate remains a separate open issue.
- **Committed:** no — renderer work remains intentionally separate from the pre-existing matching-tree distortion WIP; clean commit split requires a final visual review.
- **Stop reason (if stopped early):**

### [2026-07-12 20:21] Restore field CLUT RECT packing and scene palette masks
- **Hypothesis:** The normal field palette update is silently skipped because retail `D_800B004C..D_800B0052` is one 8-byte `RECT`, while the native port's auto-stubs allocate its four halfword labels independently; `D_800ADC24` is also still zeroed instead of carrying its retail scene-mask table.
- **Scope:** Port-only field data migration in `pc_port/src/data_field.c`; no Map014 script, camera, distortion, or model-culling logic change.
- **Change made:** Migrated the complete retail 0x20-byte `D_800ADC24` data table from `asm/field/data/3DF78.data.s`, and replaced the four independent host stubs with one contiguous 8-byte BSS block plus exact aliases for `D_800B004C`, `D_800B004E`, `D_800B0050`, and `D_800B0052`.
- **Build result:** Fresh native `./pc_port/build_port.sh` in `xenogears-dev` → `LINK OK`. Containerized `make check` completes its build and preserves the known result: SLUS FAILED (`229aa9d63118870802f34fc8afd2895948edbc6735d6fce720ebdbba18bc4a17`), field FAILED (`a87c5d5699d4866b64fc43b0306b45ee7f56b4f41c8a458f2309bea6bd1c58ba`), member-change OK (`3b9e2b890c27ae0de97fe343ac75cd35c05fe7f9605ae387d2166da0be78109c`), shop OK (`7890e14bcabddcf85368de10783ecff8daa166dc5ac254e06db5e27959cab4cf`). A host-only `make check` was not usable because that shell lacks `mips-linux-gnu-as`; it left no tracked artifact changes.
- **Runtime result:** Before the migration, a live Map014 frame-60 breakpoint showed `&D_800B004C = {0,0,0,0}`, `D_800B0050-4 = {0,0,128,0}`, and `D_800ADC24[0..7] = {0,...,0}`; the port guard skipped `LoadImage` because `h==0`. After the migration, both RECT access paths are `{0,251,128,1}`, the mask table is `{129,192,96,48,24,12,6,3}`, and `LoadImage(rect={0,251,128,1}, p=D_800AFD24)` is reached. A fresh Map001/entrance6/camera7 smoke reached `func_8007554C` with `map=1`, `actors=143`, and the expected `{0,251,16,1}` UI capture RECT.
- **Proven:** Retail declares the four BSS labels consecutively at `asm/field/data/3FAFC.bss.s:574-585`; `FieldLoadUITextures` writes them as a `RECT` before `StoreImage`, and `func_80074108` addresses the same object through `D_800B0050-4` before its per-frame CLUT `LoadImage`. The old host layout violated that pointer arithmetic; the new aliases preserve it.
- **Not proven / still open:** Map014's early close-up remains a submitted-but-cull-heavy model/effect path (9 model batches, 16 packets emitted at f60); the compass/background quads are intentionally gated off there by authored `D_800B21D1=1`, so this data repair should not be credited with solving that specific composite.
- **Committed:** no — the data repair is isolated and validated locally; existing renderer and distortion WIP remain uncommitted.
- **Stop reason (if stopped early):**

### [2026-07-12 21:20] Adversarial review gate, six confirmed fixes, and the commit split
- **Hypothesis:** The uncommitted renderer/data/distortion set was runtime-validated but never adversarially reviewed; a multi-agent review against retail asm would either clear it for the pending commit split or surface real defects first.
- **Scope:** 34-agent review workflow (4 review dimensions, 3 refuters per finding) over the four uncommitted diffs; fixes for confirmed findings only; retail-exact completion of `func_800726E8`; re-validation; the commit split. No new subsystems.
- **Change made:** (1) FT4 variant-0 walker: OT bucket corrected to `otz >> D_80050100` (no +2) and the NCLIP MAC0 `blez` cull restored. (2) `FieldDistortionInitialize`: i<0xE branch bodies un-swapped (rows 0-13 = per-column framebuffer pages with the context-1 0x100 tpage; rows 14-16 = 0x3C0 capture strips). (3) `FieldDistortionDraw`: wrapped strip right edge waves (`waveX+0x140`, asm 800A509C); row 13 overwrites `waveY` with the row-14 wave (asm 800A532C) exactly as retail. (4) `build_port.sh`: `g_xeno_vram_fb_synced` suppresses the stale deferred-PBO splat between adjacent DR_MOVEs. (5) `func_800726E8` fully retranscribed: both D_800AFE9C pad-rotate blocks with the g_Scene+0x48 bit15 refusal gates, the scene65 else path with ±0x200 heading updates, and the transition-active skip of both auto blocks.
- **Build result:** `./pc_port/build_port.sh` LINK OK (221 stubs) after every stage. `make check` before fixes: SLUS `229aa9d6…`, field `a87c5d56…`, member/shop OK. After: SLUS/member/shop unchanged, field `efb1ebd9d7c5e040fadc124831877eb8245bdcd28c46d1a08b5698827a8c3bde` (distortion + misc2 semantic decomps; field.bin was already non-matching). Match evidence: func_800726E8 19/212 objdump lines differ (register/CSE choice only); distortion Initialize 61/286; Draw semantic.
- **Runtime result:** Map014 f60/f180 close-up garble RESOLVED (dark room + fiery painting subject + warped floor, matching retail staging); f240/f360 now render Fei fully at the easel (previously a black silhouette — FT4 depth fix); f600 wide shot coherent; well capture byte-identical (eye2.y=-914, mean 0.4410). Camera-rotate probe on the final binary: bit 8 → 0x600→0x800 over the 8-frame countdown, bit 4 → back down, bit15 set → fully refused, bit15 cleared → stays inert without input.
- **Proven:** All six review findings were 3/3 refuter-confirmed and re-derived by hand before fixing. CORRECTION to the [2026-07-12 19:50] entry: its claim that the E268 walker "never reads MAC0" is WRONG — retail reads MAC0 at 8002E37C (`mfc2 $t4, $24`) and culls with `blez` at 8002E384; the port now matches retail (backface cull present). The 19:50 runtime evidence stands; only that asm reading is superseded.
- **Not proven / still open:** Map014 darkness/warp-phase pixel alignment vs the retail video (structural staging matches; no pixel/phase comparison run); well re-entry suppression (separate line); the synthetic new-game party `(0,0,0)`; Draw is a semantic match only.
- **Committed:** 47b7f27 (CLUT RECT/masks), c7ff5ec (distortion decomp), 602554a (FT4 dispatch), 0f9d735 (DR_MOVE ordering), 8456d26 (func_800726E8). This entry plus the two prior pending entries land in the log commit. Push withheld (origin 403).
- **Stop reason (if stopped early):**

### [2026-07-12 21:53] Retail-video oracle for the Map014 opening; palette-lighting root cause narrowed
- **Hypothesis:** With real retail frames (user's video, 10:00-12:30 segment) as the oracle, the port's Map014 divergences can be root-caused individually: room darkness, missing smoke FX, missing dialogue, early-frame framing.
- **Scope:** Read-only investigation + retail frame extraction (yt-dlp/ffmpeg, 140 frames, contact sheets in the session scratchpad). No tree changes. Diagnosis probes only (scratchpad/*.gdb, all marked TEMPORARY).
- **Evidence chain (each step runtime-verified):** (1) Port room renders G/B-crushed vs retail tan; walls hold constant color over time (mean drop was framing). (2) A sampled room FT4 is RAW-textured (code 0x2D) with clut id 0x7880 = VRAM row 482; the id comes verbatim from model data (D_8005010C==1 raw latch; base path func_8002CC74 never called in field). (3) Map stream decoder uploads palettes VERBATIM (no transform), rows 480-491 hold 12 per-texture palettes; row 483 IS bright tan; row 482 (fiery painting palette) is authored dark-red. (4) Hardware watchpoint: rows 480-483 written exactly once (load); no rewrite ever runs on the port. (5) Bank-swap and row-copy experiments confirm rendering exactly reflects palette content (row-copy recolored the painting sepia). (6) Retail video shows the whole scene brightening 10:21-10:28 (palette interpolation) from a near-black fire-glow open to a bright tan room.
- **Conclusion (by elimination, packet side ruled out by matched decomp):** retail rewrites the palette CONTENT (rows 480+) from RAM keyframes per lighting state during the opening reveal; the port never runs that driver, freezing every map at its load-time palette. This is the keystone divergence for the whole opening (dark room, wrong early-frame look) and likely for night scenes (video 11:44+ night-lit Lahan flashback).
- **Also learned:** opening script is RUNNING (actor 0 parked on retail hold opcode 0x5B by design; other actors advance) — dialogue is late, not blocked; opcode 0x5B/handler func_80095284 is implemented. Two benign stubs fire on Map014 (func_80028B14 devkit I/O, func_8003A450 sound).
- **Not proven / still open:** the identity of the retail palette-lighting driver function and its keyframe source (4-reader workflow wf_490489ea-c3d in flight); smoke/ember FX layer in the close-up; port timeline runs slower than retail's 21s-to-dialogue.
- **Committed:** no tree changes this cycle; log-only entry.
- **Stop reason (if stopped early):** waiting on the palette-driver identification workflow.

### [2026-07-12 22:05] CORRECTION + tighter characterization of the Map014 room color divergence
- **Corrects the 21:53 entry.** That entry claimed "port room walls sample CLUT row 482 (dark red)" and hypothesized a missing per-lighting palette-rewrite driver. Both are now DISPROVEN:
  1. A runtime row-copy test (row 483 tan -> row 482) recolored the **painting** on the easel, not the walls: row 482 is the fiery-painting palette, NOT a wall palette. My "wall packet" sample grabbed a painting quad at the shared output cursor.
  2. The 4-reader palette-flow workflow (wf_490489ea-c3d, reader phase completed; verify phase aborted on usage-credit exhaustion) found NO lighting/time-of-day palette state machine anywhere in the field module. CLUT banks (rows 480-491) are uploaded ONCE at load, verbatim from the (mapNum<<1)+0xB9 stream, and never rewritten. `func_80096AF4` (opcode 0xFE 0xAE, the only script palette-state writer) touches D_800B2340-2346 which are ANIMATION/dolly counters (consumed by misc6/misc8 anim code), not CLUT rows. So there is no missing palette driver.
- **What IS real (clean, pose-caveated):** the port opening renders the correct scene (room, painting, Fei, furniture all recognizable, retail staging) but visibly DARKER and WARMER/REDDER than the retail video. The port's loaded CLUT rows themselves contain correct tan palettes (row 480 = (23,20,14) etc.), and room FT4s are RAW-textured (code 0x2D, no per-vertex modulation), so a lighting/modulation cause is ruled out. Fei's blue collar and the painting's orange render correctly in the same frames, so it is NOT a global channel bug.
- **Confound found:** my port vs retail per-pixel color ratios were unreliable because port and retail captures at the same timestamp have DIFFERENT camera poses/timing (the port's opening runs at a different pace), so "same screen-region crop" compared different geometry. The perceptual darker/warmer difference is real; exact per-asset color deltas are NOT cleanly established.
- **Decisive next step (recommended, NOT done — expensive):** (a) capture the port and retail at a POSE-MATCHED wide establishing shot, then (b) decode the on-disc 0xBB palette sections offline (or dump the port's uploaded rows 480-491) and compare byte-for-byte against a retail reference, to determine whether the divergence is CLUT content (upload/ordering in `PcPortDrain0xBBToVram`) or a texture-pixel/upload issue. Until pose-matched, do NOT claim a room-color bug OR retail-fidelity.
- **Committed:** log-only; no tree change. No speculative fix landed.
- **Stop reason (if stopped early):** usage credits exhausted mid-workflow; model switched to opus; deferring the expensive on-disc palette comparison to a focused follow-up.

### [2026-07-12 22:20] Map014 room red-shift narrowed to depth-cue/fog (distortion ruled out)
- **Decisive tests (this cycle):**
  1. POSE-MATCHED wide establishing shot, port f600 vs retail rf_030 (same camera/geometry): retail room G/R=0.85 B/R=0.40 (even warm tan); port G/R=0.26 B/R=0.06 (red-shifted, dark falloff to black at edges). Side-by-side saved: scratchpad/cmp_wide_side.png. This is a CLEAN confirmation controlling for framing.
  2. Distortion forced OFF from f450 (g_FieldEffects.distortion.isActive=0), captured f600: room renders COMPLETE (walls, easel, boxes, painting, doorway, floor all present) and IDENTICALLY red-shifted (G/R=0.259 vs 0.259 with distortion on). scratchpad/m14_nodistort.png.
- **Ruled out (with evidence):** CLUT bank content (port rows 480/483/487/488/491 are correct tan), missing per-lighting palette driver (none exists — wf_490489ea-c3d), per-quad modulation (room FT4s are RAW-textured code 0x2D), and the distortion/feedback composite (test 2). Also NOT a global channel bug (Fei's blue collar + painting's orange render correctly in the same frames).
- **Surviving hypothesis (specific, code-targeted):** the room's bright-center / dark-red-edge falloff is a DEPTH-CUE (fog) mismatch — distant wall quads blend toward a too-dark/too-red far-color, or the fog Z-range starts too near. Next session: inspect the field's fog/depth-cue setup (GTE DQA/DQB depth-cue regs + far-color; SetFarColor/SetFogNear-equivalent in the field render init) and compare the port's far-color and OT depth range against retail. Candidate files: the field render-context init (misc4 FieldInitializeRenderContexts / func_80078D44 region) and the PsyCross GTE depth-cue path.
- **Committed:** log-only; no tree change; no speculative fix.
- **Stop reason (if stopped early):** usage credits exhausted mid-session (workflow verify phase); handing off the fog hypothesis with a clean decisive evidence trail rather than guess a fix.
