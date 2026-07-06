# Active Handoff

## Current Verified State

- Native PC field repro builds and links cleanly inside `xenogears-dev`.
- Branch state at final verification: `ai-private-main` was 13 commits ahead of origin `b7d9ac3`.
  - **Note:** `git` is NOT on PATH inside the distrobox container. Run git commands on the HOST at `/home/blizz/Projects/xenogears-decomp`.
- Working convention: keep this canonical handoff updated as the project workbench after each meaningful proof, fix, visual confirmation, or checkpoint commit.
- Latest checkpoint commit: `f5aa055 Checkpoint recovered field pipeline; visual mismatch unresolved`.
- Follow-up workbench/cleanup commits:
  - `9c680b9 Update workbench after field recovery checkpoint`
  - `9f6381d Make root handoff point to canonical workbench`
  - `b4290ec Ignore local diagnostic artifacts`
  - `cb0afb6 Update workbench after diagnostic ignore cleanup`
  - `0d3ffe0 Mark PSX cpp tool executable`
  - `78e1b5d Update workbench after tool permission cleanup`
  - `709f922 Record final field recovery verification`
  - `2627286 Implement field VM actor direction handler`
- Working tree is clean after the final tool-permission cleanup. `tools/gcc-2.7.2-psx/cpp` is intentionally executable (`100755`).
- Ignored local diagnostic artifacts: `"Xenogears (PC port).log"`, `captures/`, and `xenogears-decomp-ai`.
- Final verification after cleanup:
  - Build command completed with `LINK OK -> pc_port/build_native/xeno-port`.
  - `work_list_port.c ok` during port-only source compilation.
  - Kernel0 field run completed the 45s timeout path without crash.
  - Latest final verification log: `captures/render_diag/final_verify_kernel0_20260705_100734.log`.
  - Work-list/sprite-frame stubs remain absent.
  - Actor sprite primitive linking remains restored: `frames=2 linked=2`, `frames=6 linked=6`, `frames=8 linked=8`, `frames=3 linked=3`.
  - Only observed `[stub]` line in the final verification log is `func_8009AD6C`.
- Next-phase stub cleanup:
  - Implemented `func_8009AD6C` in `src/field/main/misc7.c`.
  - Build still links cleanly; generated function stub count dropped from `260` to `259`.
  - Latest verification log: `captures/render_diag/func8009AD6C_verify_20260705_101605.log`.
  - `grep`/`rg` result: zero `[stub]` lines in that run.
  - Actor sprite primitive linking remains intact (`frames=2/6/8/3`, `done linked=...`).
- Menu-route probe (`XENO_KERNEL_SEL=4`) is **not** a normal missing C-function proof:
  - Latest probe log: `captures/render_diag/menu_sel4_probe_20260705_101937.log`.
  - It hits exactly one stub: `[stub] func_801C62A8`.
  - `func_801C62A8` is an overlay entry address expected at `0x801C62A8` after a menu overlay has been loaded to `0x801C5000`, not a standalone source function found in the static tree.
  - GDB context log: `captures/render_diag/menu_sel4_func801C62A8_context_bytes_20260705_102058.log`.
  - At the stub hit: `D_80059460=0`, `g_MenuDebugEnabled=0`, `D_80059171=0`.
  - Stack: `func_801C62A8 -> MenuExecute -> MenuMain -> MainLoop -> KernelMenuMain -> MainLoop -> main`.
  - `MenuMain()` documents that callers are expected to have loaded the correct menu overlay before entry. `MenuExecute()` only loads that overlay itself when `g_MenuDebugEnabled != 0`.
  - Original `g_MainGameStates[5]` has `hasOverlay=0`, matching `PcPort_InitGameStates()`, so this is not evidence that the port state table is wrong.
  - Do **not** implement a fake `func_801C62A8`; the direct kernel-menu menu route needs an overlay-aware harness or a field/menu caller path that performs the expected overlay load.
- Extended Kernel0 field probe:
  - Log: `captures/render_diag/kernel0_extended_stub_probe_20260705_102300.log`.
  - Command used `XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 timeout -s KILL 90 ./pc_port/build_native/xeno-port`.
  - Result: `RUN_RC=124` timeout success, zero `[stub]` lines.
  - The log is only 79 lines because the current diagnostic printfs are front-loaded/capped and the timeout appended `RUN_RC=124` to a final partial line; treat this as a stability/no-new-stub proof, not as a deep frame trace.
- Field diagnostics are now opt-in:
  - Existing `[field-diag]` printfs in `src/slus_006.64/system/rendering.c` and `src/field/main/misc2.c` are gated by `XENO_FIELD_DIAG`.
  - Default verification log: `captures/render_diag/field_diag_default_quiet_20260705_103544.log` (`RUN_RC=124`, zero `[field-diag]`, zero `[stub]`).
  - Opt-in verification log: `captures/render_diag/field_diag_optin_20260705_103624.log` (`RUN_RC=124`, 59 `[field-diag]` lines, zero `[stub]`).
  - This cleanup does not alter render logic, primitive linking, fade logic, or the remaining assert-only rare branches in `func_80075B44`.
- `func_80075B44` rare-branch asm audit:
  - Source asserts are at `src/field/main/misc2.c:1821`, `:1915`, `:1923`, and `:1950`.
  - Original asm is `asm/field/matchings/main/misc2/func_80075B44.s`.
  - `actorFlags4 & 0x2000` branch (`.L80076300`) updates a special actor/global work structure rooted at `D_801E8670`; it is real behavior, not a dummy branch.
  - Far-color branch (`D_800B2357 == 0 && D_800B218E != 0`, `.L800760AC`) computes a GTE color from `D_80059598` and calls `SpriteSetColor`.
  - Double-render branch (`((actorData->E8 + 0x22) & 0xFFFF) < 2`, `.L80076118`) draws two sprite passes with different colors/masks and OT offsets.
  - Rotated/special actor draw branch (`actorData->134 & 0x60`, `.L80076234`) calls `func_8001E2F8` and/or `func_8001E368` depending on subflags.
  - Bounded gdb attempts:
    - `captures/render_diag/func80075B44_assert_branch_probe_20260705_103942.log`
    - `captures/render_diag/func80075B44_assert_branch_probe2_20260705_104045.log`
    - `captures/render_diag/func80075B44_assert_branch_probe3_20260705_104148.log`
  - The first two gdb runs timed out with `RUN_RC=124` and did not stop at the branch line breakpoints, but the `commands` blocks were not attached cleanly in batch mode; treat them only as "no breakpoint stop observed", not as a clean hit count.
  - The third process-substitution attempt failed before running (`/dev/fd` invalid in this gdb/container setup).
- Clean `func_80075B44` rare-branch reachability proof:
  - Used one plain break-and-stop gdb run per branch. No gdb `commands` blocks, no source edits.
  - `captures/render_diag/func80075B44_special_0x2000_breakstop_20260705_104428.log`: breakpoint set at `misc2.c:1821`, `RUN_RC=124`, `RESULT special_0x2000 NO_HIT_OBSERVED`.
  - `captures/render_diag/func80075B44_far_color_breakstop_20260705_104513.log`: breakpoint set at `misc2.c:1915`, `RUN_RC=124`, `RESULT far_color NO_HIT_OBSERVED`.
  - `captures/render_diag/func80075B44_double_render_breakstop_20260705_104558.log`: breakpoint set at `misc2.c:1923`, `RUN_RC=124`, `RESULT double_render NO_HIT_OBSERVED`.
  - `captures/render_diag/func80075B44_rotated_actor_breakstop_20260705_104644.log`: breakpoint set at `misc2.c:1950`, `RUN_RC=124`, `RESULT rotated_actor NO_HIT_OBSERVED`.
  - Conclusion: the recovered Kernel0 field route does not reach the four currently asserted `func_80075B44` rare branches during the bounded proof window. They remain real missing behavior for broader field coverage, but they are not current Kernel0 recovery blockers.
- Field-map selector harness:
  - Added `XENO_FIELD_MAP=<n>` for `XENO_FIELD_TEST` mode in `pc_port/src/port_main.c`.
  - It sets original field selector `D_8006F94E`, which `FieldMain()` copies into `g_GameSceneMapNum`.
  - Default route still works after selector addition: `captures/render_diag/field_map_default_after_selector_20260705_105127.log` (`RUN_RC=124`).
  - Field 1 probe: `captures/render_diag/field_map1_probe_20260705_105207.log` aborts quickly with `*** stack smashing detected ***`, `RUN_RC=134`.
  - Field 1 backtrace: `captures/render_diag/field_map1_stack_smash_bt_20260705_105217.log`.
  - Backtrace root: `__stack_chk_fail -> func_80080A74(actorIndex=0) at src/field/main/misc8.c:247 -> func_80080F44 -> FieldLoad`.
  - Suspected cause: `func_80080A74` declares `s16 stateBuf[6]`, but original asm uses stack scratch from `sp+0x18` and advances one cursor by `0x10` bytes per loop plus another by `0x8` bytes, leaving far more than 12 bytes available before saved registers at `sp+0x80`. Field 1 likely has `D_800AFB54 > 1`, causing the current C buffer to overflow.
- Kernel0 field test run confirmed stable (no crash, `RUN_RC=124` timeout success):
  - `XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 timeout 45 build_native/xeno-port`
  - Latest audit run reached frame 7 without crash.
  - `D_800ADC18` reaches `0` at frame 4.
  - `primSubmits` becomes nonzero at frame 4 (`primSubmits=2`) and remains nonzero afterward.
- 30-second kernel1 timeout run confirmed immediate abort:
  - `XENO_FIELD_TEST=1 XENO_KERNEL_SEL=1 timeout -s KILL 30 build_native/xeno-port`
  - Exactly 1 stub: `func_8001B6C4`, then nothing else — battle state main function is stubbed.
- Build-generated function stub count is `259` after supplying the work-list/sprite-frame functions from a port-only file and implementing `func_8009AD6C`.
- Field overlay loads and field main loop runs.
- Actor sprite data reaches `27/27` after VM/post-VM setup.
- Actor draw path runs, and `func_8001E3D8` links real `code=2d` actor `POLY_FT4` packets into the OT in early-frame logs.
- `DrawOTag` runs (called once per frame).
- `FieldAddPrimitives` reports `primSubmits=0` until `D_800ADC18` clears, then `primSubmits=2`:
  - Primitive submission via `FieldAddPrimitives` is gated behind `if (D_800ADC18 == 0)` in `func_8007554C` (`misc2.c:1508`).
  - `D_800ADC18` is a field transition/fade-in counter that starts at 4 and decrements by 1 per frame.
  - **D_800ADC18 lifecycle (full source trace)**:
    - INIT: Set to 4 at `misc3.c:299` during field-load bulk initialization (~100+ vars reset across lines 240–370).
    - DECREMENT: Once per frame at `misc4.c:21–22` inside `func_80078B5C()`.
    - EFFECT while ≠0: `FieldMathUpdateAngle()` skips interpolation (`misc2.c:716`), `FieldAddPrimitives()` not called (`misc2.c:1508`), multiple other render paths gated.
  - Current 45s audit run reached the gate clear: counter goes 4→3→2→1→0 across frames 0–4.
  - Actor packets are still drawn via direct OT-linking via `func_8001E3D8`, independent of `FieldAddPrimitives`.
- Primitive submission functions (`AddPrim`, `AddPrims`, `CatPrim`, `DrawPrim`) are **NOT stubbed** — provided by Psy-X (compiled implementations).
- Visual recovery is **confirmed by the user** for the earlier field-view sprite/fade behavior: the sprite is visible again and slowly zooms in during the field view.
- Recovery cause: the current build was skipping `src/slus_006.64/system/work_list.c`, so the runtime fell back to stubs for `func_8001D298`, `WorkListsReset`, `func_8001D2B0`, `TimerWorkListUpdate`, `func_8001D468`, and `WorkListUpdate`. A new port-only file, `pc_port/src/work_list_port.c`, now supplies those six functions without touching the skipped decomp TU.
- Latest recovery log: `captures/render_diag/worklist_port_recovery_20260705_095407.log`.
  - No work-list/sprite-frame stub lines appear.
  - `func_8001E3D8` again reports nonzero frame counts and linked actor sprite primitives: `frames=2 linked=2`, `frames=6 linked=6`, `frames=8 linked=8`, `frames=3 linked=3`.
  - User visually confirmed: "yes its back".

### XENO_KERNEL_SEL Routing Mechanism

- `psyq_compat.c:318–340` — `PcPort_ForcedKernelSelect()`:
  - On first call, reads `XENO_KERNEL_SEL` env var (default -1 = disabled).
  - Also reads `XENO_KERNEL_DELAY` (default 60 frames).
  - When `g_KernelMenuIsRunning` and frame count reaches delay, sets `g_KernelMenuCurChoice = sel` and ORs `g_C1ButtonStateReleased |= 0x20` (CTRL_BTN_CIRCLE).
- `psyq_compat.c:343–375` — `Vsync(int mode)` override calls `PcPort_ForcedKernelSelect()` every frame.
- `game_overrides.c:247–277` — `PcPort_InitGameStates()` populates `g_MainGameStates[7]` table:
  - `[0]` = `KernelMenuMain` (boot state)
  - `[1]` = `FieldMain` (field state; selected when `XENO_KERNEL_SEL=0`)
  - `[2]` = `func_8001B6C4` (battle state; selected when `XENO_KERNEL_SEL=1`) — **STUBBED**
  - `[5]` = `MenuMain` (menu state; selected when `XENO_KERNEL_SEL=4`)
  - `[3]`, `[4]`, and `[6]` are currently left NULL in the port override table, while the original table points them at overlay entry addresses (`0x80070CFC`, `0x80088E90`, `0x800737EC`) with `hasOverlay=1`.
- KernelMenu option→state mapping: 0=Field, 1=Battle, 2=Worldmap, 3=Battling, 4=Menu, 5=Movie.

### Kernel1 Root Cause Chain (func_8001B6C4 stub)

1. `psyq_compat.c:327` — `getenv("XENO_KERNEL_SEL")` → `sel=1` (Battle)
2. `psyq_compat.c:336–337` — Sets `g_KernelMenuCurChoice=1`, presses Circle
3. KernelMenu auto-selects option 1 → `ChangeGameState(1)`
4. `game_overrides.c:260` — `g_MainGameStates[2].pFnMain = func_8001B6C4`
5. `temp3.c:301` — `INCLUDE_ASM` (nonmatching MIPS assembly — function exists only as raw .s)
6. `stubs.c:479` — Stub prints `[stub] func_8001B6C4`, returns 0
7. State 2 aborts immediately → no field rendering at all (only 2 log lines total)

### Screen Coordinate Anomaly

- Frame 0 (OT1, `useOT2=0`): screen addresses like `0x7ffdfe4a0093`, `0x7ffd00700088` — high 16 bits = `0x7ffd`
- Frame 1 (OT2, `useOT2=1`): screen addresses like `0x25fe4a0093`, `0x2500700088` — high byte = `0x25`
- Frame 2 (OT1, `useOT2=0`): back to `0x7ffd...`
- Lower 32 bits appear consistent — only the high portion differs, suggesting a pointer/address formation issue in OT switching or screen coordinate calculation.

## Last Changes

- Fixed `FieldScene` host layout drift in `include/field/main.h`.
  - Changed `FieldScene.unk7C` from `u_long` to `u_int`.
  - Reason: PS1 `u_long` is 4 bytes, but host `u_long` is 8 bytes.
  - Before this, `g_Scene.worldToScreenMatrix` was at host `+0xDC` while original code passes raw `g_Scene+0xD4` to `func_80024FF4`.
  - After the fix, gdb confirmed `&g_Scene.worldToScreenMatrix == g_Scene+0xD4`.
- Fixed missing original scene projection defaults in `func_8007254C` in `src/field/main/misc2.c`.
  - Original asm writes through `g_CamInterpolation`-relative raw slots:
    - `+0x74 = 0x200`
    - `+0x78 = 0x1e`
    - `+0x7a = 0x1000`
  - These correspond to:
    - `g_Scene+0x68` / `sceneScrZ = 0x200`
    - `g_Scene+0x6C` / `sceneDIP = 0x1e`
    - `g_Scene+0x6E` / `sceneScale = 0x1000`
  - The C port had skipped these while replacing the PSX-relative camera/scene writes with named globals.
- Existing durable PsyCross patching in `pc_port/build_port.sh` remains in place:
  - full-size `TILE` packets use len `3`
  - `DR_MODE`/draw-env processing consumes the full declared packet length
  - raw field OT clearing uses the PC-port 4-byte raw clear while `FieldDisplay` host `ot3` clear is preserved

## Runtime Evidence

- Before the `FieldScene` layout fix:
  - `func_80024FF4((MATRIX*)(g_Scene+0xD4))` copied 8 bytes before the real host matrix.
  - `D_8004FBB8` got bogus translation such as `[53233, 0, 0]`.
  - Actor packet samples included saturated/offscreen coordinates such as `x0=1023`.
- After the `FieldScene` layout fix but before `sceneScrZ` init:
  - first actor sprite base matrix was sane, but GTE projection had `C2_H=0` and `g_Scene.sceneScrZ=0`.
  - first quad input vertices were nonzero, but projected output collapsed to center: `(160,112)` for all vertices.
- After the `func_8007254C` projection defaults fix:
  - gdb at `RotTransPers4` showed `C2_H=512`, `g_Scene.sceneScrZ=512`, `OFX=160`, `OFY=112`.
  - first quad input vertices:
    - `(-144,-352,0)`, `(32,-352,0)`, `(32,-224,0)`, `(-144,-224,0)`
  - sample linked actor packets are now plausible on-screen coordinates:
    - `xy0=(157,95)`, `xy0=(157,97)`, `xy0=(157,102)`, `xy0=(160,96)`
  - filtered repro remains stable: `RUN_RC=124`.
- PsyCross OT traversal reaches actor packets:
  - conditional gdb breakpoint on `ParsePrimitive` with `polyTag->code == 0x2d` hits during `DrawOTag`.
  - sample parsed actor packet:
    - `len=9`, `code=2d`, `xy0=(160,94)`, `xy1=(159,94)`, `xy2=(160,96)`, `xy3=(159,96)`, `tpage=0016`, `clut=7914`
  - `ParsePrimitive` returns `9`, matching `POLY_FT4`.

## Current Frontier

- **Kernel0 field test confirmed** (`XENO_KERNEL_SEL=0`): latest audit run reached frame 7 and timed out cleanly (`RUN_RC=124`) without crash.
- **Kernel0 extended field probe confirmed**: 90s bounded run timed out cleanly (`RUN_RC=124`) with zero `[stub]` lines. No new live field-route function target surfaced.
- **Field diagnostics are quiet by default**: set `XENO_FIELD_DIAG=1` to re-enable the current bounded `[field-diag]` prints.
- **`func_80075B44` rare branches are mapped but not implemented**: asm confirms the asserted branches are real behavior. Clean break-and-stop gdb probes did not observe any of the four branches in the recovered Kernel0 field route.
- **`XENO_FIELD_MAP` selector added for coverage expansion**: default map 0 remains stable; map 1 exposes a stack-smash in `func_80080A74` during actor init.
- **Kernel4 direct menu test is an invalid/incomplete direct route for now** (`XENO_KERNEL_SEL=4`): it enters `MenuMain()` without a menu overlay loaded and therefore reaches `func_801C62A8` as a generated stub. This should be treated as a harness/overlay-loading problem, not as a real source function to implement.
- **`D_800ADC18` gate now observed clearing**: field-transition counter starts at 4 (`misc3.c:299`), decrements once per frame (`misc4.c:21-22`), and reaches 0 at frame 4 in the 45s audit run.
- `FieldAddPrimitives` submission is observed after the gate clears: `primSubmits=2` at frame 4 and later.
- Actor packets ARE created/OT-linked via `func_8001E3D8` independently of the `D_800ADC18` gate — so actor rendering is not blocked by the gate, only primitive submission is.
- PsyCross `ParsePrimitive` hits actor `code=0x2d` `POLY_FT4` during `DrawOTag`.
- **Screen coordinate anomaly unresolved**: OT1 frames have high 16 bits = `0x7ffd`, OT2 frame has high byte = `0x25`. Lower 32 bits consistent across both. Possible OT-switching or screen-coordinate address-formation issue.
- Visual correctness for the recovered field-view sprite/fade milestone is confirmed by the user after restoring the work-list/sprite-frame path through `pc_port/src/work_list_port.c`.
- The earlier `func_8001E3D8` concern is cleared: gdb proved it is called after `D_800ADC18 == 0`, and the frame-0-only filtered log was capped/misleading.
- Remaining frontier: preserve this recovered state before starting new diagnostics or implementation work.

## July 5 Map1 Expansion Checkpoint

- User visually confirmed the default map0 sprite/fade/slow-zoom behavior is back. Treat this as the protected visual baseline.
- Build continues to pass via:
  `distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp && ./pc_port/build_port.sh'`
- After each map1 expansion edit, default map0 was rerun with:
  `XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 timeout -s KILL 35 ./pc_port/build_native/xeno-port`
  and survived to timeout (`RUN_RC=137`).
- Map1 has advanced beyond the earlier blockers:
  - `func_80080A74` stack scratch overwrite.
  - `func_8009E574` null `pSpriteData`.
  - missing model primitive descriptors for prim5v0, prim13v0, prim5v2, prim4v0, prim4v2, prim12v2, prim12v0, and prim8v0.
  - `func_800748E8` actorIndex/D_800ADBFC assertion for model-only slots.
  - `func_80084158` / `func_8008399C` source assertions for observed actor interaction branches.
- Latest verified map1 run:
  `captures/render_diag/field_map1_after_prim8v0_20260705_112439.log`
  - `RUN_RC=134`
  - Current stop:
    `src/field/main/misc2.c:2005: func_800764B4: Assertion '0 && "func_800764B4 active actor quad path is not migrated"' failed.`
- This is a failsafe, not another small descriptor entry. The original asm for `func_800764B4` is a full active-actor quad path: matrix/normal setup, actor scale, `RotAverage4`, and OT insertion. Do not replace the assertion with a blind skip and do not make a broad rendering migration without an explicit plan.
- Recommended next single step:
  - Audit `func_800764B4` against `asm/field/matchings/main/misc2/func_800764B4.s` and decide whether to implement the exact active actor quad path or defer map1 until a smaller visual target is chosen. Protect default map0 before and after any experiment.

## July 5 Map0 Actor-18 Visual Baseline Correction (VERIFIED)

- **Corrected Map0 draw-list baseline: 6 sprite actors (1, 2, 16, 23, 25, 26).** Actor 18 is correctly hidden by its own field-load script. This SUPERSEDES the earlier `worklist_port_recovery_20260705_095407.log` baseline, which showed 7 actors (incl. 18) — that extra actor was a **stub artifact**, not correct rendering.
- Root cause of the apparent "missing sprite": actor 18's init script legitimately runs `HideActor` (opcode `0x23`) on itself after being shown. At the 09:54 GOOD log, `func_8009AD6C` (opcode `0x5f`) was still a **no-op stub** (`[stub] func_8009AD6C` appears in that log), so the VM spun on it (IP stuck at 2336, never advancing) and never reached the `HideActor` opcode — leaving actor 18 visible by accident.
- Commit `2627286` ("Implement field VM actor direction handler", 10:17:14) implemented `func_8009AD6C`, so the script now advances past it (`IP += 2`) to `DisableDialogActivation` (`0x2a`) → `HideActor` (`0x23`). Actor 18 hides as the original game intends.
- Actor 18's verified load opcode trace: `IP2324 op0x0b func_800A1624` (show) → `IP2327 op0xfe FieldScriptVM2Run` → `IP2336 op0x5f func_8009AD6C` (direction) → `IP2338 op0x2a DisableDialogActivation` → `IP2339 op0x23 HideActor` → `IP2340 op0x00 func_800A1B70` (idle spin).
- **`func_8009AD6C` verified FAITHFUL** against `asm/field/nonmatchings/main/misc7/func_8009AD6C.s` (byte-exact: reads 1 direction operand, sets `rotation` SVECTOR @0x104/0x106/0x108 with a `D_800ADB1C` conditional on `vz`, `IP += 2`, no script branch). No edit made.
- **This regression is NOT caused by the uncommitted working-tree diff** (`func_800764B4` skip, map1 work, `port_main.c` init refactor). None of actor 18's script handlers are uncommitted-changed functions. The uncommitted `misc2.c`/`port_main.c` changes are behaviorally inert on Map0.
- Map0 render path remains healthy: `RUN_RC=124`, zero `[stub]` lines, `func_8001E3D8` links sprites (`2/6/8/3/2/4`), `DrawOTag=1`, `D_800ADC18` gate behaves identically to the GOOD log.
- **Do not force actor 18 visible and do not re-stub `func_8009AD6C`.** If a future visual milestone needs actor 18 shown, that must come from a legitimate un-hide trigger, not from breaking the VM.

## July 5 func_800764B4 Active-Actor Quad Path (VERIFIED, Version B / TR-drop)

- **`func_800764B4` implemented** (active-actor billboard quad render path), replacing the skip-stub. Faithful port of `asm/field/matchings/main/misc2/func_800764B4.s` (real disassembly; the `/* Handwritten function */` header only means the GTE COP2 opcodes were hand-annotated).
- **Exact files changed this turn: `src/field/main/misc2.c` ONLY** (the `func_800764B4` body). No other file edited. The rest of the dirty file set (`docs/ai_context/ACTIVE_HANDOFF.md`, `pc_port/src/game_overrides.c`, `pc_port/src/port_main.c`, `src/field/main/misc.c`, `misc6.c`, `misc7.c`, `misc8.c`, and the `func_800748E8` assert-guard hunk within `misc2.c`) was **pre-existing and unchanged** by this turn.
- `s_ActiveActorSkipCount` retained (per instruction) via `(void)` reference — kept as a fallback for any other unmigrated path.
- **GTE-op → wrapper mapping** (all implemented, non-stubbed `T` symbols): `op` (outer product) → `OuterProduct12`; `mvmva 1,0,3,3,0` (RT×IR) → `ApplyMatrixSV` (column-wise); `ScaleMatrix`; `RotAverage4`; `VectorNormal`; `SetRot/TransMatrix`.
- **TR-drop decision (Version B), empirically PROVEN** — do not add `+worldToScreenMatrix.t`:
  - The asm translation step @`80076850` is `mvmva 1,0,0,0,0` (cv=0, adds GTE TR). The sibling `func_80075B44.s` @`80075DE4` is the *identical* `cv=0`.
  - But the port's `ApplyMatrixLV` (PsyCross `LIBGTE.C:526`, and ROM `ApplyMatrixLV.s`) is **rotation-only** (`gte_SetRotMatrix`+`RTIR`, no `m->t`). The proven-working sibling `func_80075B44` (misc2.c:1848-1853) feeds `ApplyMatrixLV` output straight into `actorMatrix.t` and renders correctly.
  - Runtime proof (gdb, `worldToScreenMatrix.t=[0,-189,24794]`): for actor pos `[-100,0,100]`, `ApplyMatrixLV` OUT = `[0,-340,258]` == `R*pos>>12` **exactly** (TR-drop). TR-add would give `[0,-529,25052]` (Z=25052 → actor pushed off-screen). So `func_800764B4` mirrors the sibling: `actorMatrix.t = ApplyMatrixLV(...)`, no explicit `+t`.
- **Build:** `LINK OK -> pc_port/build_native/xeno-port` (`misc2.c` compiled clean, `compiled=43`).
- **Map1 target test** (`XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 XENO_FIELD_MAP=1`):
  - **`func_800764B4` skip count: 16 → 0** — Map1 now executes the real actor quad path.
  - **gdb count: 128 OT splices** at the OT-insert line (`misc2.c` `func_800764B4` tail) before the crash — ~21 actor-quads/frame actively linked (was 0/all-skipped).
  - **`primSubmits` clears to 2** (gate `D_800ADC18` reaches 0 at frame 4).
  - **Run advances to frame 5** (pre-impl capture stalled at frame 2 with `primSubmits=0`).
  - **`DrawOTag=1` every frame**, with **no crash inside `func_800764B4` or `DrawOTag`** — the 128 splices are well-formed (a corrupt splice would crash OT traversal).
  - Latest Map1 log: `captures/render_diag/map1_764B4_20260705_172450.log`.
- **Map0 guard remains SAFE:** `RUN_RC=124`, draw list unchanged **`1, 2, 16, 23, 25, 26`**, `active=22 plain=6 status20=16` (actor 18 still hidden), **0 `[stub]` lines**.
- **Map1 `RUN_RC=139`** is the **pre-existing downstream text-render crash**, present since before any of this session's work — NOT introduced by `func_800764B4`.
- **Current next blocker (from crash backtrace):**
  - `SIGSEGV in func_80034FFC at src/slus_006.64/system/system.c:924`: `glyph = *(u16*)glyphData;` (null/bad `glyphData`).
  - Stack: `func_80034FFC` → `func_80033DF0` (system.c:470) → `func_80034888` (system.c:724, text-box render) → `func_8008004C` (text_box_render.c:624) → `func_8007554C` (misc2.c:1676) → `func_80078D44` / `FieldMain`.
  - **`func_800764B4` is ABSENT from the crash stack.**
- Remaining Map1 `[stub]` field functions still present: `func_80085C90`, `func_80072254`, `func_8008D0F4`, `func_80091F84`, `func_80095284`, `func_800975C0`, `func_80098038`, `func_800980FC`, `func_8009E91C`.
- **Bigger goal:** aim for a fully readable/visible field, not stub-artifact single-sprite visuals. After the text-render crash, prioritize camera/depth/scale/OT/texture correctness of the now-rendered quads.

## July 5 func_80034FFC Glyph Offset Sign-Extension Fix (VERIFIED)

- **Root cause:** 64-bit pointer **sign-extension bug** in `func_80034FFC` glyph-offset math (`src/slus_006.64/system/system.c`). The original PS1 asm (`func_80034FFC.s`: `subu`/`addu`) computes the glyph offset in **signed 32-bit** arithmetic, so low glyph codes (below the font table base) index **negatively** into the font block below `D_8005935C`. The port did **unsigned** subtraction and added the **zero-extended** 32-bit result to the 64-bit host font pointer → corrupt addresses.
- **Evidence:** at the crash `lead=0, trail=3, D_8005935C=0x67d282, D_80059364=16 (table base)`. Correct offset = `(3-16)*0x16 = -286`; correct `glyphData = 0x67d282 - 286 = 0x67d164` (valid `g_PsxRam`, reads `0x2510`). The port produced `0x10067d164` (delta `0xFFFFFEE2` = -286 zero-extended). Font init is correct (`D_8005934C=254`, `D_80059350=5236`, `D_80059364=16` loaded from `pSystemFont`), and char 3 is a legit negative-index glyph (not a control code: `func_80033DF0` handles `0/1/2/0x0F`). NOT a missing-init bug.
- **Fix (signed-delta form, no null guard, no fallback glyph):** cast the deltas to `s32` so they sign-extend into the 64-bit pointer:
  - lead==0: `s32 glyphOffset = ((s32)trail - (s32)D_80059364) * 0x16; glyphData = (u8*)(uintptr_t)D_8005935C + glyphOffset;`
  - lead/trail table path: `s32 leadOffset = ((s32)lead - (s32)D_8005934C) * 0x1600; s32 trailOffset = (s32)trail * 0x16; glyphData = (u8*)(uintptr_t)D_8005935C + D_80059350 + trailOffset + leadOffset;`
  - `0xFF/0xFF` special case unchanged. Text-box activation logic untouched.
- **Files changed: `src/slus_006.64/system/system.c` ONLY.**
- **Build:** `LINK OK` (`compiled=43`).
- **Map1 result** (`captures/render_diag/map1_glyphfix_20260705_185647.log`): `RUN_RC=134`. **The `system.c:924` SIGSEGV is GONE** (0 occurrences). Map1 advances past the text-box render (frame 5, `primSubmits=2`, `func_800764B4` skip=0).
- **Map0 guard:** `RUN_RC=124`, draw list unchanged **`1, 2, 16, 23, 25, 26`**, `active=22 plain=6 status20=16` (actor 18 still hidden), 0 `[stub]`. SAFE.
- **Next blocker (new, different failsafe — NOT from this fix):** `func_800248D4` assertion `"opcode path is not implemented"` at `src/slus_006.64/system/temp1.c:729` (sprite **animation-script VM**). Stack: `func_800248D4(pSpriteData) [temp1.c:729]` → `AnimScriptTick [temp1.c:91]` → `func_800752C8 [misc2.c:1511]` → `func_8007554C [misc2.c:1651]`. Also a new `[stub] func_80081F80` appears just before it. This is an unmigrated anim-script opcode handler (a deliberate assert, like `func_800764B4` was), reached only because the glyph fix let Map1 run further.
- **Open follow-up (unchanged from prior note):** whether Map1 should auto-display the dialogue at all (text box activated via the `pWindow+0x8C` node list; script is structured encoded text) — a dialogue-activation question, separate from the (now-fixed) font-pointer bug.

## July 5 Anim-Script Opcode 0xA0 Implementation (VERIFIED, bounded)

- **Root cause:** opcode `0xA0` is a genuinely unmigrated **two-layer** animation-script opcode (NOT a mis-dispatch, NOT a no-op). The C port asserted at `func_800248D4` (temp1.c:729) because its opcode dispatcher omitted the generic default path, and `func_8001FBE4` also lacked the `0xA0` sub-handler.
- **ASM evidence:**
  - `func_800248D4.s`: opcode `>=0x80` indexes `jtbl_800186E0[opcode-0x80]`; `[0x20]` (opcode `0xA0`, rodata 800.rodata.s:8815) = `0x80024EC8` = the **default handler** `.L80024EC8`, which calls `func_8001FBE4(pData, opcode, pc+1)` then advances `pData+0x64` by `D_8004FC40[0xA0] == 2` (→ `pc+3`). Runtime script bytes: `a0 14 13 03 …` (opcode + 2 operands, next opcode `0x03`).
  - `func_8001FBE4.s`: dispatches `jtbl_800183D8[opcode-0x8A]`; `[0x16]` (opcode `0xA0`, 800.rodata.s:8586) = `0x80021958` (a **real** handler, not the shared no-op `0x80021AB8`): `pData[0x18] = ((s8)op0*16 * (D_80059198+1) * (s16)pData[0x82]) >>12 <<8` (neg rounds `+0xFFF`), then `func_80022974(pData)`.
  - Runtime (gdb): `opcode=0xa0 pc=0x5c0a92 pSpriteData=0x5db8a8`, `D_8004FC40[0xA0]=0x02`.
  - Deps all present in port: `func_80022974` (T), `D_80059198` (B, =1 from init.c:39), `func_8001FBE4`/`func_800248D4` (T).
- **Fix (bounded, per-opcode — no full anim VM, no D_8004FC40 table, no no-op, no silenced assert):**
  - `src/slus_006.64/system/temp1.c` `func_800248D4`: added `if (opcode == 0xa0)` → `func_8001FBE4(pData, opcode, pc+1); pData+0x64 = pc+3; return;`.
  - `src/slus_006.64/system/animation_scripts.c` `func_8001FBE4`: added `if (dispatchIndex == 0x16)` handler (the `0x80021958` computation + `func_80022974`); also added `extern s32 D_80059198;` / `extern void func_80022974(void*)` forward decls so the TU compiles (without them the build silently skips/stubs `animation_scripts.c`).
- **Files changed: `src/slus_006.64/system/temp1.c` + `src/slus_006.64/system/animation_scripts.c` only.**
- **Build:** `LINK OK` (`compiled=43`; `animation_scripts.c` compiled, not stubbed).
- **Map1 result** (`captures/render_diag/map1_opcode_a0_20260705_192940.log`): `RUN_RC=124` — went from `134` (assert abort) to a **clean 20s timeout, no crash**. `temp1.c:729` opcode-`0xA0` assert and `func_8001FBE4` assert both **gone** (0 each). Reaches frame 5, `primSubmits=2`, `DrawOTag=1`.
- **Map0 guard:** `RUN_RC=124`, draw list unchanged **`1, 2, 16, 23, 25, 26`**, `active=22 plain=6 status20=16` (actor 18 hidden), 0 `[stub]`. SAFE.
- **Next blocker:** **no hard crash/assert remains on Map1** — it now runs to timeout. Remaining are **soft (non-crashing) stubs** that were already present: `func_80085C90`, `func_80081F80`, `func_80072254`, `func_80091F84`, `func_80095284`, `func_800975C0`, `func_80098038`, `func_800980FC`, `func_8009E91C`. These are missing behavior, not crashes. Next frontier is either implementing those soft stubs or assessing Map1 visual correctness now that it runs stably.

## July 5 XENO_FIELD_ENTRANCE Harness Spawn Selector (VERIFIED)

- **Problem:** Map1 near-black was NOT a renderer/texture/camera-math bug. Full trace: player (actor 1) *was* placed, but at spawn-table **entrance 0 = `[-906,-863]`**, which is *outside* Map1's walkable bounds `X[-877,453] Z[-775,1000]`. `func_8007CD80` (walkmesh lookup) rejects the out-of-bounds actor, so `func_80072A38` falls back to a mesh-edge camera target (`x≈-2`) far from the player → the player projects off-screen (top-center). The camera/projection/walkmesh code is all correct.
- **Root cause:** the field-load script (`func_800A08B8` → `func_8009FA54`) picks the spawn-table entry from field-script **variable 2**, which the real worldmap→field / field→field transition sets. Direct `XENO_FIELD_MAP` entry skips that transition → var 2 = 0 → entrance 0. Valid in-bounds entries exist (3 `[210,736]`, 5 `[370,900]`, 6 `[405,-72]`, 7 `[-751,218]`).
- **Copy chain (found by watchpointing each stage) — the source is 3 layers up:**
  - `D_8006F954` (transition input, sister global of the map selector `D_8006F94E`) → FieldMain (main.c:408) copies it to `g_GameState+0x1932`
  - `g_GameState+0x1930` → FieldLoad `func_800705DC` (misc3.c:390-396) copies it into `g_FieldScriptMemory`
  - `g_FieldScriptMemory+2` = field-script var 2, read by `func_800A08B8`.
- **Two mis-targeted attempts (documented so we don't repeat):**
  - Writing `g_FieldScriptMemory+2` at boot → **overwritten by FieldLoad's copy** from `g_GameState+0x1930` (misc3.c:394).
  - Writing `g_GameState+0x1932` at boot → **overwritten by FieldMain** (`main.c:408`, `= D_8006F954`).
  - (Both looked "safe" at first because a value-based gdb `watch` doesn't fire on a `0→0` copy; setting the value to 6 first exposed each clobber.)
- **Correct fix:** write **`D_8006F954`** — the top of the chain, the same global the game's transition writes, set exactly like the working `XENO_FIELD_MAP → D_8006F94E`. Coordinates still come from the game's own spawn table; only the index is selected.
- **Fix (`pc_port/src/port_main.c` ONLY):** added `extern unsigned short D_8006F954;` and, in the field-entry block after `XENO_FIELD_MAP`, `D_8006F954 = (unsigned short)entrance;` guarded by an `XENO_FIELD_ENTRANCE=N` env var (parse + `0..0xFFFF` validity check; unset → no write → current behavior preserved).
- **Build:** `LINK OK` (`compiled=43`).
- **Map1 result** (`captures/render_diag/map1_entrance6_d8006f954_20260705_211304.log`, `XENO_FIELD_ENTRANCE=6`): `RUN_RC=124`, prints `XENO_FIELD_ENTRANCE=6`. Verified full chain: `D_8006F954=6` → `g_GameState+0x1932=6` (FieldMain) → `g_FieldScriptMemory var2=6` (at `func_800A08B8`) → **player spawns at `[405,0,-72]`** (entry-6 table coords, was `[-906,-863]`), now **inside** the walkable bounds. Camera target X = **405** now tracks the player (was pinned at edge `-2`).
- **Map0 guard (env unset):** `RUN_RC=124`, draw list unchanged **`1, 2, 16, 23, 25, 26`**, `active=22 plain=6 status20=16` (actor 18 hidden), 0 `[stub]`. SAFE.
- **Open follow-up:** with entry 6, `g_CameraAt2.z >> 16 = -28567` (X tracks the player correctly, but the Z target is far outside `Z[-775,1000]`) — worth a look, but secondary; the primary goal (in-bounds spawn so the camera can frame the player) is achieved. **Visual confirmation from the user still pending** — does the Map1 sprite now move away from top-center into a proper field view?

## Exact Next Function To Implement

- **No bounded kernel0 field stub blocker remains in the verified path** — latest 45s verification run after `func_8009AD6C` implementation has zero `[stub]` lines.
- `primSubmits=0` before frame 4 is **expected** — `FieldAddPrimitives` is gated behind `D_800ADC18 == 0`. In the latest 45s audit run the countdown reaches 0 at frame 4 and `primSubmits` becomes 2.
- Actor packets ARE drawn via direct OT-linking (`func_8001E3D8`), independent of `FieldAddPrimitives`.
- **45s timeout test: COMPLETED** — frames 0–7, D_800ADC18=4→3→2→1→0, gate cleared, `primSubmits=2`.
- **`XENO_KERNEL_SEL=1` test: COMPLETED** — hits `func_8001B6C4` stub immediately (2-line log at `captures/render_diag/kernel1_30s_20260704_170523.log`). Root cause fully traced (7-step chain: `psyq_compat.c:327` → `g_KernelMenuCurChoice=1` → `ChangeGameState(1)` → `game_overrides.c:260` → `temp3.c:301` INCLUDE_ASM → `stubs.c:479` stub → returns 0, state aborts).
- Do not start broad feature work or add unrelated stub replacements yet.
- Next single step:
  - Stop at the current `func_800764B4` active actor quad failsafe. It is a large visual path and should not be patched around. Audit first, then implement only if the exact behavior is understood.
- Do not clamp coordinates, skip primitives, fake rendering, or add dummy packets.

## Commands Verified

- **Build**: `distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp && ./pc_port/build_port.sh'`
- **Visual recovery run**:
  `distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp && XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 timeout -s KILL 45 ./pc_port/build_native/xeno-port'`
  - Latest log: `captures/render_diag/worklist_port_recovery_20260705_095407.log`.
  - User visually confirmed the sprite/fade/slow-zoom milestone is restored.
- **Checkpoint snapshot after visual recovery**:
  - `captures/render_diag/sprite_recovered_status_20260705_095615.txt`
  - `captures/render_diag/sprite_recovered_full_diff_20260705_095615.patch`
- **Kernel0 45s audit run** (field path, XENO_KERNEL_SEL=0):
  `distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp/pc_port && XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 timeout 45 build_native/xeno-port'`
  - Latest audit result: `RUN_RC=124`, no crash, frames 0-7 observed, `D_800ADC18=4→3→2→1→0`, `primSubmits=2` from frame 4 onward.
- **Kernel0 30s run** (field path, XENO_KERNEL_SEL=0):
  `distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp/pc_port && XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 timeout -s KILL 30 build_native/xeno-port 2>&1 | tee captures/render_diag/kernel0_30s_$(date +%Y%m%d_%H%M%S).log; rc=${PIPESTATUS[0]}; echo RUN_RC=$rc'`
  - Historical log: `captures/render_diag/kernel0_30s_20260704_170009.log` (79 lines, 3 frames, D_800ADC18=4→3→2, primSubmits=0, DrawOTag=1, ~25 POLY_FT4 packets OT-linked, RC=137)
- **Kernel0 30s filtered run** (grep for key events):
  `distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp/pc_port && XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 timeout -s KILL 30 build_native/xeno-port 2>&1 | grep -E "RUN_RC|\\[stub\\]|func_8001E3D8 link|func_80075B44 frame|frame=|Unhandled zero length|did not output valid primitive|ParsePrimitive|ParsePrimitivesLinkedList|assert|SIG|Aborted" | tee captures/render_diag/kernel0_filtered_$(date +%Y%m%d_%H%M%S).log; rc=${PIPESTATUS[0]}; echo RUN_RC=$rc'`
- **Kernel1 30s run** (battle path, XENO_KERNEL_SEL=1):
  `distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp/pc_port && XENO_FIELD_TEST=1 XENO_KERNEL_SEL=1 timeout -s KILL 30 build_native/xeno-port 2>&1 | tee captures/render_diag/kernel1_30s_$(date +%Y%m%d_%H%M%S).log; rc=${PIPESTATUS[0]}; echo RUN_RC=$rc'`
  - Latest log: `captures/render_diag/kernel1_30s_20260704_170523.log` (2 lines: `[stub] func_8001B6C4`, `RUN_RC=137`)
- **Note:** `rg` is NOT available inside the distrobox container — use `grep -E` instead.
- **Note:** All run logs are persisted under `captures/render_diag/` in the repo (not Copilot temp files).
- Targeted gdb runs for:
  - `FieldScene` offsets and `worldToScreenMatrix` address.
  - `func_80024FF4` source matrix copied into `D_8004FBB8`.
  - first `func_8001E3D8` / `RotTransPers4` quad input and GTE geometry registers before and after `func_8007254C` projection-default fix.
  - conditional `ParsePrimitive` hit for actor `code=0x2d` `POLY_FT4` during `DrawOTag`.
