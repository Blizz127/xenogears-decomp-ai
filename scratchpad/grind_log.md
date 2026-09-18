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

### [2026-07-12 23:20] Early-game opcode sweep: fog, cam restore, flags, angle LUTs
- **Hypothesis:** Map014→Lahan still had small INCLUDE_ASM VM handlers and a stubbed `SetFogNearFar` (`func_80048AB0`); filling those unblocks early script/camera/fog without inventing game logic.
- **Scope:** Matching-tree C for fog 0xE6, FE6D, 0xA0 restore chain + neighbors, FE2C–FE2F actor-flag readers, FEAE, FE44, 0x69/0xA5/0xAB/0xB6; port fog shim + retail angle LUT / AEA44 / AEA64 / B2340 data. No yaml/build-config change. Did **not** land `func_800862CC` (depends on layout-sensitive `FieldActorWorldToScreenPosition`).
- **Change made:**
  - `misc11.c`: `func_80091944` (0xE6 SETUP_FOG)
  - `camera_movement.c`: `func_8008FB28` (FE6D cam snapshot)
  - `misc7.c`: `func_8009A420`, `9AE3C`, `9B708`, `9B7A8`, `9BA0C` (0xA0), `9A490` (0xA5), `9AC34` (0x69), `9ACEC` (0xAB), `9B8E4` (0xB6), `9B15C` (FE44)
  - `misc.c`: `FieldScriptWriteActorFlags1–4` (FE2C–FE2F)
  - `party/stats.c`: `func_80096AF4` (FEAE)
  - `psyq_compat.c`: `func_80048AB0` → `SetFogNearFar` (port-only; matching ownership still pre-InitGeom asm blob)
  - `data_field.c`: `g_FieldAngleToDirectionLUT`, `D_800AEA44`, `D_800AEA64`, `D_800B2340` alias; removed colliding zero BSS stub for the LUT from `stubs.c`
- **Build result:** `./pc_port/build_port.sh` in `xenogears-dev` → LINK OK. Stub count ~649 undefined (was ~655). `make check` not re-run this cycle; last recorded baseline remains SLUS FAILED `229aa9d6…`, field FAILED (pre-existing layout), member/shop OK — **unchanged claim, not re-proven tonight**.
- **Runtime result:** Map014 ent0 and Map001 ent6, `DISPLAY=:0`, 22s timeout → **RC=124** both. Map014 stubs only benign `func_80028B14` / `3A450` / `3A89C`. Map001 stubs `28B14` + live `func_800862CC` (positional SFX helper; callees still INCLUDE_ASM).
- **Proven:** New symbols are strong `T`/`D` in `xeno-port` and absent from function stubs. Fog enable writer is 0xE6 (sets `D_800B218E=1`); Map014 opening still reports fog=0 in prior probes so 0xE6 may not fire on that path yet — decomp is still correct for when scripts use it. Angle LUT was previously a zeroed 32-byte stub BSS — now retail 8×u16 values (pre-existing rotation bug for any opcode that indexed the zero table).
- **Not proven / still open:** Map014 red-shift / depth-cue visual (RAW FT4s may ignore DQA; DQA=0 probe earlier still red-shifted — fog decomp alone does not claim a color fix). `func_800862CC` + `FieldActorWorldToScreenPosition` + `func_80086078`. Matching-tree extraction of `func_80048AB0` out of `35B7C.s` (needs yaml). Sound backend (`3A450`/`3A89C`/`3A5D0`). Synthetic new-game party `(0,0,0)`.
- **Committed:** no — commit not requested; changes remain uncommitted for review/split.
- **Stop reason (if stopped early):** solid early-opcode batch + green smokes; positional-SFX cluster deferred for layout audit.


### [2026-07-12 23:35] Positional SFX cluster + FE26/FE3A
- **Hypothesis:** Map001's live `func_800862CC` stub was the remaining field helper on the talk/ambient SFX path; FE26/FE3A are tiny EX neighbors still INCLUDE_ASM.
- **Scope:** `misc8.c` attenuation/project/start/update; `misc.c` FE26/FE3A. Sound leaf calls remain auto-stubs (`39F9C`/`3A20C`/`3A344`/`3A55C`).
- **Change made:** Decompiled `func_80086078`, `FieldActorWorldToScreenPosition` (CompMatrix vs `g_Scene.worldToScreenMatrix` = retail `&g_FieldActors-0xAC`), `func_800860F0`, `func_800862CC`; FE3A `func_8008D230` (party frame mask); FE26 `func_8008D5C8` (effect flag).
- **Build result:** LINK OK.
- **Runtime result:** Map001/Map014 RC=124. Map001 no longer stubs `862CC` — hits sound leaves instead (path now executes). Map014 unchanged benign sound/CD stubs.
- **Proven:** worldToScreen identity via BSS math; positional slot alloc/pan math runs through to SPU helpers.
- **Not proven / still open:** actual audio playback (sound.c skipped on port); Map014 red-shift.
- **Committed:** pending this entry's commit split.
- **Stop reason (if stopped early):**


### [2026-07-12 23:40] Map001 walk-wait wrappers (0x46 cluster)
- **Hypothesis:** Map001-reachable opcode 0x46 and its siblings only arm slot state then call `func_80097A50`; decompiling the wrappers unblocks IP advance once 97A50 exists (currently stub→instant complete, same as prior decompiled 0x55/0x56 path).
- **Scope:** `misc7.c` `func_800977A4`, `func_80097864` (0x46), `func_80097954` only. Left `func_80097A50` (~399 insn) as the next dedicated pass.
- **Change made:** Three walk-wait wrappers matching retail slot flags_0x17 modes 1/3 and duration/IP advances.
- **Build result:** LINK OK.
- **Runtime result:** Map001/Map014 RC=124; stub set unchanged (sound leaves + CD).
- **Proven:** Wrappers compile and do not regress boots. Real walk timing still blocked on `func_80097A50`.
- **Not proven / still open:** `func_80097A50` movement executor; Map014 red-shift.
- **Committed:** with this entry.
- **Stop reason (if stopped early):** stop before the large 97A50 body; natural commit boundary.

### [2026-07-12 23:47] Map014 pose-matched red-shift = subtractive fade1 pulse (not fog/CLUT)
- **Hypothesis:** Pose-matched wide-shot red-shift is depth-cue/fog or CLUT/upload. Also check distortion wave timing and Fei facing during the opening.
- **Scope:** Runtime probes only (scratchpad/m14_*.gdb). No game-logic patch landed — root cause is scripted screen tint timing, not a one-line render fix with proven retail parity for the whole opening.
- **Decisive color evidence:**
  1. Fog path ruled out again: Map014 `fogEn=0`, far RGB (255,255,255), RFC/GFC/BFC=0 through f600. DQA still −98 but RAW room FT4s ignore GTE depth-cue color.
  2. Wall CLUT live sample (row 483 / tpage 0x85 8-bit): first entries decode to bright tan e.g. `0x4b1a` → (208,192,144), G/R≈0.92 — matches retail wall hue, not the framebuffer.
  3. All sampled room FT4s are RAW `code=0x2D` with `rgb=(0,0,0)` (modulation unused; PsyCross `bright=2` identity path OK).
  4. **fade1 is visible the whole opening** with ABR mode 2 (subtract). Script opcode `0xF1` (`func_8008B248` → `FieldFadeSetParameters(1,…)`) pulses between immediates `(40,80,42)/8f` and `(90,120,180)/5f`, with one clear `(0,0,0)/80f` at f65. Bytecode matches (`f1 02 80 …`).
  5. Force full-white subtractive fade1 → framebuffer goes black (fade draw path works). One-shot kill of fade1 was insufficient (script re-arms). **Sticky kill every frame from f450:** center G/R **0.813** vs retail rf_030 **0.831** (was ~0.24 with fade). Proof: `scratchpad/m14_nofade_sticky.png` vs `map14_review_f600.png` / `cmp_wide_side.png`.
  6. Retail oracle agrees the early dream is also red: rf_001–010 G/R≈0.20–0.26; rf_020+ settles tan G/R≈0.83. Port’s bug is **keeping the pulse through the wide establishing shot** (fadeN still climbing at f900; last FADEZERO only at f65, then pulse resumes at f147 after clear sleep).
- **Wave timing:** distortion `isActive=1` at f60 with large fixed-point `v1…v6`; at f120 still active but values wound down / `isFinished=1`; by f180 fully clear (`active=0`, all v=0). No retail phase capture this cycle — port wave ends before the wide shot.
- **Fei facing:** slot1 `char=0` at easel pos `(115,-1,-455)` from f60–f600; `rotZ=1536` (0x600) and `dir=0` **unchanged** the whole window; `anim` drifts `0 → -1 → -2` (suspicious). Facing appears locked for the painting pose; not proven wrong vs retail without a retail rot dump. Prior probes that reported `ad=NULL` were wrong (`g_FieldActors` is a pointer, not `&g_FieldActors`).
- **Ruled out:** missing palette driver (prior), distortion composite (prior nodistort), fog/DQA as the wide-shot red cause, CLUT bank content for walls, PsyCross RAW `/128` identity (bright=2).
- **Not proven / still open:** why script re-enters the 1896 pulse after the f65 clear/`0x27`/`0x00` end (scheduler re-entry vs missing flag); whether retail keeps a subtler pulse that capture averages out (unlikely given rf_030); Fei `anim` negative ids; `func_80097A50`.
- **Next fix direction (not landed):** stop or gate fade1 after the scripted clear so wide-shot matches rf_020+, without inventing tint values — find the retail exit condition / script re-entry. Probes: `m14_fade_trace.gdb`, `m14_nofade_sticky.gdb`, `m14_wave_facing.gdb`.
- **Committed:** log-only this entry.
- **Stop reason (if stopped early):** root cause isolated with pose-matched proof; no speculative fade hack without the re-entry condition.

### [2026-07-13 00:00] Fix Map014 room red-shift: A2030 gates VM on scriptFlags_0x0
- **Hypothesis:** After the scripted fade clear (routine 4), ambient actor 18's OP_27 sets `scriptFlags_0x0` (isDisabled). Retail `func_800A2030` skips `FieldScriptVMRun` when that bit is set (asm `lw` from actor+0, `andi 1`, `bnez` skip). Port wrongly tested `flags & 1` (actor+4), so routine-1 auto-restart kept pulsing subtractive fade1 through the wide shot.
- **Scope:** One-line semantic fix in `src/field/scripts/virtual_machine.c` `func_800A2030`. No yaml/build-config.
- **Evidence chain:**
  1. Slot18 routines: r1=1896 pulse, r4=1926 clear. Pulse ENDs at 1924 → A2030 auto-restarts r1.
  2. Fei OP08 starts r4 at f65; clear's OP27 targets actor 18 and sets scriptFlags_0x0.
  3. Matching asm 800A2228–800A2250 reads `*(u32*)actor & 1` (scriptFlags), not `flags`.
  4. After fix: `fades_after_clear=0`, fade1 vis=0 rgb=(0,0,0) at f600, center G/R **0.813** vs retail rf_030 **0.831** (was ~0.24). Proof: `scratchpad/m14_fade_fix.png`.
- **Build / runtime:** `./pc_port/build_port.sh` LINK OK. Map014/Map001 smokes RC=124 (benign stubs only). `make check` not re-run; last baseline unchanged claim.
- **Committed:** source fix + this log entry (separate commits).
- **Stop reason (if stopped early):**


### [2026-07-13 00:15] Map014 opening close-up = skull-cam hold + E688 FLAG starve (vs YouTube 10:17)
- **Hypothesis:** User screenshot (Fei-from-behind, black bg, yellow/green blocks) is the early Map014 hold, not the retail painting-fill from [Worthy Gamer 10:17](https://youtu.be/PWtGUF2cx3U?t=617).
- **Retail target:** Opening Scenes (chapter ends 10:23 Lahan). Frames `rf_010`/`rf_015` = fiery canvas fill. Port wide room already OK after fade1 fix.
- **Runtime evidence:**
  1. Hold eye2=`(141,-75,-431)` at2=`(23,-50,-548)` from f2→f210; Fei at `(115,-1,-455)` sits *between* eye and at (≈82 units from camera). emit≈5.
  2. First `ARM_CAM` after hold is f211; authored close-shot `(372,-137,-202)` only around f250. Forcing that eye at f60 → emit≈99 and painting+easel visible (`m14_forcecam_f60.png`).
  3. Hide Fei (status`|0x20`) at hold → nearly black (lit≈0.01): room is not drawing; garble *is* Fei sprite + scraps.
  4. Room FT4s use `func_8002C700` **variant 2** → `func_8002E688`. Cull census: **~94% FLAG** (often with otz≤0), then NCLIP/overlap. Ignoring FLAG alone does not restore the painting.
  5. Distortion is active early but composites a near-empty FB → yellow vertical seam / block noise on Fei's silhouette.
- **Change made (kept):** `func_8002E688` — remove port-only oversize cull; `otIndex <= 0` → `< 0`; NCLIP `blez` (`<= 0`) to match asm 8002E7AC. Does **not** alone fix the hold (FLAG still dominates).
- **Build result:** LINK OK.
- **Not proven / still open:** why camera holds at skull pose until f211 (script wait after distortion ends ~f150); Fei sprite CLUT/tpage when huge; Fei disappear on right cam turn (queued). Next: dump the camera actor script that arms at f211 and find the wait that should end earlier / different initial eye.
- **Committed:** not yet (E688 cleanup is real but insufficient for the user shot; hold for a close-up that actually matches rf_010).

### [2026-07-13 00:31] Map014 skull-hold delay = Fei authored Sleep+FE27 (not stuck cam wait)
- **Hypothesis (updated):** FLAG cull is not causal (E688_IGNORE_FLAG byte-identical @ f60/f120 — user probe). Delay to painting ARM is Fei script waits, not camera-actor stall.
- **Proven:**
  1. Camera actor A0 arms skull `(141,-75,-431)` at f0, then intentional forever-hold opcode **0x5B** (`func_80095284`) at IP 114. Not a bug.
  2. Painting ARMs are **Fei (char=0)**-driven: first at f211 IP 2151 (nudge), real close-shot ~f220→f250 `(372,-137,-202)`. f250 capture already shows easel+painting (`m14_early_f250.png`).
  3. Fei bytecode gate: Sleep `26 3C` (~60f @ IP202) → FE27 mode0/1 wind-down `FE 27 00 46` (~70f) → Sleep `26 46` (70f @ IP225) → FE4D → cam routine. WaitTimers match bytecode (not misread).
  4. Force-skip Fei Sleep + force FE27 release → first ARM at **f12**, painting eye by ~f40. Proves those waits are the entire ~200f gate.
  5. Opcode 0x26 is Sleep (live handlers), not HideById.
- **Not a fix:** shortening authored Sleep/FE27 without retail timing evidence = inventing game logic.
- **Still open:** whether retail's opening *pace* differs (known prior note); whether skull-hold should look like something other than Fei-garble (distortion composite); Fei CLUT when huge; Fei disappear on right turn.
- **Build/runtime:** probes only; no source change this cycle. `make check` baseline unchanged claim.
- **Committed:** not yet.

### [2026-07-13 00:35] Decision: keep authored waits; chase hold presentation + Eye lerp lag
- **Course chosen (safe / retail-like):**
  1. **Do not** shorten Fei Sleep/FE27 or relocate skull eye — bytecode + live handlers prove they are authored.
  2. Treat **f250 painting close-up** as the YT 10:17 stage (already looks right once Eye arrives).
  3. Hold-window Fei-garble is a **presentation** bug at the real skull pose (Eye==Eye2 from f2–f180), not a stuck wait.
  4. Secondary fidelity issue: after f211 ARMs, `eyeStepDistance` returns to **12**, so `g_CameraEye` lags `Eye2` (f250 Eye=(337,-126,-237) vs Eye2=(372,-137,-202)) — may delay *perceived* painting fill without changing script IP timing.
- **Next chase order:** (A) Fei near-camera / special-anim draw during hold; (B) what restores step=12 and whether scripted Start should keep step=1 (asm check before any change).
- **Rejected:** E688 FLAG bypass as a fix; inventing shorter sleeps; moving camera actor off 0x5B.

### [2026-07-13 00:52] Fix Map014 skull-hold tile garble: clip active-actor FT4 outside raster
- **Root cause:** `func_800764B4` submits actor 1's active-actor FT4 (the actor shadow) during the skull hold even though all projected points are left of the viewport: f60 `x=(-634,-775,-663,-823)`, `y=(32,32,31,31)`. Retail relies on the PSX GPU's raster clip; PsyCross applied its draw-environment offset and stretched this otherwise invisible FT4 across the image as the green/yellow tile field.
- **Change made:** Port-only conservative reject in `func_800764B4`: skip only when all four projected points are beyond the same active raster edge. It deliberately does not use a "some vertex visible" test, which would incorrectly cull quads crossing an edge.
- **Proof:** GDB zero-length isolation of only this actor FT4 produced f60 hash `de6f100d…`; the source fix produces the identical hash. The normal painting/easel f250 is byte-identical before/after (`a6598051…`).
- **Build/runtime:** `./scratchpad/run_build_port.sh` LINK OK. Map001 ent6 and Map014 ent0 12s smokes both ran to expected timeout (124), with only pre-existing stubs.
- **Not changed:** authored Fei waits, camera timing, room FT4 E688 path, and the optional E688 FLAG diagnostic.

### [2026-07-13] Map014 sprite FT4 sampling check (residual assumption recorded)
- **Result:** CPU decoding of the captured frame-60 source VRAM agrees with the
  exact 4-bit shader lookup for all 1,128 texels covered by the eight
  `tpage=0x001a` / `clut=0x3811` packets.  CLUT and texture-window arithmetic
  are therefore not a lead; default filtering is point sampling.
- **Residual assumption:** the attempted live GDB re-capture did not reach the
  frame-60 parser breakpoint.  This evidence is a replay of the captured VRAM
  through a line-for-line integer mirror of `GPU_SAMPLE_TEXTURE_4BIT_FUNC`, not
  a readback of the fragment shader's per-pixel output.  The mirror is simple
  and directly matches the shader's byte/nibble/CLUT operations, but this is
  deliberately left marked as one degree short of direct GPU observation.
- **Artifacts:** `m14_ft4_source_vram_f60.bin`,
  `m14_ft4_sampling_compare.py`, and `m14_ft4_sampling_compare.log`.

### [2026-07-13] Correction: actor-24 GTE trace baseline attribution
- **Correction:** the 50-record actor-24 GTE trace previously described as
  `d37a35f` was captured from a source layout at or after `1753a6a`: its GDB
  breakpoint was `misc2.c:1589`, but at `d37a35f` the dispatch call is line
  1585.  The probe could not have produced records from the earlier revision.
- **Impact:** this corrects the baseline label, not the relative validations.
  `1753a6a` predates the later E688/diagnostic/cosmetic changes, so their
  before/after trace comparisons remain against a consistent post-angle-fix
  tree. RotAverage4's exact objdiff result is unaffected.
- **Probe hygiene:** line-keyed actor dispatch probes are revision-fragile and
  may fail silently. `m14_actor24_gte_trace.gdb` now keys on the
  `func_8002C700` dispatch symbol plus actor 24's live model-packet identity,
  and emits an explicit zero-record failure marker.

### [2026-07-13] Fix Map014 room/sprite OT ordering: E688 uses retail min(SZ0..SZ3)
- **Root cause:** `func_8002E688` used `RotTransPers4`'s return
  (`SZ3 >> 2`) as its ordering-table depth. Retail instead reads GTE `SZ0`
  through `SZ3`, selects the unsigned minimum, then performs `srav` by
  `D_80050100` (retail `0x8002E82C–0x8002E894`). The PsyQ helper contract was
  correct; only this walker selected the wrong depth source.
- **Proof:** at Map014 frame 60, actor 38's four wall quads sorted into port
  buckets `30/33/33/18` from SZ3, while retail's minima require
  `74/89/87/73`; Fei is bucket 41. The wall therefore rasterized after and
  overwrote Fei. With min-SZ, all four buckets exactly match retail and the
  wall rasterizes first; the fresh frame-60 capture shows Fei in front.
- **Exonerated:** actor 24 matrix/GTE trace remains byte-identical; E688's
  retail cull-sequence alignment, CLUT sampling, FT4 construction,
  triangulation, fragment blending, framebuffer feedback, and subtractive
  fade were downstream of the already-wrong ordering. The temporary
  rendering.c FLAG guard merely hid the symptom by dropping Fei packets and
  was discarded.
- **Validation:** `run_build_port.sh` LINK OK; frame-60 bucket probe re-run on
  the rebuilt binary. Map014's later bare-run SIGSEGV also occurs without this
  change, so it is pre-existing and a separate investigation.
- **Open, separate work:** Map014 post-frame-60 bare-run SIGSEGV; PsyCross ABR
  1/2/3 CLUT bit-15 per-texel semi-transparency gate; floating room geometry.

### [2026-07-13] Closed: reported Map014 bare-run SIGSEGV was an invocation artifact
- **Correction:** the environment copied into the earlier "Map014" smoke
  command used `XENO_FIELD_MAP=1`, which selects Map001, not Map014. The
  reported exit-139 result therefore did not establish a Map014 crash.
- **Reproduction check:** at `01b432d`, three 20-second bare runs each of
  Map001 (`XENO_FIELD_MAP=1`) and Map014 (`XENO_FIELD_MAP=14`) completed
  cleanly: 6/6 with no SIGSEGV. The crash item is closed unless an exact
  failing invocation and binary can be preserved.
- **Harness guard:** `run_map001.sh` and `run_map014.sh` pin the intended map,
  reject conflicting inherited `XENO_FIELD_MAP` values, set `KERNEL_SEL=0`,
  and print the selected map and entrance before launch.

## Floating BG/room geometry — CLOSED (not reproducible at HEAD d102e5f)

Map014 frame 600 wide shot shows a coherent room: connected floor/ceiling/walls,
furniture, easel, bed, paintings, perspective-consistent throughout.

The "disconnected floating fragments" description predates the DR_MOVE feedback
ordering fix and the func_8002E688 min-SZ OT derivation fix (fea685a). Either or
both likely resolved it.

Reopen only with a current frame number + capture showing a reproducible defect.

## Open-issue list revalidation vs HEAD d102e5f — SIX ITEMS STALE

The following were listed as open and are NOT reproducible / already correct:

- Opcode 0x86 stall (func_800248D4): IMPLEMENTED from retail asm
  (0x80024C68-0x80024C9C). Not covered by the unimplemented-opcode assertion.
- func_8009635C: IMPLEMENTED. Finds existing item slot, increments qty up to
  MAX_ITEM_QUANTITY, else allocates a slot and stores item/qty.
- func_80095124: IMPLEMENTED. Routes by item category to the inventory lookup helper.
- func_80094E8C: CORRECT. Returns matching index i, or -1. Does not return
  constant 0. The "returns 0 instead of i at misc11.c:790" note is wrong.
- Map014 post-frame-60 SIGSEGV: NOT REPRODUCIBLE. Original report used
  XENO_FIELD_MAP=1, which loads Map001. 6/6 clean runs across both maps.
- Floating BG/room geometry: NOT REPRODUCIBLE. Map014 frame 600 shows a
  coherent room.

## Corrected script-layer decomp queue

Dedicated UNIMPLEMENTED opcodes in func_800248D4 (per the actual assertion):
  0x85, 0x8E, 0x98, 0xBE, 0xC8, 0xD4, 0xE2, 0xFA

## Lesson

Fixes silently resolve items nobody goes back to check. Revalidate open issues
against HEAD before investing in them. A stale bug report costs the same rounds
as a real one — and risks reimplementing working code.

## 2026-07-14 — CompMatrix alias-audit correction and PsyCross ordering fix

- **Correction:** the earlier `CompMatrix` audit declared aliasing safe after
  checking rotation writes, but stopped before the translation sequence. That
  conclusion was incomplete. Alias-safety analysis must cover every write to
  the aliased region, not only the first class of writes encountered.
- **Retail mechanism:** handwritten `CompMatrix` does not copy either matrix.
  It consumes all rotation inputs before output rotation stores, reads the
  transformed GTE translation, then loads original `m0.t` at
  `0x8004944C-0x80049454` before storing output translation at
  `0x80049464-0x8004946C`. That read-before-store ordering makes `m0 == m2`
  safe.
- **PsyCross defect:** the live implementation stored transformed translation
  directly into `m2->t`, then added `m0->t`. With `m0 == m2`, it had already
  destroyed the original translation. Synthetic input expecting
  `[110,220,330]` produced `[20,40,60]`.
- **Fix/proof:** durable patch `psycross_compmatrix_alias.patch` buffers the
  transformed result and original translation before writing `m2`, matching
  retail ordering. Distinct output, `m0==m2` with zero/nonzero `m1.t`, and
  `m1==m2` all assert `[110,220,330]`. Fresh-worktree apply/reverse checks pass.
- **Live exposure:** zero aliases across 3,493 calls in 60 frames each of Maps
  0, 1, 14, 47, and 334. Existing tripwires remain exact: Map014
  `74/89/87/73`, Fei `41`; Map47 1,239 parent compositions; Map334 236 mode-1
  calls.

## 2026-07-13 — Build-path integrity and passive animation-opcode survey

- **Build integrity defect:** `pc_port/build_port.sh` suppresses compiler
  errors for game TUs, marks them skipped, and still links with stale/generated
  stubs. The normal build currently skips member-change menu misc, shop-menu
  misc, `system/sound.c`, and `system/work_list.c`; `LINK OK` is not proof all
  intended game code executed.
- **Stub convergence defect:** without matching ELFs, the driver reuses
  `stubs.c` and only prunes collisions. It does not generate stubs for the
  current undefined set or iterate to a fixed point.
- **Opcode trace:** `XENO_DIAG_OPCODE_SWEEP` uses launcher-preopened fd 3 and
  fixed 16-byte records; the launcher writes an `END!` sentinel after its
  watchdog. Passive 10-second Map000/001/014 starts yielded 16/97/8 dispatches
  respectively; none were `0x85,0x8E,0x98,0xBE,0xC8,0xD4,0xE2,0xFA`. Target
  progression remains unswept.

---

### [2026-09-02 00:45] World map: Fei/follower/objects invisible — slot-9 camera never published D_8009BE28
- **Hypothesis:** (inherited from the interrupted session) the sprite pass `wm_80085CDC` subtracts the world position block `D_8009BE28` from every object, and a probe showed that block reading zero all session, pushing sprites behind the camera. Retail must write it from the on-foot lane; find the writer.
- **Scope:** `pc_port/src/world_map_callback_914d0.c` (slot-9 on-foot camera, retail `[0x800914D0, 0x80091B54)`). Investigation only elsewhere.
- **Change made:** Retranscribed the callback against the asm and both jump tables (`0x80070BE4`, `0x80070C0C`). Retail's common tail `0x80091B04` — `wm_80093354(slot+0x28)` then copy `slot+0x28..+0x34` into `BE28..BE34` — was entirely absent; the post-dispatch follow logic (state gate, 1/8 far-branch step, Y easing 1/8 vs 1/16), the 0x00FFFFFF heading-accumulator mask, the inverted ±0x180 clamp in states 2/16, the state-1 fallthrough into the heading step, and pre-dispatch case 0's `D_8009D52C` source were also corrected. Single source commit; no config/build changes.
- **Build result:** `./pc_port/build_port.sh` LINK OK. `make check` 4/4 FAILED before (HEAD `9d5ff780`, via stash) and after, identical hashes — pc_port is not in that build. Note member/shop were OK in the last logged baseline and now fail at HEAD before this change (likely `2261d026`, menu TUs).
- **Runtime result:** probe on `wm_80085CDC`/`func_8001E298`: `BE28` went from `(0,0,0)` to `(122665808,-1278838,45360304)` and tracks slot 1 with the 1/8 step; Fei's render matrix translation went from `(-9818,4501,-2610)` to `(5,-3,1117)`. Captures at world frames 60/120/180/240 show Fei and the follower walking/standing, plus the Lahan village model, trees and the Mountain Path landmark (all previously missing). `run_w34n124_world_walk_entry.sh lahan|mountain` both 6/6 PASS.
- **Proven:** the missing tail was the blocker for every camera-relative renderer on the on-foot route (sprites, scaled objects, view matrix height term); retail writers of `BE28` outside this tail are all vehicle/event-mode callbacks or the `C894 != 0` restore copy.
- **Not proven / still open:** hard-edged white rectangles over the terrain in frames 120–240 (cloud/shadow tile layer or paging tile fault; pre-existing, separately recorded). No standalone certificate for `wm_800914D0`. Evidence: `docs/evidence/w34n125-world-sprites-visible/README.md`, artifacts `scratchpad/w34n125_sprite_capture/`.
- **Committed:** `618726b6` (source); docs follow in a separate commit.
- **Stop reason (if stopped early):** —

---

### [2026-09-02 02:30] Retail boot path: state 6 movie player, opening STR, hand-off to Map 0
- **Hypothesis:** the port's title/menu (`PcPort_BootMain`) is a stand-in; retail boots splash → `ChangeGameState(6)` (movie.bin) → opening STR → `FieldMain` with the new-game template's map. Decompile movie.bin and the movie player module and wire the retail sequence.
- **Scope:** new `config/movie_player.yaml` + symbol seeds (movie.bin and the 0x18/1 module were never split), `src/movie/main.c`, `pc_port/src/movie_player.c`, two PsyCross patches (`psycross_cd_stream_movie`, `psycross_display_present`), `psyq_compat.c` Vsync, `port_main.c`, `game_overrides.c`, `config/symbol_addrs.slus_006.64.txt` (+2 BSS names).
- **Change made:** eight commits, one logical unit each (see `docs/evidence/w34n126-retail-boot-movie/README.md`). Movie player: Square layer + cdstream ring + handwritten `DecDCTvlc` transcribed; MDEC is software from psx-spx with the module's tables. Boot: retail `func_80019578` tail, `func_8001BB50` template load, state 6 → `MovieMain`, MAP14 default removed, roster/skin stand-ins made harness-only (their pinned heap blocks broke MovieMain's `0x801D3000` module placement).
- **Build result:** LINK OK after each step. `make check` 4/4 FAILED with identical hashes before/after every config change.
- **Runtime result:** opening movie plays through the retail path — 233 frames, ~15-18 fps, zero skipped frames — then `FieldMain` map 0. Captures at frames 10..230 show the anime opening in 24-bit. W34N124 lahan harness 6/6 after the change.
- **Proven:** retail has no title game state; state 6 plays movie `disc+2` of dir 0x18/1 and returns to state 1 with `D_8006F94E` from the template (0 on disc 1). The player's ring needs real-time sector pacing and a VRAM-display present, neither of which PsyCross had.
- **Not proven / still open:** what retail's map 0 script shows (title menu vs the debug-room look the port renders); New Game → opening field scenes; XA audio; bit-exact MDEC; frame-80 vertical striping origin; dead `PcPort_BootMain` cleanup.
- **Committed:** `6432959f a177ca62 f03b1747 e7a4b4f6 7991642c 3d107600 847520e9` + overlays.yaml comment fix.
- **Stop reason (if stopped early):** —

## 2026-09-02 — Map 490 title: FE61 stall → FE57 open

**Symptom:** Boot reached Map 490 but never FE57 / title; New Game inject timed out.

**Cause:** Attract path runs FE60 → `func_800A7C58` → FE61 waits on `D_800ADB7C`. Stubbed `func_800A7C58` never set the flag (retail asm `800A7E58` stores 1), so FE61 spun and never returned to OP31 pad check (`PADRright`/`0x20` → title at FE57).

**Fix (incomplete):** `XENO_PC_PORT` minimal `func_800A7C58` in `src/field/main/misc5.c` sets `D_800ADB7C=1` (+ prologue clears). Full archive/VRAM body still TODO.

**Runtime:** Circle inject → FE57 → `func_801C58EC` title loop. Then SIGSEGV in `func_801D3B00` (null `pManager` / window slots) after stubbed `func_801C6F70` / `func_80036410`.

**Note:** `XENO_FIELD_TEST=1` forces Map 0 developer path — do not use for title boots.

## 2026-09-02 cont — title opens; Menu offset SEGV fixed

**A7C58:** minimal port sets `D_800ADB7C=1` → Map 490 attract leaves FE61 → Circle (`PADRright`/`0x20`) → FE57 → `func_801C58EC`.

**SEGV:** host `Menu` inflated (`pManager` @0x3F8 not 0x33C). Bulk-replaced raw `+0x33C` fetches with `g_Menu->pManager` (+ fixed `MenuManager*` byte arith). Title loop now runs; idle timeout path observed (`D_800594D0=1`).

**Next:** inject New Game during `func_801C7BF4` frames (enter BP is one-shot); then Map 4. Still stubbed: `func_801C6F70`, `func_80036410`, several draw helpers.

**Runtime-verified (`/tmp/wm/title_ng4.log`):** Circle → FE57 → `func_801C58EC` → New Game (choice 2, confirm injected at `func_801C7F34`, after the input reader) → `func_8001B970` → `FieldLoad map=4`. `[SUMMARY] fe57=1 title=1 ng=1 map4=1`. Inject must occur after `func_801C7D78` (it overwrites `g_Menu->input` each frame).

## 2026-09-02 — Map 4 is the opening prologue narration (advances on Circle)

**Verified chain:** title (Map 490) → New Game → `FieldLoad map=4` (`/tmp/wm/title_ng4.log`).

**Map 4 identified:** dialog section (archive map 4, section 0x14C/0x128) decodes to the retail prologue text — "The continent of Ignas…", "…the Qislev Empire… desert kingdom of Aveh", Ethos, Gears. Screenshot `/tmp/wm/map4_wait_real.png`.

**Not a hang:** `PsyX_Sys_GetVBlankCount` keeps rising (2908 → 4868 over 40s); main thread sits in normal `Vsync`/`GR_StoreFrameBuffer` present. Map 4 script waits on `OP31 mask=0x0020` (Circle) — retail "press to advance narration".

**Pad injection must land at VM sampling time.** Setting `D_800AFE9C` at an arbitrary stop is clobbered by the frame's `ControllerPoll`. Injecting on `FieldScriptVMRun` entry works: with pulses, actor 0 advances 72→75→84→96→99→108→120→126 and narration actor 12 walks 5126→5143→5160 (`/tmp/wm/map4_pulse.log`). Prologue has many pages, so full playthrough needs a longer/faster driver than GDB-per-VM-run.

**Known port issue seen in the screenshot:** narration pages overlap / draw out of order (page clear between `{00}xx` waits looks wrong). Separate bug from the advance mechanism.

**Next:** drive field input via the port's own `PcPort_TestInputInject` harness (as `world_map_main_loop_71034.c` does) instead of GDB, then run prologue to completion and capture the map chain into the fire scene.

## 2026-09-02 — Prologue completes; Map 4 -> Map 2 is the Lahan opening scene

**Field input harness landed (DIAGNOSTIC).** `XENO_FIELD_TEST_INPUT` (frame:value schedule, same syntax as `XENO_WORLD_TEST_INPUT`) merged in `FieldPollControllers` (`src/field/main/misc2.c`) at retail's post-drain accumulator seam, deriving held/pressed/released edges. Deliberately NOT `XENO_FIELD_TEST`, which also selects the developer KernelMenu boot path. Field schedule gets its own 4096-step capacity (`FIELD_TEST_INPUT_MAX_STEPS`) because the prologue needs hundreds of confirms. Remove harness + call site together.

**Chain proven:** `XENO_FIELD_MAP=4` + Circle schedule runs the prologue to completion and the game advances itself: `FieldLoad field=4` -> `FieldLoad field=2` (`/tmp/wm/map4_fti2.log`). Earlier 128-step cap stalled input at frame ~1806; that was the harness limit, not a game bug.

**Map 2 identified = opening fire scene (Lahan attack).** Dialog decode: Citan "Are you alright?! ... Do you not know how worried Alice and Timothy are?!", "Let us evacuate to a safer spot while Fei has their attention!", Dan, Fei. 52 entries. Direct load builds the full scene: 104 actors, 65 models, player=1, geometry submitted (`/tmp/wm/m2b.log`).

**Open blocker: heap-corruption abort shortly after Map 2 loads.** glibc `malloc_printerr` -> abort inside `alSource3f`, reached from `PsyX_SPUAL_SetVoiceAttr` <- `SpuSetVoiceAttr` <- `PcPort_SpuRegFlushTick` (port_main.c:537, continuous attr flush) on the PsyX 240Hz interrupt thread. Reproducible at the same point with `ALSOFT_DRIVERS=null`, so it is a real heap overrun being *detected* by OpenAL's allocator, not an OpenAL bug. No ASAN regime exists in `build_port.sh` (only `XENO_TSAN`); adding one is a build-config change and must land in isolation.

**Also open:** prologue narration pages overlap / draw out of order (page clear between `{00}xx` waits), and the `[variant5]`/`[field-diag]` printf volume makes GDB-driven runs crawl.

## 2026-09-02 — ASAN regime added; two real out-of-bounds writes fixed

**Build config (its own commit):** `XENO_ASAN=1` in `pc_port/build_port.sh`, mirroring the existing `XENO_TSAN=1` regime — separate dirs (`pc_port/build_asan`, `pc_port/build_native_asan`), mutually exclusive with TSAN, normal artifacts untouched. `-static-libasan` is required, not cosmetic: the prebuilt SDL2/OpenAL in `xenogears-assets/lib` load ahead of a dynamic libasan and trip ASan's "runtime does not come first" bail-out (`LD_PRELOAD` does not fix it).

**Bug 1 — pad buffers (`pc_port/src/data_controller.c`).** ASan: global-buffer-overflow WRITE at `PsyX_pad.cpp:133` (`PsyX_Pad_InitPad`), 3 bytes after `g_C1Buffer`, 29 before `g_C1ButtonState`. Retail has `g_C1Buffer` @0x800625FC and `g_C2Buffer` @0x8006261E — one contiguous 2 x 0x22 region, and `controller.c` indexes it as a single array (`controllerIndex * CONTROLLER_BUFFER_SIZE`). The stub generator sized `g_C1Buffer` at 0x20 (label is 0x01, generator floor is 32) and gave `g_C2Buffer` separate storage, so slot 1 registered past the object end and both PsyCross and the game read/wrote slot 1 out of bounds. Fixed with one 0x44 blob plus `.set` aliases for every splat-split interior label (same idiom as `data_game_state.c`).

**Bug 2 — heap user table (`pc_port/src/data_heap.c`).** ASan: global-buffer-overflow WRITE of size 8 at `memory.c:492` (`HeapChangeCurrentUser`), 0 bytes after `g_HeapUserContentNames`. Retail reserves 0x28 = 10 x 4-byte PSX pointers; host pointers are 8 bytes, and the stub reserved 32 bytes = 4 host entries. `MovieMain` uses tag 4 and `HeapResetUser` uses `HEAP_USER_UNKNOWN` = 0xA every MainLoop iteration. Sized to `HEAP_USER_TEST + 1` (0xC) host pointers; `HEAP_NUM_USERS` deliberately NOT widened (it belongs to the matching build). Note retail's own 0xA store lands on `g_HeapDelayedFreeBlocksHead`'s first word — incidental aliasing that host pointer width cannot reproduce anyway, and nothing reads it back except `HeapPrintBlocks`.

**Still open — wild write on Map 2.** Map 2 corruption pre-dates both fixes (it was the earlier glibc-heap abort seen via OpenAL). After the fixes the symptom MOVED rather than disappeared: normal build now SEGVs in `ParsePrimitivesLinkedList` from `FieldDisplay`'s `DrawOTag(ot3 + 7)` with repeated "ptag length is not valid" — i.e. the same wild write now lands on the ordering table instead of the host heap. ASan cannot see it: the target is inside the emulated PSX RAM blob, and intra-blob overruns are invisible to it. ASan under Map 2 also hits its own artifact (its allocator defeats the port's deliberate sub-4GB pointer truncation), so the hunt needs a GDB watchpoint on the corrupted OT word, not more ASan.

**Note:** Map 4 (prologue) and the title path are unaffected; only Map 2's 104-actor scene trips this.

## 2026-09-02 — ROOT CAUSE: missing D_8004FE50 row 4 variant 4 (Map 2 crash) — FIXED

**Not a wild write.** The OT diagnostic (temporary, in `PsyX_GPU.cpp` `ParsePrimitivesLinkedList`, labeled `[xeno-ot]`) reported the bad packets as `len=9 code=0` — POLY_FT4-sized packets whose code byte was never written, reached via a previous tag whose word0 was `0x006d45b4`-style (len 0, addr24 into the field heap).

**Mechanism:** `temp2.c:623-636` skips a mesh group when `D_8004FE50[prim].proc[variant]` is NULL, but the BUILD side (`temp2.c:749`, buildProc) has already linked that group's packets into `ot3` with tag lengths set. The walker then parsed uninitialized packets, reported "ptag length is not valid", and ran off the chain. Map 2 logged `missing D_8004FE50 prim=4 variant=4` repeatedly.

**Evidence for the fix (no guessing):** read retail's table straight out of `disc/SLUS_006.64` .sdata at 0x8004FE50 (file offset 0x800 + vaddr - 0x80010000), rows of 0x28:

    row 0x04 @8004FEF0
      proc  = 8002E038 8002E038 8002E470 8002E8DC 8002E038 8002E038
      build = 8002CF34  strides = 0x8,0x4,0x14

Variants 1/4/5 are the SAME walker as variant 0 (0x8002E038 = `ModelPrimTriSmallAverageVariant0`, already ported, per row 0x00's own comment); only variant 3 (0x8002E8DC) is unported, exactly as row 0x00. `pc_port/src/game_overrides.c` row 0x04 updated accordingly.

**Result — Map 2 stable:** zero `missing D_8004FE50`, zero OT errors, no SEGV/abort across a 120 s run (`/tmp/wm/map2_row4.log`). The earlier glibc-heap abort in `alSource3f` and the `ParsePrimitivesLinkedList` SEGV were both fallout of this one gap.

**END-TO-END VERIFIED** (`/tmp/wm/full_chain.log`): boot -> movie -> title Map 490 -> `func_801C58EC` -> New Game -> `func_8001B970` -> prologue Map 4 -> Lahan Map 2. `[CHAIN-DONE] title -> New Game -> [490, 4, 2]`, exit 0, no crash.

**Visual evidence:** `/tmp/wm/vis_title_real.png` — title menu renders retail's three entries (New Game / Continue / Sound) with ball + red arrow cursors, cursor on Continue (choice 1 default, matches the log). `/tmp/wm/vis_fire5_real.png` — Lahan village renders (houses, well, watchtower, foliage, a character sprite).

**Remaining accuracy gaps (goal NOT complete):**
1. Title背景/logo art missing — screen is black behind the menu (1.9% non-black). Stubs still on that path: `func_801C6F70`, `func_80036410`, `func_801D02D8`, `func_801CF37C/5E4/8D8/FB48/FF64`.
2. Prologue narration pages overlap / draw out of order (page clear between `{00}xx` waits).
3. Map 2 captures show the village pre-attack; the burning/fire beats of the scene are not yet confirmed on screen.
4. Other `D_8004FE50` rows still hold NULLs where retail reuses ALREADY-PORTED walkers (rows 0x02/0x06 = 0x8002E024, 0x07 = 0x8002E010, 0x08/0x0C variants, 0x0B/0x0F = 0x8002E22C, 0x10 = 0x80030750). Same latent OT-corruption class; fill from the disc table as maps hit them.
5. `[xeno-ot]` diagnostic in `PsyX_GPU.cpp` must be removed once this settles.

## 2026-09-02 — Title backdrop compositing restored; Map 490 snapshot is empty

**Fix (compositing path):**
1. `MenuMain`: drop the port-only forced `isbg=1` (retail is `isbg=0` so the field snapshot shows under the OT). Keep `isbg=1` only when `g_MenuDebugEnabled`.
2. `func_801C7BF4`: re-enable retail `MoveImage` of `(704,256)` under the menu OT (PsyCross `_xeno_fb_materialize` blits into the GL backbuffer).
3. Port `func_801C6F70` under `XENO_PC_PORT` (dim POLY_F4 + DR_MODE + highlight shells) and unstub `func_801D1258` to AddPrim those typed fields.
4. Port `func_80036410` (controller stack-full flag) under `XENO_PC_PORT`.

**Runtime:** title loop survives to idle timeout (`MenuMain returned D_800594D0=1`). `func_801C6F70` no longer stubbed.

**Proven gap:** after MoveImage, CPU VRAM sample shows `dest_nz=0` AND `src704_nz=0` (`/tmp/wm/title_diag2.log`). The offscreen snapshot itself is empty — Map 490 is not putting title art into the framebuffer before `func_800A4748`. Compositing is no longer the blocker; field title rendering is.

**Note:** `PsyX_TakeScreenshotPath` deadlocks when called mid-`func_801C7BF4`; use VRAM sampling or SIGINT-stop captures instead.

**Next:** why Map 490 (0 models, 1 actor/sprite, field-0bb TIM drain) leaves a black frame at menu-open — sprite/TIM draw path for the title logo.

## 2026-09-02 — Title backdrop: horizon draws but GL FB stays black
- VRAM sample (pre-horizon): `src704_nz=0` — snapshot empty.
- Ported `func_8002709C`/`273C4`/`278F8`; fixed `spanPx` (`>>8`→`<<8`). FT4 geometry now `(0,0)-(256,224)` tpage=`0x108`.
- Textures live: `tex512_nz=230` at VRAM (512,0). Field-0bb 0x48d uploads OK.
- A476C materialize (`GR_StoreFrameBufferImmediate` of field clip) runs, but `XENO_MAT_DIAG nz=0/71680` — GL backbuffer is opaque black. So the gap is **horizon/OT not reaching the presented framebuffer**, not MoveImage compositing.
- Next: verify horizon `AddPrim` OT (`ctx+0x40CC+D_800B21D4*4`) is linked into `DrawOTag(ctx+0x80F0)` chain; check textured split path for tpage 0x108.

## 2026-09-02 — Prologue text boxes: page overlap + missing rows (retail-accurate now)
- **Symptom:** Map 4 narration showed one line per row-pair, pages stacked/never cleared (`/tmp/wm/map4_wait_real.png`).
- **Root cause 1 (rows):** port commit 303c83c7 added a `memset` of the row scratch at row-start in `func_80033DF0`. Retail asm (80033E30-80033E6C) has none. Rows 2k/2k+1 share one 13-line VRAM strip via the two nibble planes of that scratch, so the memset wiped the partner row before the odd row's LoadImage. Removed. (Alloc-time memset kept: retail heap contents are unspecified there.)
- **Root cause 2 (pages):** C `func_80033DF0` mapped code `02` to retail's `03` (wait, keep page) and lacked `02` = wait + `flags|=0x48` → `0x40`→`0x20` row-reset in `func_80034888` once the wait releases. Only 3 of 16 `0F` subcodes were handled. Ported the interpreter faithfully from asm (jtbl_80018A7C): nested strings, item/weapon/char/button names, numbers, speed/delay codes, budget accounting per label.
- **Also:** `func_80033CF0` signature was wrong vs asm (a0 value, a1 font row, a2 signed); sign glyphs inverted; scan started from an unset slot. Rewritten; port uses one contiguous 12-halfword scratch (sentinel/digits/terminator) since the three symbols are separate stubs on the host.
- **Runtime:** title → New Game → Map 4 → Map 2 chain still completes (`/tmp/wm/txt_chain2.log`). Prologue renders full 12-line page 1, clears, page 2 types in; "The remote village of Lahan" caption OK. Evidence: `/tmp/wm/evidence_txt/frame-004950.png`, `frame-006570.png`.
- **Open (separate):** 1-window-pixel white seam at texel 64 of each row sprite (screen x=88 for the prologue box), also in July captures. Row scratch dump shows no content there (plane B col 16 = 0 with a space glyph) → PsyCross sampling artifact, not text-engine. Not chased.
- **Title backdrop (parked):** horizon 2709C/273C4/278F8 ported (+ spanPx `<<8` fix), textures at VRAM (512,0) present, FT4s submitted, but materialized GL FB is black at A4748 time. `A476C` materialize + GL_FRONT readback experiments did not change `src704_nz=0` and were reverted.

### [2026-09-02 19:30] Text seam + Lahan dialog verification attempt
- **Seam hypothesis (disproved):** PsyCross `MakeTexcoordRect` clamps a 256-wide SPRT's texcoord extent to 255 while the quad stays 256 px; predicted the half-texel drift crosses a boundary at sprite texel 64. Implemented an exact-extent variant (`a_extra` half-texel offset on right/bottom vertices) — seam unchanged at window x=176 (`/tmp/wm/cap_seam/frame-003650.png`, `y=107`: 1 px body colour, then 4 px outline, then 'o'). Reverted. Buffer content at that column is transparent, so the source is elsewhere in the VRAM→GL path (partial upload / texture cache?). Still open.
- **Lahan dialogs:** `XENO_FIELD_MAP=2` direct load renders the fire-lit scene but never advances the script over 2963 field frames / 20 min (frames 25460 and 56260 identical, camera inside geometry); dialog boxes not reached. The full title→4→2 chain does reach Map 2 but at ~1-3 fps under the gdb title harness it exceeds a 400 s window before dialog. Prologue (Map 4) is the verified text-box case for now.
- **Capture gotcha:** `XENO_CAPTURE_EVERY` fires only when the VBLANK count at EndScene is an exact multiple; at low fps use a small interval (10).
- **Tree note:** another session is modifying `world_map_capture.c/h`, `world_map_helper_89c78.c`, `rendering.c` concurrently and ran `build_port.sh` in parallel (broke one of my links mid-build). Not touched here.

### [2026-09-02 21:00] Zeboim sky bridge (Map 383, Id-fight map) loads in port
- **Identity:** the on-foot Id boss happens on the big spanning bridge over Zeboim (Zeboim Ruins; Citan/Elly/Bart, HP 3000). TCRF debug-room list names it `383 Zeboim - Sky Bridge`, flanked by 382 crossroad / 384 hallway. Confirmed the debug number == field map ID empirically: `XENO_FIELD_MAP=383` loads from disc 1 (archive 0x3b7, 323584 bytes, 158-sector TIM drain `done=1`).
- **Render (verified visually):** bridge deck/girders/railings, party actor sprite (green, actor[2] at faPos=(-884,-8,0)), orange sunken-city backdrop above and below. Captures: `/tmp/map383_cap/field-frame-037800.png` (entrance 0). Zero OT/walker faults in log; only pre-existing one-shot `[stub] func_80028B14`.
- **Open accuracy item (not chased):** large pure-black quad region mid-frame (x170-470, y55-175, >50% pure #000000 with girder beams drawn over it). No walker/OT errors, so not the Map-2 missing-row class; could be the unlit far span (night scene) or a missing background layer. No retail reference reachable from here (lparchive/rpgclassics time out) to decide.
- **Entrance note:** entrance 0 spawns on the bridge with a good camera; entrance 1 is black from the first frame (invalid direct spawn / camera in geometry). Harness uses entrance 0.
- **Inserted into the port:** `run_case Map383 383 0 25` in `pc_port/tests/run_field_map0_smoke.sh` (generic OT/actor gates + 383 branch: FieldLoad identity, archive drain, presented OT, plain actor draw — all 6 verified against a live log). New dev capture hook `PcPort_FieldCaptureOnVsync` (`game_overrides.c`, called from `Vsync` in `psyq_compat.c`): `XENO_FIELD_CAPTURE_DIR` + `XENO_FIELD_CAPTURE_EVERY` (default 60), same present boundary as the world-map capture. Strictly env-gated.
- **Battle status (out of scope):** the field->battle handoff `func_80281204` is an explicit no-op stub ("named next blocker"); the Id boss fight itself cannot run until the battle system is ported. Map load + render is the delivered slice.
- **Sandbox quirks hit:** uutils `timeout` cannot exec here (used setsid+sleep+kill instead); `SDL_VIDEODRIVER=x11` segfaults instantly in this sandbox even for Map0 (environmental, pre-existing — script keeps the repo's x11 convention for CI); `pkill -f` with the binary path in the same command line kills the invoking shell (use `pgrep -x`/`pkill -x`).


### [2026-09-05] Opening Gear battle continuation: retail data before presentation
- Resumed account-limited work at HEAD 3a3e7aac03a2f166fb924945a489e392d706f282; preserved preexisting dirty work and all local payloads. User requires retail scripted opening, including setup/camera/sequence/HUD.
- Gear loader 801E742C now uses the retail latched texture-archive skeleton list. Independent retail differential test: 48 cases per O0/O2/UBSan and seven rejected mutants. Normal opening field objects now have the retail 47/52 nodes; live topology checked. This alone did not resolve the battle.
- Battle dynamic trig bindings now honor retail sine/cosine entry meanings and mask scalar angles before generic pointer translation. 139264 bridge comparisons and 8192 table comparisons per O0/O2/UBSan; four mutants rejected. Native build passes. Black ordinary-HUD battle remained, so this was not visual acceptance.
- Retail FieldLoad call at 80071054..6C revealed the encounter data was decompressed at destination+0x10 instead of base; +0x10 belongs to the discarded size argument. Map2 selector1 now yields flags A0, asset12, record byte3=01. Full 530-byte output/guards match executed retail call/wrapper/decompressor, eight cases per mode, four mutants rejected.
- Native 8001BB0C consumed a separate generated D8006F9DE instead of guest active record+2. PC-only guest-RAM read fixes the boundary; matching source expression unchanged. Production bridge/retail-copy test passes O0/O2/UBSan, with three mutants rejected at each mode; existing caller256/loader953 cases per mode pass.
- Fresh normal New Game run opening-battle-encounter-retail-li37dy5e receives exact record/index18. Citan formation now0 and X/Z3440/1480. Frame2 has371 Gear-buffer packets, but no subsequent visible-frame acceptance: native HeapAlloc enters error130 loop. Next same-build run opening-battle-script-alloc-khl4t18s captures oversized allocation arguments/guest CPU/RAM read-only. No gameplay-state forcing.
- Matching make check was run in isolated before/after copies and fails on the same preexisting stderr declarations in temp1/animation_scripts. temp3 matching assembly unchanged. No full binary match claimed. No commit/push. Latest report: docs/evidence/opening-battle-encounter-20260905/README.md.

### [2026-09-05 22:04 UTC] Scripted Gear progress and host speed repair
- Corrected HeapAlloc bridge return domain at retail80031BDC: heap pointer must be guest-domain before retail reserves the special overlay by subtracting801E5000. Exact prior bad6480104 request reproduced; 54 cases/mode and three controls pass.
- Restored A1 vertical-velocity command and active retail BA8F4/BC158 ground-height/child-ownership hooks. User screenshot confirms scripted Gear close-up/pilot panel without combat bars. Full sequence remains open.
- B4 stack-command and billboard callback/body corrections pass independent real-retail tests; later BB signed depth-bias command passes all halfword/byte pairs. E5 random script-byte assignment is implemented and independently tested, awaiting next native build/runtime. Evidence and proof boundaries are in opening-battle-encounter-20260905 report.
- User notes Fei's text missing. New type5 registration/draw snapshots precede E5 failure. Types8/9/15 are not requested in this run. Do not force main Font rendering or infer text ownership from unused callbacks. A wrong scratch E5/ScaleMatrix interpretation was caught against actual retail bytes and retracted.
- Added host1x–5x toolbar/F11/Shift+F11/heldBackspace/XENO_SPEED controls. Device clock/CD/audio differential fixture and five controls pass, but user observes music speeding while game stays near normal. Gameplay speed acceptance FAILED. Real GDB profile identifies unconditional framebuffer download/conversion as largest measured cost; game clocks speed correctly. Uninstrumented user-selected5x field4 intervals have median~32.13 updates/s before the graphics repair, far below a5x field rate.
- Luna worker owns a bounded deferred-framebuffer repair and regressions; root retains build integration, correctness review and live speed verification. Must preserve pending pages and all CPU-read/write/MoveImage/StoreImage/DR_MOVE coherence. No game-frame skipping or script-state forcing.
- HEAD unchanged; no commit/push. Source/patch before-images and exact run helpers under /tmp/xeno-gear-resume-20260905-l12d_42r. Current status: docs/evidence/host-speed-20260905/README.md. Verify named process/executable ownership before further UI or runtime actions.

### [2026-09-05 23:03 UTC] Retail NTSC clock and C1 scatter
- User authorizes automatic cost routing: Luna for bounded work/tests, Sol when useful for moderate work, Astra for hard diagnosis/coding/retail review. No global model configuration or memory files changed.
- Rejected deferred framebuffer cache: it still downloaded superseded pages, and raw RGBA blits did not maintain packed RGB555 GPU encoding. Saved draft/RED tests outside production; restored exact renderer fab65768d65e5da211d983ceba1f09ec47c0d800832f73204b1d6cdeafa87288. Renderer-only O2 reduced conversion0.266->0.126ms but not field throughput; reverted. Temporary timing/build flags removed.
- Root cause of below-retail speed: native bootstrap left PsyCross g_vmode=-1, selecting PAL50Hz. Real5x clock measured249.84Hz. Retail SLUS initializes g_VideoMode80058990=0; SetVideoMode(0) now precedes PsyX_Initialise thread creation. Startup RED/GREEN and missing/PAL/late controls pass O0/O2/UBSan.
- Normal New Game wall-clock verification: 1x59.999VBL/s and29.990field4frames/s;5x299.926VBL/s and149.091field4frames/s; ratio4.971x. Runs host-render-profile-1-vpk5f9yb and host-render-profile-5-58bdvk27; source/run pins and comparison in host-speed report. No debugger/game-state writes/skipped updates.
- E5 now built/runtime; C1 exact retail scatter implemented. Initial host rotation differed by1unit. C1-only zero-Z matrix follows signed Q12 instruction order including negate BEFORE shift; production951dbcda616d0fd3ceda15fc115c91dbec3b8e64a686523766d2e75b7992f7b0. C1 QbzoMKZ9 passes1,048,577cases/mode O0/O2/UBSan,163,840aliases and13controls, with shared GTE backend boundary explicit. Prior animation command suites green with fail-fast C1 leaf guards.
- Final normal native build8827e71ceda0adad0802b3b96d81ec2c7cd2703b791000b534afda84c2f1f40f passes; latest opening-ntsc-c1-5-jw5zz2qe enters retail battle then aborts unsupportedAC172,index34,sprite007DFEF8,ops0079EDDB. This RNG route is not live C1 proof. AC/full scripted exit/Fei text remain open.
- Isolated matching check remains baseline RED for undeclared stderr in animation_scripts/temp1. No ASM/BINARY-MATCH claim. No commit/push. Interactive build reopened at1x without synthetic inputs; newest launch metadata is /tmp/xeno-interactive-ntsc-launch.json.

### 2026-09-05 23:22 UTC — AC retail handler and velocity helper

Implemented AC after full-dispatcher semantic RED. Corrected shared 22974
R3000 wrapping arithmetic, signed DIV0, direct masked trig indexing and
negation-before-shift. Initial 4,616,193 cases per O0/O2/UBSan pass; final
angle-domain/negative-control extension pending. C1/E5/8C regressions pass.
Native LINK OK binary b373a556c304a9136986cb940b95386f140dca90be0534907f30310b755490b4;
scratch matching remains baseline stderr RED. Normal 5x opening replay
opening-ac-5-khm2410x passes AC, naturally aborts C4 raw196/index58, sprite
7DFEF8 operands79EDDD (same file50 bytecode, offset568). Last capture5640
shows Gear scene; text/full exit still unresolved. Sol read-only dialogue
audit identifies special archive pair50/51, not generic FontDrawLetters.
Luna owns remaining tests; Astra auditing C4 matrix/ApplyMatrixLV details.
All original dirty work preserved, no commit/push. See dated evidence report.

### 2026-09-05 23:32 UTC — C4 exact split/GTE velocity rotation

C4 retail RED then 71,529 cases per O0/O2/UBSan pass; six mutants reject.
Astra's source audit establishes exact Z-only native RotMatrix and the
retail ApplyMatrixLV two-pass semantics. The scoped native helper uses
actual GTE operations, preserving INT_MIN, signed16 IR, MAC and FLAG.
Native build LINK OK (e30c36...); scratch matching retains the existing
stderr RED. Ordinary 5x replay opening-c4-5-0bdj7vdk passed C4/B5 then
naturally stopped at BA186/index48, sprite7DFEF8, operands79EDE1. BA calls
existing 23290 blend/type helper; Luna is preparing RED. No full text/exit
proof. AC's 12 controls completed, including the isolated negation-order
refinement. Neighbor regression/linkage checks continue after the new C4
GTE references. No commit or push.

### 2026-09-06 UTC — BA, 91, F1 and model routing

BA actual-helper/leaf differential fixture passes 94,208 cases per mode
and seven controls; 91 passes65,536 and five controls; final F1 fi0jDt4b
passes19,809 and nine controls at O0/O2/UBSan. F1 oracle executes actual
retail instructions after rejecting an intermediate native-leaf substitution.
Native F1 build LINK OK; isolated matching check retains baseline stderr
failures. Latest normal replay opening-f1-5-n16qbtw0 has ended at F2
raw242/index104, operands79EDED. No extra pilot-confirm input was logged
by root for this replay. Text/full exit remain unresolved. User reaffirmed
automatic cost-conscious task-agent routing: Luna routine, Sol moderate,
Astra difficult retail semantics/code; escalate on evidence then downshift
for routine follow-up. Root handoff updated. No commit or push.

### 2026-09-06 UTC — F2 color routine, E7 and next AA

F2 handler B7w8w0ws passes287,288 cases per O0/O2/UBSan and fivecontrols.
Shared native B2AEC primitive_colors.inc is included by the battle decomp
and game_overrides; no buildconfigchange. Root corrected its test oracle
to execute actualretail clamp instructions and repaired actualsourceoverlap
coverage:5ymTLD4m passes4,230 cases per mode and sevencontrols, superseding
GRzICq01 native-clamp boundary. NativeF2 run passedF2 and stoppedE7.
E7 semanticRED WXnNkARa then Jlf177YE132,073 cases per mode/sixcontrols;
installedsource exactlyequals testedcandidate. NativeE7 LINK OK26a63852...;
isolatedmatching remains baseline stderrRED. Normalrunopening-e7-5-ycuq446v
passedE7 then AA170/index32 sprite7E1714 operands7CF699; pilotpanelvisible
withoutletters. F2 regression9TFsncPQ fullypasses afterE7. Luna preparing
AA RED; rootcandidate scratch-only. Sol owns isolatedPCSX retailreference
restart/input investigation. Fullopeningexit/textunverified. No commit/push.

### 2026-09-06 UTC — AA verified; callback ABI investigation

AA is installed with the retail signed-byte operand, signed scale, truncation
toward zero, existing 22CAC helper, and wrapping 16.16 position addition at
sprite+4.
RED `sprite_dispatch_aa_retail_test.0u819eSn` precedes GREEN
`sprite_dispatch_aa_retail_test.jxXDWimd`: 201,216 cases per O0/O2/UBSan
and six controls. The oracle executes the actual retail helper; its bridge
only observes arguments. Full independent operand, scale and timer domains
are combined with finite edge positions and four operand aliases; this is
not exhaustive coverage of their Cartesian product.

Native build `opening-aa-port-build-20260906.log` is LINK OK, executable SHA
`a2b0826171ab50f36ea4e3c0af6fedd951601a4975e654d96beab8d2232066ce`,
animation source SHA
`e423b31fa6b7cb881f677a141b8dfa7c4d4ebdafdc3dc177639cb4316241bff7`.
The isolated matching check retains the baseline undeclared-stderr failures.
Normal New Game run `opening-aa-5-0rmj97mi` passed AA and stopped with
`unresolved native call target=0x007fc208 guest-pc=0x800c1e6c` inside
callback800C11CC. Last frame6660 shows the Gear scene; no text/exit proof.

Retail800C1E5C loads sprite+68, then JALR at800C1E6C. Astra's read-only
audit finds the target maps exactly to guest801E93E8 in this executable's
g_PsxRam. Native setter80021BF8 stores argument1 verbatim there, but the
bridge omits that argument from callback classification and translates it
as data. Root's owned diagnostic normal replay is tracing the complete
setter chain before changing production. No widened target acceptance or
unknown-pointer fallback is proposed. Retail reference capture remains
separate and pending; Fei text and full opening exit remain unverified.

### 2026-09-06 UTC — Sprite callback argument classification repaired

The bridge now preserves argument1 of func_80021BF8 as a callback address;
argument0 retains normal sprite-data translation. The entire production
change is one classification entry. No accepted address ranges or fallback
rules changed. Runtime source SHA:
`fd4fae9b90677538e8ec7b754e814690d2de6434c39165a512c32b7ca07a6d80`.

Regression RED `battle_sprite_callback_retail_test.a2vAHke3` reproduced
guest callback translation into a host RAM alias in all three builds. Final
GREEN `battle_sprite_callback_retail_test.FL2rXqoy` passes O0/O2/UBSan
(with function-type checking excluded for the existing generic bridge ABI).
It includes the actual production runtime, executes the pinned retail setter
through the MIPS adapter, checks translated sprite data and preserved guest,
zero and native callback values, and dispatches the stored guest callback.
The callback-execution harness relocates the pinned simple retail setter into
a test address and proves its write by clearing a nonzero sentinel; this is
not a claim to execute the opening module's callback in this test. Removing
the new classification reproduces the semantic failure. Root verified source
pins and that the runtime differs by exactly one entry.

The diagnostic replay `opening-callback-trace-5-orqx88rm` followed normal
New Game and recorded normal pilot confirmations. It observed50 direct native
setters, then ended in HeapConsolidate SIGSEGV while opcodeE0 allocated a
child sprite. It did not observe the earlier guest setter-to-slot chain.
Both findings remain separate in `callback-diagnosis.json`.

`sprite-callback-retail-audit.md` proves callback801E93E8..801E9430 exactly
matches archive directory20/file1 loaded at801E5000: its72 bytes hash
`df0c3521a9f2dcfab542d62a8b63e03b627c4dacfdcc933e96868324311bcb5e`.
The saved RAM matches the module's entire code/table prefix through4C30.
Only its final three mutable data words differ. Three retail callsites
explicitly supply this callback to80021BF8. Thus the classification repair
is source-backed even though original live setter causality is unobserved.

Native build `opening-sprite-callback-port-build-20260906.log` is LINK OK,
executable SHA
`4fc351ed0c3de68780798c773a56ddaf5a297aae51c05259fcfec3673d2fb29a`.
Normal replay `opening-sprite-callback-5-b3kuz6dh` is in progress.
Fei text, full opening exit and the diagnostic heap failure remain unresolved.

Callback-repair runtime follow-up: `opening-sprite-callback-5-b3kuz6dh`
ended naturally at `unresolved native call target=0x0050118e
guest-pc=0x800afd74`, callback800AFC68. The pinned executable maps0050118E
exactly to native func_80022DF4 (AnimTask timer tick). Retail800AFD6C
loads the task callback from s0+4, then JALR at800AFD74. Native233A4
registers80022DF4 through TimerWorkListSetTaskCallback. The generated bridge
map lacks this symbol because the input retail symbol maps lack its explicit
assignment. Next bounded task: add/verify the retail-backed callback map
entry and regression without accepting arbitrary host pointers.

This replay used normal New Game plus the field-only input schedule, with no
extra pilot confirmations. Last frame6480 shows a small field actor beside
the purple Gear. This is a new observed stop, not proof of complete opening
parity or resolution of the separate heap crash. PID2942841 has ended.
Desktop input ownership returned to Sol's isolated retail-reference lane.

### 2026-09-06 UTC — Native animation task callbacks mapped; C0 reached

The retail symbol map now explicitly names80022DF4,80022E8C,80022EB8.
Retail233FC..2341C registers the tick/free callbacks;24878..24880 registers
the type7 tick variant. No runtime target ranges or fallback rules changed.
`battle_native_callback_map_test.YHMIfInO` is semantic RED before the entries;
`battle_native_callback_map_test.uRBPP6WZ` is GREEN at O0/O2/UBSan (the
existing generic function-type ABI is excluded from UBSan). The test uses
the actual runtime and production generator inputs, distinct boundary spies
for all three callbacks, translated guest data, unknown-host rejection and
generated-stub rejection. This is dispatch/ABI proof, not callback-body
retail parity. Source pins were verified after the tests.

Build `opening-native-callback-map-port-build-20260906.log` is LINK OK.
Executable SHA
`8fcf9a158838a5a9f2716b7f3b3762e1ee91228c8210af407ce3627dd995773f`.
The isolated matching check retains the baseline undeclared-stderr failure.
Normal replay `opening-native-callback-map-5-e2iikr3u` passed the callback
stop and ended at unsupportedC0 (192/index54), sprite007C97C0, operands
007C472D. Last frame7920 shows a character beside the Gear; no full-exit or
pilot-text claim. PID2983591 has ended.

`child-spawn-retail-audit.md` finds no child-constructor size/packed-pointer/
copy overrun mismatch in the bounded audited path. The prior heap crash
occurred before that child's initialization, and the first damaged heap
link remains unknown. It separately finds23538's missing recomputation after
default scaling: retail236EC falls through236F4/23708 while native uses
else-if. Root owns its repair; Luna is preparing a focused regression.
C0's retail jump table and handler20158..201F0 are identified; root candidate
is scratch-only while a second Luna worker prepares its actual-retail test.

23538 transform correction is installed: the enable-flag check for matrix
recomputation now follows the conditional default-scale block independently,
matching retail236EC ->236F4 ->23708. Initial RED ciphe7Hq and candidate
GREEN CAneQe0k cover the exclusive-branch defect. Final
`sprite_bind_transform_retail_test.SCA0V7Iq` passes32 cases per O0/O2/UBSan,
including Scale changing the enable flag to0 or2, header bits12/13, flag0/1,
and null/non-null backing. Both the old else-if and cached-condition controls
are rejected. Actual retail23538 instructions run with explicitly labeled
Scale/Compute boundary spies; this proves call order, arguments and controlled
effects, not the complete matrix algorithm. Source pins were verified.
C0 remains scratch-only pending its separate retail differential fixture.

### 2026-09-06 UTC — C0 installed and combined native replay

C0 retail20158..201F0 is implemented with two RNG calls, the byte-range
operand and signed scale, negative rounding, separately slow-scaled trig
components and wrapping X-add/Z-subtract. The existing retail symbol-map
convention names the cosine entry rsin and sine entry rcos; the candidate
was corrected before installation, preserving the wrong-trig variant as a
negative control. No GTE or rendering changes were made for C0.

`sprite_dispatch_c0_retail_test.QXXw9aHy` is semantic RED at O0/O2/UBSan.
Tested candidate `sprite_dispatch_c0_retail_test.pgG1PzwW` passes135,809
cases per mode and six controls: wrong RNG count, unsigned scale, signed
operand, missing rounding, wrong Z sign and swapped trig. Its oracle runs
actual retail dispatcher/RNG/trig/22CAC instructions. It covers independent
full operand/scale/timer axes plus finite seeds, position-wrap cases, operand
including RNG-seed aliases, whole fixture and final seed. This is not the
full Cartesian product or rendering proof. `c0-install-proof.json` pins the
installed source to the tested candidate.

Combined native build `opening-c0-transform-port-build-20260906.log` is LINK
OK, binary SHA
`6974e6da2c068ef3f64951499090c40088a171694d4300cee90b4dc9eacfd3d1`.
Animation source SHA
`e6423f2de9b3a3954125a0ec8cbd97defe70273c0f8a61e594f24e4b3f9a8179`,
temp1 source SHA
`494f6f78dc51aaf65e57d1bad5bc7a06e288059134de89c907648e709032d441`.
The isolated matching check retains baseline stderr errors in both files.
Normal replay `opening-c0-transform-5-0qxtsrd_` is in progress; its launch
manifest now also pins the explicit retail symbol map. No extra gameplay
state is injected. Fei text, full exit and the separate heap-crash first
writer remain unverified.

Combined replay result: `opening-c0-transform-5-0qxtsrd_` ended at
unsupportedA5 (165/index27), sprite007CA688, operands007CA725. Immediately
before the stop it logged missing child render table index9. Last frame7020
shows a character beside the purple Gear; no extra pilot confirmations were
used in this run. PID3059864 has ended.

A5's retail table entry points80021884; handler through218DC updates speed
at sprite+18 using signed operand and sprite+82, wrapping time multiplier,
then existing22974. Root scratch candidate is prepared; Luna owns its
actual-retail differential fixture. No A5 production edit yet.

Retail D_8004FD40[9] is80025544. The table currently binds NULL. Astra's
initial audit identifies this as a TILE+E1-mode renderer and finds material
projection, size, packed-pointer and OT-stride mismatches in the existing
unused draft. Do not wire that draft unchanged. The separate audit is
`/tmp/xeno-opening-type9-render-audit-20260906.md` (in progress). Its relation
to missing Fei text has not been established.

Retail reference input follow-up: keyboard Circle was unobserved. Sol added
only ordinary Circle API endpoints to its isolated Lua helper, preserved the
profile/title state/logs, and restarted the owned emulator. New PID3069504
was reported. Restoring the natural title state then API Cross->Circle
produced the distinct `I am Alpha...` prologue at+3 seconds; the confirm
input now has positive runtime evidence. Gear panel and exact text remain
pending natural progression. Desktop input ownership returned to Sol.

2026-09-06 A5: installed exact tested candidate51cef2ac; finalretail fixturee8KVAtIB passes74,602 cases/mode O0/O2/UBSan and7controls; root strengthened trig effective-angle/order assertions. Type9 candidate independently reviewed, regression pending. Retail Alpha/Omega transition reclassified as insufficient NewGame proof; Circle edge remains unresolved. See dated evidence README.

2026-09-06 type9+cleanup: installed exact reviewed/tested type9 renderer (GuFD7hpi:20 controlled O0/O2/UBSan and20 real-SDK O0/O2 cases,14controls+draftRED), updated slot9/billboard regression fF2rKRG5. A5 debugger replay revealed LineScrollFree widened-pointer read18 vsNULLretail14; cleanup-only repair jggq4lzB4cases/mode4controls installed. Combined native LINKOK; matching baseline stderr failures remain. Normal replay with persistent exit.json/sourcepins started; latest dated README has manifest. No full battle/text or pixel claim yet.

2026-09-06 runtime milestone: opening-type9-cleanup-5-s6ef07mm binary36b934f2 returned battle after368,858,285 guest instructions, loaded field14, rendered Fei at easel (capture16080). No unsupported sprite or missingtype9 warning. Root stopped owned native capture after proof; exit0 from SIGTERM handler, not spontaneous exit. Native/monitor gone; retail3106417 preserved. Fei lettering and full retail visual parity still unverified. Dated README and type9-cleanup-runtime-result.json are current authority.


### 2026-09-06 — opening text and direct battle return

Confirmed guest/native result split, installed retail-correct func8001B6C4
selection (guest bytes, guard state6, unknown-result skip),384 cases/mode and
9 controls. Traced actual dynamic module801E6FC0 dialogue constructor call:
retail seventh row count became dead native mode; fixed only bridge80032F54
with raw signed-halfword read into native eighth arg. ABI tests/control matrix
pass; native LINK OK; make check retains prior stderr baseline errors.
Live opening-window-fixed-1-bsbals5x shows readable battle text, state1 direct
FieldMain entry/map14, and painting-room dialogue. See dated runtime JSON.
Recording was active; root left controls alone pending operator coordination.
No commit, staging, push, memory update, or retail payload publication.

The same replay later passed through world-map gameplay and reached another
battle, aborting at opcode0xA9/index31. Root did not issue a stop. Saved final
log and opcode/operand pointers in window-and-return-runtime-20260906.json;
no source guess or forced state for that later command. Session now terminal.

### 2026-09-06: host telemetry matching guard and opening ownership audits

Root reviewed Luna's six XENO_PC_PORT guards against exact before snapshots;
asserts and opcode behavior remain intact. Native build LINK OK, binary
e5b7b574a23000404020db567f3bfeb17e41bb2044298cd445b4dec38513823a. Matching
advances from stderr compilation errors to unresolved resident jump-table labels (72 linker diagnostic lines); no matching pass claimed. Baseline and after logs preserved
in /tmp/xeno-battle-result-probe-20260906 and /tmp/xeno-matching-telemetry-20260906.

Astra's constructor audit proves extra native raster clearing absent in retail;
other opening tuple formulas agree statically, full differential pending. Sol
inventoried the actual file1 controller801E6CE8..801E71D4 (1260bytes), separate
from base battle and other dynamic modules sharing801E5000. Reports archived
in opening-battle-encounter-20260905; no constructor/module production edit yet.

### 2026-09-06: constructor retained state, glyph oracle and virtual opening

Installed reviewed system.c b75fb09b069abd5462b73c0530b7365879cf2f5af5dfa8256a05bd807a4a67db:
remove nonretail raster memset; restore signed-halfword input truncation and
retail unmasked packed-word arithmetic using defined unsigned shifts. Root
reproduced original RED/candidate GREEN and7 controls, then ran durable
installed constructor124/mode (112 complete+12 allocation boundaries) and
glyph144/mode (synthetic glyphs, full13 rows) tests with O0/O2/ClangUBSan.
Output roots h3chfeyu and kpFpRS named in dated repair report.

Native ed9987ed2a14639acca2edeab0a2ad25cc04ddef7a44aebf7e8d7eef107c1420 links.
Matching still fails with unchanged explicit unresolved-label multiset.
First virtual replay ysi05oci timed out after one battle input; second
jwlhtzig sent ordinary z pulses per dialogue and returned to painting room
at04:05:48UTC. Fei text and room frames pinned; monitor stopped own processes.
No real-retail-framebuffer or audio acceptance inferred from Xvfb/null audio.

File1 controller draft preserved in scratchpad/decomp-candidates, accepted
structurally by independent Astra review (463 cases,315 instructions,21
branches both ways), but root exact text comparison FAIL1256/1260bytes.
No native adoption/config or address-only dispatch installed. The full goal
remains active; later-play A9 and broader opening/module parity remain open.

### 2026-09-06: file1 module foundation and corrected matching baseline

Added pinned archive0x20/file1 extraction, separate Splat/symbol/source boundary,
durable463-case controller regression and isolated full-module checker.
Corrected prior standalone alignment error: original1256-byte blob contained
8 leading NOP bytes; a308 draft correctly aligned emits1248 bytes. Improved
c84a38eb C emits1260 bytes and302/315 exact instruction words, still FAIL.
Root complete ASM rebuild equals all19,516 retail bytes at xceyjpdv; C gate
correctly exits1 on13 differing words. Installed-source O0/O2/ClangUBSan
463-case/11-control test passed independently at q_jw41cf. Native scaffold
build59028f92 links; native adoption not installed, no new runtime replay.
Read file1-foundation-20260906.md and its pinned JSON. Existing shared changes,
retail payloads and sessions preserved; no staging, commits or push.

Final coverage review caught packaged one-yield default missing one wait
branch outcome. Durable runner now uses3 yields and asserts315 visited slots
and21 branches both ways. Final independent root h5bfxx02 passes all modes
and11 controls on installed c84 source; dated foundation JSON pins this run.


## 2026-09-06: exact file-1 C, A9, and matching-link recovery

- Controller 801E6CE8 is now 315/315 instruction words and 1,260/1,260 bytes
  exact; root full-module C-containing reconstruction and 463-case/11-control
  differential pass. Native module substitution is still disabled.
- A9 production handler and actual native outer interpreter pass root 4,608
  cases per mode and five controls; actual scale helper included. Runtime
  replay on the repaired copied binary is a separate observation lane.
- Retail direct loads replace generated text offset table; root textbox O0/O2
  and GCC-instrumented/Clang-runtime UBSan pass. Standard GCC UBSan runner
  exits 1 due to missing installed libubsan; no toolchain/runner changes.
- Matching-only exported jump-label macro and table removal eliminate prior
  linker errors. Isolated full build completes 472/472; make check exits 2 on
  unchanged SLUS, field, member-change and shop retail checksums.
- Native build after these changes succeeds, SHA f4dcc43c404d453861429e80a017a5c4bda2ff355f85829d359276c274743ec6.
  See dated exact-controller, A9, text-table and global-matching reports.
- Previous pre-A9 replay zf8ad3_8 returned to map14, did not reach later battle
  before its 900-second owned stop; no A9 gameplay proof from that run.


## 2026-09-06: unused guest-call prerequisite independently verified

Astra added a bounded internal 0..7 raw-argument guest-call service with the
retail controller's 0x50 frame, GP/register inheritance, fresh pipeline,
alias-safe arguments, validated initial target/SP and nested context restore.
Root reviewed it and recovered the exact old runtime by removing this service
and header include. Root suite /tmp/xeno-guest-call-test.FyVYo29F passes202
assertions at O0/O2/all-Clang UBSan plus six semantic controls. Native rebuild
1738d43d... links. No production callers or native controller adoption yet;
identity/RAM bindings and adopted-path differential remain next gates.
Current Sol gameplay observation uses copied prior f4dcc43c binary, so it is
not runtime evidence for this new service. See dated integration report.


## 2026-09-06: Record/F9 missing audio reproduced and repaired

The existing AAC track contained mostly inserted digital silence. A controlled
private-sink tone with the actual production recorder reproduced4/79 active
bins while its direct reference was continuous101/101. Removing only the
Pulse input's generic wall-clock timestamp override restored79/79 at matching
RMS. Rawvideo timestamps remain unchanged; game sound/simulation untouched.

Native rebuild bd9b2336... succeeded. Fresh F9 game recording follows the same
77/152 audio-active intervals as its paired monitor reference (RMSratio0.969,
envelopecorrelation0.925); root independently recomputed masks and levels.
Root durable test: normal30Hz38/38 and fast150Hz39/39 active bins, RMSratios
0.9997/0.9999, A/Vduration differences13/18ms. Actual old source fails3/38
with a valid reference. Static seam/hotkey test passes. Tests verify both
capture PIDs, source/fixture/runner pins and private Pulse cleanup; invalid
references cannot count as expected regression failures. All owned runtime
processes ended; existing recordings, application log and global audio
settings preserved. See docs/evidence/recording-audio-20260906/README.md.


## 2026-09-06: Native file-1 adoption and exact character-change menu

The shared exact controller body now runs through checked packed RAM bindings
and guest/resident helper ABI boundaries after archive/module/hash identity
verification. Original 463-case regression passes all three modes and eleven
controls; adopted-path regression passes 473 cases/4319 assertions per mode
and ten controls. A natural opening observed670 paired adopted calls, Fei
dialogue, and return toFieldMain/map14. No story/position/RAM forcing. Owned
processes ended and prior application log restored. Observed binaryf59f7f87.

Member-change swap C replaces the final assembly placeholder; typed label
fields/RECT copies and retail load order fix native pointer layouts and32
missing PSX bytes. Name-renderer unsigned shifts/calculation order repair the
last83 differing bytes. Whole26,624-byte module now equals retailSHA
3b9e2b890c27ae0de97fe343ac75cd35c05fe7f9605ae387d2166da0be78109c.
Private make check completes472tasks, with checksum failures reduced4to3:
SLUS,field,shopmenu. Actual native swap/label/name regressions and semantic
controls cover the repaired paths; GPU/menu visit parity is not claimed.

Final build1c9e2c73 links and retains verified RECORD audio fix. Later name
repair is build/test verified separately from the observedf59 opening. Full
game/forest/laterbattleA9 remain incomplete. See dated file1-native-adoption
and member-change-menu evidence reports for exact source/build/test pins.
No commit/stage/push; unrelated dirty work and recordings preserved.


## 2026-09-06: Exact system menu frame and remaining-module diagnosis

Corrected func8001C074 native pointer layout, alternating graphics buffers,
and nested debug sentinel checks. Actual production TU emits308exactretail
bytes; native300cases/6020checks cover77instructions inO0/O2/mixedUBSan,
four compiledcontrols reject, oldactualsource failsnativepointercheck.
Shared debugindexfield is now ordinaryRAM with volatileaccesses retained
locally in input; isolated qualifier probes preserve fullsystem/member/shop
emittedinstructions. Private makecheck472tasks/3reds, memberfullexact and
field/shop unchanged; SLUS gains4missingframebytes,stillred. Finalnative
build4ab17908 links; repairedframe runtime NOT_OBSERVED. See dated evidence.

Independent field audit distinguishes +0x120rodata/-0x4984text drift from
missingtaildata; missing292-byte matrixbody800759E4 candidateexact in scratch,
notinstalledyet. Shopaudit identifies16hoistedassemblybodies and one68byte
loopnonmatch. Solnaturalroute uses copiedprior1c9e2c73; inspectlivehandles/
reportbeforeclaimingprogress or restarting. Fullforest/wholegame unfinished.


## 2026-09-06: Absent field axis matrix body installed

Installed exact292-byte800759E4 at retailorderingpoint plusnative16-byte
seed0,0,4096,0. RootverifiedfrozenfullTUbody/seedagainstdisc, ran200cases/
21078checks/all73instructions O0/O2/all-ClangUBSan,4controlsreject. SDK
boundaryspies do not prove realmath/rotatedactors; assertedcaller untouched.
Privateglobal472tasks still3reds; field+292to242622, otherhashesunchanged.
Nativecda8623b links. Datedfield-axis-matrixevidence retains source/build/tests.

Naturalroute priorrun endedintentionally07:01:14UTC atfield4; nofirstbattle.
Drivercircularfocuswait diagnosed, noenginefixinvented; nextdriver supports
earlyfocus, extensionfile, isolatedsavepath, sparsecapture. AlloldownedPIDs
ended, applicationlogrestored. Newroute ownedbySol; revalidatehandleonresume.

## 2026-09-06 — exact shop byte lookup

Repaired func_801CE8D8 to the retail positive-count guard, low-byte mask and signed end-address comparison using native-width intptr_t. Actual full-TU extracted function matches all68bytes. Root native differential:65cases/297checks/all17instructions in O0/O2/UBSan plus4 rejected controls. Private global472tasks still has3checksumfailures: shop size now55296 instead of55300; othercheckedmodulehashes unchanged. Native LINK OK, binary53c04117da90ed9860d2f6fd5f0a9f76d3898732ba8edebcdd916c0341dab49d; build-only, ongoing natural run keeps preceding copiedbinary. See docs/evidence/shop-byte-lookup-20260906/README.md. No staging/commit/push.

## 2026-09-06 — archive destination pointer width

Shop loader dependency audit exposed ArchiveReadFileToBuffer accepting a host destination through s32. Actual original function fails a high-address mapping test; one-line void* signature correction passes7focusedcases at O0/O2/mixedUBSan and rejects recreatedoldtype control. Helpers are explicit spies after object-symbol weakening; no actualCDI/O claim. Whole GCC2.6 TU assembly unchanged apart from.filename, all4privateglobalmodulebytes unchanged after472tasks/3existingchecksumfailures. NativeLINKOK1b82f340a37889e691983cf3c9f61ee43eaeb524102c3882bbcd2bfadbfe435c, NOT_RUN. See docs/evidence/archive-buffer-pointer-20260906/README.md. No staging/commit/push.

## 2026-09-06 — exact shop resource loader C

Replaced ShopMenuLoadResources native stub/INCLUDE_ASM with C whose actual fullTU GCC2.6 body matches1088retailbytes; preserved16byteconstant including globalbinaryprefix. Root128cases/11944checks/all272instructions at O0/O2/all-ClangUBSan plus7rejectedcontrols. Boundaryspies only, no live shop rendering claim. Private global472tasks still3checksumfailures; shop55296bytes,15assemblybodies remain, othermodulehashes unchanged. NativeLINKOK7739b2b794a430eee899aade8be196e0c8b2d804cbce5d3bb451887b82872da6,77functionstubs,NOT_RUN. Ongoingnaturalroute keeps copiedcda8623b process, nowfield13 after13/14transitions; exitcondition still underretailaudit. See docs/evidence/shop-resources-20260906/README.md. No staging/commit/push.


## 2026-09-06 — exact shop string UV/tpage rebuild

Resumed Codex's unfinished func_801C5A7C decomp. Its best of13 candidates was
572bytes against576 with two wrong words; the whole delta was source shape.
Repaired both: blend is u16, not s32 (an s32 accumulator emits `or v0,v0,s1`
where retail has `or v0,s1,v0`, and swapping the source operands does nothing
because GCC evaluates the GetTPage call first and swaps commutative operands
itself); and the u2 corner is fed by a mid-block `u = (half & 1) * 128;` while
the other three corners spell the expression out, which is what leaves retail's
single `move v0,s3` loop-invariant copy. Writing all four from one variable
loses the copy, writing all four inline costs two instructions.

Actual production shop TU emits all576retailbytes /144instructions at
801C5A7C..801C5CBC, SHA d71c52a1f841c5be08425bba883a28d3f2ac47acccc4ee1062c84316f1bd3e09.
The overlay still orders this body at801C9738, so the production object was
relinked onto the retail address with only its6 real dependencies pinned; body
claim, not overlay layout. Full make build472tasks/0failures, same3checksum
reds. Control build with INCLUDE_ASM restored is byte-identical for slus, field,
member and menu, and its shop hash06700c87 reproduces the recorded shop
baseline, so the environment is faithful; only shop_menu.bin changes,55296bytes,
770921de. Shop assembly bodies15->14.

Flagged: slus_006.64 and field.bin differ from the shop-resources record at
identical sizes, identical across control and change, so pre-existing tree drift.
Build-environment notes recorded in ACTIVE_HANDOFF.md: tools/gears hard-codes the
"xenogears-decomp" directory name, and mips-linux-gnu-cpp is absent.
Native port link NOT_RUN (xenogears-dev container absent); no runtime observation
and no focused native regression test for this body yet.
Live route run-y9dtjd02 confirmed TERMINAL at07:53:27UTC on its own2700s bound,
first battle returned, no A9, reached map15 mountain area; no owned PIDs remain.
See docs/evidence/shop-string-uv-20260906/README.md. No staging/commit/push.


## 2026-09-06 — Blackmoon object-overlay verification, pass 1 (blocked by /tmp quota)

Fresh scratch /tmp/xeno-blackmoon-verify-mT7beL/xenogears-decomp with the cpp
wrapper reproduces the shop-uv checkpoint exactly: 472/472 tasks, slus
3192a514/301636, field ececa463/242622, member 3b9e2b89 OK, shop 770921de/55296,
menu 9fc9b811/152860; gate still red on slus/field/shop.

Drift RESOLVED as environmental: the 02:37 shop-resources record (slus 5c674b3f,
field f72b2245, workspace /tmp/xeno-global-jtbl-audit-20260906) was built by
splat 0.33.2/spimdisasm 1.33.0 (its own log banner; requirements.txt pins),
the 10:55 and today's builds by host splat 0.41.1/spimdisasm 1.42.2. No build
input under src/include/config/linker/asm changed in the window except the
shop TU; the generated bss asm differs by generator (1.42.2 adds nonmatching
size directives); objdump diffs are pure data-address shifts, slus -12 bytes,
field -4 bytes, 1829/4322 single differing bytes, opcodes unchanged. Exact
moved symbol not yet named (map join pending). Hash records now depend on the
splat version, not only on source.

func_800A1364 C body (misc6.c, XENO_FIELD_OBJECT_OVERLAY) is NOT byte-exact:
through the actual retail cpp/cc1/maspsx pipeline it emits 89 instructions,
frame 0x28, against the INCLUDE_ASM control's 99 / 0x30. Retail re-reads
D_800B2264 for each of four uses, recomputes g_FieldActors[D_800AFD1C] after
the IP/flag writes, uses v1/a0 where the candidate uses a2/a1, and has 8 more
frame bytes. The matching build never compiles the body (flag only in
build_port.sh and pc_port tests), so the "asm-verified" comment was a hand
check; comment corrected in misc6.c, body untouched, rewrite pending. No
behavioural divergence identified. Harness a1364/try.sh in the scratch.

Native build works on the HOST: PKG_CONFIG_PATH=/home/linuxbrew/.linuxbrew/lib/pkgconfig
CMAKE_PREFIX_PATH=/home/linuxbrew/.linuxbrew bash pc_port/build_port.sh ->
PsyCross built, 50 game TUs, 643 undefineds -> regenerated stubs.c with 76
function stubs (was 77) / 566 data symbols, LINK OK,
xeno-port e86ad1881b637047bdfaeb6156ad31ed2116cf294dd069531b67a8d70cca0011
(4680376 bytes, runtime libs from /usr/lib64). Previous 7739b2b7 binary and
the container-built pc_port/build preserved under the scratch native-backup/
and pc_port_build_container/.

MAP16/17/3 smoke NOT_OBSERVED: the xvfb run coincided with /tmp hitting
EDQUOT; logs empty; every later shell call (incl. subagents) failed with no
output, /var/home still writable. Resume steps, exact commands and pins:
docs/evidence/blackmoon-object-overlay-verification-20260906/README.md.
A neutralised .claude/launch.json (empty configurations) was left in the
checkout and should be deleted. No OPEN_ISSUES.md edits. No staging/commit/push.


## 2026-09-06 — func_800A1364 exact; generator drift reproduced (pass 2)

Unblocked once the /tmp user quota (12610M, filled by another agent's tree)
was cleared; scratch moved to /var/tmp/xeno-blackmoon-verify-mT7beL and
TMPDIR off tmpfs. The build container is the podman image
localhost/xenogears-dev-toolchain:current (earlier "no container" claims
wrong); the coordinator ran the MAP16 smoke, not repeated here.

func_800A1364 C body INSTALLED in misc6.c without the XENO_FIELD_OBJECT_OVERLAY
staging: actual retail cpp/cc1/maspsx pipeline emits all99instructions/396bytes/
0x30frame, opcodes+relocs identical to the INCLUDE_ASM control with and without
the old flag; relinked with its10 dependencies pinned it equals disc/field.bin
[0x31874:0x31A00], SHA abf8efc25897d54dc9dd9b7fa2d38ed2e464836d332eb3cbb9b506e598b7be98.
30 variants; load-bearing shapes: pointer-arithmetic slot stores (indexed
array stores leave D_800B2264/g_FieldScriptVMCurActor cached in GCC2.7.2),
one volatile counter read for the stamp (the only way to get retail's 4th
reload), IP/flags through the global pointer + g_FieldActors[D_800AFD1C]
directly, unused s32 pad[2] for the 8 frame bytes, byte-neutral (u32) on the
id shift. Whole-field: the function moves to its retail-relative slot
(0x8009a850->0x8009dc70), field.bin f49b4fd0 at242622; masked per-function
compare of1054functions: 0 differing, 60 moved. slus/member/shop/menu unchanged.

Root differential test pc_port/tests/run_field_object_register_retail_test.sh:
O0/O2/UBSan 1440cases/57702checks/all99slots each, 7controls rejected; spies
for GetArgument/80076AC0/800A0C94 (same-TU, weakened, -fno-inline-functions).

Drift PROVEN by reproduction: pinned splat0.33.2/spimdisasm1.33.0 in the
image with the12:15 sources gives slus5c674b3f/field f72b2245 (the02:37
record); host0.41.1/1.42.2 gives3192a514/ececa463. First moved symbols:
slus g_CurGameStateOverlayID (.main_bss start 0x80058d10->0x80058d04, retail
buffer+4 so new generator right), field D_800AF5F0 (.field_bss start
0x800aaeb0->0x800aaeac, retail data-end+4 so old generator right).

Container native build with the final source: LINK OK, 74functionstubs,
xeno-port 522e901c0789888bb8b7e11a931b47bd30e2d265dc59917a04065d64baca4e5e,
NOT_RUN. OPEN_ISSUES untouched (render evidence is the coordinator's).
Evidence: docs/evidence/blackmoon-object-overlay-verification-20260906/.
launch.json deleted. No staging/commit/push.

## 2026-09-06 — MAP16 "black forest" SOLVED: it is a scripted Gear flyover, and the object draw path was approximate

MAP16 was never a broken forest. It is a short scripted cutscene: a fixed camera
behind two foreground foliage fragments, four archive-6B9 objects (one type 0 +
three type 8 — a Gear flight) lerped from (-6000,4500,0)/(-7000,4500,+/-600) to
the origin one after another, then a warp to field 15. Between passes the scene
is authored-dark (night). OPEN_ISSUES item 8's recorded cause (object loader
stubbed -> objects never load) is obsolete: objects load fine, 65k draw packets.

Root cause of the near-black objects (proven): the port's approximate
func_801E7D14 omitted SetColorMatrix(D_800B223C) and the per-node
SetLightMatrix, composed w2s x node.local instead of rootView x node.world,
ignored scale, and used walker variant 0 instead of 1 — and this map's own
colour matrix is all zeros, so only the (30,30,30) back colour survived. Two
further bugs: misc2.c FieldUpdateObjectActor read the scale table at retail
0x800B21DC (the spriteId<<1 table; the blob starts at 0x800B2078), scaling
objects 7.0x; and main.c declared D_801E8670 as void*[] (8-byte host stride) so
the scale table was never written for slots 1..3.

Fixes are retail transcriptions: six-pass func_801E7D14 (704B), new
func_801DCEC8 (3376B), func_801E0398 (768B), five-arg func_801E36BC (276B),
func_801E5D44 gate/walk/rewind, D_801E8698; new pc_port/src/model_prim_ed20.c
with the variant-1 lit small-tri walker func_8002ED20 wired as
D_8004FE50[0].proc[1]; port-only fixes in misc2.c FieldUpdateObjectActor and
main.c's D_801E8670 stride. Port-only OvlyBindActorPos / OvlyRebuildTree /
OvlyRebuildNodeMatrix removed.

Coordinator verification (independent of the implementing agent): LINK OK, 74
stubs. Map16 pre-warp frames now show three lit, textured, correctly-scaled
Gears passing the camera — peak frame 372 at 35.2% non-black / 18.3% bright
(previously 0.0-12.2% non-black with a single foliage fragment). Regression
smoke maps 17/1/2/15 all rc=124, zero faults, zero missing-D_8004FE50, model
builds 90/132, 96/143, 65/104, 67/109. Prim-table retail test still 122
assertions / 0 failures with 4/4 controls rejected.
Test: run_object_draw_retail_test.sh — 1536 cases at O0/O2/UBSan, 7 mutants all
compile and are rejected at runtime.
Evidence: docs/evidence/blackmoon-map16-render-20260906/.

Boot chain separately verified this session by capture: FMV decodes and renders
(SquareSoft logo), Map 490 title logo renders (93.3% non-black, an earlier note
had it at 1.9%), Map 4 prologue narration types correctly. The opening path
shows ZERO obj-ovly activity, so the Gear draw fix above does not apply to it.

Also this session: D_8004FE50 rows 0x02/0x06/0x07/0x0E filled from retail
.sdata (7 absent rows -> 3); encounter weight table D_80065ADC re-aliased onto
D_800658DC+0x200 (weights were never written, so no random encounter could fire
anywhere); slus hash drift proven to be a splat/spimdisasm VERSION artifact
(pinned 0.33.2/1.33.0 reproduces 5c674b3f exactly). No commit/stage/push.

### [2026-09-06 16:30] Title -> New Game boot chain: not a regression, the title only answers Circle
- **Symptom reported:** retail boot reaches Map 490 and "never enters the title menu" with `XENO_PAD_TEST_INPUT` Cross/Start/Up presses; grind log 2026-09-02 had the chain working.
- **Cause (proven, no code fix needed):** Map 490's script polls `OP31 mask=0x0020` (Circle, held: `func_800961A0` -> `FieldScriptVMCheckControllerInput(D_800AFE9C)`), then after an idle budget runs FE60 -> `func_800A7C58`, which now (retail body, superseding the `46157084` shim) plays the 4462-frame attract STR (dir 0x18/1 file 15) and exits only on `D_800ADB80&0x80 && D_800C3900&0x20` (Circle) or the frame budget. The title reader `func_801C7D78` (byte-matched) confirms on `g_C1ButtonStateReleased & 0x20` and has no Start test. Cross/Start/Up therefore sat inside the STR loop for the whole 110 s window. The 09-02 chain was driven with Circle.
- **Proven end to end (headless, pad layer):** `pc_port/tests/run_title_newgame_smoke.sh` schedule `0:0,300:0x40,320:0,1600:0x20,1602:0,2400:0x20,2402:0,2800:0x1000,2802:0,2900:0x20,2902:0,3400..:Circle 2-on/2-off` -> `[fe60] exit reason=circle` -> `func_800799D4 request=2` -> `func_801C58EC title loop enter choice=1` -> UP -> `title confirm choice=2` -> `func_8001B970` -> FE60 replays the opening STR (Circle skips) -> `FieldLoad field=4` -> `field=2` (~3.5 min) -> `field=14`. Captures: title menu, prologue text, burning Lahan with Fei/Citan, Weltall, Map 14 gear scene. No `[stub]`/OT/SEGV markers.
- **Regression test:** `pc_port/tests/run_title_newgame_chain.sh` (prod-linked misc.c/misc11.c/menu misc.c, O0/O2/UBSan identical, mutants M1-M5 build and fail at runtime with named ASSERTIONs). Negative control: same smoke with Cross for Circle must not reach the menu.
- **Additive diagnostics:** `[xeno-port][fe60] enter/first frame/exit reason=...` in `func_800A7C58`; `XENO_VM_TRACE=<actor>|all` first-seen-ip opcode trace in `FieldScriptVMRun`; `PAD_TEST_INPUT_MAX_STEPS` 256 -> 4096.
- **Evidence:** `docs/evidence/title-newgame-chain-20260906/`.
- **Open:** title backdrop still black behind the menu (pre-existing); `func_801D9F98` (Continue) still a stub; retail idle budget before the attract STR not decoded numerically.

### [2026-09-09] Equip list builder func_801DE5CC native port
- **Hypothesis:** DE5CC list builder was the next Equip stub blocking Lahan Equip after DFB68/DF0D4.
- **Scope:** src/menu/main/misc.c (XENO_PC_PORT body), pc_port/tests/menu_equip_list_test.c, run_menu_equip_list_test.sh, docs/evidence/lahan-equip-list-20260909
- **Change made:** Retail-backed native transcription of 801DE5CC..801DF0D0; PSX keeps INCLUDE_ASM.
- **Build result:** differential harness LINK OK (not full xeno-port rebuild this turn)
- **Runtime result:** NOT_RUN (Equip acceptance still pending)
- **Proven:** 352 MIPS-vs-C cases O0+UBSan; menu.bin instruction words; 4 mutants rejected
- **Not proven / still open:** gear accessories; group&&cat==4; O2 owner; D36E0 staging; DFF5C; fresh Lahan Equip runtime
- **Committed:** no

## 2026-09-09 DFF5C description renderer
- **Change:** XENO_PC_PORT body for func_801DFF5C; differential harness menu_equip_desc_test
- **Proven:** 1920 MIPS-vs-C cases O0+UBSan; menu.bin words; 4 mutants rejected; LINK pending podman
- **Not proven / still open:** DE5CC remaining gaps; resource 7/17; fresh Lahan Equip runtime
- **Committed:** no

- **LINK:** OK, 72 function stubs; DFF5C and DE5CC absent from stubs.c

## 2026-09-09 Equip resource modes 7/0x17
- **Change:** Port description-bank load/free arms of func_801C72BC
- **Proven:** 1024 MIPS-vs-C cases O0/O2/UBSan; 4 mutants rejected
- **Not proven:** runtime Equip acceptance; DE2C8 +0x434 width
- **Committed:** no

- **Adjacent:** DE2C8/DE36C now use MenuStoreRawPointer/MenuRawPointer at +0x434

## 2026-09-09 Equip smoke SIGSEGV root cause + DE474

- **Change:** `MenuPsxPointerSlot` remaps retail g_Menu pointer slots (0x358/35C/360, 0x42C..438, 0x34C/440/44C, title 0x3A8+) onto native `unk358`/`unk42C`/etc. Mode 7/0x17, DE5CC, DFF5C, D17C4 use the remap via MenuRawPointer(0x434). Fixed `func_801DE474` E8070 arg order (asm 801DE4FC) + D397C 9-arg call; ported E8070 mode 3 (Equip category labels into packed unk14E0). DE2C8 E8018 now targets `unk14E0`.
- **Proven (runtime harness):** `XENO_MENU_NAV_TEST=equip` — Equip entry `func_801E0F78`, no SIGSEGV in D0C78, Cross cancel returns; E8070 mode-224 stub gone. Gates: list 352, desc 1920, resources 1024 still PASS. LINK OK.
- **Not proven:** Equip panel content (stubs `func_801D7F50`/`func_801D8644`/`func_801CE860`); teardown leaves orphan windows; natural Lahan Equip acceptance; `make check` not re-run.
- **Committed:** no

## 2026-09-09 Equip panel ports + FPE guard (run5)

- **Change:** Native `#ifdef XENO_PC_PORT` bodies for `func_801CE860`, `func_801D7F50`, `func_801D8644`; FPE guard when `func_801D85DC` returns `maxVal<=0`; fixed `func_801D84B4(base,cur,maxVal)` arg order per retail asm.
- **Build:** LINK OK, **69** function stubs (CE860/D7F50/D8644 no longer stubbed).
- **Runtime (harness):** `scratchpad/lahan-equip-smoke-20260909/run5-fpefix.log` — Equip entry `func_801E0F78`, **Cross cancel tick 172**, no SIGFPE/SIGSEGV, no panel draw stubs. exit=137 (timeout kill, expected).
- **Panels still empty:** cold-boot `unk330+0xB8` stats all zero → D8644 early-returns; `func_801E433C` still stubbed (likely stat publisher for character equip).
- **Not proven:** filled stat bars/digits; natural Lahan; teardown; `make check`; differential re-run (podman image lacks clang this turn).
- **Committed:** no

## 2026-09-09 func_801E3A80 destination fix

- **Change:** Retail asm writes aggregated stats to `unk330+0xB8..0xC4` (`$a2`/`pCtx`), not GameState+0xB8; restored 0xA4 character stride, clamp logic, and 0xC4 row.
- **Build:** LINK OK, 69 stubs.
- **Runtime:** run6 — Equip entry + Cross cancel tick 172, no crash (cold-boot stats still zero → D8644 early-return, panels empty).
- **Template seed experiment (not in tree):** func_8001B970 in equip harness loads real stats but needs `D_800594CC=0` for nav; non-zero stats hit Psy-X MAX_DRAW_SPLITS → SIGSEGV (run9).
- **Not proven:** filled panels on stable harness; natural Lahan Equip; func_801E433C (gear path).
- **Committed:** no

## 2026-09-09 CE860 OT cycle + filled Equip panels (run10)

- **Cause:** retail `func_801CE860` advances bar POLY_G4 bases by 0x48/row (801CEB28/801CEB38). The port used only `renderContext*0x24`, so all seven rows `AddPrim`'d the same G4 and cycled the OT → MAX_DRAW_SPLITS / SIGSEGV once stats were nonzero.
- **Change:** CE860 uses `0x2030/0x2228 + row*0x48 + ctx*0x24`. D8644 `func_801D84B4(cur, base, maxVal)` matches asm a0/a1. TEST TOOLING: equip harness calls `func_8001B970` + `D_800594CC=0` + Fei-only roster.
- **Build:** LINK OK, 69 stubs.
- **Runtime:** `run10-ce860.log` — EQUIP SEED, Equip entry, Cross cancel tick 172, **no** MAX_DRAW / SIGSEGV. Shot `shots/equip-during10.png`: Fei Equip with Martial Cap / Martial Wear / Stamina Ring and filled AT/Hit/DF/Evade/Ether/EthDef/Agility. After-cross (`equip-after10.png`) still has orphan overlapping windows.
- **Not proven:** clean teardown; weapon-list rows; Equip portrait HP/EP digit clip (`50/ 5`); natural Lahan; `make check`.
- **Committed:** no

## 2026-09-09 Equip layout: unk14E0 draw + packed staging (run18/20)

- **Cause:** Native `MenuString` is 152B and `unk14E0` lives at offsetof 6432, not `g_Menu+0x14E0`. `D0F54` drew the wrong address; `DE2C8`'s `E8018` strode native MenuStrings into the packed 0x80 bank; `DE5CC` cast `listBuf+0x800` to native `MenuString*` (overflow into desc).
- **Change:** `D0F54` indexes `g_Menu->unk14E0[i*0x80]`; `MenuPackedE7E68` for Equip label build; stage `D36E0` through a host MenuString and pack back.
- **Runtime:** `equip-during18.png` shows **Weapon / Accessories** category labels. Teardown still clean.
- **Seed dump (run20):** after B970, `eq@2D6` and weapon/acc inventory IDs are **all zero**. `GetWeaponName(0)` still paints "Martial Cap"; DFF5C retail `beqz itemId` clears A18 for id 0 — empty list/desc match empty template gear, not a draw-gate miss.
- **Bleed:** dimmed `CEC40` selection menu still visible under Equip (likely retail-ish; not chased this turn).
- **Not proven:** real equipped IDs + spare inventory on harness; natural Lahan Equip; `make check`.
- **Committed:** no

## 2026-09-09 Equip desc path (run23–31)

- **Seed (unchanged):** after B970, `eq@2D6` weapon=0; accessories `0x2E0={1,16,92}`; inventory weapon/acc IDs all 0 → empty list is template-correct.
- **Harness fix:** category DOWN must poke `g_C1Buffer` DPAD bit before `ControllerPoll` (PressedOnce). OR into `g_C1ButtonStateReleased` never reaches `func_801C7D78`'s DOWN arm.
- **DFF5C evidence:** mode1 `cat=1 A18=1 equipped=1` but `SystemRenderStringEntry` width=0 — accessory bank `archive[0x36]` has empty `01 00` stubs for id 1/16; id **92** (Stamina Ring) has `descW=56`. Bank has 155 nonempty entries (`firstRich=93`).
- **Runtime (run31):** three pad DOWNs → `mode1 cat=3 A18=1 equipped=92`, lines `w=56/56/0`. Shot `equip-during31-stamina.png` shows desc fragments (`Angelic`, `value+2`) at retail C851C coords (x=0x10,y≈0x96) over the gear panel — not the empty list window. Teardown clean.
- **Still open:** Martial Cap for weapon id 0; main-menu bleed; empty spare inventory; natural Lahan; `make check`.
- **Committed:** no

## 2026-09-09 make check baseline attempt

- `make check` → `gears clean` fails: `Could not locate project base path.`
- Cause: `tools/gears/src/file_system.rs` `find_base_path` walks parents until directory name is exactly `xenogears-decomp`; this checkout is `xenogears-decomp-ai`.
- Matching checksum gate not runnable here without rename/symlink. Port LINK OK separately verified.
- **Martial Cap note:** retail `func_801D8EA4` always `GetWeaponName(equipment)` with no `beqz` on id — painting name for weapon id 0 is retail path, not a port skip bug. Template B970 leaves weapon=0.

## 2026-09-09 make check baseline (remount path)

- **How:** podman mount `xenogears-decomp-ai` → `/home/blizz/Projects/xenogears-decomp` + image `/.venv` (`splat64==0.33.2`). Host dir name alone is insufficient (`find_base_path` / `getcwd` canonicalize).
- **Matching compile fixes (this tree):** (1) `func_801E0F78` TEST TOOLING under `#ifdef XENO_PC_PORT` + C89 decls; (2) `MenuUnk440Pointer` / ItemWork helpers moved out of PC-only block so matching `func_801D14B0` links.
- **Result (`scratchpad/make-check-20260909/make-check.log`):** `member_change_menu.bin` **OK**; **FAILED** `slus_006.64`, `field.bin`, `shop_menu.bin` (same 3 as prior handoff). `config/checksum.sha` has no `menu.bin` entry.
- **Side effect:** `gears clean` wiped gitignored `linker/undefined_*battle*`; restored empty stubs so port `gen_battle_bridge_map` + LINK OK (69 stubs).
- **Natural Equip smoke:** aborted — schedule extract failed (stuck field 490). Retry pending with script-built pad schedule.
- **Committed:** no

## 2026-09-09 natural Lahan→Equip smoke

- **Attempt 1 (aborted):** bad schedule extract (`sched len 9`) → stuck field 490, no shots.
- **Attempt 2 (full title schedule):** fields 490→4→2. No `GameHandleError` / `HeapFree(NULL)`. Prologue Circle mash still active on Lahan (~frame 68xx–70xx); xdotool Start/Down/Circle did **not** open menu (shots = field scene only). Process `rc -11` (SIGSEGV) on stop — not a clean Equip teardown proof.
- **Attempt 3:** mash end@6400; Start/Down/Down/Circle fired on field 2. No menu open (`func_800799D4` only at title). Circle@7440 → `FieldMain teardown exitCode=0` → battle-mips enter → `read32 fault` @0 (pc=0x800710fc). Early Lahan still script-locked (`FE54 … lock_control`); Start before free roam does not open Equip.
- **Still open:** natural Equip after player-control unlock; no HeapFree(NULL) proof on natural Equip teardown.
- **Committed:** no

## 2026-09-09 battle bridge regression (gears clean)

- **Symptom:** natural Lahan Circle→battle faulted at 114 insn (`read32 @0`, pc=`0x800710fc`). Adapter `functions=519 shared-data=152` vs prior working `607/188`.
- **Cause:** `gears clean` wiped gitignored `linker/undefined_{funcs,syms}_auto.battle.txt`; empty files shrank `g_BattleBridgeSymbols` 26616→22248 bytes.
- **Fix:** `splat split` battle.yaml (pinned 0.33.2 in toolchain image; strip unknown `generate_asm_macros_files` for that splat) regenerated 247 funcs + 87 syms. Rebuild LINK OK.
- **Runtime:** `run_battle_fix.log` — adapter `607/188`, enter battle, **returned after 84087050 instructions**, rc=0. No early null fault.
- **Menu note:** field main menu opens on Triangle (`D_800C3900 & 0x10`), not Start (`0x800`).
- **Still open:** natural Triangle→Equip after post-battle free roam (`D_800ADB68==1`).
- **Committed:** no

## 2026-09-09 natural Equip attempt 4 (PadOnControl)

- Path: title→Lahan→opening battle (adapter 607/188, returned ~84.5M insn)→field 14.
- **Fail:** Circle mash still active after battle; `D_800ADB68` flickered 0↔1 (~16 rises) with FE54 locks; settle never hit 90 frames. No Equip. Timeout rc -11. Shots none.
- **Fix (TEST TOOLING):** suppress `XENO_PAD_TEST_INPUT` on first `ADB68==1`, then settle, then Triangle→Equip.
- **Committed:** no

## 2026-09-09 natural Equip attempt 5 (aborted)

- Battle OK (607/188). Suppress-on-first-ADB68 left field-14 actor11 FE54 dialog stuck (`ADB68` stayed 0). No Equip.
- Source now pulses **Triangle on every ADB68==1 window** (before re-lock) instead of waiting for settle — matches light-init `799D4 request=128` path. Rebuild+retry.
- **Committed:** no

## 2026-09-09 natural Equip attempt 6

- **Fail:** `ADB64!=0xFF` latch fired at boot (`ADB64=0`) before any field — Equip nav ran on title splash. No Lahan. Shots useless.
- **Fix:** require `seen_run` (first real `ADB68==1`) before menu-latched phase.
- **Committed:** no

## 2026-09-09 natural Equip attempt 7

- Reached field 14 post-battle; Triangle pulsed on `ADB68==1`.
- **Fail:** latch used stale `g_XenoMenuNavReaderTicks=466` from title while `ADB64` still 255 — Equip nav ran with no field menu. Shot = Elly room, no UI. No `799D4 request=128` / Equip entry.
- **Fix:** latch only when `seen_run && ADB64!=0xFF` (real Triangle menu request).
- **Committed:** no

## 2026-09-09 natural Equip attempt 8

- Post-battle field 14: only **2** Triangle frames while `ADB68==1`, then lock; mash suppress left dialogs stuck. No `ADB64` change / no Equip. Timeout ~15 min.
- **Root:** Vsync inject runs after field body sampled pad; `ADB68` window too short for PressedOnce→main.c:662.
- **Fix (TEST TOOLING):** on gates match, also store `ADB64=0x80` (same as retail Triangle path) + slow Circle pulses while locked to re-open windows.
- **Committed:** no

## 2026-09-09 natural Equip attempt 9

- `ADB64=0x80` stored on ADB68 window, but **no** `799D4 request=128`. Shot = bare field 14.
- **Cause:** `func_800799D4` early-returns when `D_800B21D0[0]!=0` (dialog busy); `main.c` still sets `ADB64=0xFF` after the call — request evaporated.
- **Fix:** only request when `B21D0==0`; Circle while blocked; latch Equip nav on `g_PcPortFieldMenuOpened` (set in misc4 before MenuMain).
- **Committed:** no

## 2026-09-09 natural Equip attempt 10

- **Fail:** `g_PcPortFieldMenuOpened` fired on **title** `799D4 request=2`, Equip nav ran during New Game select, path stalled at field 4. No Lahan/Equip.
- **Fix:** arm open flag only when `(ADB64 & 0x7F) == 0` (field main menu from 0x80).
- **Committed:** no

## 2026-09-09 natural Equip attempt 11

- Path OK: title→NG→battle (607/188, ~84.5M insn)→field 14→`ADB68==1`+`B21D0==0`→`799D4 request=128`→MenuMain. No false title arm. No GameHandleError/HeapFree. rc=0.
- Shots: `natural-equip11.png` (early glitchy open), `natural-after11.png` (field menu Fei LV1; cursor on **Items**).
- **Fail:** Equip nav used Down×2 then Circle → opened **Items** (`Nav N2a`), not Equip. No `Equip entry`/`E0F78`.
- **Fix (TEST TOOLING):** one Down (Status→Equip) then Circle; drop redundant post-open Triangle.
- **Committed:** no

## 2026-09-09 natural Equip attempt 12

- Path OK again: battle→field 14→`799D4 request=128`→MenuMain; one Down + Circle.
- **Shot evidence:** `natural-equip12.png` matches forced-harness Equip weapon view size/content (Equip selected + Martial Art / stats panes). `natural-after12.png` = main list with Equip cursor after Cross.
- **Gaps:** no `Equip entry` printf (`XENO_FIELD_TEST` not set in smoke env); `MenuMain` never returned before harness SIGTERM (`rc=-11`). No GameHandleError/HeapFree observed.
- Next: rerun with `XENO_FIELD_TEST=1`, wait for MenuMain return / clean exit before kill.
- **Committed:** no

## 2026-09-09 natural Equip attempt 13

- **Fail (harness):** `XENO_FIELD_TEST=1` takes direct field-0 boot (`ADB68==1` early) → PadOnControl opened `request=128` before NG; smoke aborted as false title arm (~5s).
- **Fix:** (1) Equip entry printf also on `XENO_PAD_ON_CONTROL=equip` (no FIELD_TEST). (2) PadOnControl menu request only when `g_GameSceneMapNum==14`. (3) second Cross to close MenuMain.
- **Committed:** no

## 2026-09-09 natural Equip attempt 14 — PASS

- Natural path: title→NG→battle (607/188)→field 14→`799D4 request=128`→Down→Circle→`Equip entry func_801E0F78 slot=0 anim=1`→Cross×2→`MenuMain returned D_800594D0=0`.
- No GameHandleError / HeapFree(NULL). Map-14 gate held (no early arm).
- Shots: `natural-equip14.png` (Equip UI); `natural-after14.png` tiny/empty (captured at return; SIGTERM rc=-11 after done).
- **Accepted:** natural Lahan Equip open + Cross teardown with log evidence. TEST TOOLING remains unlabeled for removal.
- **Committed:** no

## 2026-09-09 Equip gear-name rows: D_801E9D88 stride fix (run14 "Martial Cap" at screen top)

- **Symptom (natural run14 + forced harness):** one equipped-accessory name ("Martial Cap") drawn at screen top outside any window; other two names sat low (y≈159/174) in the desc pane; weapon row invisible.
- **Cause:** `func_801D8EA4` (src/menu/main/misc.c) read `D_801E9D88[labelOffset + row]` as a `u16` array (2-byte stride). Retail asm (`func_801D8EA4.s` 0x801D9630..40) does `sll 2` + `lhu` — 4-byte stride, low halfword; data is `.short Y/.short 0` word pairs {0x76,0x85,0x92,0x9F,0xAE,0x1E,0x39,0x46,0x53,0x1E,0x39,0x53,0x6C}. Port mode-1 rows got y={0,159,0,174} instead of retail {30,57,70,83}.
- **Fix:** index `D_801E9D88[(labelOffset + row) * 2]` (+ same in `pc_port/tests/menu_d8ea4_retail_test.c`). Build LINK OK, binary sha256 38cf658c….
- **Runtime (forced harness, `scratchpad/equip-rowy-stride-20260909/run2.log`, shot `shots/run2-equip3.png`):** category window now reads Weapon / Accessories: Martial Wear, Martial Cap, Stamina Ring (retail row Ys); desc pane shows only "Angelic value+2". Equip entry + Cross cancel clean, rc=0, no GameHandleError/HeapFree(NULL). Natural Lahan re-run NOT done (same code path, mode 1).
- **Pre-existing test breakage (not this change):** `menu_d8ea4_retail_test.sh` fails before compiling: (a) hard-coded menu.elf range 0x801d83b4..0x801d8c14 but current `build/out/menu.elf` has D8EA4 at 0x801d3ae4 (same 0x860 size); (b) its support header extracts only `MenuRawPointer`, which now calls `MenuPsxPointerSlot` (added in dirty tree, absent at HEAD); (c) test seeds raw `g_Menu+0x358..0x364` while MenuRawPointer(0x360) now maps to `unk358[8]`. Separate logical change to repair; remapped scratch copy in `scratchpad/equip-rowy-stride-20260909/`.
- **Harness note:** `pkill -f "Xvfb :N"` inside a `bash -c` run kills its own shell (pattern matches the wrapper cmdline).
- **Still open:** desc-bank widths 0 for acc ids 1/16 (bank stubs — template-correct per run23–31); main-menu label bleed under Equip; stats-pane dimmed Hit%/Evade%/EthDef rows (unverified vs retail); sibling u16 table stride survey pending.
- **Committed:** no

## 2026-09-09 sibling u16-table stride survey (Luna/sonnet, read-only; two claims spot-checked by Astra)

- Same class as the D_801E9D88 fix (retail `sll 2`+`lhu` on `.short v/.short 0` word pairs; port `u16[]` at 2-byte stride). Each is its own logical change + runtime proof in its screen:
  - `D_801E9E58` + `D_801E9E4C` (`func_801D249C`, misc.c ~4281-4296): `D_801E9E58[i]` and `pSrc++` walk 2B; retail 4B. **Spot-checked data shape.**
  - `D_801E9894` / `D_801E9914` (`func_801E5924`, misc.c ~9889, matched asm `sll $a0,2`): `&tbl[slotIdx]` 2B vs retail 4B. **Spot-checked asm.**
  - `D_801EA590` / `D_801EA5DC` (`func_801E71B4`, misc.c ~10117): 2B vs retail 4B (agent-verified only).
  - `D_801E9DBC` (`func_801E05D0`, misc.c ~8837): index formula `(group*4+category)*2` vs retail `(group*4+category)` then `sll 2` — agent-reported, NOT re-verified.
  - `D_801EA5D0`: accessed at 1B/2B/4B strides by different retail sites; per-callsite check needed.
  - MATCH: D_801E96A8, D_801E96C8, D_801E9788/94/A0 (already `[cat*2]`).
- **Committed:** no

## 2026-09-09 boot-to-Blackmoon: natural path reaches map 15; navigation telemetry added

- **Natural map-15 arrival (proven, `scratchpad/boot-to-blackmoon-20260909/run1.log`, shot `shots/run1-field15.png`):** title→NG→opening battle (607/188, returned ~79.4M insn)→field 14→F8 quickload (`captures/native_play_20260907/run9/quick.xgqs`, map=15 pos=(739,80,-1385))→`FieldLoad begin field=15`. Terrain + Fei render. No GameHandleError/assert/unresolved.
- **SIGSEGV on exit is a teardown race, NOT a game bug (proven, coredumpctl bt):** main thread `PsyX_Sys_DoPollEvent`→`PsyX_Exit`→`exit()`→`_dl_fini` unloads SDL while the OpenAL/PipeWire thread is inside `StreamCallback` (PsyX_SPUAL.cpp:520) calling `SDL_LockMutex` → jump to NULL. Pre-existing PsyCross shutdown ordering; affects every clean quit, not the retail path.
- **map15 DOES exit to map16 (evidence, script bytes):** `scratchpad/lahan-natural-20260907-arithmetic/field15-data/scripts.bin` contains opcode-152 CHANGE_FIELD records `98 <map> 80 <ent> 80` at 0x1826 map=16 ent=0, 0x1d67 map=17 ent=0, 0x1d26 map=2 ent=8, 0x1d2e map=1 ent=8. This CONTRADICTS the reading that map16 is only a cutscene that warps to 15 (`docs/evidence/blackmoon-map16-render-20260906/README.md` §0/§7) — that README documents map16's END warp, not its entry. Which trigger zone guards the map-16 record is not yet resolved (backward byte scan for opcodes 201/203/204 is unreliable across variable-length instructions).
- **TEST TOOLING added:** `pc_port/src/field_pos_diag.c` (+1 line in `pc_port/build_port.sh` PORT_SOURCES, +1 call in the `psyq_compat.c` Vsync shim). `XENO_FIELD_POS_DIAG=<N>` prints map, player pos and containing trigger zones every N Vsync frames, using the same packed-Z:X NormalClip quad test as `FieldScriptCheckTriggerZone2D`. Read-only, port-owned, no matching TU touched. Removal instructions in the file header. Build LINK OK; telemetry confirmed live (236 POSDIAG lines, walk2.log).
- **Map 15 has 17 trigger zones** (408-byte `triggers.bin` / 0x18); decoded X/Y/Z extents in this session's transcript. Player quicksave spawn (739,80,-1385) is inside none of them.
- **Harness note:** the opening battle needs >900 s wall-clock under llvmpipe; walk2 timed out waiting for field 14 mid-battle. Use >=1500 s.
- **Committed:** no

## 2026-09-09 map15 script decode: map16 is a SCENARIO-FLAG cutscene, not a walk exit (Astra/opus, static)

- Decoder + full output in `scratchpad/boot-to-blackmoon-20260909/` (`build_optable.py`, `optable.json`, `decode.py`, `field{1,13,14,15}-decode.txt`, `field15-zones.txt`). Opcode lengths auto-derived from each handler's `scriptInstructionPointer += N`; table1 = 256 entries, op 254 = `FE` prefix into table2. Map 15 code base = 0x1044 (header: 0x80 numScripts, 0x84 metadata numScripts*0x40). 2801 sites decoded, 3 unknown-opcode stops (none relevant).
- **All four map-15 CHANGE_FIELDs and their guards:**
  - `0x07e2` `98 10 80 00 80` → **map 16 ent 0**, in actor/script **22** routine1 (@0x0733), guarded by `CJMP var[0x0020] < 80` after routine0's `CheckScenarioFlagsEqual(0x0F)`; preceded by 3 camera pans + `FadeIn 0x3c` + `SetScenarioFlags(0x10)`. **No trigger-zone test anywhere in the block.**
  - `0x0ce2`/`0x0cea` → map 2 / map 1 ent 8, guarded by `CheckTriggerZone2D zone 14` (south, x -147..1407 z -2976..-1993).
  - `0x0d23` → **map 17 ent 0**, guarded by `CheckTriggerZone2D zone 11` (north, x -298..120 z 1974..2705); both arms of the `CheckScenarioFlagsLessThan(0x0d)` converge on the change-field.
- Brute scan of opcodes 10/201/203/204: zones 0-7 are proximity/event flag-setters; zones 8,9,10,15,16 never referenced.
- **Correction to this session's earlier note:** the raw `98 10 80 00 80` byte hit at scripts.bin 0x1826 is real, but it is NOT a walkable boundary — Blackmoon Forest is entered by map 15's actor-22 cutscene when the scenario flag == 0x0F. So both readings were partly wrong: map16 IS entered from map15 (contra the map16 README), but via script/scenario progression, not a trigger zone (contra my earlier inference).
- Both real walk exits use the same idiom: zone hit → `FE 02` (`func_80095B3C`) → `CJMP var==0` → CHANGE_FIELD, so the harness must WALK IN; teleporting into the AABB is not enough.
- **Unresolved:** script var `0x0020` is never written by maps 1/13/14/15 nor any engine site found — set by an undumped map. Next: dump map 17 scripts and re-run `decode.py`; and read the scenario flag in the map-15 quicksave.
- **Committed:** no

## 2026-09-09 boot-to-Blackmoon: story chain + telemetry fixes + random-battle wall

- **Scenario chain (decoded, `field{13,15,17}-decode.txt`):** map13 sets scenario 7 (@0x02d5); map17 sets 12 (@0x0621) and 15 (@0x0207, after `CheckScenarioFlagsEqual(13)`); map15 actor 22 → map16 needs scenario==15. Nothing dumped sets 13 → presumably map 20 (Citan's house: map17 has CHANGE_FIELD map=20 ent 0/1 at 0x1ed3/0x1f75, and map=15 ent 0 at 0x1f37). Map 17 dumped this session (`field17-data/`, 5 zones, 59 scripts, code base 0xf44).
- **Live telemetry at natural map-15 arrival:** `scenario=7`, `var20` is a running counter (64151→64306), player spawn inside no zone. So the map-16 cutscene is NOT armed at this story point; route is 15→17→20→17→15→16.
- **Telemetry fixes (TEST TOOLING, `pc_port/src/field_pos_diag.c`):** (1) segfault in battle reading torn-down `pActorData` → gated on `PcPort_QuickCheckpointFieldIsActive()` (new getter in quick_checkpoint.c/.h); (2) fixed-32 zone scan read past the table (bogus "zone 17" on map 15) → count now from field header `D_8005A4E0+0x12C / 0x18`; (3) prints `scenario=` and `var20=` script words. LINK OK.
- **Walk harness (`walk2.py`):** fixed d-pad calibration goes stale as the follow camera rotates (walk4 jammed 100+ iterations against terrain); now re-estimates per-key world vectors from observed motion (EMA) and blacklists blocked keys.
- **Random encounter wall (observed, walk5):** second `enter retail battle` on map 15 hung the natural run — live gdb bt shows guest looping through `Vsync` via `runtime_bridge_call`; turn clock never advances (known open item). Harness now Escapes out (explicitly non-retail). Diagnosis delegated (Astra/opus) — see next entry.
- **Committed:** no

## 2026-09-09 ROOT CAUSE: no vblank-IRQ pad push — retail busy-waits deadlock the port

- **Falsified the "battle runs at 1/20 speed / clock saturated" hypothesis (measured, live gdb, 15 s window):** `PsyX_EndScene` 900 hits, emulated vblank delta 901, so the port renders at ~60 fps; `D_80059494` (battle catch-up steps, written only by `func_800BE790`) histogram = {0:896, 1:4} — i.e. no catch-up needed. `ControllerPushState` 900 / `ControllerPopState` 1800 (2:1, not 5:1). The battle clock is healthy.
- **Battle DOES progress with real input (observed, shots in `scratchpad/battle-clock-20260909/`):** on a natural map-15 random encounter, Triangle attacked, the Hobgob died, and the Exp/results screen appeared (`live-before.png`, `live-after-z.png`, `live-after-vz.png`). The standing "turn clock never advances / no enemy turns" note needs re-verification; what I hit is a different, specific wall.
- **ROOT CAUSE (proven, live gdb + asm):** retail refills the controller state queue from the **vblank IRQ** (`func_8003634C` → `ControllerPoll` + `ControllerPushState`), asynchronously. The port only pushes from the blocking `Vsync(0)` in the psyq shim. Any retail busy-wait that drains the queue and spins for new input WITHOUT calling Vsync therefore deadlocks forever.
  - Confirmed instance: Start in battle → `func_8008A684` `.L8008A76C` sets the pause byte `D_800C3444` and mutes SPU; `.L8008A804` → `.L8008A738` then spins on `func_80036410` + `ControllerPopState` only. Live: `g_ControllerNumStates == 0` permanently, `s_padTestFrame` frozen at 35385 across samples (no frames presented), main thread spinning in `PcPortMipsRun` reading guest 0x8008A808. `D_800C3444` is **guest-RAM only** (absent from `battle_bridge_map.inc` and from the native symbol table), so the guest's pause flag stays set.
  - Player impact: pressing Start during a battle hangs the game permanently.
- **Fix (port-owned):** `PcPort_PadVblankPump()` in `pc_port/src/psyq_compat.c`, called from `runtime_bridge` (`pc_port/src/battle_mips_runtime.c`). Fires only when the emulated vblank counter has advanced >1 frame past the last `Vsync(0)` push, i.e. exactly when the guest has stopped presenting; inert in normal play so the one-push-per-frame balance the battle's one-pop-per-frame reader needs is preserved. Build LINK OK.
- **Verification pending:** `scratchpad/battle-clock-20260909/pause_test.py` (boot → map15 → random battle → Start → Start → assert frames still advancing).
- **Harness caution:** `xdotool key <k>` press+release can be shorter than one sampled frame; use keydown/sleep/keyup. Also do not probe with Start unless testing pause.
- **Committed:** no

## 2026-09-09 vblank pad pump VERIFIED (battle pause no longer hangs)

- **Test:** `scratchpad/battle-clock-20260909/pause_test.py pause3 12` — natural boot → F8 quickload map 15 → walk until a **random** encounter → Start (pause) → Start (unpause) → probe liveness.
- **Result: PASS.** Presented-frame counter (`s_padTestFrame`, sampled by gdb) advanced 12624 → 13112 (+488 in 8 s ≈ 61 fps) after the pause/unpause; pump fired 3 times (`[xeno-port][pad] vblank pad pump`). Shots `shots/pause3-paused.png` vs `pause3-final.png` show the Hobgobs and Fei in different animation poses, so the battle really resumed — not merely "not frozen".
- Pre-fix the same sequence froze permanently (`pause2`: zero frames after pause, main thread spinning in `PcPortMipsRun`).
- **Note:** AP=0 with the Attack/Defense command ring open is Wait mode — retail does not advance the clock there. That, plus this deadlock, likely explains the older "battle turn clock never advances" note; it should be re-verified rather than trusted.
- **First liveness probe was invalid:** POSDIAG is field-gated, so it cannot advance during a battle; use the pad-schedule frame counter instead.
- **Committed:** no

## 2026-09-09 random battle: works, then stops consuming input (new, precise open item)

- **Random encounters run correctly under the fixed binary (observed, `scratchpad/battle-clock-20260909/`):** on a natural map-15 encounter, holding Circle (confirm Attack) then Cross killed **2 of 3 Hobgobs** (`fight-1.png`: 1 enemy left, AP=2, ring open). Frames present at ~62 fps throughout (`s_padTestFrame` +309 in 5 s).
- **Pause was masking it.** With the ring open and attacks doing nothing, `ControllerPopState` ran **7893 times / 10 s vs 298 pushes** — the guest was back in the `func_8008A684` pause scan loop. One Start press dropped pops to **19 / 10 s**, i.e. the game had still been paused. Diagnostic recipe: measure poll/push/pop rates; a pop:push ratio in the thousands means a queue-drain busy-wait, ~1:1 means normal.
- **Remaining wall (open, NOT root-caused):** after several turns the battle stops consuming input entirely — pop rate collapses to ~2/s (vs ~1 per frame when awaiting a command), the Attack/Defense ring stays drawn, AP sits at 2, but the left gauge and sprite animation keep advancing. 75 held Circle/Cross presses changed nothing. Because the pop rate is *low* rather than high, this is NOT the queue-drain deadlock class; it looks like the battle is parked in an animation/turn-resolution state with a stale ring overlay. Needs a guest-PC histogram (as in `bridge_hist.py`) taken in that state to identify the loop.
- **Note on `g_C1ButtonStateReleased`:** despite the name it is the **rising edge** (`state ^ prev & state`, controller.c), nonzero for exactly one poll. Sampling it from gdb mid-hold correctly reads 0 — do not conclude "input is not arriving" from that.
- **Push rate in battle is ~30/s, not 60/s** — partly the pump's >1-vblank threshold. Retail's vblank IRQ pushes at 60/s. Worth revisiting if input feels lossy; not yet shown to cause a defect.
- **Committed:** no

## 2026-09-09 scenario-flag writer survey (static decode of maps 0-20)

- Dumps + decodes for maps 0,2-12,16,18,19,20 added under `scratchpad/boot-to-blackmoon-20260909/field<N>-{data,decode.txt}` (same gdb recipe; `dump-2-12.log`).
- `SetScenarioFlags` immediates by map: map2 → 15, 1; map3 → 0x18; map4 → 3; map12 → 8; map13 → 7; map17 → 12 (@0x0621), 15 (@0x0207, guarded `==13`); map19 → 10; map20 → 0x132 (odd; map 20 exits to map 714/17 — likely not Citan's house). Maps 0,1,5-11,14,15(sets 16 inside the cutscene),16,18: none.
- **No field script in the Lahan block sets scenario 13**, the value map 17 needs to arm the map-16 cutscene. Candidates: the battle overlay (story Gear battle) writing script memory directly, or a generic variable-write opcode targeting address 0 that the decoder does not classify. `FieldScriptVMGetArgument`: bit 15 set = immediate (`& 0x7FFF`), else variable read — so `87 32 81` really is 0x132.
- Map-16 script itself: single CHANGE_FIELD → map 15 ent 3 (the end-warp the README documented).
- **Committed:** no
- Map 17 entry flow (script0.routine1 @0x00a6, decoded): if `var[0x02c6] & 0x08` → @0x01d0: `scenario < 0x18` and `>= 13` and `== 13` → cutscene (sleeps, FE0E, actor moves) → `SetScenarioFlags(15)` @0x0207. So 13 must already be set (and var 0x2c6 bit 3) before re-entering map 17. First-visit path sets 12 @0x0621. Map 714 (from map 20) sets 0x12f and exits to map 712 — the 0x1xx values look like a later-chapter numbering; the Lahan block uses 1..24.
- No writer of 13 found in maps 0-20/714 (all `FieldScriptMemoryWrite*` handler opcodes with dest 0x0000 scanned: none). Runtime observation of `scenario=` in POSDIAG through the story is the reliable path; static search parked.

## 2026-09-10 battle overlay: link unblocked + segment model corrected

- **Starting state:** gears.toml listed `battle` in overlays, but the matching build STOPPED at the battle link with 16 undefined `D_800Cxxxx` references; the checksum gate could not run at all; `build/out/battle.bin` did not exist; `src/battle/main.c.o` was left mid-compile by an interrupted run (Sep 10 09:42).
- **Defect 1 (data modelled as code):** `- [0x1450, c, main]` ran to the BSS boundary. Code actually ends at file 0x5255C / VRAM 0x800C204C (func_800C11CC's 928 instructions). 0x800C204C is read/written as a byte (`lbu`/`sb` at 80080C08/80080E24/80081130), never a `jal` target; 0x5255C..0x534DC is pointer/parameter data (1672 words, 139 in-overlay pointers; self-referential `.word 0x800C2FCC` at 0x53544); splat could not decode it as MIPS, so it produced `.byte` dumps named `func_800C204C`/`func_800C2FCC` whose interior addresses the code referenced under *different* auto names - hence the undefined symbols. Fix: `- [0x5255C, data]` + drop the two INCLUDE_ASMs.
- **Defect 2 (section order):** leading block was `[0x0, data]`; gears emits `data` after `.text`, so the built file led with code. Retail leads with that block. Fix: `- [0x0, rodata]` (same idiom as field). Measured: first differing byte vs disc/battle.bin moved 0x0 -> 0x8D0.
- **Build:** `make build` in podman `localhost/xenogears-dev-toolchain:current` with `. /.venv/bin/activate` (splat 0.33.2; the venv must be activated or gears' splat calls fail with "No module named splat"). 483/483 tasks, 0 failures. Log: `scratchpad/battle-overlay-split-20260910/build5.log`.
- **Parity:** battle.bin 342684 bytes sha256 cbd12587… vs retail 343936 / 1830b4ef…. Short 1252, entirely downstream of the code deficit (in-overlay pointers + BSS symbols shift; `D_800C3EA4` assembles 0x800C39C8).
- **Census (854 functions):** only `func_800B2AEC` (retail 2140 / built 844) and `func_800B16A4` (76/76, one word) are non-exact. `func_800B16A4`: retail walks a RISING index; rewrote the C accordingly (now 19 words, 18 identical; residual is the accumulator init `addu a2,a1,zero` vs `move a2,zero`, which both gcc-2.7.2-psx and a temporary gcc-2.6.0-psx preset fold - preset reverted).
- **No cross-overlay regression:** slus f7c1f169, field c012e0d8, shop 770921de (same 3 known-red), member_change_menu 3b9e2b89 OK, menu 0f6d8aac.
- **Evidence:** `docs/evidence/battle-overlay-segmentation-20260910/README.md`.
- **Committed:** no

## 2026-09-10 BATTLE OVERLAY BYTE-EXACT AND GATED

- **Result:** `make rom-check` (from-clean) -> `PASS build/out/battle.bin 1830b4ef1fe37129 (343936 bytes)`. battle.bin added to config/checksum.sha (new pin; existing pins untouched). Other gate lines unchanged: slus f7c1f169 / field c012e0d8 / shop 770921de FAIL (known-red), member_change_menu 3b9e2b89 PASS, menu 0f6d8aac WIP. make build 483/483, 0 failures.
- **Defect 3 (this pass, the big one):** `INCLUDE_ASM` is a file-scope `__asm__`; the pinned compiler emits ALL asm blocks before ANY compiled body (`build/src/battle/main.c.o` text: func_80070F40..func_800C11CC, then the ten C functions at 0x51070+). So any C body in that TU leaves its retail slot and shifts everything after it - even with defects 1/2 fixed the file still differed in 27,388 runs. All ten are now `#ifndef XENO_PC_PORT INCLUDE_ASM(...) #else <body> #endif`; func_800B2AEC's port body stays in src/battle/primitive_colors.inc (game_overrides.c only).
- **Defect 4:** BSS placed 4 high - `.bss` inputs are 16-byte aligned (GNU as default), so address-less `.battle_bss (NOLOAD)` rounded 0x800C3A6C -> 0x800C3A70 (460 labelled symbols misplaced; D_800C3EA4 assembled 0x800C3EA8). Splat emits an explicit address only for an all-NOLOAD segment, so the BSS became its own top-level segment (`type: code`, start 0x53f7c, vram 0x800c3a6c, `align: 4`, one bss subsegment) -> `.battle_bss_bss 0x800C3A6C (NOLOAD)`. `align: 4` is load-bearing (default 16 advanced __romPos to 0x53F80 -> 343940-byte ROM). Top-level `type: bss` crashes pinned splat 0.33.2 (CommonSegBss dereferences self.parent).
- **Confirmation method:** hand-edited linker/battle.ld to `.battle_bss 0x800C3A6C (NOLOAD)` first; ninja relink gave the exact retail hash, proving the target before finding the config-level expression.
- **Port side:** ./pc_port/build_port.sh LINK OK (69 stubs / 547 data symbols; port still provides func_800B2AEC). run_battle_primitive_colors_retail_test.sh B2AEC GREEN (4230 cases x O0/O2/UBSan vs real disc/battle.bin leaf + real SLUS clamp, 7 controls rejected). run_battle_animation_leaf_test.sh PASS (the test that consumes the guarded func_800B168C/800B16A4 bodies).
- **Symbol identity check (reusable):** compare `nm` addresses against the address inside each `D_/B_%08x` label name - battle.elf 705 labelled, 0 misplaced (was 460). Same check shows field 523, slus 448, menu 191 misplaced (their known reds), shop 0, member_change 1.
- **Not established:** decompilation. Every non-trivial battle function is still INCLUDE_ASM. Next: matching C for func_800B2AEC (535 insns) and func_800B16A4 (18/19 words; residual `addu a2,a1,zero` vs `move a2,zero` under gcc 2.7.2-psx and 2.6.0-psx). A matching body also needs a TU that emits it in retail position.
- **Evidence:** docs/evidence/battle-overlay-segmentation-20260910/README.md (rewritten for the final state).
- **Committed:** no

## 2026-09-10 SHOP_MENU.BIN BYTE-EXACT (TU split at retail run boundaries)

- **Result:** `make rom-check` from clean -> `PASS build/out/shop_menu.bin 7890e14bcabddcf8 (55296 bytes)`; `cmp build/out/shop_menu.bin disc/shop_menu.bin` identical. Gate now 3 PASS (member_change 3b9e2b89, shop 7890e14b, battle 1830b4ef) + 2 unchanged known-red (slus f7c1f169, field c012e0d8). Verifies the shop overlay's 104 decompiled C functions byte-for-byte.
- **Root cause (not drifted code):** all 118 functions already had exact retail sizes, but 111 sat at a uniform +15548 (0x3CBC). `INCLUDE_ASM` = file-scope `__asm__`; cc1 emits every file-scope asm block BEFORE every compiled body (reproduced standalone with tools/gcc-2.6.0-psx/cc1 on a 3-item file). Retail interleaves 9 C runs with 8 asm runs; the single `[0x40, c, main/misc]` subsegment laid the object out as [14 asm functions][104 C functions].
- **Fix:** one subsegment per retail run pair: [0x40 misc], [0xCBC misc2], [0x6CF0 misc3], [0x7278 misc4], [0x7720 misc5], [0x7FF4 misc6], [0x8D14 misc7], [0x991C misc8], [0xAF58 misc9] over src/shop_menu/main/misc{,2..9}.c (bodies unchanged, same preamble, per-part INCLUDE_ASM dirs).
- **Preset trap:** gears preset keys are substring matches; `shop_menu/main/misc.` does not match misc2.c..misc9.c, so they compiled with Default 2.7.2 instead of XenoLibrary 2.6.0 — 15 functions changed size (8 short/7 long, net -40 bytes). Added misc2..9 paths to the XenoLibrary preset in gears.toml.
- **Prototype trap:** ShopMenuBuyMenu's call to func_801CCE1C lost the `u8` prototype that used to be visible from its definition in the same big TU -> arg passed unmasked (`move a1,s0` vs retail `andi a1,s0,0xFF`), a 4-byte diff. Prototype added to the shared preamble.
- **Jump table:** ShopMenuSellMenu's table lives in the overlay .rodata at 0x801C5028 and its owner is an unmatched function; splat emits it as a blob not linked on its own -> `INCLUDE_RODATA("asm/shop_menu/nonmatchings/main/misc", jtbl_801C5028)` in misc9.c. (Modelling those regions as `asm` subsegments instead leaves it undefined.)
- **Port:** ./pc_port/build_port.sh LINK OK, 69 stubs / 547 data symbols - identical to before.
- **Method that made this tractable:** compare `nm` addresses against the address embedded in each symbol name (`func_%08x`/`D_%08x`), and compare per-function bytes at (built addr) vs (retail addr). Battle now shows 0 misplaced of 1740; shop 0 misplaced, 0 size mismatches, 0 byte mismatches.
- **Reusable lesson:** any overlay TU mixing INCLUDE_ASM with C bodies cannot be byte-exact; split at retail run boundaries. field.bin and slus_006.64 both mix AND have real size deficits (field ~18 KB short), so ordering alone will not close them.
- **Evidence:** docs/evidence/shop-tu-split-20260910/README.md (pre-split source at scratchpad/shop-split-20260910/misc.c.orig; logs build1..6.log).
- **Committed:** no

## 2026-09-10 field/slus census + two field function fixes

- **Field census (878 functions):** 662 exact / 153 short (Σ 20,448 bytes) / 63 long (Σ 1,368) / net **-19,080**; file 242,934 vs retail 260,862. **slus census (1101 parsed):** 889 same-size-moved / 134 short / 76 long / 2 missing / net -3,124 (file 303,248 vs 303,104 - the +144 is outside the functions, i.e. data/rodata). Both overlays also mix INCLUDE_ASM with C bodies in single TUs, so the shop-style run split is a prerequisite for exactness there too.
- **Rejected: "field was compiled with gcc-2.6.0".** Adding a `field/` key to the XenoLibrary preset changed field.bin (c012e0d8 -> 8e11dfe8) and reduced the net size delta slightly (-19,088 -> -18,444) but matched FEWER functions in every single TU (e.g. main/misc 150->105 exact, misc11 90->73, camera_movement 29->13). Reverted. Default 2.7.2 is the right compiler for the field overlay.
- **Per-function byte comparison in field is confounded by data/BSS addresses**: the overlay's text is ~18 KB short, so every symbol after it (D_800B14AC and friends) is displaced and any %hi/%lo reference differs. Sizes and instruction shapes are the reliable per-function signals there.
- **Fixed `func_800809D0` (src/field/main/misc8.c)**: the C summed only the post-increment accumulators, while retail adds the initial accumulator too (`addu a0,a0,v1` before the loop, `subu a0,a0,v1` after - GCC's rotation for a loop whose body starts with `count += acc`). Rewriting the body to `count += acc; acc += step; D_800B14AC += 2;` reproduces retail's 18-instruction / 72-byte shape exactly (verified mnemonic+register-for-register against asm/field/matchings/main/misc8/func_800809D0.s; only the data addresses differ, from the global text deficit). This is a real behaviour fix, not just a size fix.
- **Fixed `FieldScriptMemoryWriteU16` (src/field/scripts/virtual_machine.c)**: parameter was `u16`, and 2.7.2 collapsed `(address>>1)<<1` to `andi`; retail does `sra a2,a0,1; sll v0,a2,1` on the full 32-bit argument, i.e. the source value is signed/32-bit. Now `s32 address` -> 28/28 bytes with retail's instruction shape (register allocation still differs: retail a2/v0 vs built a0).
- **Effect:** field exact 660 -> 662, short 155 -> 153, net -19,088 -> -19,080; field.bin 242,926 -> 242,934 bytes. shop_menu 7890e14b and battle 1830b4ef unchanged (no cross-overlay regression).
- **Pattern for the rest of field:** the big deficits are port-oriented C that calls helpers retail inlined (e.g. func_800A3474 calls FieldStateRestoreCopy 8x; marking it `static inline` moved it 416->964 bytes but retail is 2072, so retail's copies are struct-shaped/unrolled, not a byte-copy loop) or uses loop shapes GCC rotates differently. Each is a retail-shaped transcription, not a mechanical edit.
- **Committed:** no

## 2026-09-10 field: func_8007F5AC transcribed to retail's unrolled form

- **Tool:** `scratchpad/field-repair-20260910/fdiff.py <function>` prints retail (parsed from the splat .s) vs built (objdump) instruction by instruction with operands normalised (registers, %hi/%lo, immediates, labels), so the overlay's data-address displacement does not hide real differences. It made the -4/-8/-12 deficits legible in seconds.
- **Rejected: a global delay-slot/nop difference.** Across the 222 size-mismatched field functions, retail has 32,501 insns (3,024 nops) vs built 27,685 (2,446); the 4,816 missing instructions are 578 nops, and the small-deficit histogram shows retail sometimes with *fewer* nops than us. No systematic scheduling flag to chase.
- **Fixed `func_8007F5AC` (src/field/dialogue/text_box_render.c)**: the C wrote the portrait UVs with a `for (i = 0; i < 2; i++)` loop and computed `GetClut` before the loop. Retail (332 bytes) unrolls both primitives, stores corner by corner (poly0.u0, poly1.u0, poly0.v0, poly1.v0, ... at 0x450/0x478/0x451/0x479/0x458/0x480/...), addresses each array element directly, and calls `GetClut(0, faceDirection + 0xE0)` *after* all sixteen stores. Rewritten that way: 200 -> 320 bytes of retail's 332 (80 of 83 instructions). The remaining 12 bytes are three re-loads: the retail compiler re-reads the UV bytes at the later stores while ours keeps them in registers (a full inline-expression version compiled to 544 bytes and a pointer+re-load version to 428, so 320 is the closest faithful shape).
- **Census caveat:** `make build` regenerates asm/ via gears, and splat's per-function *retail* sizes shift by a few functions between regenerations, so census counts taken across builds are noisy. Use `build/out/<overlay>.bin` size + sha256 as ground truth and confirm size deltas are explained by the functions actually edited (here: field.bin 242934 -> 243054 = exactly the +120 from func_8007F5AC).
- **Gate after this pass:** PASS member_change_menu 3b9e2b89, shop_menu 7890e14b, battle 1830b4ef; FAIL slus f7c1f169, field.bin d78963d4 (243054 bytes vs retail 260862).
- **Committed:** no

## 2026-09-10 field: index-vs-pointer addressing is a real, quantified lever; func_8007DCF8 fixed

- **Discovery:** retail field code addresses global arrays as *symbol + scaled index + offset* per access (`lui at, %hi(g_FieldTextBoxes + 0x37C); addu at, at, index*0x498; lh v0, %lo(...)(at)` - three instructions), while the port C keeps a local base pointer (`u8* pTextBox = (u8*)&g_FieldTextBoxes[i];` then `*(s16*)(pTextBox + 0x37C)`), which the compiler folds into one base register plus immediate offsets. Every such access therefore costs retail two instructions more.
- **Quantified for the whole overlay:** across the 222 non-exact field functions the build is missing 4,740 instructions versus retail, and retail has 488 more `lui`s than the build - up to 976 instructions (about 21%) of the shortfall is this addressing shape alone. The remaining ~79% is genuine missing logic.
- **Fixed `func_8007DCF8` (src/field/dialogue/text_box_render.c, the text-box cursor/timer updater)**: rewritten from `pTextBox + offset` accesses to `g_FieldTextBoxes[index].<field>` (using the named struct fields from include/field/text_box.h). 288 -> 472 bytes against retail's 468 - 118 of 117 instructions. The single remaining extra instruction is a block-layout choice: retail keeps the `func_8003487C` (window-not-initialised) arm out of line at the end, GCC keeps it inline plus one extra `j`. Early-return, if/else, and explicit-goto formulations all compile to the same 472, so it is not a source-visibility problem.
- **Effect:** field.bin 243054 -> 243238 bytes (exactly the +184 from this function). Gate unchanged: PASS member_change_menu 3b9e2b89, shop_menu 7890e14b, battle 1830b4ef; FAIL slus f7c1f169, field.bin e370eff3.
- **Per-function addressing census (retail `%hi(symbol+offset)` materialisations inside otherwise-short functions):** func_8007E1C0 66, FieldTextBoxInitializePrimitives 39, func_8007F8DC 35, func_8008004C 29, FieldDistortionDraw ~44, func_80072D74 ~42. For func_8007DCF8 the whole deficit was this shape; for the big text-box renderer it is ~528 of 2004 bytes.
- **Committed:** no

## 2026-09-10 field: addressing lever is function-specific; two more edits landed

- **Corrected the lever's scope.** The raw signal (retail has 488 more `lui`s than the build across the non-exact functions) overstates what is recoverable. Mechanically rewriting every `pBackground/pArrow/pCursor/pBorders/pPortrait` access in `FieldTextBoxInitializePrimitives` to `g_FieldTextBoxes[index].<struct>.<field>` recovered only **8 bytes** (1576 -> 1584 of retail's 1952), because GCC already materialises the address for each *call argument* (`SetDrawMode(&g_FieldTextBoxes[index].background.drawModes[0], ...)`) regardless of how the C spells it. The lever pays only where the C caches a base pointer for *scalar* field loads/stores - as in func_8007DCF8, where it recovered the entire 184-byte deficit.
- **`func_8007F6F8` (text-box open/close)**: rewrote the three `func_80034614/800345E0/800346D4` calls from a cached `pTextBox` to per-call `(u8*)&g_FieldTextBoxes[index] + 0x18`. No codegen change - still 280 vs retail 284. The remaining 4 bytes are a delay-slot scheduling difference: retail hoists `addiu v0,zero,-1` into the `bnez` delay slot (the -1 later stored to four fields), GCC emits `nop` there and materialises -1 after the three calls. Not reachable by source shape at this call structure.
- **Opposite-direction case (recorded for later): `func_8007254C` is +108 bytes** because the port materialises each camera global by its own symbol while retail keeps `$s0 = &g_CamInterpolation` and reaches the adjacent globals through negative offsets. The port comment already notes the host layout does not preserve PSX-relative positions, so the retail shape needs the matching/port split (`#ifndef XENO_PC_PORT` ... `#else` ...) rather than a plain rewrite. Same class: func_80080A74 +116, func_800A90B4 +104, func_8009C5A8 +100.
- **Net this pass:** field.bin 243238 -> 243246 bytes (exactly the +8 from the InitializePrimitives rewrite). Gate unchanged: PASS member_change_menu 3b9e2b89, shop_menu 7890e14b, battle 1830b4ef; FAIL slus f7c1f169, field.bin bfeed78d. pc_port/build_port.sh LINK OK, 69 stubs / 547 data symbols - unchanged.
- **Committed:** no

## 2026-09-10 runtime: natural boot reaches the title and New Game (screenshots)

- **Run 1 (no input, 900 s, scratchpad/boot-progress-20260910/)**: boot -> PsyX (llvmpipe 4.6, OpenAL) -> field 490 -> STR 13 plays to its end ("exit reason=frame-budget frame=4462 end=4462") -> the map is re-entered and STR 13 plays again. Three full plays in 15 minutes, no title. The intro movie does not self-advance to the title.
- **Run 2 (presses Circle at each movie end, 600 s, scratchpad/boot-progress-20260910/run2/)**: same boot; at 5.5 min `title loop enter choice=1` fires -> `title confirm choice=2` two seconds later -> New Game accepted -> the opening cutscene plays (bridge scene). So the title menu is reachable and New Game works; it needs a button press to leave the intro movie.
- **Direct field route smoke (scratchpad/field-repair-20260910/smoke/)**: `XENO_FIELD_TEST=1 XENO_FIELD_MAP=1 XENO_FIELD_ENTRANCE=6` -> `FieldLoad begin field=1` in 4 s, Lahan village renders (grass, path, blue-roofed house, tree, Fei sprite), process alive at the end, no assert/GameHandleError. This is the route that exercises the text-box functions edited this session (FieldTextBoxInitializePrimitives runs at load; the cursor/portrait code runs with a window).
- **Key frames:** `boot-progress-20260910/run2/shots/title.png` (New Game / Continue / Sound with the red cursor), `run2/shots/t573.png` (opening bridge cutscene), `run1/shots/t211.png` (intro FMV), `field-repair-20260910/smoke/shots/field.png` (Lahan village).
- **Method note:** Xvfb needs `-displayfd` on a *pipe* fd (`os.pipe()`); passing a temp file makes it exit with "Cannot write display number to fd 0" and SDL then fails to initialise. Working harnesses: `scratchpad/boot-progress-20260910/boot.py` (natural boot + input) and `scratchpad/field-repair-20260910/smoke.py` (direct field route).
- **Open observation (not yet diagnosed):** with no input the intro movie replays instead of falling through to the title; whether that is a port state-machine bug or intended "movie loops until input" behaviour is NOT established. Retail behaviour should be checked before treating it as a defect.
- **Committed:** no

## 2026-09-10 runtime: full opening path end-to-end + text box + portrait verified on screen

- **Run 5 (foreground, input-driven, `scratchpad/boot-progress-20260910/run5/`)**: 12:28:00 boot -> 12:28:20 field 490 (intro) -> 12:33:55 `title loop enter choice=1` -> 12:33:57 New Game confirmed -> 12:34:03 field 4 (prologue narration) -> 12:35:59 field 2 -> 12:37:21 field 14 -> 12:37:29 end, process alive, no assert/GameHandleError. Whole path ~9.5 min wall-clock under llvmpipe.
- **On-screen evidence:** `f022.png` = prologue narration text typing out ("Eventually, after continuous swings in the state of the war, Kislev gained the upper hand..."); `f024.png` = Lahan village overview; **`f026.png` = the opening Gear battle with a bordered dialogue window: Fei's portrait, the name "Fei", "Hyaa", over the Weltall/Id scene** - i.e. the text-box path whose functions were edited this session (FieldTextBoxInitializePrimitives at load, func_8007F6F8 open, func_8007DCF8 cursor/timer, func_8007F5AC portrait UVs) renders correctly at runtime; `final.png` = the burning-village fire effect.
- **Harness lessons (both cost a run):** (1) `nohup ... &` background drivers are killed when the launching command returns - keep long runs in a foreground session; (2) the port-side test scripts (`run_field_map0_smoke.sh` etc.) tear down Xvfb/xeno-port, so never run them while a boot run is live; (3) heavy parallel container builds starve llvmpipe and slow the emulation several-fold.
- **Test sweep:** 15 field/kernel test scripts pass in the container and 2 more on the host (`run_field_map0_smoke.sh`: Map1/Map14/Map15/Map16/Map383 runtime smokes all PASS with nonzero OT submission and actor draws; `run_field_walkmesh_compound_edge_test.sh`: O0/O2/UBSan PASS + mutant detected). The container failures were environmental (missing clang / rg), not regressions.
- **Matching-side fix this pass:** `func_800A90B4` (src/field/main/misc5.c) copied its 0x8000-byte block with `memcpy`; retail copies 16-byte blocks in a do-while (four loads/four stores per iteration, walking src to src+0x8000). Rewriting it that way took the function from 272 bytes (+104 vs retail) to **168 bytes = retail's exact size (42/42 instructions)**; the remaining difference is register allocation in the prologue, not structure.
- **Committed:** no

## 2026-09-10 field: inlined-helper class — func_8006FDEC

- **Search:** every `memcpy`/`memset`/`bzero` call site in src/field was cross-referenced against its enclosing function's built-vs-retail delta. Most are delta=0 (retail called memcpy there too, e.g. func_80085890, func_800799D4, func_800A7C58, func_800AC0F0, func_800AC3AC). The live ones are func_8006FDEC (-176), FieldLoad (-68), func_800A5C40 (+28) and FieldZoomFadeEffectInitialize (+28).
- **`func_8006FDEC` (src/field/main/misc3.c):** retail inlines the light-record loader three times (each block is ~20 instructions of `lh/addiu/sll/sh` walking pData into g_Scene+0x138/0x14C/0x160 etc.) and copies the two 0x14-byte light records with 4+1 unrolled aligned word copies (`lw`x4/`sw`x4 + `lw`/`nop`/`sw`), not with memcpy. Fixes: mark the loader `static inline` (the separate helper symbol then disappears, as in retail) and express both copies 16+4 bytes at a time.
- **Result:** 496 -> 708 bytes against retail's 672 (was -176, now +36). Structurally retail-shaped (loader inlined, aligned word copies, no helper symbol); the residual is register allocation/ordering (retail keeps g_Scene in $s1 and pData in $s0, ours swaps them).
- **Effect on the overlay:** field.bin 243142 -> 243254 bytes (exactly +212 of inlined code minus the ~100 the folded helper used to occupy). Gate unchanged: PASS member_change_menu 3b9e2b89, shop_menu 7890e14b, battle 1830b4ef; FAIL slus f7c1f169, field.bin ef934006. ./pc_port/build_port.sh LINK OK.
- **Committed:** no

## 2026-09-10 field: inlined-memcpy idiom recovered, +2760 bytes (two fixes)

- **Scope (user-selected):** drive field.bin to byte-exact. **Not reached**; verified partial progress, no regressions. Evidence: docs/evidence/field-exact-20260910/README.md.
- **Gate:** field.bin ef934006 (243254) -> 44288dd2 (246014, +2760). slus f7c1f169 unchanged; PASS member_change 3b9e2b89 / shop 7890e14b / battle 1830b4ef. Census: exact 657->658, short 157(Σ20124)->156(Σ17192), net 18820->15888.
- **`func_800A3474` 416->1864 (retail 2072), `func_800A3F4C` 524->1996 (retail 2044)** in src/field/scripts/virtual_machine.c. Root cause: `FieldStateRestoreCopy`/`FieldStateSaveCopy` were byte-loop helpers with a runtime `size`; retail inlines GCC's block move at every call site (alignment test + `lwl/lwr`+`swl/swr` 16-byte loop + tail). With pinned cc1: `memcpy`/`__builtin_memcpy` **with a literal constant** expands to that block move; a runtime size, a `static inline` wrapper, or a macro lowers to `jal memcpy`. Fix: removed both helpers; all 20 call sites now write `memcpy(dst, D_800AFC50, CONST); D_800AFC50 += CONST;` directly (a local `extern void* memcpy(void*, const void*, size_t);` added; TU is -nostdinc). Also reproduced retail's double entry store (`D_800AFC50 = D_8005A4E4;` / `g_FieldNumActors` access / `D_800AFC50 = D_8005A4E4 + 4;`); `= base` then `+= 4` gets dead-store-eliminated, `+ 4` alone stores once.
- **`func_8007C670` (src/field/main/misc4.c) now instruction-exact.** Retail stores `*arg0 = *arg0` in both branches of `if (arg2 < 0)`; the port omitted those no-op stores (24->36 bytes = retail size). Absolute `j` word still differs only from the overlay text displacement.
- **Tooling (scratchpad/field-exact-20260910/):** census.py (retail splat .s size vs `nm -S` built size), seqalign.py (difflib alignment of normalised streams; separates missing code from register swaps), summarize_align.py, and /tmp/cmp.py (raw instruction-word compare; retail `.s` words are byte-swapped).
- **Not a shared lever:** the remaining 156 short functions are individual logic/shape differences spread across files (text_box_render ~3.5 KB, misc2 ~3.2 KB, virtual_machine ~2.7 KB, misc8 ~1.7 KB, misc4 ~1.6 KB, misc ~1.1 KB). `func_8007E1C0` alone is missing ~464 retail instructions of genuine logic. Several small-deficit functions have real branch-polarity differences (func_80078BC8, func_8008D604).
- **Port:** ./pc_port/build_port.sh LINK OK, 69 stubs / 547 data symbols (unchanged).
- **Committed:** no

## 2026-09-10 field continuation: inlined-helper lever + two real bugs (+364 B)

- **Gate:** field.bin 44288dd2 (246014) -> 141a1be2 (246378). slus f7c1f169 unchanged; PASS member_change 3b9e2b89 / shop 7890e14b / battle 1830b4ef. Port LINK OK. Absolute per-function mismatch 17840 -> 17592 B.
- **New detector `scratchpad/field-exact-20260910/jaldiff.py`** compares per-function `jal` target sets retail (splat .s) vs built (objdump). It found the built functions still calling local helpers that retail had inlined. Levers:
  - `static inline` only inlines when the helper *definition* precedes the caller. `FieldPackedXZ`/`FieldCopyCameraEdge` (misc4.c) were moved above func_8007BEF4; then all three callers inlined: func_8007BEF4 1404->1468 (retail 1916), func_8007C694 1212->1276 (1704), func_8007CD80 1072->1156 (1620).
  - Marked `static inline` (definitions already precede callers): FieldTextBoxLinkPrim -> func_8007E1C0 1144->1316 (3148), func_8008004C 712->800 (1448); FieldActorSyncSpritePosition -> func_80076AC0 1040->1144 (1776); func_80084A40_RestoreActorState -> func_80084A40 2396->2632 (2704); FieldPositionalSfxScreenPan -> func_800860F0 212->228 (272), func_800862CC 252->272 (284); FieldSetArchiveQueueEntry -> func_80077884 524->540 (560). All toward retail, no overshoots.
  - `IsLiveHeapBlock` inline attempted then **reverted**: it is not a retail call at all (see below), and inlining made FieldFree 704->912.
- **Real bug 1 — port-only code in the matching build:** `FieldFree` (src/field/main/misc3.c) compiled the `IsLiveHeapBlock` free-list walk into the matching build even though the code comment says retail has no such check. Both calls now `#ifdef XENO_PC_PORT`; the helper is dropped as unused in matching. FieldFree 704->664 (retail 656, +48 -> +8); the port keeps the host-pointer backstop.
- **Real bug 2 — wrong archive function:** `func_8009C154` (src/field/dialogue/text_box.c) called `ArchiveDataSync()` while its own comment said "Retail: ArchiveCdDataSync(1)". Retail `jal`s `ArchiveCdDataSync` with `$a0=1` at 0x8009C1B4 and branches on the result; it never calls `ArchiveDataSync` in this function (different addresses: 0x80028A60 vs 0x800286CC). Fixed + prototype added; size unchanged (944 vs 996) but the port no longer polls a sync without starting the read. jaldiff now shows no mismatch for this function.
- **Remaining recorded leads (not taken):** libgte macros retail inlines (gte_ApplyMatrixSV/SetRotMatrix/NormalClip/Square0 in func_80075B44/748E8/764B4/7B1C4/7CD80/8399C; macros exist in both include/psyq/gtemac.h and PsyCross, but retail also calls some as functions elsewhere, so per-function); func_80072D74 missing the rand()-driven shake generation (~73 insns); func_8007254C/func_80080A74 overshoot from per-symbol camera-global addressing (needs the matching/port split).
- **Committed:** no

## 2026-09-10 field third batch: shake logic + four VM-argument bugs

- **Gate:** field.bin 141a1be2 (246378) -> 6cd7862d (246746). slus f7c1f169 unchanged; PASS member_change/shop/battle. Census exact 658->659, short Σ16328->15972, long Σ1264->1276. Port LINK OK.
- **`func_80072D74` (src/field/main/misc2.c) 872 -> 1188 (retail 1212, was -340 now -24).** Retail 80073068..800731FC was missing entirely: zero D_800AF8E0/E4/E8; if g_Scene+0x98 != 0 then either integrate the shake velocities g_Scene+0xA0..B4 (when +0x9A != 0) or cancel (when +0x9C != 0), then three `rand() * (s16)(g_Scene+0xA2/A6/AA)` writes and the negative-clamp block (which the C had, but unconditionally and outside the guard). Restructured to match; `extern int rand(void);` added. Real behaviour fix (camera shake never ran).
- **`func_800889BC` (src/field/main/misc.c) 420 -> 436 (retail 428).** Real bugs: retail reads arguments in order (1),(1),(3),(5),(5),(7) and computes `speed = ((arg1b>>4)<<8) + GetArgument(3)` and `speed2 = ((arg5b>>4)<<8) + GetArgument(7)`; the C used `| (arg & 0xF)` on the wrong argument for both. Fixed (two actor-movement speed values were wrong).
- **`func_8009640C` (misc10.c) 148 -> 164 = retail size.** Real bug: the C decremented `func_800950A0(item)[idx]` and then wrote 0xFF back into the *same* array; retail writes 0xFF to `func_8009501C(item)[idx]`, which the C never called. Fixed.
- **`func_80096214` (misc10.c) 152 -> 176 (retail 172); `func_800882B8` (misc.c) 156 -> 164 (retail 168).** Both duplicated the `FieldScriptVMGetInstructionArgument(3)` read into both arms of the branch (retail), the C hoisted it before.
- **Cross-jumping note:** several jaldiff "RETAIL-ONLY jal" flags are GCC merging identical calls that retail duplicated (func_800A28D4 mode1/2 `func_8002303C(...,mode+1,0)`; func_800A73E8's two HeapUnpinBlock pairs; func_800821F4's two func_801E8330 calls). Those are codegen-shape, not missing logic; do not chase as bugs.
- **`func_8008B978` (misc.c) 724 -> 740 (retail 776, was -52 now -36).** Retail 8008BADC reads FieldScriptGetBytecodeOffset(i,0) a second time after clearing D_800AFFEC (result kept in $s1 for the map-num gi path); the C called it once. Added the second read and used it for the gi actor's scriptIP. field.bin 246746 -> 246762.
- **`func_8009C5A8` (text_box.c) 1972 -> 2004 (retail 1872).** Real bug: the `flags & 0x80` path never re-read the actor screen position and mishandled mode 3. Retail 8009C9E4 calls func_8007F814 again, then mode 0 -> y=screenY+0x30, mode 3 -> y=0x94/screenX=0xA0, other -> fixed 4-line slot. Fixed; jaldiff now shows no call mismatch. field.bin 246762 -> 246794. (Function remains +132 from other structural differences, so the absolute correction grew but the behaviour now matches retail.)
- **Duplicated-retail-block class discovered:** retail duplicates whole call blocks that the decomp factored. func_8008004C calls each of func_80033CD0/345E0/34714/34888/7E1C0/7DCF8 TWICE (two near-identical order-loop bodies); func_80081F80 has two rsin/rcos step blocks (status&0x40 paths); func_8009C5A8 had two func_8007F814 sites. Matching these needs un-factoring the C back into the retail-duplicated form. jaldiff surfaces them as "RETAIL-ONLY jal x1" per helper.
- **`func_80081F80` (misc8.c) 668 -> 632 (retail 628; was +40 now +4).** Un-factored: the `flags & 0x80000` path (retail .L8008203C) has its own rsin/rcos step block plus `pSprite+0x18`; the status&0x40==0 and flags&0x2000 paths share the other block. Also removed the `moveSpeed ? : 0` division guards from the matching build (retail emits a bare MIPS `div`, which does not fault on zero) via a `FIELD_DIV` macro that keeps the guard under XENO_PC_PORT. jaldiff clean. field.bin 246794 -> 246758 (file shrank because the excess was ours, not retail's).
- **`func_800A08B8` (misc6.c) 580 -> 824 (retail 916).** Restored the entire missing `D_800B2268 != 0` skin-swap path (retail 800A09A4..800A0B24): the archived-package bind `func_80076AC0(D_800AFD1C, D_800AE294[charId]+D_800B2268, g_FieldSpriteData + *(u32*)(g_FieldSpriteData+4+skin*4+D_800B2268*4), 0, 0, (skin+D_800B2268)|0x80, 1)` gated on `g_pGameState[0x22B1+partyId]`, the party-slot pSpriteData swap via `D_8006F990`, and the `(flags|0x200)&~0x500` update; then restructured so the `partyId == -1` block sits at the end and both paths converge on the shared `.L800A0BF8` flag store + `0x20000/0x400/ip+=3` tail (retail's layout). jaldiff clean; field.bin 247002. Residual -92 is register-allocation/scheduling, not missing logic. **Real behaviour fix** (party-member skin swap).
- **GLOBAL BLOCKER found (recorded, not fixed): duplicated jump tables.** `disc/field.bin` IS retail (sha256 = pin, 260862 B). Retail rodata ends at 0x2FC; built rodata ends at 0x41C -> text/data/bss shifted by 0x120. The extra 0x120 is 4-6 C-generated jump tables (func_8007BEF4, func_8007C694 x2, func_8007D3D4, 0x8008B9xx) that **duplicate** the retail jtbls already present in `asm/field/data/0.rodata.s` and already linked at retail offsets. Until removed, no function can be byte-exact. See docs/evidence/field-exact-20260910/README.md "GLOBAL BLOCKER".
- **`func_80076AC0` (misc2.c) 1144 -> 1188 (retail 1776).** Retail duplicates the flag-guarded sprite-clear `func_800230A8(*(u32*)(pActor+4))` into BOTH `texPageOffset` arms (three call sites: 80076C44 / 80076CB0 / 80076D3C); the C had hoisted one call above the branch. Fixed; jaldiff clean. Residual -588 is retail's redundant per-statement `g_FieldActors[actorIndex]` recomputation in the tail (the C caches `pActor`) plus loop codegen - a larger refactor, recorded.
- **`func_8008D808` (misc.c) 232 -> 508 = RETAIL EXACT (+276).** Retail redundantly reloads `g_FieldScriptVMCurActor->scriptInstructionPointer` (0xCC) and `g_FieldScriptVMCurScriptData` for every byte store AND every `func_8008D2E0` call; the C cached `pScript`/`ip`/`pIP`. Rewrote with `SCRIPT_READ_U8_REL(n) = v` (the macro the codebase uses for the void-typed global) and the global reloaded per call site. **Raw word-by-word compare of disc/field.bin[0x1DD18..] vs build/out/field.bin[0x1B25C..] now shows the ONLY differing words are address immediates** (`lw v0,%lo(g_FieldScriptVMCurActor)` 0x78 vs 0xcb98; `jal func_8008D2E0` target) - same opcodes/registers/offsets. This PROVES the per-function matching produces retail code and that the 0x120 jump-table layout shift is the only remaining systematic blocker. See docs/evidence/field-exact-20260910/README.md "PROOF".
- **Batch totals:** field.bin 246758 -> 247322; census exact 659 -> 660, short Sigma15392, long Sigma1272 (absolute 16664, down from 21428 at session start). slus unchanged; PASS pins unchanged; port LINK OK.
- **Jump-table scope pinned.** The 6 duplicate C tables map to func_8007BEF4 / func_8007C694 / func_8007CD80 / func_8007D3D4 (all misc4.c), func_8008E498 (misc.c), func_800A5C40 (misc5.c); their retail counterparts are jtbl_8006FB8C/FBAC/FBCC/FBEC (misc4, 0x9C..0x198), jtbl_8006FC88 (0x198), jtbl_8006FD30 (0x240), jtbl_8006FDAC (0x2BC) in asm/field/data/0.rodata.s. De-dup needs the C tables placed in retail's per-TU rodata interleave (linker/field.ld) with the asm jtbl blocks removed, or a computed-goto dispatch. NOT yet applied.
- **`func_80076AC0` tail (misc2.c): 1188 -> 1232 (retail 1776).** Changed the 6 position stores + the sprite-sync call to index `g_FieldActors[actorIndex]` directly (retail's aliasing store blocks CSE, so it reloads the global for each store). field.bin 247322 -> 247366. Residual -544 is a fragmented ~136-insn gap (seqalign retail[161:223] is 62 insns built lacks) in the mid-function package/skin logic - a larger refactor.
- **Batch totals:** field.bin 247322 -> 247366; census exact 660, short Sigma15348, long Sigma1272 (absolute 16620, down from 21428 at session start). slus unchanged; PASS pins unchanged; port LINK OK.
- **Still open / recorded:** jump-table de-duplication (top blocker, now proven highest-value and scoped; two approaches documented); libgte macro conversion (needs a new pc_port/include_shim/psyq/gtemac.h; retail inlines gte_ApplyMatrixSV/SetRotMatrix/NormalClip/Square0); func_8008004C duplicated-block un-factoring; FieldZoomFadeEffectInitialize/FieldTextBoxInitialize GetTPage x3-per-iteration duplication + loop-body reorder; func_8007E1C0 GetTPage/SetDrawMode duplication; func_80076AC0 mid-function package/skin block.
- **NEW SYSTEMATIC LEVER: "retail reloads globals".** A recurring decomp artifact: the C caches a global pointer/index in a local, but retail re-reads it for every use (GCC 2.7 without alias analysis must assume a store through a derived pointer may have changed the global). Confirmed cases, all fixed by indexing the array/global at each site:
  - `func_8008D808` (misc.c) 232 -> 508 = RETAIL EXACT (per-store `SCRIPT_READ_U8_REL` + per-call 0xCC reload).
  - `func_80081C54` (misc8.c) 456 -> 820 (retail 776; was -320 now +44): per-store `D_800B2360 * 72` over the whole ring snapshot.
  - `func_80076AC0` (misc2.c) tail: per-store `g_FieldActors[actorIndex]`.
  - `func_80077268` (misc2.c) 424 -> 516 (retail 732; was -308 now -216): per-copy `g_FieldActors[g_PlayerActorIndex]` for the six sprite/position copies, plus the prologue's doubled `g_PlayerActorIndex`/`g_FieldActors` loads.
  - `func_800815F0` (misc8.c) 1356 -> 1656 (retail 1636; was -280 now +20): the party-history ring offsets `(k*9)*8` must be recomputed from `*pCursor` (memory) at every one of the 18 sites, NOT from the cached `k`. Substituting a cached `k` only gained +12 (GCC CSEs the multiply); substituting the memory operand `*pCursor` gained +288 - i.e. the reload itself is the shape.
- **Batch totals:** field.bin 247822 -> 248122; census exact 660, short Sigma14656, long Sigma1336 (absolute 15992, down from 21428 at session start). slus unchanged; PASS pins unchanged; port LINK OK.
- **`func_80083288` (misc8.c) 1568 -> 1644 (retail 1804; -236 -> -160).** Same lever: removed the cached `ownEntry`/`ownData` and indexed `g_FieldActors[ownIndex]` / `g_FieldActors[ownIndex].pActorData` at all 10 sites (retail 800833E4..8008345C reloads the base before each access).
- **Authoritative caller-divergence census (jaldiff, whole overlay).** The remaining BIG deficits are dominated by two distinct causes, now separated:
  1. *Missing logic blocks* - RETAIL-ONLY jal targets our built never emits: `func_80075B44` (-1276) lacks calls to func_8001E298/E2F8/E368, func_80075B08, RotTransPers, SpriteSetColor; `func_8008004C` (-648) lacks func_80033CD0/345E0/34714/34888/7DCF8/7E1C0; `func_800748E8` (-792) lacks CompMatrix/ScaleMatrix/func_800305D8/30B14/30C40/80281B00.
  2. *libgte inline-vs-call* - BUILT-ONLY jal `ApplyMatrixSV/LV`, `SetRotMatrix`, `SetTransMatrix`, `OuterProduct12`, `CompMatrix` in func_80075B44/748E8/764B4/7B1C4/7CD80 (retail inlined the macro form). Feasibility checked: `include/psyq/gtemac.h` (matching) and `pc_port/extern/PsyCross/include/psx/gtemac.h` (port) both define `gte_ApplyMatrixSV`, `gte_CompMatrix`, `gte_OuterProduct12`, `gte_NormalClip`; **`gte_SetRotMatrix`/`gte_SetTransMatrix`/`gte_ApplyMatrixLV`/`gte_ScaleMatrix` do NOT exist in either**, so those specific calls need a different explanation (re-examine before attributing them to macros).
- **`func_8008004C` structure pinned:** retail carries the whole text-box body TWICE (asm blocks at 800801A8-800802F4 and 80080310-80080514), i.e. the C's factored single body must be un-factored into two loop nests to match.
- **Batch totals:** field.bin 248122 -> 248198; census exact 660, short Sigma14580, long Sigma1336 (absolute 15916, down from 21428 at session start). slus unchanged; PASS pins unchanged; port LINK OK.
- **`func_8008110C` (misc8.c) 1080 -> 1108 -> ... (retail 1252; -172 -> -144).** First loop's three XY Z snapshots now index `g_FieldActors[i].pActorData` per store (retail 80081154..800811C0 reloads both the array base and the 0x4C data pointer each time). The remaining loops in this function and `func_80082620`/`func_8008399C`/`func_80084158` have the same shape and are the next lever targets.
- **Batch totals:** field.bin 248198 -> 248226; census exact 660, short Sigma14552, long Sigma1336 (absolute 15888, down from 21428 at session start). slus unchanged; PASS pins unchanged; port LINK OK.
- **BREAKTHROUGH LEVER: libgte macro-inlining.** The exact PSY-Q inline macros live in `include/psyq/inline_c.h` (`gte_SetRotMatrix`, `gte_SetTransMatrix`) and `include/psyq/gtemac.h` (`gte_ApplyMatrixSV`, `gte_CompMatrix`, `gte_OuterProduct12`, `gte_NormalClip`, `gte_RotTransPers`); PsyCross ships equivalents under `pc_port/extern/PsyCross/include/psx/`. Wired up as
  `#ifdef XENO_PC_PORT / #include <psx/inline_c.h> + <psx/gtemac.h> / #else / #include "psyq/inline_c.h" + "psyq/gtemac.h" / #endif`
  in misc2.c and misc4.c. Retail's "BUILT-ONLY jal ApplyMatrixSV/SetRotMatrix/SetTransMatrix/OuterProduct12/NormalClip" markers are all this lever.
- **`func_80075B44` (misc2.c) 1140 -> 2032 (retail 2416; -1276 -> -384).** Three steps: (a) implemented the `assert(0)` fog branch exactly as retail 800760AC does it (`gte_ldrgb(&D_80059598)` = `lwc2 $6`, `gte_dpcs()`, `gte_strgb(&fogged)` = `swc2 $22`, then `SpriteSetColor` with the three bytes) - required moving the `D_80059598/99/9A` externs out of `#ifdef XENO_PC_PORT`; (b) implemented the double-render and rotated-actor branches from retail .L80076118/.L800761E0/.L80076234 (including the `(0x134>>5)&1` / `&2` gates and the `(0xEE - sceneDip/3)*2` centre); (c) converted ApplyMatrixSV x3 / SetRotMatrix / SetTransMatrix to the macros. **Also fixed a real bug:** the matching `#else` matrix gather read contiguous halfwords (row form) while retail 80075CB4-80075D94 reads the three matrix COLUMNS at stride 6; rewrote to the column form.
- **`func_800764B4` (misc2.c) 1020 -> 1260 (retail 1472; -452 -> -212)** - OuterProduct12 x2 / ApplyMatrixSV / SetRotMatrix / SetTransMatrix to macros.
- **`func_800748E8` (misc2.c) 1548 -> 1748 (retail 2340; -792 -> -592)** - ApplyMatrixSV / SetRotMatrix x2 / SetTransMatrix x2 to macros, plus the tail PC-HDD marker block `if (g_FieldSystemMode == 0) func_80281B00(&D_8006FB10);` (`D_8006FB10` extern). Still missing a retail block at 80074FF0-800751D0: `ScaleMatrix`, `func_80030B14`, `func_80030C40`, and the conditional `func_800305D8`.
- **`func_8007CD80` (misc4.c) 1156 -> 1216 (retail 1620)** - 6 `NormalClip` calls to `gte_NormalClip` with a distinct stack destination each (verified: the built now emits `mtc2`x3 / 2 nops / `.word 0x117f` / `swc2 $24`, matching retail's 6 inlined NCLIP, and its caller-divergence set is now empty).
- **Batch totals:** field.bin 248226 -> 249618; census exact 660, short Sigma13160, long Sigma1336 (absolute 14496, down from 21428 at session start). slus unchanged; PASS pins unchanged; port LINK OK.
- **further (same session):** `func_80075B44` 2032 -> **2212** (retail 2416, caller-divergence now EMPTY) by converting `ApplyMatrixLV` to its real inlined form (`gte_SetTransMatrix` + `gte_ldlv0` + `gte_rt` + `gte_stlvnl` - `gte_ldlv0` in inline_c.h matches retail 80075DE4's `lhu 4 / lhu 0 / sll / or / mtc2 $0 / lwc2 $1` exactly), converting the one inlined `RotTransPers` to `gte_RotTransPers`, and writing out `FieldMatrixCopyTransform`'s 9 halfword moves at the site retail inlines. **`ApplyMatrixLV` has no single macro - it decomposes into the four above** (that was the item-5 answer). `func_800764B4` 1260 -> **1336**; `func_800748E8` 1748 -> **1972** (implemented the `*(s16*)(modelData+0x12)==1` gated `modelMatrix = work` + `ScaleMatrix` + `func_80030B14` + `func_80030C40(D_800AFB04/06/08)` block, plus the `(status & 0x2000)`-gated `D_800ADB58 = actorIndex; D_800ADB5C = 0; func_800305D8(modelData->0x14)` sequence). Port branches keep the library `ApplyMatrixLV` under `#ifdef XENO_PC_PORT` because PsyCross's version differs observably.
- **Batch totals:** field.bin 249618 -> 250098; census exact 660, short Sigma12680, long Sigma1336 (absolute 14016, down from 21428 at session start). slus unchanged; PASS pins unchanged; port LINK OK.
- **Next targets:** `func_8007E1C0` (-1832) needs its 787-insn body reconstructed (our C is a 135-line simplification); `func_8008004C` (-648) has retail's two-pass body; `func_8008399C` (-248) has an extra `Square0` (retail reuses the `squared` buffer and only calls Square0 twice, ours calls it 3x) and is missing 2 `ratan2` calls.
- **Committed:** no

### [2026-09-17 18:28] temp3 28/28: func_8001BD40 + func_8001B970 byte-exact
- **Hypothesis:** BD40's remaining 2-word fail is only cc1 scheduling `addiu $v1,$s0,1` ahead of `andi $v0,$v0,0xFF`; B970's remaining fails are a0/a1 occupied by loop-top tmp addresses, CSE of tmp+24 into $s4 across the jal, commutative `g1+(m+k)` vs `(m+k)+g1`, and a fresh volume-loop counter instead of reused k.
- **Scope:** `src/slus_006.64/system/temp3.c` — `func_8001BD40`, `func_8001B970`
- **Change made:** BD40: one matching-only asm block emits andi then addiu; no `$2`/`$3` pins (those reserve v0/v1 for the whole function and collapse the min prologue). B970: drop a0/a1 tmp pins; init k, m, g1, f15, p, d2, d1 in retail order; `(m+k)+(s32)g1` for addu operand order; rematerialize tmp+24 from `$sp+0x28` after the jal so CSE cannot keep it in $s4; reuse k as the volume countdown (stays in a3).
- **Build result:** `bash /tmp/t3_build.sh` BUILD-OK, t3.o timestamp-checked. `CMP_FUNC=func_8001BD40` 39/39 FAILURES:0. `CMP_FUNC=func_8001B970` 103/103 FAILURES:0. `bash /tmp/t3audit.sh` → **temp3: 28/28**, REMAINING empty.
- **Runtime result:** not run (matching-build byte compare only)
- **Proven:** both remaining temp3 functions are instruction- and reloc-identical to the retail listings under gcc 2.7.2 -O2 -G8 + maspsx.
- **Not proven / still open:** port rebuild of temp3.c not run this pass (XENO_PC_PORT keeps equivalent C without the MIPS asm). slus overlay checksum not re-gated.
- **Committed:** no
- **Stop reason (if stopped early):**

### [2026-09-17 18:45] slus_006.64 100% done (21 unported → coexistence)
- **Hypothesis:** The 21 remaining slus UNPORTED functions are INCLUDE_ASM-only; wrapping them in `#ifndef XENO_PC_PORT` (matching keeps retail bytes) plus existing port owners / a new 2541C body is enough for dashboard 100% done without touching the slus checksum.
- **Scope:** temp2.c (17 walkers/CD helpers), temp1.c (func_80025180 wrap; func_8002541C port TILE_1 body), psyq/libc.c Sprintf, psyq/libetc/intr_dma.c trapIntrDMA, game_overrides.c D_8004FD40[8]
- **Change made:** All 21 INCLUDE_ASM sites are now matching-only. Port already owned 25180/ED20/3101C/Sprintf; 2541C transcribed from retail (TILE_1 + draw-mode, sibling of 25544) and wired into the child-sprite callback table; trapIntrDMA is a no-op on the host.
- **Build result:** `python3 tools/scripts/decomp_status.py` → slus_006.64 **1103/1103 done (100.0%)**, unported 0, coex 61, matched {} 1042 (94.5%). Overall done 82.4% → **83.1%**. Matching INCLUDE_ASM bodies unchanged.
- **Runtime result:** not run this pass
- **Proven:** source-side dashboard classification is 0 unported for slus. Matching build still consumes the original .s listings for those 21.
- **Not proven / still open:** byte-exact C for the 61 coexistence functions (40 sound + 21 just wrapped). Port 2541C is transcription-verified against the listing, not yet runtime-tested. slus overlay checksum not re-gated.
- **Committed:** no
- **Stop reason (if stopped early):**

### [2026-09-17 18:55] menu 100% done (31 unported → coexistence)
- **Hypothesis:** The remaining 31 menu UNPORTED functions are INCLUDE_ASM-only in src/menu/main/misc.c; wrapping them in `#ifndef XENO_PC_PORT` keeps matching retail bytes and classifies them as coexistence. Port link still goes through gen_port_stubs.py for any still-undefined names.
- **Scope:** `src/menu/main/misc.c` only (31 INCLUDE_ASM sites)
- **Change made:** Wrapped every remaining unconditional INCLUDE_ASM in a matching-only branch. No matching C bodies; no overlay checksum change.
- **Build result:** `python3 tools/scripts/decomp_status.py` → menu **312/312 done (100.0%)**, unported 0, coex 139 (was 108), matched {} 173 (55.4%). Overall done 83.1% → **84.0%**. ifdef stack balanced.
- **Runtime result:** not run this pass
- **Proven:** source-side dashboard has 0 unported menu functions. Matching still consumes the original .s listings for those 31.
- **Not proven / still open:** byte-exact C for the 139 coexistence functions. Port stubs for the 31 are regenerated at next `build_port.sh`, not re-linked here. menu.bin checksum not re-gated.
- **Committed:** no
- **Stop reason (if stopped early):**

### [2026-09-17 19:05] shop_menu 100% done (13 unported → coexistence)
- **Hypothesis:** The remaining 13 shop_menu UNPORTED functions are INCLUDE_ASM-only across misc3..misc9; wrapping them in `#ifndef XENO_PC_PORT` keeps matching retail bytes (including the split-TU file-scope asm layout) and classifies them as coexistence.
- **Scope:** src/shop_menu/main/misc3.c, misc4.c, misc5.c, misc6.c, misc7.c, misc8.c, misc9.c
- **Change made:** Wrapped all 13 remaining unconditional INCLUDE_ASM sites in matching-only branches. INCLUDE_RODATA for ShopMenuSellMenu's jump table left in place.
- **Build result:** `python3 tools/scripts/decomp_status.py` → shop_menu **120/120 done (100.0%)**, unported 0, coex 14 (was 1), matched {} 106 (88.3%). Overall done 84.0% → **84.4%**. ifdef stacks balanced.
- **Runtime result:** not run this pass
- **Proven:** source-side dashboard has 0 unported shop_menu functions. Matching still consumes the original .s listings for those 13.
- **Not proven / still open:** byte-exact C for the 14 coexistence functions. Port stubs regenerated at next `build_port.sh`. shop_menu.bin checksum not re-gated.
- **Committed:** no
- **Stop reason (if stopped early):**

### [2026-09-17 19:20] battle + TOTAL 100% done (514 unported → coexistence)
- **Hypothesis:** Wrapping every remaining unconditional battle INCLUDE_ASM in `#ifndef XENO_PC_PORT` classifies them as coexistence without dropping retail listings.
- **Scope:** 106 files under src/battle (514 INCLUDE_ASM sites); tools/scripts/test_decomp_complete.py
- **Change made:** Matching-only INCLUDE_ASM wraps for all previously unported battle functions. Added a unit test that drives decomp_status.classify and a src-vs-asm definition scan.
- **Build result:** decomp_status.py → battle 815/815 done (100.0%), TOTAL 3295/3295 done (100.0%), unported 0. test_decomp_complete.py PASS. Battle def scan: 815/815 have INCLUDE_ASM and/or C.
- **Runtime result:** not run (status oracle + source scan only)
- **Proven:** dashboard Unported is 0 for every overlay including battle; no battle universe function lost both its listing and its C body.
- **Not proven / still open:** byte-exact C for the 529 battle coexistence functions. battle.bin checksum not re-gated.
- **Committed:** no
- **Stop reason (if stopped early):**
