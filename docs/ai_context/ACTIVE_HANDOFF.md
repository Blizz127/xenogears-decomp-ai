# Active Handoff

## Current Verified State

- Native PC field repro builds and links cleanly inside `xenogears-dev`.
- Git HEAD: `7cb7e8b` on branch `ai-private-main` (4 commits ahead of origin `b7d9ac3`).
  - **Note:** `git` is NOT on PATH inside the distrobox container. Run git commands on the HOST at `/home/blizz/Projects/xenogears-decomp`.
- Exact repro command runs to timeout/no-crash:
  - `XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 timeout -s KILL 10 build_native/xeno-port`
  - Observed result is `RUN_RC=124` (timeout kill, no crash).
- A filtered 10-second run prints **zero** `[stub]` lines — none of the 262 stubs are triggered.
- Build-generated function stub count is `262`.
- `func_8009E574` was investigated as a candidate bounded blocker: confirmed **NOT a stub** — it is compiled C code (nm symbol type `T` at `0x41cf83`, absent from `stubs.c`).
- Field overlay loads and field main loop runs.
- Actor sprite data reaches `27/27` after VM/post-VM setup.
- Actor draw path runs, and `func_8001E3D8` links real `code=2d` actor `POLY_FT4` packets into the OT.
- `DrawOTag` runs.
- `FieldAddPrimitives` reports `primSubmits=0` — this is **expected behavior**, NOT a bug:
  - Primitive submission via `FieldAddPrimitives` is gated behind `if (D_800ADC18 == 0)` in `func_8007554C`.
  - `D_800ADC18` is a countdown timer that starts at 4 and decrements by 1 per frame.
  - In a 10-second timeout run, only ~3 frames execute before the process is killed.
  - Observed: `D_800ADC18` goes 4→3→2 over 3 frames, never reaching 0 before timeout.
  - Actor packets are still drawn via direct OT-linking, independent of `FieldAddPrimitives`.
- Primitive submission functions (`AddPrim`, `AddPrims`, `CatPrim`, `DrawPrim`) are **NOT stubbed** — provided by Psy-X (compiled implementations).
- Visual output is still not externally confirmed. Host `import -window root` screenshot capture failed from this shell.

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

- No live stubs appear during the 10-second filtered field repro.
- Actor packets are now OT-linked with sane projected coordinates.
- PsyCross parses at least one actor `POLY_FT4` from the OT during `DrawOTag`.
- Visible output is not yet confirmed from an external screenshot.
- Next practical frontier is to prove whether those parsed actor packets emit backend vertices and present visibly, or whether the next issue is texture/CLUT/tpage decode, display capture, draw-env/display-env state, or another packet/render compatibility mismatch.

## Exact Next Function To Implement

- `func_8009E574` was investigated as the next candidate bounded blocker — confirmed **NOT a stub**.
  - It is compiled C code (nm symbol type `T` at `0x41cf83`, absent from `stubs.c`).
  - No action needed on this function.
- **No bounded stub blocker was found** in the current test configuration.
  - Zero `[stub]` lines in a filtered 10-second run.
  - `primSubmits=0` is expected — `FieldAddPrimitives` is gated behind `D_800ADC18 == 0`, and the countdown (4→3→2) never reaches 0 within the 3 frames that execute before the 10s timeout.
  - Actor packets are still drawn via direct OT-linking, independent of `FieldAddPrimitives`.
- Do not start broad feature work or add stub replacements.
- Next investigative directions (not code changes yet):
  - Run with a longer timeout (e.g. 30s) to pass the `D_800ADC18` countdown and confirm whether `FieldAddPrimitives` primitives begin submitting.
  - Try `XENO_KERNEL_SEL=1` to see if a different kernel path reveals different stubs.
  - Try different field IDs to exercise different field overlays.
  - Inspect actual rendered output or PsyCross handling of the now-sane actor `POLY_FT4` packets.
  - If output is still blank or corrupted, investigate: actor packet decode path for `code=0x2d`, tpage/CLUT/texture upload state, draw-env/display-env/present path, OT traversal, or host-width issues around `long*` depth/flag outputs from GTE wrappers.
- Do not clamp coordinates, skip primitives, fake rendering, or add dummy packets.

## Commands Verified

- `distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp && ./pc_port/build_port.sh'`
- `distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp/pc_port && XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 timeout -s KILL 10 build_native/xeno-port; rc=$?; echo RUN_RC=$rc'`
- `distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp/pc_port && XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 timeout -s KILL 10 build_native/xeno-port 2>&1 | grep -E "RUN_RC|\\[stub\\]|func_8001E3D8 link|func_80075B44 frame|frame=|Unhandled zero length|did not output valid primitive|ParsePrimitive|ParsePrimitivesLinkedList|assert|SIG|Aborted"; rc=${PIPESTATUS[0]}; echo RUN_RC=$rc'`
  - **Note:** `rg` is NOT available inside the distrobox container — use `grep -E` instead.
- Targeted gdb runs for:
  - `FieldScene` offsets and `worldToScreenMatrix` address.
  - `func_80024FF4` source matrix copied into `D_8004FBB8`.
  - first `func_8001E3D8` / `RotTransPers4` quad input and GTE geometry registers before and after `func_8007254C` projection-default fix.
  - conditional `ParsePrimitive` hit for actor `code=0x2d` `POLY_FT4` during `DrawOTag`.
