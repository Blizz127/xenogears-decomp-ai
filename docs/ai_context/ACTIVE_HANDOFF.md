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
- **Kernel4 direct menu test is an invalid/incomplete direct route for now** (`XENO_KERNEL_SEL=4`): it enters `MenuMain()` without a menu overlay loaded and therefore reaches `func_801C62A8` as a generated stub. This should be treated as a harness/overlay-loading problem, not as a real source function to implement.
- **`D_800ADC18` gate now observed clearing**: field-transition counter starts at 4 (`misc3.c:299`), decrements once per frame (`misc4.c:21-22`), and reaches 0 at frame 4 in the 45s audit run.
- `FieldAddPrimitives` submission is observed after the gate clears: `primSubmits=2` at frame 4 and later.
- Actor packets ARE created/OT-linked via `func_8001E3D8` independently of the `D_800ADC18` gate — so actor rendering is not blocked by the gate, only primitive submission is.
- PsyCross `ParsePrimitive` hits actor `code=0x2d` `POLY_FT4` during `DrawOTag`.
- **Screen coordinate anomaly unresolved**: OT1 frames have high 16 bits = `0x7ffd`, OT2 frame has high byte = `0x25`. Lower 32 bits consistent across both. Possible OT-switching or screen-coordinate address-formation issue.
- Visual correctness for the recovered field-view sprite/fade milestone is confirmed by the user after restoring the work-list/sprite-frame path through `pc_port/src/work_list_port.c`.
- The earlier `func_8001E3D8` concern is cleared: gdb proved it is called after `D_800ADC18 == 0`, and the frame-0-only filtered log was capped/misleading.
- Remaining frontier: preserve this recovered state before starting new diagnostics or implementation work.

## Exact Next Function To Implement

- **No bounded kernel0 field stub blocker remains in the verified path** — latest 45s verification run after `func_8009AD6C` implementation has zero `[stub]` lines.
- `primSubmits=0` before frame 4 is **expected** — `FieldAddPrimitives` is gated behind `D_800ADC18 == 0`. In the latest 45s audit run the countdown reaches 0 at frame 4 and `primSubmits` becomes 2.
- Actor packets ARE drawn via direct OT-linking (`func_8001E3D8`), independent of `FieldAddPrimitives`.
- **45s timeout test: COMPLETED** — frames 0–7, D_800ADC18=4→3→2→1→0, gate cleared, `primSubmits=2`.
- **`XENO_KERNEL_SEL=1` test: COMPLETED** — hits `func_8001B6C4` stub immediately (2-line log at `captures/render_diag/kernel1_30s_20260704_170523.log`). Root cause fully traced (7-step chain: `psyq_compat.c:327` → `g_KernelMenuCurChoice=1` → `ChangeGameState(1)` → `game_overrides.c:260` → `temp3.c:301` INCLUDE_ASM → `stubs.c:479` stub → returns 0, state aborts).
- Do not start broad feature work or add unrelated stub replacements yet.
- Next single step:
  - Do not change the recovered render path. Pick exactly one next target: either inspect the remaining `func_80075B44` assert-only rare branches against asm, or start an overlay-aware route proof for menu/worldmap/movie without fabricating overlay entry functions.
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
